/*
 * gba_sram.c — cartridge-SRAM driver backing the fileio save path.
 *
 * SRAM on GBA lives at 0x0E000000 and accepts only 8-bit loads/stores,
 * so every access here goes through a volatile uint8_t pointer one byte
 * at a time. On first boot (or on a cart with garbage in SRAM) the magic
 * check fails and we zero the area before use.
 */

#include "platform/gba_sram.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>

#define CK_SRAM_BASE        ((volatile uint8_t *)0x0E000000)
#define CK_SRAM_SIZE        0x8000u     /* 32 KiB */

#define CK_SRAM_HEADER_SIZE 16
#define CK_SRAM_NAME_MAX    16
#define CK_SRAM_SLOT_META   32          /* name(16) + size(2) + reserved(14) */
#define CK_SRAM_SLOT_SIZE   (CK_SRAM_SLOT_META + CK_SRAM_MAX_FILE_SIZE)
#define CK_SRAM_SLOT_COUNT  ((CK_SRAM_SIZE - CK_SRAM_HEADER_SIZE) / CK_SRAM_SLOT_SIZE)

#define CK_SRAM_VERSION     1

/* Many emulators and flashcarts look for a literal "SRAM_Vnnn" string in
 * the ROM to pick the save backend. `used` alone isn't enough — with
 * -ffunction-sections/-fdata-sections the marker ends up in its own
 * .rodata section, and --gc-sections drops sections that aren't reached
 * from any live code. So we also take its address at runtime in
 * CK_SRAM_Init so the linker sees it as live. */
__attribute__((used))
const char ck_gba_sram_autodetect_marker[] = "SRAM_V113";

/* Volatile sink written in CK_SRAM_Init that anchors the marker as live. */
volatile const char *ck_gba_sram_autodetect_marker_anchor = NULL;

static inline size_t ck_slot_offset(unsigned idx) {
    return (size_t)CK_SRAM_HEADER_SIZE + (size_t)idx * CK_SRAM_SLOT_SIZE;
}

static void sram_read(size_t offset, uint8_t *buf, size_t len) {
    volatile const uint8_t *src = CK_SRAM_BASE + offset;
    for (size_t i = 0; i < len; i++) buf[i] = src[i];
}

static void sram_write(size_t offset, const uint8_t *buf, size_t len) {
    volatile uint8_t *dst = CK_SRAM_BASE + offset;
    for (size_t i = 0; i < len; i++) dst[i] = buf[i];
}

static inline uint8_t sram_read_byte(size_t offset) {
    return CK_SRAM_BASE[offset];
}

static inline void sram_write_byte(size_t offset, uint8_t v) {
    CK_SRAM_BASE[offset] = v;
}

static void normalize_name(const char *in, char out[CK_SRAM_NAME_MAX]) {
    int i;
    for (i = 0; i < CK_SRAM_NAME_MAX - 1 && in && in[i]; i++) {
        out[i] = (char)toupper((unsigned char)in[i]);
    }
    for (; i < CK_SRAM_NAME_MAX; i++) out[i] = '\0';
}

static int find_slot(const char *name) {
    char upper[CK_SRAM_NAME_MAX];
    normalize_name(name, upper);
    for (unsigned i = 0; i < CK_SRAM_SLOT_COUNT; i++) {
        size_t off = ck_slot_offset(i);
        if (sram_read_byte(off) == 0) continue;
        char stored[CK_SRAM_NAME_MAX];
        sram_read(off, (uint8_t *)stored, CK_SRAM_NAME_MAX);
        if (memcmp(stored, upper, CK_SRAM_NAME_MAX) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static int find_free_slot(void) {
    for (unsigned i = 0; i < CK_SRAM_SLOT_COUNT; i++) {
        if (sram_read_byte(ck_slot_offset(i)) == 0) return (int)i;
    }
    return -1;
}

static void format_sram(void) {
    for (size_t i = 0; i < CK_SRAM_SIZE; i++) sram_write_byte(i, 0);
    sram_write_byte(0, 'C');
    sram_write_byte(1, 'K');
    sram_write_byte(2, 'S');
    sram_write_byte(3, 'V');
    sram_write_byte(4, CK_SRAM_VERSION);
    sram_write_byte(5, (uint8_t)CK_SRAM_SLOT_COUNT);
}

void CK_SRAM_Init(void) {
    /* WAITCNT SRAM wait control (bits 0..1): 0=4, 1=3, 2=2, 3=8 cycles.
     * Eight cycles is the safe value that works with every SRAM chip
     * actually shipped in GBA carts, so use it. */
    volatile uint16_t *waitcnt = (volatile uint16_t *)0x04000204;
    *waitcnt = (uint16_t)((*waitcnt & ~(uint16_t)0x0003) | (uint16_t)0x0003);

    /* Anchor the "SRAM_V113" marker as live so --gc-sections keeps it. */
    ck_gba_sram_autodetect_marker_anchor = ck_gba_sram_autodetect_marker;

    if (sram_read_byte(0) != 'C' ||
        sram_read_byte(1) != 'K' ||
        sram_read_byte(2) != 'S' ||
        sram_read_byte(3) != 'V' ||
        sram_read_byte(4) != CK_SRAM_VERSION) {
        format_sram();
    }
}

int CK_SRAM_Read(const char *name, void *buf, size_t buf_size) {
    int idx = find_slot(name);
    if (idx < 0) return -1;
    size_t off = ck_slot_offset((unsigned)idx);
    size_t size = (size_t)sram_read_byte(off + 16) |
                  ((size_t)sram_read_byte(off + 17) << 8);
    if (size > CK_SRAM_MAX_FILE_SIZE) return -1;
    if (buf && buf_size) {
        size_t take = size < buf_size ? size : buf_size;
        sram_read(off + CK_SRAM_SLOT_META, (uint8_t *)buf, take);
    }
    return (int)size;
}

int CK_SRAM_Write(const char *name, const void *buf, size_t size) {
    if (!name || (!buf && size) || size > CK_SRAM_MAX_FILE_SIZE) return -1;

    int idx = find_slot(name);
    if (idx < 0) idx = find_free_slot();
    if (idx < 0) return -1;

    char upper[CK_SRAM_NAME_MAX];
    normalize_name(name, upper);
    size_t off = ck_slot_offset((unsigned)idx);

    sram_write(off, (const uint8_t *)upper, CK_SRAM_NAME_MAX);
    sram_write_byte(off + 16, (uint8_t)(size & 0xFF));
    sram_write_byte(off + 17, (uint8_t)((size >> 8) & 0xFF));
    for (size_t i = 18; i < CK_SRAM_SLOT_META; i++) sram_write_byte(off + i, 0);
    if (size) sram_write(off + CK_SRAM_SLOT_META, (const uint8_t *)buf, size);
    return (int)size;
}

bool CK_SRAM_Exists(const char *name) {
    return find_slot(name) >= 0;
}
