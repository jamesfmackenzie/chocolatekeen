#---------------------------------------------------------------------------------
# Chocolate Keen — Game Boy Advance port
#
# Build: run build_gba.sh in this directory, or simply `make` once your
# environment has DEVKITPRO (and DEVKITARM) exported. Output is
# chocolate-keen.gba, a fixed-up 32 MiB-limit GBA cartridge ROM.
#---------------------------------------------------------------------------------
.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error "DEVKITPRO is not set. export DEVKITPRO=/opt/devkitpro")
endif
ifeq ($(strip $(DEVKITARM)),)
DEVKITARM := $(DEVKITPRO)/devkitARM
export DEVKITARM
endif

include $(DEVKITARM)/gba_rules

#---------------------------------------------------------------------------------
# EPISODE selects which Commander Keen episode this ROM contains.
#   make EPISODE=1  (default)  -> Commander Keen 1
#   make EPISODE=2             -> Commander Keen 2
#   make EPISODE=3             -> Commander Keen 3
# Each episode's bake, build tree, and output file are separate so the
# three ROMs can coexist in this directory.
#---------------------------------------------------------------------------------
EPISODE     ?= 1
ifneq ($(filter-out 1 2 3,$(EPISODE)),)
$(error EPISODE must be 1, 2, or 3 (got '$(EPISODE)'))
endif

TARGET      := chocolate-keen-ep$(EPISODE)
BUILD       := obj-ep$(EPISODE)
TOPDIR      := $(CURDIR)
REPO        := $(abspath $(CURDIR)/../..)
SRC         := $(REPO)/src
RSRC        := $(REPO)/rsrc
DATA_IN     := $(REPO)/data
GEN_DIR     := $(CURDIR)/generated-ep$(EPISODE)
DATA_DIR    := $(DATA_IN)/GAMEDATA/KEEN$(EPISODE)

GAME_TITLE  := CHOCOLATE KEEN
GAME_CODE   := CKE$(EPISODE)
MAKER_CODE  := CK

#---------------------------------------------------------------------------------
# Code generation flags
#
# - Build the CPU-hot engine in ARM; most files stay in Thumb to save ROM.
# - Target episode 1 only to fit comfortably in a 32 MiB cart.
# - Force-include gba_sdl.h so SDL2 symbols resolve to GBA stubs.
#---------------------------------------------------------------------------------
ARCH    := -mthumb -mthumb-interwork
CFLAGS  := -g -Wall -Wno-unused-function -Wno-unused-variable \
           -Wno-unused-parameter -Wno-unused-but-set-variable \
           -Wno-sign-compare -Wno-missing-braces -Wno-parentheses \
           -Wno-switch -Wno-implicit-fallthrough -Wno-format \
           -ffunction-sections -fdata-sections \
           -mcpu=arm7tdmi -mtune=arm7tdmi \
           -Os $(ARCH) -std=gnu99 \
           -DCHOCOLATE_KEEN_TARGET_GBA=1 \
           -DCHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE=$(EPISODE) \
           -DCHOCOLATE_KEEN_IS_EPISODE$(EPISODE)_ENABLED=1 \
           -DENGINE_EGA_MEMORY_IN_BITS=0x10000 \
           -I$(SRC) -I$(CURDIR) -I$(GEN_DIR) \
           -I$(CURDIR)/include_shim \
           -I$(DEVKITPRO)/libgba/include \
           -include $(SRC)/platform/gba_sdl.h
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS   = -g $(ARCH) -Wl,-Map,$(TARGET).map -Wl,--gc-sections \
            -specs=gba.specs \
            -L$(DEVKITPRO)/libgba/lib

LIBS    := -lgba

