/*****************************************************
Menus follow with menu items, but without some details
like choices arrays to be filled during runtime.
*****************************************************/

// Main menu

#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
GUI_Menu_Item_T guiMenuItemStartKeen1 = {
0, 0, 0, 0, 0, 0, "Play Keen 1 v1.31", NULL, 0, 0, &CVort_gui_handler_startKeen1
};
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
GUI_Menu_Item_T guiMenuItemStartKeen2 = {
0, 0, 0, 0, 0, 0, "Play Keen 2 v1.31", NULL, 0, 0, &CVort_gui_handler_startKeen2
};
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
GUI_Menu_Item_T guiMenuItemStartKeen3 = {
0, 0, 0, 0, 0, 0, "Play Keen 3 v1.31", NULL, 0, 0, &CVort_gui_handler_startKeen3
};
#endif

// In case no episode is found...
GUI_Menu_Item_T guiMenuItemNoKeenGameEpisode = {
0, 0, 0, 0, 0, 0, "No supported Keen episode found", NULL, 0, 0, NULL
};

GUI_Menu_Item_T guiMenuItemKeysPassToBiosToggle = {
0, 0, 0, 0, 0, 0, "Add vanilla Keen cmdline arg /K", NULL, 0, 0, &CVort_gui_handler_toggleKeyPassToBios
};

GUI_Menu_Item_T guiMenuItemSettings = {
0, 0, 0, 0, 0, 0, "Settings", NULL, 0, 0, &CVort_gui_handler_showSettingsPage
};

GUI_Menu_Item_T guiMenuItemQuit = {
0, 0, 0, 0, 0, 0, "Quit", NULL, 0, 0, &CVort_gui_handler_askToQuit
};

extern GUI_Menu_T guiQuitMenu;

GUI_Menu_T guiMainMenu = {
	0, 0, "Chocolate Keen Launcher", 0,
	(GUI_Menu_Item_T *[])
	{
	// In practice the array is dynamically filled based on
	// available episodes, but we list everything here for reference.
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
	&guiMenuItemStartKeen1,
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
	&guiMenuItemStartKeen2,
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
	&guiMenuItemStartKeen3,
#endif
	&guiMenuItemKeysPassToBiosToggle,
	&guiMenuItemSettings,
	&guiMenuItemQuit,
	0
	},
	&guiQuitMenu, NULL, NULL, 0, false
};

// Quit menu

GUI_Menu_Item_T guiMenuItemConfirmQuit = {
0, 0, 0, 0, 0, 0, "Yes", NULL, 0, 0, &CVort_gui_handler_quit
};

GUI_Menu_Item_T guiMenuItemDontQuit = {
0, 0, 0, 0, 0, 0, "No", NULL, 0, 0, &CVort_gui_handler_dontQuit
};

GUI_Menu_T guiQuitMenu = {
	0, 0, "Are you sure you want to quit?", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemConfirmQuit,
	&guiMenuItemDontQuit,
	0
	},
	&guiMainMenu, NULL, NULL, 0, true
};

// Settings menu

GUI_Menu_Item_T guiMenuItemVideoSettings = {
0, 0, 0, 0, 0, 0, "Video Settings", NULL, 0, 0, &CVort_gui_handler_showVideoSettingsPage
};

GUI_Menu_Item_T guiMenuItemMoreSettings = {
0, 0, 0, 0, 0, 0, "More Settings", NULL, 0, 0, &CVort_gui_handler_showMoreSettingsPage
};

GUI_Menu_Item_T guiMenuItemInputMapper = {
0, 0, 0, 0, 0, 0, "Input Mapper", NULL, 0, 0, &CVort_gui_handler_showInputMapper
};

GUI_Menu_Item_T guiMenuItemResetMapperSetup = {
0, 0, 0, 0, 0, 0, "Reset Mapper Setup", NULL, 0, 0, &CVort_gui_handler_resetMapperSetup
};

GUI_Menu_Item_T guiMenuItemCompatibilityOptions = {
0, 0, 0, 0, 0, 0, "Compatibility Options", NULL, 0, 0, &CVort_gui_handler_showCompatibilityOptionsPage
};

GUI_Menu_T guiSettingsMenu = {
	0, 0, "Settings", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemVideoSettings,
	&guiMenuItemMoreSettings,
	&guiMenuItemInputMapper,
	&guiMenuItemResetMapperSetup,
	&guiMenuItemCompatibilityOptions,
	0
	},
	&guiMainMenu, NULL, NULL, 0, false
};

// Video settings menu, page 1

GUI_Menu_Item_T guiMenuItemFullres = {
0, 0, 0, 0, 0, 0, "Fullscreen resolution*", NULL, 0, 0, &CVort_gui_handler_pickFullScreenResolution
};

GUI_Menu_Item_T guiMenuItemWindowres = {
0, 0, 0, 0, 0, 0, "Windowed resolution*", NULL, 0, 0, &CVort_gui_handler_pickWindowedResolution
};

GUI_Menu_Item_T guiMenuItemFullscreenToggle = {
0, 0, 0, 0, 0, 0, "Fullscreen", NULL, 0, 0, &CVort_gui_handler_toggleFullScreen
};

#if SDL_VERSION_ATLEAST(2,0,0)
GUI_Menu_Item_T guiMenuItemDisplayNumber = {
0, 0, 0, 0, 0, 0, "Display number", NULL, 0, 0, &CVort_gui_handler_pickDisplayNumber
};
#endif

GUI_Menu_Item_T guiMenuItemResolutionComment = {
0, 0, 0, 0, 0, 0, "* This is the resolution after scaling,\nand it may differ in practice.",
NULL, 0, 0, NULL
};

// Video settings menu, page 2

GUI_Menu_Item_T guiMenuItemVideoOutput = {
0, 0, 0, 0, 0, 0, "Output method**", NULL, 0, 0, &CVort_gui_handler_pickVideoOutput
};

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
GUI_Menu_Item_T guiMenuItemGLVersion = {
0, 0, 0, 0, 0, 0, "OpenGL version**", NULL, 0, 0, &CVort_gui_handler_pickGLVersion
};
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
GUI_Menu_Item_T guiMenuItemSDLRendererDriver = {
0, 0, 0, 0, 0, 0, "SDL renderer driver**", NULL, 0, 0, &CVort_gui_handler_pickSDLRendererDriver
};

GUI_Menu_Item_T guiMenuItemVSyncToggle = {
0, 0, 0, 0, 0, 0, "Sync to VBlank", NULL, 0, 0, &CVort_gui_handler_toggleVSync
};
#else

GUI_Menu_Item_T guiMenuItemVSyncToggle = {
0, 0, 0, 0, 0, 0, "Sync to VBlank***", NULL, 0, 0, &CVort_gui_handler_toggleVSync
};

#endif

GUI_Menu_Item_T guiMenuItemRequiredRestartComment = {
0, 0, 0, 0, 0, 0,
#if SDL_VERSION_ATLEAST(2,0,0)
"** May require a restart",
#else
"** May require a restart\n*** May require skipping the launcher",
#endif
NULL, 0, 0, NULL
};

// Video settings menu, page 3

GUI_Menu_Item_T guiMenuItemScaleType = {
0, 0, 0, 0, 0, 0, "Scale type", NULL, 0, 0, &CVort_gui_handler_pickScaleType
};

GUI_Menu_Item_T guiMenuItemScaleFactor = {
0, 0, 0, 0, 0, 0, "Scale factor", NULL, 0, 0, &CVort_gui_handler_pickScaleFactor
};

GUI_Menu_Item_T guiMenuItemBilinearToggle = {
0, 0, 0, 0, 0, 0, "Bilinear interpolation", NULL, 0, 0, &CVort_gui_handler_toggleBilinearInterpolation
};

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
GUI_Menu_Item_T guiMenuItemPalCyclingToggle = {
0, 0, 0, 0, 0, 0, "GPU pal cycling (GL (ES) 2.0)", NULL, 0, 0, &CVort_gui_handler_toggleGPUPaletteCycling
};
#endif

