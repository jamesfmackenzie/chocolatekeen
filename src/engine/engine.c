// engine.c: Implements engine for the engine subsystem.

#include <stdio.h>
#include "SDL.h"
#include "SDL_endian.h"

#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "core/globals.h"
#include "platform/platform.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"
#include "third_party/cgenius/fileio/compression/Cunlzexe.h"
#include "engine/engine_config.h"
#include "engine/engine_io.h"
#include "engine/engine_timing.h"

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
#include "SDL_opengl.h"
#endif

/*******************************************************************************
Note: An SDL UYVY Overlay can be used. Here, the pixel format is packed, meaing
that a single 32-bit UYVY value represents two pixels. Each of them has an
individual Y channel value, while the U and V ones are shared.
However, we avoid this by doubling the width and using a single UYVY value per
pixel. This is based on DOSBox behavior described here:
http://vogons.zetafleet.com/viewtopic.php?t=20267
 *******************************************************************************/

const uint32_t engine_egaRGBColorTable[] = {
    0X000000, 0x0000aa, 0x00aa00, 0x00aaaa, 0xaa0000, 0xaa00aa, 0xaa5500, 0xaaaaaa,
    0x555555, 0x5555ff, 0x55ff55, 0x55ffff, 0xff5555, 0xff55ff, 0xffff55, 0xffffff
};

