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

PLATFORM_OBJECTS=$(OBJ)/platform_sleep_emscripten.o \
	$(OBJ)/platform_input_stub.o
PLATFORM_SLEEP_OBJECT=$(OBJ)/platform_sleep_emscripten.o
PLATFORM_SLEEP_SOURCE=$(SRC)/platform/sleep_emscripten.c
PLATFORM_INPUT_OBJECT=$(OBJ)/platform_input_stub.o
PLATFORM_INPUT_SOURCE=$(SRC)/platform/input_stub.c

include ../common_objects.mk

OBJECTS=$(COMMON_OBJECTS) $(EPISODE_OBJECTS) $(PLATFORM_OBJECTS)
include ../common_flags.mk

ifeq ($(CC), emcc)
	INTCXXFLAGS+= -I$(SRC) -s USE_SDL=2
	INTLDFLAGS=-s USE_SDL=2 -s WASM=1 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1
else
	INTCXXFLAGS+= -I$(SRC) `$(SDLCONFIG) --cflags`
	INTLDFLAGS=`$(SDLCONFIG) --libs`
endif

.PHONY: all game clean veryclean

all: game

game: $(EXE_PATH)

PRELOAD_FILE=
ifeq ($(CC), emcc)
ifneq ($(wildcard data),)
	PRELOAD_FILES= --preload-file data@/
endif
endif

$(EXE_PATH): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(INTLDFLAGS) -o $@ $(PRELOAD_FILES)
#ifeq ($(DEBUG),0)
#	$(STRIPBIN) $(EXE_PATH)
#endif

$(EXE_PATH): $(OBJECTS)

include ../common_rules.mk

clean:
	-rm -rf $(OBJ)
	-rm -f chocolate*
