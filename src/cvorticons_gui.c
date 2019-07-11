#include <assert.h>

#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "cvorticons.h"
// FIXME: Why do we need this here??? (For "unnecessary" SDL 2.0 check)
// Actually cvorticons.h includes that anyway...
#include "SDL.h"

struct GUI_Menu_Item_Struct {
	int x, y, text_y, width, height, choiceMaxPixWidth;
	const char *label;
	const char **choices;
	int value, numOfChoices;
	void (*handler) (struct GUI_Menu_Item_Struct *thisItem); // Called upon activation
};

typedef struct GUI_Menu_Item_Struct GUI_Menu_Item_T;
typedef void GUI_Menu_Item_HandlerNonPtr_T (GUI_Menu_Item_T *);
typedef GUI_Menu_Item_HandlerNonPtr_T *GUI_Menu_Item_Handler_T;

// Function forward declarations
GUI_Menu_Item_HandlerNonPtr_T
	// Main menu
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
	CVort_gui_handler_startKeen1,
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
	CVort_gui_handler_startKeen2,
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
	CVort_gui_handler_startKeen3,
#endif
	CVort_gui_handler_toggleKeyPassToBios,
	CVort_gui_handler_showSettingsPage,
	CVort_gui_handler_askToQuit,
	// Quit menu
	CVort_gui_handler_dontQuit,
	CVort_gui_handler_quit,
	// Settings menu
	CVort_gui_handler_showVideoSettingsPage,
	CVort_gui_handler_showMoreSettingsPage,
	CVort_gui_handler_showInputMapper,
	CVort_gui_handler_resetMapperSetup,
	CVort_gui_handler_showCompatibilityOptionsPage,
	// Video settings menu, page 1
	CVort_gui_handler_pickFullScreenResolution,
	CVort_gui_handler_pickWindowedResolution,
	CVort_gui_handler_toggleFullScreen,
#if SDL_VERSION_ATLEAST(2,0,0)
	CVort_gui_handler_pickDisplayNumber,
#endif
	// Video settings menu, page 2
	CVort_gui_handler_pickVideoOutput,
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	CVort_gui_handler_pickGLVersion,
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
	CVort_gui_handler_pickSDLRendererDriver,
#endif
	CVort_gui_handler_toggleVSync,
	CVort_gui_handler_toggleBilinearInterpolation,
	// Video settings menu, page 3
	CVort_gui_handler_pickScaleType,
	CVort_gui_handler_pickScaleFactor,
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	CVort_gui_handler_toggleGPUPaletteCycling,
#endif
#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
	CVort_gui_handler_toggleOffScreenRendering,
#endif

	// Input mapper warning menu
	CVort_gui_handler_doEnterInputMapper,
	CVort_gui_handler_doNotEnterInputMapper,

	// Mapper reset warning menu
	//CVort_gui_handler_doNotEnterInputMapper, // HACK: "Borrowing" function
	CVort_gui_handler_doResetMapper,

	// More settings menu
	CVort_gui_handler_pickGfxCard,
	CVort_gui_handler_toggleMouseCursorAutoLock,
	CVort_gui_handler_toggleMouseCursorHiding,
	CVort_gui_handler_pickSampleRate,
	CVort_gui_handler_toggleSoundSystem,
	// Compatibility options menu
	CVort_gui_handler_toggleInitialInputDevice,
	CVort_gui_handler_toggleVorticonsDemoMode,
	// Internal mapper menu (for a specific emulated event)
	CVort_gui_handler_prepareToRecordMapperBinding,
	CVort_gui_handler_deleteMapperBinding,
	CVort_gui_handler_showNextMapperBinding,
	//CVort_gui_handler_showPrevMapperBinding,
	//CVort_gui_handler_leaveMapperBindingMenu,
	CVort_gui_handler_pickMapperBindingModifiers;

struct GUI_Menu_Struct {
	int title_x, title_y;
	const char *title;
	int width;
	GUI_Menu_Item_T **items;
	struct GUI_Menu_Struct *backPage, *prevPage, *nextPage;
	// An alternative for backPage (used in the internal mapper menu)
	void (*altBackHandler)(void);
	// Don't draw back button even if there's a back page/handler
	// (but accept input from a dedicated back button like the Escape key).
	// Used when there is already a button in the menu for that.
	bool hideBackButton;
};

typedef struct GUI_Menu_Struct GUI_Menu_T;
GUI_Menu_T *guiCurrentMenuPtr;
GUI_Menu_Item_T **guiCurrentMenuItemSelectionPtr;

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