void CVort_engine_parseCalculatedEngineArguments(void) {
    if (engine_arguments.isEmulatedGfxCardVga) {
        engine_arguments.calc.scaledRefreshRate = ENGINE_VGA_REFRESHRATE_SCALED;
	engine_arguments.calc.gfxHeightScalingFactor = 2;
        engine_arguments.calc.overscanGfxLeft = ENGINE_VGA_GFX_OVERSCAN_LEFT;
        engine_arguments.calc.overscanGfxRight = ENGINE_VGA_GFX_OVERSCAN_RIGHT;
        engine_arguments.calc.overscanGfxTop = ENGINE_VGA_GFX_OVERSCAN_TOP_AFTER_DOUBLING;
        engine_arguments.calc.overscanGfxBottom = ENGINE_VGA_GFX_OVERSCAN_BOTTOM_AFTER_DOUBLING;
        engine_arguments.calc.overscanTxtLeft = ENGINE_VGA_TXT_OVERSCAN_LEFT;
        engine_arguments.calc.overscanTxtRight = ENGINE_VGA_TXT_OVERSCAN_RIGHT;
        engine_arguments.calc.overscanTxtTop = ENGINE_VGA_TXT_OVERSCAN_TOP;
        engine_arguments.calc.overscanTxtBottom = ENGINE_VGA_TXT_OVERSCAN_BOTTOM;
        engine_arguments.calc.txtTotalScanHeight = ENGINE_VGA_TOTAL_SCANLINE_COUNT;
        engine_arguments.calc.txtVertRetraceLen = ENGINE_VGA_VERTICAL_RETRACE_LEN;
        engine_arguments.calc.gfxTotalScanHeight = ENGINE_VGA_TOTAL_SCANLINE_COUNT;
        engine_arguments.calc.gfxVertRetraceLen = ENGINE_VGA_VERTICAL_RETRACE_LEN;
        engine_arguments.calc.txtPixelWidth = ENGINE_VGA_TXT_PIXEL_WIDTH;
        engine_arguments.calc.txtPixelHeight = ENGINE_VGA_TXT_PIXEL_HEIGHT;
        engine_arguments.calc.txtCharPixWidth = engine_arguments.calc.txtPixelWidth / ENGINE_EGAVGA_TXT_COLS_NUM;
        engine_arguments.calc.txtCharPixHeight = engine_arguments.calc.txtPixelHeight / ENGINE_EGAVGA_TXT_ROWS_NUM;
        engine_arguments.calc.txtCursorBlinkRate = ENGINE_VGA_CURSOR_BLINK_VERT_FRAME_RATE;
        engine_arguments.calc.txtBlinkRate = ENGINE_VGA_TEXT_BLINK_VERT_FRAME_RATE;
        engine_arguments.calc.txtFontPtr = vga_8x16TextFont;
    } else {
        engine_arguments.calc.scaledRefreshRate = ENGINE_EGA_REFRESHRATE_SCALED;
	engine_arguments.calc.gfxHeightScalingFactor = 1;
        engine_arguments.calc.overscanGfxLeft = ENGINE_EGA_GFX_OVERSCAN_LEFT;
        engine_arguments.calc.overscanGfxRight = ENGINE_EGA_GFX_OVERSCAN_RIGHT;
        engine_arguments.calc.overscanGfxTop = ENGINE_EGA_GFX_OVERSCAN_TOP;
        engine_arguments.calc.overscanGfxBottom = ENGINE_EGA_GFX_OVERSCAN_BOTTOM;
	if (engine_arguments.isEgaMonitorInUse) {
            engine_arguments.calc.overscanTxtLeft = ENGINE_EGA_TXT_OVERSCAN_LEFT;
            engine_arguments.calc.overscanTxtRight = ENGINE_EGA_TXT_OVERSCAN_RIGHT;
            engine_arguments.calc.overscanTxtTop = ENGINE_EGA_TXT_OVERSCAN_TOP;
            engine_arguments.calc.overscanTxtBottom = ENGINE_EGA_TXT_OVERSCAN_BOTTOM;
            engine_arguments.calc.txtPixelWidth = ENGINE_EGA_TXT_PIXEL_WIDTH;
            engine_arguments.calc.txtPixelHeight = ENGINE_EGA_TXT_PIXEL_HEIGHT;
            engine_arguments.calc.txtFontPtr = ega_8x14TextFont;
            engine_arguments.calc.txtTotalScanHeight = ENGINE_EGA_TXT_TOTAL_SCANLINE_COUNT;
            engine_arguments.calc.txtVertRetraceLen = ENGINE_EGA_TXT_VERTICAL_RETRACE_LEN;
        } else {
            engine_arguments.calc.overscanTxtLeft = ENGINE_EGACGA_TXT_OVERSCAN_LEFT;
            engine_arguments.calc.overscanTxtRight = ENGINE_EGACGA_TXT_OVERSCAN_RIGHT;
            engine_arguments.calc.overscanTxtTop = ENGINE_EGACGA_TXT_OVERSCAN_TOP;
            engine_arguments.calc.overscanTxtBottom = ENGINE_EGACGA_TXT_OVERSCAN_BOTTOM;
            engine_arguments.calc.txtPixelWidth = ENGINE_EGACGA_TXT_PIXEL_WIDTH;
            engine_arguments.calc.txtPixelHeight = ENGINE_EGACGA_TXT_PIXEL_HEIGHT;
            engine_arguments.calc.txtFontPtr = cga_8x8TextFont;
            engine_arguments.calc.txtTotalScanHeight = ENGINE_EGACGA_TOTAL_SCANLINE_COUNT;
            engine_arguments.calc.txtVertRetraceLen = ENGINE_EGACGA_VERTICAL_RETRACE_LEN;
	}
        engine_arguments.calc.gfxTotalScanHeight = ENGINE_EGACGA_TOTAL_SCANLINE_COUNT;
        engine_arguments.calc.gfxVertRetraceLen = ENGINE_EGACGA_VERTICAL_RETRACE_LEN;
        engine_arguments.calc.txtCharPixWidth = engine_arguments.calc.txtPixelWidth / ENGINE_EGAVGA_TXT_COLS_NUM;
        engine_arguments.calc.txtCharPixHeight = engine_arguments.calc.txtPixelHeight / ENGINE_EGAVGA_TXT_ROWS_NUM;
        engine_arguments.calc.txtCursorBlinkRate = ENGINE_EGA_CURSOR_BLINK_VERT_FRAME_RATE;
        engine_arguments.calc.txtBlinkRate = ENGINE_EGA_TEXT_BLINK_VERT_FRAME_RATE;
    }
}

