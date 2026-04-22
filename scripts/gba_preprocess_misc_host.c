/*
 * gba_preprocess_misc_host.c — host-side pre-bake for level and text data.
 *
 * Runtime on the GBA doesn't have the heap to malloc a compressed LEVEL,
 * expand it with CRLE, and keep the plaintext — nor to malloc+mutate the
 * text files. This tool applies both transforms on the build host, so the
 * runtime can memcpy (levels) or point straight at ROM (text files).
 *
 * Transforms applied in-place in the staged dir:
 *
 *   LEVEL*.<ext>       CRLE-expand, then drop the leading int16_t word
 *                      (matching the runtime memmove). Output size is
 *                      finsize-2 bytes (finsize from the file header).
 *
 *   STORYTXT.<ext>     Apply CVort_process_text_file: 0x1F → 0x0D, and for
 *   HELPTEXT.<ext>     each lone 0x0D (not followed by another 0x0D two
 *   ENDTEXT.<ext>      bytes later) overwrite as 0x20 + memmove the tail
 *   PREVIEWS.<ext>     one byte forward. Terminator is 0x1A.
 *
 *   KEEN2.EXE /        Apply the same text-file transform in place to the
 *   KEEN3.EXE          four embedded HELP/STORY/END/PREVIEWS regions. At
 *                      runtime these pointers resolve into cart ROM, so the
 *                      GBA build skips CVort_process_text_file entirely.
 *                      Offsets are relative to the MZ-stripped exeImage
 *                      origin (matching src/episodes/episode{2,3}.h).
 *
 * Usage: gba_preprocess_misc_host <staged_dir> <ext>
 *
 * Missing files are silently skipped (per-episode bakes may stage only a
 * subset of text files).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>

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

/* Mirror of CRLE_expandSwapped from src/third_party/cgenius (inlined here so
 * the host tool doesn't need SDL). finsize is a byte count stored in the
 * first LE word; elements written is finsize/2. */
static void crle_expand_swapped(uint16_t *dst, const uint8_t *src, uint16_t key) {
    size_t finsize = ((size_t)src[1] << 8) | src[0];
    finsize /= 2;
    size_t elementnum = 0;
    size_t i = 2;
    while (elementnum < finsize) {
        uint16_t value = (uint16_t)(((uint16_t)src[i+1] << 8) | src[i]);
        if (value == key) {
            size_t howmany = ((size_t)src[i+3] << 8) | src[i+2];
            value = (uint16_t)(((uint16_t)src[i+5] << 8) | src[i+4]);
            for (size_t j = 0; j < howmany; j++, elementnum++) {
                dst[elementnum] = value;
            }
            i += 6;
        } else {
            dst[elementnum] = value;
            elementnum++;
            i += 2;
        }
    }
}

/* Mirror of CVort_process_text_file in src/game/menus.c. Operates in-place. */
static void process_text_file(uint8_t *buffer, size_t len) {
    uint16_t numOfChars = 0, charCounter = 0;
    /* Original code trusts the presence of a 0x1A terminator. Guard against
     * a missing terminator so the host tool doesn't walk off the buffer. */
    while (numOfChars < len && buffer[numOfChars] != 0x1A) numOfChars++;
    if (numOfChars >= len) return;

    while (buffer[charCounter] != 0x1A) {
        if (buffer[charCounter] == 0x1F) {
            buffer[charCounter] = 0x0D;
            charCounter++;
        } else if (buffer[charCounter] == 0x0D) {
            if (buffer[charCounter + 2] == 0x0D) {
                do {
                    charCounter += 2;
                } while (buffer[charCounter] == 0x0D);
            } else {
                buffer[charCounter] = 0x20;
                memmove(buffer + charCounter + 1, buffer + charCounter + 2,
                        (size_t)(numOfChars - charCounter - 1));
                numOfChars--;
                charCounter++;
            }
        } else {
            charCounter++;
        }
    }
}

static int process_level(const char *path) {
    size_t in_size = 0;
    uint8_t *in = read_file(path, &in_size);
    if (!in) { perror(path); return 1; }
    if (in_size < 2) {
        fprintf(stderr, "%s: truncated (%zu bytes)\n", path, in_size);
        free(in);
        return 1;
    }
    size_t finsize_bytes = ((size_t)in[1] << 8) | in[0];
    if (finsize_bytes < 2 || (finsize_bytes & 1)) {
        fprintf(stderr, "%s: bad finsize header (%zu)\n", path, finsize_bytes);
        free(in);
        return 1;
    }
    size_t words = finsize_bytes / 2;
    uint16_t *dst = (uint16_t *)calloc(words, sizeof(uint16_t));
    if (!dst) { fprintf(stderr, "oom\n"); free(in); return 1; }
    crle_expand_swapped(dst, in, 0xFEFE);
    free(in);

    /* Runtime then memmoves one word forward, dropping dst[0] (which held
     * the uncompressed word count / 2). Output is the remaining words. */
    size_t out_words = words - 1;
    size_t out_bytes = out_words * 2;
    int rc = write_file(path, dst + 1, out_bytes);
    free(dst);
    if (rc == 0) {
        fprintf(stderr, "gba_preprocess_misc_host: %s (CRLE expanded, %zu bytes)\n",
                path, out_bytes);
    }
    return rc;
}

