/*
 * test_crle.c - Tests for CRLE decompression
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_framework.h"

/* Include the CRLE implementation */
#include "../src/decompression/CGenius/fileio/compression/CRLE.h"

/* Test: Simple no-compression case
 * When there's no key marker, data should be copied as-is */
TEST_SUITE(test_crle_no_compression)
{
    /* Input: size=8 (little-endian, in bytes), then 8 bytes = 4 words */
    byte src[] = { 0x08, 0x00,  /* Final size: 8 bytes = 4 words */
                   0x01, 0x00,  /* word 0x0001 */
                   0x02, 0x00,  /* word 0x0002 */
                   0x03, 0x00,  /* word 0x0003 */
                   0x04, 0x00 }; /* word 0x0004 */

    word dst[4] = { 0 };
    word expected[] = { 0x0001, 0x0002, 0x0003, 0x0004 };
    word key = 0xFEFE;  /* Standard RLE key */

    CRLE_expandSwapped(dst, src, key);

    TEST_ASSERT_MEM_EQUAL(expected, dst, sizeof(expected));
}

/* Test: Simple RLE compression
 * Key marker followed by count and value should repeat */
TEST_SUITE(test_crle_simple_rle)
{
    /* Input: size=2, then key(0xFEFE), count=3, value=0x0042 */
    byte src[] = { 0x02, 0x00,  /* Final size: 2 */
                   0xFE, 0xFE,  /* Key marker */
                   0x03, 0x00,  /* Count: 3 */
                   0x42, 0x00 }; /* Value: 0x0042 */

    word dst[4] = { 0 };
    word expected[] = { 0x0042, 0x0042, 0x0042, 0x0000 };
    word key = 0xFEFE;

    CRLE_expandSwapped(dst, src, key);

    /* Only check first 3 elements since that's what we expect */
    for (int i = 0; i < 3; i++) {
        tests_run++;
        if (dst[i] == expected[i]) {
            tests_passed++;
        } else {
            tests_failed++;
            printf("  FAIL: dst[%d] = 0x%04X, expected 0x%04X (%s:%d)\n",
                i, dst[i], expected[i], __FILE__, __LINE__);
        }
    }
}

/* Test: Mixed data and RLE
 * Some literal data, then RLE, then more literal data */
TEST_SUITE(test_crle_mixed)
{
    /* Input: size=12 bytes (6 words)
     * word 0x0001 (literal)
     * word 0x0002 (literal)
     * key(0xFEFE), count=2, value=0x0033 (RLE)
     * word 0x0004 (literal)
     * word 0x0005 (literal) */
    byte src[] = { 0x0C, 0x00,  /* Final size: 12 bytes = 6 words */
                   0x01, 0x00,  /* literal 0x0001 */
                   0x02, 0x00,  /* literal 0x0002 */
                   0xFE, 0xFE,  /* Key marker */
                   0x02, 0x00,  /* Count: 2 */
                   0x33, 0x00,  /* Value: 0x0033 */
                   0x04, 0x00,  /* literal 0x0004 */
                   0x05, 0x00 }; /* literal 0x0005 */

    word dst[6] = { 0 };
    word expected[] = { 0x0001, 0x0002, 0x0033, 0x0033, 0x0004, 0x0005 };
    word key = 0xFEFE;

    CRLE_expandSwapped(dst, src, key);

    TEST_ASSERT_MEM_EQUAL(expected, dst, sizeof(expected));
}

/* Test: Empty input (size=0) */
TEST_SUITE(test_crle_empty)
{
    byte src[] = { 0x00, 0x00 };  /* Final size: 0 */
    word dst[1] = { 0xFFFF };  /* Fill with sentinel */
    word key = 0xFEFE;

    CRLE_expandSwapped(dst, src, key);

    /* dst should remain unchanged since nothing was written */
    TEST_ASSERT_EQUAL(0xFFFF, dst[0]);
}

/* Main runner for CRLE tests */
void test_crle(void)
{
    RUN_TEST(test_crle_no_compression);
    RUN_TEST(test_crle_simple_rle);
    RUN_TEST(test_crle_mixed);
    RUN_TEST(test_crle_empty);
}
