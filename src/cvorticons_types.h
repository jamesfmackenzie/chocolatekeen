#ifndef _CHOCOLATE_KEEN_VORTICONS_TYPES_
#define _CHOCOLATE_KEEN_VORTICONS_TYPES_

#include <stdbool.h>
#include <stdint.h>
#include "SDL.h"
#include "chocolate-keen_config.h"
#include "cvorticons_constants.h"
#include "cvorticons_gfx.h"

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
#ifndef NO_SDL_GLEXT
#define NO_SDL_GLEXT
#endif
#include "SDL_opengl.h"
#ifdef WIN32
#include "../windows/include/GL/glext.h"
#else
#include "GL/glext.h"
#endif
#endif

typedef struct {
    int32_t scores[7];
    uint16_t parts[7][4];
    uint16_t targets[7];
    uint16_t unknown[7];
    char names[7][13];
} HighScoresTable_T;

typedef struct {
    uint16_t stuff[9];
    uint16_t levels[16];
    int16_t lives;
    uint16_t ammo;
    int32_t score;
    uint32_t mapX, mapY; // keen wmap location
    uint32_t screenX, screenY; // wmap screen position
    uint16_t targets[8]; // 1 = saved, 0 = not saved
    uint16_t unknown;
} GameProfile_T;

typedef struct {
    int16_t xPoll, yPoll;
} JoystickPoll_T;

typedef struct {
    uint16_t direction, but1jump, but2pogo;
} GameInput_T;

typedef struct {
    int16_t tx, ty; // Coordinates of level (tiles)
    int16_t type_; // Type of level (0 == one square, 1 == four squares)
} Maplevel_T;

typedef struct {
    int32_t destX, destY;
    int16_t isOnSnow;
} Teleporter_T;

typedef enum DemoStatus_T {
    DEMO_OFF = 0, DEMO_PLAY, DEMO_RECORD
} DemoStatus_T;

typedef enum DisplayChar_T {
    CHAR_LINEFEED = 10, CHAR_CARRIAGE = 13, CHAR_EOF = 26, CHAR_SPACE = 32
} DisplayChar_T;

typedef enum LevelEnd_T {
    LEVEL_END_DIE = 0, LEVEL_END_EXIT, LEVEL_END_SECRET, LEVEL_END_TANTALUS
} LevelEnd_T;

typedef enum gfxOutputSystem_T {
    OUTPUTSYS_SURFACE = 0,
#if SDL_VERSION_ATLEAST(2,0,0)
    OUTPUTSYS_TEXTURE,
#else
    OUTPUTSYS_OVERLAY,
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    OUTPUTSYS_OPENGL,
#endif
} gfxOutputSystem_T;

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
typedef enum gfxOutputGLVer_T {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    OUTPUTGL_1_1,
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
    OUTPUTGL_2_0,
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
    OUTPUTGL_ES_2_0,
#endif
} gfxOutputGLVer_T;

// Similar to gfxOutputGLVer_T but used to classify renderer:
// Same code is used for OpenGL 2.0 and ES 2.0,
// possibly with few different function pointers (say for FBO handling).
typedef enum gfxOutputGLClass_T {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    OUTPUTGL_CLASS_1_1,
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    OUTPUTGL_CLASS_2_0,
#endif
} gfxOutputGLClass_T;
#endif

typedef enum gfxScaleType_T {
    GFX_SCALE_ASPECT = 0,
    GFX_SCALE_FILL,
    GFX_SCALE_BOXED,
} gfxScaleType_T;

typedef enum vanillaInitialEmulatedInputDevice_T {
    EMU_INITIAL_INPUT_KEYBOARD, // The one picked in vanilla Keen 1-3
    EMU_INITIAL_INPUT_MOUSE,
    EMU_INITIAL_INPUT_JOY1,
    EMU_INITIAL_INPUT_JOY2,
    EMU_INITIAL_INPUT_LAST
} vanillaInitialEmulatedInputDevice_T;