#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
GUI_Menu_Item_T guiMenuItemOffscreenToggle = {
0, 0, 0, 0, 0, 0, "Offscreen rendering", NULL, 0, 0, &CVort_gui_handler_toggleOffScreenRendering
};
#endif

GUI_Menu_T guiVideoSettingsMenus[] = {
	{ // Page 1
	0, 0, "Video Settings", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemFullres,
	&guiMenuItemWindowres,
	&guiMenuItemFullscreenToggle,
#if SDL_VERSION_ATLEAST(2,0,0)
	&guiMenuItemDisplayNumber,
#endif
	&guiMenuItemResolutionComment,
	0
	},
	&guiSettingsMenu, NULL, &guiVideoSettingsMenus[1]
	},
	{ // Page 2
	0, 0, "Video Settings", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemVideoOutput,
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	&guiMenuItemGLVersion,
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
	&guiMenuItemSDLRendererDriver,
#endif
	&guiMenuItemVSyncToggle,
	&guiMenuItemRequiredRestartComment,
	0
	},
	&guiSettingsMenu, &guiVideoSettingsMenus[0], &guiVideoSettingsMenus[2]
	},
	{ // Page 3
	0, 0, "Video Settings", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemScaleType,
	&guiMenuItemScaleFactor,
	&guiMenuItemBilinearToggle,
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	&guiMenuItemPalCyclingToggle,
#endif
#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
	&guiMenuItemOffscreenToggle,
#endif
	0
	},
	&guiSettingsMenu, &guiVideoSettingsMenus[1], NULL
	},
	0,
	false
};

// Input mapper warning menu

GUI_Menu_Item_T guiMenuItemInputMapperEntranceWarningText = {
0, 0, 0, 0, 0, 0, "You should know what you're doing here.\nIf things don't work then loading a\n(non)-fullscreen launcher may help.", NULL, 0, 0, NULL
};

GUI_Menu_Item_T guiMenuItemInputMapperDoEnter = {
0, 0, 0, 0, 0, 0, "Continue", NULL, 0, 0, &CVort_gui_handler_doEnterInputMapper
};

GUI_Menu_Item_T guiMenuItemInputMapperCancel = {
0, 0, 0, 0, 0, 0, "Cancel", NULL, 0, 0, &CVort_gui_handler_doNotEnterInputMapper
};

GUI_Menu_T guiInputMapperEntranceWarningMenu = {
	0, 0, "Warning", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemInputMapperEntranceWarningText,
	&guiMenuItemInputMapperDoEnter,
	&guiMenuItemInputMapperCancel,
	0
	},
	&guiSettingsMenu, NULL, NULL, 0, true
};

// Mapper reset warning menu

GUI_Menu_Item_T guiMenuItemMapperResetWarningText = {
0, 0, 0, 0, 0, 0, "Are you sure you want to do that? Your\ninput mapping setup will be removed!", NULL, 0, 0, NULL
};

// HACK: We can "borrow" CVort_gui_handler_doNotEnterInputMapper here
GUI_Menu_Item_T guiMenuItemMapperResetCancel = {
0, 0, 0, 0, 0, 0, "Do not reset current mapping setup", NULL, 0, 0, &CVort_gui_handler_doNotEnterInputMapper
};

GUI_Menu_Item_T guiMenuItemMapperResetConfirm = {
0, 0, 0, 0, 0, 0, "Reset anyway", NULL, 0, 0, &CVort_gui_handler_doResetMapper
};

GUI_Menu_T guiMapperResetWarningMenu = {
	0, 0, "Warning", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemMapperResetWarningText,
	&guiMenuItemMapperResetCancel,
	&guiMenuItemMapperResetConfirm,
	0
	},
	&guiSettingsMenu, NULL, NULL, 0, true
};

// More settings menu

GUI_Menu_Item_T guiMenuItemEmulatedGfxCard = {
0, 0, 0, 0, 0, 0, "Emulated gfx", NULL, 0, 0, &CVort_gui_handler_pickGfxCard
};

GUI_Menu_Item_T guiMenuItemMouseCursorAutoLock = {
0, 0, 0, 0, 0, 0, "Autolock cursor in fullscreen", NULL, 0, 0, &CVort_gui_handler_toggleMouseCursorAutoLock
};

GUI_Menu_Item_T guiMenuItemMouseCursorHiding = {
0, 0, 0, 0, 0, 0, "Always hide cursor in game", NULL, 0, 0, &CVort_gui_handler_toggleMouseCursorHiding
};

GUI_Menu_Item_T guiMenuItemAudioSampleRate = {
0, 0, 0, 0, 0, 0, "Audio sample rate (in Hz)", NULL, 0, 0, &CVort_gui_handler_pickSampleRate
};

GUI_Menu_Item_T guiMenuItemSoundSystemToggle = {
0, 0, 0, 0, 0, 0, "Disable sound system*", NULL, 0, 0, &CVort_gui_handler_toggleSoundSystem
};

GUI_Menu_Item_T guiMenuItemSoundVanillaBehaviorsComment = {
0, 0, 0, 0, 0, 0, "* May break vanilla Keen behaviors",
NULL, 0, 0, NULL
};

GUI_Menu_T guiMoreSettingsMenu = {
	0, 0, "More Settings", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemEmulatedGfxCard,
	&guiMenuItemMouseCursorAutoLock,
	&guiMenuItemMouseCursorHiding,
	&guiMenuItemAudioSampleRate,
	&guiMenuItemSoundSystemToggle,
	&guiMenuItemSoundVanillaBehaviorsComment,
	0
	},
	&guiSettingsMenu, NULL, NULL, 0, false
};

// Compatibility options menu

GUI_Menu_Item_T guiMenuItemInitialInputDeviceToggle = {
0, 0, 0, 0, 0, 0, "First (emu) input device*", NULL, 0, 0, &CVort_gui_handler_toggleInitialInputDevice
};

GUI_Menu_Item_T guiMenuItemVorticonsDemoModeToggle = {
0, 0, 0, 0, 0, 0, "\"Vorticons Keen demo mode\"", NULL, 0, 0, &CVort_gui_handler_toggleVorticonsDemoMode
};

GUI_Menu_Item_T guiMenuItemEmulatedInputDeviceComment = {
0, 0, 0, 0, 0, 0, "* The (emulated) input device picked\non game launch. In vanilla Keen 1-3,\nthis is a keyboard.",
NULL, 0, 0, NULL
};

GUI_Menu_T guiCompatibilityOptionsMenu = {
	0, 0, "Compatibility options", 0,
	(GUI_Menu_Item_T *[])
	{
	&guiMenuItemInitialInputDeviceToggle,
	&guiMenuItemVorticonsDemoModeToggle,
	&guiMenuItemEmulatedInputDeviceComment,
	0
	},
	&guiSettingsMenu, NULL, NULL, 0, false
};

// Internal mapper menu (for a specific emulated event)
GUI_Menu_Item_T guiMenuItemAddMapperBinding = {
0, 0, 0, 0, 0, 0, "Add new binding", NULL, 0, 0, &CVort_gui_handler_prepareToRecordMapperBinding
};

GUI_Menu_Item_T guiMenuItemDelMapperBinding = {
0, 0, 0, 0, 0, 0, "Delete current binding", NULL, 0, 0, &CVort_gui_handler_deleteMapperBinding
};

GUI_Menu_Item_T guiMenuItemNextMapperBinding = {
0, 0, 0, 0, 0, 0, "Next binding", NULL, 0, 0, &CVort_gui_handler_showNextMapperBinding
};

GUI_Menu_Item_T guiMenuItemMapperBindingLabel = {
0, 0, 0, 0, 0, 0, "To be written later", NULL, 0, 0, NULL
};

// SPECIAL CASE: We initialize here the list, right away
GUI_Menu_Item_T guiMenuItemMapperBindingModifiers = {
0, 0, 0, 0, 0, 0, "Binding modifiers",
(const char *[]){"None", "mod1          ", "     mod2     ", "mod1 mod2     ", "          mod3", "mod1      mod3", "     mod2 mod3", "mod1 mod2 mod3", 0},
0, 8, &CVort_gui_handler_pickMapperBindingModifiers
};

