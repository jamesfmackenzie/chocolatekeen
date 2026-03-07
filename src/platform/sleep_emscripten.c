// sleep_emscripten.c: Implements sleep emscripten for the platform subsystem.

#include "platform/platform.h"

#include <emscripten.h>

void CK_PlatformSleepMs(unsigned int ms) {
    emscripten_sleep(ms);
}