#if 0
GUI_Menu_Item_T guiMenuItemPrevMapperBinding = {
0, 0, 0, 0, 0, 0, "Previous binding", NULL, 0, 0, &CVort_gui_handler_showPrevMapperBinding
};
#endif

#if 0
GUI_Menu_Item_T guiMenuItemMapperBindingLeaveMenu = {
0, 0, 0, 0, 0, 0, "Leave current menu", NULL, 0, 0, &CVort_gui_handler_leaveMapperBindingMenu
};
#endif

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

void CVort_gui_handler_leaveMapperBindingMenu(void);

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

int getMaxStringLength(const char **stringArray) {
	int maxLength = 0;
	for (; *stringArray; stringArray++) {
		if (maxLength < strlen(*stringArray)) {
			maxLength = strlen(*stringArray);
		}
	}
	return maxLength;
}

int getNumOfStrings(const char **stringArray) {
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

struct GUI_Mapper_Tile_Struct;

// To move between tiles in the UI with a keyboard/d-pad
struct GUI_Mapper_NearTiles_Struct {
	struct GUI_Mapper_Tile_Struct *left, *right, *up, *down;
};

typedef struct GUI_Mapper_NearTiles_Struct GUI_Mapper_NearTiles_T;

struct GUI_Mapper_Tile_Struct {
	int x, y, width, height;
	const char *topLeftLabel, *midLeftLabel, *botLeftLabel;
	struct {
		EmulatedInput_T emulatedInput;
		int value;
	} emuEvent;
	// To move between tiles in the UI with a keyboard/d-pad
	GUI_Mapper_NearTiles_T nearTiles;
};

typedef struct GUI_Mapper_Tile_Struct GUI_Mapper_Tile_T;

struct GUI_Mapper_Page_Struct {
	GUI_Mapper_Tile_T **tiles;
	/* There is never a "back" mapper page, but it's always possible
	 * to leave a mapper page and return to the usual launcher UI.
	 */
	struct GUI_Mapper_Page_Struct *prevPage, *nextPage;
};

typedef struct GUI_Mapper_Page_Struct GUI_Mapper_Page_T;

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
#if 0
	guiMapperHandlerSaveInputMappings = {GUI_MAPPER_TOP_ROW_LEFTMOST_XPOS, GUI_MAPPER_TOP_ROW_TOPMOST_YPOS+5*GUI_MAPPER_SPACED_TILE_HEIGHT,
	                                     GUI_MAPPER_TILE_WIDTH+4*GUI_MAPPER_SPACED_TILE_WIDTH, GUI_MAPPER_TILE_HEIGHT,
	                                     NULL, "Input mapping saving", NULL, {EMULATEDINPUT_HANDLER, INPUTHANDLER_SAVEINPUTMAPPINGS},
	                                     {NULL, NULL, &guiMapperHandlerFullScreen, NULL}
	                                    },
#endif
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
		for (; *label; x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH, label++) {
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
		for (; *label; x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH, label++) {
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
		for (; *label; x+=GUI_MENU_ITEM_TEXT_CHAR_WIDTH, label++) {
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

void CVort_gui_drawBackButton(bool isMouseSelected);
void CVort_gui_drawPrevButton(bool isMouseSelected);
void CVort_gui_drawNextButton(bool isMouseSelected);

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


typedef struct {
	int w, h;
} GUI_VideoMode_T;

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
	strcpy(fullScreenModeStrData, "Desktop");
	*fullScreenModeStrs = fullScreenModeStrData;
	fullScreenModeStrs++;
	fullScreenModeStrData += strlen("Desktop") + 1;
	strcpy(windowedModeStrData, "Default");
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
	const int strLengthUpperBoundWithNull = 3;
	void *buffers = malloc(strLengthUpperBoundWithNull * numOfModes + // Room for display numbers
	                       (numOfModes + 1) * sizeof(char *)          // Pointer table
	);
	char **displayNumStrs = (char **)((uint8_t *)buffers + 3 * numOfModes);
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
	guiMenuItemScaleType.value = engine_arguments.scaleType, scaleTypeStrs;

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

/*** Main page handlers ***/

#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
void CVort_gui_handler_startKeen1(GUI_Menu_Item_T *item) {
	CVort_engine_loadKeen(GAMEVER_KEEN1);
}
#endif

#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
void CVort_gui_handler_startKeen2(GUI_Menu_Item_T *item) {
	CVort_engine_loadKeen(GAMEVER_KEEN2);
}
#endif

#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
void CVort_gui_handler_startKeen3(GUI_Menu_Item_T *item) {
	CVort_engine_loadKeen(GAMEVER_KEEN3);
}
#endif

void CVort_gui_handler_toggleKeyPassToBios(GUI_Menu_Item_T *item) {
	engine_arguments.passKeysToBios = (item->value == 0);
}

void CVort_gui_handler_showSettingsPage(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiSettingsMenu);
}

void CVort_gui_handler_askToQuit(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiQuitMenu);
}

/*** Quit page handlers ***/

void CVort_gui_handler_quit(GUI_Menu_Item_T *item) {
	CVort_engine_shutdown();
	exit(0);
}

void CVort_gui_handler_dontQuit(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiMainMenu);
}

/*** Settings page handlers ***/

void CVort_gui_handler_showVideoSettingsPage(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiVideoSettingsMenus[0]);
}

