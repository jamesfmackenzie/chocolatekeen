DEBUG=0
BUILDASCPP=0
BINPREFIX=

ifeq ($(BUILDASCPP), 1)
	CXX=$(BINPREFIX)g++
else
	CXX=$(BINPREFIX)gcc
endif

STRIPBIN=$(BINPREFIX)strip
WINDRES=$(BINPREFIX)windres
SDLCONFIG=sdl2-config
SRC=src
RSRC=rsrc
OBJ=obj

USE_OPENGL=1

SINGLE_EPISODE=0

OBJECTS=$(OBJ)/cvorticons.o \
        $(OBJ)/cvorticons_memdata.o \
        $(OBJ)/cvorticons_engine.o \
        $(OBJ)/cvorticons_gfx.o \
        $(OBJ)/cvorticons_input.o \
        $(OBJ)/cvorticons_gui.o \
	$(OBJ)/lz.o \
	$(OBJ)/CRLE.o \
	$(OBJ)/Cunlzexe.o \
	$(OBJ)/imageRLE.o \
	$(OBJ)/chocolate-keen.o

ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 1))
	OBJECTS+= $(OBJ)/cvorticons1.o \
	          $(OBJ)/cvorticons1_memdata.o \
	          $(OBJ)/cvorticons1_engine.o
endif
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 2))
	OBJECTS+= $(OBJ)/cvorticons2.o \
	          $(OBJ)/cvorticons2_memdata.o \
	          $(OBJ)/cvorticons2_engine.o
endif
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 3))
	OBJECTS+= $(OBJ)/cvorticons3.o \
	          $(OBJ)/cvorticons3_memdata.o \
	          $(OBJ)/cvorticons3_engine.o
endif

INTCXXFLAGS=

ifeq ($(DEBUG),1)
	INTCXXFLAGS+= -g -DCHOCOLATE_KEEN_CONFIG_DEBUG
else
	INTCXXFLAGS+= -O2
endif

INTCXXFLAGS+= -I$(SRC) `$(SDLCONFIG) --cflags`
INTLDFLAGS=`$(SDLCONFIG) --libs`

ifeq ($(BUILDASCPP), 0)
	INTCXXFLAGS+= -std=c99
endif

ifeq ($(SINGLE_EPISODE),0)
	INTCXXFLAGS+= -DCHOCOLATE_KEEN_IS_EPISODE1_ENABLED -DCHOCOLATE_KEEN_IS_EPISODE2_ENABLED -DCHOCOLATE_KEEN_IS_EPISODE3_ENABLED
else
	INTCXXFLAGS+= -DCHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE=$(SINGLE_EPISODE) -DCHOCOLATE_KEEN_IS_EPISODE$(SINGLE_EPISODE)_ENABLED
endif

ifeq ($(USE_OPENGL), 1)
	INTCXXFLAGS+= -D_CHOCOLATE_KEEN_ENABLE_OPENGL_
#	ifeq ($(PLATFORM), WINDOWS)
#		INTLDFLAGS+= -lopengl32
#	else
#		INTLDFLAGS+= -lGL
#	endif
endif

ifeq ($(PLATFORM), WINDOWS)
	OBJECTS+= $(OBJ)/chocolate-keen_icon.o
	EXE_EXT=.exe
endif

ifeq ($(SINGLE_EPISODE),0)
	EXE_PATH=chocolate-keen$(EXE_EXT)
else
	EXE_PATH=chocolate-keen$(SINGLE_EPISODE)$(EXE_EXT)
endif

.PHONY: all game clean veryclean

all: game

game: $(EXE_PATH)

$(EXE_PATH): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(INTLDFLAGS) -o $@
ifeq ($(DEBUG),0)
	$(STRIPBIN) $(EXE_PATH)
endif

$(EXE_PATH): $(OBJECTS)

$(OBJ)/chocolate-keen_icon.o: $(RSRC)/chocolate-keen.rc
	$(WINDRES) $< -O coff -o $@

$(OBJ)/lz.o: $(SRC)/decompression/CGenius/fileio/lz.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/CRLE.o: $(SRC)/decompression/CGenius/fileio/compression/CRLE.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/Cunlzexe.o: $(SRC)/decompression/CGenius/fileio/compression/Cunlzexe.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/imageRLE.o: $(SRC)/decompression/imageRLE.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/%.o: $(SRC)/%.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
veryclean:
	-rm -f $(OBJ)/* chocolate-keen$(EXE_EXT) chocolate-keen1$(EXE_EXT) chocolate-keen2$(EXE_EXT) chocolate-keen3$(EXE_EXT)
clean:
	-rm -f $(EXE_PATH) $(OBJECTS)