// If there is any binding at all, show all menu items.
// Otherwise we should simply offer to record a new binding or leave menu.

GUI_Menu_Item_T *guiMenuItemsMapperNoBinding[] = {
&guiMenuItemAddMapperBinding,
//&guiMenuItemMapperBindingLeaveMenu,
//&guiMenuItemMapperBindingLabel,
0
};

GUI_Menu_Item_T *guiMenuItemsMapperSomeBindings[] = {
&guiMenuItemAddMapperBinding,
&guiMenuItemDelMapperBinding,
&guiMenuItemNextMapperBinding,
//&guiMenuItemPrevMapperBinding,
&guiMenuItemMapperBindingModifiers,
//&guiMenuItemMapperBindingLeaveMenu,
&guiMenuItemMapperBindingLabel,
0
};

static void CVort_gui_handler_leaveMapperBindingMenu(void);

GUI_Menu_T guiMapperMenu = {
	0, 0, "Current (emulated) event" /* May change */, 0,
	guiMenuItemsMapperNoBinding, /* May change */
	NULL, NULL, NULL, &CVort_gui_handler_leaveMapperBindingMenu, false
};

// An internal mapper static menu (text shown while waiting for a new binding)
GUI_Menu_Item_T guiMenuItemMapperBindingRecordingLabel = {
0, 0, 0, 0, 0, 0, "Please press on a key or a button, or\nmove mouse or a joystick axis/hat\n(5 seconds timeout).",
NULL, 0, 0, NULL
};

GUI_Menu_T guiMapperBindingRecordingMenu = {
	0, 0, "Waiting for user input", 0,
	(GUI_Menu_Item_T *[]){&guiMenuItemMapperBindingRecordingLabel, 0},
	NULL, NULL, NULL, 0, false
};

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
} guiCurrentMenuStatus, guiCurrentMapperStatus;

/*******************
Some mapper UI stuff
*******************/

// Dimensions of a "small" keyboard's key (like 'Q') in pixels,
// with no spacing (With spacing, we can think of 34x34 as the whole size)
#define GUI_MAPPER_TILE_WIDTH 32
#define GUI_MAPPER_TILE_HEIGHT 32
// Same but with spacing
#define GUI_MAPPER_SPACED_TILE_WIDTH 34
#define GUI_MAPPER_SPACED_TILE_HEIGHT 34
// Leftmost position of a tile in top row, without any spacing
// (near the GUI "back" button, HACK)
#define GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS 48
// Topmost position of a tile in top row, without any spacing
#define GUI_MAPPER_TOP_ROW_TOPMOST_YPOS ((GUI_MAPPER_SPACED_TILE_HEIGHT-GUI_MAPPER_TILE_HEIGHT)/2)

// First declare the tiles, because one may reference the other later
extern GUI_Mapper_Tile_T
	/* Main keyboard section, left part */
	// Top row
	guiMapperKeyEsc,
	guiMapperKeyF1,
	guiMapperKeyF2,
	guiMapperKeyF3,
	guiMapperKeyF4,
	guiMapperKeyF5,
	// Number row
	guiMapperKeyGrave,
	guiMapperKey1,
	guiMapperKey2,
	guiMapperKey3,
	guiMapperKey4,
	guiMapperKey5,
	guiMapperKey6,
	guiMapperKey7,
	// QWERTY row
	guiMapperKeyTab,
	guiMapperKeyQ,
	guiMapperKeyW,
	guiMapperKeyE,
	guiMapperKeyR,
	guiMapperKeyT,
	guiMapperKeyY,
	// ASDFGH row
	guiMapperKeyCaps,
	guiMapperKeyA,
	guiMapperKeyS,
	guiMapperKeyD,
	guiMapperKeyF,
	guiMapperKeyG,
	guiMapperKeyH,
	// ZXCVB(N) row
	guiMapperKeyLShift,
	guiMapperKeyLessThan, // Found in 102-keys layout
	guiMapperKeyZ,
	guiMapperKeyX,
	guiMapperKeyC,
	guiMapperKeyV,
	guiMapperKeyB,
	// Bottom row
	guiMapperKeyLCtrl,
	guiMapperKeyLAlt,
	guiMapperKeySpace,
	/* Main keyboard section, right part */
	// Top row
	guiMapperKeyF6,
	guiMapperKeyF7,
	guiMapperKeyF8,
	guiMapperKeyF9,
	guiMapperKeyF10,
	guiMapperKeyF11,
	guiMapperKeyF12,
	// Number row
	guiMapperKey8,
	guiMapperKey9,
	guiMapperKey0,
	guiMapperKeyMinus,
	guiMapperKeyEquals,
	guiMapperKeyBSpace,
	// QWERTY row
	guiMapperKeyU,
	guiMapperKeyI,
	guiMapperKeyO,
	guiMapperKeyP,
	guiMapperKeyLBracket,
	guiMapperKeyRBracket,
	// QWERTY/ASDFGH
	guiMapperKeyEnter,
	// ASDFGH row
	guiMapperKeyJ,
	guiMapperKeyK,
	guiMapperKeyL,
	guiMapperKeySemiColon,
	guiMapperKeyQuote,
	guiMapperKeyBackSlash,
	// ZXCVB(N) row
	guiMapperKeyN,
	guiMapperKeyM,
	guiMapperKeyComma,
	guiMapperKeyPeriod,
	guiMapperKeySlash,
	guiMapperKeyRShift,
	// Bottom row
	guiMapperKeyRAlt,
	guiMapperKeyRCtrl,
	/* The remaining keys */
	// Top row
	guiMapperKeyPrintScreen,
	guiMapperKeyScrollLock,
	guiMapperKeyPause,
	// A group of 6 keys
	guiMapperKeyInsert,
	guiMapperKeyHome,
	guiMapperKeyPageUp,
	guiMapperKeyDelete,
	guiMapperKeyEnd,
	guiMapperKeyPageDown,
	// Arrow keys
	guiMapperKeyUp,
	guiMapperKeyLeft,
	guiMapperKeyDown,
	guiMapperKeyRight,
	// Keypad
	guiMapperKeyNumLock,
	guiMapperKeyKPDivide,
	guiMapperKeyKPMultiply,
	guiMapperKeyKPMinus,
	guiMapperKeyKP7,
	guiMapperKeyKP8,
	guiMapperKeyKP9,
	guiMapperKeyKPPlus,
	guiMapperKeyKP4,
	guiMapperKeyKP5,
	guiMapperKeyKP6,
	guiMapperKeyKP1,
	guiMapperKeyKP2,
	guiMapperKeyKP3,
	guiMapperKeyKPEnter,
	guiMapperKeyKP0,
	guiMapperKeyKPPeriod,
	/* Joysticks section */
	guiMapperJoystick0,
	guiMapperJoystick0StickSpace,
	guiMapperJoystick0Stick,
	guiMapperJoystick0Button0,
	guiMapperJoystick0Button1,
	guiMapperJoystick0Left,
	guiMapperJoystick0Right,
	guiMapperJoystick0Up,
	guiMapperJoystick0Down,
	guiMapperJoystick1,
	guiMapperJoystick1StickSpace,
	guiMapperJoystick1Stick,
	guiMapperJoystick1Button0,
	guiMapperJoystick1Button1,
	guiMapperJoystick1Left,
	guiMapperJoystick1Right,
	guiMapperJoystick1Up,
	guiMapperJoystick1Down,
	/* Mouse section */
	guiMapperMouse,
	guiMapperMouseLeftButton,
	guiMapperMouseMiddleButton,
	guiMapperMouseRightButton,
	guiMapperMouseMotionLeft,
	guiMapperMouseMotionRight,
	guiMapperMouseMotionUp,
	guiMapperMouseMotionDown,
	/* Handlers and modifiers section */
	guiMapperHandlersDescription,
	guiMapperHandlerShutdown,
	guiMapperHandlerCaptureCursor,
	guiMapperHandlerFullScreen,
	//guiMapperHandlerSaveInputMappings,
	guiMapperModifiersDescription,
	guiMapperMod1,
	guiMapperMod2,
	guiMapperMod3;