typedef enum gameversion_T {
    GAMEVER_KEEN1 = 0, GAMEVER_KEEN2, GAMEVER_KEEN3, GAMEVER_TOTALAMOUNT
} gameversion_T;

typedef struct {
    int fullWidth, fullHeight, windowWidth, windowHeight, zoomLevel;
    gfxOutputSystem_T outputSystem;
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    gfxOutputGLVer_T outputGLVersion;
#endif
    gfxScaleType_T scaleType;
    bool doForceCutFullScreen;
    bool isFullscreen, disableSoundSystem;
    int sndSampleRate;
    bool bilinearInterpolation, vSync;
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    bool gpuPaletteCycling;
#endif
#if (defined _CHOCOLATE_KEEN_ENABLE_OPENGL_) || SDL_VERSION_ATLEAST(2,0,0)
    bool offScreenRendering;
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
    int rendererDriverIndex;
    int displayNumber;
#endif
    bool isEmulatedGfxCardVga;
    bool isEgaMonitorInUse; // Maybe a CGA monitor is used with the EGA
    bool alwaysHideCursor, cursorAutoLock;
    bool passKeysToBios; // Never really used but... oh well.

    // Optional settings that may break vanilla Keen behaviors
    struct {
        vanillaInitialEmulatedInputDevice_T initialEmulatedInputDevice;
        bool vorticonsDemoModeToggle;
    } extras;
    // Calculated values not given by the user directly
    struct {
        uint32_t scaledRefreshRate;
        uint16_t overscanGfxLeft, overscanGfxRight, overscanGfxTop, overscanGfxBottom;
        uint16_t gfxHeightScalingFactor; // Used for double-scanned VGA 200-lines mode
        uint16_t overscanTxtLeft, overscanTxtRight, overscanTxtTop, overscanTxtBottom;
        uint16_t txtPixelWidth, txtPixelHeight;
        uint16_t txtCharPixWidth, txtCharPixHeight;
        uint8_t txtCursorBlinkRate, txtBlinkRate;
        const uint8_t *txtFontPtr;
        uint16_t txtTotalScanHeight, txtVertRetraceLen;
        uint16_t gfxTotalScanHeight, gfxVertRetraceLen;
    } calc;
} EngineArguments_T;

typedef enum CVort_Log_Message_Class_T {
    CVORT_LOG_MSG_NORMAL, CVORT_LOG_MSG_WARNING, CVORT_LOG_MSG_ERROR
} CVort_Log_Message_Class_T;

