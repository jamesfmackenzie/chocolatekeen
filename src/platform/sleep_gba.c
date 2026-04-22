/*
 * sleep_gba.c — CK_PlatformSleepMs backing for the GBA.
 *
 * Short sleeps (the hot path: CK_PlatformSleepMs(1) inside the engine's
 * delayInGameTicks / shortSleep wait loops) busy-spin on SDL_GetTicks so
 * they actually last ~1 ms. VBlankIntrWait would halt ~16.7 ms per call,
 * which stretches each game frame well past its 6-PIT-tick budget (~41 ms)
 * and causes visible slow-motion gameplay.
 *
 * Longer sleeps still halt-to-VBlank so idle waits (e.g. long UI pauses)
 * don't drain the battery.
 */

#include "platform/platform.h"
#include "platform/gba_sdl.h"

#include <gba_systemcalls.h>

void CK_PlatformSleepMs(unsigned int ms) {
    if (ms == 0) return;
    Uint32 start = SDL_GetTicks();
    if (ms <= 4) {
        while ((SDL_GetTicks() - start) < (Uint32)ms) { /* spin */ }
        return;
    }
    while ((SDL_GetTicks() - start) < (Uint32)ms) VBlankIntrWait();
}
