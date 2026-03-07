#include <string.h>

#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "core/globals.h"
#include "ui/gui_internal.h"
#include "ui/gui_loop_common.h"
#include "ui/gui_menu_loop.h"
#include "ui/gui_runtime.h"

static GUI_NavButtonsMouseState_T guiCurrentMenuStatus;

/************************************************************
For each menu item:
- Have a total height of 34 pixels at the least.
- 14 pixels height for a row of text.
- 10 pixels of top invisible border, and 10 of bottom border.
************************************************************/
#define GUI_MENU_ITEM_TEXT_Y1_OFFSET 10
#define GUI_MENU_ITEM_TEXT_Y2_OFFSET 10
#define GUI_MENU_ITEM_TEXT_CHAR_WIDTH 8
#define GUI_MENU_ITEM_TEXT_CHAR_HEIGHT 14

typedef struct {
	int leftX0, leftX1;
	int rightX0, rightX1;
} GUI_MenuChoiceHitBounds_T;

static void CVort_gui_calcMenuChoiceHitBounds(const GUI_Menu_Item_T *item, GUI_MenuChoiceHitBounds_T *bounds) {
	bounds->leftX0 = item->x + (guiCurrentMenuPtr->width - item->choiceMaxPixWidth - 4 * GUI_MENU_ITEM_TEXT_CHAR_WIDTH);
	bounds->leftX1 = item->x + (guiCurrentMenuPtr->width - item->choiceMaxPixWidth / 2 - 2 * GUI_MENU_ITEM_TEXT_CHAR_WIDTH);
	bounds->rightX0 = bounds->leftX1;
	bounds->rightX1 = item->x + guiCurrentMenuPtr->width;
}

static bool CVort_gui_isMouseInChoiceLeftArrow(
	const GUI_MenuChoiceHitBounds_T *bounds,
	int pressX,
	int releaseX
) {
	return (pressX >= bounds->leftX0) && (pressX < bounds->leftX1) &&
	       (releaseX >= bounds->leftX0) && (releaseX < bounds->leftX1);
}

void CVort_gui_resetMenuStatus(void) {
	CVort_gui_resetNavButtonsMouseState(&guiCurrentMenuStatus);
}


// Assumption: item is a valid pointer to a menu item.
void CVort_gui_drawMenuItem(GUI_Menu_Item_T *item) {
	int labelColor = item->handler ? ((guiCurrentMenuItemSelectionPtr && (*guiCurrentMenuItemSelectionPtr == item)) ? 15 : 7) : 3;
	//CVort_engine_gui_drawColoredLine(item->y + item->height - 1, engine_screen.dims.clientRect.w, 7);
	// First draw menu item label
	const char *txtPtr = item->label;
	for (int x = item->x, y = item->text_y; *txtPtr; txtPtr++) {
		if (*txtPtr == '\n') {
			x = item->x;
			y+=GUI_MENU_ITEM_TEXT_CHAR_HEIGHT;
		} else {
			CVort_engine_gui_drawFontChar_ptr(x, y, 8, 14, ega_8x14TextFont+112*(unsigned char)(*txtPtr), labelColor, 1);
			x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH;
		}
	}
	// Next draw choice if it's relevant
	if (item->choices) {
		// FIXME: Accessing menu width directly
		// Draw double left arrow, followed by a space
		int x = item->x + (guiCurrentMenuPtr->width - item->choiceMaxPixWidth - 4*GUI_MENU_ITEM_TEXT_CHAR_WIDTH);
		CVort_engine_gui_drawFontChar_ptr(x, item->text_y, 8, 14, ega_8x14TextFont+112*174, 12, 1);
		x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH;
		CVort_engine_gui_drawFontChar_ptr(x, item->text_y, 8, 14, ega_8x14TextFont+112*32, 12, 1);
		x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH + (item->choiceMaxPixWidth-strlen(item->choices[item->value])*GUI_MENU_ITEM_TEXT_CHAR_WIDTH)/2;
		// Draw selection
		for (txtPtr = item->choices[item->value]; *txtPtr; txtPtr++, x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH) {
			CVort_engine_gui_drawFontChar_ptr(x, item->text_y, 8, 14, ega_8x14TextFont+112*(unsigned char)(*txtPtr), 14, 1);
		}
		// Draw double right arrow twice, preceded by a space
		x = item->x + (guiCurrentMenuPtr->width - 2*GUI_MENU_ITEM_TEXT_CHAR_WIDTH);
		CVort_engine_gui_drawFontChar_ptr(x, item->text_y, 8, 14, ega_8x14TextFont+112*32, 12, 1);
		x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH;
		CVort_engine_gui_drawFontChar_ptr(x, item->text_y, 8, 14, ega_8x14TextFont+112*175, 12, 1);
	}
}

