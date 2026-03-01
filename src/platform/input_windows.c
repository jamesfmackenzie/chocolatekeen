#include "platform/platform.h"

#include "windows.h"

void CK_PlatformPrepareInput(void) {
    LoadKeyboardLayout("00000409", KLF_ACTIVATE | KLF_SETFORPROCESS);
}