// Similarly declare the mapper pages
extern GUI_Mapper_Page_T guiMapperMainLeftKeyPage, guiMapperMainRightKeyPage, guiMapperRemainingKeyPage,
       guiMapperJoysticksPage, guiMapperMousePage, guiMapperHandlersAndModifiersPage;

// Now "implement"
GUI_Mapper_Tile_T
	/* Main keyboard section, left part */
	// Top row
	guiMapperKeyEsc = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "Esc", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_ESC},
	                   {NULL, &guiMapperKeyF1, NULL, &guiMapperKeyGrave}
	                  },
	guiMapperKeyF1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F1", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F1},
	                   {&guiMapperKeyEsc, &guiMapperKeyF2, NULL, &guiMapperKey2}
	                 },
	guiMapperKeyF2 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F2", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F2},
	                   {&guiMapperKeyF1, &guiMapperKeyF3, NULL, &guiMapperKey3}
	                 },
	guiMapperKeyF3 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F3", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F3},
	                   {&guiMapperKeyF2, &guiMapperKeyF4, NULL, &guiMapperKey4}
	                 },
	guiMapperKeyF4 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F4", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F4},
	                   {&guiMapperKeyF3, &guiMapperKeyF5, NULL, &guiMapperKey5}
	                 },
	guiMapperKeyF5 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+13*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F5", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F5},
	                   {&guiMapperKeyF4, NULL, NULL, &guiMapperKey6}
	                 },
	// Number row
	guiMapperKeyGrave = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     "~", NULL, "`", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_GRAVE},
	                     {NULL, &guiMapperKey1, &guiMapperKeyEsc, &guiMapperKeyTab}
	                    },
	guiMapperKey1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "!", NULL, "1", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_1},
	                 {&guiMapperKeyGrave, &guiMapperKey2, &guiMapperKeyEsc, &guiMapperKeyTab}
	                },
	guiMapperKey2 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "@", NULL, "2", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_2},
	                 {&guiMapperKey1, &guiMapperKey3, &guiMapperKeyF1, &guiMapperKeyQ}
	                },
	guiMapperKey3 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "#", NULL, "3", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_3},
	                 {&guiMapperKey2, &guiMapperKey4, &guiMapperKeyF2, &guiMapperKeyW}
	                },
	guiMapperKey4 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "$", NULL, "4", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_4},
	                 {&guiMapperKey3, &guiMapperKey5, &guiMapperKeyF3, &guiMapperKeyE}
	                },
	guiMapperKey5 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "%", NULL, "5", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_5},
	                 {&guiMapperKey4, &guiMapperKey6, &guiMapperKeyF4, &guiMapperKeyR}
	                },
	guiMapperKey6 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "^", NULL, "6", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_6},
	                 {&guiMapperKey5, &guiMapperKey7, &guiMapperKeyF5, &guiMapperKeyT}
	                },
	guiMapperKey7 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "&", NULL, "7", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_7},
	                 {&guiMapperKey6, NULL, &guiMapperKeyF5, &guiMapperKeyY}
	                },
	// QWERTY row
	guiMapperKeyTab = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   3*GUI_MAPPER_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "Tab", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_TAB},
	                   {NULL, &guiMapperKeyQ, &guiMapperKeyGrave, &guiMapperKeyCaps}
	                  },
	guiMapperKeyQ = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "Q", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_Q},
	                 {&guiMapperKeyTab, &guiMapperKeyW, &guiMapperKey2, &guiMapperKeyA}
	                },
	guiMapperKeyW = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "W", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_W},
	                 {&guiMapperKeyQ, &guiMapperKeyE, &guiMapperKey3, &guiMapperKeyS}
	                },
	guiMapperKeyE = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "E", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_E},
	                 {&guiMapperKeyW, &guiMapperKeyR, &guiMapperKey4, &guiMapperKeyD}
	                },
	guiMapperKeyR = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+9*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "R", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_R},
	                 {&guiMapperKeyE, &guiMapperKeyT, &guiMapperKey5, &guiMapperKeyF}
	                },
	guiMapperKeyT = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "T", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_T},
	                 {&guiMapperKeyR, &guiMapperKeyY, &guiMapperKey6, &guiMapperKeyG}
	                },
	guiMapperKeyY = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+13*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "Y", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_Y},
	                 {&guiMapperKeyT, NULL, &guiMapperKey7, &guiMapperKeyH}
	                },
	// ASDFGH row
	guiMapperKeyCaps = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   2*GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "Caps", NULL, "Lock", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_CAPSLOCK},
	                   {NULL, &guiMapperKeyA, &guiMapperKeyTab, &guiMapperKeyLShift}
	                  },
	guiMapperKeyA = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "A", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A},
	                   {&guiMapperKeyCaps, &guiMapperKeyS, &guiMapperKeyQ, &guiMapperKeyLessThan}
	                  },
	guiMapperKeyS = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "S", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_S},
	                   {&guiMapperKeyA, &guiMapperKeyD, &guiMapperKeyW, &guiMapperKeyZ}
	                  },
	guiMapperKeyD = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "D", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_D},
	                   {&guiMapperKeyS, &guiMapperKeyF, &guiMapperKeyE, &guiMapperKeyX}
	                  },
	guiMapperKeyF = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "F", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F},
	                   {&guiMapperKeyD, &guiMapperKeyG, &guiMapperKeyR, &guiMapperKeyC}
	                  },
	guiMapperKeyG = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "G", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_G},
	                   {&guiMapperKeyF, &guiMapperKeyH, &guiMapperKeyT, &guiMapperKeyV}
	                  },
	guiMapperKeyH = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "H", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_H},
	                   {&guiMapperKeyG, NULL, &guiMapperKeyY, &guiMapperKeyB}
	                  },
	// ZXCVB(N) row
	guiMapperKeyLShift = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      3*GUI_MAPPER_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                      NULL, "Shift", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_LSHIFT},
	                      {NULL, &guiMapperKeyLessThan, &guiMapperKeyCaps, &guiMapperKeyLCtrl}
	                     },
	guiMapperKeyLessThan = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        ">", NULL, "<", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_LESSTHAN},
	                        {&guiMapperKeyLShift, &guiMapperKeyZ, &guiMapperKeyA, &guiMapperKeyLCtrl}
	                       },
	guiMapperKeyZ = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "Z", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_Z},
	                 {&guiMapperKeyLessThan, &guiMapperKeyX, &guiMapperKeyS, &guiMapperKeyLAlt}
	                },
	guiMapperKeyX = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "X", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_X},
	                 {&guiMapperKeyZ, &guiMapperKeyC, &guiMapperKeyD, &guiMapperKeyLAlt}
	                },
	guiMapperKeyC = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+9*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "C", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_C},
	                 {&guiMapperKeyX, &guiMapperKeyV, &guiMapperKeyF, &guiMapperKeySpace}
	                },
	guiMapperKeyV = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "V", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_V},
	                 {&guiMapperKeyC, &guiMapperKeyB, &guiMapperKeyG, &guiMapperKeySpace}
	                },
	guiMapperKeyB = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+13*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "B", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_B},
	                 {&guiMapperKeyV, NULL, &guiMapperKeyH, &guiMapperKeySpace}
	                },
	// Bottom row
	guiMapperKeyLCtrl = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     3*GUI_MAPPER_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                     NULL, "Ctrl", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_LCTRL},
	                     {NULL, &guiMapperKeyLAlt, &guiMapperKeyLShift, NULL}
	                    },
	guiMapperKeyLAlt = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                    3*GUI_MAPPER_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                    NULL, "Alt", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_LALT},
	                    {&guiMapperKeyLCtrl, &guiMapperKeySpace, &guiMapperKeyZ, NULL}
	                   },
	guiMapperKeySpace = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                    4*GUI_MAPPER_TILE_WIDTH/*HACK*/+6, GUI_MAPPER_TILE_HEIGHT,
	                    NULL, NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_SPACE},
	                    {&guiMapperKeyLAlt, NULL, &guiMapperKeyC, NULL}
	                   },
	/* Main keyboard section, right part */
	// Top row
	guiMapperKeyF6 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                  GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                  NULL, "F6", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F6},
	                  {NULL, &guiMapperKeyF7, NULL, &guiMapperKey8}
	                 },
	guiMapperKeyF7 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                  GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                  NULL, "F7", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F7},
	                  {&guiMapperKeyF6, &guiMapperKeyF8, NULL, &guiMapperKey9}
	                 },
	guiMapperKeyF8 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                  GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                  NULL, "F8", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F8},
	                  {&guiMapperKeyF7, &guiMapperKeyF9, NULL, &guiMapperKey0}
	                 },
	guiMapperKeyF9 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                  GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                  NULL, "F9", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F9},
	                  {&guiMapperKeyF8, &guiMapperKeyF10, NULL, &guiMapperKeyMinus}
	                 },
	guiMapperKeyF10 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+9*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F10", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F10},
	                   {&guiMapperKeyF9, &guiMapperKeyF11, NULL, &guiMapperKeyEquals}
	                  },
	guiMapperKeyF11 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F11", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F11},
	                   {&guiMapperKeyF10, &guiMapperKeyF12, NULL, &guiMapperKeyBSpace}
	                  },
	guiMapperKeyF12 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+13*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "F12", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_F12},
	                   {&guiMapperKeyF11, NULL, NULL, &guiMapperKeyBSpace}
	                  },
	// Number row
	guiMapperKey8 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "*", NULL, "8", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_8},
	                 {NULL, &guiMapperKey9, &guiMapperKeyF6, &guiMapperKeyU}
	                },
	guiMapperKey9 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "(", NULL, "9", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_9},
	                 {&guiMapperKey8, &guiMapperKey0, &guiMapperKeyF7, &guiMapperKeyI}
	                },
	guiMapperKey0 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 ")", NULL, "0", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_0},
	                 {&guiMapperKey9, &guiMapperKeyMinus, &guiMapperKeyF8, &guiMapperKeyO}
	                },
	guiMapperKeyMinus = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     "_", NULL, "-", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_MINUS},
	                     {&guiMapperKey0, &guiMapperKeyEquals, &guiMapperKeyF9, &guiMapperKeyP}
	                    },
	guiMapperKeyEquals = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+9*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                      "+", NULL, "=", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_EQUALS},
	                      {&guiMapperKeyMinus, &guiMapperKeyBSpace, &guiMapperKeyF10, &guiMapperKeyLBracket}
	                     },
	guiMapperKeyBSpace = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                      NULL, "BSpace", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_BSPACE},
	                      {&guiMapperKeyEquals, NULL, &guiMapperKeyF12, &guiMapperKeyEnter}
	                     },
	// QWERTY row
	guiMapperKeyU = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "U", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_U},
	                 {NULL, &guiMapperKeyI, &guiMapperKey8, &guiMapperKeyJ}
	                },
	guiMapperKeyI = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "I", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_I},
	                 {&guiMapperKeyU, &guiMapperKeyO, &guiMapperKey9, &guiMapperKeyK}
	                },
	guiMapperKeyO = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "O", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_O},
	                 {&guiMapperKeyI, &guiMapperKeyP, &guiMapperKey0, &guiMapperKeyL}
	                },
	guiMapperKeyP = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "P", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_P},
	                 {&guiMapperKeyO, &guiMapperKeyLBracket, &guiMapperKeyMinus, &guiMapperKeySemiColon}
	                },
	guiMapperKeyLBracket = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        "{", NULL, "[", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_LBRACKET},
	                        {&guiMapperKeyP, &guiMapperKeyRBracket, &guiMapperKeyEquals, &guiMapperKeyQuote}
	                       },
	guiMapperKeyRBracket = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        "}", NULL, "]", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_RBRACKET},
	                        {&guiMapperKeyLBracket, &guiMapperKeyEnter, &guiMapperKeyBSpace, &guiMapperKeyBackSlash}
	                       },
	// QWERTY/ASDFGH
	guiMapperKeyEnter = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+13*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     "Ent", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_ENTER},
	                     {&guiMapperKeyBackSlash, NULL, &guiMapperKeyBSpace, &guiMapperKeyRShift}
	                    },
	// ASDFGH row
	guiMapperKeyJ = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "J", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_J},
	                 {NULL, &guiMapperKeyK, &guiMapperKeyU, &guiMapperKeyN}
	                },
	guiMapperKeyK = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "K", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_K},
	                 {&guiMapperKeyJ, &guiMapperKeyL, &guiMapperKeyI, &guiMapperKeyM}
	                },
	guiMapperKeyL = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "L", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_L},
	                 {&guiMapperKeyK, &guiMapperKeySemiColon, &guiMapperKeyO, &guiMapperKeyComma}
	                },
	guiMapperKeySemiColon = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                         GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                         ":", NULL, ";", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_SEMICOLON},
	                         {&guiMapperKeyL, &guiMapperKeyQuote, &guiMapperKeyP, &guiMapperKeyPeriod}
	                        },
	guiMapperKeyQuote = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+9*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     "\"", NULL, "'", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_QUOTE},
	                     {&guiMapperKeySemiColon, &guiMapperKeyBackSlash, &guiMapperKeyLBracket, &guiMapperKeySlash}
	                    },
	guiMapperKeyBackSlash = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                         GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                         "|", NULL, "\\", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_BACKSLASH},
	                         {&guiMapperKeyQuote, &guiMapperKeyEnter, &guiMapperKeyRBracket, &guiMapperKeyRShift}
	                        },
	// ZXCVB(N) row
	guiMapperKeyN = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "N", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_N},
	                 {NULL, &guiMapperKeyM, &guiMapperKeyJ, &guiMapperKeyRAlt}
	                },
	guiMapperKeyM = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                 "M", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_M},
	                 {&guiMapperKeyN, &guiMapperKeyComma, &guiMapperKeyK, &guiMapperKeyRAlt}
	                },
	guiMapperKeyComma = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     "<", NULL, ",", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_COMMA},
	                     {&guiMapperKeyM, &guiMapperKeyPeriod, &guiMapperKeyL, &guiMapperKeyRAlt}
	                    },
	guiMapperKeyPeriod = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                      ">", NULL, ".", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_PERIOD},
	                      {&guiMapperKeyComma, &guiMapperKeySlash, &guiMapperKeySemiColon, &guiMapperKeyRAlt}
	                     },
	guiMapperKeySlash = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     "?", NULL, "/", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_SLASH},
	                     {&guiMapperKeyPeriod, &guiMapperKeyRShift, &guiMapperKeyQuote, &guiMapperKeyRAlt}
	                    },
	guiMapperKeyRShift = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     3*GUI_MAPPER_TILE_WIDTH/2+GUI_MAPPER_SPACED_TILE_WIDTH/*HACK*/+1, GUI_MAPPER_TILE_HEIGHT,
	                     NULL, "Shift", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_RSHIFT},
	                     {&guiMapperKeySlash, NULL, &guiMapperKeyEnter, &guiMapperKeyRCtrl}
	                    },
	// Bottom row
	guiMapperKeyRAlt = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                    3*GUI_MAPPER_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                    NULL, "Alt", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_RALT},
	                    {NULL, &guiMapperKeyRCtrl, &guiMapperKeySlash, NULL}
	                   },
	guiMapperKeyRCtrl = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH/*HACK*/+1, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     3*GUI_MAPPER_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                     NULL, "Ctrl", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_RCTRL},
	                     {&guiMapperKeyRAlt, NULL, &guiMapperKeyRShift, NULL}
	                    },
	/* The remaining keys */
	// Top row
	guiMapperKeyPrintScreen = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                           GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                           "Prt", "Scn", "SRq", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_PRINTSCREEN},
	                           {NULL, &guiMapperKeyScrollLock, NULL, &guiMapperKeyInsert}
	                          },
	guiMapperKeyScrollLock = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          "Scl", NULL, "Lck", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_SCROLLLOCK},
	                          {&guiMapperKeyPrintScreen, &guiMapperKeyPause, NULL, &guiMapperKeyHome}
	                         },
	guiMapperKeyPause = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     "Pse", NULL, "Brk", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_PAUSE},
	                     {&guiMapperKeyScrollLock, &guiMapperKeyNumLock, NULL, &guiMapperKeyPageUp}
	                    },
	// A group of 6 keys
	guiMapperKeyInsert = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                      NULL, "Ins", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_INSERT},
	                      {NULL, &guiMapperKeyHome, &guiMapperKeyPrintScreen, &guiMapperKeyDelete}
	                     },
	guiMapperKeyHome = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                    GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                    NULL, "Hme", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_HOME},
	                    {&guiMapperKeyInsert, &guiMapperKeyPageUp, &guiMapperKeyScrollLock, &guiMapperKeyEnd}
	                   },
	guiMapperKeyPageUp = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                      "Pg", NULL, "Up", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_PAGEUP},
	                      {&guiMapperKeyHome, &guiMapperKeyNumLock, &guiMapperKeyPause, &guiMapperKeyPageDown}
	                     },
	guiMapperKeyDelete = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                      NULL, "Del", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_DELETE},
	                      {NULL, &guiMapperKeyEnd, &guiMapperKeyInsert, &guiMapperKeyLeft}
	                     },
	guiMapperKeyEnd = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   NULL, "End", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_END},
	                   {&guiMapperKeyDelete, &guiMapperKeyPageDown, &guiMapperKeyHome, &guiMapperKeyUp}
	                  },
	guiMapperKeyPageDown = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        "Pg", NULL, "Dn", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_PAGEDOWN},
	                        {&guiMapperKeyEnd, &guiMapperKeyKP7, &guiMapperKeyPageUp, &guiMapperKeyRight}
	                       },
	// Arrow keys
	guiMapperKeyUp = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                  GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                  NULL, (const char *)((unsigned char []){24, 0}), NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_UP},
	                  {NULL, &guiMapperKeyKP1, &guiMapperKeyEnd, &guiMapperKeyDown}
	                 },
	guiMapperKeyLeft = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                    GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                    NULL, (const char *)((unsigned char []){27, 0}), NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_LEFT},
	                    {NULL, &guiMapperKeyDown, &guiMapperKeyDelete, NULL}
	                   },
	guiMapperKeyDown = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                    GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                    NULL, (const char *)((unsigned char []){25, 0}), NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_DOWN},
	                    {&guiMapperKeyLeft, &guiMapperKeyRight, &guiMapperKeyUp, NULL}
	                   },
	guiMapperKeyRight = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                     GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                     NULL, (const char *)((unsigned char []){26, 0}), NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_RIGHT},
	                     {&guiMapperKeyDown, &guiMapperKeyKP0, &guiMapperKeyPageDown, NULL}
	                    },
	// Keypad
	guiMapperKeyNumLock = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                       GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                       "Num", NULL, "Lck", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_NUMLOCK},
	                       {&guiMapperKeyPageUp, &guiMapperKeyKPDivide, NULL, &guiMapperKeyKP7}
	                      },
	guiMapperKeyKPDivide = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        NULL, "/", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_DIVIDE},
	                        {&guiMapperKeyNumLock, &guiMapperKeyKPMultiply, NULL, &guiMapperKeyKP8}
	                       },
	guiMapperKeyKPMultiply = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          NULL, "*", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_MULTIPLY},
	                          {&guiMapperKeyKPDivide, &guiMapperKeyKPMinus, NULL, &guiMapperKeyKP9}
	                         },
	guiMapperKeyKPMinus = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                       GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                       NULL, "-", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_MINUS},
	                       {&guiMapperKeyKPMultiply, NULL, NULL, &guiMapperKeyKPPlus}
	                      },
	guiMapperKeyKP7 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "7", NULL, "Hom", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_7},
	                   {&guiMapperKeyPageDown, &guiMapperKeyKP8, &guiMapperKeyNumLock, &guiMapperKeyKP4}
	                  },
	guiMapperKeyKP8 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "8", NULL, (const char *)((unsigned char []){24, 0}), {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_8},
	                   {&guiMapperKeyKP7, &guiMapperKeyKP9, &guiMapperKeyKPDivide, &guiMapperKeyKP5}
	                  },
	guiMapperKeyKP9 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "9", NULL, "PUp", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_9},
	                   {&guiMapperKeyKP8, &guiMapperKeyKPPlus, &guiMapperKeyKPMultiply, &guiMapperKeyKP6}
	                  },
	guiMapperKeyKPPlus = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      NULL, "+", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_PLUS},
	                      {&guiMapperKeyKP9, NULL, &guiMapperKeyKPMinus, &guiMapperKeyKPEnter}
	                     },
	guiMapperKeyKP4 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "4", NULL, (const char *)((unsigned char []){27, 0}), {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_4},
	                   {NULL, &guiMapperKeyKP5, &guiMapperKeyKP7, &guiMapperKeyKP1}
	                  },
	guiMapperKeyKP5 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "5", NULL, NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_5},
	                   {&guiMapperKeyKP4, &guiMapperKeyKP6, &guiMapperKeyKP8, &guiMapperKeyKP2}
	                  },
	guiMapperKeyKP6 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "6", NULL, (const char *)((unsigned char []){26, 0}), {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_6},
	                   {&guiMapperKeyKP5, &guiMapperKeyKPPlus, &guiMapperKeyKP9, &guiMapperKeyKP3}
	                  },
	guiMapperKeyKP1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "1", NULL, "End", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_1},
	                   {&guiMapperKeyUp, &guiMapperKeyKP2, &guiMapperKeyKP4, &guiMapperKeyKP0}
	                  },
	guiMapperKeyKP2 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "2", NULL, (const char *)((unsigned char []){25, 0}), {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_2},
	                   {&guiMapperKeyKP1, &guiMapperKeyKP3, &guiMapperKeyKP5, &guiMapperKeyKP0}
	                  },
	guiMapperKeyKP3 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "3", NULL, "PDn", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_3},
	                   {&guiMapperKeyKP2, &guiMapperKeyKPEnter, &guiMapperKeyKP6, &guiMapperKeyKPPeriod}
	                  },
	guiMapperKeyKPEnter = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                       GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                       NULL, "Ent", NULL, {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_ENTER},
	                       {&guiMapperKeyKPPeriod, NULL, &guiMapperKeyKPPlus, NULL}
	                      },
	guiMapperKeyKP0 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                   GUI_MAPPER_TILE_WIDTH+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                   "0", NULL, "Ins", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_0},
	                   {&guiMapperKeyRight, &guiMapperKeyKPPeriod, &guiMapperKeyKP1, NULL}
	                  },
	guiMapperKeyKPPeriod = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        ".", NULL, "Del", {EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_KP_PERIOD},
	                        {&guiMapperKeyKP0, &guiMapperKeyKPEnter, &guiMapperKeyKP3, NULL}
	                       },
	/* Joysticks section */
	guiMapperJoystick0 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_WIDTH+2*GUI_MAPPER_SPACED_TILE_WIDTH,
	                      "Joystick 0", NULL, NULL, {EMULATEDINPUT_NONE, 0},
	                      {NULL, NULL, NULL, NULL}
	                     },
	// Somewhat hackish
	guiMapperJoystick0StickSpace = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT+12,
	                                3*GUI_MAPPER_SPACED_TILE_WIDTH/2, 3*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                                NULL, NULL, NULL, {EMULATEDINPUT_NONE, 0},
	                                {NULL, NULL, NULL, NULL}
	                               },
	guiMapperJoystick0Stick = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+19, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT+29,
	                           GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_SPACED_TILE_WIDTH/2,
	                           NULL, NULL, NULL, {EMULATEDINPUT_NONE, 0},
	                           {NULL, NULL, NULL, NULL}
	                          },
	guiMapperJoystick0Button0 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                             GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                             NULL, "B0", NULL, {EMULATEDINPUT_JOYBUTTONPRESS, 1},
	                             {&guiMapperJoystick0Button1, &guiMapperJoystick0Right, &guiMapperJoystick0Up, &guiMapperJoystick0Button1}
	                            },
	guiMapperJoystick0Button1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                             GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                             NULL, "B1", NULL, {EMULATEDINPUT_JOYBUTTONPRESS, 2},
	                             {&guiMapperJoystick0Left, &guiMapperJoystick0Button0, &guiMapperJoystick0Button0, &guiMapperJoystick0Down}
	                            },
	guiMapperJoystick0Left = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS-GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          NULL, (const char *)((unsigned char []){27, 0}), NULL, {EMULATEDINPUT_JOYMOTION, -1},
	                          {NULL, &guiMapperJoystick0Button1, NULL, NULL}
	                         },
	guiMapperJoystick0Right = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                           GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                           NULL, (const char *)((unsigned char []){26, 0}), NULL, {EMULATEDINPUT_JOYMOTION, 1},
	                           {&guiMapperJoystick0Button0, &guiMapperJoystick1Up, NULL, &guiMapperJoystick1Left}
	                          },
	guiMapperJoystick0Up = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        NULL, (const char *)((unsigned char []){24, 0}), NULL, {EMULATEDINPUT_JOYMOTION, -2},
	                        {NULL, NULL, NULL, &guiMapperJoystick0Button0}
	                       },
	guiMapperJoystick0Down = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          NULL, (const char *)((unsigned char []){25, 0}), NULL, {EMULATEDINPUT_JOYMOTION, 2},
	                          {NULL, &guiMapperJoystick1Left, &guiMapperJoystick0Button1, NULL}
	                         },
	// Repeat
	guiMapperJoystick1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                      GUI_MAPPER_TILE_WIDTH+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_WIDTH+2*GUI_MAPPER_SPACED_TILE_WIDTH,
	                      "Joystick 1", NULL, NULL, {EMULATEDINPUT_NONE, 0},
	                      {NULL, NULL, NULL, NULL}
	                     },
	//
	guiMapperJoystick1StickSpace = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT+12,
	                                3*GUI_MAPPER_SPACED_TILE_WIDTH/2, 3*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                                NULL, NULL, NULL, {EMULATEDINPUT_NONE, 0},
	                                {NULL, NULL, NULL, NULL}
	                               },
	guiMapperJoystick1Stick = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+19+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT+29,
	                           GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_SPACED_TILE_WIDTH/2,
	                           NULL, NULL, NULL, {EMULATEDINPUT_NONE, 0},
	                           {NULL, NULL, NULL, NULL}
	                          },
	guiMapperJoystick1Button0 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+6*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                             GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                             NULL, "B0", NULL, {EMULATEDINPUT_JOYBUTTONPRESS, 4},
	                             {&guiMapperJoystick1Button1, &guiMapperJoystick1Right, &guiMapperJoystick1Up, &guiMapperJoystick1Button1}
	                            },
	guiMapperJoystick1Button1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                             GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                             NULL, "B1", NULL, {EMULATEDINPUT_JOYBUTTONPRESS, 8},
	                             {&guiMapperJoystick1Left, &guiMapperJoystick1Button0, &guiMapperJoystick1Button0, &guiMapperJoystick1Down}
	                            },
	guiMapperJoystick1Left = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          NULL, (const char *)((unsigned char []){27, 0}), NULL, {EMULATEDINPUT_JOYMOTION, -3},
	                          {&guiMapperJoystick0Down, &guiMapperJoystick1Button1, &guiMapperJoystick0Right, NULL}
	                         },
	guiMapperJoystick1Right = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+7*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                           GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                           NULL, (const char *)((unsigned char []){26, 0}), NULL, {EMULATEDINPUT_JOYMOTION, 3},
	                           {&guiMapperJoystick1Button0, NULL, NULL, NULL}
	                          },
	guiMapperJoystick1Up = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                        GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                        NULL, (const char *)((unsigned char []){24, 0}), NULL, {EMULATEDINPUT_JOYMOTION, -4},
	                        {&guiMapperJoystick0Right, NULL, NULL, &guiMapperJoystick1Button0}
	                       },
	guiMapperJoystick1Down = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+6*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          NULL, (const char *)((unsigned char []){25, 0}), NULL, {EMULATEDINPUT_JOYMOTION, 4},
	                          {NULL, NULL, &guiMapperJoystick1Button1, NULL}
	                         },
	/* Mouse section */
	guiMapperMouse = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                  GUI_MAPPER_TILE_WIDTH+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                  NULL, NULL, "Mouse", {EMULATEDINPUT_NONE, 0},
	                  {NULL, NULL, NULL, NULL}
	                 },
	guiMapperMouseLeftButton = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+2*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                            GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                            NULL, "L", NULL, {EMULATEDINPUT_MOUSEBUTTONPRESS, 1},
	                            {&guiMapperMouseMotionLeft, &guiMapperMouseMiddleButton, NULL, &guiMapperMouseMotionDown}
	                           },
	guiMapperMouseMiddleButton = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                              GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                              NULL, "M", NULL, {EMULATEDINPUT_MOUSEBUTTONPRESS, 4},
	                              {&guiMapperMouseLeftButton, &guiMapperMouseRightButton, &guiMapperMouseMotionUp, &guiMapperMouseMotionDown}
	                             },
	guiMapperMouseRightButton = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                             GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                             NULL, "R", NULL, {EMULATEDINPUT_MOUSEBUTTONPRESS, 2},
	                             {&guiMapperMouseMiddleButton, &guiMapperMouseMotionRight, NULL, &guiMapperMouseMotionDown}
	                            },
	guiMapperMouseMotionLeft = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                            GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                            NULL, (const char *)((unsigned char []){27, 0}), NULL, {EMULATEDINPUT_MOUSEMOTION, -1},
	                            {NULL, &guiMapperMouseLeftButton, &guiMapperMouseLeftButton, NULL}
	                           },
	guiMapperMouseMotionRight = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+5*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                             GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                             NULL, (const char *)((unsigned char []){26, 0}), NULL, {EMULATEDINPUT_MOUSEMOTION, 1},
	                             {&guiMapperMouseRightButton, NULL, &guiMapperMouseRightButton, NULL}
	                            },
	guiMapperMouseMotionUp = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                          GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                          NULL, (const char *)((unsigned char []){24, 0}), NULL, {EMULATEDINPUT_MOUSEMOTION, -2},
	                          {NULL, NULL, NULL, &guiMapperMouseMiddleButton}
	                         },
	guiMapperMouseMotionDown = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+3*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+11*GUI_MAPPER_SPACED_TILE_HEIGHT/2,
	                            GUI_MAPPER_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                            NULL, (const char *)((unsigned char []){25, 0}), NULL, {EMULATEDINPUT_MOUSEMOTION, 2},
	                            {NULL, NULL, &guiMapperMouseMiddleButton, NULL}
	                           },
	/* Handlers and modifiers section */
	guiMapperHandlersDescription = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                                GUI_MAPPER_TILE_WIDTH+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                                NULL, "Handlers:", NULL, {EMULATEDINPUT_NONE, 0},
	                                {NULL, NULL, NULL, NULL}
	                               },
	guiMapperHandlerShutdown = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                            GUI_MAPPER_TILE_WIDTH+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                            NULL, "Shutdown", NULL, {EMULATEDINPUT_HANDLER, INPUTHANDLER_SHUTDOWN},
	                            {NULL, &guiMapperMod1, NULL, &guiMapperHandlerCaptureCursor}
	                           },
	guiMapperHandlerCaptureCursor = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                                 GUI_MAPPER_TILE_WIDTH+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                                 NULL, "Cursor lock toggle", NULL, {EMULATEDINPUT_HANDLER, INPUTHANDLER_CAPTURECURSOR},
	                                 {NULL, &guiMapperMod2, &guiMapperHandlerShutdown, &guiMapperHandlerFullScreen}
	                                },
	guiMapperHandlerFullScreen = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                              GUI_MAPPER_TILE_WIDTH+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                              NULL, "Fullscreen toggle", NULL, {EMULATEDINPUT_HANDLER, INPUTHANDLER_FULLSCREEN},
	                              {NULL, &guiMapperMod3, &guiMapperHandlerCaptureCursor, NULL/*&guiMapperHandlerSaveInputMappings*/}
	                             },
	guiMapperModifiersDescription = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS,
	                                 GUI_MAPPER_TILE_WIDTH+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                                 NULL, "Modifiers:", NULL, {EMULATEDINPUT_NONE, 0},
	                                 {NULL, NULL, NULL, NULL}
	                                },
	guiMapperMod1 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+2*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                 NULL, "Mod 1", NULL, {EMULATEDINPUT_MODTOGGLE, 1},
	                 {&guiMapperHandlerShutdown, NULL, NULL, &guiMapperMod2}
	                },
	guiMapperMod2 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+3*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                 NULL, "Mod 2", NULL, {EMULATEDINPUT_MODTOGGLE, 2},
	                 {&guiMapperHandlerCaptureCursor, NULL, &guiMapperMod1, &guiMapperMod3}
	                },
	guiMapperMod3 = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS+11*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+4*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                 GUI_MAPPER_TILE_WIDTH+3*GUI_MAPPER_SPACED_TILE_WIDTH/2, GUI_MAPPER_TILE_HEIGHT,
	                 NULL, "Mod 3", NULL, {EMULATEDINPUT_MODTOGGLE, 4},
	                 {&guiMapperHandlerFullScreen, NULL, &guiMapperMod2, NULL}
	                };

