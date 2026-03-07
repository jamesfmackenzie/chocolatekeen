#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "core/globals.h"
#include "platform/platform.h"
#include "ui/gui_menu_loop.h"
#include "ui/gui_mapper.h"
#include "ui/gui_runtime.h"

static struct {
	bool waitForMouseButtonRelease;
	bool isBackButtonMouseSelected, isPrevButtonMouseSelected, isNextButtonMouseSelected;
} guiCurrentMapperStatus;

// Local copy for mapper loop coordinate handling.
static void CVort_gui_transformMouseCoordinates(int *xPtr, int *yPtr) {
	(*xPtr) -= engine_screen.dims.borderedViewportRect.x;
	(*yPtr) -= engine_screen.dims.borderedViewportRect.y;
	*xPtr = (*xPtr) * ENGINE_GUI_WIDTH / engine_screen.dims.borderedViewportRect.w;
	*yPtr = (*yPtr) * ENGINE_GUI_HEIGHT / engine_screen.dims.borderedViewportRect.h;
}

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

void CVort_gui_mapper_drawTile(GUI_Mapper_Tile_T *mapperTile) {
	//int labelColor = 0;
	int innerColor = (guiCurrentMapperTilePtr && (*guiCurrentMapperTilePtr == mapperTile)) ? 12 : 15;
	int borderColor = (guiCurrentMapperTilePtr && (*guiCurrentMapperTilePtr == mapperTile)) ? 4 : 7;
	CVort_engine_gui_drawRoundedRectBorder_ptr(mapperTile->x, mapperTile->y, mapperTile->width, mapperTile->height, borderColor, innerColor, 1);
	int x, y;
	// FIXME? Code duplication...
	const char *label = mapperTile->topLeftLabel;
	if (label) {
		x = mapperTile->x + GUI_MAPPER_ITEM_TEXT_X1_OFFSET;
		y = mapperTile->y + GUI_MAPPER_ITEM_TEXT_Y1_OFFSET;
		for (; *label; x+=GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH, label++) {
			CVort_engine_gui_drawFontChar_ptr(
				x, y,
				GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH,
				GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT,
				cga_8x8TextFont+GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH*GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT*(unsigned char)(*label),
				0, 1
			);
		}
	}
	label = mapperTile->midLeftLabel;
	if (label) {
		x = mapperTile->x + GUI_MAPPER_ITEM_TEXT_X1_OFFSET;
		y = mapperTile->y + (mapperTile->height - GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT) / 2;
		for (; *label; x+=GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH, label++) {
			CVort_engine_gui_drawFontChar_ptr(
				x, y,
				GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH,
				GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT,
				cga_8x8TextFont+GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH*GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT*(unsigned char)(*label),
				0, 1
			);
		}
	}
	label = mapperTile->botLeftLabel;
	if (label) {
		x = mapperTile->x + GUI_MAPPER_ITEM_TEXT_X1_OFFSET;
		y = mapperTile->y + (mapperTile->height - GUI_MAPPER_ITEM_TEXT_Y2_OFFSET - GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT);
		for (; *label; x+=GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH, label++) {
			CVort_engine_gui_drawFontChar_ptr(
				x, y,
				GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH,
				GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT,
				cga_8x8TextFont+GUI_MAPPER_ITEM_TEXT_CHAR_WIDTH*GUI_MAPPER_ITEM_TEXT_CHAR_HEIGHT*(unsigned char)(*label),
				0, 1
			);
		}
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
	guiCurrentMapperStatus.waitForMouseButtonRelease = false;
	guiCurrentMapperStatus.isBackButtonMouseSelected = guiCurrentMapperStatus.isPrevButtonMouseSelected = guiCurrentMapperStatus.isNextButtonMouseSelected = false;
}
/************************************
Some additions for the mapper UI loop
************************************/

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

// FIXME: Unnecessary code duplication

void CVort_gui_mapper_handle_dpad_left(void) {
	if (!guiCurrentMapperTilePtr) {
		// FIXME: Pick a different tile
		for (guiCurrentMapperTilePtr = guiCurrentMapperPagePtr->tiles; (*guiCurrentMapperTilePtr)->emuEvent.emulatedInput == EMULATEDINPUT_NONE; guiCurrentMapperTilePtr++);
	} else if ((*guiCurrentMapperTilePtr)->nearTiles.left) {
		guiCurrentMapperTilePtr = &((*guiCurrentMapperTilePtr)->nearTiles.left);
	}
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
}

void CVort_gui_mapper_handle_dpad_right(void) {
	if (!guiCurrentMapperTilePtr) {
		for (guiCurrentMapperTilePtr = guiCurrentMapperPagePtr->tiles; (*guiCurrentMapperTilePtr)->emuEvent.emulatedInput == EMULATEDINPUT_NONE; guiCurrentMapperTilePtr++);
	} else if ((*guiCurrentMapperTilePtr)->nearTiles.right) {
		guiCurrentMapperTilePtr = &((*guiCurrentMapperTilePtr)->nearTiles.right);
	}
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
}

void CVort_gui_mapper_handle_dpad_up(void) {
	if (!guiCurrentMapperTilePtr) {
		// FIXME: Pick a different tile?
		for (guiCurrentMapperTilePtr = guiCurrentMapperPagePtr->tiles; (*guiCurrentMapperTilePtr)->emuEvent.emulatedInput == EMULATEDINPUT_NONE; guiCurrentMapperTilePtr++);
	} else if ((*guiCurrentMapperTilePtr)->nearTiles.up) {
		guiCurrentMapperTilePtr = &((*guiCurrentMapperTilePtr)->nearTiles.up);
	}
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
}

void CVort_gui_mapper_handle_dpad_down(void) {
	if (!guiCurrentMapperTilePtr) {
		for (guiCurrentMapperTilePtr = guiCurrentMapperPagePtr->tiles; (*guiCurrentMapperTilePtr)->emuEvent.emulatedInput == EMULATEDINPUT_NONE; guiCurrentMapperTilePtr++);
	} else if ((*guiCurrentMapperTilePtr)->nearTiles.down) {
		guiCurrentMapperTilePtr = &((*guiCurrentMapperTilePtr)->nearTiles.down);
	}
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
}

void CVort_gui_mapper_handle_dpad_activate(void) {
	if (!guiCurrentMapperTilePtr) {
		return;
	}
	CVort_gui_mapper_handle_tile((*guiCurrentMapperTilePtr)->emuEvent.emulatedInput, (*guiCurrentMapperTilePtr)->emuEvent.value);
}

// Does NOT set current mapper page, which should be set externally (if at all).
void CVort_gui_mapper_runLoop(void) {
	int origPointerX, origPointerY;
	GUI_Mapper_Tile_T *mapperTile;
	SDL_Event event;
	guiMapperLoopRunningStatus = true;
	// BIG FIXME: Lots of code duplication, although not identical.
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
#if SDL_VERSION_ATLEAST(2,0,0)
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_ESCAPE:
						CVort_gui_mapper_handle_dpad_back();
						//guiCurrentPagePtr->pageEscapeHandler();
						break;
					case SDL_SCANCODE_LEFT:
						CVort_gui_mapper_handle_dpad_left();
						break;
					case SDL_SCANCODE_RIGHT:
						CVort_gui_mapper_handle_dpad_right();
						break;
					case SDL_SCANCODE_UP:
						CVort_gui_mapper_handle_dpad_up();
						break;
					case SDL_SCANCODE_DOWN:
						CVort_gui_mapper_handle_dpad_down();
						break;
					case SDL_SCANCODE_RETURN:
						CVort_gui_mapper_handle_dpad_activate();
						break;
					case SDL_SCANCODE_PAGEUP:
						CVort_gui_mapper_changeToPrevPage();
						break;
					case SDL_SCANCODE_PAGEDOWN:
						CVort_gui_mapper_changeToNextPage();
						break;
				}
#else
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						CVort_gui_mapper_handle_dpad_back();
						//guiCurrentPagePtr->pageEscapeHandler();
						break;
					case SDLK_LEFT:
						CVort_gui_mapper_handle_dpad_left();
						break;
					case SDLK_RIGHT:
						CVort_gui_mapper_handle_dpad_right();
						break;
					case SDLK_UP:
						CVort_gui_mapper_handle_dpad_up();
						break;
					case SDLK_DOWN:
						CVort_gui_mapper_handle_dpad_down();
						break;
					case SDLK_RETURN:
						CVort_gui_mapper_handle_dpad_activate();
						break;
					case SDLK_PAGEUP:
						CVort_gui_mapper_changeToPrevPage();
						break;
					case SDLK_PAGEDOWN:
						CVort_gui_mapper_changeToNextPage();
						break;
				}
