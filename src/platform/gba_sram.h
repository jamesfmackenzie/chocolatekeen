/*
 * gba_sram.h — cartridge-SRAM-backed named-blob storage for Chocolate
 * Keen on GBA. Used by the fileio shim to persist CTRLPANL.CKx, save
 * games, and high scores across power cycles.
 *
 * Layout (32 KiB total):
 *   [0..15]        header: magic "CKSV" + version + slot count
 *   [16..16+N*S)   N fixed-size slots, each S bytes:
 *                    name[16]  (null-terminated, uppercase; first byte
 *                               == 0 means empty slot)
 *                    size_le   (uint16_t)
 *                    reserved  (14 bytes, zero)
 *                    data[CK_SRAM_MAX_FILE_SIZE]
 *
 * SRAM is byte-access only, so all I/O must go through the helpers below.
 */

#ifndef CHOCOLATE_KEEN_GBA_SRAM_H
#define CHOCOLATE_KEEN_GBA_SRAM_H

#include <stddef.h>
#include <stdbool.h>

#define CK_SRAM_MAX_FILE_SIZE 2016

#ifdef __cplusplus
extern "C" {
#endif

void CK_SRAM_Init(void);

/* Returns the stored size on success (may be > buf_size, in which case
 * only buf_size bytes were copied) or -1 if no such entry exists. */
int  CK_SRAM_Read(const char *name, void *buf, size_t buf_size);

/* Returns bytes written or -1 on failure (name too long, data too big,
 * or directory full). */
int  CK_SRAM_Write(const char *name, const void *buf, size_t size);

bool CK_SRAM_Exists(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* CHOCOLATE_KEEN_GBA_SRAM_H */
