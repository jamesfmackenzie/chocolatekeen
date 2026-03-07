#include <assert.h>

#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "core/globals.h"
#include "SDL.h"
#include "platform/platform.h"
#include "ui/gui_internal.h"
#include "ui/gui_runtime.h"
#include "ui/gui_types.h"

GUI_Menu_T *guiCurrentMenuPtr;
GUI_Menu_Item_T **guiCurrentMenuItemSelectionPtr;

static int getMaxStringLength(const char **stringArray) {
	int maxLength = 0;
	for (; *stringArray; stringArray++) {
		if (maxLength < strlen(*stringArray)) {
			maxLength = strlen(*stringArray);
		}
	}
	return maxLength;
}

static int getNumOfStrings(const char **stringArray) {
	int count = 0;
	for (; *stringArray; stringArray++, count++);
	return count;
}

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



// Calculates menu item dimensions without horizontal spacing (but not the positions)
void CVort_gui_layoutMenuItem(GUI_Menu_Item_T *item) {
	int numOfRows = 1, maxLabelRowWidth = 0, currRowWidth = 0;
	for (const char *chPtr = item->label; *chPtr; chPtr++) {
		if (*chPtr == '\n') {
			numOfRows++;
			maxLabelRowWidth = (currRowWidth > maxLabelRowWidth) ? currRowWidth : maxLabelRowWidth;
			currRowWidth = 0;
		} else {
			currRowWidth++;
		}
	}
	// For last row
	maxLabelRowWidth = (currRowWidth > maxLabelRowWidth) ? currRowWidth : maxLabelRowWidth;

	item->width = GUI_MENU_ITEM_TEXT_CHAR_WIDTH*maxLabelRowWidth;
	if (item->choices) {
		int maxLength = getMaxStringLength(item->choices);
		item->choiceMaxPixWidth = GUI_MENU_ITEM_TEXT_CHAR_WIDTH*maxLength;
		// Add one separator char,
		// two "prev choice" chars and two "next choice" chars.
		item->width += GUI_MENU_ITEM_TEXT_CHAR_WIDTH*5+item->choiceMaxPixWidth;
	}
	item->height = GUI_MENU_ITEM_TEXT_CHAR_HEIGHT*numOfRows + GUI_MENU_ITEM_TEXT_Y1_OFFSET + GUI_MENU_ITEM_TEXT_Y2_OFFSET;
}

// Calculates title and menu items' positions, and items' dimensions
void CVort_gui_layoutMenu(GUI_Menu_T *menu) {
	// First check the title
	int numOfTitleRows = 1, maxTitleRowWidth = 0, currTitleRowWidth = 0;
	for (const char *chPtr = menu->title; *chPtr; chPtr++) {
		if (*chPtr == '\n') {
			numOfTitleRows++;
			maxTitleRowWidth = (currTitleRowWidth > maxTitleRowWidth) ? currTitleRowWidth : maxTitleRowWidth;
			currTitleRowWidth = 0;
		} else {
			currTitleRowWidth++;
		}
	}
	// For last row
	maxTitleRowWidth = (currTitleRowWidth > maxTitleRowWidth) ? currTitleRowWidth : maxTitleRowWidth;
	// Store total menu pixel dimensions so far, including vertical spacing
	int menuPixWidth, menuPixHeight, titlePixWidth;
	menuPixWidth = titlePixWidth = GUI_MENU_ITEM_TEXT_CHAR_WIDTH*maxTitleRowWidth;
	menuPixHeight = GUI_MENU_ITEM_TEXT_CHAR_HEIGHT*numOfTitleRows + GUI_MENU_ITEM_TEXT_Y1_OFFSET + GUI_MENU_ITEM_TEXT_Y2_OFFSET;
	// Next take the menu items into consideration, calculating their sizes
	for (GUI_Menu_Item_T **items = menu->items; *items; items++) {
		CVort_gui_layoutMenuItem(*items);
		menuPixWidth = ((*items)->width > menuPixWidth) ? (*items)->width : menuPixWidth;
		menuPixHeight += (*items)->height;
	}
	// We add 8 for vertical menu borders (and a bit more)
	assert(menuPixWidth+8 <= engine_screen.dims.clientRect.w);
	assert(menuPixHeight <= engine_screen.dims.clientRect.h);
	// We are ready to reposition anything now
	menu->title_x = (engine_screen.dims.clientRect.w-titlePixWidth)/2;
	//menu->title_y = (engine_screen.dims.clientRect.h-menuPixHeight)/2;
	menu->title_y = GUI_MENU_ITEM_TEXT_Y1_OFFSET;
	//menu->width = menuPixWidth;
	//int commonXCoord = (engine_screen.dims.clientRect.w-menuPixWidth)/2;
	menu->width = engine_screen.dims.clientRect.w-8;
	int commonXCoord = 4;
	int lastYCoord = menu->title_y + GUI_MENU_ITEM_TEXT_CHAR_HEIGHT*numOfTitleRows + GUI_MENU_ITEM_TEXT_Y2_OFFSET;
	for (GUI_Menu_Item_T **items = menu->items; *items; items++) {
		(*items)->x = commonXCoord;
		(*items)->y = lastYCoord;
		(*items)->width = menu->width;
		// Keep height as-is
		(*items)->text_y = lastYCoord + GUI_MENU_ITEM_TEXT_Y1_OFFSET;
		lastYCoord += (*items)->height;
	}
}