GUI_Mapper_Page_T guiMapperMainLeftKeyPage = {
	(GUI_Mapper_Tile_T *[])
	{
		// Top row
		&guiMapperKeyEsc,
		&guiMapperKeyF1,
		&guiMapperKeyF2,
		&guiMapperKeyF3,
		&guiMapperKeyF4,
		&guiMapperKeyF5,
		// Number row
		&guiMapperKeyGrave,
		&guiMapperKey1,
		&guiMapperKey2,
		&guiMapperKey3,
		&guiMapperKey4,
		&guiMapperKey5,
		&guiMapperKey6,
		&guiMapperKey7,
		// QWERTY row
		&guiMapperKeyTab,
		&guiMapperKeyQ,
		&guiMapperKeyW,
		&guiMapperKeyE,
		&guiMapperKeyR,
		&guiMapperKeyT,
		&guiMapperKeyY,
		// ASDFGH row
		&guiMapperKeyCaps,
		&guiMapperKeyA,
		&guiMapperKeyS,
		&guiMapperKeyD,
		&guiMapperKeyF,
		&guiMapperKeyG,
		&guiMapperKeyH,
		// ZXCVB(N) row
		&guiMapperKeyLShift,
		&guiMapperKeyLessThan,
		&guiMapperKeyZ,
		&guiMapperKeyX,
		&guiMapperKeyC,
		&guiMapperKeyV,
		&guiMapperKeyB,
		// Bottom row
		&guiMapperKeyLCtrl,
		&guiMapperKeyLAlt,
		&guiMapperKeySpace,
		0
	},
	NULL, &guiMapperMainRightKeyPage
};

