/*
 * test_framework.h - Minimal test framework for Chocolate Keen
 *
 * Simple unit testing without external dependencies.
 * Use TEST_ASSERT(condition) to check conditions.
 * Use TEST_ASSERT_EQUAL(expected, actual) for equality checks.
 */

#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test counters */
extern int tests_run;
extern int tests_passed;
extern int tests_failed;

/* Test macros */
#define TEST_ASSERT(condition) do { \
    tests_run++; \
    if (condition) { \
        tests_passed++; \
    } else { \
        tests_failed++; \
        printf("  FAIL: %s (%s:%d)\n", #condition, __FILE__, __LINE__); \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual) do { \
    tests_run++; \
    if ((expected) == (actual)) { \
        tests_passed++; \
    } else { \
        tests_failed++; \
        printf("  FAIL: Expected %ld, got %ld (%s:%d)\n", \
            (long)(expected), (long)(actual), __FILE__, __LINE__); \
    } \
} while(0)

#define TEST_ASSERT_MEM_EQUAL(expected, actual, size) do { \
    tests_run++; \
    if (memcmp((expected), (actual), (size)) == 0) { \
        tests_passed++; \
    } else { \
        tests_failed++; \
        printf("  FAIL: Memory mismatch (%s:%d)\n", __FILE__, __LINE__); \
    } \
} while(0)

/* Test suite runner */
#define TEST_SUITE(name) void name(void)
#define RUN_TEST(test) do { \
    printf("  Running %s... ", #test); \
    test(); \
    printf("\n"); \
} while(0)

#define RUN_SUITE(suite) do { \
    printf("Running %s:\n", #suite); \
    suite(); \
} while(0)

/* Print final results */
#define TEST_SUMMARY() do { \
    printf("\n--- Test Results ---\n"); \
    printf("Passed: %d/%d\n", tests_passed, tests_run); \
    printf("Failed: %d/%d\n", tests_failed, tests_run); \
    if (tests_failed > 0) { \
        printf("TESTS FAILED\n"); \
        exit(1); \
    } else { \
        printf("ALL TESTS PASSED\n"); \
        exit(0); \
    } \
} while(0)

#endif /* TEST_FRAMEWORK_H_ */