// HACK/FIXME: Always using 4 chars for back/prev/next button
#define GUI_MENU_BACK_BUTTON_X 2
#define GUI_MENU_BACK_BUTTON_Y 8
#define GUI_MENU_BACK_BUTTON_WIDTH (4*GUI_MENU_ITEM_TEXT_CHAR_WIDTH + 4)
#define GUI_MENU_BACK_BUTTON_HEIGHT (GUI_MENU_ITEM_TEXT_CHAR_HEIGHT + 4)

#define GUI_MENU_PREV_BUTTON_X 2
#define GUI_MENU_PREV_BUTTON_WIDTH (4*GUI_MENU_ITEM_TEXT_CHAR_WIDTH + 4)
#define GUI_MENU_PREV_BUTTON_HEIGHT (GUI_MENU_ITEM_TEXT_CHAR_HEIGHT + 4)
#define GUI_MENU_PREV_BUTTON_Y (engine_screen.dims.clientRect.h-9-GUI_MENU_PREV_BUTTON_HEIGHT)

#define GUI_MENU_NEXT_BUTTON_WIDTH (4*GUI_MENU_ITEM_TEXT_CHAR_WIDTH + 4)
#define GUI_MENU_NEXT_BUTTON_HEIGHT (GUI_MENU_ITEM_TEXT_CHAR_HEIGHT + 4)
#define GUI_MENU_NEXT_BUTTON_X (engine_screen.dims.clientRect.w-2-GUI_MENU_NEXT_BUTTON_WIDTH)
#define GUI_MENU_NEXT_BUTTON_Y (engine_screen.dims.clientRect.h-9-GUI_MENU_NEXT_BUTTON_HEIGHT)

// ASSUMPTION: There is a back button
bool CVort_gui_isBackButtonSelectedByMouse(int x, int y) {
	return ((x >= GUI_MENU_BACK_BUTTON_X) && (x < GUI_MENU_BACK_BUTTON_X + GUI_MENU_BACK_BUTTON_WIDTH) &&
	        (y >= GUI_MENU_BACK_BUTTON_Y) && (y < GUI_MENU_BACK_BUTTON_Y + GUI_MENU_BACK_BUTTON_HEIGHT));
}

// ASSUMPTION: There is a prev button
bool CVort_gui_isPrevButtonSelectedByMouse(int x, int y) {
	return ((x >= GUI_MENU_PREV_BUTTON_X) && (x < GUI_MENU_PREV_BUTTON_X + GUI_MENU_PREV_BUTTON_WIDTH) &&
	        (y >= GUI_MENU_PREV_BUTTON_Y) && (y < GUI_MENU_PREV_BUTTON_Y + GUI_MENU_PREV_BUTTON_HEIGHT));
}
// ASSUMPTION: There is a next button
bool CVort_gui_isNextButtonSelectedByMouse(int x, int y) {
	return ((x >= GUI_MENU_NEXT_BUTTON_X) && (x < GUI_MENU_NEXT_BUTTON_X + GUI_MENU_NEXT_BUTTON_WIDTH) &&
	        (y >= GUI_MENU_NEXT_BUTTON_Y) && (y < GUI_MENU_NEXT_BUTTON_Y + GUI_MENU_NEXT_BUTTON_HEIGHT));
}

