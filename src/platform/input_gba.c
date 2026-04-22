/*
 * input_gba.c — GBA native input + platform policy.
 *
 * Mirrors the design of input_vita.c:
 *   - CK_PlatformPollInputState reads REG_KEYINPUT into a platform mask.
 *   - CK_PlatformApplyInputPolicy pushes the mask into the engine's
 *     emulated joystick and key_map state, without going through SDL.
 *
 * Button mapping (GBA → Commander Keen):
 *   D-Pad   → movement
 *   A       → jump (virtual Enter in menus)
 *   B       → pogo / back (virtual Esc in menus)
 *   L       → fire raygun
 *   R       → status / pause screen
 *   Start   → confirm (virtual Enter)
 *   Select  → modifier: Select+B=quit, Select+A=god mode, etc. (unused today)
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform/platform.h"
#include "platform/gba_sdl.h"

#include "core/globals.h"
#include "input/input.h"

#include <gba_base.h>
#include <gba_input.h>

/* DOS scancodes the engine expects when translating "virtual" key presses. */
#define GBA_DOS_SCANCODE_ESC    1
#define GBA_DOS_SCANCODE_ENTER  0x1C
#define GBA_DOS_SCANCODE_SPACE  0x39
#define GBA_DOS_SCANCODE_Y      0x15
#define GBA_DOS_SCANCODE_N      0x31
#define GBA_DOS_SCANCODE_T      0x14
#define GBA_DOS_SCANCODE_D      0x20

/* Mask used on the emulated joystick for jump / pogo. Must not collide
 * with any bit the engine already uses; bits 0..3 are axis hats, so we
 * start at 0x100 / 0x200 for safety. */
#define GBA_JOY_BUTTON_JUMP  0x0001
#define GBA_JOY_BUTTON_POGO  0x0002

/* Press-edge tracking so we only feed a scancode event on key-down, not
 * every frame it stays held. */
static bool s_wasEnterPressed = false;
static bool s_wasEscPressed   = false;
static bool s_wasStatusPressed = false;
static bool s_wasYPressed     = false;
static bool s_wasNPressed     = false;

static void ck_set_virtual_key(uint8_t dosScanCode, bool isPressed, bool *wasPressed) {
    if (!dosScanCode) { *wasPressed = isPressed; return; }
    if (isPressed) {
        /* Edge detection: fresh press OR the engine just called
         * CVort_clear_keys while the button was still held. The latter
         * happens on transitions like "Please press a key" — the engine
         * zeroes key_map before waiting for input, and if the user was
         * already holding a button, a pure press-edge check would never
         * re-arm the scancode and translate_key(0) would hang. Treat a
         * cleared key_map[scancode] as an implicit edge. */
        const bool isEdge = !(*wasPressed) || !g_input.key_map[dosScanCode];
        g_input.key_map[dosScanCode] = 1;
        if (isEdge) {
            g_input.key_scane = dosScanCode | 0x80;
            engine_lastScanCode = dosScanCode;
            engine_lastKeyTime = SDL_GetTicks();
            engine_isBeforeKeyDelay = true;
        }
    } else if (*wasPressed) {
        g_input.key_map[dosScanCode] = 0;
        if (engine_lastScanCode == dosScanCode) engine_lastScanCode = 0;
    }
    *wasPressed = isPressed;
}

void CK_PlatformPrepareInput(void) {
    /* libgba wants scanKeys called periodically so keysDown/keysHeld work,
     * but we read REG_KEYINPUT directly in PollInputState so there's
     * nothing to configure here. */
}

bool CK_PlatformPollInputState(CK_PlatformInputState_T *state) {
    if (!state) return false;
    /* KEYINPUT is active-low: bit clear = pressed. */
    uint16_t raw = (uint16_t)(~REG_KEYINPUT) & 0x03FF;
    uint32_t mask = 0;
    if (raw & KEY_UP)     mask |= CK_PLATFORM_BTN_DPAD_UP;
    if (raw & KEY_DOWN)   mask |= CK_PLATFORM_BTN_DPAD_DOWN;
    if (raw & KEY_LEFT)   mask |= CK_PLATFORM_BTN_DPAD_LEFT;
    if (raw & KEY_RIGHT)  mask |= CK_PLATFORM_BTN_DPAD_RIGHT;
    if (raw & KEY_A)      mask |= CK_PLATFORM_BTN_FACE_BOTTOM;
    if (raw & KEY_B)      mask |= CK_PLATFORM_BTN_FACE_RIGHT;
    if (raw & KEY_L)      mask |= CK_PLATFORM_BTN_SHOULDER_L;
    if (raw & KEY_R)      mask |= CK_PLATFORM_BTN_SHOULDER_R;
    if (raw & KEY_START)  mask |= CK_PLATFORM_BTN_MENU_CONFIRM;
    if (raw & KEY_SELECT) mask |= CK_PLATFORM_BTN_MENU_BACK;
    state->buttonsMask = mask;
    state->leftX = 128;
    state->leftY = 128;
    state->valid = true;
    return true;
}

