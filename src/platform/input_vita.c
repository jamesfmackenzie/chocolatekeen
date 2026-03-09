// input_vita.c: Implements input vita for the platform subsystem.

#include "platform/platform.h"
#include "platform/vita_input_map.h"

#include "SDL.h"
#include "core/globals.h"
#include "input/input.h"
#include <psp2/ctrl.h>
#include <stdio.h>
#include <sys/stat.h>

#define VITA_RO_DATA_PREFIX "app0:/"
#define VITA_RW_MISC_DIR "ux0:data/chocolatekeen"
#define VITA_RW_MISC_PREFIX "ux0:data/chocolatekeen/"

static int g_rwMiscDirPrepared = 0;
static bool g_wasEnterPressed = false;
static bool g_wasStatusPressed = false;
static bool g_wasEscPressed = false;
static bool g_wasPromptYPressed = false;
static bool g_wasPromptTPressed = false;
static bool g_wasPromptDPressed = false;

static int16_t get_poll_from_analog_u8(uint8_t analogVal) {
    return (int16_t)((analogVal * 500) / 255);
}

static int16_t get_deadzone_poll_from_analog_u8(uint8_t analogVal) {
    const int center = 128;
    const int deadzone = 32;
    int delta = (int)analogVal - center;
    if ((delta > -deadzone) && (delta < deadzone)) {
        return 250;
    }
    return get_poll_from_analog_u8(analogVal);
}

static void set_virtual_scancode_state_ex(
    uint8_t dosScanCode,
    bool isPressedNow,
    bool emitScanEvent,
    bool *wasPressed
) {
    if (isPressedNow) {
        g_input.key_map[dosScanCode] = 1;
        if (emitScanEvent && !(*wasPressed)) {
            g_input.key_scane = dosScanCode | 0x80;
            engine_lastScanCode = dosScanCode;
            engine_lastKeyTime = SDL_GetTicks();
            engine_isBeforeKeyDelay = true;
        }
    } else if (*wasPressed) {
        g_input.key_map[dosScanCode] = 0;
        if ((g_input.key_scane & 0x7F) == dosScanCode) {
            g_input.key_scane = 0;
        }
        if (engine_lastScanCode == dosScanCode) {
            engine_lastScanCode = 0;
        }
    }
    *wasPressed = isPressedNow;
}

static void set_virtual_scancode_state(uint8_t dosScanCode, bool isPressedNow, bool *wasPressed) {
    set_virtual_scancode_state_ex(dosScanCode, isPressedNow, true, wasPressed);
}

void CK_PlatformPrepareInput(void) {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
}

bool CK_PlatformPollInputState(CK_PlatformInputState_T *state) {
    SceCtrlData ctrl;
    int ret;

    if (!state) {
        return false;
    }

    ret = sceCtrlPeekBufferPositive(0, &ctrl, 1);
    if (ret <= 0) {
        state->valid = false;
        return false;
    }

    state->buttonsMask = 0;
    if (ctrl.buttons & SCE_CTRL_UP) {
        state->buttonsMask |= CK_PLATFORM_BTN_DPAD_UP;
    }
    if (ctrl.buttons & SCE_CTRL_DOWN) {
        state->buttonsMask |= CK_PLATFORM_BTN_DPAD_DOWN;
    }
    if (ctrl.buttons & SCE_CTRL_LEFT) {
        state->buttonsMask |= CK_PLATFORM_BTN_DPAD_LEFT;
    }
    if (ctrl.buttons & SCE_CTRL_RIGHT) {
        state->buttonsMask |= CK_PLATFORM_BTN_DPAD_RIGHT;
    }
    if (ctrl.buttons & SCE_CTRL_CROSS) {
        state->buttonsMask |= CK_PLATFORM_BTN_FACE_BOTTOM;
    }
    if (ctrl.buttons & SCE_CTRL_CIRCLE) {
        state->buttonsMask |= CK_PLATFORM_BTN_FACE_RIGHT;
    }
    if (ctrl.buttons & SCE_CTRL_SQUARE) {
        state->buttonsMask |= CK_PLATFORM_BTN_FACE_LEFT;
    }
    if (ctrl.buttons & SCE_CTRL_TRIANGLE) {
        state->buttonsMask |= CK_PLATFORM_BTN_FACE_TOP;
    }
    if (ctrl.buttons & SCE_CTRL_LTRIGGER) {
        state->buttonsMask |= CK_PLATFORM_BTN_SHOULDER_L;
    }
    if (ctrl.buttons & SCE_CTRL_RTRIGGER) {
        state->buttonsMask |= CK_PLATFORM_BTN_SHOULDER_R;
    }
    if (ctrl.buttons & SCE_CTRL_START) {
        state->buttonsMask |= CK_PLATFORM_BTN_MENU_CONFIRM;
    }

    state->leftX = ctrl.lx;
    state->leftY = ctrl.ly;
    state->valid = true;
    return true;
}