// Draws a back/next/prev button (assuming it should be drawn)
void CVort_gui_drawMenuNavigationButton(int x, int y, const char *label, bool isSelected) {
	size_t len = strlen(label);
	CVort_engine_gui_drawRoundedRectBorder_ptr(x, y, len*GUI_MENU_ITEM_TEXT_CHAR_WIDTH + 4, GUI_MENU_ITEM_TEXT_CHAR_HEIGHT+4, isSelected ? 12 : 7, isSelected ? 4 : 8, 1);
	x += 2; y += 2;
	for (; *label; x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH, label++) {
		CVort_engine_gui_drawFontChar_ptr(x, y, 8, 14, ega_8x14TextFont+112*(unsigned char)(*label), 15, 1);
	}
}

void CVort_gui_drawBackButton(bool isMouseSelected) {
	static const unsigned char backButtonStr[] = {32, 174, 174, 32, 0};
	CVort_gui_drawMenuNavigationButton(GUI_MENU_BACK_BUTTON_X, GUI_MENU_BACK_BUTTON_Y, (const char *)backButtonStr, isMouseSelected);
}

void CVort_gui_drawPrevButton(bool isMouseSelected) {
	static const char prevButtonStr[] = "Prev";
	CVort_gui_drawMenuNavigationButton(GUI_MENU_PREV_BUTTON_X, GUI_MENU_PREV_BUTTON_Y, prevButtonStr, isMouseSelected);
}

void CVort_gui_drawNextButton(bool isMouseSelected) {
	static const char nextButtonStr[] = "Next";
	CVort_gui_drawMenuNavigationButton(GUI_MENU_NEXT_BUTTON_X, GUI_MENU_NEXT_BUTTON_Y, nextButtonStr, isMouseSelected);
}

void CVort_gui_drawCurrentMenu(void) {
	// First draw menu title
	const char *txtPtr = guiCurrentMenuPtr->title;
	int x = guiCurrentMenuPtr->title_x;
	int y = guiCurrentMenuPtr->title_y;
	for (; *txtPtr; txtPtr++) {
		if (*txtPtr == '\n') {
			x = guiCurrentMenuPtr->title_x;
			y += GUI_MENU_ITEM_TEXT_CHAR_HEIGHT;
			continue;
		}
		CVort_engine_gui_drawFontChar_ptr(x, y, 8, 14, ega_8x14TextFont+112*(unsigned char)(*txtPtr), 11, 1);
		x += GUI_MENU_ITEM_TEXT_CHAR_WIDTH;
	}
	// Next draw items and item separators
	GUI_Menu_Item_T **items;
	for (items = guiCurrentMenuPtr->items; *items; items++) {
		CVort_engine_gui_drawColoredLine((*items)->y, engine_screen.dims.clientRect.w, 2);
		CVort_gui_drawMenuItem(*items);
	}
	// Add an extra separator below the bottommost item, if needed
	if ((*(items-1))->handler) {
		CVort_engine_gui_drawColoredLine((*(items-1))->y + (*(items-1))->height, engine_screen.dims.clientRect.w, 2);
	}
	// Further add horizontal and vertical bars surrounding the viewport
	CVort_engine_gui_drawColoredLine(0, engine_screen.dims.clientRect.w, 2);
	CVort_engine_gui_drawColoredLine(engine_screen.dims.clientRect.h-2, engine_screen.dims.clientRect.w, 2);
	CVort_engine_gui_drawColoredColumn(0, engine_screen.dims.clientRect.h-2/*1*/, 2);
	CVort_engine_gui_drawColoredColumn(engine_screen.dims.clientRect.w-1, engine_screen.dims.clientRect.h-2/*1*/, 2);
	// Navigation buttons
	if (!(guiCurrentMenuPtr->hideBackButton) && (guiCurrentMenuPtr->backPage || guiCurrentMenuPtr->altBackHandler)) {
		CVort_gui_drawBackButton(guiCurrentMenuStatus.isBackButtonMouseSelected);
	}
	if (guiCurrentMenuPtr->prevPage) {
		CVort_gui_drawPrevButton(guiCurrentMenuStatus.isPrevButtonMouseSelected);
	}
	if (guiCurrentMenuPtr->nextPage) {
		CVort_gui_drawNextButton(guiCurrentMenuStatus.isNextButtonMouseSelected);
	}
	engine_isFrameReadyToDisplay = true;
}

