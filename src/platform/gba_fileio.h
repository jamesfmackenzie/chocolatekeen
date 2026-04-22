/*
 * gba_fileio.h — in-ROM FILE* shim for Chocolate Keen on GBA.
 *
 * The engine reads its assets through standard C stdio (fopen/fread/…),
 * picking files like "EGAHEAD.CK1" out of a directory supplied by the
 * platform layer. The GBA has no filesystem, so we keep the engine's
 * call-sites intact and redirect them to a name-indexed table of blobs
 * that the linker has baked into ROM.
 *
 * The redirection is implemented via #define of the libc names to our
 * own ck_gba_* variants. Behaviour:
 *   - fopen(path, "rb")  : looks up the basename in the ROM table,
 *                          returns a FILE* whose fread/fseek/ftell/fclose
 *                          walk the in-ROM buffer.
 *   - fopen(path, "wb"/"w") : returns NULL (no writable storage).
 *
 * If CHOCOLATE_KEEN_ENABLE_SRAM is defined in the future, rw_misc paths
 * can be routed to a simple SRAM-backed arena here.
 */

#ifndef CHOCOLATE_KEEN_GBA_FILEIO_H
#define CHOCOLATE_KEEN_GBA_FILEIO_H

#include <stddef.h>
#include <stdarg.h>

/* The real libc FILE type is not used; we return pointers to our own
 * struct and cast them to the public FILE* opaque type. */
typedef struct ck_gba_file ck_gba_file;
#define FILE ck_gba_file

#ifdef __cplusplus
extern "C" {
#endif

FILE *ck_gba_fopen(const char *path, const char *mode);
int   ck_gba_fclose(FILE *fp);
size_t ck_gba_fread(void *buf, size_t size, size_t count, FILE *fp);
size_t ck_gba_fwrite(const void *buf, size_t size, size_t count, FILE *fp);
int   ck_gba_fseek(FILE *fp, long offset, int whence);
long  ck_gba_ftell(FILE *fp);
void  ck_gba_rewind(FILE *fp);
int   ck_gba_feof(FILE *fp);
int   ck_gba_ferror(FILE *fp);
int   ck_gba_fgetc(FILE *fp);
char *ck_gba_fgets(char *buf, int n, FILE *fp);
int   ck_gba_fputs(const char *s, FILE *fp);
int   ck_gba_fputc(int ch, FILE *fp);
int   ck_gba_fprintf(FILE *fp, const char *fmt, ...);
int   ck_gba_vfprintf(FILE *fp, const char *fmt, va_list ap);
int   ck_gba_fflush(FILE *fp);

/* A sentinel "always-empty file" pointer used as the shim's stderr so that
 * fprintf(stderr, …) calls compile without any host-side output. */
extern FILE *ck_gba_stderr;
extern FILE *ck_gba_stdout;

/* Redirect libc stdio to the shim. These macros are scoped by the
 * force-include; they don't leak outside GBA compilation. */
/* The newlib stdio that ships with devkitARM defines several of these as
 * function-like macros. Undef first so our redirection wins. */
#undef fopen
#undef fclose
#undef fread
#undef fwrite
#undef fseek
#undef ftell
#undef rewind
#undef feof
#undef ferror
#undef fgetc
#undef fgets
#undef fputs
#undef fputc
#undef fprintf
#undef vfprintf
#undef fflush

#define fopen  ck_gba_fopen
#define fclose ck_gba_fclose
#define fread  ck_gba_fread
#define fwrite ck_gba_fwrite
#define fseek  ck_gba_fseek
#define ftell  ck_gba_ftell
#define rewind ck_gba_rewind
#define feof   ck_gba_feof
#define ferror ck_gba_ferror
#define fgetc  ck_gba_fgetc
#define fgets  ck_gba_fgets
#define fputs  ck_gba_fputs
#define fputc  ck_gba_fputc
#define fprintf ck_gba_fprintf
#define vfprintf ck_gba_vfprintf
#define fflush ck_gba_fflush

/* stdio.h already declared stderr as a real FILE*; redefine to our sink. */
#undef stderr
#undef stdout
#define stderr ck_gba_stderr
#define stdout ck_gba_stdout

#ifdef __cplusplus
}
#endif

#endif /* CHOCOLATE_KEEN_GBA_FILEIO_H */
