include ../common_base.mk
ifeq ($(origin CC), file)
CC=arm-vita-eabi-gcc
endif
BUILD_PROFILE=vita
ifeq ($(origin USE_OPENGL), file)
USE_OPENGL=0
endif
include ../common_profiles.mk
ifeq ($(strip $(VITA_SDL_CFLAGS)),)
VITA_SDL_CFLAGS=-I$(VITASDK)/arm-vita-eabi/include/SDL2 -D_REENTRANT
endif
ifeq ($(strip $(VITA_SDL_LDFLAGS)),)
VITA_SDL_LDFLAGS=-L$(VITASDK)/arm-vita-eabi/lib -lSDL2
endif

PLATFORM_OBJECTS=$(OBJ)/platform_sleep_vita.o \
	$(OBJ)/platform_input_vita.o
PLATFORM_SLEEP_OBJECT=$(OBJ)/platform_sleep_vita.o
PLATFORM_SLEEP_SOURCE=$(SRC)/platform/sleep_vita.c
PLATFORM_INPUT_OBJECT=$(OBJ)/platform_input_vita.o
PLATFORM_INPUT_SOURCE=$(SRC)/platform/input_vita.c

include ../common_objects.mk

OBJECTS=$(COMMON_OBJECTS) $(EPISODE_OBJECTS) $(PLATFORM_OBJECTS)
include ../common_flags.mk

include ../common_toolchain.mk

VITA_TITLE ?= Chocolate Keen
VITA_TITLE_ID ?= CHKN00001
VITA_ICON ?= ../../assets/vita/icon0.png
VELF_PATH=chocolate-keen.velf
SELF_PATH=eboot.bin
VPK_PATH=chocolate-keen-vita.vpk
DIST_DIR=dist
DIST_ROOT=$(DIST_DIR)/vpkroot

.PHONY: all game vpk check-vita-tools clean veryclean

all: game

game: check-vita-tools | $(OBJ)
game: $(EXE_PATH)

vpk: check-vita-tools | $(OBJ)
vpk: $(VPK_PATH)

check-vita-tools:
	@test -n "$(VITASDK)" || (echo "VITASDK is not set. Source your VitaSDK environment (env.sh)." && exit 1)
	@command -v arm-vita-eabi-gcc >/dev/null 2>&1 || (echo "Missing tool: arm-vita-eabi-gcc" && exit 1)
	@command -v vita-elf-create >/dev/null 2>&1 || (echo "Missing tool: vita-elf-create" && exit 1)
	@command -v vita-make-fself >/dev/null 2>&1 || (echo "Missing tool: vita-make-fself" && exit 1)
	@command -v vita-mksfoex >/dev/null 2>&1 || (echo "Missing tool: vita-mksfoex" && exit 1)
	@command -v zip >/dev/null 2>&1 || (echo "Missing tool: zip" && exit 1)

$(OBJ):
	mkdir -p $(OBJ)

$(EXE_PATH): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(INTLDFLAGS) -o $@

$(EXE_PATH): $(OBJECTS)

$(VELF_PATH): $(EXE_PATH)
	vita-elf-create $< $@

$(SELF_PATH): $(VELF_PATH)
	vita-make-fself -s $< $@

$(VPK_PATH): $(SELF_PATH)
	rm -rf $(DIST_ROOT)
	mkdir -p $(DIST_ROOT)/sce_sys $(DIST_ROOT)/GAMEDATA
	cp $(SELF_PATH) $(DIST_ROOT)/eboot.bin
	vita-mksfoex -s TITLE_ID=$(VITA_TITLE_ID) "$(VITA_TITLE)" $(DIST_ROOT)/sce_sys/param.sfo
	if [ -f $(VITA_ICON) ]; then \
		cp $(VITA_ICON) $(DIST_ROOT)/sce_sys/icon0.png; \
	else \
		echo "Missing Vita icon: $(VITA_ICON)"; \
		echo "Add a 128x128 PNG at that path or override VITA_ICON=..."; \
		exit 1; \
	fi
	if [ -f ../linux/chocolate-keen.cfg ]; then \
		cp ../linux/chocolate-keen.cfg $(DIST_ROOT)/chocolate-keen.cfg; \
	else \
		printf "keyboard=0x0\nfullscreen=1\n" > $(DIST_ROOT)/chocolate-keen.cfg; \
	fi
	cp -r ../../data/GAMEDATA/* $(DIST_ROOT)/GAMEDATA/
	(cd $(DIST_ROOT) && zip -r ../$(VPK_PATH) .)
	mv $(DIST_DIR)/$(VPK_PATH) $(VPK_PATH)

include ../common_rules.mk

CLEAN_DIRS=$(OBJ) $(DIST_DIR)
CLEAN_FILES=chocolate-keen.elf chocolate-keen.velf eboot.bin chocolate-keen-vita.vpk
include ../common_clean.mk
