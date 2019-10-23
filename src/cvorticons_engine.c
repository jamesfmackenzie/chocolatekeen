#include <stdio.h>
//#include <vector>
#include "SDL.h"
#include "SDL_endian.h"
#include <emscripten.h>

/* TODO: Currently unused */
#if 0
#ifdef __WIN32__
#include <direct.h> // For _mkdir
#include <shlobj.h> // For SHGetFolderPath
#else
#include <sys/stat.h> // For mkdir
#endif
#endif

#include "../rsrc/chocolate-keen_vga_fonts.h"
#include "cvorticons.h"
#include "cvorticons1.h"
#include "cvorticons2.h"
#include "cvorticons3.h"
#include "decompression/CGenius/fileio/compression/Cunlzexe.h"

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
#if 0
const uint32_t engine_defaultEgaABGRColorTableLE[] = {
    0X00000000, 0x0000aa00, 0x00aa0000, 0x00aaaa00, 0xaa000000, 0xaa00aa00, 0xaa550000, 0xaaaaaa00,
    0x55555500, 0x5555ff00, 0x55ff5500, 0x55ffff00, 0xff555500, 0xff55ff00, 0xffff5500, 0xffffff00
};
#endif
#if 0
const uint32_t engine_defaultEgaABGRColorTableLE[] = {
    0X00000000, 0x00aa0000, 0x0000aa00, 0x00aaaa00, 0x000000aa, 0x00aa00aa, 0x000055aa, 0x00aaaaaa,
    0x00555555, 0x00ff5555, 0x0055ff55, 0x00ffff55, 0x005555ff, 0x00ff55ff, 0x0055ffff, 0x00ffffff
};
#endif
#if 0
const uint32_t engine_defaultEgaABGRColorTableLE[] = {
    0X000000, 0x0000aa, 0x00aa00, 0x00aaaa, 0xaa0000, 0xaa00aa, 0xaa5500, 0xaaaaaa,
    0x555555, 0x5555ff, 0x55ff55, 0x55ffff, 0xff5555, 0xff55ff, 0xffff55, 0xffffff
};
#endif

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

bool CVort_engine_parseConfigLine(const char *line);

bool privProcessEngineArguments(int argc, char **argv) {
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

    if (!CVort_engine_setVideoMode(3)) {
        CVort_engine_shutdownSDL();
        return false;
    }

    CVort_engine_puts("Chocolate Keen v0.9.4 (April 29th, 2014), Alpha release");
    CVort_engine_puts("-------------------------------------------------------");
    CVort_engine_puts("");

    return true;
}

bool privClearKeysAndCheckForQuit(void) {
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

bool privCheckForUserInputAndQuit(void) {
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
    if (SDL_Init(SDL_INIT_TIMER |
            SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
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
    if (privClearKeysAndCheckForQuit()) {
        CVort_engine_shutdownSDL();
        return;
    }
    do {
        if (privCheckForUserInputAndQuit()) {
            CVort_engine_shutdownSDL();
            return;
        }
        // We cannot use this here - key_map is not ready!
        // Furthermore, we are waiting for SDL events directly.
        //engine_shortSleep();
        // But let's use this:
        CVort_engine_updateActualDisplay();
        //SDL_Delay(1);
        emscripten_sleep(1);
    } while (1);
    // We should NEVER reach this stage... so not calling CVort_engine_shutdownSDL().
}

bool CVort_engine_processArguments(int argc, char **argv) {
    if (privProcessEngineArguments(argc, argv)) {
        return true;
    }
    CVort_engine_displayCommandLineHelp();
    return false;
}

uint16_t CVort_private_continue_game();
uint16_t CVort_demo_toggle_prepare_to_record();
void CVort_private_engine_setTicks(uint32_t currTicks);
uint32_t CVort_private_engine_getTicks();
void CVort_private_engine_setTicksSync(uint32_t currTicks);
uint32_t CVort_private_engine_getTicksSync();

void CVort_engine_initializeKeenVerStructs(void);

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
#if 0
    // Otherwise, we simply set this.
    if (engine_arguments.rendererDriver) {
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, engine_arguments.rendererDriver);
    }
#endif
#endif

    if (SDL_Init(SDL_INIT_TIMER |
            SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    CVort_engine_setupInputMappings();
    SDL_JoystickEventState(SDL_ENABLE);

    if (!CVort_engine_prepareScreen()) {
        return false;
    }

    CVort_engine_initializeKeenVerStructs();

    if (engine_skipLauncher) {
        CVort_engine_loadKeen(engine_gameVersion);
    }

    if (!CVort_engine_setVideoMode(-1)) {
        return false;
    }

    SDL_ShowCursor(SDL_ENABLE);

#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    // Already done in privProcessEngineArguments otherwise
    engine_forceSpecificEpisode = true;
#endif
    CVort_gui_runLoop();

    return true;
}

bool privLoadEXE(gameversion_T gameVer, uint8_t **pExeImageBuffer);

void CVort_engine_prepareEpisodeSpecificFuncPointers(void) {
    size_t numOfTiles;
    switch (engine_gameVersion) {
        //case GAMEVER_AUTO: return; // Don't allocate memory for animating tiles
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
        case GAMEVER_KEEN1:
            numOfTiles = CVort1_TILENUM;
            CVort_ptr_engine_processEXE = &CVort1_engine_processEXE;
            CVort_ptr_show_pause_menu = &CVort1_show_pause_menu;
            CVort_ptr_init_level = &CVort1_init_level;
            CVort_ptr_draw_win = &CVort1_draw_win;
            CVort_ptr_draw_scores = &CVort1_draw_scores;
            CVort_ptr_show_ordering = &CVort1_show_ordering;
            CVort_ptr_draw_ordering_info = &CVort1_draw_ordering_info;
            CVort_ptr_do_ordering = &CVort1_do_ordering;
            CVort_ptr_inlevel_message = &CVort1_inlevel_message;
            CVort_ptr_worldmap_sprites = &CVort1_worldmap_sprites;
            CVort_ptr_contact_keen = &CVort1_contact_keen;
            break;
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
        case GAMEVER_KEEN2:
            numOfTiles = CVort2_TILENUM;
            CVort_ptr_engine_processEXE = &CVort2_engine_processEXE;
            CVort_ptr_show_pause_menu = &CVort2_show_pause_menu;
            CVort_ptr_init_level = &CVort2_init_level;
            CVort_ptr_draw_win = &CVort2_draw_win;
            CVort_ptr_draw_scores = &CVort2_draw_scores;
            CVort_ptr_show_ordering = &CVort2_show_ordering;
            CVort_ptr_draw_ordering_info = &CVort2_draw_ordering_info;
            CVort_ptr_do_ordering = &CVort2_do_ordering;
            CVort_ptr_inlevel_message = &CVort2_inlevel_message;
            //CVort_ptr_worldmap_sprites = &CVort2_worldmap_sprites;
            CVort_ptr_contact_keen = &CVort2_contact_keen;
            break;
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
        case GAMEVER_KEEN3:
            numOfTiles = CVort3_TILENUM;
            CVort_ptr_engine_processEXE = &CVort3_engine_processEXE;
            CVort_ptr_show_pause_menu = &CVort3_show_pause_menu;
            CVort_ptr_init_level = &CVort3_init_level;
            CVort_ptr_draw_win = &CVort3_draw_win;
            CVort_ptr_draw_scores = &CVort3_draw_scores;
            CVort_ptr_show_ordering = &CVort3_show_ordering;
            CVort_ptr_draw_ordering_info = &CVort3_draw_ordering_info;
            CVort_ptr_do_ordering = &CVort3_do_ordering;
            CVort_ptr_inlevel_message = &CVort3_inlevel_message;
            CVort_ptr_worldmap_sprites = &CVort3_worldmap_sprites;
            CVort_ptr_contact_keen = &CVort3_contact_keen;
            break;
#endif
    }
    anim_frame_tiles[0] = (uint16_t *)malloc(4*numOfTiles*sizeof(uint16_t));
    anim_frame_tiles[1] = anim_frame_tiles[0] + numOfTiles;
    anim_frame_tiles[2] = anim_frame_tiles[0] + 2*numOfTiles;
    anim_frame_tiles[3] = anim_frame_tiles[0] + 3*numOfTiles;
}

#define KEEN1_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN1/"
#define KEEN2_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN2/"
#define KEEN3_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN3/"

size_t engine_gameDataPrefixLen;
char *engine_gameDataFullPathBuffer = NULL;

void privPrepareGameDataFilePathBuffers(gameversion_T gameVer) {
	if (engine_gameDataFullPathBuffer) {
		free(engine_gameDataFullPathBuffer);
	}
	switch (gameVer) {
		case GAMEVER_KEEN1:
			engine_gameDataPrefixLen = strlen(KEEN1_GAMEDATA_PREFIX_WITH_SLASH);
			engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen+13);
			strcpy(engine_gameDataFullPathBuffer, KEEN1_GAMEDATA_PREFIX_WITH_SLASH);
			break;
		case GAMEVER_KEEN2:
			engine_gameDataPrefixLen = strlen(KEEN2_GAMEDATA_PREFIX_WITH_SLASH);
			engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen+13);
			strcpy(engine_gameDataFullPathBuffer, KEEN2_GAMEDATA_PREFIX_WITH_SLASH);
			break;
		case GAMEVER_KEEN3:
			engine_gameDataPrefixLen = strlen(KEEN3_GAMEDATA_PREFIX_WITH_SLASH);
			engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen+13);
			strcpy(engine_gameDataFullPathBuffer, KEEN3_GAMEDATA_PREFIX_WITH_SLASH);
			break;
	}
}