static bool process_engine_arguments(int argc, char **argv) {
    // First set internal default
    engine_arguments.fullWidth = engine_arguments.fullHeight = engine_arguments.windowWidth = engine_arguments.windowHeight = 0;
    engine_arguments.zoomLevel = 0;
    engine_arguments.isFullscreen = true;
    engine_arguments.sndSampleRate = ENGINE_SNDDEFAULTSAMPLERATE;
    engine_arguments.disableSoundSystem = false;
    engine_arguments.scaleType = GFX_SCALE_ASPECT;
    engine_arguments.doForceCutFullScreen = false;
    engine_arguments.isEmulatedGfxCardVga = true;
    engine_arguments.cursorAutoLock = true;
    engine_arguments.alwaysHideCursor = false;
    engine_arguments.passKeysToBios = true;

#if SDL_VERSION_ATLEAST(2,0,0)
    engine_arguments.outputSystem = OUTPUTSYS_TEXTURE;
#elif defined _CHOCOLATE_KEEN_ENABLE_OPENGL_
    engine_arguments.outputSystem = OUTPUTSYS_OPENGL;
#else
    engine_arguments.outputSystem = OUTPUTSYS_OVERLAY;
#endif

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
    engine_arguments.outputGLVersion = OUTPUTGL_2_0;
#elif defined _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    engine_arguments.outputGLVersion = OUTPUTGL_1_1;
#elif defined _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
    engine_arguments.outputGLVersion = OUTPUTGL_ES_2_0;
#else
#error "Unknown OpenGL version!"
#endif
#endif // _CHOCOLATE_KEEN_ENABLE_OPENGL_

    engine_arguments.bilinearInterpolation = true;
    engine_arguments.vSync = true;

#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
    engine_arguments.offScreenRendering = true;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    engine_arguments.gpuPaletteCycling = true;
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
    engine_arguments.displayNumber = 0;
    engine_arguments.rendererDriverIndex = -1;
#endif

    engine_arguments.extras.initialEmulatedInputDevice = EMU_INITIAL_INPUT_KEYBOARD;
    engine_arguments.extras.vorticonsDemoModeToggle = false;

    // Then load settings from file
    CVort_engine_loadConfigFile();

    // Just before going over command line arguments
    engine_skipLauncher = true;
#ifdef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    engine_forceSpecificEpisode = true;
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
    engine_gameVersion = GAMEVER_KEEN1;
#elif (defined CHOCOLATE_KEEN_IS_EPISODE2_ENABLED)
    engine_gameVersion = GAMEVER_KEEN2;
#elif (defined CHOCOLATE_KEEN_IS_EPISODE3_ENABLED)
    engine_gameVersion = GAMEVER_KEEN3;
#endif
#else // CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    engine_forceSpecificEpisode = false;
#endif // CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE

    // Now go over command line arguments
    for (int loopVar = 1; loopVar < argc; loopVar++) {
#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
        if (!strcmp(argv[loopVar], "-startkeen1")) {
            engine_gameVersion = GAMEVER_KEEN1;
            engine_forceSpecificEpisode = true;
            continue;
        }
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
        if (!strcmp(argv[loopVar], "-startkeen2")) {
            engine_gameVersion = GAMEVER_KEEN2;
            engine_forceSpecificEpisode = true;
            continue;
        }
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
        if (!strcmp(argv[loopVar], "-startkeen3")) {
            engine_gameVersion = GAMEVER_KEEN3;
            engine_forceSpecificEpisode = true;
            continue;
        }
#endif
#endif	// Not defined CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
        if (!strcmp(argv[loopVar], "-showlauncher")) {
            engine_skipLauncher = false;
            engine_isLauncherFullscreen = false;
            continue;
        }
        if (!strcmp(argv[loopVar], "-showfulllauncher")) {
            engine_skipLauncher = false;
            engine_isLauncherFullscreen = true;
            continue;
        }
        if (*(argv[loopVar]) != '-') {
            return false;
        }
        if (!CVort_engine_parseConfigLine(argv[loopVar]+1)) {
            return false;
        }
    }

#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    if (engine_skipLauncher && !engine_forceSpecificEpisode) {
        int availableEpisodes = 0;
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
        if (CVort_engine_isGameExeAvailable(GAMEVER_KEEN1)) {
            availableEpisodes++;
        }
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
        if (CVort_engine_isGameExeAvailable(GAMEVER_KEEN2)) {
            availableEpisodes++;
        }
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
        if (CVort_engine_isGameExeAvailable(GAMEVER_KEEN3)) {
            availableEpisodes++;
        }
#endif
        if (CK_PlatformShouldAutoShowLauncher(availableEpisodes)) {
            engine_skipLauncher = false;
        }
    }
#endif

    // Finally calculate some additional fields
    CVort_engine_parseCalculatedEngineArguments();
    return true;
}

