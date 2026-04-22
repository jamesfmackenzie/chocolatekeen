/*
 * gba_decomp_graphics_host.c — host-side EGALATCH/EGASPRIT decompressor
 *                              and pre-unpacker.
 *
 * Keen 1's EGALATCH LZW-decompresses to ~120 KiB and EGASPRIT to ~31 KiB,
 * both larger than the GBA's ~84 KiB heap. Instead of fighting RAM at
 * runtime, this tool:
 *
 *   1. Decompresses EGALATCH/EGASPRIT payloads in place (matching the
 *      compression bits in EGAHEAD) so the runtime can map them straight
 *      out of ROM.
 *
 *   2. Splits the EGAHead sections into standalone blobs whose layout
 *      matches the runtime struct arrays. This lets gfx_gba cast a ROM
 *      pointer to (EGAHeadUnmasked_T *) / (MaskedSpriteEntry_T *) with
 *      zero malloc.
 *        EGABMPHD.<ext>  — bmpNum * sizeof(EGAHeadUnmasked_T)   (16 B/ea)
 *        EGASPRHD.<ext>  — 4*spriteNum * sizeof(MaskedSpriteEntry_T)  (32 B/ea)
 *
 *   3. Unpacks the sprite and font plane-packed EGA pixels into flat 8bpp
 *      pools, so the runtime can point engine_egaSpriteData /
 *      engine_egaFontData at ROM instead of allocating heap for the
 *      unpack. The 5-plane sprite unpack and 4-plane font unpack here
 *      mirror the loops in render/gfx_gba.c.
 *        EGASPRUNP.<ext>  — flat sprite pool (each base entry: 8*w*h bytes)
 *        EGAFNTUNP.<ext>  — flat font pool (fontNum * 64 bytes)
 *
 *   4. Clears EGAHEAD's compression bits so the fallback non-bake path
 *      still makes sense if anyone ever fread()s EGAHEAD directly.
 *
 * Pulls in the engine's own lz.c so the host and target share a single
 * decompressor implementation.
 *
 * Usage: gba_decomp_graphics_host <staged_dir> <ext>
 * Missing files are silently ignored (per-episode bakes may stage a subset).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

extern char lz_decompress(FILE *lzfile, unsigned char *outbuffer);

static uint32_t le32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint16_t le16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint8_t *read_file(const char *path, size_t *size_out) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    long sz = ftell(fp);
    if (sz < 0) { fclose(fp); return NULL; }
    rewind(fp);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz + 1);
    if (!buf) { fclose(fp); return NULL; }
    if (sz > 0 && fread(buf, 1, (size_t)sz, fp) != (size_t)sz) {
        free(buf); fclose(fp); return NULL;
    }
    fclose(fp);
    *size_out = (size_t)sz;
    return buf;
}

static int write_file(const char *path, const void *buf, size_t size) {
    FILE *fp = fopen(path, "wb");
    if (!fp) { perror(path); return 1; }
    if (size > 0 && fwrite(buf, 1, size, fp) != size) {
        perror(path); fclose(fp); return 1;
    }
    fclose(fp);
    return 0;
}

static int decompress_in_place(const char *path, size_t decsize, const char *label) {
    FILE *in = fopen(path, "rb");
    if (!in) { perror(path); return 1; }
    uint8_t *buf = (uint8_t *)malloc(decsize);
    if (!buf) { fclose(in); fprintf(stderr, "oom\n"); return 1; }
    char rc = lz_decompress(in, buf);
    fclose(in);
    if (rc) {
        fprintf(stderr, "gba_decomp_graphics_host: lz_decompress(%s) failed\n", path);
        free(buf);
        return 1;
    }
    int wrc = write_file(path, buf, decsize);
    free(buf);
    if (wrc == 0) {
        fprintf(stderr, "gba_decomp_graphics_host: %s (%s, %zu bytes uncompressed)\n",
                path, label, decsize);
    }
    return wrc;
}

/* Pack the EGAHead bitmap subsection into EGABMPHD.<ext>. The runtime
 * struct (EGAHeadUnmasked_T) already matches the on-disk layout:
 * h(u16) v(u16) loc(u32) name[8] = 16 bytes/entry. */