void CK_PlatformApplyInputPolicy(const CK_PlatformInputState_T *state, bool isWaitingForCharInput) {
    if (!state || !state->valid) return;

    const bool dLeft  = (state->buttonsMask & CK_PLATFORM_BTN_DPAD_LEFT)  != 0;
    const bool dRight = (state->buttonsMask & CK_PLATFORM_BTN_DPAD_RIGHT) != 0;
    const bool dUp    = (state->buttonsMask & CK_PLATFORM_BTN_DPAD_UP)    != 0;
    const bool dDown  = (state->buttonsMask & CK_PLATFORM_BTN_DPAD_DOWN)  != 0;

    /* Map D-Pad onto the emulated joystick axis "polls" (for the
     * CONTROL_JOY_1 path in gameplay). 0/250/500 are the low/center/high
     * bucket values used by the original DOS driver. */
    int16_t xPoll = 250, yPoll = 250;
    if (dLeft)  xPoll = 0;
    if (dRight) xPoll = 500;
    if (dUp)    yPoll = 0;
    if (dDown)  yPoll = 500;
    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[0] = xPoll;
    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[1] = yPoll;

    /* NOTE: We deliberately do NOT mirror the D-Pad onto sc_dir[] DOS
     * scancodes. CVort_draw_title's joystick branch treats any translated
     * non-Esc key press as "confirm", so an armed arrow scancode would
     * fire menu selection on every D-Pad tap. getJoystickCtrl reads
     * joystickAxesPolls directly, and CK_ActionState_MOVE_* are unused
     * by the engine, so the scancode mirror is dead weight here. */

    /* Jump = A, Pogo = B (only during gameplay, not menus). */
    uint32_t jbm = engine_inputMappings.currEmuInputStatus.joystickButtonsMask;
    jbm &= ~(GBA_JOY_BUTTON_JUMP | GBA_JOY_BUTTON_POGO);
    if (state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) jbm |= GBA_JOY_BUTTON_JUMP;
    if ((state->buttonsMask & CK_PLATFORM_BTN_FACE_RIGHT) && !isWaitingForCharInput) {
        jbm |= GBA_JOY_BUTTON_POGO;
    }
    engine_inputMappings.currEmuInputStatus.joystickButtonsMask = jbm;

    /* sc_but1/sc_but2 are intentionally NOT armed for the same reason as
     * sc_dir[] above: getJoystickCtrl reads joystickButtonsMask, and
     * CK_ACTION_JUMP/POGO already OR in the joystick mask. Arming the DOS
     * Ctrl/Alt scancodes here would also feed the title-menu "any key =
     * confirm" path. */

    /* Virtual keys — menu navigation.
     * START→ENTER / SELECT→ESC cover generic confirm/back. A→Y and B→N
     * cover dialogs that ask "(Y/N)" or "(D)os or (T)itle" explicitly
     * (toupper of 'y'/'n' is 'Y'/'N'). In the title-menu joystick
     * branch, any nonzero trans_map result reads as confirm, so A/B
     * still act as "pick the highlighted item" there. */
    const bool confirm = (state->buttonsMask & CK_PLATFORM_BTN_MENU_CONFIRM) != 0;
    const bool back    = (state->buttonsMask & CK_PLATFORM_BTN_MENU_BACK)   != 0;
    const bool yes     = (state->buttonsMask & CK_PLATFORM_BTN_FACE_BOTTOM) != 0;
    const bool no      = (state->buttonsMask & CK_PLATFORM_BTN_FACE_RIGHT)  != 0;
    const bool status  = (state->buttonsMask & CK_PLATFORM_BTN_SHOULDER_R) ||
                         (state->buttonsMask & CK_PLATFORM_BTN_FACE_TOP);
    ck_set_virtual_key(GBA_DOS_SCANCODE_ENTER, confirm, &s_wasEnterPressed);
    ck_set_virtual_key(GBA_DOS_SCANCODE_ESC,   back,    &s_wasEscPressed);
    ck_set_virtual_key(GBA_DOS_SCANCODE_Y,     yes,     &s_wasYPressed);
    ck_set_virtual_key(GBA_DOS_SCANCODE_N,     no,      &s_wasNPressed);
    ck_set_virtual_key(GBA_DOS_SCANCODE_SPACE, status,  &s_wasStatusPressed);
}

uint16_t CK_PlatformPreferredPlayer1ControlType(uint16_t currentControlType, int numOfJoysticks) {
    (void)numOfJoysticks;
    /* GBA always acts as "a joystick" as far as the engine is concerned. */
    return CONTROL_JOY_1;
}

void CK_PlatformApplyDefaultJoystickCalibration(int16_t joystickCtrl[4][3]) {
    joystickCtrl[0][1] = 180;
    joystickCtrl[1][1] = 320;
    joystickCtrl[2][1] = 180;
    joystickCtrl[3][1] = 320;
}