void CVort_gui_handler_showMoreSettingsPage(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiMoreSettingsMenu);
}

void CVort_gui_mapper_setCurrentPage(GUI_Mapper_Page_T *page);
void CVort_gui_mapper_runLoop(void);

void CVort_gui_handler_showInputMapper(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiInputMapperEntranceWarningMenu);
}

void CVort_gui_handler_resetMapperSetup(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiMapperResetWarningMenu);
}

void CVort_gui_handler_showCompatibilityOptionsPage(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiCompatibilityOptionsMenu);
}

/*** Video settings page 1 handlers ***/

void CVort_gui_handler_pickFullScreenResolution(GUI_Menu_Item_T *item) {
	if (!item->value) {
		engine_arguments.fullWidth = engine_arguments.fullHeight = 0;
	} else {
		engine_arguments.fullWidth = guiVideoModes[item->value-1].w;
		engine_arguments.fullHeight = guiVideoModes[item->value-1].h;
	}
}

void CVort_gui_handler_pickWindowedResolution(GUI_Menu_Item_T *item) {
	if (!item->value) {
		engine_arguments.windowWidth = engine_arguments.windowHeight = 0;
	} else {
		engine_arguments.windowWidth = guiVideoModes[item->value-1].w;
		engine_arguments.windowHeight = guiVideoModes[item->value-1].h;
	}
}

void CVort_gui_handler_toggleFullScreen(GUI_Menu_Item_T *item) {
	engine_arguments.isFullscreen = (item->value == 1);
}

#if SDL_VERSION_ATLEAST(2,0,0)
void CVort_gui_handler_pickDisplayNumber(GUI_Menu_Item_T *item) {
	// ? widget->handlers.select(widget);
	engine_arguments.displayNumber = item->value;
	// Screen resulution lists should be reset!!
	CVort_gui_deleteScreenResolutionBuffers();
	CVort_gui_createScreenResolutionBuffers();

	CVort_gui_setChoicesBuffer(&guiMenuItemFullres, guiFullScreenModeStrs);
	// HACK? updating resolution choice not just in GUI
	engine_arguments.fullWidth = engine_arguments.fullHeight = 0;
	guiMenuItemFullres.value = 0;
	CVort_gui_setChoicesBuffer(&guiMenuItemWindowres, guiWindowedModeStrs);
	// HACK? updating resolution choice not just in GUI
	engine_arguments.windowWidth = engine_arguments.windowHeight = 0;
	guiMenuItemWindowres.value = 0;
}
#endif

/*** Video settings page 2 handlers ***/

void CVort_gui_handler_pickVideoOutput(GUI_Menu_Item_T *item) {
	engine_arguments.outputSystem = (gfxOutputSystem_T)(item->value);
}

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
void CVort_gui_handler_pickGLVersion(GUI_Menu_Item_T *item) {
	engine_arguments.outputGLVersion = (gfxOutputGLVer_T)(item->value);
}
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
void CVort_gui_handler_pickSDLRendererDriver(GUI_Menu_Item_T *item) {
	engine_arguments.rendererDriverIndex = item->value - 1;
}
#endif

void CVort_gui_handler_toggleVSync(GUI_Menu_Item_T *item) {
	engine_arguments.vSync = (item->value == 1);
}

void CVort_gui_handler_toggleBilinearInterpolation(GUI_Menu_Item_T *item) {
	engine_arguments.bilinearInterpolation = (item->value == 1);
}

/*** Video settings page 3 handlers ***/

void CVort_gui_handler_pickScaleType(GUI_Menu_Item_T *item) {
	engine_arguments.scaleType = (gfxScaleType_T)(item->value);
}

void CVort_gui_handler_pickScaleFactor(GUI_Menu_Item_T *item) {
	engine_arguments.zoomLevel = item->value;
}

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
void CVort_gui_handler_toggleGPUPaletteCycling(GUI_Menu_Item_T *item) {
	engine_arguments.gpuPaletteCycling = (item->value == 1);
}
#endif

