/*
 * test_runner.c - Test runner for Chocolate Keen
 */

#include "test_framework.h"
#include "test_crle.c"
#include "test_serialization_io.c"

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

int main(int argc, char *argv[])
{
    printf("=== Chocolate Keen Test Suite ===\n\n");

    RUN_SUITE(test_crle);
    RUN_SUITE(test_serialization_io);

    TEST_SUMMARY();
    return 0;
}