static int emit_bmphd(const uint8_t *head, size_t head_size,
                      uint32_t imgDataStart, uint16_t bmpNum,
                      const char *out_path) {
    size_t want = (size_t)bmpNum * 16u;
    if (imgDataStart > head_size || want > head_size - imgDataStart) {
        fprintf(stderr, "%s: bmp header section overruns EGAHEAD\n", out_path);
        return 1;
    }
    int rc = write_file(out_path, head + imgDataStart, want);
    if (rc == 0) {
        fprintf(stderr, "gba_decomp_graphics_host: %s (%u bmp headers, %zu bytes)\n",
                out_path, (unsigned)bmpNum, want);
    }
    return rc;
}

/* Pack the EGAHead sprite subsection into EGASPRHD.<ext>. MaskedSpriteEntry_T
 * on-disk layout is 8*u16 + char[12] + u32 = 32 bytes/entry; there are
 * 4*spriteNum entries (one per shift). */
static int emit_sprhd(const uint8_t *head, size_t head_size,
                      uint32_t sprDataStart, uint16_t spriteNum,
                      const char *out_path) {
    size_t want = (size_t)spriteNum * 4u * 32u;
    if (sprDataStart > head_size || want > head_size - sprDataStart) {
        fprintf(stderr, "%s: sprite header section overruns EGAHEAD\n", out_path);
        return 1;
    }
    int rc = write_file(out_path, head + sprDataStart, want);
    if (rc == 0) {
        fprintf(stderr, "gba_decomp_graphics_host: %s (%u*4 sprite entries, %zu bytes)\n",
                out_path, (unsigned)spriteNum, want);
    }
    return rc;
}

/* Compute the unpacked sprite pool. Mirrors the 5-plane unpack loop in
 * render/gfx_gba.c:285..315 (and gfx.c). One base entry per 4 shifts. */
static int emit_sprunp(const uint8_t *head, size_t head_size,
                       const uint8_t *sprit, size_t sprit_size,
                       uint32_t sprDataStart, uint16_t spriteNum,
                       uint32_t spriteLoc, uint32_t sprPlaneSize,
                       const char *out_path) {
    if (sprDataStart > head_size) return 1;
    const uint8_t *entries = head + sprDataStart;
    size_t entries_bytes = (size_t)spriteNum * 4u * 32u;
    if (entries_bytes > head_size - sprDataStart) {
        fprintf(stderr, "%s: sprite header section overruns EGAHEAD\n", out_path);
        return 1;
    }
    /* Sum unpacked pool size from base entries (every 4th). */
    size_t pool_size = 0;
    for (uint32_t i = 0; i < (uint32_t)spriteNum * 4u; i += 4) {
        uint16_t w = le16(entries + i * 32 + 0);
        uint16_t h = le16(entries + i * 32 + 2);
        pool_size += (size_t)8 * w * h;
    }
    uint8_t *pool = (uint8_t *)malloc(pool_size ? pool_size : 1);
    if (!pool) { fprintf(stderr, "oom (sprite pool)\n"); return 1; }
    uint8_t *dst = pool;
    for (uint32_t i = 0; i < (uint32_t)spriteNum * 4u; i += 4) {
        uint16_t w          = le16(entries + i * 32 + 0);
        uint16_t h_rows     = le16(entries + i * 32 + 2);
        uint16_t loc_offset = le16(entries + i * 32 + 4);
        uint16_t location   = le16(entries + i * 32 + 6);
        size_t base = (size_t)spriteLoc + loc_offset + 16u * location;
        size_t nPix = (size_t)8 * w * h_rows;
        size_t planeBytes = (nPix + 7u) / 8u;
        /* Each plane starts at `base + k*sprPlaneSize` and supplies `planeBytes`
         * bytes of bits. The first plane at offset `base` must fit inside a
         * single plane window (sprPlaneSize). Since EGASPRIT is laid out as
         * five consecutive plane windows, this also guarantees the plane-4
         * read stays within the file. The previous check used
         * `base + 5*sprPlaneSize > sprit_size` which, when sprit_size equals
         * 5*sprPlaneSize exactly (the normal case), rejected every sprite
         * with a nonzero base and silently zero-filled it. */
        if (base + planeBytes > sprPlaneSize) {
            fprintf(stderr, "%s: sprite %u base 0x%zx+%zu overruns plane (plane=0x%x) — zero-filling\n",
                    out_path, (unsigned)i, base, planeBytes, sprPlaneSize);
            memset(dst, 0, nPix);
            dst += nPix;
            continue;
        }
        size_t sprBit = 0;
        for (size_t p = 0; p < nPix; p++) {
            uint8_t v = 0;
            uint8_t mask = (uint8_t)(1 << (sprBit % 8 ^ 7));
            const uint8_t *bp = sprit + base + sprBit / 8;
            if (bp[0] & mask) v |= 1;
            if (bp[sprPlaneSize] & mask) v |= 2;
            if (bp[2 * sprPlaneSize] & mask) v |= 4;
            if (bp[3 * sprPlaneSize] & mask) v |= 8;
            if (bp[4 * sprPlaneSize] & mask) v |= 16;
            *dst++ = v;
            sprBit++;
        }
    }
    int rc = write_file(out_path, pool, pool_size);
    free(pool);
    if (rc == 0) {
        fprintf(stderr, "gba_decomp_graphics_host: %s (sprite pool, %zu bytes)\n",
                out_path, pool_size);
    }
    return rc;
}