GUI_Menu_Item_T **CVort_gui_getMenuItemSelectionPtrFromMouse(int x, int y) {
	//Vort_gui_transformMouseCoordinates(&x, &y);
	for (GUI_Menu_Item_T **itemPtr = guiCurrentMenuPtr->items; *itemPtr; itemPtr++) {
		if ((x >= (*itemPtr)->x) && (x < (*itemPtr)->x + (*itemPtr)->width) &&
		    (y >= (*itemPtr)->y) && (y < (*itemPtr)->y + (*itemPtr)->height) &&
		    (*itemPtr)->handler) {
			return itemPtr;
		}
	}
	return NULL;
}

void CVort_gui_selectPrevItemChoice(GUI_Menu_Item_T *item) {
	if (item->value) {
		item->value--;
	} else {
		item->value = item->numOfChoices-1;
	}
	item->handler(item);
	// FIXME HACK: Redrawing menu (still, not a common event)
	CVort_engine_gui_clearScreen();
	CVort_gui_drawCurrentMenu();
	//CVort_gui_drawMenuItem(item);
}

void CVort_gui_selectNextItemChoice(GUI_Menu_Item_T *item) {
	item->value++;
	if (item->value == item->numOfChoices) {
		item->value = 0;
	}
	item->handler(item);
	// FIXME HACK: Redrawing menu (still, not a common event)
	CVort_engine_gui_clearScreen();
	CVort_gui_drawCurrentMenu();
	//CVort_gui_drawMenuItem(item);
}

void CVort_gui_setCurrentMenu(GUI_Menu_T *menu) {
	guiCurrentMenuPtr = menu;
	for (guiCurrentMenuItemSelectionPtr = menu->items; (*guiCurrentMenuItemSelectionPtr) && !((*guiCurrentMenuItemSelectionPtr)->handler); guiCurrentMenuItemSelectionPtr++);
	if (!(*guiCurrentMenuItemSelectionPtr)) // End of menu items
		guiCurrentMenuItemSelectionPtr = NULL;
	CVort_gui_resetMenuStatus();
	CVort_engine_gui_clearScreen();
	CVort_gui_drawCurrentMenu();
}

void CVort_gui_changeToPrevPage(void) {
	if (guiCurrentMenuPtr->prevPage) {
		CVort_gui_setCurrentMenu(guiCurrentMenuPtr->prevPage);
	}
}

void CVort_gui_changeToNextPage(void) {
	if (guiCurrentMenuPtr->nextPage) {
		CVort_gui_setCurrentMenu(guiCurrentMenuPtr->nextPage);
	}
}

void CVort_gui_handle_dpad_back(void) {
	if (guiCurrentMenuPtr->backPage) {
		CVort_gui_setCurrentMenu(guiCurrentMenuPtr->backPage);
	} else if (guiCurrentMenuPtr->altBackHandler) {
		(guiCurrentMenuPtr->altBackHandler)();
	}
}

void CVort_gui_handle_dpad_left(void) {
	if (!guiCurrentMenuItemSelectionPtr) {
		return;
	}
	if ((*guiCurrentMenuItemSelectionPtr)->choices) {
		CVort_gui_selectPrevItemChoice(*guiCurrentMenuItemSelectionPtr);
	}
}

void CVort_gui_handle_dpad_right(void) {
	if (!guiCurrentMenuItemSelectionPtr) {
		return;
	}
	if ((*guiCurrentMenuItemSelectionPtr)->choices) {
		CVort_gui_selectNextItemChoice(*guiCurrentMenuItemSelectionPtr);
	}
}

