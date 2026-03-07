#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "core/globals.h"
#include "ui/gui_loop_common.h"
#include "ui/gui_menu_loop.h"
#include "ui/gui_mapper.h"
#include "ui/gui_mapper_nav.h"
#include "ui/gui_runtime.h"

static GUI_NavButtonsMouseState_T guiCurrentMapperStatus;

/************************************************************
For each mapper UI tile (say a key):
- Have a total height of 34 pixels at the least.
- 14 pixels height for a row of text.
- 10 pixels of top invisible border, and 10 of bottom border.
************************************************************/
#define GUI_MAPPER_ITEM_TEXT_X1_OFFSET 2
#define GUI_MAPPER_ITEM_TEXT_Y1_OFFSET 2
#define GUI_MAPPER_ITEM_TEXT_Y2_OFFSET 2
#define GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH 8
#define GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT 8

static void CVort_gui_mapper_drawTileLabel(const char *label, int x, int y) {
	for (; *label; x += GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH, label++) {
		CVort_engine_gui_drawFontChar_ptr(
			x, y,
			GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH,
			GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT,
			cga_8x8TextFont + GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH * GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT * (unsigned char)(*label),
			0, 1
		);
	}
}

void CVort_gui_mapper_drawTile(GUI_Mapper_Tile_T *mapperTile) {
	//int labelColor = 0;
	int innerColor = (guiCurrentMapperTilePtr && (*guiCurrentMapperTilePtr == mapperTile)) ? 12 : 15;
	int borderColor = (guiCurrentMapperTilePtr && (*guiCurrentMapperTilePtr == mapperTile)) ? 4 : 7;
	CVort_engine_gui_drawRoundedRectBorder_ptr(mapperTile->x, mapperTile->y, mapperTile->width, mapperTile->height, borderColor, innerColor, 1);
	int x, y;
	const char *label = mapperTile->topLeftLabel;
	if (label) {
		x = mapperTile->x + GUI_MAPPER_ITEM_TEXT_X1_OFFSET;
		y = mapperTile->y + GUI_MAPPER_ITEM_TEXT_Y1_OFFSET;
		CVort_gui_mapper_drawTileLabel(label, x, y);
	}
	label = mapperTile->midLeftLabel;
	if (label) {
		x = mapperTile->x + GUI_MAPPER_ITEM_TEXT_X1_OFFSET;
		y = mapperTile->y + (mapperTile->height - GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT) / 2;
		CVort_gui_mapper_drawTileLabel(label, x, y);
	}
	label = mapperTile->botLeftLabel;
	if (label) {
		x = mapperTile->x + GUI_MAPPER_ITEM_TEXT_X1_OFFSET;
		y = mapperTile->y + (mapperTile->height - GUI_MAPPER_ITEM_TEXT_Y2_OFFSET - GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT);
		CVort_gui_mapper_drawTileLabel(label, x, y);
	}
}

void CVort_gui_mapper_drawCurrentPage(void) {
	for (GUI_Mapper_Tile_T **mapperTilePtr = guiCurrentMapperPagePtr->tiles; *mapperTilePtr; mapperTilePtr++) {
		CVort_gui_mapper_drawTile(*mapperTilePtr);
	}
	CVort_gui_drawBackButton(guiCurrentMapperStatus.isBackButtonMouseSelected); // Always done
	if (guiCurrentMapperPagePtr->prevPage) {
		CVort_gui_drawPrevButton(guiCurrentMapperStatus.isPrevButtonMouseSelected);
	}
	if (guiCurrentMapperPagePtr->nextPage) {
		CVort_gui_drawNextButton(guiCurrentMapperStatus.isNextButtonMouseSelected);
	}
	engine_isFrameReadyToDisplay = true;
}
// Code duplication, but small for now...
void CVort_gui_mapper_resetStatus(void) {
	CVort_gui_resetNavButtonsMouseState(&guiCurrentMapperStatus);
}

