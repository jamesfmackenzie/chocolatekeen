#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	chocolate-keen
BUILD		:=	build
SOURCES		:=	../../src
DATA		:=	data
INCLUDES	:=

SRC := $(CURDIR)/../$(SOURCES)


SINGLE_EPISODE=0
INTCXXFLAGS=

OBJECTS=cvorticons.o \
        cvorticons_memdata.o \
        cvorticons_engine.o \
        cvorticons_gfx.o \
        cvorticons_input.o \
        cvorticons_gui.o \
	lz.o \
	CRLE.o \
	Cunlzexe.o \
	imageRLE.o \
	chocolate-keen.o

ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 1))
	OBJECTS+= cvorticons1.o \
	          cvorticons1_memdata.o \
	          cvorticons1_engine.o
endif
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 2))
	OBJECTS+= cvorticons2.o \
	          cvorticons2_memdata.o \
	          cvorticons2_engine.o
endif
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 3))
	OBJECTS+= cvorticons3.o \
	          cvorticons3_memdata.o \
	          cvorticons3_engine.o
endif

ifeq ($(SINGLE_EPISODE),0)
	INTCXXFLAGS+= -DCHOCOLATE_KEEN_IS_EPISODE1_ENABLED -DCHOCOLATE_KEEN_IS_EPISODE2_ENABLED -DCHOCOLATE_KEEN_IS_EPISODE3_ENABLED
else
	INTCXXFLAGS+= -DCHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE=$(SINGLE_EPISODE) -DCHOCOLATE_KEEN_IS_EPISODE$(SINGLE_EPISODE)_ENABLED
endif



#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS	= -g -O2 -Wall $(MACHDEP) $(INCLUDE) `$(PREFIX)pkg-config --cflags sdl2` $(INTCXXFLAGS)
CXXFLAGS	=	$(CFLAGS)

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	`$(PREFIX)pkg-config --libs sdl2` -lwiiuse -lbte -logc -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export OFILES := $(OBJECTS) #$(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:= -L$(LIBOGC_LIB) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol

#---------------------------------------------------------------------------------
run:
	wiiload $(TARGET).dol


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

$(OFILES_SOURCES) : $(HFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .jpg extension
#---------------------------------------------------------------------------------
%.jpg.o	%_jpg.h :	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------


lz.o: $(SRC)/decompression/CGenius/fileio/lz.c
	$(CC) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
CRLE.o: $(SRC)/decompression/CGenius/fileio/compression/CRLE.c
	$(CC) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
Cunlzexe.o: $(SRC)/decompression/CGenius/fileio/compression/Cunlzexe.c
	$(CC) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
imageRLE.o: $(SRC)/decompression/imageRLE.c
	$(CC) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
