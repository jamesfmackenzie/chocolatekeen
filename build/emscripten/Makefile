include ../common_base.mk
BUILD_PROFILE=emscripten
ifeq ($(origin CC), file)
CC=emcc
endif
include ../common_profiles.mk

PLATFORM_OBJECTS=$(OBJ)/platform_sleep_emscripten.o \
	$(OBJ)/platform_input_stub.o
PLATFORM_SLEEP_OBJECT=$(OBJ)/platform_sleep_emscripten.o
PLATFORM_SLEEP_SOURCE=$(SRC)/platform/sleep_emscripten.c
PLATFORM_INPUT_OBJECT=$(OBJ)/platform_input_stub.o
PLATFORM_INPUT_SOURCE=$(SRC)/platform/input_stub.c

include ../common_objects.mk

OBJECTS=$(COMMON_OBJECTS) $(EPISODE_OBJECTS) $(PLATFORM_OBJECTS)
include ../common_flags.mk

include ../common_toolchain.mk

.PHONY: all game clean veryclean

all: game

game: $(EXE_PATH)

PRELOAD_DATA_DIR=data
PRELOAD_REQUIRE_EXISTING_DIR=1
include ../common_preload.mk

$(EXE_PATH): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(INTLDFLAGS) -o $@ $(PRELOAD_FILES)
#ifeq ($(DEBUG),0)
#	$(STRIPBIN) $(EXE_PATH)
#endif

$(EXE_PATH): $(OBJECTS)

include ../common_rules.mk

CLEAN_DIRS=$(OBJ)
CLEAN_FILES=chocolate*
include ../common_clean.mk