bool CVort_engine_prepareInstructionsScreen() {
    //arguments.gameVersion = GAMEVER_AUTO;
    engine_arguments.zoomLevel = 1;
    engine_arguments.isFullscreen = false;
    //engine_arguments.disableSoundSystem = true;
    engine_arguments.isEmulatedGfxCardVga = true;
    engine_arguments.outputSystem = OUTPUTSYS_SURFACE;

    engine_arguments.calc.scaledRefreshRate = ENGINE_VGA_REFRESHRATE_SCALED;
    engine_arguments.calc.gfxHeightScalingFactor = 2;
    engine_arguments.calc.overscanGfxLeft = ENGINE_VGA_GFX_OVERSCAN_LEFT;
    engine_arguments.calc.overscanGfxRight = ENGINE_VGA_GFX_OVERSCAN_RIGHT;
    engine_arguments.calc.overscanGfxTop = ENGINE_VGA_GFX_OVERSCAN_TOP_AFTER_DOUBLING;
    engine_arguments.calc.overscanGfxBottom = ENGINE_VGA_GFX_OVERSCAN_BOTTOM_AFTER_DOUBLING;
    engine_arguments.calc.overscanTxtLeft = ENGINE_VGA_TXT_OVERSCAN_LEFT;
    engine_arguments.calc.overscanTxtRight = ENGINE_VGA_TXT_OVERSCAN_RIGHT;
    engine_arguments.calc.overscanTxtTop = ENGINE_VGA_TXT_OVERSCAN_TOP;
    engine_arguments.calc.overscanTxtBottom = ENGINE_VGA_TXT_OVERSCAN_BOTTOM;
    engine_arguments.calc.txtTotalScanHeight = ENGINE_VGA_TOTAL_SCANLINE_COUNT;
    engine_arguments.calc.txtVertRetraceLen = ENGINE_VGA_VERTICAL_RETRACE_LEN;
    engine_arguments.calc.txtPixelWidth = ENGINE_VGA_TXT_PIXEL_WIDTH;
    engine_arguments.calc.txtPixelHeight = ENGINE_VGA_TXT_PIXEL_HEIGHT;
    engine_arguments.calc.txtCharPixWidth = engine_arguments.calc.txtPixelWidth / ENGINE_EGAVGA_TXT_COLS_NUM;
    engine_arguments.calc.txtCharPixHeight = engine_arguments.calc.txtPixelHeight / ENGINE_EGAVGA_TXT_ROWS_NUM;
    engine_arguments.calc.txtCursorBlinkRate = ENGINE_VGA_CURSOR_BLINK_VERT_FRAME_RATE;
    engine_arguments.calc.txtBlinkRate = ENGINE_VGA_TEXT_BLINK_VERT_FRAME_RATE;
    engine_arguments.calc.txtFontPtr = vga_8x16TextFont;

    // We do NOT set windowWidth and windowHeight now.
    // GFX_SCALE_BOXED takes care of the dimensions in its own way.
    engine_arguments.scaleType = GFX_SCALE_BOXED;
    engine_arguments.doForceCutFullScreen = false;

    if (!CVort_engine_setVideoMode(CVORT_VIDEO_MODE_TEXT)) {
        CVort_engine_shutdownSDL();
        return false;
    }

    CVort_engine_puts("Chocolate Keen v0.9.4 (April 29th, 2014), Alpha release");
    CVort_engine_puts("-------------------------------------------------------");
    CVort_engine_puts("");

    return true;
}

static bool clear_keys_and_check_for_quit(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // Clear keys
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
        case SDL_QUIT:
            //CVort_engine_shutdownSDL();
            return true;
        }
    }
    return false;
}

static bool check_for_user_input_and_quit(void) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
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
        case SDL_MOUSEBUTTONDOWN:
        case SDL_JOYBUTTONDOWN:
        case SDL_QUIT:
            //CVort_engine_shutdownSDL();
            return true;
        }
    }
    return false;
}

void CVort_engine_displayCommandLineHelp() {
#if SDL_VERSION_ATLEAST(2,0,0)
    const uint32_t sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER;
#else
    const uint32_t sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#endif
    if (SDL_Init(sdlInitFlags) < 0) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't initialize SDL for displaying command-line arguments: %s\n", SDL_GetError());
        return;
    }

    if (!CVort_engine_prepareScreen()) {
        CVort_engine_shutdownSDL();
        return;
    }

    if (!CVort_engine_prepareInstructionsScreen()) {
        return;
    }
#ifdef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    char lineBuffer[78];
#endif

    CVort_engine_puts("Usage:");
#ifdef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    snprintf(lineBuffer, sizeof(lineBuffer), "chocolate-keen%u <optional args>", CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE);
    CVort_engine_puts(lineBuffer);
#else
    CVort_engine_puts("chocolate-keen <optional args>");
#endif
    CVort_engine_puts("");
#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    CVort_engine_puts("* It is possible to pick a specific game episode as follows:");
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
    CVort_engine_puts("-startkeen1: Load Keen 1 v1.31.");
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
    CVort_engine_puts("-startkeen2: Load Keen 2 v1.31.");
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
    CVort_engine_puts("-startkeen3: Load Keen 3 v1.31.");
#endif
    CVort_engine_puts("");