GUI_VideoMode_T *guiVideoModes;
const char **guiFullScreenModeStrs;
const char **guiWindowedModeStrs;
#if SDL_VERSION_ATLEAST(2,0,0)
const char **guiDisplayNumStrs;
static void *guiRendererDriversBuffers;
#endif

// Returns max. amount of characters per resolution entry string
int CVort_gui_createScreenResolutionBuffers(void) {
#if SDL_VERSION_ATLEAST(2,0,0)
	int numOfModes = SDL_GetNumDisplayModes(engine_arguments.displayNumber);
	SDL_DisplayMode dispMode;
#else
	int numOfModes = 0 ;
	SDL_Rect **sdlVideoModes = SDL_ListModes(engine_screen.sdl.videoInfo->vfmt, SDL_FULLSCREEN);
	if ((sdlVideoModes != NULL) && (sdlVideoModes != (SDL_Rect **)(-1))) {
		for (; sdlVideoModes[numOfModes]; numOfModes++);
	}
#endif
	const int strLengthUpperBoundWithNull = 13;
	int reportedModeLoopVar, actualModeLoopVar, innerLoopVar;
	void *buffers = malloc(strLengthUpperBoundWithNull*numOfModes + // Room for fullscreen resolutions
	                       strLengthUpperBoundWithNull*numOfModes + // Room for windowed resolutions
	                       strlen("Desktop") + 1 +              // Fullscreen desktop resolution
	                       strlen("Default") + 1 +              // Default windowed resolution
	                       (numOfModes + 2) * sizeof(char *) +  // Fullscreen res pointer table
	                       (numOfModes + 2) * sizeof(char *) +  // Windowed res pointer table
	                       numOfModes * sizeof(GUI_VideoMode_T) // The modes in non-string form
	);
	guiVideoModes = (GUI_VideoMode_T *)buffers;
	char **fullScreenModeStrs = (char **)((uint8_t *)buffers + numOfModes * sizeof(GUI_VideoMode_T));
	guiFullScreenModeStrs = (const char **)fullScreenModeStrs;
	char **windowedModeStrs = (char **)((uint8_t *)fullScreenModeStrs + (numOfModes + 2) * sizeof(char *));
	guiWindowedModeStrs = (const char **)windowedModeStrs;
	char *fullScreenModeStrData = (char *)((uint8_t *)windowedModeStrs + (numOfModes + 2) * sizeof(char *));
	char *windowedModeStrData = (char *)((uint8_t *)fullScreenModeStrData + strlen("Desktop") + 1 + strLengthUpperBoundWithNull*numOfModes);

	// First add our default choices
	memcpy(fullScreenModeStrData, "Desktop", sizeof("Desktop"));
	*fullScreenModeStrs = fullScreenModeStrData;
	fullScreenModeStrs++;
	fullScreenModeStrData += strlen("Desktop") + 1;
	memcpy(windowedModeStrData, "Default", sizeof("Default"));
	*windowedModeStrs = windowedModeStrData;
	windowedModeStrs++;
	windowedModeStrData += strlen("Default") + 1;
	// Independently of the SDL branch, the screen resolutions
	// as reported by SDL should be listed in descending order
	for (reportedModeLoopVar = numOfModes-1, actualModeLoopVar = 0; reportedModeLoopVar >= 0; reportedModeLoopVar--) {
#if SDL_VERSION_ATLEAST(2,0,0)
		SDL_GetDisplayMode(engine_arguments.displayNumber, reportedModeLoopVar, &dispMode);
		guiVideoModes[actualModeLoopVar].w = dispMode.w;
		guiVideoModes[actualModeLoopVar].h = dispMode.h;
#else
		guiVideoModes[actualModeLoopVar].w = sdlVideoModes[reportedModeLoopVar]->w;
		guiVideoModes[actualModeLoopVar].h = sdlVideoModes[reportedModeLoopVar]->h;
#endif
		// Check for a possible duplication
		for (innerLoopVar = 0; innerLoopVar < actualModeLoopVar; innerLoopVar++) {
			if ((guiVideoModes[innerLoopVar].w == guiVideoModes[actualModeLoopVar].w)
			    && (guiVideoModes[innerLoopVar].h == guiVideoModes[actualModeLoopVar].h)) {
				break;
			}
		}
		// Again, check for duplication
		if (innerLoopVar < actualModeLoopVar) {
			continue;
		}
		// No duplication so let's add the mode (and keep it in guiVideoModes)
		snprintf(fullScreenModeStrData, strLengthUpperBoundWithNull, "%dx%d", guiVideoModes[actualModeLoopVar].w, guiVideoModes[actualModeLoopVar].h);
		*fullScreenModeStrs = fullScreenModeStrData;
		fullScreenModeStrs++;
		fullScreenModeStrData += strLengthUpperBoundWithNull;

		snprintf(windowedModeStrData, strLengthUpperBoundWithNull, "%dx%d", guiVideoModes[actualModeLoopVar].w, guiVideoModes[actualModeLoopVar].h);
		*windowedModeStrs = windowedModeStrData;
		windowedModeStrs++;
		windowedModeStrData += strLengthUpperBoundWithNull;

		actualModeLoopVar++;
	}
	*fullScreenModeStrs = *windowedModeStrs = 0; // Ends of string lists
	return strLengthUpperBoundWithNull-1;
}