void CVort_engine_loadKeen(gameversion_T gameVer) {
	bool exeLoadedSuccessfully;
	if (engine_forceSpecificEpisode) {
		engine_gameVersion = gameVer;
		exeLoadedSuccessfully = privLoadEXE(gameVer, &exeImage);
	} else {
		do { // a Use loop so we can break when
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
			exeLoadedSuccessfully = privLoadEXE(GAMEVER_KEEN1, &exeImage);
			if (exeLoadedSuccessfully) {
				engine_gameVersion = GAMEVER_KEEN1;
				break;
			}
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
			exeLoadedSuccessfully = privLoadEXE(GAMEVER_KEEN2, &exeImage);
			if (exeLoadedSuccessfully) {
				engine_gameVersion = GAMEVER_KEEN2;
				break;
			}
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
			exeLoadedSuccessfully = privLoadEXE(GAMEVER_KEEN3, &exeImage);
			if (exeLoadedSuccessfully) {
				engine_gameVersion = GAMEVER_KEEN3;
				break;
			}
#endif
		} while (0);
	}
	if (!exeLoadedSuccessfully) {
		char lineBuffer[78];

		if (!CVort_engine_prepareInstructionsScreen()) {
			exit(1);
		}

		if (engine_forceSpecificEpisode) {
			snprintf(lineBuffer, sizeof(lineBuffer), "Sorry, can't find a copy of Commander Keen %d, v1.31.", gameVer+1);
			CVort_engine_puts(lineBuffer);
		} else {
			// Actually we shouldn't reach this if
			// CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE is defined...
			CVort_engine_puts("Sorry, can't find any supported copy of a Commander Keen episode.");
			CVort_engine_puts("Supported Commander Keen 1/2/3 version: 1.31");
		}

		if (privClearKeysAndCheckForQuit()) {
			// Don't call CVort_engine_shutdown:
			// We don't want to save (video) settings.
			// FIXME: If the user changed something in the launcher?
			CVort_engine_shutdownSDL();
			exit(0);
		}
		do {
			if (privCheckForUserInputAndQuit()) {
				// Don't call CVort_engine_shutdown:
				// We don't want to save (video) settings.
				// FIXME: If the user changed something in the launcher?
				CVort_engine_shutdownSDL();
				exit(0);
			}
			// We cannot use this here - key_map is not ready!
			// Furthermore, we are waiting for SDL events directly.
			//engine_shortSleep();
			// But let's use this:
			CVort_engine_updateActualDisplay();
        //SDL_Delay(1);
        emscripten_sleep(1);
		} while (1);
		// We should NEVER reach this stage...
	}
	if (!CVort_engine_setVideoMode(3)) {
		// Don't call CVort_engine_shutdown:
		// We don't want to save (video) settings (makes sense here).
		CVort_engine_shutdownSDL();
		exit(1);
	}
	CVort_engine_prepareEpisodeSpecificFuncPointers();
	CVort_ptr_engine_processEXE();

	if (engine_arguments.extras.vorticonsDemoModeToggle) {
		// We basically swap the roles of ticks and ticks_sync,
		// so demo playbacks can be reproduced in a consistent manner.
		CVort_ptr_engine_getTicks = CVort_private_engine_getTicksSync;
		CVort_ptr_engine_setTicks = CVort_private_engine_setTicksSync;
		CVort_ptr_engine_getTicksSync = CVort_private_engine_getTicks;
		CVort_ptr_engine_setTicksSync = CVort_private_engine_setTicks;
		CVort_ptr_continue_game = CVort_demo_toggle_prepare_to_record;
	} else {
		CVort_ptr_engine_getTicks = CVort_private_engine_getTicks;
		CVort_ptr_engine_setTicks = CVort_private_engine_setTicks;
		CVort_ptr_engine_getTicksSync = CVort_private_engine_getTicksSync;
		CVort_ptr_engine_setTicksSync = CVort_private_engine_setTicksSync;
		CVort_ptr_continue_game = CVort_private_continue_game;
	}

	// TODEmulatedKey_TO: Support more format and adapt chunksize to the rate?
	// TODO: Check against Mix_QuerySpec.
	if (!engine_arguments.disableSoundSystem) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't initialize audio subsystem: %s\n", SDL_GetError());
			CVort_engine_shutdownSDL();
			exit(1);
		}
		SDL_AudioSpec audioFormat;
		audioFormat.freq = engine_arguments.sndSampleRate;
		audioFormat.format = AUDIO_S16SYS;
		audioFormat.channels = 1;
		audioFormat.samples = 1024;
		audioFormat.callback = &CVort_engine_sndCallback;
		audioFormat.userdata = NULL;
		if (SDL_OpenAudio(&audioFormat, NULL) < 0) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't open audio device: %s\n", SDL_GetError());
			CVort_engine_shutdownSDL();
			exit(1);
		}
	}

	//CVort_engine_toggleCursorLock(engine_arguments.isFullscreen && engine_arguments.cursorAutoLock);