GUI_Mapper_Page_T guiMapperMainRightKeyPage = {
	(GUI_Mapper_Tile_T *[])
	{
		// Top row
		&guiMapperKeyF6,
		&guiMapperKeyF7,
		&guiMapperKeyF8,
		&guiMapperKeyF9,
		&guiMapperKeyF10,
		&guiMapperKeyF11,
		&guiMapperKeyF12,
		// Number row
		&guiMapperKey8,
		&guiMapperKey9,
		&guiMapperKey0,
		&guiMapperKeyMinus,
		&guiMapperKeyEquals,
		&guiMapperKeyBSpace,
		// QWERTY row
		&guiMapperKeyU,
		&guiMapperKeyI,
		&guiMapperKeyO,
		&guiMapperKeyP,
		&guiMapperKeyLBracket,
		&guiMapperKeyRBracket,
		// QWERTY/ASDFGH
		&guiMapperKeyEnter,
		// ASDFGH row
		&guiMapperKeyJ,
		&guiMapperKeyK,
		&guiMapperKeyL,
		&guiMapperKeySemiColon,
		&guiMapperKeyQuote,
		&guiMapperKeyBackSlash,
		// ZXCVB(N) row
		&guiMapperKeyN,
		&guiMapperKeyM,
		&guiMapperKeyComma,
		&guiMapperKeyPeriod,
		&guiMapperKeySlash,
		&guiMapperKeyRShift,
		// Bottom row
		&guiMapperKeyRAlt,
		&guiMapperKeyRCtrl,
		0
	},
	&guiMapperMainLeftKeyPage, &guiMapperRemainingKeyPage
};

