#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "core/globals.h"

#define CHOCOLATE_KEEN_CFG_FILENAME "chocolate-keen.cfg"
#define CHOCOLATE_KEEN_CFG_LINE_LEN_BOUND 160

bool CVort_engine_parseBool(const char *key, bool *pVal, const char *line) {
    if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
        return false;
    }
    if (!strcmp(line + strlen(key) + 1, "true")) {
        *pVal = true;
        return true;
    }
    if (!strcmp(line + strlen(key) + 1, "false")) {
        *pVal = false;
        return true;
    }
    return false;
}

bool CVort_engine_parseInt(const char *key, int *pVal, const char *line) {
    if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
        return false;
    }
    return sscanf(line + strlen(key) + 1, "%d", pVal) == 1;
}

bool CVort_engine_parseDims(const char *key, int *pWidth, int *pHeight, const char *line) {
    if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
        return false;
    }
    line += strlen(key) + 1;
    return sscanf(line, "%dx%d", pWidth, pHeight) == 2;
}

bool CVort_engine_parseEnum(const char *key, int *pVal, const char **choices, const char *line) {
    if ((strstr(line, key) != line) || (line[strlen(key)] != '=')) {
        return false;
    }
    for (int val = 0; *choices; val++, choices++) {
        if (!strcmp(line + strlen(key) + 1, *choices)) {
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
    if (!strcmp(line + strlen(key) + 1, "auto")) {
        *pVal = -1;
        return true;
    }
    SDL_RendererInfo sdlRendererInfo;
    for (int val = 0; val < SDL_GetNumRenderDrivers(); val++) {
        SDL_GetRenderDriverInfo(val, &sdlRendererInfo);
        if (!strcmp(line + strlen(key) + 1, sdlRendererInfo.name)) {
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
            if (*buffer && (buffer[strlen(buffer) - 1] == '\n')) {
                buffer[strlen(buffer) - 1] = '\0';
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
    for (int counter = 0; counter < val; counter++, choices++)
        ;
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
        // Keep startup/shutdown resilient: failing to persist config is non-fatal.
        return;
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