#endif	// Not defined CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    CVort_engine_puts("* A launcher can be shown by specifying -showlauncher.");
    CVort_engine_puts("It can be used to configure non-vanilla Keen settings.");
    CVort_engine_puts("For a fullscreen launcher -showfulllauncher should be specified.");
    CVort_engine_puts("");
    CVort_engine_puts("* Furthermore, other command line arguments are used to override settings");
    CVort_engine_puts("from the generated chocolate-keen.cfg file.");
    CVort_engine_puts("");
#ifdef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    snprintf(lineBuffer, sizeof(lineBuffer), "Example 1: chocolate-keen%u -fullscreen=false -scalefactor=3", CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE);
    CVort_engine_puts(lineBuffer);
    snprintf(lineBuffer, sizeof(lineBuffer), "Example 2: chocolate-keen%u -showlauncher", CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE);
    CVort_engine_puts(lineBuffer);
#else
    CVort_engine_puts("Example 1: chocolate-keen -startkeen1 -fullscreen=false -scalefactor=3");
    CVort_engine_puts("Example 2: chocolate-keen -showlauncher");
#endif
    if (clear_keys_and_check_for_quit()) {
        CVort_engine_shutdownSDL();
        return;
    }
    do {
        if (check_for_user_input_and_quit()) {
            CVort_engine_shutdownSDL();
            return;
        }
        // We cannot use this here - g_input.key_map is not ready!
        // Furthermore, we are waiting for SDL events directly.
        //engine_shortSleep();
        // But let's use this:
        CVort_engine_updateActualDisplay();
        CK_PlatformSleepMs(1);
    } while (1);
    // We should NEVER reach this stage... so not calling CVort_engine_shutdownSDL().
}

bool CVort_engine_processArguments(int argc, char **argv) {
    if (process_engine_arguments(argc, argv)) {
        return true;
    }
    CVort_engine_displayCommandLineHelp();
    return false;
}

void CVort_engine_initializeKeenVersionInfo(void);

bool CVort_engine_start(void) {
#if SDL_VERSION_ATLEAST(2,0,0)
    // The following should be done BEFORE initializing the SDL video subsystem.

    // We do not really take advantage of 3D acceleration in case a window
    // surface is used. So, disable by default in order to save system RAM.
    //
    // BUT this results in problems for the launcher UI and more now
    //if (engine_arguments.outputSystem == OUTPUTSYS_SURFACE) {
    //    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    // }
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
    const uint32_t sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER;
#else
    const uint32_t sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#endif

    if (SDL_Init(sdlInitFlags)) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    CVort_engine_setupInputMappings();
    SDL_JoystickEventState(SDL_ENABLE);

    if (!CVort_engine_prepareScreen()) {
        return false;
    }

    CVort_engine_initializeKeenVersionInfo();

    if (engine_skipLauncher) {
        CVort_engine_loadKeen(engine_gameVersion);
    }

    if (!CVort_engine_setVideoMode(CVORT_VIDEO_MODE_LAUNCHER)) {
        return false;
    }

    SDL_ShowCursor(SDL_ENABLE);

#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    // Already done in process_engine_arguments otherwise
    engine_forceSpecificEpisode = true;
#endif
    CVort_gui_runLoop();

    return true;
}

void CVort_engine_shutdown() {
    CVort_engine_teardownInputMappings();

    //if (!engine_arguments.disableSoundSystem)
    //    SDL_CloseAudio();

    // The paletted pages were created with a SINGLE allocation command!
    //	free(engine_palettedpages[0]);

    //	SDL_FreeSurface(engine_egapages[0]);
    //	SDL_FreeSurface(engine_egapages[1]);
    CVort_engine_saveConfigFile();
    CVort_engine_shutdownSDL();
}

void CVort_engine_handleQuit() {
    if (clear_keys_and_check_for_quit()) {
        CVort_engine_shutdown();
        exit(0);
    }
    // Now begin for real
    do {
        if (check_for_user_input_and_quit()) {
            CVort_engine_shutdown();
            exit(0);
        }
        // We we are waiting for SDL events
        // directly, so we cannot use this.
        //engine_shortSleep();
        // But let's use this:
        CVort_engine_updateActualDisplay();
        CK_PlatformSleepMs(1);
    } while (1);
}

void CVort_engine_shutdownSDL() {
    SDL_CloseAudio();
    SDL_Quit();
}

