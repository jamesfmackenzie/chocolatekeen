// actions.h: Declares input action interfaces for the input subsystem.

#ifndef _CHOCOLATE_KEEN_INPUT_ACTIONS_
#define _CHOCOLATE_KEEN_INPUT_ACTIONS_

#include <stdbool.h>
#include <stdint.h>

typedef enum CK_Action {
    CK_ACTION_MOVE_UP = 0,
    CK_ACTION_MOVE_DOWN,
    CK_ACTION_MOVE_LEFT,
    CK_ACTION_MOVE_RIGHT,
    CK_ACTION_JUMP,
    CK_ACTION_POGO,
    CK_ACTION_FIRE,
    CK_ACTION_STATUS,
    CK_ACTION_MENU_CONFIRM,
    CK_ACTION_MENU_BACK,
    CK_ACTION_PROMPT_YES,
    CK_ACTION_PROMPT_NO,
    CK_ACTION_F1,
    CK_ACTION_F2,
    CK_ACTION_F3,
    CK_ACTION_F4,
    CK_ACTION_F5,
    CK_ACTION_F6,
    CK_ACTION_F7,
    CK_ACTION_F8,
    CK_ACTION_F9,
    CK_ACTION_F10,
    CK_ACTION_F11,
    CK_ACTION_F12,
    CK_ACTION_COUNT
} CK_Action;

typedef struct CK_ActionFrameState_T {
    uint8_t held[CK_ACTION_COUNT];
    uint8_t pressed[CK_ACTION_COUNT];
    uint8_t released[CK_ACTION_COUNT];
} CK_ActionFrameState_T;

void CK_ActionState_Init(void);
void CK_ActionState_BeginFrame(void);
void CK_ActionState_UpdateFromLegacyInput(void);
const CK_ActionFrameState_T *CK_ActionState_Get(void);
bool CK_Action_IsHeld(CK_Action action);
bool CK_Action_WasPressed(CK_Action action);
bool CK_Action_WasReleased(CK_Action action);

#endif
