/*
 * test_input_nav_runner.c - Runner for input mapping traversal tests
 */

#include "test_framework.h"
#include "test_input_mapping_nav.c"

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

int main(void)
{
    printf("=== Chocolate Keen Input Mapping Tests ===\n\n");

    RUN_SUITE(test_input_mapping_nav);

    TEST_SUMMARY();
    return 0;
}