typedef struct {
    struct {
        SDL_Surface *windowSurface;
        SDL_Surface *secondarySurface;
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_Window *window;
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
        SDL_GLContext glContext;
#endif
        Uint32 desktopPixelFormat;
        SDL_Renderer *renderer;
        //int rendererDriverIndex;
        SDL_RendererInfo rendererInfo;
        SDL_Texture *texture, *renderTexture;
        void *texturePixels;
        int texturePitch;
        Uint32 textureFormat;
#else
        SDL_Overlay *overlay;
        const SDL_VideoInfo *videoInfo;
#endif
    } sdl;
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    struct {
        GLuint textureNames[3];
        GLuint texturePixFormat; // 8-bit for palette cycling
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
        GLuint outputCallListName;
        GLsizei numOfCallLists;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
        // Variable pointer to the palette texture name
        GLuint *palTextureName;

        GLuint vertexShader, fragmentShader;
        GLuint shaderProgram;
        // Attribute locations
        GLint positionLoc, texCoordLoc;
        // Sampler location
        GLint samplerLoc;
        // Palette sampler location
        GLint palSamplerLoc;
        // Palette cycling boolean location
        GLint palCyclingIntLoc;
        // Vertex locations and texture coordinates
        GLfloat positionsBuffer[20];
        // Secondary buffer, in case glBlitFramebuffer is not available
        GLfloat secondaryPositionsBuffer[20];
        // Should we do GPU palette cycling?
        bool gpuPaletteCycling;
#endif
        GLuint frameBufferName;
        bool offScreenRendering, haveFramebufferBlit;
        // Effective version of GL used for rendering
        // (not making a difference between GL 2.0 and GL ES 2.0)
        gfxOutputGLClass_T outputGLClass;
    } gl;
#endif
    // Roughly reprenents the current status of the emulated video adapter
    struct {
        // The following is used to simulate the state of EGA video memory
        // in graphics mode, allocating one indexed-color byte per pixel.
        uint8_t byteEgaMemory[ENGINE_EGA_MEMORY_IN_BITS];
        // The state of EGA memory in text mode
        uint8_t egaTxtMemory[ENGINE_EGA_TEXTUAL_MEMORY_IN_BYTES];

        int16_t mouseCursorPosX, mouseCursorPosY;
        uint8_t txtCursorPosX, txtCursorPosY;

        // Acts like ega_regen_start_addr sent to CRTC
        uint32_t currEgaStartAddr;

        // Acts like pel_panning sent to video card
        uint16_t currPanning;

        int16_t currVidMode;

        uint16_t totalScanHeight, vertRetraceLen;

        // Palette and border color, after converting from EGA signals
        uint8_t currParsedPalette[16];
        uint8_t currParsedBorderColor;
    } client;
    struct {
        int zoomFactor;
        // Here, the horizontal overscan strips are TRUNCATED if
        // if VGA is chosen (with 200-line mode double-scalling).
        int clientBorderedWidth, clientBorderedHeight;
        // But these strips are NOT truncated here if
        // the zoom factor is an even number (say 2).
        int clientZoomedBorderedWidth, clientZoomedBorderedHeight;
        // Aspect ratio for contents INCLUDING overscan,
        // if the contents only have 4:3 (used for some calculations)
        int64_t borderedRatioHoriz, borderedRatioVert;
        // Viewport rects inside host window:
        // Without, and with overscan borders.
        SDL_Rect viewportRect, borderedViewportRect;
        // A basic client rect without the overscan border (say with
        // dims 320x200) and an offset of (0,0)
        SDL_Rect clientRect;
        // Client scan line length (384 pixels for graphics mode)
        int clientScanLineLength;
        // The rectangle representing the zoomed client output without
        // the border, but with an offset representing the border.
        // As with clientZoomedBorderedHeight, the horizontal overscan
        // strips are NOT truncated if the zoom factor is an even number.
        SDL_Rect clientOffsettedZoomedRect;
    } dims;
    struct {
        //gfxOutputSystem_T outputSystem;
        int desktopWidth, desktopHeight;
        int fullWidth, fullHeight;
        bool useFullDesktopDims;
        int winWidth, winHeight;
        int bytesPerPixel;
        bool isIndexedColorFormatted;
        //bool isFullscreen;

        bool bilinearInterpolation;

        // The following is used to simulate the state of EGA video memory
        // in graphics mode, for output to physical display.
        // It is also used for text mode graphics output internally.
        // The actual size of the buffer depends on the host/renderer
	// color depth.
        uint8_t *egaMemoryPtr;
        // The 16 (default) EGA colors used when (emulated) mode 0xD/0x3
        // is set, in an implementation-specific format.
        uint8_t *colorTable;
        // The actual colors used after setting a specific palette
        // (roughly represented by EGA signals)
        uint8_t *mappedEgaColorTable;

        int texPitch;
    } host;
} CVort_engine_screen_T;

typedef struct {
    void *chunk;
    int len; // In bytes
} sndChunk_T;

typedef struct {
    uint8_t *trans_map;
    uint8_t *rnd_vals;

    // Used with jump table
    uint16_t *fibs_17;

    int16_t *points_tbl;

    uint8_t *palettes[4];

    uint8_t *char_map;

    uint8_t *uppercase_table;
    //int16_t *win_array_1, *win_array_2, *win_array_3;
    uint8_t *endScreen;
} CVort_EXE_FIELDS_T;

typedef void (*DrawFunction_T)();

#endif
