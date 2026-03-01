#include "platform/platform.h"

#include "SDL.h"

void CK_PlatformSleepMs(unsigned int ms) {
    SDL_Delay(ms);
}
