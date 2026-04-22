/*
 * gba_fileio.c — implementation of the in-ROM/SRAM FILE* shim.
 *
 * Read path:
 *   - fopen(path, "rb") first asks the SRAM layer for the basename; if
 *     present the blob is copied into the file handle's ram_buf and
 *     served from there (SRAM is byte-access only and memcpy would
 *     issue wider loads). Otherwise the ROM table (gba_data_gen.c) is
 *     consulted and the handle's base points into ROM.
 *
 * Write path:
 *   - fopen(path, "wb") / "w" allocates a handle whose ram_buf is the
 *     staging area. fwrite copies into it; fclose hands the final buffer
 *     to CK_SRAM_Write, which persists it in cartridge SRAM.
 *
 * The SRAM-first read ordering means a saved CTRLPANL.CKx overrides the
 * ROM-bundled defaults the engine would otherwise fall back to, matching
 * how the DOS build prefers on-disk config.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "platform/gba_data.h"
#include "platform/gba_sram.h"

#define CK_GBA_FILE_NAME_MAX 16
#define CK_GBA_FILE_BUF_MAX  CK_SRAM_MAX_FILE_SIZE

/* The shim's FILE type. Defined here (not in the header) so external
 * code sees it as fully opaque. */
struct ck_gba_file {
    const uint8_t *base;          /* Read source (ROM ptr or ram_buf). */
    size_t         size;          /* Total readable bytes. */
    size_t         pos;
    int            in_use;
    int            eof;
    int            write_mode;    /* 1 if opened for write (→ SRAM on close). */
    char           name[CK_GBA_FILE_NAME_MAX];
    uint8_t        ram_buf[CK_GBA_FILE_BUF_MAX];
};

#define CK_GBA_MAX_OPEN_FILES 4
static struct ck_gba_file s_file_pool[CK_GBA_MAX_OPEN_FILES];

/* Sentinel "sink" file used for stdout/stderr redirection. */
static struct ck_gba_file s_sink = {0};
struct ck_gba_file *ck_gba_stderr = &s_sink;
struct ck_gba_file *ck_gba_stdout = &s_sink;

/* Case-insensitive compare for basenames. DOS short names are uppercase
 * in the data bundle; the engine sometimes hands us paths that are
 * uppercased elsewhere, but we don't want to rely on it. */
static int ck_strcasecmp_basename(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (char)toupper((unsigned char)*a);
        char cb = (char)toupper((unsigned char)*b);
        if (ca != cb) return (int)(ca - cb);
        a++; b++;
    }
    return (int)((unsigned char)*a - (unsigned char)*b);
}

static const char *ck_basename(const char *path) {
    const char *slash = strrchr(path, '/');
    if (slash) return slash + 1;
    const char *back = strrchr(path, '\\');
    if (back) return back + 1;
    return path;
}

static const ck_gba_rom_entry *ck_find_rom_entry(const char *basename) {
    for (unsigned i = 0; i < ck_gba_rom_entry_count; i++) {
        if (ck_strcasecmp_basename(basename, ck_gba_rom_entries[i].name) == 0) {
            return &ck_gba_rom_entries[i];
        }
    }
    return NULL;
}

static struct ck_gba_file *ck_alloc_slot(void) {
    for (int i = 0; i < CK_GBA_MAX_OPEN_FILES; i++) {
        if (!s_file_pool[i].in_use) {
            memset(&s_file_pool[i], 0, sizeof(s_file_pool[i]));
            s_file_pool[i].in_use = 1;
            return &s_file_pool[i];
        }
    }
    return NULL;
}

static void copy_basename(const char *basename, char out[CK_GBA_FILE_NAME_MAX]) {
    size_t i = 0;
    for (; i < CK_GBA_FILE_NAME_MAX - 1 && basename[i]; i++) {
        out[i] = basename[i];
    }
    for (; i < CK_GBA_FILE_NAME_MAX; i++) out[i] = '\0';
}

int ck_gba_lookup_rom(const char *path, const uint8_t **out_ptr, size_t *out_size) {
    if (!path || !out_ptr || !out_size) return -1;
    const ck_gba_rom_entry *ent = ck_find_rom_entry(ck_basename(path));
    if (!ent) return -1;
    *out_ptr  = ent->data;
    *out_size = ent->size;
    return 0;
}

