#include "platform.h"

#include <emscripten.h>

void CK_PlatformSleepMs(unsigned int ms) {
    emscripten_sleep(ms);
}