int16_t CVort_engine_readGamePort() {
    /* The original call gets game port input, with four potential buttons
     * identified by the INVERTED masks ~16, ~32, ~64 and ~128.
     * This function returns 1 if the first button is pressed.
     * If not, then 2 is returned if the second button is pressed.
     * Otherwise, 0 is returned.
     */
    if (engine_inputMappings.currEmuInputStatus.joystickButtonsMask & 1) return 1;
    if (engine_inputMappings.currEmuInputStatus.joystickButtonsMask & 2) return 2;
    return 0;
}

bool CVort_engine_isJoystickInUse() {
    // Known limitation: this does not reflect runtime device-selection state.
    // We currently preserve legacy behavior and keep joystick logic available.
    return true;
}

void CVort_engine_toggleCursorLock(bool toggle) {
    engine_isCursorLocked = toggle;
#ifndef CHOCOLATE_KEEN_CONFIG_DEBUG
#if SDL_VERSION_ATLEAST(2,0,0)
    //SDL_SetWindowGrab(engine_screen.sdl.window, toggle ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(toggle ? SDL_TRUE : SDL_FALSE);
#else
    SDL_ShowCursor(engine_arguments.alwaysHideCursor ? SDL_DISABLE : (toggle ? SDL_DISABLE : SDL_ENABLE));
    SDL_WM_GrabInput(toggle ? SDL_GRAB_ON : SDL_GRAB_OFF);
#endif
    // If the following is NOT done, when the cursor is re-locked
    // a bit of "random" motion may be in effect right afterwards
    SDL_GetRelativeMouseState(NULL, NULL);
#endif
}

JoystickPoll_T CVort_engine_pollJoystick(int16_t joystickNum) {
    //const int16_t engineJoyId = joystickNum - 1;
    JoystickPoll_T result;
    result.xPoll = engine_inputMappings.currEmuInputStatus.joystickAxesPolls[joystickNum*2-2];
    result.yPoll = engine_inputMappings.currEmuInputStatus.joystickAxesPolls[joystickNum*2-1];
    return result;
}

GameInput_T CVort_engine_getKeybCtrlState() {
    GameInput_T result;
    int16_t horiz = 0, vert = 0;
    if (g_input.key_map[g_input.sc_dir[0]])
        vert = -1;
    if (g_input.key_map[g_input.sc_dir[2]])
        horiz = 1;
    if (g_input.key_map[g_input.sc_dir[4]])
        vert = 1;
    if (g_input.key_map[g_input.sc_dir[6]])
        horiz = -1;
    if (g_input.key_map[g_input.sc_dir[1]]) {
        vert = -1;
        horiz = 1;
    }
    if (g_input.key_map[g_input.sc_dir[7]])
        vert = horiz = -1;
    if (g_input.key_map[g_input.sc_dir[3]])
        vert = horiz = 1;
    if (g_input.key_map[g_input.sc_dir[5]]) {
        vert = 1;
        horiz = -1;
    }
    switch (vert * 3 + horiz + 4) {
        case 0: result.direction = 7;
            break;
        case 1: result.direction = 0;
            break;
        case 2: result.direction = 1;
            break;
        case 3: result.direction = 6;
            break;
        case 4: result.direction = 8;
            break;
        case 5: result.direction = 2;
            break;
        case 6: result.direction = 5;
            break;
        case 7: result.direction = 4;
            break;
        case 8: result.direction = 3;
            break;
        default: break;
    }
    result.but1jump = g_input.key_map[g_input.sc_but1];
    result.but2pogo = g_input.key_map[g_input.sc_but2];
    return result;
}

