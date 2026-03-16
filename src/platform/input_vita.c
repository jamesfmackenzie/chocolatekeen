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

typedef struct {
    bool isMenuContext;
    bool isWorldMapContext;
    bool isMainMenuContext;
} VitaInputContext_T;

typedef struct {
    uint32_t sceButtons;
    uint32_t platformMask;
} VitaButtonMaskMap_T;

static const VitaButtonMaskMap_T g_vitaButtonMaskMap[] = {
    {SCE_CTRL_UP, CK_PLATFORM_BTN_DPAD_UP},
    {SCE_CTRL_DOWN, CK_PLATFORM_BTN_DPAD_DOWN},
    {SCE_CTRL_LEFT, CK_PLATFORM_BTN_DPAD_LEFT},
    {SCE_CTRL_RIGHT, CK_PLATFORM_BTN_DPAD_RIGHT},
    {SCE_CTRL_CROSS, CK_PLATFORM_BTN_FACE_BOTTOM},
    {SCE_CTRL_CIRCLE, CK_PLATFORM_BTN_FACE_RIGHT},
    {SCE_CTRL_SQUARE, CK_PLATFORM_BTN_FACE_LEFT},
    {SCE_CTRL_TRIANGLE, CK_PLATFORM_BTN_FACE_TOP},
    {SCE_CTRL_LTRIGGER, CK_PLATFORM_BTN_SHOULDER_L},
    {SCE_CTRL_RTRIGGER, CK_PLATFORM_BTN_SHOULDER_R},
    {SCE_CTRL_START, CK_PLATFORM_BTN_MENU_CONFIRM},
};

#define CHOCOLATE_KEEN_ARRAY_LEN(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))

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

static VitaInputContext_T get_input_context(void) {
    VitaInputContext_T ctx;
    ctx.isMenuContext = (draw_func != NULL);
    ctx.isWorldMapContext = (g_game.on_world_map != 0);
    ctx.isMainMenuContext = (ctx.isMenuContext && !ctx.isWorldMapContext) ||
        (!ctx.isWorldMapContext && (g_game.current_level == 90));
    return ctx;
}

static int16_t resolve_x_poll(const CK_PlatformInputState_T *state) {
    int16_t xPoll = get_deadzone_poll_from_analog_u8(state->leftX);
    if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_LEFT) {
        xPoll = 0;
    } else if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_RIGHT) {
        xPoll = 500;
    }
    if ((state->buttonsMask & (CK_PLATFORM_BTN_DPAD_UP | CK_PLATFORM_BTN_DPAD_DOWN)) &&
        !(state->buttonsMask & (CK_PLATFORM_BTN_DPAD_LEFT | CK_PLATFORM_BTN_DPAD_RIGHT))) {
        xPoll = 250;
    }
    return xPoll;
}

static int16_t resolve_y_poll(const CK_PlatformInputState_T *state) {
    int16_t yPoll = get_deadzone_poll_from_analog_u8(state->leftY);
    if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_UP) {
        yPoll = 0;
    } else if (state->buttonsMask & CK_PLATFORM_BTN_DPAD_DOWN) {
        yPoll = 500;
    }
    if ((state->buttonsMask & (CK_PLATFORM_BTN_DPAD_LEFT | CK_PLATFORM_BTN_DPAD_RIGHT)) &&
        !(state->buttonsMask & (CK_PLATFORM_BTN_DPAD_UP | CK_PLATFORM_BTN_DPAD_DOWN))) {
        yPoll = 250;
    }
    return yPoll;
}

static void apply_joystick_axis_polls(const CK_PlatformInputState_T *state) {
    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[0] = resolve_x_poll(state);
    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[1] = resolve_y_poll(state);
}

