// gui_loop_common.c: Implements gui loop common for the ui subsystem.

#include "ui/gui_loop_common.h"

#include "core/globals.h"
#include "platform/platform.h"
#include "ui/gui_menu_loop.h"

void CVort_gui_resetNavButtonsMouseState(GUI_NavButtonsMouseState_T *state) {
	state->waitForMouseButtonRelease = false;
	state->isBackButtonMouseSelected = state->isPrevButtonMouseSelected = state->isNextButtonMouseSelected = false;
}

GUI_NavButton_T CVort_gui_beginNavButtonMouseSelection(
	GUI_NavButtonsMouseState_T *state,
	int x, int y,
	bool hasBackButton,
	bool hasPrevButton,
	bool hasNextButton
) {
	state->waitForMouseButtonRelease = true;
	if (hasBackButton && CVort_gui_isBackButtonSelectedByMouse(x, y)) {
		state->isBackButtonMouseSelected = true;
		return GUI_NAV_BUTTON_BACK;
	}
	if (hasPrevButton && CVort_gui_isPrevButtonSelectedByMouse(x, y)) {
		state->isPrevButtonMouseSelected = true;
		return GUI_NAV_BUTTON_PREV;
	}
	if (hasNextButton && CVort_gui_isNextButtonSelectedByMouse(x, y)) {
		state->isNextButtonMouseSelected = true;
		return GUI_NAV_BUTTON_NEXT;
	}
	return GUI_NAV_BUTTON_NONE;
}

GUI_NavButton_T CVort_gui_getNavButtonMouseReleaseAction(
	const GUI_NavButtonsMouseState_T *state,
	int x, int y
) {
	if (state->isBackButtonMouseSelected && CVort_gui_isBackButtonSelectedByMouse(x, y)) {
		return GUI_NAV_BUTTON_BACK;
	}
	if (state->isPrevButtonMouseSelected && CVort_gui_isPrevButtonSelectedByMouse(x, y)) {
		return GUI_NAV_BUTTON_PREV;
	}
	if (state->isNextButtonMouseSelected && CVort_gui_isNextButtonSelectedByMouse(x, y)) {
		return GUI_NAV_BUTTON_NEXT;
	}
	return GUI_NAV_BUTTON_NONE;
}

GUI_KeyNavAction_T CVort_gui_handleKeydownAsNavigation(const SDL_KeyboardEvent *keyEvent, const GUI_KeyNavHandlers_T *handlers) {
#if SDL_VERSION_ATLEAST(2,0,0)
	switch (keyEvent->keysym.scancode) {
		case SDL_SCANCODE_ESCAPE:
			if (handlers->back) handlers->back();
			return GUI_KEY_NAV_BACK;
		case SDL_SCANCODE_LEFT:
			if (handlers->left) handlers->left();
			return GUI_KEY_NAV_LEFT;
		case SDL_SCANCODE_RIGHT:
			if (handlers->right) handlers->right();
			return GUI_KEY_NAV_RIGHT;
		case SDL_SCANCODE_UP:
			if (handlers->up) handlers->up();
			return GUI_KEY_NAV_UP;
		case SDL_SCANCODE_DOWN:
			if (handlers->down) handlers->down();
			return GUI_KEY_NAV_DOWN;
		case SDL_SCANCODE_RETURN:
			if (handlers->activate) handlers->activate();
			return GUI_KEY_NAV_ACTIVATE;
		case SDL_SCANCODE_PAGEUP:
			if (handlers->prevPage) handlers->prevPage();
			return GUI_KEY_NAV_PREV_PAGE;
		case SDL_SCANCODE_PAGEDOWN:
			if (handlers->nextPage) handlers->nextPage();
			return GUI_KEY_NAV_NEXT_PAGE;
		default:
			return GUI_KEY_NAV_NONE;
	}
#else
	switch (keyEvent->keysym.sym) {
		case SDLK_ESCAPE:
			if (handlers->back) handlers->back();
			return GUI_KEY_NAV_BACK;
		case SDLK_LEFT:
			if (handlers->left) handlers->left();
			return GUI_KEY_NAV_LEFT;
		case SDLK_RIGHT:
			if (handlers->right) handlers->right();
			return GUI_KEY_NAV_RIGHT;
		case SDLK_UP:
			if (handlers->up) handlers->up();
			return GUI_KEY_NAV_UP;
		case SDLK_DOWN:
			if (handlers->down) handlers->down();
			return GUI_KEY_NAV_DOWN;
		case SDLK_RETURN:
			if (handlers->activate) handlers->activate();
			return GUI_KEY_NAV_ACTIVATE;
		case SDLK_PAGEUP:
			if (handlers->prevPage) handlers->prevPage();
			return GUI_KEY_NAV_PREV_PAGE;
		case SDLK_PAGEDOWN:
			if (handlers->nextPage) handlers->nextPage();
			return GUI_KEY_NAV_NEXT_PAGE;
		default:
			return GUI_KEY_NAV_NONE;
	}
#endif
	return GUI_KEY_NAV_NONE;
}

void CVort_gui_transformMouseCoordinates(int *xPtr, int *yPtr) {
	(*xPtr) -= engine_screen.dims.borderedViewportRect.x;
	(*yPtr) -= engine_screen.dims.borderedViewportRect.y;
	*xPtr = (*xPtr) * ENGINE_GUI_WIDTH / engine_screen.dims.borderedViewportRect.w;
	*yPtr = (*yPtr) * ENGINE_GUI_HEIGHT / engine_screen.dims.borderedViewportRect.h;
}

void CVort_gui_updateDisplayAndSleep(void) {
	if (engine_isFrameReadyToDisplay || ((uint32_t)(SDL_GetTicks() - engine_lastDisplayUpdateTime) >= 100)) {
		CVort_engine_updateActualDisplay();
		engine_lastDisplayUpdateTime = SDL_GetTicks();
	}
	CK_PlatformSleepMs(1);
}