GameInput_T CVort_engine_getMouseCtrl() {
    GameInput_T result;
    int16_t horiz = 0, vert = 0;
    //int sdlRelativeX = 0, sdlRelativeY = 0;
    //uint8_t buttonMask = engine_isCursorLocked ? SDL_GetRelativeMouseState(&sdlRelativeX, &sdlRelativeY) : 0;

    //engine_mouseCursorPosX += sdlRelativeX;
    //engine_mouseCursorPosY += sdlRelativeY;
    //engine_mouseCursorPosX = engine_inputMappings.currEmuInputStatus.mouseColumn;
    //engine_mouseCursorPosY = engine_inputMappings.currEmuInputStatus.mouseRow;
    /* Step 2: Ensure the cursor is still within the window's bounds */
/*    if (engine_mouseCursorPosX < 0) {
        engine_mouseCursorPosX = 0;
    } else if (engine_mouseCursorPosX >= 2*ENGINE_EGA_GFX_WIDTH) {
        engine_mouseCursorPosX = 2*ENGINE_EGA_GFX_WIDTH - 1;
    }
    if (engine_mouseCursorPosY < 0) {
        engine_mouseCursorPosY = 0;
    } else if (engine_mouseCursorPosY >= ENGINE_EGA_GFX_HEIGHT) {
        engine_mouseCursorPosY = ENGINE_EGA_GFX_HEIGHT - 1;
    }*/
    /* We can now work with these coordinates, just like
    vanilla Keen 1-3. But first, look for button presses. */
    result.but1jump = ((engine_inputMappings.currEmuInputStatus.mouseButtonsMask & 1) == 1);
    result.but2pogo = ((engine_inputMappings.currEmuInputStatus.mouseButtonsMask & 2) == 2);
    /* We are next handling cursor position (for the motion).
    Note that for determining motion, the horizontal range is
    re-scaled so 320 units represent the screen's width.   */
    if ((engine_inputMappings.currEmuInputStatus.mouseColumn - ENGINE_EGA_GFX_WIDTH) / 2 > g_input.mouse_ctrl_1) {
        horiz = 1;
        engine_inputMappings.currEmuInputStatus.mouseColumn -= (g_input.mouse_ctrl_1 << 1);
    } else if ((engine_inputMappings.currEmuInputStatus.mouseColumn - ENGINE_EGA_GFX_WIDTH) / 2 < -g_input.mouse_ctrl_1) {
        horiz = -1;
        engine_inputMappings.currEmuInputStatus.mouseColumn += (g_input.mouse_ctrl_1 << 1);
    }
    /* On the other hand, the vertical range's units are kept intact. */
    if (engine_inputMappings.currEmuInputStatus.mouseRow - ENGINE_EGA_GFX_HEIGHT/2 > g_input.mouse_ctrl_1) {
        vert = 1;
        engine_inputMappings.currEmuInputStatus.mouseRow -= g_input.mouse_ctrl_1;
    } else if (engine_inputMappings.currEmuInputStatus.mouseRow - ENGINE_EGA_GFX_HEIGHT/2 < -g_input.mouse_ctrl_1) {
        vert = -1;
        engine_inputMappings.currEmuInputStatus.mouseRow += g_input.mouse_ctrl_1;
    }
    /* Finally we extract motion info (same as engine_getKeybCtrlState) */
    switch (vert * 3 + horiz + 4) {
        case 0: result.direction = 7;
            break;
        case 1: result.direction = 0;
            break;
        case 2: result.direction = 1;
            break;
        case 3: result.direction = 6;
            break;
        case 4: result.direction = 8;
            break;
        case 5: result.direction = 2;
            break;
        case 6: result.direction = 5;
            break;
        case 7: result.direction = 4;
            break;
        case 8: result.direction = 3;
            break;
        default: break;
    }
    return result;
}

GameInput_T CVort_engine_getJoystickCtrl(int16_t joy_id) {
    const int16_t engineJoyId = joy_id - 1;
    GameInput_T result;
    int16_t horiz = 0, vert = 0;
    memset(&result, 0, sizeof (result));
    JoystickPoll_T currJoystickPoll = CVort_engine_pollJoystick(joy_id);
    if ((currJoystickPoll.xPoll > 500) || (currJoystickPoll.yPoll > 500)) {
        currJoystickPoll.xPoll = g_input.joystick_ctrl[0][joy_id] + 1;
        currJoystickPoll.yPoll = g_input.joystick_ctrl[2][joy_id] + 1;
    }
    if (g_input.joystick_ctrl[1][joy_id] < currJoystickPoll.xPoll)
        horiz = 1;
    else if (g_input.joystick_ctrl[0][joy_id] > currJoystickPoll.xPoll)
        horiz = -1;
    if (g_input.joystick_ctrl[3][joy_id] < currJoystickPoll.yPoll)
        vert = 1;
    else if (g_input.joystick_ctrl[2][joy_id] > currJoystickPoll.yPoll)
        vert = -1;
    switch (vert * 3 + horiz + 4) {
        case 0: result.direction = 7;
            break;
        case 1: result.direction = 0;
            break;
        case 2: result.direction = 1;
            break;
        case 3: result.direction = 6;
            break;
        case 4: result.direction = 8;
            break;
        case 5: result.direction = 2;
            break;
        case 6: result.direction = 5;
            break;
        case 7: result.direction = 4;
            break;
        case 8: result.direction = 3;
            break;
        default: break;
    }
    result.but1jump = (engine_inputMappings.currEmuInputStatus.joystickButtonsMask & (1 << (2*(joy_id-1)))) ? 1 : 0;
    result.but2pogo = (engine_inputMappings.currEmuInputStatus.joystickButtonsMask & (2 << (2*(joy_id-1)))) ? 1 : 0;
    return result;
}