#if SDL_VERSION_ATLEAST(2,0,0)
// Returns max. amount of characters per display number string
int CVort_gui_createDisplayNumberBuffers(void) {
	int numOfModes = SDL_GetNumVideoDisplays();
	const int strLengthUpperBoundWithNull = 12;
	void *buffers = malloc(strLengthUpperBoundWithNull * numOfModes + // Room for display numbers
	                       (numOfModes + 1) * sizeof(char *)          // Pointer table
	);
	char **displayNumStrs = (char **)((uint8_t *)buffers + strLengthUpperBoundWithNull * numOfModes);
	guiDisplayNumStrs = (const char **)displayNumStrs;
	char *displayNumbersData = (char *)buffers;
	for (int index = 0; index < numOfModes; index++) {
		snprintf(displayNumbersData, strLengthUpperBoundWithNull, "%d", index);
		*displayNumStrs = displayNumbersData;
		displayNumStrs++;
		displayNumbersData += strLengthUpperBoundWithNull;
	}
	*displayNumStrs = 0; // End of string list
	return strLengthUpperBoundWithNull-1;
}

static void CVort_gui_deleteRendererDriverBuffers(void) {
	free(guiRendererDriversBuffers);
	guiRendererDriversBuffers = NULL;
}

static const char **CVort_gui_createRendererDriverChoices(int numDrivers) {
	const size_t driverNameMaxLenWithNull = 32;
	const size_t choicesCount = numDrivers + 2; // "auto", drivers and NULL terminator
	void *buffers = malloc(choicesCount * sizeof(char *) + (choicesCount - 1) * driverNameMaxLenWithNull);
	if (!buffers) {
		return NULL;
	}
	guiRendererDriversBuffers = buffers;

	char **rendererDrivers = (char **)buffers;
	char *rendererDriversData = (char *)((uint8_t *)buffers + choicesCount * sizeof(char *));
	rendererDrivers[0] = rendererDriversData;
	snprintf(rendererDriversData, driverNameMaxLenWithNull, "auto");
	rendererDriversData += driverNameMaxLenWithNull;

	SDL_RendererInfo sdlRendererInfo;
	for (int driverIndex = 0; driverIndex < numDrivers; driverIndex++) {
		SDL_GetRenderDriverInfo(driverIndex, &sdlRendererInfo);
		rendererDrivers[driverIndex + 1] = rendererDriversData;
		snprintf(rendererDriversData, driverNameMaxLenWithNull, "%s", sdlRendererInfo.name ? sdlRendererInfo.name : "unknown");
		rendererDriversData += driverNameMaxLenWithNull;
	}
	rendererDrivers[numDrivers + 1] = NULL;
	return (const char **)rendererDrivers;
}
#endif