/* Compute the unpacked font pool. Mirrors the 4-plane unpack loop in
 * render/gfx_gba.c:355..376. fontNum fonts * 64 pixels each (8x8). */
static int emit_fntunp(const uint8_t *latch, size_t latch_size,
                       uint32_t fontLoc, uint16_t fontNum,
                       uint32_t latchPlaneSize, const char *out_path) {
    size_t pool_size = (size_t)fontNum * 64u;
    if ((size_t)fontLoc + 4u * latchPlaneSize > latch_size) {
        fprintf(stderr, "%s: font plane reach overruns EGALATCH (%zu > %zu)\n",
                out_path, (size_t)fontLoc + 4u * latchPlaneSize, latch_size);
        return 1;
    }
    uint8_t *pool = (uint8_t *)malloc(pool_size ? pool_size : 1);
    if (!pool) { fprintf(stderr, "oom (font pool)\n"); return 1; }
    uint8_t *dst = pool;
    const uint8_t *vanilla = latch + fontLoc;
    size_t fntBit = 0;
    for (uint32_t i = 0; i < fontNum; i++) {
        for (uint32_t k = 0; k < 64; k++) {
            uint8_t v = 0;
            uint8_t mask = (uint8_t)(1 << (fntBit % 8 ^ 7));
            const uint8_t *bp = vanilla + fntBit / 8;
            if (bp[0] & mask) v |= 1;
            if (bp[latchPlaneSize] & mask) v |= 2;
            if (bp[2 * latchPlaneSize] & mask) v |= 4;
            if (bp[3 * latchPlaneSize] & mask) v |= 8;
            *dst++ = v;
            fntBit++;
        }
    }
    int rc = write_file(out_path, pool, pool_size);
    free(pool);
    if (rc == 0) {
        fprintf(stderr, "gba_decomp_graphics_host: %s (font pool, %zu bytes)\n",
                out_path, pool_size);
    }
    return rc;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <staged_dir> <ext>\n", argv[0]);
        return 2;
    }
    const char *dir = argv[1];
    const char *ext = argv[2];

    char head_path[512], latch_path[512], sprit_path[512];
    char bmphd_path[512], sprhd_path[512], sprunp_path[512], fntunp_path[512];
    snprintf(head_path,   sizeof head_path,   "%s/EGAHEAD.%s",   dir, ext);
    snprintf(latch_path,  sizeof latch_path,  "%s/EGALATCH.%s",  dir, ext);
    snprintf(sprit_path,  sizeof sprit_path,  "%s/EGASPRIT.%s",  dir, ext);
    snprintf(bmphd_path,  sizeof bmphd_path,  "%s/EGABMPHD.%s",  dir, ext);
    snprintf(sprhd_path,  sizeof sprhd_path,  "%s/EGASPRHD.%s",  dir, ext);
    snprintf(sprunp_path, sizeof sprunp_path, "%s/EGASPRUN.%s",  dir, ext);
    snprintf(fntunp_path, sizeof fntunp_path, "%s/EGAFNTUN.%s",  dir, ext);

    size_t head_size = 0;
    uint8_t *head = read_file(head_path, &head_size);
    if (!head) {
        /* No EGAHEAD for this episode — nothing to do. */
        return 0;
    }
    if (head_size < 48) {
        fprintf(stderr, "%s: truncated (%zu bytes)\n", head_path, head_size);
        free(head);
        return 1;
    }

    uint32_t latchPlaneSize = le32(head + 0);
    uint32_t sprPlaneSize   = le32(head + 4);
    uint32_t imgDataStart   = le32(head + 8);
    uint32_t sprDataStart   = le32(head + 12);
    uint16_t fontNum        = le16(head + 16);
    uint32_t fontLoc        = le32(head + 18);
    uint16_t bmpNum         = le16(head + 34);
    uint16_t spriteNum      = le16(head + 40);
    uint32_t spriteLoc      = le32(head + 42);
    uint16_t compression    = le16(head + 46);

    int rewrite_head = 0;

    if (compression & 2) {
        if (decompress_in_place(latch_path, (size_t)latchPlaneSize * 4, "EGALATCH")) {
            free(head); return 1;
        }
        rewrite_head = 1;
    }
    if (compression & 1) {
        if (decompress_in_place(sprit_path, (size_t)sprPlaneSize * 5, "EGASPRIT")) {
            free(head); return 1;
        }
        rewrite_head = 1;
    }

    if (rewrite_head) {
        /* Clear the two compression bits, leave any other upper bits alone. */
        head[46] &= (uint8_t)~0x03;
        if (write_file(head_path, head, head_size)) {
            free(head); return 1;
        }
        fprintf(stderr, "gba_decomp_graphics_host: %s (compression bits cleared)\n", head_path);
    }

    /* Emit split EGAHead metadata blobs. */
    if (bmpNum && emit_bmphd(head, head_size, imgDataStart, bmpNum, bmphd_path)) {
        free(head); return 1;
    }
    if (spriteNum && emit_sprhd(head, head_size, sprDataStart, spriteNum, sprhd_path)) {
        free(head); return 1;
    }

    /* Emit pre-unpacked sprite pool. Needs EGASPRIT (now decompressed). */
    if (spriteNum) {
        size_t sprit_size = 0;
        uint8_t *sprit = read_file(sprit_path, &sprit_size);
        if (sprit) {
            int rc = emit_sprunp(head, head_size, sprit, sprit_size,
                                 sprDataStart, spriteNum, spriteLoc,
                                 sprPlaneSize, sprunp_path);
            free(sprit);
            if (rc) { free(head); return 1; }
        }
    }

    /* Emit pre-unpacked font pool. Needs EGALATCH (now decompressed). */
    if (fontNum) {
        size_t latch_size = 0;
        uint8_t *latch = read_file(latch_path, &latch_size);
        if (latch) {
            int rc = emit_fntunp(latch, latch_size, fontLoc, fontNum,
                                 latchPlaneSize, fntunp_path);
            free(latch);
            if (rc) { free(head); return 1; }
        }
    }

    free(head);
    return 0;
}
