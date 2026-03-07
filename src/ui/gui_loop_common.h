// gui_loop_common.h: Declares gui loop common interfaces for the ui subsystem.

#ifndef _CHOCOLATE_KEEN_GUI_LOOP_COMMON_H_
#define _CHOCOLATE_KEEN_GUI_LOOP_COMMON_H_

#include <stdbool.h>
#include "SDL.h"

typedef enum {
	GUI_NAV_BUTTON_NONE,
	GUI_NAV_BUTTON_BACK,
	GUI_NAV_BUTTON_PREV,
	GUI_NAV_BUTTON_NEXT
} GUI_NavButton_T;

typedef struct {
	bool waitForMouseButtonRelease;
	bool isBackButtonMouseSelected, isPrevButtonMouseSelected, isNextButtonMouseSelected;
} GUI_NavButtonsMouseState_T;

typedef struct {
	void (*back)(void);
	void (*left)(void);
	void (*right)(void);
	void (*up)(void);
	void (*down)(void);
	void (*activate)(void);
	void (*prevPage)(void);
	void (*nextPage)(void);
} GUI_KeyNavHandlers_T;

typedef enum {
	GUI_KEY_NAV_NONE,
	GUI_KEY_NAV_BACK,
	GUI_KEY_NAV_LEFT,
	GUI_KEY_NAV_RIGHT,
	GUI_KEY_NAV_UP,
	GUI_KEY_NAV_DOWN,
	GUI_KEY_NAV_ACTIVATE,
	GUI_KEY_NAV_PREV_PAGE,
	GUI_KEY_NAV_NEXT_PAGE
} GUI_KeyNavAction_T;

void CVort_gui_resetNavButtonsMouseState(GUI_NavButtonsMouseState_T *state);
GUI_NavButton_T CVort_gui_beginNavButtonMouseSelection(
	GUI_NavButtonsMouseState_T *state,
	int x, int y,
	bool hasBackButton,
	bool hasPrevButton,
	bool hasNextButton
);
GUI_NavButton_T CVort_gui_getNavButtonMouseReleaseAction(
	const GUI_NavButtonsMouseState_T *state,
	int x, int y
);
GUI_KeyNavAction_T CVort_gui_handleKeydownAsNavigation(const SDL_KeyboardEvent *keyEvent, const GUI_KeyNavHandlers_T *handlers);

void CVort_gui_transformMouseCoordinates(int *xPtr, int *yPtr);
void CVort_gui_updateDisplayAndSleep(void);

#endif