void CK_PlatformApplyInputPolicy(const CK_PlatformInputState_T *state, bool isWaitingForCharInput) {
    int16_t xPoll;
    int16_t yPoll;
    bool isMenuContext;
    bool isWorldMapContext;
    bool isMainMenuContext;

    if (!state || !state->valid) {
        return;
    }

    xPoll = get_deadzone_poll_from_analog_u8(state->leftX);
    yPoll = get_deadzone_poll_from_analog_u8(state->leftY);

    if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_LEFT) {
        xPoll = 0;
    } else if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_RIGHT) {
        xPoll = 500;
    }

    if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_UP) {
        yPoll = 0;
    } else if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_DOWN) {
        yPoll = 500;
    }

    if ((state->buttonsMask & (CK_PLATFORM_BTN_DPAD_UP | CK_PLATFORM_BTN_DPAD_DOWN)) &&
        !(state->buttonsMask & (CK_PLATFORM_BTN_DPAD_LEFT | CK_PLATFORM_BTN_DPAD_RIGHT))) {
        xPoll = 250;
    }
    if ((state->buttonsMask & (CK_PLATFORM_BTN_DPAD_LEFT | CK_PLATFORM_BTN_DPAD_RIGHT)) &&
        !(state->buttonsMask & (CK_PLATFORM_BTN_DPAD_UP | CK_PLATFORM_BTN_DPAD_DOWN))) {
        yPoll = 250;
    }

    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[0] = xPoll;
    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[1] = yPoll;

    isMenuContext = (draw_func != NULL);
    isWorldMapContext = (g_game.on_world_map != 0);
    isMainMenuContext = (isMenuContext && !isWorldMapContext) ||
        (!isWorldMapContext && (g_game.current_level == 90));

    engine_inputMappings.currEmuInputStatus.joystickButtonsMask &= ~(VITA_JOY_BUTTONMASK_JUMP | VITA_JOY_BUTTONMASK_POGO);
    if (state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) {
        engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= VITA_JOY_BUTTONMASK_JUMP;
    }
    if (!isMenuContext && !isMainMenuContext && !g_game.on_world_map &&
        (state->buttonsMask & CK_PLATFORM_BTN_FACE_RIGHT)) {
        engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= VITA_JOY_BUTTONMASK_POGO;
    }
    if (!isMenuContext && !isMainMenuContext &&
        (state->buttonsMask & CK_PLATFORM_BTN_FACE_LEFT)) {
        engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= (VITA_JOY_BUTTONMASK_JUMP | VITA_JOY_BUTTONMASK_POGO);
    }

    set_virtual_scancode_state(
        VITA_DOS_SCANCODE_ENTER,
        ((state->buttonsMask & CK_PLATFORM_BTN_MENU_CONFIRM) != 0) ||
            (!isMainMenuContext && ((state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0)),
        &g_wasEnterPressed
    );
    set_virtual_scancode_state(
        VITA_DOS_SCANCODE_SPACE,
        !isWaitingForCharInput && !isMainMenuContext &&
            ((state->buttonsMask & CK_PLATFORM_BTN_FACE_TOP) != 0),
        &g_wasStatusPressed
    );
    // Main-menu Circle should act as dismiss only (via key_map/CK_Action), not as
    // a translated "any key confirms" scan event in legacy menu code.
    set_virtual_scancode_state_ex(
        VITA_DOS_SCANCODE_ESC,
        (isMainMenuContext || isWorldMapContext) &&
            ((state->buttonsMask & CK_PLATFORM_BTN_FACE_RIGHT) != 0),
        (!isMainMenuContext) || isWaitingForCharInput,
        &g_wasEscPressed
    );
    set_virtual_scancode_state(
        VITA_DOS_SCANCODE_Y,
        isWaitingForCharInput && isMainMenuContext &&
            ((state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0),
        &g_wasPromptYPressed
    );
    set_virtual_scancode_state(
        VITA_DOS_SCANCODE_T,
        isWaitingForCharInput && isWorldMapContext &&
            ((state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0),
        &g_wasPromptTPressed
    );
    set_virtual_scancode_state(
        VITA_DOS_SCANCODE_D,
        isWaitingForCharInput && isWorldMapContext &&
            ((state->buttonsMask & CK_PLATFORM_BTN_FACE_LEFT) != 0),
        &g_wasPromptDPressed
    );
}

uint16_t CK_PlatformPreferredPlayer1ControlType(uint16_t currentControlType, int numOfJoysticks) {
    if (numOfJoysticks > 0) {
        return CONTROL_JOY_1;
    }
    return currentControlType;
}

void CK_PlatformApplyDefaultJoystickCalibration(int16_t joystickCtrl[4][3]) {
    joystickCtrl[0][1] = 180;
    joystickCtrl[1][1] = 320;
    joystickCtrl[2][1] = 180;
    joystickCtrl[3][1] = 320;
}

void CK_PlatformEnsureRwMiscDir(void) {
    if (g_rwMiscDirPrepared) {
        return;
    }
    (void)mkdir("ux0:data", 0777);
    (void)mkdir(VITA_RW_MISC_DIR, 0777);
    g_rwMiscDirPrepared = 1;
}

bool CK_PlatformBuildRoDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    int written = snprintf(outPath, outPathSize, "%s%s", VITA_RO_DATA_PREFIX, relativePath);
    return (written > 0) && ((size_t)written < outPathSize);
}

bool CK_PlatformBuildRwMiscPath(const char *filename, char *outPath, size_t outPathSize) {
    int written = snprintf(outPath, outPathSize, "%s%s", VITA_RW_MISC_PREFIX, filename);
    return (written > 0) && ((size_t)written < outPathSize);
}
