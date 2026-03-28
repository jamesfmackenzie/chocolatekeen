// platform.h: Declares platform interfaces for the platform subsystem.

#ifndef _CHOCOLATE_KEEN_PLATFORM_
#define _CHOCOLATE_KEEN_PLATFORM_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t buttonsMask;
    uint8_t leftX;
    uint8_t leftY;
    bool valid;
} CK_PlatformInputState_T;

enum {
    CK_PLATFORM_BTN_DPAD_UP      = 1u << 0,
    CK_PLATFORM_BTN_DPAD_DOWN    = 1u << 1,
    CK_PLATFORM_BTN_DPAD_LEFT    = 1u << 2,
    CK_PLATFORM_BTN_DPAD_RIGHT   = 1u << 3,
    CK_PLATFORM_BTN_FACE_BOTTOM  = 1u << 4,
    CK_PLATFORM_BTN_FACE_RIGHT   = 1u << 5,
    CK_PLATFORM_BTN_FACE_LEFT    = 1u << 6,
    CK_PLATFORM_BTN_FACE_TOP     = 1u << 7,
    CK_PLATFORM_BTN_SHOULDER_L   = 1u << 8,
    CK_PLATFORM_BTN_SHOULDER_R   = 1u << 9,
    CK_PLATFORM_BTN_MENU_BACK    = 1u << 10,
    CK_PLATFORM_BTN_MENU_CONFIRM = 1u << 11,
};

void CK_PlatformSleepMs(unsigned int ms);
void CK_PlatformPrepareInput(void);
bool CK_PlatformPollInputState(CK_PlatformInputState_T *state);
void CK_PlatformApplyInputPolicy(const CK_PlatformInputState_T *state, bool isWaitingForCharInput);
uint16_t CK_PlatformPreferredPlayer1ControlType(uint16_t currentControlType, int numOfJoysticks);
void CK_PlatformApplyDefaultJoystickCalibration(int16_t joystickCtrl[4][3]);
void CK_PlatformEnsureRwMiscDir(void);
bool CK_PlatformBuildRoDataPath(const char *relativePath, char *outPath, size_t outPathSize);
bool CK_PlatformBuildExternalGameDataPath(const char *relativePath, char *outPath, size_t outPathSize);
bool CK_PlatformBuildRwMiscPath(const char *filename, char *outPath, size_t outPathSize);
bool CK_PlatformShouldAutoShowLauncher(int availableEpisodes);

#endif
