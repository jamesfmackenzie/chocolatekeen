/*
 * gba_unlzexe_host.c — host-side LZEXE decompressor.
 *
 * Compiled by scripts/bake_gba_data.sh and run once per data bake to
 * unpack KEEN*.EXE into a raw DOS MZ image. The GBA has nowhere near
 * enough heap to do this at boot (Cunlzexe peaks around 200 KiB for
 * Keen 1's 51 KiB compressed executable), so we move the cost to the
 * build machine and bake the uncompressed payload into ROM directly.
 *
 * Pulls in the engine's own Cunlzexe.c so the host and target share the
 * same decompressor and no second implementation can drift.
 *
 * Usage: gba_unlzexe_host <input.exe> <output.exe>
 * Behavior:
 *   - If the input is a recognised LZEXE'd DOS exe, writes the fully
 *     reconstructed uncompressed exe (MZ header + payload) to output.
 *   - If the input is already an uncompressed MZ exe, copies bytes
 *     through unchanged so the bake can treat every exe uniformly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../src/third_party/cgenius/fileio/compression/Cunlzexe.h"

static int copy_through(const char *in_path, const char *out_path) {
    FILE *in = fopen(in_path, "rb");
    if (!in) { perror(in_path); return 1; }
    FILE *out = fopen(out_path, "wb");
    if (!out) { perror(out_path); fclose(in); return 1; }
    unsigned char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) { perror(out_path); fclose(in); fclose(out); return 1; }
    }
    fclose(in);
    fclose(out);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <input.exe> <output.exe>\n", argv[0]);
        return 2;
    }
    const char *in_path  = argv[1];
    const char *out_path = argv[2];

    FILE *in = fopen(in_path, "rb");
    if (!in) { perror(in_path); return 1; }
    fseek(in, 0, SEEK_END);
    long in_size = ftell(in);
    fseek(in, 0, SEEK_SET);
    if (in_size <= 0) { fprintf(stderr, "%s: empty\n", in_path); fclose(in); return 1; }
    uint8_t *in_buf = (uint8_t *)malloc((size_t)in_size);
    if (!in_buf) { fprintf(stderr, "oom\n"); fclose(in); return 1; }
    if (fread(in_buf, 1, (size_t)in_size, in) != (size_t)in_size) {
        perror(in_path); free(in_buf); fclose(in); return 1;
    }
    fclose(in);

    uint8_t *out_buf = NULL;
    if (!Cunlzexe_decompress(in_buf, &out_buf)) {
        /* Not an LZEXE'd exe — pass through untouched. */
        if (out_buf) { Cunlzexe_free(&out_buf); }
        free(in_buf);
        return copy_through(in_path, out_path);
    }
    free(in_buf);

    unsigned long uncompressed_size = Cunlzexe_getUncompressedExeSize();
    FILE *out = fopen(out_path, "wb");
    if (!out) { perror(out_path); Cunlzexe_free(&out_buf); return 1; }
    if (fwrite(out_buf, 1, uncompressed_size, out) != uncompressed_size) {
        perror(out_path); fclose(out); Cunlzexe_free(&out_buf); return 1;
    }
    fclose(out);
    Cunlzexe_free(&out_buf);
    fprintf(stderr, "gba_unlzexe_host: %s -> %s (%lu bytes)\n",
            in_path, out_path, uncompressed_size);
    return 0;
}
