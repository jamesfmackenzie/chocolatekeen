DEBUG=0
BUILDASCPP=0
BINPREFIX=
CC=gcc

STRIPBIN=$(BINPREFIX)strip
WINDRES=windres
SDLCONFIG=sdl2-config
SRC=../../src
RSRC=../../rsrc
OBJ=obj

USE_OPENGL=1

SINGLE_EPISODE=0

PLATFORM_OBJECTS=$(OBJ)/platform_sleep_native.o \
	$(OBJ)/platform_input_stub.o
PLATFORM_SLEEP_OBJECT=$(OBJ)/platform_sleep_native.o
PLATFORM_SLEEP_SOURCE=$(SRC)/platform/sleep_native.c
PLATFORM_INPUT_OBJECT=$(OBJ)/platform_input_stub.o
PLATFORM_INPUT_SOURCE=$(SRC)/platform/input_stub.c

include ../common_objects.mk

OBJECTS=$(COMMON_OBJECTS) $(EPISODE_OBJECTS) $(PLATFORM_OBJECTS)
include ../common_flags.mk

EMCC_CFLAGS=-s USE_SDL=2 -s WASM=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_FILE=data.binary -s ALLOW_MEMORY_GROWTH=1
EMCC_LDFLAGS=-s USE_SDL=2 -s WASM=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_FILE=data.binary -s ALLOW_MEMORY_GROWTH=1
include ../common_toolchain.mk

.PHONY: all game clean veryclean test

all: game

game: $(EXE_PATH)

PRELOAD_FILE=
ifeq ($(CC), emcc)
	PRELOAD_FILES= --preload-file data@/
endif

$(EXE_PATH): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(INTLDFLAGS) -o $@ $(PRELOAD_FILES)
#ifeq ($(DEBUG),0)
#	$(STRIPBIN) $(EXE_PATH)
#endif

$(EXE_PATH): $(OBJECTS)

include ../common_rules.mk

CLEAN_DIRS=$(OBJ) GAMEDATA
CLEAN_FILES=chocolate-keen
include ../common_clean.mk

# Run tests
test:
	$(MAKE) -C ../../tests run
