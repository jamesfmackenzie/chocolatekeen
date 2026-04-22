include ../common_base.mk
include ../common_profiles.mk

PLATFORM_OBJECTS=$(OBJ)/platform_sleep_native.o \
	$(OBJ)/platform_input_stub.o $(OBJ)/macbundle.o

PLATFORM_SLEEP_OBJECT=$(OBJ)/platform_sleep_native.o
PLATFORM_SLEEP_SOURCE=$(SRC)/platform/sleep_native.c
PLATFORM_INPUT_OBJECT=$(OBJ)/platform_input_stub.o
PLATFORM_INPUT_SOURCE=$(SRC)/platform/input_stub.c

include ../common_objects.mk

OBJECTS=$(COMMON_OBJECTS) $(EPISODE_OBJECTS) $(PLATFORM_OBJECTS)
include ../common_flags.mk

include ../common_toolchain.mk

INTCXXFLAGS += -mmacosx-version-min=10.10 -arch arm64 -arch x86_64
INTLDFLAGS += -mmacosx-version-min=10.10 -arch arm64 -arch x86_64

.PHONY: all game clean veryclean test

all: game

game: $(EXE_PATH) chocolate-keen.app

PRELOAD_DATA_DIR=data
PRELOAD_REQUIRE_EXISTING_DIR=0
include ../common_preload.mk

$(EXE_PATH): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(INTLDFLAGS) -o $@ $(PRELOAD_FILES) -framework Foundation
#ifeq ($(DEBUG),0)
#	$(STRIPBIN) $(EXE_PATH)
#endif

$(EXE_PATH): $(OBJECTS)

include ../common_rules.mk

CLEAN_DIRS=$(OBJ) GAMEDATA
CLEAN_FILES=chocolate-keen
include ../common_clean.mk

chocolate-keen.app: $(EXE_PATH)
	mkdir -p $@/Contents/MacOS
	mkdir -p $@/Contents/Resources
	mkdir -p $@/Contents/Frameworks
	cp -Rf /usr/local/lib/libSDL2-2.0.0.dylib $@/Contents/Frameworks
	cp -f ../../rsrc/Info.plist $@/Contents
	cp -f ../../rsrc/chocolate-keen.icns $@/Contents/Resources
	cp -f $(EXE_PATH) $@/Contents/MacOS
	cp -Rf ../../data/* $@/Contents/Resources
	install_name_tool -add_rpath @executable_path/../Frameworks $@/Contents/MacOS/$(EXE_PATH)

# Run tests
test:
	$(MAKE) -C ../../tests run
