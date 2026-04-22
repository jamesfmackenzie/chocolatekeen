/*
 * gba_main.c — program entry point for the GBA port.
 *
 * Wires libgba hardware init into the existing engine entry points.
 * The build selects which Keen episode this ROM contains via the
 * -DCHOCOLATE_KEEN_IS_EPISODE{1,2,3}_ENABLED flag set by the Makefile;
 * the block below fixes engine_gameVersion / game_ext accordingly.
 * The launcher is switched off so the DOS startup path runs end-to-end
 * without waiting for a user action.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "SDL.h"
#include "core/globals.h"
#include "engine/engine_api.h"
#include "platform/gba_sram.h"

#include <gba_base.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

static void gba_installInterrupts(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);
}

static void gba_populateDefaultArguments(void) {
    memset(&engine_arguments, 0, sizeof(engine_arguments));
    engine_arguments.fullWidth = 240;
    engine_arguments.fullHeight = 160;
    engine_arguments.windowWidth = 240;
    engine_arguments.windowHeight = 160;
    engine_arguments.zoomLevel = 1;
    engine_arguments.outputSystem = OUTPUTSYS_SURFACE;
    engine_arguments.scaleType = GFX_SCALE_FILL;
    engine_arguments.isFullscreen = true;
    engine_arguments.disableSoundSystem = false;
    engine_arguments.sndSampleRate = 18157;
    engine_arguments.bilinearInterpolation = false;
    engine_arguments.vSync = true;
    engine_arguments.isEmulatedGfxCardVga = true;
    engine_arguments.isEgaMonitorInUse = false;
    engine_arguments.alwaysHideCursor = true;
    engine_arguments.cursorAutoLock = false;
    engine_arguments.passKeysToBios = false;

    engine_skipLauncher = true;
    engine_isLauncherFullscreen = true;
    engine_forceSpecificEpisode = true;
#if defined(CHOCOLATE_KEEN_IS_EPISODE1_ENABLED)
    engine_gameVersion = GAMEVER_KEEN1;
    strncpy(game_ext, "CK1", sizeof(game_ext) - 1);
#elif defined(CHOCOLATE_KEEN_IS_EPISODE2_ENABLED)
    engine_gameVersion = GAMEVER_KEEN2;
    strncpy(game_ext, "CK2", sizeof(game_ext) - 1);
#elif defined(CHOCOLATE_KEEN_IS_EPISODE3_ENABLED)
    engine_gameVersion = GAMEVER_KEEN3;
    strncpy(game_ext, "CK3", sizeof(game_ext) - 1);
#else
#  error "No episode enabled for GBA build"
#endif
    game_ext[sizeof(game_ext) - 1] = '\0';
}

int main(void) {
    gba_installInterrupts();
    CK_SRAM_Init();
    gba_populateDefaultArguments();
    CVort_engine_parseCalculatedEngineArguments();

    if (!CVort_engine_start()) {
        while (1) VBlankIntrWait();
    }

    CVort_engine_shutdown();
    while (1) VBlankIntrWait();
    return 0;
}