void CVort_engine_cross_logMessage(CVort_Log_Message_Class_T msgClass, const char *format, ...) {
    // NOTE: Logging currently targets stderr only.
    // Logging currently writes to stderr only.
    va_list args;
    va_start(args, format);
    switch (msgClass) {
        case CVORT_LOG_MSG_NORMAL: fprintf(stderr, "Log: "); break;
        case CVORT_LOG_MSG_WARNING: fprintf(stderr, "Warning: "); break;
        case CVORT_LOG_MSG_ERROR: fprintf(stderr, "Error: "); break;
    }
    vfprintf(stderr, format, args);
    va_end(args);
}

int16_t CVort_engine_signExtend8To16(int8_t inputVal) {
    int16_t result = ((int16_t)inputVal) & 0xFF;
    if (inputVal & 0x80)
        result += 0xFF00;
    return result;
}

int32_t CVort_engine_signExtend16To32(int16_t inputVal) {
    int32_t result = ((int32_t)inputVal) & 0xFFFF;
    if (inputVal & 0x8000)
        result += 0xFFFF0000;
    return result;
}

int16_t CVort_engine_toupper(int16_t c) {
    if (c == -1)
        return -1;
    if (exeFields.uppercase_table[c & 0xff] & 8)
        return (c & 0xff) - 0x20;
    return (c & 0xff);
}

// Known gap: DOS text-mode edge cases (wrapping, attributes, cursor movement)
// are only partially verified against original behavior.

void CVort_engine_puts(const char *str) {
    uint8_t *txtMemPtr;
    static const uint8_t *txtMemEndPtr = engine_screen.client.egaTxtMemory + sizeof(engine_screen.client.egaTxtMemory);
    uint8_t lastCharAttributes;
    int loopVar;
    do {
        txtMemPtr = engine_screen.client.egaTxtMemory + ((ENGINE_EGAVGA_TXT_COLS_NUM * engine_screen.client.txtCursorPosY + engine_screen.client.txtCursorPosX) << 1);
        switch (*str) {
            case 9: // Tab
                for (loopVar = 0; loopVar < 7; loopVar++) {
                    if (txtMemPtr < txtMemEndPtr)
                        txtMemPtr[0] = 0x20; // A space
                    engine_screen.client.txtCursorPosX++;
                    txtMemPtr += 2;
                }
                break;
            case 0xA: // Line Feed
                engine_screen.client.txtCursorPosY++;
                break;
            case 0xD: // Carriage Return
                engine_screen.client.txtCursorPosX = 0;
                break;
            case 0: // End of string, so add a new line and finish.
                engine_screen.client.txtCursorPosY++;
                engine_screen.client.txtCursorPosX = 0;
                break;
            default:
                txtMemPtr[0] = *str;
                engine_screen.client.txtCursorPosX++;
        }
        if (engine_screen.client.txtCursorPosX >= ENGINE_EGAVGA_TXT_COLS_NUM) {
            engine_screen.client.txtCursorPosX %= ENGINE_EGAVGA_TXT_COLS_NUM;
            engine_screen.client.txtCursorPosY++;
        }
        if (engine_screen.client.txtCursorPosY >= ENGINE_EGAVGA_TXT_ROWS_NUM) {
            engine_screen.client.txtCursorPosY--;
            lastCharAttributes = engine_screen.client.egaTxtMemory[sizeof (engine_screen.client.egaTxtMemory) - 1];
            memmove(engine_screen.client.egaTxtMemory, engine_screen.client.egaTxtMemory + 2 * ENGINE_EGAVGA_TXT_COLS_NUM, 2 * ENGINE_EGAVGA_TXT_COLS_NUM * (ENGINE_EGAVGA_TXT_ROWS_NUM - 1));
            for (loopVar = sizeof (engine_screen.client.egaTxtMemory) - 2 * ENGINE_EGAVGA_TXT_COLS_NUM; loopVar < sizeof (engine_screen.client.egaTxtMemory); loopVar += 2) {
                engine_screen.client.egaTxtMemory[loopVar] = 0x20; // Space
                engine_screen.client.egaTxtMemory[loopVar + 1] = lastCharAttributes; // IS THAT THE CORRECT BEHAVIOR???
            }
        }
        if (!(*str)) {
            // For the sake of simplicity refresh the whole thing...
            CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr(0, 0, ENGINE_EGAVGA_TXT_COLS_NUM, ENGINE_EGAVGA_TXT_ROWS_NUM);
            return;
	}
        str++;
    } while (1);
}
