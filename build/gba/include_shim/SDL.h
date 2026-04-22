/*
 * SDL.h shim for the GBA port. The Makefile force-includes
 * src/platform/gba_sdl.h before every translation unit, so by the time
 * source code does `#include "SDL.h"` all SDL types, constants and
 * function declarations it might look for are already visible. This
 * file exists only so the preprocessor's #include search succeeds.
 */

#ifndef CHOCOLATE_KEEN_GBA_SDL_SHIM_H
#define CHOCOLATE_KEEN_GBA_SDL_SHIM_H

#ifndef CHOCOLATE_KEEN_TARGET_GBA
#error "this SDL.h shim is GBA-only; do not pick it up on other builds"
#endif

/* Force-include provides the real declarations. */

#endif
