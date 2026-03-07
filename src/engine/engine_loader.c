// engine_loader.c: Implements engine loader for the engine subsystem.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "core/globals.h"
#include "engine/engine_io.h"
#include "engine/engine_timing.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"
#include "platform/platform.h"
#include "third_party/cgenius/fileio/compression/Cunlzexe.h"

uint16_t CVort_private_continue_game(void);
uint16_t CVort_demo_toggle_prepare_to_record(void);

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
            return true;
        }
    }
    return false;
}

void CVort_engine_prepareEpisodeSpecificFuncPointers(void) {
    size_t numOfTiles;
    switch (engine_gameVersion) {
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
    default:
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Unsupported game version while preparing episode pointers.\n");
        return;
    }
    anim_frame_tiles[0] = (uint16_t *)malloc(4 * numOfTiles * sizeof(uint16_t));
    anim_frame_tiles[1] = anim_frame_tiles[0] + numOfTiles;
    anim_frame_tiles[2] = anim_frame_tiles[0] + 2 * numOfTiles;
    anim_frame_tiles[3] = anim_frame_tiles[0] + 3 * numOfTiles;
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

static KeenVer_Info_T keen_version_info[GAMEVER_TOTALAMOUNT];

void CVort_engine_initializeKeenVerStructs(void) {
    keen_version_info[GAMEVER_KEEN1].exeSize.compressed = KEEN1_EXE_COMPRESSED_SIZE;
    keen_version_info[GAMEVER_KEEN1].exeSize.decompressed = KEEN1_EXE_DECOMPRESSED_SIZE;
    keen_version_info[GAMEVER_KEEN1].exeFilename = KEEN1_EXE_FILENAME;
    keen_version_info[GAMEVER_KEEN2].exeSize.compressed = KEEN2_EXE_COMPRESSED_SIZE;
    keen_version_info[GAMEVER_KEEN2].exeSize.decompressed = KEEN2_EXE_DECOMPRESSED_SIZE;
    keen_version_info[GAMEVER_KEEN2].exeFilename = KEEN2_EXE_FILENAME;
    keen_version_info[GAMEVER_KEEN3].exeSize.compressed = KEEN3_EXE_COMPRESSED_SIZE;
    keen_version_info[GAMEVER_KEEN3].exeSize.decompressed = KEEN3_EXE_DECOMPRESSED_SIZE;
    keen_version_info[GAMEVER_KEEN3].exeFilename = KEEN3_EXE_FILENAME;
}

static bool load_exe_image(gameversion_T gameVer, uint8_t **pExeImageBuffer) {
    uint8_t *wholeExeData;
    CVort_engine_prepareGameDataFilePathBuffers(gameVer);
    FILE *fp = CVort_engine_cross_ro_data_fopen(keen_version_info[gameVer].exeFilename);
    if (!fp)
        return false;
    uint32_t len = CVort_filelength(fp);
    // If len differs from the compressed size, this path treats input as a
    // plain uncompressed MZ image and strips its header.
    wholeExeData = (uint8_t *)malloc(len);
    if (!wholeExeData) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Out of memory for temporary EXE storage!\n");
        return false;
    }
    if (fread(wholeExeData, len, 1, fp) != 1) {
        fclose(fp);
        free(wholeExeData);
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Failed reading EXE image data.\n");
        return false;
    }
    fclose(fp);

    uint32_t headerSize;
    if (len == keen_version_info[gameVer].exeSize.compressed) {
        BYTE *uncompressedExeData;
        Cunlzexe_decompress(wholeExeData, &uncompressedExeData);
        headerSize = Cunlzexe_getHeaderSize();
        *pExeImageBuffer = (uint8_t *)malloc(Cunlzexe_getUncompressedExeSize() - headerSize);
        if (!(*pExeImageBuffer)) {
            free(wholeExeData);
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Out of memory for uncompressed EXE image storage!\n");
            return false;
        }
        memcpy(*pExeImageBuffer, uncompressedExeData + headerSize, Cunlzexe_getUncompressedExeSize() - headerSize);
        Cunlzexe_free(&uncompressedExeData);
    } else { // Non-compressed-size path: treat as uncompressed MZ image.
        headerSize = 16 * (wholeExeData[8] + 16 * wholeExeData[9]);
        *pExeImageBuffer = (uint8_t *)malloc(len - headerSize);
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
    CVort_engine_prepareGameDataFilePathBuffers(gameVer);
    FILE *fp = CVort_engine_cross_ro_data_fopen(keen_version_info[gameVer].exeFilename);
    if (!fp) {
        return false;
    }
    uint32_t len = CVort_filelength(fp);
    fclose(fp);
    return ((len == keen_version_info[gameVer].exeSize.compressed) || (len == keen_version_info[gameVer].exeSize.decompressed));
}

void CVort_engine_loadKeen(gameversion_T gameVer) {
    bool exeLoadedSuccessfully;
    if (engine_forceSpecificEpisode) {
        engine_gameVersion = gameVer;
        exeLoadedSuccessfully = load_exe_image(gameVer, &exeImage);
    } else {
        do { // a Use loop so we can break when
#ifdef CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
            exeLoadedSuccessfully = load_exe_image(GAMEVER_KEEN1, &exeImage);
            if (exeLoadedSuccessfully) {
                engine_gameVersion = GAMEVER_KEEN1;
                break;
            }
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
            exeLoadedSuccessfully = load_exe_image(GAMEVER_KEEN2, &exeImage);
            if (exeLoadedSuccessfully) {
                engine_gameVersion = GAMEVER_KEEN2;
                break;
            }
#endif
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
            exeLoadedSuccessfully = load_exe_image(GAMEVER_KEEN3, &exeImage);
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
            snprintf(lineBuffer, sizeof(lineBuffer), "Sorry, can't find a copy of Commander Keen %d, v1.31.", gameVer + 1);
            CVort_engine_puts(lineBuffer);
        } else {
            // Actually we shouldn't reach this if
            // CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE is defined...
            CVort_engine_puts("Sorry, can't find any supported copy of a Commander Keen episode.");
            CVort_engine_puts("Supported Commander Keen 1/2/3 version: 1.31");
        }

        if (clear_keys_and_check_for_quit()) {
            // Don't call CVort_engine_shutdown:
            // We don't want to save (video) settings.
            // Current behavior intentionally skips settings persistence on this path.
            CVort_engine_shutdownSDL();
            exit(0);
        }
        do {
            if (check_for_user_input_and_quit()) {
                // Don't call CVort_engine_shutdown:
                // We don't want to save (video) settings.
                // Current behavior intentionally skips settings persistence on this path.
                CVort_engine_shutdownSDL();
                exit(0);
            }
            // We cannot use this here - g_input.key_map is not ready!
            // Furthermore, we are waiting for SDL events directly.
            //engine_shortSleep();
            // But let's use this:
            CVort_engine_updateActualDisplay();
            CK_PlatformSleepMs(1);
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

    // Future enhancement: support more output formats and adapt chunk size to
    // the actual opened device format/rate (e.g., from queried audio spec).
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

#ifdef CHOCOLATE_KEEN_CONFIG_DEBUG
    SDL_ShowCursor(engine_arguments.alwaysHideCursor ? SDL_DISABLE : SDL_ENABLE);
#endif
    draw_func = NULL;
    engine_lastScanCode = 0;
    engine_lastKeyTime = 0;
    engine_isBeforeKeyDelay = true;
    engine_doResetMomentToDelay = true;
    engine_refreshTimeInMicroSec = ((int64_t)1000000) * ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR / engine_arguments.calc.scaledRefreshRate;
    engine_sdlMicroTicksDelayOffset = 0;
    engine_gameTicksDelayOffset = 0;
    engine_gameTicksStart = 0;
    CVort_private_engine_setTicks(0);

    CVort_main();
}

void CVort_engine_freeEXE(uint8_t *exeImageBuffer) {
    free(exeImageBuffer);
}