GUI_Mapper_Tile_T **CVort_gui_mapper_getTileSelectionPtrFromMouse(int x, int y) {
	//Vort_gui_transformMouseCoordinates(&x, &y);
	for (GUI_Mapper_Tile_T **tilePtr = guiCurrentMapperPagePtr->tiles; *tilePtr; tilePtr++) {
		if (((*tilePtr)->emuEvent.emulatedInput != EMULATEDINPUT_NONE) &&
		    (x >= (*tilePtr)->x) && (x < (*tilePtr)->x + (*tilePtr)->width) &&
		    (y >= (*tilePtr)->y) && (y < (*tilePtr)->y + (*tilePtr)->height)) {
			return tilePtr;
		}
	}
	return NULL;
}

void CVort_gui_mapper_setCurrentPage(GUI_Mapper_Page_T *page) {
	guiCurrentMapperPagePtr = page;
	guiCurrentMapperTilePtr = NULL;
	CVort_gui_mapper_resetStatus();
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
}

void CVort_gui_mapper_changeToPrevPage(void) {
	if (guiCurrentMapperPagePtr->prevPage) {
		CVort_gui_mapper_setCurrentPage(guiCurrentMapperPagePtr->prevPage);
	}
}

void CVort_gui_mapper_changeToNextPage(void) {
	if (guiCurrentMapperPagePtr->nextPage) {
		CVort_gui_mapper_setCurrentPage(guiCurrentMapperPagePtr->nextPage);
	}
}

static bool guiMapperLoopRunningStatus;

void CVort_gui_mapper_handle_tile(EmulatedInput_T emuInput, int value) {
	// Technically return to main launcher UI, but in a special menu
	guiMapperLoopRunningStatus = false;
	// Set emulated event (to be stored internally)
	guiCurrentEmuEventDetails.emulatedInput = emuInput;
	guiCurrentEmuEventDetails.value = value;
	CVort_gui_showMapperMenu();
}

void CVort_gui_mapper_handle_dpad_back(void) {
	// Return to main launcher UI loop with the settings menu
	guiMapperLoopRunningStatus = false;
	//CVort_engine_saveInputMappings();
	CVort_gui_setCurrentMenu(&guiSettingsMenu);
}

static void CVort_gui_mapper_handle_dpad_move(GUI_Mapper_MoveDirection_T direction) {
	CVort_gui_mapper_moveSelection(guiCurrentMapperPagePtr, &guiCurrentMapperTilePtr, direction);
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
}

void CVort_gui_mapper_handle_dpad_left(void) {
	CVort_gui_mapper_handle_dpad_move(GUI_MAPPER_MOVE_LEFT);
}

void CVort_gui_mapper_handle_dpad_right(void) {
	CVort_gui_mapper_handle_dpad_move(GUI_MAPPER_MOVE_RIGHT);
}

void CVort_gui_mapper_handle_dpad_up(void) {
	CVort_gui_mapper_handle_dpad_move(GUI_MAPPER_MOVE_UP);
}

void CVort_gui_mapper_handle_dpad_down(void) {
	CVort_gui_mapper_handle_dpad_move(GUI_MAPPER_MOVE_DOWN);
}

void CVort_gui_mapper_handle_dpad_activate(void) {
	if (!guiCurrentMapperTilePtr) {
		return;
	}
	CVort_gui_mapper_handle_tile((*guiCurrentMapperTilePtr)->emuEvent.emulatedInput, (*guiCurrentMapperTilePtr)->emuEvent.value);
}

static const GUI_KeyNavHandlers_T guiMapperKeyNavHandlers = {
	.back = CVort_gui_mapper_handle_dpad_back,
	.left = CVort_gui_mapper_handle_dpad_left,
	.right = CVort_gui_mapper_handle_dpad_right,
	.up = CVort_gui_mapper_handle_dpad_up,
	.down = CVort_gui_mapper_handle_dpad_down,
	.activate = CVort_gui_mapper_handle_dpad_activate,
	.prevPage = CVort_gui_mapper_changeToPrevPage,
	.nextPage = CVort_gui_mapper_changeToNextPage
};

