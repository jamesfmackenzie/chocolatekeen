/*
 * test_serialization_io.c - Focused tests for serialization IO helpers
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/core/io_helpers.h"

typedef struct {
    uint8_t a[4];
    uint8_t b[2];
    uint8_t c;
} TestPayload_T;

static bool read_payload(FILE *fp, TestPayload_T *out) {
    bool loadOk = true;
    CK_IO_EXPECT(loadOk, fread(out->a, sizeof(out->a), 1, fp), 1);
    CK_IO_EXPECT(loadOk, fread(out->b, sizeof(out->b), 1, fp), 1);
    CK_IO_EXPECT(loadOk, fread(&out->c, sizeof(out->c), 1, fp), 1);
    return loadOk;
}

static bool write_payload(FILE *fp, const TestPayload_T *in) {
    bool saveOk = true;
    CK_IO_EXPECT(saveOk, fwrite(in->a, sizeof(in->a), 1, fp), 1);
    CK_IO_EXPECT(saveOk, fwrite(in->b, sizeof(in->b), 1, fp), 1);
    CK_IO_EXPECT(saveOk, fwrite(&in->c, sizeof(in->c), 1, fp), 1);
    return saveOk;
}

static void write_bytes_or_fail(FILE *fp, const void *ptr, size_t size) {
    size_t written = fwrite(ptr, size, 1, fp);
    TEST_ASSERT_EQUAL(1, written);
}

TEST_SUITE(test_serialization_read_success)
{
    TestPayload_T expected = {{1, 2, 3, 4}, {5, 6}, 7};
    TestPayload_T actual = {0};
    FILE *fp = tmpfile();
    TEST_ASSERT(fp != NULL);
    if (!fp)
        return;

    write_bytes_or_fail(fp, expected.a, sizeof(expected.a));
    write_bytes_or_fail(fp, expected.b, sizeof(expected.b));
    write_bytes_or_fail(fp, &expected.c, sizeof(expected.c));
    rewind(fp);

    TEST_ASSERT(read_payload(fp, &actual));
    TEST_ASSERT_MEM_EQUAL(&expected, &actual, sizeof(expected));

    fclose(fp);
}

TEST_SUITE(test_serialization_read_short_middle_fails)
{
    TestPayload_T actual = {0};
    uint8_t partial_a[4] = {1, 2, 3, 4};
    uint8_t partial_b[1] = {5}; /* intentionally short */

    FILE *fp = tmpfile();
    TEST_ASSERT(fp != NULL);
    if (!fp)
        return;

    write_bytes_or_fail(fp, partial_a, sizeof(partial_a));
    write_bytes_or_fail(fp, partial_b, sizeof(partial_b));
    rewind(fp);

    TEST_ASSERT(!read_payload(fp, &actual));
    fclose(fp);
}

TEST_SUITE(test_serialization_write_success)
{
    TestPayload_T expected = {{10, 20, 30, 40}, {50, 60}, 70};
    TestPayload_T actual = {0};
    FILE *fp = tmpfile();
    TEST_ASSERT(fp != NULL);
    if (!fp)
        return;

    TEST_ASSERT(write_payload(fp, &expected));
    rewind(fp);
    TEST_ASSERT(read_payload(fp, &actual));
    TEST_ASSERT_MEM_EQUAL(&expected, &actual, sizeof(expected));

    fclose(fp);
}

TEST_SUITE(test_serialization_write_to_readonly_fails)
{
    TestPayload_T payload = {{1, 2, 3, 4}, {5, 6}, 7};
    char tmpPath[128];
    FILE *fp = NULL;

    snprintf(tmpPath, sizeof(tmpPath), "/tmp/chocolatekeen_test_%ld.tmp", (long)getpid());
    fp = fopen(tmpPath, "wb");
    TEST_ASSERT(fp != NULL);
    if (!fp)
        return;
    fclose(fp);

    fp = fopen(tmpPath, "rb");
    TEST_ASSERT(fp != NULL);
    if (!fp)
        return;

    TEST_ASSERT(!write_payload(fp, &payload));
    fclose(fp);
    remove(tmpPath);
}

TEST_SUITE(test_serialization_io)
{
    RUN_TEST(test_serialization_read_success);
    RUN_TEST(test_serialization_read_short_middle_fails);
    RUN_TEST(test_serialization_write_success);
    RUN_TEST(test_serialization_write_to_readonly_fails);
}