static int process_text(const char *path) {
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) return 0; /* missing text files are silently skipped */
    process_text_file(buf, sz);
    int rc = write_file(path, buf, sz);
    free(buf);
    if (rc == 0) {
        fprintf(stderr, "gba_preprocess_misc_host: %s (text file processed, %zu bytes)\n",
                path, sz);
    }
    return rc;
}

/* Mirror of the four CVort{2,3}_{HELP,STORY,END,PREVIEWS}_TEXT_OFFSET
 * constants in src/episodes/episode{2,3}.h. Offsets are relative to the
 * MZ-stripped exeImage origin. */
typedef struct {
    const char *filename;
    size_t offsets[4]; /* HELP, STORY, END, PREVIEWS */
} ExeTextTable_T;

static const ExeTextTable_T kExeTextTables[] = {
    { "KEEN2.EXE", { 0x15BC0, 0x16AC0, 0x15840, 0x163A0 } },
    { "KEEN3.EXE", { 0x179D0, 0x18BD0, 0x181A0, 0x184E0 } },
};

static int process_exe_embedded_texts(const char *path, const ExeTextTable_T *tbl) {
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) return 0; /* missing is fine — not every episode stages an EXE */
    if (sz < 64 || buf[0] != 'M' || buf[1] != 'Z') {
        fprintf(stderr, "%s: not an MZ executable, skipping\n", path);
        free(buf);
        return 0;
    }
    size_t hdr = 16u * ((size_t)buf[8] | ((size_t)buf[9] << 8));
    if (hdr >= sz) {
        fprintf(stderr, "%s: MZ header size %zu >= file size %zu\n", path, hdr, sz);
        free(buf);
        return 1;
    }
    for (size_t i = 0; i < 4; i++) {
        size_t off = tbl->offsets[i];
        size_t abs = hdr + off;
        if (abs >= sz) {
            fprintf(stderr, "%s: text region %zu (off=0x%zX) out of bounds\n",
                    path, i, off);
            free(buf);
            return 1;
        }
        process_text_file(buf + abs, sz - abs);
    }
    int rc = write_file(path, buf, sz);
    free(buf);
    if (rc == 0) {
        fprintf(stderr, "gba_preprocess_misc_host: %s (embedded texts processed)\n",
                path);
    }
    return rc;
}

/* Case-insensitive suffix/equality helpers against an already-uppercased
 * 8.3 name. */
static int str_eq_icase(const char *a, const char *b) {
    for (;;) {
        unsigned char ca = (unsigned char)*a, cb = (unsigned char)*b;
        if (ca >= 'a' && ca <= 'z') ca = (unsigned char)(ca - 32);
        if (cb >= 'a' && cb <= 'z') cb = (unsigned char)(cb - 32);
        if (ca != cb) return 0;
        if (!ca) return 1;
        a++; b++;
    }
}

static int is_level_name(const char *base, const char *ext_dot) {
    /* "LEVEL<digits><ext_dot>" where ext_dot is e.g. ".CK1" */
    if (strncasecmp(base, "LEVEL", 5) != 0) return 0;
    const char *p = base + 5;
    if (!(*p)) return 0;
    while (*p && *p != '.') {
        if (*p < '0' || *p > '9') return 0;
        p++;
    }
    return str_eq_icase(p, ext_dot);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <staged_dir> <ext>\n", argv[0]);
        return 2;
    }
    const char *dir_path = argv[1];
    const char *ext = argv[2];
    char ext_dot[32];
    snprintf(ext_dot, sizeof ext_dot, ".%s", ext);

    const char *text_bases[] = {
        "STORYTXT", "HELPTEXT", "ENDTEXT", "PREVIEWS"
    };

    DIR *d = opendir(dir_path);
    if (!d) { perror(dir_path); return 1; }
    struct dirent *ent;
    int rc = 0;
    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        char full[1024];
        snprintf(full, sizeof full, "%s/%s", dir_path, ent->d_name);
        struct stat st;
        if (stat(full, &st) != 0 || !S_ISREG(st.st_mode)) continue;

        if (is_level_name(ent->d_name, ext_dot)) {
            if (process_level(full)) { rc = 1; break; }
            continue;
        }
        for (size_t i = 0; i < sizeof(text_bases)/sizeof(text_bases[0]); i++) {
            char want[32];
            snprintf(want, sizeof want, "%s%s", text_bases[i], ext_dot);
            if (str_eq_icase(ent->d_name, want)) {
                if (process_text(full)) { rc = 1; }
                break;
            }
        }
        if (rc) break;
        for (size_t i = 0; i < sizeof(kExeTextTables)/sizeof(kExeTextTables[0]); i++) {
            if (str_eq_icase(ent->d_name, kExeTextTables[i].filename)) {
                if (process_exe_embedded_texts(full, &kExeTextTables[i])) { rc = 1; }
                break;
            }
        }
        if (rc) break;
    }
    closedir(d);
    return rc;
}