// Does NOT set current mapper page, which should be set externally (if at all).
void CVort_gui_mapper_runLoop(void) {
	int origPointerX, origPointerY;
	SDL_Event event;
	guiMapperLoopRunningStatus = true;
	do {
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
					CVort_gui_handleKeydownAsNavigation(&event.key, &guiMapperKeyNavHandlers);
				break;
				//case SDL_KEYUP:
				case SDL_MOUSEBUTTONDOWN:
				{
					if (event.button.button != SDL_BUTTON_LEFT) {
						if (event.button.button == SDL_BUTTON_RIGHT) {
							CVort_gui_mapper_handle_dpad_back();
						}
						break;
					}
					origPointerX = event.button.x;
					origPointerY = event.button.y;
					CVort_gui_transformMouseCoordinates(&origPointerX, &origPointerY);
					GUI_Mapper_Tile_T **lastTilePtr = guiCurrentMapperTilePtr;
					GUI_NavButton_T selectedNavButton = CVort_gui_beginNavButtonMouseSelection(
						&guiCurrentMapperStatus,
						origPointerX, origPointerY,
						true,
						guiCurrentMapperPagePtr->prevPage,
						guiCurrentMapperPagePtr->nextPage
					);
					if (selectedNavButton != GUI_NAV_BUTTON_NONE) {
						if (lastTilePtr) {
							guiCurrentMapperTilePtr = NULL;
							CVort_gui_mapper_drawTile(*lastTilePtr);
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
					guiCurrentMapperTilePtr = CVort_gui_mapper_getTileSelectionPtrFromMouse(origPointerX, origPointerY);
					if (lastTilePtr) {
						CVort_gui_mapper_drawTile(*lastTilePtr);
						engine_isFrameReadyToDisplay = true;
					}
					if (guiCurrentMapperTilePtr) {
						CVort_gui_mapper_drawTile(*guiCurrentMapperTilePtr);
						engine_isFrameReadyToDisplay = true;
					}
				}
				break;
				case SDL_MOUSEBUTTONUP:
				{
					if (!guiCurrentMapperStatus.waitForMouseButtonRelease || (event.button.button != SDL_BUTTON_LEFT)) {
						break;
					}
					int lastPointerX = event.button.x, lastPointerY = event.button.y;
					CVort_gui_transformMouseCoordinates(&lastPointerX, &lastPointerY);
					GUI_NavButton_T releasedNavButton = CVort_gui_getNavButtonMouseReleaseAction(&guiCurrentMapperStatus, lastPointerX, lastPointerY);
					if (releasedNavButton == GUI_NAV_BUTTON_BACK) {
						CVort_gui_mapper_resetStatus();
						CVort_gui_mapper_handle_dpad_back();
						break;
					}
					if (releasedNavButton == GUI_NAV_BUTTON_PREV) {
						CVort_gui_mapper_resetStatus();
						CVort_gui_mapper_changeToPrevPage();
						break;
					}
					if (releasedNavButton == GUI_NAV_BUTTON_NEXT) {
						CVort_gui_mapper_resetStatus();
						CVort_gui_mapper_changeToNextPage();
						break;
					}
					if (guiCurrentMapperTilePtr && (guiCurrentMapperTilePtr == CVort_gui_mapper_getTileSelectionPtrFromMouse(lastPointerX, lastPointerY))) {
						CVort_gui_mapper_resetStatus();
						CVort_gui_mapper_handle_tile((*guiCurrentMapperTilePtr)->emuEvent.emulatedInput, (*guiCurrentMapperTilePtr)->emuEvent.value);
						// We now (temporarily) exit the mapper loop
						break;
					}
					CVort_gui_mapper_resetStatus();
					CVort_gui_mapper_drawCurrentPage();
				}
				break;
			case SDL_QUIT:
				CVort_engine_shutdown();
				exit(0);
			default:;

			}
		}
		CVort_gui_updateDisplayAndSleep();
	} while (guiMapperLoopRunningStatus);
}
