/*
 * gba_data.h — ROM-embedded GAMEDATA table.
 *
 * Generated per-build by scripts/bake_gba_data.sh; the generator writes
 * gba_data_gen.c, which defines these symbols. Everything the FILE*
 * shim needs is expressed here.
 */

#ifndef CHOCOLATE_KEEN_GBA_DATA_H
#define CHOCOLATE_KEEN_GBA_DATA_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char    *name;     /* basename, uppercase ("EGAHEAD.CK1"). */
    const uint8_t *data;     /* pointer into .rodata (cart ROM). */
    size_t         size;     /* size in bytes. */
} ck_gba_rom_entry;

extern const ck_gba_rom_entry ck_gba_rom_entries[];
extern const unsigned         ck_gba_rom_entry_count;

/* Resolve a ROM-backed file by path and hand back the raw cart pointer
 * and size. Returns 0 on success, -1 if no entry matches. No FILE slot
 * is consumed — callers that only read should prefer this over fopen
 * + fread to avoid copying megabyte-scale blobs into EWRAM. */
int ck_gba_lookup_rom(const char *path, const uint8_t **out_ptr, size_t *out_size);

#endif