void CVort_gui_deleteScreenResolutionBuffers(void) {
	free(guiVideoModes);
}

extern void CVort_gui_setChoicesBuffer(GUI_Menu_Item_T *item, const char **choices);
inline void CVort_gui_setChoicesBuffer(GUI_Menu_Item_T *item, const char **choices) {
	item->choices = choices;
	item->numOfChoices = getNumOfStrings(choices);
}

/* Prepares choices arrays for menu items, and picks current choices */
void CVort_gui_prepareMenuItemsChoiceBuffers(void) {
	static const char *booleanStrChoices[] = {"Off", "On", 0};
	static const int numOfBooleanChoices = 2;

	/*** Game launch parameter ***/

	// Pass keys to BIOS message? (Has an effect on some real DOS setups.)
	CVort_gui_setChoicesBuffer(&guiMenuItemKeysPassToBiosToggle, booleanStrChoices);
	guiMenuItemKeysPassToBiosToggle.value = engine_arguments.passKeysToBios ? 0 : 1;

	/*** Video settings ***/

	// Fullscreen resolution
	int maxLength = CVort_gui_createScreenResolutionBuffers(), currChoiceIndex;
	const char **modeStrPtr;
	for (modeStrPtr = guiFullScreenModeStrs, currChoiceIndex = 1; *modeStrPtr; modeStrPtr++, currChoiceIndex++) {
		if ((guiVideoModes[currChoiceIndex-1].w == engine_arguments.fullWidth) && (guiVideoModes[currChoiceIndex-1].h == engine_arguments.fullHeight)) {
			break;
		}
	}
	if (!(*modeStrPtr)) {
		currChoiceIndex = 0;
	}
	CVort_gui_setChoicesBuffer(&guiMenuItemFullres, guiFullScreenModeStrs);
	guiMenuItemFullres.value = currChoiceIndex;
	// Windowed resolution
	for (modeStrPtr = guiWindowedModeStrs, currChoiceIndex = 1; *modeStrPtr; modeStrPtr++, currChoiceIndex++) {
		if ((guiVideoModes[currChoiceIndex-1].w == engine_arguments.windowWidth) && (guiVideoModes[currChoiceIndex-1].h == engine_arguments.windowHeight)) {
			break;
		}
	}
	if (!(*modeStrPtr)) {
		currChoiceIndex = 0;
	}
	CVort_gui_setChoicesBuffer(&guiMenuItemWindowres, guiWindowedModeStrs);
	guiMenuItemWindowres.value = currChoiceIndex;
	// Fullscreen?
	CVort_gui_setChoicesBuffer(&guiMenuItemFullscreenToggle, booleanStrChoices);
	guiMenuItemFullscreenToggle.value = engine_arguments.isFullscreen ? 1 : 0;
#if SDL_VERSION_ATLEAST(2,0,0)
	// Display number
	maxLength = CVort_gui_createDisplayNumberBuffers();
	CVort_gui_setChoicesBuffer(&guiMenuItemDisplayNumber, guiDisplayNumStrs);
	guiMenuItemDisplayNumber.value = ((engine_arguments.displayNumber >= 0) && (engine_arguments.displayNumber < SDL_GetNumVideoDisplays())) ? engine_arguments.displayNumber : 0;
#endif
	// Output method
	static const char *videoOutputStrs[] = {
	"Surface",
#if SDL_VERSION_ATLEAST(2,0,0)
	"SDL renderer",
#else
	"Overlay",
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	"OpenGL",
#endif
	0
	};

	CVort_gui_setChoicesBuffer(&guiMenuItemVideoOutput, videoOutputStrs);
	guiMenuItemVideoOutput.value = engine_arguments.outputSystem;

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	// OpenGL version
	static const char *glVersionStrs[] = {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
	"1.1",
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
	"2.0",
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
	"ES 2.0",
#endif
	0
	};

	CVort_gui_setChoicesBuffer(&guiMenuItemGLVersion, glVersionStrs);
	guiMenuItemGLVersion.value = engine_arguments.outputGLVersion;
#endif // _CHOCOLATE_KEEN_ENABLE_OPENGL_

#if SDL_VERSION_ATLEAST(2,0,0)
	// SDL renderer driver
	CVort_gui_deleteRendererDriverBuffers();
	int numDrivers = SDL_GetNumRenderDrivers();
	if (numDrivers < 0) {
		numDrivers = 0;
	}
	const char **rendererDrivers = CVort_gui_createRendererDriverChoices(numDrivers);
	static const char *fallbackRendererDrivers[] = {"auto", NULL};
	if (!rendererDrivers) {
		rendererDrivers = fallbackRendererDrivers;
	}
	currChoiceIndex = 0;
	CVort_gui_setChoicesBuffer(&guiMenuItemSDLRendererDriver, rendererDrivers);
	if ((engine_arguments.rendererDriverIndex >= -1) && (engine_arguments.rendererDriverIndex < numDrivers)) {
		currChoiceIndex = engine_arguments.rendererDriverIndex + 1;
	}
	guiMenuItemSDLRendererDriver.value = currChoiceIndex;
#endif

	// VSync
	CVort_gui_setChoicesBuffer(&guiMenuItemVSyncToggle, booleanStrChoices);
	guiMenuItemVSyncToggle.value = engine_arguments.vSync ? 1 : 0;

	// Scale type
	static const char *scaleTypeStrs[] = {
	"4:3", "Fill", "Boxed",
	0
	};

	CVort_gui_setChoicesBuffer(&guiMenuItemScaleType, scaleTypeStrs);
	guiMenuItemScaleType.value = engine_arguments.scaleType;

	// Scale factor
	static const char *scaleFactorStrs[] = {
	"Default", "1", "2", "3", "4",
	0
	};

	CVort_gui_setChoicesBuffer(&guiMenuItemScaleFactor, scaleFactorStrs);
	guiMenuItemScaleFactor.value = ((engine_arguments.zoomLevel >= 0) && (engine_arguments.zoomLevel <= 4)) ? engine_arguments.zoomLevel : 0;

	// Bilinear interpolation
	CVort_gui_setChoicesBuffer(&guiMenuItemBilinearToggle, booleanStrChoices);
	guiMenuItemBilinearToggle.value = engine_arguments.bilinearInterpolation ? 1 : 0;

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	// GPU palette cycling
	CVort_gui_setChoicesBuffer(&guiMenuItemPalCyclingToggle, booleanStrChoices);
	guiMenuItemPalCyclingToggle.value = engine_arguments.gpuPaletteCycling ? 1 : 0;
#endif

#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
	// Offscreen rendering
	CVort_gui_setChoicesBuffer(&guiMenuItemOffscreenToggle, booleanStrChoices);
	guiMenuItemOffscreenToggle.value = engine_arguments.offScreenRendering ? 1 : 0;
#endif

	/*** More settings ***/

	// Emulated graphics adapter ("Ega")
	static const char *videoHardwareStrs[] = {
	"VGA, VGA monitor", "EGA, EGA monitor", "EGA, CGA monitor",
	0
	};

	CVort_gui_setChoicesBuffer(&guiMenuItemEmulatedGfxCard, videoHardwareStrs);
	guiMenuItemEmulatedGfxCard.value = engine_arguments.isEmulatedGfxCardVga ? 0 : engine_arguments.isEgaMonitorInUse ? 1 : 2;

	// Autolock mouse cursor in fullscreen?
	CVort_gui_setChoicesBuffer(&guiMenuItemMouseCursorAutoLock, booleanStrChoices);
	guiMenuItemMouseCursorAutoLock.value = engine_arguments.cursorAutoLock ? 1 : 0;

	// Always hide mouse cursor in game?
	CVort_gui_setChoicesBuffer(&guiMenuItemMouseCursorHiding, booleanStrChoices);
	guiMenuItemMouseCursorHiding.value = engine_arguments.alwaysHideCursor ? 1 : 0;

	// Audio sample rate
	static const char *sampleRateStrs[] = {
	"8000", "11025", "12000", "16000", "22050", "24000", "32000", "44100", "48000",
	0
	};
	const char **rateStr;
	for (rateStr = sampleRateStrs, currChoiceIndex = 0; *rateStr; rateStr++, currChoiceIndex++) {
		if (atoi(*rateStr) == engine_arguments.sndSampleRate) {
			break;
		}
	}
	if (!(*rateStr)) {
		currChoiceIndex = 7; // Some random choice
	}

	CVort_gui_setChoicesBuffer(&guiMenuItemAudioSampleRate, sampleRateStrs);
	guiMenuItemAudioSampleRate.value = currChoiceIndex;

	// Disable sound system?
	CVort_gui_setChoicesBuffer(&guiMenuItemSoundSystemToggle, booleanStrChoices);
	guiMenuItemSoundSystemToggle.value = engine_arguments.disableSoundSystem ? 1 : 0;

	/*** Compatibility options ***/

	// Don't force (emulated) keyboard on game launch?
	static const char *initialInputDeviceStrs[] = {
	"Keyboard", "Mouse", "Joystick1", "Joystick2", "Last",
	0
	};
	CVort_gui_setChoicesBuffer(&guiMenuItemInitialInputDeviceToggle, initialInputDeviceStrs);
	guiMenuItemInitialInputDeviceToggle.value = engine_arguments.extras.initialEmulatedInputDevice;

	// Enable "Vorticons Keen demo mode"?
	CVort_gui_setChoicesBuffer(&guiMenuItemVorticonsDemoModeToggle, booleanStrChoices);
	guiMenuItemVorticonsDemoModeToggle.value = engine_arguments.extras.vorticonsDemoModeToggle ? 1 : 0;
}

#if  0
void CVort_gui_widget_default_handler(GUI_Widget_T *widget) {
}

void CVort_gui_page_esc_default_handler() {
}
#endif