#ifdef CHOCOLATE_KEEN_CONFIG_DEBUG
	SDL_ShowCursor(engine_arguments.alwaysHideCursor ? SDL_DISABLE : SDL_ENABLE);
#endif
	// Set current overscan border color to... cyan, like in vanilla Keen.
	// UPDATE: Done while graphics are loaded and decompressed.
	//engine_setBorderColor(3);

	// I haven't seen this set to NULL anywhere, but it *really* should!
	draw_func = NULL;

	engine_lastScanCode = 0;
	engine_lastKeyTime = 0;
	engine_isBeforeKeyDelay = true;
  
	//engine_momentToDelayToInScanlines = engine_getTimeInScanlines();
	engine_doResetMomentToDelay = true;
	engine_refreshTimeInMicroSec = ((int64_t)1000000)*ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR/engine_arguments.calc.scaledRefreshRate;
	engine_sdlMicroTicksDelayOffset = 0;
	engine_gameTicksDelayOffset = 0;
	engine_gameTicksStart = 0;
	extern void CVort_private_engine_setTicks(uint32_t currTicks);
	CVort_private_engine_setTicks(0);
	//engine_gameTicksDelayOffsets[0] = engine_gameTicksDelayOffsets[1] = 0;
	// engine_isFrameReadyToDisplay = true;

	CVort_main();
}

#if 0
uint64_t CVort_engine_getTimeInScanlines()
{
   return ((uint64_t)SDL_GetTicks())*engine_arguments.calc.scaledRefreshRate*engine_arguments.totalScanHeight/(1000*ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR);
}
#endif

void CVort_engine_shutdown() {
    // TODO/FIXME: Free host joystick resources

    //if (!engine_arguments.disableSoundSystem)
    //    SDL_CloseAudio();

    // The paletted pages were created with a SINGLE allocation command!
    //	free(engine_palettedpages[0]);

    //	SDL_FreeSurface(engine_egapages[0]);
    //	SDL_FreeSurface(engine_egapages[1]);
#if 0
    if (engine_arguments.outputSystem == OUTPUTSYS_OVERLAY)
      SDL_FreeYUVOverlay(engine_overlay);
#endif
    CVort_engine_saveConfigFile();
    CVort_engine_shutdownSDL();
}

#define KEEN1_EXE_COMPRESSED_SIZE 51190
#define KEEN1_EXE_DECOMPRESSED_SIZE 100484
#define KEEN1_EXE_FILENAME "KEEN1.EXE"
#define KEEN2_EXE_COMPRESSED_SIZE 58335
#define KEEN2_EXE_DECOMPRESSED_SIZE 118672
#define KEEN2_EXE_FILENAME "KEEN2.EXE"
#define KEEN3_EXE_COMPRESSED_SIZE 61599
#define KEEN3_EXE_DECOMPRESSED_SIZE 127616 
#define KEEN3_EXE_FILENAME "KEEN3.EXE"

typedef struct KeenVer_Info_T {
	struct {
		uint32_t compressed;
		uint32_t decompressed;
	} exeSize;
	const char *exeFilename;
} KeenVer_Info_T;

KeenVer_Info_T CVorticons_engine_keenVersionInfo[GAMEVER_TOTALAMOUNT];

void CVort_engine_initializeKeenVerStructs(void) {
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN1].exeSize.compressed = KEEN1_EXE_COMPRESSED_SIZE;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN1].exeSize.decompressed = KEEN1_EXE_DECOMPRESSED_SIZE;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN1].exeFilename = KEEN1_EXE_FILENAME;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN2].exeSize.compressed = KEEN2_EXE_COMPRESSED_SIZE;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN2].exeSize.decompressed = KEEN2_EXE_DECOMPRESSED_SIZE;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN2].exeFilename = KEEN2_EXE_FILENAME;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN3].exeSize.compressed = KEEN3_EXE_COMPRESSED_SIZE;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN3].exeSize.decompressed = KEEN3_EXE_DECOMPRESSED_SIZE;
	CVorticons_engine_keenVersionInfo[GAMEVER_KEEN3].exeFilename = KEEN3_EXE_FILENAME;
}

bool privLoadEXE(gameversion_T gameVer, uint8_t **pExeImageBuffer) {
    uint8_t *wholeExeData;
    privPrepareGameDataFilePathBuffers(gameVer);
    FILE *fp = CVort_engine_cross_ro_data_fopen(CVorticons_engine_keenVersionInfo[gameVer].exeFilename);
    if (!fp)
        return false;
    uint32_t len = CVort_filelength(fp);
    // TODO: If len = compressedSize,
    // we currently assume valid uncompressed data.
    wholeExeData = (uint8_t *) malloc(len);
    if (!wholeExeData) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Out of memory for temporary EXE storage!\n");
        return false;
    }
    fread(wholeExeData, len, 1, fp);
    fclose(fp);

    uint32_t headerSize;
    if (len == CVorticons_engine_keenVersionInfo[gameVer].exeSize.compressed) {
        BYTE *uncompressedExeData;
        Cunlzexe_decompress(wholeExeData, &uncompressedExeData);
        headerSize = Cunlzexe_getHeaderSize();
        *pExeImageBuffer = (uint8_t *) malloc(Cunlzexe_getUncompressedExeSize() - headerSize);
        if (!(*pExeImageBuffer)) {
            free(wholeExeData);
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Out of memory for uncompressed EXE image storage!\n");
            return false;
        }
        memcpy(*pExeImageBuffer, uncompressedExeData + headerSize, Cunlzexe_getUncompressedExeSize() - headerSize);
        Cunlzexe_free(&uncompressedExeData);
    } else { // TODO: For now assume a valid uncompressed file
        headerSize = 16 * (wholeExeData[8] + 16 * wholeExeData[9]);
        *pExeImageBuffer = (uint8_t *) malloc(len - headerSize);
        if (!(*pExeImageBuffer)) {
            free(wholeExeData);
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Out of memory for uncompressed EXE image storage!\n");
            return false;
        }
        memcpy(*pExeImageBuffer, wholeExeData + headerSize, len - headerSize);
    }
    free(wholeExeData);
    return true;
}