GUI_Mapper_Page_T guiMapperRemainingKeyPage = {
	(GUI_Mapper_Tile_T *[])
	{
		// Top row
		&guiMapperKeyPrintScreen,
		&guiMapperKeyScrollLock,
		&guiMapperKeyPause,
		// A group of 6 keys
		&guiMapperKeyInsert,
		&guiMapperKeyHome,
		&guiMapperKeyPageUp,
		&guiMapperKeyDelete,
		&guiMapperKeyEnd,
		&guiMapperKeyPageDown,
		// Arrow keys
		&guiMapperKeyUp,
		&guiMapperKeyLeft,
		&guiMapperKeyDown,
		&guiMapperKeyRight,
		// Keypad
		&guiMapperKeyNumLock,
		&guiMapperKeyKPDivide,
		&guiMapperKeyKPMultiply,
		&guiMapperKeyKPMinus,
		&guiMapperKeyKP7,
		&guiMapperKeyKP8,
		&guiMapperKeyKP9,
		&guiMapperKeyKPPlus,
		&guiMapperKeyKP4,
		&guiMapperKeyKP5,
		&guiMapperKeyKP6,
		&guiMapperKeyKP1,
		&guiMapperKeyKP2,
		&guiMapperKeyKP3,
		&guiMapperKeyKPEnter,
		&guiMapperKeyKP0,
		&guiMapperKeyKPPeriod,
		0
	},
	&guiMapperMainRightKeyPage, &guiMapperJoysticksPage
};

