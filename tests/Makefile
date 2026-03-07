# Makefile for Chocolate Keen tests

CC=gcc
CFLAGS=-std=c99 -I../src -I/usr/include/SDL2 -Wall -Wextra -g
NAV_CFLAGS=$(CFLAGS)
SDLCONFIG=sdl2-config

# Link against SDL (needed for CRLE which uses SDL types)
LDFLAGS=`$(SDLCONFIG) --libs`

# Test executables
TEST_EXE=test_runner
NAV_TEST_EXE=test_input_nav_runner

# Source files
TEST_SRC=test_runner.c
NAV_TEST_SRC=test_input_nav_runner.c

# Source files under test
CRLE_SRC=../src/third_party/cgenius/fileio/compression/CRLE.c
INPUT_SRC=../src/input/input.c
INPUT_NAMES_SRC=../src/input/input_names.c
INPUT_DEFAULT_MAPPINGS_SRC=../src/input/input_default_mappings.c
MEMDATA_SRC=../src/render/memdata.c

# Build rules
$(TEST_EXE): $(TEST_SRC) $(CRLE_SRC) $(INPUT_SRC) $(MEMDATA_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) $(CRLE_SRC) $(LDFLAGS) -o $(TEST_EXE)

$(NAV_TEST_EXE): $(NAV_TEST_SRC) $(CRLE_SRC) $(INPUT_SRC) $(INPUT_NAMES_SRC) $(INPUT_DEFAULT_MAPPINGS_SRC) $(MEMDATA_SRC)
	$(CC) $(NAV_CFLAGS) $(NAV_TEST_SRC) $(CRLE_SRC) $(INPUT_SRC) $(INPUT_NAMES_SRC) $(INPUT_DEFAULT_MAPPINGS_SRC) $(MEMDATA_SRC) $(LDFLAGS) -o $(NAV_TEST_EXE)

# Run tests
run: $(TEST_EXE)
	./$(TEST_EXE)

run-nav: $(NAV_TEST_EXE)
	./$(NAV_TEST_EXE)

run-all: run run-nav

# Clean
clean:
	rm -f $(TEST_EXE) $(NAV_TEST_EXE)

.PHONY: run run-nav run-all clean
