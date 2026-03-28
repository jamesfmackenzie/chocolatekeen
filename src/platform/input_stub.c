// input_stub.c: Implements input stub for the platform subsystem.

#include "platform/platform.h"

#include <stdio.h>

void CK_PlatformPrepareInput(void) {
}

bool CK_PlatformPollInputState(CK_PlatformInputState_T *state) {
    (void)state;
    return false;
}

void CK_PlatformApplyInputPolicy(const CK_PlatformInputState_T *state, bool isWaitingForCharInput) {
    (void)state;
    (void)isWaitingForCharInput;
}

uint16_t CK_PlatformPreferredPlayer1ControlType(uint16_t currentControlType, int numOfJoysticks) {
    (void)numOfJoysticks;
    return currentControlType;
}

void CK_PlatformApplyDefaultJoystickCalibration(int16_t joystickCtrl[4][3]) {
    (void)joystickCtrl;
}

void CK_PlatformEnsureRwMiscDir(void) {
}

bool CK_PlatformBuildRoDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    int written = snprintf(outPath, outPathSize, "%s", relativePath);
    return (written > 0) && ((size_t)written < outPathSize);
}

bool CK_PlatformBuildExternalGameDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    (void)relativePath;
    (void)outPath;
    (void)outPathSize;
    return false;
}

bool CK_PlatformBuildRwMiscPath(const char *filename, char *outPath, size_t outPathSize) {
    int written = snprintf(outPath, outPathSize, "%s", filename);
    return (written > 0) && ((size_t)written < outPathSize);
}

bool CK_PlatformShouldAutoShowLauncher(int availableEpisodes) {
    (void)availableEpisodes;
    return false;
}
