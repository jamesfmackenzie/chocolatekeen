#ifndef _CHOCOLATE_KEEN_GUI_HANDLERS_H_
#define _CHOCOLATE_KEEN_GUI_HANDLERS_H_

#include "ui/gui_types.h"

typedef struct GUI_CurrentEmuEventDetails_Struct {
    EmulatedInput_T emulatedInput;
    int value;
} GUI_CurrentEmuEventDetails_T;

typedef struct GUI_CurrentHostEventDetails_Struct {
    HostInput_T inputT;
    int inputId;
    int inputVal;
} GUI_CurrentHostEventDetails_T;

GUI_Menu_Item_HandlerNonPtr_T
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
    CVort_gui_handler_dontQuit,
    CVort_gui_handler_quit,
    CVort_gui_handler_showVideoSettingsPage,
    CVort_gui_handler_showMoreSettingsPage,
    CVort_gui_handler_showInputMapper,
    CVort_gui_handler_resetMapperSetup,
    CVort_gui_handler_showCompatibilityOptionsPage,
    CVort_gui_handler_pickFullScreenResolution,
    CVort_gui_handler_pickWindowedResolution,
    CVort_gui_handler_toggleFullScreen,
#if SDL_VERSION_ATLEAST(2,0,0)
    CVort_gui_handler_pickDisplayNumber,
#endif
    CVort_gui_handler_pickVideoOutput,
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    CVort_gui_handler_pickGLVersion,
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
    CVort_gui_handler_pickSDLRendererDriver,
#endif
    CVort_gui_handler_toggleVSync,
    CVort_gui_handler_toggleBilinearInterpolation,
    CVort_gui_handler_pickScaleType,
    CVort_gui_handler_pickScaleFactor,
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    CVort_gui_handler_toggleGPUPaletteCycling,
#endif
#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
    CVort_gui_handler_toggleOffScreenRendering,
#endif
    CVort_gui_handler_doEnterInputMapper,
    CVort_gui_handler_doNotEnterInputMapper,
    CVort_gui_handler_doResetMapper,
    CVort_gui_handler_pickGfxCard,
    CVort_gui_handler_toggleMouseCursorAutoLock,
    CVort_gui_handler_toggleMouseCursorHiding,
    CVort_gui_handler_pickSampleRate,
    CVort_gui_handler_toggleSoundSystem,
    CVort_gui_handler_toggleInitialInputDevice,
    CVort_gui_handler_toggleVorticonsDemoMode,
    CVort_gui_handler_prepareToRecordMapperBinding,
    CVort_gui_handler_deleteMapperBinding,
    CVort_gui_handler_showNextMapperBinding,
    CVort_gui_handler_pickMapperBindingModifiers;

void CVort_gui_handler_leaveMapperBindingMenu(void);

extern GUI_CurrentEmuEventDetails_T guiCurrentEmuEventDetails;
extern GUI_CurrentHostEventDetails_T guiCurrentHostEventDetails;
extern MappedInputEvent_T *guiCurrentMappedInputEvent;
extern char guiMapperMenuBindingString[64];

#endif