#endif
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
					guiCurrentMapperStatus.waitForMouseButtonRelease = true;
					origPointerX = event.button.x;
					origPointerY = event.button.y;
					CVort_gui_transformMouseCoordinates(&origPointerX, &origPointerY);
					GUI_Mapper_Tile_T **lastTilePtr = guiCurrentMapperTilePtr;
					if (CVort_gui_isBackButtonSelectedByMouse(origPointerX, origPointerY)) {
						guiCurrentMapperStatus.isBackButtonMouseSelected = true;
						if (lastTilePtr) {
							guiCurrentMapperTilePtr = NULL;
							CVort_gui_mapper_drawTile(*lastTilePtr);
						}
						CVort_gui_drawBackButton(guiCurrentMapperStatus.isBackButtonMouseSelected);
						engine_isFrameReadyToDisplay = true;
						break;
					}
					if (guiCurrentMapperPagePtr->prevPage && CVort_gui_isPrevButtonSelectedByMouse(origPointerX, origPointerY)) {
						guiCurrentMapperStatus.isPrevButtonMouseSelected = true;
						if (lastTilePtr) {
							guiCurrentMapperTilePtr = NULL;
							CVort_gui_mapper_drawTile(*lastTilePtr);
						}
						CVort_gui_drawPrevButton(guiCurrentMapperStatus.isPrevButtonMouseSelected);
						engine_isFrameReadyToDisplay = true;
						break;
					}
					if (guiCurrentMapperPagePtr->nextPage && CVort_gui_isNextButtonSelectedByMouse(origPointerX, origPointerY)) {
						guiCurrentMapperStatus.isNextButtonMouseSelected = true;
						if (lastTilePtr) {
							guiCurrentMapperTilePtr = NULL;
							CVort_gui_mapper_drawTile(*lastTilePtr);
						}
						CVort_gui_drawNextButton(guiCurrentMapperStatus.isNextButtonMouseSelected);
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
					if (guiCurrentMapperStatus.isBackButtonMouseSelected && CVort_gui_isBackButtonSelectedByMouse(lastPointerX, lastPointerY)) {
						CVort_gui_mapper_resetStatus();
						CVort_gui_mapper_handle_dpad_back();
						break;
					}
					if (guiCurrentMapperStatus.isPrevButtonMouseSelected && CVort_gui_isPrevButtonSelectedByMouse(lastPointerX, lastPointerY)) {
						CVort_gui_mapper_resetStatus();
						CVort_gui_mapper_changeToPrevPage();
						break;
					}
					if (guiCurrentMapperStatus.isNextButtonMouseSelected && CVort_gui_isNextButtonSelectedByMouse(lastPointerX, lastPointerY)) {
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
			case SDL_QUIT: // TODO Is this ok?
				CVort_engine_shutdown();
				exit(0);
			default:;

			}
		}
		if (engine_isFrameReadyToDisplay || ((uint32_t)(SDL_GetTicks() - engine_lastDisplayUpdateTime) >= 100)) {
			CVort_engine_updateActualDisplay();
			engine_lastDisplayUpdateTime = SDL_GetTicks();
		}
        CK_PlatformSleepMs(1);
	} while (guiMapperLoopRunningStatus);
}