bool CVort_engine_isGameExeAvailable(gameversion_T gameVer) {
	privPrepareGameDataFilePathBuffers(gameVer);
	FILE *fp = CVort_engine_cross_ro_data_fopen(CVorticons_engine_keenVersionInfo[gameVer].exeFilename);
	if (!fp) {
		return false;
	}
	uint32_t len = CVort_filelength(fp);
	fclose(fp);
	return ((len == CVorticons_engine_keenVersionInfo[gameVer].exeSize.compressed) || (len == CVorticons_engine_keenVersionInfo[gameVer].exeSize.decompressed));
}

void CVort_engine_freeEXE(uint8_t *exeImageBuffer) {
    free(exeImageBuffer);
}

void CVort_engine_handleQuit() {
    if (privClearKeysAndCheckForQuit()) {
        CVort_engine_shutdown();
        exit(0);
    }
    // Now begin for real
    do {
        if (privCheckForUserInputAndQuit()) {
            CVort_engine_shutdown();
            exit(0);
        }
        // We we are waiting for SDL events
        // directly, so we cannot use this.
        //engine_shortSleep();
        // But let's use this:
        CVort_engine_updateActualDisplay();
        //SDL_Delay(1);
        emscripten_sleep(1);
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
    // TODO/FIXME: Let one have an effect on that?
    return true;
}

const uint32_t pitAltCounter = 0x2000;

int64_t CVort_engine_convertMomentFromGameTicksToMicroSec(uint32_t momentInGameTicks) {
    return (((int64_t)(momentInGameTicks-engine_gameTicksStart))*((int64_t)1000000)*((int64_t)pitAltCounter))/((int64_t)PC_PIT_RATE) + engine_sdlMicroTicksStart;
}

void CVort_private_engine_setTicks(uint32_t currTicks) {
   engine_gameTicksStart = currTicks-(CVort_private_engine_getTicks()-engine_gameTicksStart);
#if 0
   engine_gameTicksStart = currTicks;
   engine_sdlMicroTicksStart = ((int64_t)1000)*((int64_t)SDL_GetTicks());
   /* We need to take the offsets into consideration, BUT in an intelligent
    * way. If we naively increase engine_gameTicksStart in order to
    * compensate for the offset, things can go wrong later.
    */
   engine_gameTicksStart -= (CVort_private_engine_getTicks() - currTicks);
#endif
}

uint32_t CVort_private_engine_getTicks() {
    return engine_gameTicksStart-engine_gameTicksDelayOffset+(uint32_t)((((int64_t)(((int64_t)1000)*((int64_t)SDL_GetTicks())-(int64_t)(engine_sdlMicroTicksStart)-(int64_t)(engine_sdlMicroTicksDelayOffset)))*(int64_t)(PC_PIT_RATE))/(((int64_t)1000000)*((int64_t)pitAltCounter)));
}

uint32_t CVort_private_engine_getActualTicks() {
    return engine_gameTicksStart+(uint32_t)((((int64_t)(((int64_t)1000)*((int64_t)SDL_GetTicks())-(int64_t)(engine_sdlMicroTicksStart)))*(int64_t)(PC_PIT_RATE))/(((int64_t)1000000)*((int64_t)pitAltCounter)));
}

void CVort_private_engine_setTicksSync(uint32_t currTicks) {
    ticks_sync = currTicks;
}

uint32_t CVort_private_engine_getTicksSync() {
    return ticks_sync;
}

void CVort_engine_delay(int16_t length)
{
    if (engine_doResetMomentToDelay)
    {
        engine_doResetMomentToDelay = false;
        /* We are RIGHT after a call to engine_shortSleep, if not on
         * application startup, so better update the display contents
         * so nothing gets missed when it's unexpected.
         */
        CVort_engine_updateActualDisplay();
        engine_sdlMicroTicksDelayOffset = 0;
        //engine_gameTicksDelayOffset = 0;
        engine_momentToDelayInMicroSec = 1000*((int64_t)SDL_GetTicks());
    }
    /* Simulate waiting while in vertical blank first, and then
     * waiting while NOT in vertical blank. In practice, we jump to
     * the very beginning of the next "refresh cycle".
     * This is repeated for a total of 'length' times.
     */
    if (length > 0) {
        engine_momentToDelayInMicroSec -= (engine_momentToDelayInMicroSec % engine_refreshTimeInMicroSec);
        engine_momentToDelayInMicroSec += length * engine_refreshTimeInMicroSec;
    }
    CVort_engine_updateInputStatus();
    while ((int64_t)(1000*((int64_t)SDL_GetTicks()) - engine_momentToDelayInMicroSec) < 0) {
        CVort_engine_doWaitInterval();
        CVort_engine_updateInputStatus();
    }

    /* The delay may have taken longer than intended (due to OS scheduling),
     * but the game should better "think" the passed time is really what's
     * expected. So we (re)set an offset for calls to CVort_private_engine_getTicks(),
     * measured in microseconds.
     */
    engine_sdlMicroTicksDelayOffset = 1000*((int64_t)SDL_GetTicks()) - engine_momentToDelayInMicroSec;
    engine_gameTicksDelayOffset = 0;
}

void CVort_engine_delayInGameTicks(uint32_t startOfDelayMoment, uint16_t length) {
    /* WARNING: Without this check, upon returning to the main menu from the
     * help text, the game is expected to HANG for long!
     * (Actually it still checks the input so you may unlock cursor and quit.)
     */
    if ((CVort_private_engine_getTicks()&0xFFFF)-(startOfDelayMoment&0xFFFF) >= length) {
        return;
    }
    if (engine_doResetMomentToDelay) {
        engine_doResetMomentToDelay = false;
        /* We are RIGHT after a call to engine_shortSleep, if not on
         * application startup, so better update the display contents
         * so nothing gets missed when it's unexpected.
         */
        CVort_engine_updateActualDisplay();
        // Maybe we do not wait at all, so refresh these offsets
        engine_sdlMicroTicksDelayOffset = 0;
        engine_gameTicksDelayOffset = 0;
        //
        engine_momentToDelayInMicroSec = 1000*((int64_t)SDL_GetTicks());
    }
    uint32_t endOfDelayMoment = startOfDelayMoment + ((uint32_t)length);
    int64_t momentToDelayInMicroSec = CVort_engine_convertMomentFromGameTicksToMicroSec(endOfDelayMoment);
    // It IS possible that we have already passed the given
    // time, even if it is accurate (e.g. on game startup).
    if ((int64_t)(momentToDelayInMicroSec - engine_momentToDelayInMicroSec) <= 0) {
        return;
    }
    engine_momentToDelayInMicroSec = momentToDelayInMicroSec;
    CVort_engine_updateInputStatus();
    while ((int64_t)(1000*((int64_t)SDL_GetTicks()) - engine_momentToDelayInMicroSec) < 0) {
        CVort_engine_doWaitInterval();
        CVort_engine_updateInputStatus();
    }
    /* The delay may have taken longer than intended (due to OS scheduling),
     * but the game should better "think" the passed time is really what's
     * expected. So we (re)set an offset for calls to CVort_private_engine_getTicks(),
     * measured in game ticks.
     */
    engine_sdlMicroTicksDelayOffset = 0;
    engine_gameTicksDelayOffset = CVort_private_engine_getActualTicks() - endOfDelayMoment;
}

void CVort_engine_doWaitInterval(void) {
    /* For long delays we should update the screen so there's no garbage; But
     * not too often - it's unnecessary and actually results in a HIGHER power
     * consumption, comparing to an actual running gameplay in progress.
     * Furthermore, an update is forced if there is an actual change to the
     * screen contents.
     */
    if (engine_isFrameReadyToDisplay || ((uint32_t)(SDL_GetTicks() - engine_lastDisplayUpdateTime) >= 100)) {
        CVort_engine_updateActualDisplay();
        engine_lastDisplayUpdateTime = SDL_GetTicks();
    } else { // Maybe we have waited for (host, real) VSync anyway?
        //SDL_Delay(1);
        emscripten_sleep(1);
    }
}

void CVort_engine_shortSleep(void) {
    /* As in CVort_engine_doWaitInterval, we update the screen if there are actual
     * changes or some time has passed.
     */
    if (engine_isFrameReadyToDisplay || ((uint32_t)(SDL_GetTicks() - engine_lastDisplayUpdateTime) >= 100)) {
    //if ((int32_t)(SDL_GetTicks() - engine_lastDisplayUpdateTime) >= 100) {
        CVort_engine_updateActualDisplay();
        engine_lastDisplayUpdateTime = SDL_GetTicks();
    }
        //SDL_Delay(1);
        emscripten_sleep(1);
    CVort_engine_updateInputStatus();
    engine_doResetMomentToDelay = true;
}

void CVort_engine_loadSounds(void) {
    if (engine_arguments.disableSoundSystem) {
        return;
    }
    // For Keen 2 and 3, the sounds are stored in the EXE,
    // while for Keen 1 they are stored in an external file (SOUNDS.CK1).

    uint32_t len;
    uint8_t *soundDataPtr;

    if (engine_gameVersion == GAMEVER_KEEN1) {
        strcpy(string_buf, "SOUNDS.");
        strcat(string_buf, game_ext);
        FILE *fp = CVort_engine_cross_ro_data_fopen(string_buf);
        // TODO: What if we fail to load the file?
        if (fp) {
            len = CVort_filelength(fp);
            soundDataPtr = (uint8_t *) malloc(len);
            fread(soundDataPtr, len, 1, fp);
            fclose(fp);
        }
    } else if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {
        soundDataPtr = (uint8_t*) exeImage + (engine_gameVersion == GAMEVER_KEEN2? CVort2_SOUNDS_OFFSET : CVort3_SOUNDS_OFFSET);
        len = SDL_SwapLE16(*(uint16_t*)(soundDataPtr+4));
    } else {
        // Couldn't find anything
        return;
    }
    // Note: The data has been loaded from the file as-is,
    // in LITTLE-ENDIAN order wherever relevant.
    // Shouldn't this be soundDataPtr + 8?
    engine_sndCount = SDL_SwapLE16(*(uint16_t *) (soundDataPtr + 8));
    sndChunks = (sndChunk_T *) malloc(engine_sndCount * sizeof (sndChunk_T));
    // Sound priorities
    sndPriorities = (uint8_t *) malloc(engine_sndCount);

    uint32_t samplesPerBeep = pitAltCounter * engine_arguments.sndSampleRate / PC_PIT_RATE;
    // Allocate data for all potential beeps. We consider the
    // file's contents without the header or sound definitions.
    sndChunksData = malloc(ENGINE_SNDBYTEDEPTH * samplesPerBeep * (len - 16 * (engine_sndCount + 1)));

    // Now fill the data.
    // TODO: Again we assume 16-bit depth and a single channel.
    uint16_t *currSndChunkPtr = (uint16_t *) sndChunksData;
    // 16-bit is always the size of a value representing a beep in
    // in vanilla Keen - and in LITTLE ENDIAN order (as loaded from file).
    uint16_t *sndPtr, sndVal;
    uint32_t numOfBeeps;

    uint16_t beepWaveVal;
    uint32_t sampleCounter, beepHalfCycleCounter;

    for (uint16_t loopVar = 0; loopVar < engine_sndCount; loopVar++) {
        sndChunks[loopVar].chunk = currSndChunkPtr;
        // Get sound priority
        sndPriorities[loopVar] = soundDataPtr[16 * (loopVar + 1) + 2];
        // Get to the correct beep start as implied by the offset.
        sndPtr = (uint16_t *) (soundDataPtr + SDL_SwapLE16(*(uint16_t *) (soundDataPtr + 16 * (loopVar + 1))));
        numOfBeeps = 0;
        while (*sndPtr != 0xFFFF) { // This is endianness independent...
            if (*sndPtr) { // Beep
                sndVal = SDL_SwapLE16(*sndPtr);
                // The requested frequency is
                // f ~ PC_PIT_RATE/sndVal.
                // There are f cycles, each of length
                // sndSampleRate/f. Each such cycle
                // is made of two halves - "top" and "bottom".
                // The length of such a half is
                // sndSampleRate/(2f) ~
                // sndSampleRate*sndVal/(2*PIT_RATE)

                beepWaveVal = 4095; // 32767 - Too loud
                beepHalfCycleCounter = 0;
                for (sampleCounter = 0; sampleCounter < samplesPerBeep; sampleCounter++) {
                    *currSndChunkPtr = beepWaveVal;
                    beepHalfCycleCounter += 2 * PC_PIT_RATE;
                    if (beepHalfCycleCounter >= engine_arguments.sndSampleRate * sndVal) {
                        beepHalfCycleCounter %= engine_arguments.sndSampleRate*sndVal;
                        beepWaveVal = -beepWaveVal;
                    }
                    currSndChunkPtr++;
                }
            } else { // Silence
                memset(currSndChunkPtr, 0, 2*samplesPerBeep);
                currSndChunkPtr += samplesPerBeep;
            }
            numOfBeeps++;
            sndPtr++;
        }
        sndChunks[loopVar].len = (uint8_t *) currSndChunkPtr - (uint8_t *) sndChunks[loopVar].chunk;
    }
    engine_currSoundPlaying = -1;
    SDL_PauseAudio(0);
}

void CVort_engine_sndCallback(void *unused, uint8_t *stream, int len) {
    int16_t currSoundPlaying = engine_currSoundPlaying;
#if SDL_VERSION_ATLEAST(2,0,0)
    memset(stream, 0, len);
#endif
    if (currSoundPlaying < 0)
        return;
    int amount = sndChunks[currSoundPlaying].len - engine_currSoundLocation;
    if (amount > len)
        amount = len;

    if (want_sound & 0xFF) { // As done on Vanilla Keen...sort of
        memcpy(stream, (uint8_t *) sndChunks[currSoundPlaying].chunk + engine_currSoundLocation, amount);
    }
    engine_currSoundLocation += amount;
    if (engine_currSoundLocation == sndChunks[currSoundPlaying].len)
        engine_currSoundPlaying = -1;
}

void CVort_engine_setCurSound(uint16_t sound) {
    if (engine_arguments.disableSoundSystem)
        return;
    // Keen counts the sounds from 1, probably so the first 16 bytes of
    // the header are skipped. Since we have already taken care of that...
    sound--;

    if (!sound_disabled && ((engine_currSoundPlaying < 0) || (sndPriorities[sound] >= sound_limiter))) {
        SDL_PauseAudio(1);
        engine_currSoundPlaying = sound;
        sound_limiter = sndPriorities[sound];
        engine_currSoundLocation = 0;
        SDL_PauseAudio(0);
    }
}

void CVort_engine_saveCurSound() {
    if (engine_arguments.disableSoundSystem)
        return;
    if (!sound_disabled && (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING))
        SDL_PauseAudio(1);
}

void CVort_engine_restoreCurSound() {
    if (engine_arguments.disableSoundSystem)
        return;
    if (!sound_disabled && (SDL_GetAudioStatus() == SDL_AUDIO_PAUSED))
        SDL_PauseAudio(0);
}

void CVort_engine_finishCurSound() {
    if (engine_arguments.disableSoundSystem)
        return;
    if (!sound_disabled)
        while (engine_currSoundPlaying >= 0) {
            CVort_engine_shortSleep();
            //CVort_engine_updateActualDisplay();
        }
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
#if 0
    if (toggle) {
        SDL_WarpMouse(engine_aspectCorrectedScreenRect.x + engine_aspectCorrectedScreenRect.w / 2,
                      engine_aspectCorrectedScreenRect.y + engine_aspectCorrectedScreenRect.h / 2);
    }
#endif
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
    if (key_map[sc_dir[0]])
        vert = -1;
    if (key_map[sc_dir[2]])
        horiz = 1;
    if (key_map[sc_dir[4]])
        vert = 1;
    if (key_map[sc_dir[6]])
        horiz = -1;
    if (key_map[sc_dir[1]]) {
        vert = -1;
        horiz = 1;
    }
    if (key_map[sc_dir[7]])
        vert = horiz = -1;
    if (key_map[sc_dir[3]])
        vert = horiz = 1;
    if (key_map[sc_dir[5]]) {
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
    result.but1jump = key_map[sc_but1];
    result.but2pogo = key_map[sc_but2];
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
    if ((engine_inputMappings.currEmuInputStatus.mouseColumn - ENGINE_EGA_GFX_WIDTH) / 2 > mouse_ctrl_1) {
        horiz = 1;
        engine_inputMappings.currEmuInputStatus.mouseColumn -= (mouse_ctrl_1 << 1);
    } else if ((engine_inputMappings.currEmuInputStatus.mouseColumn - ENGINE_EGA_GFX_WIDTH) / 2 < -mouse_ctrl_1) {
        horiz = -1;
        engine_inputMappings.currEmuInputStatus.mouseColumn += (mouse_ctrl_1 << 1);
    }
    /* On the other hand, the vertical range's units are kept intact. */
    if (engine_inputMappings.currEmuInputStatus.mouseRow - ENGINE_EGA_GFX_HEIGHT/2 > mouse_ctrl_1) {
        vert = 1;
        engine_inputMappings.currEmuInputStatus.mouseRow -= mouse_ctrl_1;
    } else if (engine_inputMappings.currEmuInputStatus.mouseRow - ENGINE_EGA_GFX_HEIGHT/2 < -mouse_ctrl_1) {
        vert = -1;
        engine_inputMappings.currEmuInputStatus.mouseRow += mouse_ctrl_1;
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
        currJoystickPoll.xPoll = joystick_ctrl[0][joy_id] + 1;
        currJoystickPoll.yPoll = joystick_ctrl[2][joy_id] + 1;
    }
    if (joystick_ctrl[1][joy_id] < currJoystickPoll.xPoll)
        horiz = 1;
    else if (joystick_ctrl[0][joy_id] > currJoystickPoll.xPoll)
        horiz = -1;
    if (joystick_ctrl[3][joy_id] < currJoystickPoll.yPoll)
        vert = 1;
    else if (joystick_ctrl[2][joy_id] > currJoystickPoll.yPoll)
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

#define CHOCOLATE_KEEN_CFG_FILENAME "chocolate-keen.cfg"
#define CHOCOLATE_KEEN_CFG_LINE_LEN_BOUND 160

bool CVort_engine_parseBool(const char *key, bool *pVal, const char *line) {
	if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
		return false;
	}
	if (!strcmp(line+strlen(key)+1, "true")) {
		*pVal = true;
		return true;
	}
	if (!strcmp(line+strlen(key)+1, "false")) {
		*pVal = false;
		return true;
	}
	return false;
}

bool CVort_engine_parseInt(const char *key, int *pVal, const char *line) {
	if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
		return false;
	}
	*pVal = atoi(line+strlen(key)+1);
	return true;
}

bool CVort_engine_parseDims(const char *key, int *pWidth, int *pHeight, const char *line) {
	if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
		return false;
	}
	line += strlen(key)+1;
	if (sscanf(line, "%dx%d", pWidth, pHeight) == 2) {
		return true;
	}
	return false;
}

