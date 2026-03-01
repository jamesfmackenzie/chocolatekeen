DEBUG ?= 0
BUILDASCPP ?= 0
BINPREFIX ?=
ifeq ($(origin CC), default)
CC := gcc
endif

STRIPBIN ?= $(BINPREFIX)strip
WINDRES ?= windres
SDLCONFIG ?= sdl2-config
SRC ?= ../../src
RSRC ?= ../../rsrc
OBJ ?= obj

USE_OPENGL ?= 1
SINGLE_EPISODE ?= 0
BUILD_PROFILE ?= native