GUI_Mapper_Page_T guiMapperJoysticksPage = {
	(GUI_Mapper_Tile_T *[])
	{
		&guiMapperJoystick0,
		&guiMapperJoystick0StickSpace,
		&guiMapperJoystick0Stick,
		&guiMapperJoystick0Button0,
		&guiMapperJoystick0Button1,
		&guiMapperJoystick0Left,
		&guiMapperJoystick0Right,
		&guiMapperJoystick0Up,
		&guiMapperJoystick0Down,
		&guiMapperJoystick1,
		&guiMapperJoystick1StickSpace,
		&guiMapperJoystick1Stick,
		&guiMapperJoystick1Button0,
		&guiMapperJoystick1Button1,
		&guiMapperJoystick1Left,
		&guiMapperJoystick1Right,
		&guiMapperJoystick1Up,
		&guiMapperJoystick1Down,
		0
	},
	&guiMapperRemainingKeyPage, &guiMapperMousePage
};

GUI_Mapper_Page_T guiMapperMousePage = {
	(GUI_Mapper_Tile_T *[])
	{
		&guiMapperMouse,
		&guiMapperMouseLeftButton,
		&guiMapperMouseMiddleButton,
		&guiMapperMouseRightButton,
		&guiMapperMouseMotionLeft,
		&guiMapperMouseMotionRight,
		&guiMapperMouseMotionUp,
		&guiMapperMouseMotionDown,
		0
	},
	&guiMapperJoysticksPage, &guiMapperHandlersAndModifiersPage
};

GUI_Mapper_Page_T guiMapperHandlersAndModifiersPage = {
	(GUI_Mapper_Tile_T *[])
	{
		&guiMapperHandlersDescription,
		&guiMapperHandlerShutdown,
		&guiMapperHandlerCaptureCursor,
		&guiMapperHandlerFullScreen,
		//&guiMapperHandlerSaveInputMappings,
		&guiMapperModifiersDescription,
		&guiMapperMod1,
		&guiMapperMod2,
		&guiMapperMod3,
		0
	},
	&guiMapperMousePage, NULL
};

GUI_Mapper_Page_T *guiCurrentMapperPagePtr;
GUI_Mapper_Tile_T **guiCurrentMapperTilePtr;