bool CVort_engine_parseEnum(const char *key, int *pVal, const char **choices, const char *line) {
	if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
		return false;
	}
	for (int val = 0; *choices; val++, choices++) {
		if (!strcmp(line+strlen(key)+1, *choices)) {
			*pVal = val;
			return true;
		}
	}
	return false;
}

#if SDL_VERSION_ATLEAST(2,0,0)
// SPECIAL
bool CVort_engine_parseSDLRendererDriver(const char *key, int *pVal, const char *line) {
	if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
		return false;
	}
	if (!strcmp(line+strlen(key)+1, "auto")) {
		*pVal = -1;
		return true;
	}
	SDL_RendererInfo sdlRendererInfo;
	for (int val = 0; val < SDL_GetNumRenderDrivers(); val++) {
		SDL_GetRenderDriverInfo(val, &sdlRendererInfo);
		if (!strcmp(line+strlen(key)+1, sdlRendererInfo.name)) {
			*pVal = val;
			return true;
		}
	}
	return false;
}
#endif

static const char *videoOutputCfgStrs[] = {
	"surface",
#if SDL_VERSION_ATLEAST(2,0,0)
	"sdlrenderer",
#else
	"overlay",
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	"opengl",
#endif
	0
};

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
static const char *videoGLVersionCfgStrs[] = {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
	"1.1",
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
	"2.0",
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
	"es2.0",
#endif
	0
};
#endif