void CVort_gui_handle_dpad_up(void) {
	GUI_Menu_Item_T **menuItemSelectionPtr = guiCurrentMenuItemSelectionPtr;
	GUI_Menu_T *menu = guiCurrentMenuPtr;
	if (!menuItemSelectionPtr) {
		menuItemSelectionPtr = menu->items;
		// Get to last menu entry plus 1 (to be decremented soon)
		while (*menuItemSelectionPtr) {
			menuItemSelectionPtr++;
		}
	}
	do {
		// As long as we don't get past the beginning of
		// buffer of menu items for the current menu
		do {
			menuItemSelectionPtr--;
			// Is it *not* a static label? (Also not past the beginning of buffer)
			if ((menuItemSelectionPtr+1 != menu->items) && (*menuItemSelectionPtr)->handler) {
				break;
			}
		} while (menuItemSelectionPtr+1 != menu->items);
		if (menuItemSelectionPtr+1 != menu->items) { // Match found
			break;
		}
		// Check preceding page (if any)
		if (menu->prevPage) {
			menu = menu->prevPage;
			menuItemSelectionPtr = menu->items;
			// Get to last entry plus 1 (to be decremented soon)
			while (*menuItemSelectionPtr) {
				menuItemSelectionPtr++;
			}
			continue;
		}
		break;
	} while (true);
	if (menuItemSelectionPtr+1 != menu->items) {
		guiCurrentMenuPtr = menu;
		guiCurrentMenuItemSelectionPtr = menuItemSelectionPtr;
		CVort_engine_gui_clearScreen();
		CVort_gui_drawCurrentMenu();
	}
}

void CVort_gui_handle_dpad_down(void) {
	GUI_Menu_Item_T **menuItemSelectionPtr = guiCurrentMenuItemSelectionPtr;
	GUI_Menu_T *menu = guiCurrentMenuPtr;
	if (!menuItemSelectionPtr) {
		menuItemSelectionPtr = menu->items - 1; // To be incremented soon
	}
	do {
		// As long as we don't get pass the end of
		// buffer of menu items for the current menu
		do {
			menuItemSelectionPtr++;
			// Is it *not* a static label? (Also not past the end of buffer)
			if ((*menuItemSelectionPtr) && (*menuItemSelectionPtr)->handler) {
				break;
			}
		} while (*menuItemSelectionPtr);
		if (*menuItemSelectionPtr) { // Match found
			break;
		}
		// Check following page (if any)
		if (menu->nextPage) {
			menu = menu->nextPage;
			menuItemSelectionPtr = menu->items - 1; // To be incremented soon
			continue;
		}
		break;
	} while (true);
	if (*menuItemSelectionPtr) {
		guiCurrentMenuPtr = menu;
		guiCurrentMenuItemSelectionPtr = menuItemSelectionPtr;
		CVort_engine_gui_clearScreen();
		CVort_gui_drawCurrentMenu();
	}
}

void CVort_gui_handle_dpad_activate(void) {
	if (!guiCurrentMenuItemSelectionPtr) {
		return;
	}
	if ((*guiCurrentMenuItemSelectionPtr)->choices) {
		CVort_gui_selectNextItemChoice(*guiCurrentMenuItemSelectionPtr);
	} else {
		(*guiCurrentMenuItemSelectionPtr)->handler(*guiCurrentMenuItemSelectionPtr);
	}
}

// Similar to CVort_gui_handle_dpad_activate (press "enter" key) but also
// handles clicks on arrows for going backwards/forwards in a choices buffer
void CVort_gui_handle_mouse_selection(int pressX, int pressY, int releaseX, int releaseY) {
	// ASSUMPTION: guiCurrentMenuItemSelectionPtr is non-NULL
	if ((*guiCurrentMenuItemSelectionPtr)->choices) {
		GUI_MenuChoiceHitBounds_T bounds;
		CVort_gui_calcMenuChoiceHitBounds(*guiCurrentMenuItemSelectionPtr, &bounds);
		if (CVort_gui_isMouseInChoiceLeftArrow(&bounds, pressX, releaseX)) {
			CVort_gui_selectPrevItemChoice(*guiCurrentMenuItemSelectionPtr);
		} else {
			CVort_gui_selectNextItemChoice(*guiCurrentMenuItemSelectionPtr);
		}
	} else {
		(*guiCurrentMenuItemSelectionPtr)->handler(*guiCurrentMenuItemSelectionPtr);
	}
}

