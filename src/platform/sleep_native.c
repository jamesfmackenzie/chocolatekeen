// sleep_native.c: Implements sleep native for the platform subsystem.

#include "platform/platform.h"

#include "SDL.h"

void CK_PlatformSleepMs(unsigned int ms) {
    SDL_Delay(ms);
}