static const char *scaleTypeCfgStrs[] = {
	"aspect", "fill", "boxed",
	0
};

static const char *emulatedGfxCfgStrs[] = {
	"vga", "ega", "egacga",
	0
};

static const char *initialInputDeviceStrs[] = {
	"keyboard", "mouse", "joy1", "joy2", "last",
	0
};

// Parses a NULL-terminated configuration file line, without the newline char.
bool CVort_engine_parseConfigLine(const char *line) {
	if (CVort_engine_parseBool("fullscreen", &engine_arguments.isFullscreen, line))
		return true;
	if (CVort_engine_parseDims("fullres", &engine_arguments.fullWidth, &engine_arguments.fullHeight, line))
		return true;
	if (CVort_engine_parseDims("windowres", &engine_arguments.windowWidth, &engine_arguments.windowHeight, line))
		return true;
#if SDL_VERSION_ATLEAST(2,0,0)
	if (CVort_engine_parseInt("displaynum", &engine_arguments.displayNumber, line))
		return true;
#endif
	int enumVal;
	if (CVort_engine_parseEnum("output", &enumVal, videoOutputCfgStrs, line)) {
		engine_arguments.outputSystem = (gfxOutputSystem_T)enumVal;
		return true;
	}
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	if (CVort_engine_parseEnum("glversion", &enumVal, videoGLVersionCfgStrs, line)) {
		engine_arguments.outputGLVersion = (gfxOutputGLVer_T)enumVal;
		return true;
	}
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
	if (CVort_engine_parseSDLRendererDriver("renderer", &engine_arguments.rendererDriverIndex, line))
		return true;
#endif
	if (CVort_engine_parseBool("vsync", &engine_arguments.vSync, line))
		return true;
	if (CVort_engine_parseBool("bilinear", &engine_arguments.bilinearInterpolation, line))
		return true;
	if (CVort_engine_parseEnum("scaletype", &enumVal, scaleTypeCfgStrs, line)) {
		engine_arguments.scaleType = (gfxScaleType_T)enumVal;
		return true;
	}
	if (CVort_engine_parseInt("scalefactor", &engine_arguments.zoomLevel, line))
		return true;
#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
	if (CVort_engine_parseBool("offscreen", &engine_arguments.offScreenRendering, line))
		return true;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	if (CVort_engine_parseBool("gpupalcycling", &engine_arguments.gpuPaletteCycling, line))
		return true;
#endif
	if (CVort_engine_parseBool("forcecutfullscreen", &engine_arguments.doForceCutFullScreen, line))
		return true;
	if (CVort_engine_parseEnum("emulatedgfx", &enumVal, emulatedGfxCfgStrs, line)) {
		engine_arguments.isEmulatedGfxCardVga = (enumVal == 0);
		engine_arguments.isEgaMonitorInUse = (enumVal == 1);
		return true;
	}
	if (CVort_engine_parseBool("autolock", &engine_arguments.cursorAutoLock, line))
		return true;
	if (CVort_engine_parseBool("alwayshidecursor", &engine_arguments.alwaysHideCursor, line))
		return true;
	if (CVort_engine_parseInt("audiorate", &engine_arguments.sndSampleRate, line))
		return true;
	if (CVort_engine_parseBool("disablesndsystem", &engine_arguments.disableSoundSystem, line))
		return true;

	// BACKWARDS COMPATIBILITY
	bool boolVal;
	if (CVort_engine_parseBool("compat_lastinput", &boolVal, line)) {
		engine_arguments.extras.initialEmulatedInputDevice = boolVal ? EMU_INITIAL_INPUT_LAST : EMU_INITIAL_INPUT_KEYBOARD;
		return true;
	}
	// New alternative
	if (CVort_engine_parseEnum("compat_initialemuinput", &enumVal, initialInputDeviceStrs, line)) {
		engine_arguments.extras.initialEmulatedInputDevice = (vanillaInitialEmulatedInputDevice_T)enumVal;
		return true;
	}

	if (CVort_engine_parseBool("compat_demomode", &engine_arguments.extras.vorticonsDemoModeToggle, line))
		return true;

	if (CVort_engine_parseBool("vanillaargument_k", &boolVal, line)) {
		engine_arguments.passKeysToBios = !boolVal;
		return true;
	}
	return false;
}