static void apply_joystick_button_mask(const CK_PlatformInputState_T *state, const VitaInputContext_T *ctx) {
    engine_inputMappings.currEmuInputStatus.joystickButtonsMask &= ~(VITA_JOY_BUTTONMASK_JUMP | VITA_JOY_BUTTONMASK_POGO);

    if (state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) {
        engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= VITA_JOY_BUTTONMASK_JUMP;
    }
    if (!ctx->isMenuContext && !ctx->isMainMenuContext && !ctx->isWorldMapContext &&
        (state->buttonsMask & CK_PLATFORM_BTN_FACE_RIGHT)) {
        engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= VITA_JOY_BUTTONMASK_POGO;
    }
    if (!ctx->isMenuContext && !ctx->isMainMenuContext &&
        (state->buttonsMask & CK_PLATFORM_BTN_FACE_LEFT)) {
        engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= (VITA_JOY_BUTTONMASK_JUMP | VITA_JOY_BUTTONMASK_POGO);
    }
}

static void apply_virtual_key_policy(
    const CK_PlatformInputState_T *state,
    const VitaInputContext_T *ctx,
    bool isWaitingForCharInput
) {
    const bool shouldPressEnter =
        ((state->buttonsMask & CK_PLATFORM_BTN_MENU_CONFIRM) != 0) ||
        (!ctx->isMainMenuContext && ((state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0));
    const bool shouldPressStatus =
        !isWaitingForCharInput && !ctx->isMainMenuContext &&
        ((state->buttonsMask & CK_PLATFORM_BTN_FACE_TOP) != 0);
    const bool shouldPressEsc =
        (ctx->isMainMenuContext || ctx->isWorldMapContext) &&
        ((state->buttonsMask & CK_PLATFORM_BTN_FACE_RIGHT) != 0);
    const bool shouldPressY =
        isWaitingForCharInput && ctx->isMainMenuContext &&
        ((state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0);
    const bool shouldPressT =
        isWaitingForCharInput && ctx->isWorldMapContext &&
        ((state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0);
    const bool shouldPressD =
        isWaitingForCharInput && ctx->isWorldMapContext &&
        ((state->buttonsMask & CK_PLATFORM_BTN_FACE_LEFT) != 0);

    set_virtual_scancode_state(VITA_DOS_SCANCODE_ENTER, shouldPressEnter, &g_wasEnterPressed);
    set_virtual_scancode_state(VITA_DOS_SCANCODE_SPACE, shouldPressStatus, &g_wasStatusPressed);
    // Main-menu Circle should act as dismiss only (via key_map/CK_Action), not as
    // a translated "any key confirms" scan event in legacy menu code.
    set_virtual_scancode_state_ex(
        VITA_DOS_SCANCODE_ESC,
        shouldPressEsc,
        (!ctx->isMainMenuContext) || isWaitingForCharInput,
        &g_wasEscPressed
    );
    set_virtual_scancode_state(VITA_DOS_SCANCODE_Y, shouldPressY, &g_wasPromptYPressed);
    set_virtual_scancode_state(VITA_DOS_SCANCODE_T, shouldPressT, &g_wasPromptTPressed);
    set_virtual_scancode_state(VITA_DOS_SCANCODE_D, shouldPressD, &g_wasPromptDPressed);
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
    for (int loopVar = 0; loopVar < CHOCOLATE_KEEN_ARRAY_LEN(g_vitaButtonMaskMap); ++loopVar) {
        if (ctrl.buttons & g_vitaButtonMaskMap[loopVar].sceButtons) {
            state->buttonsMask |= g_vitaButtonMaskMap[loopVar].platformMask;
        }
    }

    state->leftX = ctrl.lx;
    state->leftY = ctrl.ly;
    state->valid = true;
    return true;
}

void CK_PlatformApplyInputPolicy(const CK_PlatformInputState_T *state, bool isWaitingForCharInput) {
    VitaInputContext_T ctx;

    if (!state || !state->valid) {
        return;
    }

    ctx = get_input_context();
    apply_joystick_axis_polls(state);
    apply_joystick_button_mask(state, &ctx);
    apply_virtual_key_policy(state, &ctx, isWaitingForCharInput);
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

bool CK_PlatformShouldAutoShowLauncher(int availableEpisodes) {
    return (availableEpisodes > 1);
}