#---------------------------------------------------------------------------------
# Source file set.
#
# Anything touching SDL heavily at runtime is replaced with a *_gba.c variant
# (gfx, audio, gui*). Everything else compiles against the SDL stub header.
#---------------------------------------------------------------------------------
C_SOURCES := \
    $(SRC)/game/gameplay.c \
    $(SRC)/game/worldmap.c \
    $(SRC)/game/menus.c \
    $(SRC)/game/ui.c \
    $(SRC)/game/physics.c \
    $(SRC)/game/enemies.c \
    $(SRC)/game/episode_constants.c \
    $(SRC)/render/memdata.c \
    $(SRC)/render/gfx_gba.c \
    $(SRC)/engine/engine.c \
    $(SRC)/engine/engine_audio_gba.c \
    $(SRC)/engine/engine_config.c \
    $(SRC)/engine/engine_io.c \
    $(SRC)/engine/engine_loader.c \
    $(SRC)/engine/engine_timing.c \
    $(SRC)/input/input.c \
    $(SRC)/input/actions.c \
    $(SRC)/input/input_keys.c \
    $(SRC)/input/input_names.c \
    $(SRC)/input/input_default_mappings.c \
    $(SRC)/ui/gui_gba.c \
    $(SRC)/third_party/cgenius/fileio/lz.c \
    $(SRC)/third_party/cgenius/fileio/compression/CRLE.c \
    $(SRC)/third_party/cgenius/fileio/compression/Cunlzexe.c \
    $(SRC)/decompression/imageRLE.c \
    $(SRC)/episodes/episode$(EPISODE).c \
    $(SRC)/episodes/episode$(EPISODE)_memdata.c \
    $(SRC)/episodes/episode$(EPISODE)_engine.c \
    $(SRC)/platform/gba_main.c \
    $(SRC)/platform/gba_sdl.c \
    $(SRC)/platform/sleep_gba.c \
    $(SRC)/platform/input_gba.c \
    $(SRC)/platform/gba_paths.c \
    $(SRC)/platform/gba_fileio.c \
    $(SRC)/platform/gba_sram.c \
    $(GEN_DIR)/gba_data_gen.c

# $(CURDIR) is under $(REPO), so $(REPO)/% matches generated files too.
# Filter generated paths out of the REPO bucket before doing the substitution.
REPO_SRC  := $(filter-out $(CURDIR)/%,$(filter $(REPO)/%,$(C_SOURCES)))
GEN_SRC   := $(filter $(CURDIR)/%,$(C_SOURCES))
OBJS := $(patsubst $(REPO)/%,$(BUILD)/%.o,$(REPO_SRC)) \
        $(patsubst $(CURDIR)/%,$(BUILD)/_gen_%.o,$(GEN_SRC))

DEPS := $(OBJS:.o=.d)

#---------------------------------------------------------------------------------
.PHONY: all clean rom data-gen
all: rom
rom: $(TARGET).gba

#---------------------------------------------------------------------------------
# Data bake — run scripts/bake_gba_data.sh to produce the incbin table.
#---------------------------------------------------------------------------------
$(GEN_DIR)/gba_data_gen.c: | $(GEN_DIR)
	@echo "Baking GAMEDATA (KEEN$(EPISODE)) -> $@"
	@bash $(REPO)/scripts/bake_gba_data.sh "$(DATA_DIR)" "$(GEN_DIR)"

$(GEN_DIR):
	@mkdir -p $@

data-gen: $(GEN_DIR)/gba_data_gen.c

#---------------------------------------------------------------------------------
# Compile rules. One pattern per search root so the object tree mirrors the
# source tree under $(BUILD).
#---------------------------------------------------------------------------------
$(BUILD)/%.c.o: $(REPO)/%.c | $(GEN_DIR)/gba_data_gen.c
	@mkdir -p $(dir $@)
	@echo "  CC $<"
	@$(CC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) -c $< -o $@

$(BUILD)/_gen_generated-ep$(EPISODE)/%.c.o: $(GEN_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC (gen) $<"
	@$(CC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) -c $< -o $@

#---------------------------------------------------------------------------------
# Link
#---------------------------------------------------------------------------------
$(TARGET).elf: $(OBJS)
	@echo "  LD $@"
	@$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

#---------------------------------------------------------------------------------
# gbafix comes with devkitARM and patches the cart header.
#---------------------------------------------------------------------------------
$(TARGET).gba: $(TARGET).elf
	@$(OBJCOPY) -O binary $< $@
	@gbafix $@ -t$(GAME_TITLE) -c$(GAME_CODE) -m$(MAKER_CODE) >/dev/null
	@echo "built -> $@ ($$(stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes)"

#---------------------------------------------------------------------------------
clean:
	@rm -rf obj obj-ep1 obj-ep2 obj-ep3 \
	        generated generated-ep1 generated-ep2 generated-ep3 \
	        chocolate-keen.elf chocolate-keen.gba chocolate-keen.map \
	        chocolate-keen-ep1.elf chocolate-keen-ep1.gba chocolate-keen-ep1.map \
	        chocolate-keen-ep2.elf chocolate-keen-ep2.gba chocolate-keen-ep2.map \
	        chocolate-keen-ep3.elf chocolate-keen-ep3.gba chocolate-keen-ep3.map

-include $(DEPS)