void CVort_engine_loadConfigFile(void) {
	char buffer[CHOCOLATE_KEEN_CFG_LINE_LEN_BOUND];
	FILE *fp = CVort_engine_configpath_fopen(CHOCOLATE_KEEN_CFG_FILENAME, "r");
	if (!fp) {
		return;
	}
	while (!feof(fp)) {
		if (fgets(buffer, sizeof(buffer), fp)) {
			// Remove ending newline char if there's any
			if (*buffer && (buffer[strlen(buffer)-1] == '\n')) {
				buffer[strlen(buffer)-1] = '\0';
			}
			CVort_engine_parseConfigLine(buffer);
		}
	}
	fclose(fp);
}

void CVort_engine_writeBool(const char *key, bool val, FILE *fp) {
	fprintf(fp, "%s=%s\n", key, val ? "true" : "false");
}

void CVort_engine_writeInt(const char *key, int val, FILE *fp) {
	fprintf(fp, "%s=%d\n", key, val);
}

void CVort_engine_writeDims(const char *key, int width, int height, FILE *fp) {
	fprintf(fp, "%s=%dx%d\n", key, width, height);
}

void CVort_engine_writeEnum(const char *key, int val, const char **choices, FILE *fp) {
	for (int counter = 0; counter < val; counter++, choices++);
	fprintf(fp, "%s=%s\n", key, *choices);
}