FILE *ck_gba_fopen(const char *path, const char *mode) {
    if (!path || !mode) return NULL;

    const char *basename = ck_basename(path);
    const int is_write = (mode[0] == 'w') || (mode[0] == 'a');

    /* r+ and similar "update" modes aren't needed anywhere in the engine;
     * reject them so callers don't silently get a read-only handle. */
    if (mode[0] == 'r' && mode[1] == '+') return NULL;

    if (is_write) {
        struct ck_gba_file *fp = ck_alloc_slot();
        if (!fp) return NULL;
        fp->write_mode = 1;
        fp->size = 0;
        fp->pos = 0;
        copy_basename(basename, fp->name);
        if (mode[0] == 'a') {
            /* Seed the write buffer with the current SRAM contents so
             * appends behave like a real filesystem. */
            int existing = CK_SRAM_Read(fp->name, fp->ram_buf, sizeof(fp->ram_buf));
            if (existing > 0) {
                fp->size = (size_t)existing;
                fp->pos = fp->size;
            }
        }
        fp->base = fp->ram_buf;
        return fp;
    }

    /* Read mode: SRAM overrides ROM so saved config beats the default bundle. */
    struct ck_gba_file *fp = ck_alloc_slot();
    if (!fp) return NULL;
    copy_basename(basename, fp->name);

    int sram_size = CK_SRAM_Read(fp->name, fp->ram_buf, sizeof(fp->ram_buf));
    if (sram_size >= 0) {
        fp->base = fp->ram_buf;
        fp->size = (size_t)sram_size;
        fp->pos = 0;
        return fp;
    }

    const ck_gba_rom_entry *ent = ck_find_rom_entry(basename);
    if (!ent) {
        fp->in_use = 0;
        return NULL;
    }
    fp->base = ent->data;
    fp->size = ent->size;
    fp->pos = 0;
    return fp;
}

int ck_gba_fclose(FILE *fp) {
    if (!fp || fp == &s_sink) return 0;
    if (fp->write_mode) {
        CK_SRAM_Write(fp->name, fp->ram_buf, fp->size);
    }
    fp->in_use = 0;
    return 0;
}

size_t ck_gba_fread(void *buf, size_t size, size_t count, FILE *fp) {
    if (!fp || fp == &s_sink || !buf || size == 0 || count == 0) return 0;
    if (fp->write_mode) return 0;
    size_t avail = fp->size - fp->pos;
    size_t want = size * count;
    size_t take = want > avail ? avail : want;
    if (take == 0) {
        fp->eof = 1;
        return 0;
    }
    memcpy(buf, fp->base + fp->pos, take);
    fp->pos += take;
    if (fp->pos >= fp->size) fp->eof = 1;
    return take / size;
}

size_t ck_gba_fwrite(const void *buf, size_t size, size_t count, FILE *fp) {
    if (!fp || fp == &s_sink || !buf || !fp->write_mode) return 0;
    size_t want = size * count;
    if (fp->pos + want > CK_GBA_FILE_BUF_MAX) {
        want = CK_GBA_FILE_BUF_MAX - fp->pos;
    }
    if (want == 0) return 0;
    memcpy(fp->ram_buf + fp->pos, buf, want);
    fp->pos += want;
    if (fp->pos > fp->size) fp->size = fp->pos;
    return want / size;
}

int ck_gba_fseek(FILE *fp, long offset, int whence) {
    if (!fp || fp == &s_sink) return -1;
    long base;
    switch (whence) {
        case 0: base = 0;                break; /* SEEK_SET */
        case 1: base = (long)fp->pos;    break; /* SEEK_CUR */
        case 2: base = (long)fp->size;   break; /* SEEK_END */
        default: return -1;
    }
    long newpos = base + offset;
    if (newpos < 0) return -1;
    long cap = fp->write_mode ? (long)CK_GBA_FILE_BUF_MAX : (long)fp->size;
    if (newpos > cap) newpos = cap;
    fp->pos = (size_t)newpos;
    fp->eof = 0;
    return 0;
}

long ck_gba_ftell(FILE *fp) {
    if (!fp || fp == &s_sink) return -1;
    return (long)fp->pos;
}

void ck_gba_rewind(FILE *fp) {
    if (!fp || fp == &s_sink) return;
    fp->pos = 0;
    fp->eof = 0;
}

int ck_gba_feof(FILE *fp) {
    if (!fp || fp == &s_sink) return 1;
    return fp->eof;
}

int ck_gba_ferror(FILE *fp) { (void)fp; return 0; }

int ck_gba_fgetc(FILE *fp) {
    if (!fp || fp == &s_sink || fp->write_mode) return -1;
    if (fp->pos >= fp->size) { fp->eof = 1; return -1; }
    return fp->base[fp->pos++];
}

char *ck_gba_fgets(char *buf, int n, FILE *fp) {
    if (!fp || fp == &s_sink || !buf || n <= 0 || fp->write_mode) return NULL;
    int i = 0;
    while (i < n - 1 && fp->pos < fp->size) {
        char ch = (char)fp->base[fp->pos++];
        buf[i++] = ch;
        if (ch == '\n') break;
    }
    if (i == 0) { fp->eof = 1; return NULL; }
    buf[i] = '\0';
    return buf;
}

int ck_gba_fputs(const char *s, FILE *fp) {
    if (!s || !fp || fp == &s_sink || !fp->write_mode) return 0;
    size_t len = strlen(s);
    return (int)ck_gba_fwrite(s, 1, len, fp);
}

int ck_gba_fputc(int ch, FILE *fp) {
    if (!fp || fp == &s_sink || !fp->write_mode) return ch;
    uint8_t b = (uint8_t)ch;
    ck_gba_fwrite(&b, 1, 1, fp);
    return ch;
}

int ck_gba_fprintf(FILE *fp, const char *fmt, ...) { (void)fp; (void)fmt; return 0; }
int ck_gba_vfprintf(FILE *fp, const char *fmt, va_list ap) { (void)fp; (void)fmt; (void)ap; return 0; }
int ck_gba_fflush(FILE *fp) { (void)fp; return 0; }
