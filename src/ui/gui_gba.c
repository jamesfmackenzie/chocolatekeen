/*
 * gui_gba.c — no-op launcher / config UI for the GBA port.
 *
 * The SDL-based launcher is skipped on the GBA (see
 * CK_PlatformShouldAutoShowLauncher in gba_paths.c). If something
 * does call CVort_gui_runLoop it returns immediately, dropping the
 * user straight into the game.
 */

#include "core/globals.h"

void CVort_gui_runLoop(void) {}
