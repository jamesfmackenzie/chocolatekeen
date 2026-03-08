// actions.c: Implements input actions for the input subsystem.

#include "input/actions.h"

#include "core/globals.h"
#include "input/input_keys.h"
#include <string.h>

#define CHOCOLATE_KEEN_ARRAY_LEN(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))

static CK_ActionFrameState_T g_actionState;
static uint8_t g_prevHeld[CK_ACTION_COUNT];

static bool is_dos_scancode_pressed(uint8_t dosScanCode) {
    return (dosScanCode < CHOCOLATE_KEEN_ARRAY_LEN(g_input.key_map)) && (g_input.key_map[dosScanCode] != 0);
}

static uint8_t dos_scancode_for_emulated_key(EmulatedKeyIndex_T index) {
    if ((index < 0) || (index >= engine_emulatedKeysTableCount)) {
        return 0;
    }
    return engine_emulatedKeysTable[index].dosScanCode;
}

void CK_ActionState_Init(void) {
    memset(&g_actionState, 0, sizeof(g_actionState));
    memset(g_prevHeld, 0, sizeof(g_prevHeld));
}

void CK_ActionState_BeginFrame(void) {
    int loopVar;
    for (loopVar = 0; loopVar < CK_ACTION_COUNT; ++loopVar) {
        g_prevHeld[loopVar] = g_actionState.held[loopVar];
        g_actionState.held[loopVar] = 0;
        g_actionState.pressed[loopVar] = 0;
        g_actionState.released[loopVar] = 0;
    }
}

void CK_ActionState_UpdateFromLegacyInput(void) {
    bool currHeld[CK_ACTION_COUNT] = {0};
    int loopVar;

    currHeld[CK_ACTION_MOVE_UP] =
        is_dos_scancode_pressed(g_input.sc_dir[0]) ||
        is_dos_scancode_pressed(g_input.sc_dir[1]) ||
        is_dos_scancode_pressed(g_input.sc_dir[7]);
    currHeld[CK_ACTION_MOVE_RIGHT] =
        is_dos_scancode_pressed(g_input.sc_dir[2]) ||
        is_dos_scancode_pressed(g_input.sc_dir[1]) ||
        is_dos_scancode_pressed(g_input.sc_dir[3]);
    currHeld[CK_ACTION_MOVE_DOWN] =
        is_dos_scancode_pressed(g_input.sc_dir[4]) ||
        is_dos_scancode_pressed(g_input.sc_dir[3]) ||
        is_dos_scancode_pressed(g_input.sc_dir[5]);
    currHeld[CK_ACTION_MOVE_LEFT] =
        is_dos_scancode_pressed(g_input.sc_dir[6]) ||
        is_dos_scancode_pressed(g_input.sc_dir[5]) ||
        is_dos_scancode_pressed(g_input.sc_dir[7]);

    currHeld[CK_ACTION_JUMP] =
        is_dos_scancode_pressed(g_input.sc_but1) ||
        ((engine_inputMappings.currEmuInputStatus.joystickButtonsMask & (1 | 4)) != 0);
    currHeld[CK_ACTION_POGO] =
        is_dos_scancode_pressed(g_input.sc_but2) ||
        ((engine_inputMappings.currEmuInputStatus.joystickButtonsMask & (2 | 8)) != 0);
    currHeld[CK_ACTION_FIRE] = currHeld[CK_ACTION_JUMP] && currHeld[CK_ACTION_POGO];

    currHeld[CK_ACTION_MENU_BACK] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_ESC));
    currHeld[CK_ACTION_MENU_CONFIRM] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_ENTER));
    currHeld[CK_ACTION_STATUS] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_SPACE));
    currHeld[CK_ACTION_PROMPT_YES] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_Y));
    currHeld[CK_ACTION_PROMPT_NO] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_N));

    currHeld[CK_ACTION_F1] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F1));
    currHeld[CK_ACTION_F2] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F2));
    currHeld[CK_ACTION_F3] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F3));
    currHeld[CK_ACTION_F4] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F4));
    currHeld[CK_ACTION_F5] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F5));
    currHeld[CK_ACTION_F6] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F6));
    currHeld[CK_ACTION_F7] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F7));
    currHeld[CK_ACTION_F8] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F8));
    currHeld[CK_ACTION_F9] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F9));
    currHeld[CK_ACTION_F10] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F10));
    currHeld[CK_ACTION_F11] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F11));
    currHeld[CK_ACTION_F12] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_F12));
    currHeld[CK_ACTION_PAGE_UP] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_PAGEUP));
    currHeld[CK_ACTION_PAGE_DOWN] = is_dos_scancode_pressed(dos_scancode_for_emulated_key(EMULATEDKEYINDEX_PAGEDOWN));

    for (loopVar = 0; loopVar < CK_ACTION_COUNT; ++loopVar) {
        g_actionState.held[loopVar] = currHeld[loopVar] ? 1 : 0;
        g_actionState.pressed[loopVar] = (currHeld[loopVar] && !g_prevHeld[loopVar]) ? 1 : 0;
        g_actionState.released[loopVar] = (!currHeld[loopVar] && g_prevHeld[loopVar]) ? 1 : 0;
    }
}

const CK_ActionFrameState_T *CK_ActionState_Get(void) {
    return &g_actionState;
}

bool CK_Action_IsHeld(CK_Action action) {
    return (action >= 0) && (action < CK_ACTION_COUNT) && (g_actionState.held[action] != 0);
}

bool CK_Action_WasPressed(CK_Action action) {
    return (action >= 0) && (action < CK_ACTION_COUNT) && (g_actionState.pressed[action] != 0);
}

bool CK_Action_WasReleased(CK_Action action) {
    return (action >= 0) && (action < CK_ACTION_COUNT) && (g_actionState.released[action] != 0);
}
