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
	$(OBJ)/platform_input_windows.o
PLATFORM_SLEEP_OBJECT=$(OBJ)/platform_sleep_native.o
PLATFORM_SLEEP_SOURCE=$(SRC)/platform/sleep_native.c
PLATFORM_INPUT_OBJECT=$(OBJ)/platform_input_windows.o
PLATFORM_INPUT_SOURCE=$(SRC)/platform/input_windows.c

include ../common_objects.mk

OBJECTS=$(COMMON_OBJECTS) $(EPISODE_OBJECTS) $(PLATFORM_OBJECTS)
include ../common_flags.mk

ifeq ($(CC), emcc)
	INTCXXFLAGS+= -s USE_SDL=2 -s WASM=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_FILE=data.binary -s ALLOW_MEMORY_GROWTH=1
	INTLDFLAGS=-s USE_SDL=2 -s WASM=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_FILE=data.binary -s ALLOW_MEMORY_GROWTH=1
else
	INTCXXFLAGS+= -I$(SRC) `$(SDLCONFIG) --cflags`
	INTLDFLAGS=`$(SDLCONFIG) --libs`
endif

.PHONY: all game clean veryclean

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
CLEAN_FILES=chocolate-keen.exe SDL2.dll *.cfg *.CK1 *.CK2 *.CK3
include ../common_clean.mk
