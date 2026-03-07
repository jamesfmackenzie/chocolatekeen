#include <assert.h>

#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "core/globals.h"
// FIXME: Why do we need this here??? (For "unnecessary" SDL 2.0 check)
// Actually cvorticons.h includes that anyway...
#include "SDL.h"
#include "platform/platform.h"
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

static struct {
	bool waitForMouseButtonRelease;
	bool isBackButtonMouseSelected, isPrevButtonMouseSelected, isNextButtonMouseSelected;
} guiCurrentMenuStatus;


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
	const char **rendererDrivers = (const char **)malloc((SDL_GetNumRenderDrivers()+2)*sizeof(char *));
	SDL_RendererInfo sdlRendererInfo;
	rendererDrivers[0] = "auto";
	currChoiceIndex = 0;
	int driverIndex;
	for (driverIndex = 0; driverIndex < SDL_GetNumRenderDrivers(); driverIndex++) {
		SDL_GetRenderDriverInfo(driverIndex, &sdlRendererInfo);
		rendererDrivers[driverIndex+1] = sdlRendererInfo.name;
	}
	rendererDrivers[driverIndex+1] = 0;
	// FIXME: Prevent overflow (cap driver name to, say, 10 chars)
	CVort_gui_setChoicesBuffer(&guiMenuItemSDLRendererDriver, rendererDrivers);
	guiMenuItemSDLRendererDriver.value = engine_arguments.rendererDriverIndex+1;
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

void CVort_gui_setCurrentMenu(GUI_Menu_T *menu);
void CVort_gui_drawCurrentMenu(void);
void CVort_gui_drawMenuItem(GUI_Menu_Item_T *item);

void CVort_gui_refreshMapperMenuWithSomeBinding(void) {
	guiMapperMenu.items = guiMenuItemsMapperSomeBindings;
	guiMenuItemMapperBindingModifiers.value = guiCurrentMappedInputEvent->modMask;
	switch (guiCurrentHostEventDetails.inputT) {
	case HOSTINPUT_KEYPRESS:
#if SDL_VERSION_ATLEAST(2,0,0)
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Key %s (SC %d)", SDL_GetScancodeName(guiCurrentHostEventDetails.inputVal), guiCurrentHostEventDetails.inputVal);
#else
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Key %s (KS %d)", SDL_GetKeyName(guiCurrentHostEventDetails.inputVal), guiCurrentHostEventDetails.inputVal);
#endif
		break;
	case HOSTINPUT_MOUSEBUTTONPRESS:
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse button no. %d", guiCurrentHostEventDetails.inputVal);
		break;
	case HOSTINPUT_MOUSEMOTION:
		switch (guiCurrentHostEventDetails.inputVal) {
		case 1:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement right");
			break;
		case -1:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement left");
			break;
		case 2:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement down");
			break;
		case -2:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement up");
			break;
		}
		break;
	case HOSTINPUT_JOYBUTTONPRESS:
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d button no. %d", guiCurrentHostEventDetails.inputId, guiCurrentHostEventDetails.inputVal);
		break;
	case HOSTINPUT_JOYMOTION:
		if (guiCurrentHostEventDetails.inputVal < 0) {
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d axis %d-", guiCurrentHostEventDetails.inputId, -guiCurrentHostEventDetails.inputVal-1);
		} else {
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d axis %d+", guiCurrentHostEventDetails.inputId, guiCurrentHostEventDetails.inputVal-1);
		}
		break;
	case HOSTINPUT_JOYHAT:
		if (guiCurrentHostEventDetails.inputVal < 0) {
			if (guiCurrentHostEventDetails.inputVal % 2 != 0) {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d left", guiCurrentHostEventDetails.inputId, (-guiCurrentHostEventDetails.inputVal-1)/2);
			} else {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d up", guiCurrentHostEventDetails.inputId, (-guiCurrentHostEventDetails.inputVal-2)/2);
			}
		} else {
			if (guiCurrentHostEventDetails.inputVal % 2 != 0) {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d right", guiCurrentHostEventDetails.inputId, (guiCurrentHostEventDetails.inputVal-1)/2);
			} else {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d down", guiCurrentHostEventDetails.inputId, (guiCurrentHostEventDetails.inputVal-2)/2);
			}
		}
		break;
	default:
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Unknown, this shouldn't happen");
	}
	guiMenuItemMapperBindingLabel.label = guiMapperMenuBindingString;
	CVort_gui_layoutMenu(&guiMapperMenu);
	CVort_gui_setCurrentMenu(&guiMapperMenu);
}

void CVort_gui_refreshMapperMenuWithNoBinding(void) {
	guiMapperMenu.items = guiMenuItemsMapperNoBinding;
	//guiMenuItemMapperBindingLabel.label = "";
	CVort_gui_layoutMenu(&guiMapperMenu);
	CVort_gui_setCurrentMenu(&guiMapperMenu);
}

