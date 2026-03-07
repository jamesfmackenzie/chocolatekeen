# Makefile for Chocolate Keen tests

CC=gcc
CFLAGS=-std=c99 -I../src -I/usr/include/SDL2 -Wall -Wextra -g
SDLCONFIG=sdl2-config

# Link against SDL (needed for CRLE which uses SDL types)
LDFLAGS=`$(SDLCONFIG) --libs`

# Test executable
TEST_EXE=test_runner

# Source files
TEST_SRC=test_runner.c

# Source files we're testing (need to compile these too)
CRLE_SRC=../src/third_party/cgenius/fileio/compression/CRLE.c
INPUT_SRC=../src/input/input.c
MEMDATA_SRC=../src/render/memdata.c

# Build rule
$(TEST_EXE): $(TEST_SRC) $(CRLE_SRC) $(INPUT_SRC) $(MEMDATA_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) $(CRLE_SRC) $(INPUT_SRC) $(MEMDATA_SRC) $(LDFLAGS) -o $(TEST_EXE)

# Run tests
run: $(TEST_EXE)
	./$(TEST_EXE)

# Clean
clean:
	rm -f $(TEST_EXE)

.PHONY: run clean