#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
void CVort_gui_handler_toggleOffScreenRendering(GUI_Menu_Item_T *item) {
	engine_arguments.offScreenRendering = (item->value == 1);
}
#endif

/*** Input mapper warning page handlers ***/

void CVort_gui_handler_doEnterInputMapper(GUI_Menu_Item_T *item) {
	// We give a "break" (but don't quit) to the usual launcher UI loop
	// and enter the mapper UI loop
	CVort_gui_mapper_setCurrentPage(&guiMapperMainLeftKeyPage);
	CVort_gui_mapper_runLoop();
	// The mapper UI is responsible for proper return to the right menu
	// (which may change - say if a key is selected in the mapper)
}

void CVort_gui_handler_doNotEnterInputMapper(GUI_Menu_Item_T *item) {
	CVort_gui_setCurrentMenu(&guiSettingsMenu);
}

/*** Mapper reset warning page handlers ***/

// HACK: "Borrowing" this function from above
//void CVort_gui_handler_doNotEnterInputMapper(GUI_Menu_Item_T *item) ...

void CVort_gui_handler_doResetMapper(GUI_Menu_Item_T *item) {
	CVort_engine_setDefaultInputMappings();
	// Yeah, we should do that separately
	CVort_engine_saveInputMappings();
	// Finally return to the settings menu
	CVort_gui_setCurrentMenu(&guiSettingsMenu);
}

/*** More settings page handlers ***/

void CVort_gui_handler_pickGfxCard(GUI_Menu_Item_T *item) {
	engine_arguments.isEmulatedGfxCardVga = (item->value == 0);
	engine_arguments.isEgaMonitorInUse = (item->value == 1);
	CVort_engine_parseCalculatedEngineArguments();
}

void CVort_gui_handler_toggleMouseCursorAutoLock(GUI_Menu_Item_T *item) {
	engine_arguments.cursorAutoLock = (item->value == 1);
}

void CVort_gui_handler_toggleMouseCursorHiding(GUI_Menu_Item_T *item) {
	engine_arguments.alwaysHideCursor = (item->value == 1);
}

void CVort_gui_handler_pickSampleRate(GUI_Menu_Item_T *item) {
	engine_arguments.sndSampleRate = atoi(item->choices[item->value]);
}

void CVort_gui_handler_toggleSoundSystem(GUI_Menu_Item_T *item) {
	engine_arguments.disableSoundSystem = (item->value == 1);
}

/*** Compatibility options page ***/

// void CVort_gui_page_handler_leaveCompatibilityOptionsPage(void) {}

void CVort_gui_handler_toggleInitialInputDevice(GUI_Menu_Item_T *item) {
	engine_arguments.extras.initialEmulatedInputDevice = (vanillaInitialEmulatedInputDevice_T)(item->value);
}

void CVort_gui_handler_toggleVorticonsDemoMode(GUI_Menu_Item_T *item) {
	engine_arguments.extras.vorticonsDemoModeToggle = (item->value == 1);
}

/*** Internal mapper menu (for a specific emulated event ***/

struct {
	EmulatedInput_T emulatedInput;
	int value;
} guiCurrentEmuEventDetails;

struct {
	HostInput_T inputT;
	int inputId;
	int inputVal;
} guiCurrentHostEventDetails;

MappedInputEvent_T *guiCurrentMappedInputEvent;

static char guiMapperMenuBindingString[38];

void CVort_gui_refreshMapperMenu(void);

void CVort_gui_handler_prepareToRecordMapperBinding(GUI_Menu_Item_T *item) {
	//guiMenuItemMapperBindingLabel.label = "Please press on a key or a button, or\nmove mouse or a joystick axis/hat\n(5 seconds timeout).";
	CVort_gui_setCurrentMenu(&guiMapperBindingRecordingMenu);
	//CVort_engine_gui_clearScreen();
	//CVort_gui_drawCurrentMenu();
	CVort_engine_updateActualDisplay();
	// To make things more similar to the gameplay, temporarily lock cursor
	CVort_engine_toggleCursorLock(true);
	MappedInputEvent_T *newMappedInputEvent = CVort_engine_recordNewInputMapping(guiCurrentEmuEventDetails.emulatedInput, guiCurrentEmuEventDetails.value, 5000, &guiCurrentHostEventDetails.inputT, &guiCurrentHostEventDetails.inputId, &guiCurrentHostEventDetails.inputVal);
	// Unlock cursor
	CVort_engine_toggleCursorLock(false);
	if (newMappedInputEvent) {
		CVort_engine_saveInputMappings();
		guiCurrentMappedInputEvent = newMappedInputEvent;
	}
	CVort_gui_refreshMapperMenu();
}