void CVort_gui_refreshMapperMenu(void) {
	if (guiCurrentMappedInputEvent) {
		CVort_gui_refreshMapperMenuWithSomeBinding();
	} else {
		CVort_gui_refreshMapperMenuWithNoBinding();
	}
}

static char guiMapperMenuTitle[39];

void CVort_gui_showMapperMenu(void) {
	guiCurrentMappedInputEvent = CVort_engine_getNextInputMapping(guiCurrentEmuEventDetails.emulatedInput, guiCurrentEmuEventDetails.value,
	                                                              HOSTINPUT_NONE, 0, 0,
	                                                              &guiCurrentHostEventDetails.inputT,
	                                                              &guiCurrentHostEventDetails.inputId,
	                                                              &guiCurrentHostEventDetails.inputVal
	);
	switch (guiCurrentEmuEventDetails.emulatedInput) {
	case EMULATEDINPUT_KEYPRESS:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", engine_emulatedKeysTable[guiCurrentEmuEventDetails.value].name);
		break;
	case EMULATEDINPUT_MOUSEBUTTONPRESS:
		// 1 is first (left, counting from 0), 2 second (right) and 4 third (middle)
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientMouseButtonNames[guiCurrentEmuEventDetails.value-1-((guiCurrentEmuEventDetails.value&4)>>2)]);
		break;
	case EMULATEDINPUT_MOUSEMOTION:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientMouseAxisNames[(guiCurrentEmuEventDetails.value < 0) ? (-guiCurrentEmuEventDetails.value-1)*2 : (guiCurrentEmuEventDetails.value-1)*2+1]);
		break;
	case EMULATEDINPUT_JOYBUTTONPRESS:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientJoyButtonNames[guiCurrentEmuEventDetails.value]);
		break;
	case EMULATEDINPUT_JOYMOTION:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientJoyAxisNames[(guiCurrentEmuEventDetails.value < 0) ? (-guiCurrentEmuEventDetails.value-1)*2 : (guiCurrentEmuEventDetails.value-1)*2+1]);
		break;
	case EMULATEDINPUT_HANDLER:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientHandlerNames[guiCurrentEmuEventDetails.value-1]);
		break;
	case EMULATEDINPUT_MODTOGGLE:
		// 1 is first (counting from 0), 2 second and 4 third
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientModNames[guiCurrentEmuEventDetails.value-1-((guiCurrentEmuEventDetails.value&4)>>2)]);
		break;
	}
	guiMapperMenu.title = guiMapperMenuTitle;
	CVort_gui_refreshMapperMenu();
}

