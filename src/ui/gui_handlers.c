#include <stdlib.h>

#include "core/globals.h"
#include "ui/gui_runtime.h"

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

GUI_CurrentEmuEventDetails_T guiCurrentEmuEventDetails;

GUI_CurrentHostEventDetails_T guiCurrentHostEventDetails;

MappedInputEvent_T *guiCurrentMappedInputEvent;

char guiMapperMenuBindingString[64];

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