// Prepares main menu items based on available episodes
void CVort_gui_prepareMainMenuItems(void) {
	bool someGameExeFound = false;
	GUI_Menu_Item_T **itemPtr = guiMainMenu.items;
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
	if (CVort_engine_isGameExeAvailable(GAMEVER_KEEN1)) {
		*itemPtr = &guiMenuItemStartKeen1;
		itemPtr++;
		someGameExeFound = true;
	}
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
	if (CVort_engine_isGameExeAvailable(GAMEVER_KEEN2)) {
		*itemPtr = &guiMenuItemStartKeen2;
		itemPtr++;
		someGameExeFound = true;
	}
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
	if (CVort_engine_isGameExeAvailable(GAMEVER_KEEN3)) {
		*itemPtr = &guiMenuItemStartKeen3;
		itemPtr++;
		someGameExeFound = true;
	}
#endif
	if (!someGameExeFound) {
		*itemPtr = &guiMenuItemNoKeenGameEpisode;
		itemPtr++;
	}
	*itemPtr = &guiMenuItemKeysPassToBiosToggle;
	itemPtr++;
	*itemPtr = &guiMenuItemSettings;
	itemPtr++;
	*itemPtr = &guiMenuItemQuit;
	itemPtr++;
	*itemPtr = 0;
}

static const GUI_KeyNavHandlers_T guiMenuKeyNavHandlers = {
	.back = CVort_gui_handle_dpad_back,
	.left = CVort_gui_handle_dpad_left,
	.right = CVort_gui_handle_dpad_right,
	.up = CVort_gui_handle_dpad_up,
	.down = CVort_gui_handle_dpad_down,
	.activate = CVort_gui_handle_dpad_activate,
	.prevPage = CVort_gui_changeToPrevPage,
	.nextPage = CVort_gui_changeToNextPage
};

void CVort_gui_runLoop(void) {
	CVort_gui_prepareMainMenuItems();
	CVort_gui_prepareMenuItemsChoiceBuffers();
	CVort_gui_layoutMenu(&guiMainMenu);
	CVort_gui_layoutMenu(&guiQuitMenu);
	CVort_gui_layoutMenu(&guiSettingsMenu);
	CVort_gui_layoutMenu(&guiVideoSettingsMenus[0]);
	CVort_gui_layoutMenu(&guiVideoSettingsMenus[1]);
	CVort_gui_layoutMenu(&guiVideoSettingsMenus[2]);
	CVort_gui_layoutMenu(&guiInputMapperEntranceWarningMenu);
	CVort_gui_layoutMenu(&guiMapperResetWarningMenu);
	CVort_gui_layoutMenu(&guiMoreSettingsMenu);
	CVort_gui_layoutMenu(&guiCompatibilityOptionsMenu);
	// CVort_gui_layoutMenu(&guiMapperMenu) // Not now
	CVort_gui_layoutMenu(&guiMapperBindingRecordingMenu); // But do this one
	CVort_gui_setCurrentMenu(&guiMainMenu);

	int origPointerX, origPointerY;
	GUI_Menu_Item_T *item;
	SDL_Event event;
	while (1) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
#if SDL_VERSION_ATLEAST(2,0,0)
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						CVort_engine_reactToWindowResize(event.window.data1, event.window.data2);
					} else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						CVort_engine_handleWindowSideChange();
					}
					break;
#else
				case SDL_VIDEORESIZE:
					CVort_engine_reactToWindowResize(event.resize.w, event.resize.h);
					break;