void CVort_gui_resetMenuStatus(void) {
	guiCurrentMenuStatus.waitForMouseButtonRelease = false;
	guiCurrentMenuStatus.isBackButtonMouseSelected = guiCurrentMenuStatus.isPrevButtonMouseSelected = guiCurrentMenuStatus.isNextButtonMouseSelected = false;
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
	// TODO: Handle new lines?
	const char *txtPtr = guiCurrentMenuPtr->title;
	for (int x = guiCurrentMenuPtr->title_x; *txtPtr; txtPtr++, x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH) {
		CVort_engine_gui_drawFontChar_ptr(x, guiCurrentMenuPtr->title_y, 8, 14, ega_8x14TextFont+112*(unsigned char)(*txtPtr), 11, 1);
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

static void CVort_gui_transformMouseCoordinates(int *xPtr, int *yPtr) {
	(*xPtr) -= engine_screen.dims.borderedViewportRect.x;
	(*yPtr) -= engine_screen.dims.borderedViewportRect.y;
	*xPtr = (*xPtr) * ENGINE_GUI_WIDTH / engine_screen.dims.borderedViewportRect.w;
	*yPtr = (*yPtr) * ENGINE_GUI_HEIGHT / engine_screen.dims.borderedViewportRect.h;
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
		// FIXME: Maybe store some of these offsets in the struct.
		// On the other hand, we don't calculate these that often.
		if ((pressX >= (*guiCurrentMenuItemSelectionPtr)->x + (guiCurrentMenuPtr->width - (*guiCurrentMenuItemSelectionPtr)->choiceMaxPixWidth - 4*GUI_MENU_ITEM_TEXT_CHAR_WIDTH)) &&
		    (pressX  < (*guiCurrentMenuItemSelectionPtr)->x + (guiCurrentMenuPtr->width - (*guiCurrentMenuItemSelectionPtr)->choiceMaxPixWidth/2 - 2*GUI_MENU_ITEM_TEXT_CHAR_WIDTH)) &&
		    (releaseX >= (*guiCurrentMenuItemSelectionPtr)->x + (guiCurrentMenuPtr->width - (*guiCurrentMenuItemSelectionPtr)->choiceMaxPixWidth - 4*GUI_MENU_ITEM_TEXT_CHAR_WIDTH)) &&
		    (releaseX  < (*guiCurrentMenuItemSelectionPtr)->x + (guiCurrentMenuPtr->width - (*guiCurrentMenuItemSelectionPtr)->choiceMaxPixWidth/2 - 2*GUI_MENU_ITEM_TEXT_CHAR_WIDTH))) {
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
	// TODO: Prepare string lists and more
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
#if SDL_VERSION_ATLEAST(2,0,0)
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_ESCAPE:
						CVort_gui_handle_dpad_back();
						//guiCurrentPagePtr->pageEscapeHandler();
						break;
					case SDL_SCANCODE_LEFT:
						CVort_gui_handle_dpad_left();
						break;
					case SDL_SCANCODE_RIGHT:
						CVort_gui_handle_dpad_right();
						break;
					case SDL_SCANCODE_UP:
						CVort_gui_handle_dpad_up();
						break;
					case SDL_SCANCODE_DOWN:
						CVort_gui_handle_dpad_down();
						break;
					case SDL_SCANCODE_RETURN:
						CVort_gui_handle_dpad_activate();
						CVort_gui_resetMenuStatus();
						break;
					case SDL_SCANCODE_PAGEUP:
						CVort_gui_changeToPrevPage();
						break;
					case SDL_SCANCODE_PAGEDOWN:
						CVort_gui_changeToNextPage();
						break;
				}
#else
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						CVort_gui_handle_dpad_back();
						//guiCurrentPagePtr->pageEscapeHandler();
						break;
					case SDLK_LEFT:
						CVort_gui_handle_dpad_left();
						break;
					case SDLK_RIGHT:
						CVort_gui_handle_dpad_right();
						break;
					case SDLK_UP:
						CVort_gui_handle_dpad_up();
						break;
					case SDLK_DOWN:
						CVort_gui_handle_dpad_down();
						break;
					case SDLK_RETURN:
						CVort_gui_handle_dpad_activate();
						CVort_gui_resetMenuStatus();
						break;
					case SDLK_PAGEUP:
						CVort_gui_changeToPrevPage();
						break;
					case SDLK_PAGEDOWN:
						CVort_gui_changeToNextPage();
						break;
				}
#endif
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
					guiCurrentMenuStatus.waitForMouseButtonRelease = true;
					origPointerX = event.button.x;
					origPointerY = event.button.y;
					CVort_gui_transformMouseCoordinates(&origPointerX, &origPointerY);
					GUI_Menu_Item_T **lastItemPtr = guiCurrentMenuItemSelectionPtr;
					if (!(guiCurrentMenuPtr->hideBackButton) && (guiCurrentMenuPtr->backPage || guiCurrentMenuPtr->altBackHandler) && CVort_gui_isBackButtonSelectedByMouse(origPointerX, origPointerY)) {
						guiCurrentMenuStatus.isBackButtonMouseSelected = true;
						if (lastItemPtr) {
							guiCurrentMenuItemSelectionPtr = NULL;
							CVort_gui_drawMenuItem(*lastItemPtr);
						}
						CVort_gui_drawBackButton(guiCurrentMenuStatus.isBackButtonMouseSelected);
						engine_isFrameReadyToDisplay = true;
						break;
					}
					if (guiCurrentMenuPtr->prevPage && CVort_gui_isPrevButtonSelectedByMouse(origPointerX, origPointerY)) {
						guiCurrentMenuStatus.isPrevButtonMouseSelected = true;
						if (lastItemPtr) {
							guiCurrentMenuItemSelectionPtr = NULL;
							CVort_gui_drawMenuItem(*lastItemPtr);
						}
						CVort_gui_drawPrevButton(guiCurrentMenuStatus.isPrevButtonMouseSelected);
						engine_isFrameReadyToDisplay = true;
						break;
					}
					if (guiCurrentMenuPtr->nextPage && CVort_gui_isNextButtonSelectedByMouse(origPointerX, origPointerY)) {
						guiCurrentMenuStatus.isNextButtonMouseSelected = true;
						if (lastItemPtr) {
							guiCurrentMenuItemSelectionPtr = NULL;
							CVort_gui_drawMenuItem(*lastItemPtr);
						}
						CVort_gui_drawNextButton(guiCurrentMenuStatus.isNextButtonMouseSelected);
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
					if (guiCurrentMenuStatus.isBackButtonMouseSelected && CVort_gui_isBackButtonSelectedByMouse(lastPointerX, lastPointerY)) {
						CVort_gui_resetMenuStatus();
						CVort_gui_handle_dpad_back();
						break;
					}
					if (guiCurrentMenuStatus.isPrevButtonMouseSelected && CVort_gui_isPrevButtonSelectedByMouse(lastPointerX, lastPointerY)) {
						CVort_gui_resetMenuStatus();
						CVort_gui_changeToPrevPage();
						break;
					}
					if (guiCurrentMenuStatus.isNextButtonMouseSelected && CVort_gui_isNextButtonSelectedByMouse(lastPointerX, lastPointerY)) {
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
	}
}