#if SDL_VERSION_ATLEAST(2,0,0)
// SPECIAL
void CVort_engine_writeSDLRendererDriver(const char *key, int val, FILE *fp) {
	if (val < 0) {
		fprintf(fp, "%s=auto\n", key);
	} else {
		SDL_RendererInfo sdlRendererInfo;
		SDL_GetRenderDriverInfo(val, &sdlRendererInfo);
		fprintf(fp, "%s=%s\n", key, sdlRendererInfo.name);
	}
}
#endif

void CVort_engine_saveConfigFile(void) {
	FILE *fp = CVort_engine_configpath_fopen(CHOCOLATE_KEEN_CFG_FILENAME, "w");
	if (!fp) {
		return; // TODO/FIXME: Actually what can be done at all?
	}
	CVort_engine_writeBool("fullscreen", engine_arguments.isFullscreen, fp);
	CVort_engine_writeDims("fullres", engine_arguments.fullWidth, engine_arguments.fullHeight, fp);
	CVort_engine_writeDims("windowres", engine_arguments.windowWidth, engine_arguments.windowHeight, fp);
#if SDL_VERSION_ATLEAST(2,0,0)
	CVort_engine_writeInt("displaynum", engine_arguments.displayNumber, fp);
#endif
	CVort_engine_writeEnum("output", (int)engine_arguments.outputSystem, videoOutputCfgStrs, fp);
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	CVort_engine_writeEnum("glversion", (int)engine_arguments.outputGLVersion, videoGLVersionCfgStrs, fp);
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
	CVort_engine_writeSDLRendererDriver("renderer", engine_arguments.rendererDriverIndex, fp);
#endif
	CVort_engine_writeBool("vsync", engine_arguments.vSync, fp);
	CVort_engine_writeBool("bilinear", engine_arguments.bilinearInterpolation, fp);
	CVort_engine_writeEnum("scaletype", (int)engine_arguments.scaleType, scaleTypeCfgStrs, fp);
	CVort_engine_writeInt("scalefactor", engine_arguments.zoomLevel, fp);
#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
	CVort_engine_writeBool("offscreen", engine_arguments.offScreenRendering, fp);
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	CVort_engine_writeBool("gpupalcycling", engine_arguments.gpuPaletteCycling, fp);
#endif
	CVort_engine_writeBool("forcecutfullscreen", engine_arguments.doForceCutFullScreen, fp);
	CVort_engine_writeEnum("emulatedgfx", engine_arguments.isEmulatedGfxCardVga ? 0 : engine_arguments.isEgaMonitorInUse ? 1 : 2, emulatedGfxCfgStrs, fp);
	CVort_engine_writeBool("autolock", engine_arguments.cursorAutoLock, fp);
	CVort_engine_writeBool("alwayshidecursor", engine_arguments.alwaysHideCursor, fp);
	CVort_engine_writeInt("audiorate", engine_arguments.sndSampleRate, fp);
	CVort_engine_writeBool("disablesndsystem", engine_arguments.disableSoundSystem, fp);

	//CVort_engine_writeBool("compat_lastinput", engine_arguments.extras.useLastChosenInputDevice, fp);
	CVort_engine_writeEnum("compat_initialemuinput", (int)engine_arguments.extras.initialEmulatedInputDevice, initialInputDeviceStrs, fp);
	CVort_engine_writeBool("compat_demomode", engine_arguments.extras.vorticonsDemoModeToggle, fp);

	CVort_engine_writeBool("vanillaargument_k", !engine_arguments.passKeysToBios, fp); // NOTICE THAT

	fclose(fp);
}

FILE *CVort_engine_cross_ro_data_fopen(const char *filename) {
    // TODO: For now we have no more than old style 8.3 filenames
    char *ptr = engine_gameDataFullPathBuffer+engine_gameDataPrefixLen;
    strncpy(ptr, filename, 13);
    for (size_t loopVar = 0; loopVar < strlen(ptr); loopVar++) {
        ptr[loopVar] = toupper(ptr[loopVar]);
    }
    return fopen(engine_gameDataFullPathBuffer, "rb");
}

FILE *CVort_engine_cross_rw_misc_fopen(const char *filename, const char *mode) {
    // TODO: Not modifying path at all for now
    return fopen(filename, mode);
}

FILE *CVort_engine_configpath_fopen(const char *filename, const char *mode) {
    // NOTE For now it simply opens the file given by the name, as-is...
    return fopen(filename, mode);
}

size_t CVort_engine_cross_freadInt8LE(void *ptr, size_t count, FILE *stream) {
    return fread(ptr, 1, count, stream);
}

size_t CVort_engine_cross_freadInt16LE(void *ptr, size_t count, FILE *stream) {
    count = fread(ptr, 2, count, stream);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for (size_t loopVar = 0; loopVar < count; loopVar++, ((uint16_t *) ptr)++)
        *(uint16_t *) ptr = SDL_Swap16(*(uint16_t *) ptr);
#endif
    return count;
}

size_t CVort_engine_cross_freadInt32LE(void *ptr, size_t count, FILE *stream) {
    count = fread(ptr, 4, count, stream);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for (size_t loopVar = 0; loopVar < count; loopVar++, ((uint32_t *) ptr)++)
        *(uint32_t *) ptr = SDL_Swap32(*(uint32_t *) ptr);
#endif
    return count;
}

size_t CVort_engine_cross_fwriteInt8LE(void *ptr, size_t count, FILE *stream) {
    return fwrite(ptr, 1, count, stream);
}

size_t CVort_engine_cross_fwriteInt16LE(void *ptr, size_t count, FILE *stream) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    return fwrite(ptr, 2, count, stream);
#else
    uint16_t val;
    size_t actualCount = 0;
    for (size_t loopVar = 0; loopVar < count; loopVar++, ((uint16_t *) ptr)++) {
        val = SDL_Swap16(*(uint16_t *) ptr);
        actualCount += fwrite(&val, 2, 1, stream);
    }
    return actualCount;
#endif
}

size_t CVort_engine_cross_fwriteInt32LE(void *ptr, size_t count, FILE *stream) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    return fwrite(ptr, 4, count, stream);
#else
    uint32_t val;
    size_t actualCount = 0;
    for (size_t loopVar = 0; loopVar < count; loopVar++, ((uint32_t *) ptr)++) {
        val = SDL_Swap32(*(uint32_t *) ptr);
        actualCount += fwrite(&val, 4, 1, stream);
    }
    return actualCount;
#endif
}

void CVort_engine_cross_logMessage(CVort_Log_Message_Class_T msgClass, const char *format, ...) {
    // TODO: For now we simply do this.
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

// TODO/FIXME: There are almost surely a TON of inaccuracies here, but
// for vanilla UNMODIFIED Keen 1 with no unusual error messages it seems OK.

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