void CVort_gui_handler_showNextMapperBinding(GUI_Menu_Item_T *item);

void CVort_gui_handler_deleteMapperBinding(GUI_Menu_Item_T *item) {
	CVort_engine_deleteInputMapping(guiCurrentEmuEventDetails.emulatedInput,
	                                guiCurrentEmuEventDetails.value,
	                                guiCurrentHostEventDetails.inputT,
	                                guiCurrentHostEventDetails.inputId,
	                                guiCurrentHostEventDetails.inputVal
	);
	CVort_engine_saveInputMappings();
	guiCurrentMappedInputEvent = CVort_engine_getNextInputMapping(guiCurrentEmuEventDetails.emulatedInput, guiCurrentEmuEventDetails.value,
	                                                              guiCurrentHostEventDetails.inputT,
	                                                              guiCurrentHostEventDetails.inputId,
	                                                              guiCurrentHostEventDetails.inputVal,
	                                                              &guiCurrentHostEventDetails.inputT,
	                                                              &guiCurrentHostEventDetails.inputId,
	                                                              &guiCurrentHostEventDetails.inputVal
	);
	CVort_gui_refreshMapperMenu();
}

void CVort_gui_handler_showNextMapperBinding(GUI_Menu_Item_T *item) {
	guiCurrentMappedInputEvent = CVort_engine_getNextInputMapping(guiCurrentEmuEventDetails.emulatedInput, guiCurrentEmuEventDetails.value,
	                                                              guiCurrentHostEventDetails.inputT,
	                                                              guiCurrentHostEventDetails.inputId,
	                                                              guiCurrentHostEventDetails.inputVal,
	                                                              &guiCurrentHostEventDetails.inputT,
	                                                              &guiCurrentHostEventDetails.inputId,
	                                                              &guiCurrentHostEventDetails.inputVal
	);
	// HACK...
	GUI_Menu_Item_T **menuItemSelectionPtr = guiCurrentMenuItemSelectionPtr;
	CVort_gui_refreshMapperMenu();
	// HACK...
	guiCurrentMenuItemSelectionPtr = menuItemSelectionPtr;
	CVort_engine_gui_clearScreen();
	CVort_gui_drawCurrentMenu();
}

#if 0
void CVort_gui_handler_showPrevMapperBinding(GUI_Menu_Item_T *item) {
}
#endif

void CVort_gui_handler_leaveMapperBindingMenu(void/*GUI_Menu_Item_T *item*/) {
	// We return to the mapper UI loop (but don't leave the usual loop!)
	CVort_engine_gui_clearScreen();
	CVort_gui_mapper_drawCurrentPage();
	CVort_gui_mapper_runLoop();
}

void CVort_gui_handler_pickMapperBindingModifiers(GUI_Menu_Item_T *item) {
	guiCurrentMappedInputEvent->modMask = item->value;
	// VERY IMPORTANT!
	CVort_engine_saveInputMappings();
}

/*** End of handlers list ***/

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

// Code duplication, but small for now...
void CVort_gui_mapper_resetStatus(void) {
	guiCurrentMapperStatus.waitForMouseButtonRelease = false;
	guiCurrentMapperStatus.isBackButtonMouseSelected = guiCurrentMapperStatus.isPrevButtonMouseSelected = guiCurrentMapperStatus.isNextButtonMouseSelected = false;
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


int CVort_gui_transformMouseCoordinates(int *xPtr, int *yPtr) {
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
#if 0
		} else if ((pressX >= (*guiCurrentMenuItemSelectionPtr)->x + (guiCurrentMenuPtr->width - (*guiCurrentMenuItemSelectionPtr)->choiceMaxPixWidth/2 - 2*GUI_MENU_ITEM_TEXT_CHAR_WIDTH)) &&
		           (releaseX >= (*guiCurrentMenuItemSelectionPtr)->x + (guiCurrentMenuPtr->width - (*guiCurrentMenuItemSelectionPtr)->choiceMaxPixWidth/2 - 2*GUI_MENU_ITEM_TEXT_CHAR_WIDTH))) {
			CVort_gui_selectNextItemChoice(*guiCurrentMenuItemSelectionPtr);
		}
#endif
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
		SDL_Delay(1);
	}
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
		SDL_Delay(1);
	} while (guiMapperLoopRunningStatus);
}