#endif
				case SDL_KEYDOWN:
					if (CVort_gui_handleKeydownAsNavigation(&event.key, &guiMenuKeyNavHandlers) == GUI_KEY_NAV_ACTIVATE) {
						CVort_gui_resetMenuStatus();
					}
				break;
				//case SDL_KEYUP:
				case SDL_MOUSEBUTTONDOWN:
				{
					if (event.button.button != SDL_BUTTON_LEFT) {
						if (event.button.button == SDL_BUTTON_RIGHT) {
							CVort_gui_handle_dpad_back();
						}
						break;
					}
					origPointerX = event.button.x;
					origPointerY = event.button.y;
					CVort_gui_transformMouseCoordinates(&origPointerX, &origPointerY);
					GUI_Menu_Item_T **lastItemPtr = guiCurrentMenuItemSelectionPtr;
					GUI_NavButton_T selectedNavButton = CVort_gui_beginNavButtonMouseSelection(
						&guiCurrentMenuStatus,
						origPointerX, origPointerY,
						!(guiCurrentMenuPtr->hideBackButton) && (guiCurrentMenuPtr->backPage || guiCurrentMenuPtr->altBackHandler),
						guiCurrentMenuPtr->prevPage,
						guiCurrentMenuPtr->nextPage
					);
					if (selectedNavButton != GUI_NAV_BUTTON_NONE) {
						if (lastItemPtr) {
							guiCurrentMenuItemSelectionPtr = NULL;
							CVort_gui_drawMenuItem(*lastItemPtr);
						}
						switch (selectedNavButton) {
						case GUI_NAV_BUTTON_BACK:
							CVort_gui_drawBackButton(true);
							break;
						case GUI_NAV_BUTTON_PREV:
							CVort_gui_drawPrevButton(true);
							break;
						case GUI_NAV_BUTTON_NEXT:
							CVort_gui_drawNextButton(true);
							break;
						default:
							break;
						}
						engine_isFrameReadyToDisplay = true;
						break;
					}
					guiCurrentMenuItemSelectionPtr = CVort_gui_getMenuItemSelectionPtrFromMouse(origPointerX, origPointerY);
					if (lastItemPtr) {
						CVort_gui_drawMenuItem(*lastItemPtr);
						engine_isFrameReadyToDisplay = true;
					}
					if (guiCurrentMenuItemSelectionPtr) {
						CVort_gui_drawMenuItem(*guiCurrentMenuItemSelectionPtr);
						engine_isFrameReadyToDisplay = true;
					}
				}
				break;
				case SDL_MOUSEBUTTONUP:
				{
					if (!guiCurrentMenuStatus.waitForMouseButtonRelease || (event.button.button != SDL_BUTTON_LEFT)) {
						break;
					}
					int lastPointerX = event.button.x, lastPointerY = event.button.y;
					CVort_gui_transformMouseCoordinates(&lastPointerX, &lastPointerY);
					GUI_NavButton_T releasedNavButton = CVort_gui_getNavButtonMouseReleaseAction(&guiCurrentMenuStatus, lastPointerX, lastPointerY);
					if (releasedNavButton == GUI_NAV_BUTTON_BACK) {
						CVort_gui_resetMenuStatus();
						CVort_gui_handle_dpad_back();
						break;
					}
					if (releasedNavButton == GUI_NAV_BUTTON_PREV) {
						CVort_gui_resetMenuStatus();
						CVort_gui_changeToPrevPage();
						break;
					}
					if (releasedNavButton == GUI_NAV_BUTTON_NEXT) {
						CVort_gui_resetMenuStatus();
						CVort_gui_changeToNextPage();
						break;
					}
					if (guiCurrentMenuItemSelectionPtr && (guiCurrentMenuItemSelectionPtr == CVort_gui_getMenuItemSelectionPtrFromMouse(lastPointerX, lastPointerY))) {
						CVort_gui_resetMenuStatus();
						CVort_gui_handle_mouse_selection(origPointerX, origPointerY, lastPointerX, lastPointerY);
						break;
					}
					CVort_gui_resetMenuStatus();
					CVort_gui_drawCurrentMenu();
				}
				break;
			case SDL_QUIT:
				CVort_engine_shutdown();
				exit(0);
			default:;

			}
		}
		CVort_gui_updateDisplayAndSleep();
	}
}
