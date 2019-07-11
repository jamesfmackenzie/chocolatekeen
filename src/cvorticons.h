#ifndef _CHOCOLATE_KEEN_VORTICONS_
#define _CHOCOLATE_KEEN_VORTICONS_

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "SDL.h"
#include "chocolate-keen_config.h"
#include "cvorticons_gfx.h"
#include "cvorticons_input.h"


#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
#ifndef NO_SDL_GLEXT
#define NO_SDL_GLEXT
#endif
#include "SDL_opengl.h"
#include "GL/glext.h"
#endif

#define PC_PIT_RATE 1193182 // It is actually 1193181.8181...Hz
// The refresh rate is approximately 70.086Hz
#define ENGINE_VGA_REFRESHRATE_SCALED 70086
#define ENGINE_EGA_REFRESHRATE_SCALED 59920
#define ENGINE_VGA_CURSOR_BLINK_VERT_FRAME_RATE 8
#define ENGINE_EGA_CURSOR_BLINK_VERT_FRAME_RATE 8
#define ENGINE_VGA_TEXT_BLINK_VERT_FRAME_RATE 16
#define ENGINE_EGA_TEXT_BLINK_VERT_FRAME_RATE 16
#define ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR 1000
#define ENGINE_EGA_GFX_WIDTH 320
#define ENGINE_EGA_GFX_HEIGHT 200
#define ENGINE_EGACGA_TXT_PIXEL_WIDTH 640
#define ENGINE_EGACGA_TXT_PIXEL_HEIGHT 200
#define ENGINE_EGA_TXT_PIXEL_WIDTH 640
#define ENGINE_EGA_TXT_PIXEL_HEIGHT 350
#define ENGINE_VGA_TXT_PIXEL_WIDTH 720
#define ENGINE_VGA_TXT_PIXEL_HEIGHT 400
#define ENGINE_EGAVGA_TXT_COLS_NUM 80
#define ENGINE_EGAVGA_TXT_ROWS_NUM 25

#define ENGINE_GUI_WIDTH 320
#define ENGINE_GUI_HEIGHT 240

// A scanline width of 320x2*32=384. This is the length of a scanline used in
// vanilla Keen (where only 320 pixels are actually seen).
// Note that the vanilla code itself sets this by sending the number 24
// to a CRTC register. Apparently, it is obtained by: 24 = (384/8/2).

#define ENGINE_EGA_GFX_SCANLINE_LEN 384
//#define ENGINE_EGA_SCANLINEEXTRA 32 // Leads to a width of 320+2*32=384

// Overscan border dimensions (for each side of the screen)
// For the textual case on the CGA (the adapter), see one of the posts here:
// vogons.zetafleet.com/viewtopic.php?t=9814&postdays=0&postorder=asc&start=60
#define ENGINE_EGA_GFX_OVERSCAN_LEFT 16
#define ENGINE_EGA_GFX_OVERSCAN_RIGHT 40
#define ENGINE_EGA_GFX_OVERSCAN_TOP 22
#define ENGINE_EGA_GFX_OVERSCAN_BOTTOM 24
// EGA textual mode, CGA monitor
#define ENGINE_EGACGA_TXT_OVERSCAN_LEFT 24
#define ENGINE_EGACGA_TXT_OVERSCAN_RIGHT 96
#define ENGINE_EGACGA_TXT_OVERSCAN_TOP 22
#define ENGINE_EGACGA_TXT_OVERSCAN_BOTTOM 24
// EGA textual mode, EGA monitor
#define ENGINE_EGA_TXT_OVERSCAN_LEFT 48
#define ENGINE_EGA_TXT_OVERSCAN_RIGHT 24
#define ENGINE_EGA_TXT_OVERSCAN_TOP 4
#define ENGINE_EGA_TXT_OVERSCAN_BOTTOM 0
// VGA
#define ENGINE_VGA_GFX_OVERSCAN_LEFT 8
#define ENGINE_VGA_GFX_OVERSCAN_RIGHT 8
#define ENGINE_VGA_GFX_OVERSCAN_TOP_AFTER_DOUBLING 7    // 200-line doubling
#define ENGINE_VGA_GFX_OVERSCAN_BOTTOM_AFTER_DOUBLING 7 // 200-line doubling
#define ENGINE_VGA_TXT_OVERSCAN_LEFT 8
#define ENGINE_VGA_TXT_OVERSCAN_RIGHT 8
#define ENGINE_VGA_TXT_OVERSCAN_TOP 7
#define ENGINE_VGA_TXT_OVERSCAN_BOTTOM 7
// Some more relevant info, for the simulation of vertical retrace status
// check; All values are measured in scanlines.
#define ENGINE_EGACGA_VERTICAL_RETRACE_LEN 3
#define ENGINE_EGACGA_TOTAL_SCANLINE_COUNT 262

#define ENGINE_EGA_TXT_VERTICAL_RETRACE_LEN 13
#define ENGINE_EGA_TXT_TOTAL_SCANLINE_COUNT 366

#define ENGINE_VGA_VERTICAL_RETRACE_LEN 2
#define ENGINE_VGA_TOTAL_SCANLINE_COUNT 449

// In the original game, 4 planes of EGA memory are used. For each such plane,
// 6000h=24576 bytes can be used (or 3000h=12288 per page for the page flipping
// feature). This consists of a total of 18000h=98304 bytes. In our case,
// we want to use one such area of memory - and in bytes. That's done
// since in our case, we store one byte per pixel (rather than 4 bits).
// So we want double the area. That is, 30000h bytes.
//
// ...HOWEVER, with just 0x30000 bytes, out-of-bounds write attempts are
// expected. The Apogee logo seen on startup (well, drawn but not yet seen) is
// such an example. So we... double the memory amount for now.
#define ENGINE_EGA_MEMORY_IN_BITS 0x60000
//#define ENGINE_EGA_MEMORY_IN_BITS 0x30000

// Memory for 80x25 text mode, including color attributes
#define ENGINE_EGA_TEXTUAL_MEMORY_IN_BYTES (ENGINE_EGAVGA_TXT_COLS_NUM*ENGINE_EGAVGA_TXT_ROWS_NUM*2)

#define ENGINE_SNDDEFAULTSAMPLERATE 48000
#define ENGINE_SNDBYTEDEPTH 2

#define PALETTE_NUM_OF_ENTRIES 17

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

// FIXME? Should better be a whole enum of tiles...although less useful for MODs
#define TILES_TELESNOW 0x63
#define TILES_JOYSTICK 0xDD
#define TILES_TELEDIRT 0x145
#define TILES_TELEDIRT0 0x152
#define TILES_TELESNOW0 0x156
// FIXME? Same but for sounds
#define TELEPORTSND 0x12

#if 0
// Flags for a function handling (emulated) video mode changes
typedef enum gfxVideoModeFuncCall_T {
    GFX_VIDEOMODE_FUNC_VIDEOMODECHANGE,
    GFX_VIDEOMODE_FUNC_FULLSCREENTOGGLE,
    /*GFX_VIDEOMODE_FUNC_HOSTWINDOWRESIZE,*/
} gfxVideoModeFuncCall_T;
#endif

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

/****************************************************************
Soon we have a complicated case:
- We want to define a Sprite_T type of a struct.
- It would have a pointer to a function that accepts
two pointers to Sprite_T structs as the input.
- We want to work inside an instance of some class (CVorticons).
- Furthermore, we may store a pointer to a function that belongs
to a specific child class (say CVorticons1). A static cast should
solve the latter point, although it looks a bit hackish.
***************************************************************/
/*
 * Source for the latter point:
 * http://stackoverflow.com/questions/3920157/base-classs-function-pointer-points-to-child-classs-member-function
 */

//class CVorticons;

struct SpriteStruct {
    //typedef void (*SpriteThinkFunction_T)();
    //typedef void (*SpriteContactFunction_T)(struct SpriteStruct *owner, struct SpriteStruct * contacted);
    //typedef void (CVorticons::*SpriteThinkFunction_T)();
    //typedef void (CVorticons::*SpriteContactFunction_T)(struct SpriteStruct *owner, struct SpriteStruct * contacted);

    uint16_t type_;
    uint16_t active;
    int32_t posX, posY;
    int32_t boxX1, boxY1, boxX2, boxY2;
    int16_t delX, delY, velX, velY;
    int16_t health;
    uint16_t varA;
    uint16_t frame;
    int16_t time;
    int16_t varB;
    int16_t varC; // Seems to store the old vel_x (i.e. when keen jumps he pauses, then he continues with old vel x)
    int16_t varD; // If false, then think_13_keen_ground checks tiles
    void (*think)();
    void (*contact)(struct SpriteStruct *owner, struct SpriteStruct * contacted);
    //SpriteThinkFunction_T think;
    //SpriteContactFunction_T contact;
};

typedef struct SpriteStruct Sprite_T;
typedef void (*SpriteContactFunction_T)(Sprite_T *owner, Sprite_T * contacted);
typedef void (*SpriteThinkFunction_T)();

/********************************************************
We repeat the same function pointer mess, but for Body_T.
********************************************************/

struct BodyStruct {
    //typedef void (*BodyThinkFunction_T)(struct BodyStruct * body);
    //typedef void (CVorticons::*BodyThinkFunction_T)(struct BodyStruct * body);

    int32_t tile_x, tile_y; // x and y coordinates of body map tile
    uint16_t type_; // the type of the body
    int16_t variant; // the "flavor" of the body, eg ice cannons are variant 0-3 depending on direction
    int16_t field_C, field_E, field_10, field_12, field_14, field_16, field_18, field_1A, field_1C, field_1E, field_20;
    void (*think_ptr)(struct BodyStruct * body);
    //BodyThinkFunction_T think_ptr;
};

typedef struct BodyStruct Body_T;
typedef void (*BodyThinkFunction_T)(Body_T * body);

/**********************************************************
Sadly we need static casts, to work with child classes
like CVorticons1. (We could also declare lots of functions
in CVorticons as virtual, but that may not be much better.)
**********************************************************/

//#define CAST_SPRITE_THINK_FUNC static_cast<Sprite_T::SpriteThinkFunction_T>
//#define CAST_SPRITE_CONTACT_FUNC static_cast<Sprite_T::SpriteContactFunction_T>
//#define CAST_BODY_THINK_FUNC static_cast<Body_T::BodyThinkFunction_T>

//class CVorticons {
//public:
    //void CVorticonsConst(gameversion_T ver) { engine_gameVersion = ver; }
    /*********************
    GUI/Launcher functions
    *********************/
    void CVort_gui_runLoop(void);
    /*****************************
    Engine (lower level) functions
    *****************************/
    bool CVort_engine_start();
    void CVort_engine_loadKeen(gameversion_T gameVer);
    void CVort_engine_shutdown(void);

    /*static*/ bool CVort_engine_isGameExeAvailable(gameversion_T gameVer);

    /*static*/ bool CVort_engine_processArguments(int argc, char **argv);

    extern void (*CVort_ptr_engine_processEXE)();

    void CVort_engine_setupInputMappings(void);
    void CVort_engine_setDefaultInputMappings(void);
    bool CVort_engine_tryToLoadInputMappings(void);
    bool CVort_engine_addInputMappingsFromFile(const char *fileBuffer, const char *tempBuffer, MappedInputEventList_T *pEventList);
    // Records a new mapper binding from the user (host key press / joystick motion / ...)
    // Returns NULL if timed out or there are already MAX_EMU_MAPPINGS_PER_HOST_ENTRY
    // bindings for a given host input, otherwise returns a pointer to a mapping
    // that may be changed directly (say to add modifiers)
    // Sets retInputT, retInputId and retInputVal to values representing
    // captured host input action
    // If there has already been a binding for the given action, it is chosen,
    // otherwise a new one is added
    MappedInputEvent_T *CVort_engine_recordNewInputMapping(EmulatedInput_T emuInput, int value, unsigned int mstimeout, HostInput_T *retInputT, int *retInputId, int *retInputVal);
    // Removes mapping if there's any
    void CVort_engine_deleteInputMapping(EmulatedInput_T emuInput, int emuValue, HostInput_T inputT, int inputId, int inputVal);
    // For a given emulated input event, gets the first mapping after the one
    // given as input, if there's any (possibly the exact same mapping).
    // If there's none, NULL is returned.
    // Otherwise a reference to the mapping that can be modified as returned.
    MappedInputEvent_T *CVort_engine_getNextInputMapping(
        EmulatedInput_T emuInput, int value,
        HostInput_T inputT, int inputId, int inputVal,
        HostInput_T *retInputT, int *retInputId, int *retInputVal
    );
    // Same as getNextInputMapping but traversing in the opposite direction
    MappedInputEvent_T *CVort_engine_getPrevInputMapping(
        EmulatedInput_T emuInput, int value,
        HostInput_T inputT, int inputId, int inputVal,
        HostInput_T *retInputT, int *retInputId, int *retInputVal
    );

    void CVort_engine_saveInputMappings();
    void CVort_engine_writeMappedHostInputs(FILE *fp, const MappedInputEvent_T *pTestEvent);
    void CVort_engine_writeEventModifiers(FILE *fp, int modMask);

    void CVort_engine_handleQuit();
    void CVort_engine_shutdownSDL();
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    bool CVort_engine_preparegl();
#endif

    void CVort_engine_parseCalculatedEngineArguments(void);
    bool CVort_engine_prepareInstructionsScreen();
    void CVort_engine_displayCommandLineHelp();
    //bool CVort_engine_handleCmdLineHelpPageTransition();
    void CVort_engine_decompGraphics();

    int16_t CVort_engine_readGamePort();
    bool CVort_engine_isJoystickInUse();

    extern void (*CVort_ptr_engine_setTicks)(uint32_t currTicks);
    extern uint32_t (*CVort_ptr_engine_getTicks)();
    extern void (*CVort_ptr_engine_setTicksSync)(uint32_t currTicks);
    extern uint32_t (*CVort_ptr_engine_getTicksSync)();

    int64_t CVort_engine_convertMomentFromGameTicksToMicroSec(uint32_t momentInGameTicks);

    /* An amortized delay function, which also updates input status and
     * display contents in the background. In practice, the display
     * contents are updated if there is anything to update on screen and
     * there is also the need to wait.
     * The length is given, approximately, in emulated frames
     * (~70.086 for a second with the default of VGA). It may actually wait
     * a bit longer, depending on OS scheduling. Such lost time is
     * "returned" in later calls to engine_delay, hence the delay is
     * amortized.
     * Note that the latter may not apply if a "long term" sleep is
     * requested, using engine_shortSleep.
     * Further note that this may have an effect on later calls to
     * engine_getTicks(), so the game "thinks" that no time has been lost.
     */
    void CVort_engine_delay(int16_t length);

    /* A replacement for a busy loop that waits until the following condition
     * holds:
     * (engine_getTicks()&0xFFFF)-(ticks_sync&0xFFFF) >= length
     * Or equivalently (for small length):
     * engine_getTicks() - ticks_sync >= length
     * Like engine_delay, the implied delay is amortized and it may have a
     * similar effect on later calls to engine_getTicks().
     */
    void CVort_engine_delayInGameTicks(uint32_t startOfDelayMoment, uint16_t length);

    void CVort_engine_doWaitInterval();
    /* Refreshes the screen contents (even if there is no actual update),
     * sleeps for a bit and then resets internal counters used by
     * engine_delay and engine_delayInGameTicks.
     * This function is used when one may need to wait for something
     * (like reading scrollable text while not actually scrolling).
     */
    void CVort_engine_shortSleep();

    extern void (*CVort_engine_updateEgaGfxNonPalRect_ptr) (uint32_t offset, uint16_t width, uint16_t height);
    extern void (*CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr) (uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    extern void (*CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr) (uint16_t x, uint16_t y, bool isShown);
    extern void (*CVort_engine_doDrawEgaTxtCursor_ptr) (void);
    extern void (*CVort_engine_updateBorderedZoomedRectBuffer_ptr) (uint8_t *buffer, uint32_t egaOffset, uint32_t borderLineOffset);
    extern void (*CVort_engine_updateBorderedZoomedRectBufferBorder_ptr) (uint8_t *buffer);

    extern void (*CVort_engine_gui_drawFontChar_ptr) (int x, int y, int w, int h, const uint8_t *fontCharPtr, int colorNum, int ratio);
    extern void (*CVort_engine_gui_drawRoundedRectBorder_ptr) (int x, int y, int w, int h, int borderColorNum, int innerColorNum, int ratio);

    void CVort_engine_gui_clearScreen(void);
    void CVort_engine_gui_drawColoredLine(int lineNum, int lineLength, int colorNum);
    void CVort_engine_gui_drawColoredColumn(int columnNum, int columnLength, int colorNum);

    void CVort_engine_updateActualDisplay();
    void CVort_engine_setWindowTitleAndIcon();
    bool CVort_engine_prepareScreen(void);
    bool CVort_engine_resetWindow(void);
    bool CVort_engine_setVideoMode(int16_t vidMode);
    void CVort_engine_reactToWindowResize(int width, int height);
#if SDL_VERSION_ATLEAST(2,0,0)
    void CVort_engine_handleWindowSideChange(void);
#endif
    bool CVort_engine_isVerticalBlank();

    void CVort_engine_copyToTxtMemory(uint8_t *buffer);

    void CVort_engine_drawChar(uint16_t x, uint16_t y, uint16_t val);
    void CVort_engine_drawSprite(uint16_t x, uint16_t y, uint16_t num);
    void CVort_engine_drawTile(uint16_t x, uint16_t y, uint16_t num);
    void CVort_engine_drawBitmap(uint16_t x, uint16_t y, uint16_t num);

    uint16_t CVort_engine_drawSpriteAt(int32_t posX, int32_t posY, uint16_t frame);
    uint16_t CVort_engine_drawTileAt(int32_t posX, int32_t posY, uint16_t tilenum);

    void CVort_engine_clearOverlay();
    void CVort_engine_syncDrawing();
    void CVort_engine_drawScreen();
    void CVort_engine_doDrawing();
    void CVort_engine_blitTile(uint16_t num, uint32_t firstPos);
    void CVort_engine_adaptiveTileRefresh(uint16_t initTileIndex);
    void CVort_engine_egaPageFlip();

    void CVort_engine_setBorderColor(uint8_t color);
    void CVort_engine_setPaletteAndBorderColor(const uint8_t *palette);
    void CVort_engine_gotoXY(uint8_t x, uint8_t y);

    void CVort_engine_showImageFile(const char *filename);

    void CVort_engine_loadSounds();
    /*static*/ void CVort_engine_sndCallback(void *unused, uint8_t *stream, int len);
    void CVort_engine_setCurSound(uint16_t sound);
    void CVort_engine_saveCurSound();
    void CVort_engine_restoreCurSound();
    void CVort_engine_finishCurSound();

#if 0
#if SDL_VERSION_ATLEAST(2,0,0)
    uint8_t CVort_engine_convertSDLKeysymToDOSScancode(SDL_Keycode key);
#else
    uint8_t CVort_engine_convertSDLKeysymToDOSScancode(SDLKey key);
#endif
#endif
    void CVort_engine_updateInputStatus();
    void CVort_engine_handleEvent(const MappedInputEvent_T *pMappedEvent, int32_t action);
    void CVort_engine_toggleCursorLock(bool toggle);
    JoystickPoll_T CVort_engine_pollJoystick(int16_t joystickNum);

    /* Used for loading read-only vanilla Keen data */
    /*static*/ FILE *CVort_engine_cross_ro_data_fopen(const char *filename);
    /* Used for loading read/write vanilla Keen files (CTLPANEL and more) */
    /*static*/ FILE *CVort_engine_cross_rw_misc_fopen(const char *filename, const char *mode);
    /* Opens a file used by Chocolate Keen internally (not the "emulated" game)
    from a possibly different location.                                      */
    /*static*/ FILE *CVort_engine_configpath_fopen(const char *filename, const char *mode);
    void CVort_engine_loadConfigFile(void);
    void CVort_engine_saveConfigFile(void);
    /* Used for reading buffers of a specific type, assuming Little-Endian
    byte order in the file's data itself. It gets converted to native order. */
    /*static*/ size_t CVort_engine_cross_freadInt8LE(void *ptr, size_t count, FILE *stream);
    /*static*/ size_t CVort_engine_cross_freadInt16LE(void *ptr, size_t count, FILE *stream);
    /*static*/ size_t CVort_engine_cross_freadInt32LE(void *ptr, size_t count, FILE *stream);
    /* Used for writing buffers of a specific type, converting
    native byte order to Little-Endian order within the file. */
    /*static*/ size_t CVort_engine_cross_fwriteInt8LE(void *ptr, size_t count, FILE *stream);
    /*static*/ size_t CVort_engine_cross_fwriteInt16LE(void *ptr, size_t count, FILE *stream);
    /*static*/ size_t CVort_engine_cross_fwriteInt32LE(void *ptr, size_t count, FILE *stream);

    typedef enum CVort_Log_Message_Class_T {
        CVORT_LOG_MSG_NORMAL, CVORT_LOG_MSG_WARNING, CVORT_LOG_MSG_ERROR
    } CVort_Log_Message_Class_T;
    /* Writes a message to a log in case of some problem */
    /*static*/ void CVort_engine_cross_logMessage(CVort_Log_Message_Class_T msgClass, const char *format, ...);

    int16_t CVort_engine_signExtend8To16(int8_t inputVal);
    int32_t CVort_engine_signExtend16To32(int16_t inputVal);

    int16_t CVort_engine_toupper(int16_t c);
    void CVort_engine_puts(const char *str);

    /**********************
    Mostly ported functions
     **********************/
    void CVort_start_cheating();
    extern void (*CVort_ptr_show_pause_menu)();
    void CVort_handle_cheat_keys();
    void CVort_main();
    extern void (*CVort_ptr_init_level)(uint16_t levelnum);
    void CVort_main_loop();
    void CVort_update_sprite_hitbox();
    void CVort_update_sprite_hitbox_wmap(Sprite_T *sprite);

    /* Given a GameInput_T structure input, simulates a vanilla Keen do-while busy
     * loop waiting as long as the condition (input.but1jump || input.but2pogo)
     * applies The main difference is that a sleep is requested - but NOT on the
     * first iteration of the loop. So, we are actually using a "standard" while
     * loop here.
     * Since this task is repeated often (waiting for keyboard/controller button
     * release) and requires a bit of "special" handling regarding the sleeps, it
     * resides in its own function definition now.
     */
    void CVort_waitForNoGameButtonPress(GameInput_T *pInput);

    void CVort_draw_worldmap();
    void CVort_mark_cities_done();
    void CVort_erase_parents_text();
    void CVort_draw_string_finale(const char *str);
    extern void (*CVort_ptr_draw_win)();

    //extern void (*CVort_ptr_handle_secret_city)();
    //extern void (*CVort_ptr_draw_earth_explode)();

    void CVort_game_over();
    void CVort_show_keens_left();
    void CVort_place_keen_on_worldmap(uint32_t *posX, uint32_t *posY);
    void CVort_do_intro_and_menu();
    uint16_t CVort_draw_title();
    extern uint16_t (*CVort_ptr_continue_game)();
    void CVort_do_story();
    void CVort_do_help();
    void CVort_draw_string_sel(uint16_t type, uint16_t x_pos, uint16_t y_pos, const char *str);
    void CVort_draw_text_viewer_border();
    void CVort_do_text_viewer_short(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset);
    void CVort_do_text_viewer(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset);
    void CVort_draw_menu();
    void CVort_do_start_menu();
    void CVort_do_previews();
    void CVort_show_about_us();
    void CVort_draw_mural();
    void CVort_do_draw_mural();
    void CVort_draw_about_us();
    void CVort_do_about_us();
    extern void (*CVort_ptr_draw_scores)();
    void CVort_do_scores();
    void CVort_show_scores();
    void CVort_save_game();
    void CVort_draw_apogee();
    void CVort_show_logo_text();
    void CVort_scroll_up_logo();
    extern void (*CVort_ptr_show_ordering)(uint16_t isTimed);
    extern void (*CVort_ptr_draw_ordering_info)();
    extern void (*CVort_ptr_do_ordering)();
    void CVort_draw_string_80(const char *str);
    int16_t CVort_check_world_map_col(Sprite_T *sprite);
    void CVort_move_worldmap(GameInput_T input, Sprite_T *spritedraw);
    extern int16_t (*CVort_ptr_worldmap_sprites)(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y);
    void CVort_wait_for_key();

    extern void (*CVort_ptr_inlevel_message)();

    void CVort_add_sprite_keengun(int32_t posX, int32_t posY);
    void CVort_think_zapzot();
    void CVort_think_keengun();
    void CVort_contact_keengun(Sprite_T *keengun, Sprite_T * contacted);
    void CVort_add_sprite_tankshot(int32_t posX, int32_t posY, int16_t velX);
    void CVort_contact_tankshot(Sprite_T *keengun, Sprite_T * contacted);

    void CVort_add_sprite_vorticon(int16_t tileX, int16_t tileY);
    void CVort_add_sprite_youth(int16_t tileX, int16_t tileY);
    void CVort_think_vorticon_walk();
    void CVort_think_vorticon_jump();
    void CVort_think_vorticon_search();
    void CVort_contact_vorticon(Sprite_T *vorticon, Sprite_T *contacted);
    void CVort_think_youth_walk();
    void CVort_think_youth_jump();
    void CVort_contact_youth(Sprite_T *youth, Sprite_T *contacted);

    int16_t CVort_add_sprite();
    int16_t CVort_add_body();

    void CVort_think_keen_ground();
    void CVort_think_keen_jump_ground();
    void CVort_think_keen_jump_air();
    void CVort_think_keen_shoot();
    void CVort_think_keen_pogo_air();
    void CVort_think_keen_pogo_ground();
    void CVort_think_keen_exit();
    void CVort_think_keen_death();
    void CVort_think_keen_stunned();
    extern void (*CVort_ptr_contact_keen)(Sprite_T *keen, Sprite_T * contacted);
    void CVort_body_border_flash(Body_T * body);
    void CVort_body_bridge_extend(Body_T * bridge);
    void CVort_body_bridge_retract(Body_T * bridge);

    // FIXME? This is sorta more vanilla like but not really needed...
    void CVort_contact_nop(Sprite_T *curr, Sprite_T * other);
    void CVort_think_contact_nop(Sprite_T *curr, Sprite_T * other);
    void CVort_body_nop(Body_T * body);

    void CVort_think_dead_sprite();
    void CVort_think_kill_sprite();
    void CVort_think_remove_sprite();
    void CVort_default_think();
    void CVort_default_contact(Sprite_T *curr, Sprite_T * other);

    void CVort_kill_keen();
    void CVort_kill_keen_temp();
    void CVort_move_left_right(int16_t acceleration);
    void CVort_pogo_jump(int16_t max_height, int16_t diff);

    void CVort_check_ceiling();

    void CVort_body_slide_door(Body_T * body);
    void CVort_open_door(int16_t tileX, int16_t tileY);

    void CVort_do_fall();
    int16_t CVort_compute_sprite_delta();
    int16_t CVort_check_ground();
    void CVort_carry_keen(Sprite_T *keen, Sprite_T *contacted);
    void CVort_push_keen(Sprite_T *keen, Sprite_T *pusher);

    void CVort_do_scrolling();
    int16_t CVort_sprite_active_screen();
    int16_t CVort_detect_sprite_col(Sprite_T *spr_0, Sprite_T * spr_1);
    void CVort_keen_bgtile_col();

    int16_t CVort_handle_global_keys();
    void CVort_add_score(int16_t points);
    void CVort_load_level_data(uint16_t levelnum);
    void CVort_handle_quit();
    void CVort_clear_keys();
    void CVort_init_rnd(bool doRand);
    int16_t CVort_get_random();
    /*static*/ uint32_t CVort_filelength(FILE * fp);
    void CVort_setup_jump_heights(uint16_t seed);
    int16_t CVort_calc_jump_height(uint16_t max_height);
    void CVort_chg_vid_and_error(const char *msg);

    uint16_t CVort_draw_level(uint16_t levelnum);

    void CVort_calibrate_joystick(int16_t arg_0);
    void CVort_draw_keyname(int16_t key);
    void CVort_handle_redef_keys();

    void CVort_init_ctrls();
    void CVort_save_ctrls();
    GameInput_T CVort_engine_getKeybCtrlState();
    GameInput_T CVort_engine_getMouseCtrl();
    GameInput_T CVort_engine_getJoystickCtrl(int16_t joy_id);
    GameInput_T CVort_handle_ctrl(uint16_t input_type);
    uint16_t CVort_translate_key(uint16_t type);

    void CVort_draw_box2(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
    void CVort_draw_box(uint16_t width, uint16_t height);
    // Draw a rectangle of the same char
    void CVort_draw_char_rectangle(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t character);
    void CVort_draw_box_opening_main(uint16_t width, uint16_t height);
    void CVort_draw_box_opening_horiz(uint16_t width, uint16_t height);
    void CVort_draw_box_opening_vert(uint16_t width, uint16_t height);

    uint16_t CVort_read_char_with_echo();
    void CVort_draw_string(const char *str);
    void CVort_draw_number_word(uint16_t value);
    void CVort_draw_number_dword(uint32_t value);
    void CVort_draw_stringz(const char *str);
    uint16_t CVort_get_string_input(char *strBuffer, uint16_t len);

    // Vanilla Keen gets some pointer; Either to an external file, or to
    // a location in the EXE. Here we use this.
    void CVort_process_text_file(uint8_t * buffer);
    // Keen 1 specific: Loads text from file first, and then process.
    // In contrary to vanilla Keen 1, THIS function is responsible for
    // about a missing text file, rather than process_text_file.
    void CVort_load_and_process_text_file(const char *filename, uint8_t ** pBuffer);

    uint16_t CVort_draw_text_page(uint8_t *text_src_ptr, int16_t *text_viewer_buffer, int16_t arg_6, int16_t arg_8);
    void CVort_engine_scrollText(int16_t top_line_offs, int16_t bot_line_offs, int16_t direction);
    void CVort_text_viewer_bkgrd(uint16_t box_x_pos, uint16_t box_y_pos, uint8_t *pointer, int16_t *arg_8, int16_t arg_A);
    void CVort_load_high_scores_table();
    void CVort_save_high_scores_table();
    void CVort_fade_in();
    void CVort_fade_out();
    void CVort_lights_on();
    void CVort_lights_out();
    void CVort_toggle_switch();

//protected:

    /***********
    Engine stuff
    ***********/
    extern EngineArguments_T engine_arguments;
    extern /*const*/ gameversion_T engine_gameVersion;
    extern bool engine_skipLauncher, engine_isLauncherFullscreen;
    extern bool engine_forceSpecificEpisode;

    extern InputMappingStruct_T engine_inputMappings;
    extern const EmulatedKey_T engine_emulatedKeysTable[];

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
    extern CVort_engine_screen_T engine_screen;
    // A permenantly constant table of colors
    // in a specific internal format (ARGB or XRGB)
    extern const uint32_t engine_egaRGBColorTable[];

    extern uint8_t engine_currPage;
    extern uint16_t pel_panning;
    extern uint32_t engine_currPageStart; // Acts like page_current_seg
    extern uint32_t engine_dstPage; // Acts like dstseg
    extern uint32_t engine_egaStart; // Acts like ega_regen_start_addr

    extern uint32_t engine_gameTicksStart, engine_gameTicksDelayOffset;
    extern int64_t engine_sdlMicroTicksStart, engine_sdlMicroTicksDelayOffset;
    // A moment in time to wait for, measured in micro seconds
    extern int64_t engine_momentToDelayInMicroSec;
    // The time of a single (emulated) refresh in micro seconds
    extern int64_t engine_refreshTimeInMicroSec;

    /* Use this to reset the time on a call to engine_delay if needed
     * (used on startup or after a call to engine_shortSleep)
     */
    extern bool engine_doResetMomentToDelay;

    extern bool engine_isFrameReadyToDisplay;

    extern uint32_t engine_lastDisplayUpdateTime;

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    /****** OpenGL functions retrieved dynamically if required ******/

    // Common (OpenGL 1.1 / ES 2.0 and compatible)
    typedef const GLubyte * (APIENTRY * glGetString_Func_T)(GLenum);
    typedef void (APIENTRY * glClearColor_Func_T)(GLclampf, GLclampf, GLclampf, GLclampf);
    typedef void (APIENTRY * glViewport_Func_T)(GLint, GLint, GLsizei, GLsizei);
    typedef void (APIENTRY * glGetIntegerv_Func_T)(GLenum, GLint*);
    typedef void (APIENTRY * glEnable_Func_T)(GLenum);
    typedef void (APIENTRY * glGenTextures_Func_T)(GLsizei, GLuint*);
    typedef void (APIENTRY * glBindTexture_Func_T)(GLenum, GLuint);
    typedef void (APIENTRY * glTexParameteri_Func_T)(GLenum, GLenum, GLint);
    typedef void (APIENTRY * glTexImage2D_Func_T)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
    typedef void (APIENTRY * glClear_Func_T)(GLbitfield);
    typedef void (APIENTRY * glDisable_Func_T)(GLenum);

    typedef void (APIENTRY * glDeleteTextures_Func_T)(GLsizei, GLuint*);

    typedef void (APIENTRY * glTexSubImage2D_Func_T)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
    typedef void (APIENTRY * glCopyTexSubImage2D_Func_T)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
    typedef void (APIENTRY * glScissor_Func_T)(GLint, GLint, GLsizei, GLsizei);

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    // OpenGL 1.1 (fixed pipeline, not available in ES 2.0)
    typedef void (APIENTRY * glMatrixMode_Func_T)(GLenum);
    typedef void (APIENTRY * glLoadIdentity_Func_T)(void);
    //typedef void (APIENTRY * glScalef_Func_T)(GLfloat, GLfloat, GLfloat);
    typedef void (APIENTRY * glOrtho_Func_T)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
    typedef GLuint (APIENTRY * glGenLists_Func_T)(GLsizei);
    typedef void (APIENTRY * glNewList_Func_T)(GLuint, GLenum);
    typedef void (APIENTRY * glBegin_Func_T)(GLenum);
    typedef void (APIENTRY * glTexCoord2f_Func_T)(GLfloat, GLfloat);
    typedef void (APIENTRY * glVertex3f_Func_T)(GLfloat, GLfloat, GLfloat);
    typedef void (APIENTRY * glEnd_Func_T)(void);
    typedef void (APIENTRY * glEndList_Func_T)(void);
    typedef void (APIENTRY * glPushAttrib_Func_T)(GLbitfield);
    typedef void (APIENTRY * glPopAttrib_Func_T)(void);
    typedef void (APIENTRY * glDeleteLists_Func_T)(GLuint, GLsizei);
    typedef void (APIENTRY * glCallList_Func_T)(GLuint);

    // OpenGL 1.1 (misc. functions not available in ES 2.0)
    //typedef void (APIENTRY * glDrawBuffer_Func_T) (GLenum);
    //typedef void (APIENTRY * glReadBuffer_Func_T) (GLenum);
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    // OpenGL 1.1 / ES 2.0 (used for GL 2.0 renderer only)
    typedef void (APIENTRY * glDrawElements_Func_T)(GLenum, GLsizei, GLenum, GLvoid*);

    // OpenGL 1.3 / ES 2.0 (used for palette cycling, GL 2.0 renderer only)
    typedef void (APIENTRY * glActiveTexture_Func_T) (GLenum);

    // OpenGL 2.0 / ES 2.0 (shader related functions)
    typedef GLuint (APIENTRY * glCreateShader_Func_T)(GLenum);
    typedef void (APIENTRY * glShaderSource_Func_T)(GLuint, GLsizei, const GLchar*const*, const GLint*);
    typedef void (APIENTRY * glCompileShader_Func_T)(GLuint);
    typedef void (APIENTRY * glGetShaderiv_Func_T)(GLuint, GLenum, GLint*);
    typedef void (APIENTRY * glGetShaderInfoLog_Func_T)(GLuint, GLsizei, GLsizei*, GLchar*);
    typedef void (APIENTRY * glDeleteShader_Func_T)(GLuint);

    typedef GLuint (APIENTRY * glCreateProgram_Func_T)(void);
    typedef void (APIENTRY * glAttachShader_Func_T)(GLuint, GLuint);
    typedef void (APIENTRY * glLinkProgram_Func_T)(GLuint);
    typedef void (APIENTRY * glGetProgramiv_Func_T)(GLuint, GLenum, GLint*);
    typedef void (APIENTRY * glGetProgramInfoLog_Func_T)(GLuint, GLsizei, GLsizei*, GLchar*);
    typedef void (APIENTRY * glDeleteProgram_Func_T)(GLuint);
    typedef void (APIENTRY * glUseProgram_Func_T)(GLuint);

    typedef GLint (APIENTRY * glGetAttribLocation_Func_T)(GLuint, const GLchar*);
    typedef GLint (APIENTRY * glGetUniformLocation_Func_T)(GLuint, const GLchar*);

    typedef void (APIENTRY * glVertexAttribPointer_Func_T)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
    typedef void (APIENTRY * glEnableVertexAttribArray_Func_T)(GLuint);
    typedef void (APIENTRY * glUniform1i_Func_T)(GLint, GLint);
#endif

    // OpenGL 2.0 (not available in ES 2.0)
    //typedef void (APIENTRY *glDrawBuffers_Func_T) (GLsizei, const GLenum *);

    // Framebuffer object (EXT_framebuffer_object, use core for ES 2.0)
    typedef void (APIENTRY * glBindFramebufferEXT_Func_T) (GLenum, GLuint);
    typedef void (APIENTRY * glDeleteFramebuffersEXT_Func_T) (GLsizei, const GLuint *);
    typedef void (APIENTRY * glGenFramebuffersEXT_Func_T) (GLsizei, GLuint *);
    typedef GLenum (APIENTRY * glCheckFramebufferStatusEXT_Func_T) (GLenum);
    typedef void (APIENTRY *glFramebufferTexture2DEXT_Func_T) (GLenum, GLenum, GLenum, GLuint, GLint);

    // Framebuffer blitting (EXT_framebuffer_blit, requires ES 3.0 or other ES extension)
    typedef void (APIENTRY *glBlitFramebufferEXT_Func_T)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

    // Common (OpenGL 1.1 / ES 2.0 and compatible)
    extern glGetString_Func_T engine_glGetString;
    extern glClearColor_Func_T engine_glClearColor;
    extern glViewport_Func_T engine_glViewport;
    extern glGetIntegerv_Func_T engine_glGetIntegerv;
    extern glEnable_Func_T engine_glEnable;
    extern glGenTextures_Func_T engine_glGenTextures;
    extern glBindTexture_Func_T engine_glBindTexture;
    extern glTexParameteri_Func_T engine_glTexParameteri;
    extern glTexImage2D_Func_T engine_glTexImage2D;
    extern glClear_Func_T engine_glClear;
    extern glDisable_Func_T engine_glDisable;

    extern glDeleteTextures_Func_T engine_glDeleteTextures;

    extern glTexSubImage2D_Func_T engine_glTexSubImage2D;
    extern glCopyTexSubImage2D_Func_T engine_glCopyTexSubImage2D;
    extern glScissor_Func_T engine_glScissor;

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    // OpenGL 1.1 (fixed pipeline, not available in ES 2.0)
    extern glMatrixMode_Func_T engine_glMatrixMode;
    extern glLoadIdentity_Func_T engine_glLoadIdentity;
    //extern glScalef_Func_T engine_glScalef;
    extern glOrtho_Func_T engine_glOrtho;
    extern glGenLists_Func_T engine_glGenLists;
    extern glNewList_Func_T engine_glNewList;
    extern glBegin_Func_T engine_glBegin;
    extern glTexCoord2f_Func_T engine_glTexCoord2f;
    extern glVertex3f_Func_T engine_glVertex3f;
    extern glEnd_Func_T engine_glEnd;
    extern glEndList_Func_T engine_glEndList;
    extern glPushAttrib_Func_T engine_glPushAttrib;
    extern glPopAttrib_Func_T engine_glPopAttrib;
    extern glDeleteLists_Func_T engine_glDeleteLists;
    extern glCallList_Func_T engine_glCallList;

    // OpenGL 1.1 (misc. functions not available in ES 2.0)
    //extern glDrawBuffer_Func_T engine_glDrawBuffer;
    //extern glReadBuffer_Func_T engine_glReadBuffer;
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    // OpenGL 1.1 / ES 2.0 (used for GL 2.0 renderer only)
    extern glDrawElements_Func_T engine_glDrawElements;

    // OpenGL 1.3 / ES 2.0 (used for palette cycling, GL 2.0 renderer only)
    extern glActiveTexture_Func_T engine_glActiveTexture;

    // OpenGL 2.0 / ES 2.0 (shader related functions)
    extern glCreateShader_Func_T engine_glCreateShader;
    extern glShaderSource_Func_T engine_glShaderSource;
    extern glCompileShader_Func_T engine_glCompileShader;
    extern glGetShaderiv_Func_T engine_glGetShaderiv;
    extern glGetShaderInfoLog_Func_T engine_glGetShaderInfoLog;
    extern glDeleteShader_Func_T engine_glDeleteShader;

    extern glCreateProgram_Func_T engine_glCreateProgram;
    extern glAttachShader_Func_T engine_glAttachShader;
    extern glLinkProgram_Func_T engine_glLinkProgram;
    extern glGetProgramiv_Func_T engine_glGetProgramiv;
    extern glGetProgramInfoLog_Func_T engine_glGetProgramInfoLog;
    extern glDeleteProgram_Func_T engine_glDeleteProgram;
    extern glUseProgram_Func_T engine_glUseProgram;

    extern glGetAttribLocation_Func_T engine_glGetAttribLocation;
    extern glGetUniformLocation_Func_T engine_glGetUniformLocation;

    extern glVertexAttribPointer_Func_T engine_glVertexAttribPointer;
    extern glEnableVertexAttribArray_Func_T engine_glEnableVertexAttribArray;
    extern glUniform1i_Func_T engine_glUniform1i;
#endif

    // OpenGL 2.0 (not available in ES 2.0)
    //extern glDrawBuffers_Func_T engine_glDrawBuffers;

    // Framebuffer object (EXT_framebuffer_object in 2.0, core in ES 2.0)
    extern glBindFramebufferEXT_Func_T engine_glBindFramebuffer_CoreEXT;
    extern glDeleteFramebuffersEXT_Func_T engine_glDeleteFramebuffers_CoreEXT;
    extern glGenFramebuffersEXT_Func_T engine_glGenFramebuffers_CoreEXT;
    extern glCheckFramebufferStatusEXT_Func_T engine_glCheckFramebufferStatus_CoreEXT;
    extern glFramebufferTexture2DEXT_Func_T engine_glFramebufferTexture2D_CoreEXT;

    // (FIXME HACK) A few Core/EXT agnostic symbols to define
#define ENGINE_GL_FRAMEBUFFER_COREEXT 0x8D40
#define ENGINE_GL_READ_FRAMEBUFFER_COREEXT 0x8CA8
#define ENGINE_GL_DRAW_FRAMEBUFFER_COREEXT 0x8CA9
#define ENGINE_GL_COLOR_ATTACHMENT0_COREEXT 0x8CE0
#define ENGINE_GL_FRAMEBUFFER_COMPLETE_COREEXT 0x8CD5

    // Framebuffer blitting (EXT_framebuffer_blit, requires ES 3.0 or other ES extension)
    extern glBlitFramebufferEXT_Func_T engine_glBlitFramebufferEXT;
#endif
    extern EGAHeadGeneralSection_T engine_egaHeadGeneral;
    extern EGAHeadUnmasked_T *engine_egaHeadUnmasked;
    //extern EGAHeadSprite_T *engine_egaHeadSprite;
    extern MaskedSpriteEntry_T *engine_maskedSpriteEntry;

    // Note: These are not in planar format but rather single-byte per pixel
    extern uint8_t *engine_egaBmpData, *engine_egaFontData, *engine_egaTileData, *engine_egaSpriteData;
    extern uint8_t **engine_egaBmps, **engine_egaFonts, **engine_egaTiles, **engine_egaSprites;

    extern uint16_t engine_sndCount;

    typedef struct {
        void *chunk;
        int len; // In bytes
    } sndChunk_T;
    extern sndChunk_T *sndChunks;
    extern void *sndChunksData;

    extern uint8_t sound_limiter, *sndPriorities;
    extern int engine_currSoundLocation;
    extern int16_t engine_currSoundPlaying;

#if 0
    // EGA color table, in RGBA/ABGR format. To be more specific,
    // it is RGBA on Big-Endian platforms and ABGR on Little-Endian ones.
    // TODO: Make this more cross-platform and work with other color depths?
    const uint32_t engine_egaABGRColorTableLE[] = {
        0X00000000, 0x0000aa00, 0x00aa0000, 0x00aaaa00, 0xaa000000, 0xaa00aa00, 0xaaaa0000, 0xaaaaaa00,
        0x00005500, 0x0000ff00, 0x00aa5500, 0x00aaff00, 0xaa005500, 0xaa00ff00, 0xaaaa5500, 0xaaaaff00,
        0x00550000, 0x0055aa00, 0x00ff0000, 0x00ffaa00, 0xaa550000, 0xaa55aa00, 0xaaff0000, 0xaaffaa00,
        0x00555500, 0x0055ff00, 0x00ff5500, 0x00ffff00, 0xaa555500, 0xaa55ff00, 0xaaff5500, 0xaaffff00,
        0x55000000, 0x5500aa00, 0x55aa0000, 0x55aaaa00, 0xff000000, 0xff00aa00, 0xffaa0000, 0xffaaaa00,
        0x55005500, 0x5500ff00, 0x55aa5500, 0x55aaff00, 0xff005500, 0xff00ff00, 0xffaa5500, 0xffaaff00,
        0x55550000, 0x5555aa00, 0x55ff0000, 0x55ffaa00, 0xff550000, 0xff55aa00, 0xffff0000, 0xffffaa00,
        0x55555500, 0x5555ff00, 0x55ff5500, 0x55ffff00, 0xff555500, 0xff55ff00, 0xffff5500, 0xffffff00
    };
#endif

    extern uint8_t engine_lastScanCode;
    extern uint32_t engine_lastKeyTime;
    extern bool engine_isBeforeKeyDelay;

    extern bool engine_isCursorLocked;
 
    // EXE file data (the uncompressed EXE without the header)
    extern uint8_t *exeImage;

    // Things that get loaded from the original EXE
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
    extern CVort_EXE_FIELDS_T exeFields;

#if 0
#if VORTICONS_EPISODE == 1
    struct {
        uint32_t *ordering_sprites_x, *ordering_sprites_y;
        uint16_t *ordering_sprites_frames;

        // Actually, win3 is used before win1.
        int16_t *win1, *win2, *win3;
    } exeFieldsEp1;
#elif VORTICONS_EPISODE == 2
    struct {
        uint16_t *earth_expl3, *earth_explX, *earth_explY, *earth_expl;
    } exeFieldsEp2;
#endif
#endif

    /************************************
    Vanilla Keen variables; Well, mostly.
     ************************************/
    // Initialized Variables
#if 0
    const static uint8_t rnd_vals[];
    const static uint8_t trans_map[];
    const static uint8_t char_map[];
    const static uint8_t uppercase_table[];
    const static int16_t points_tbl[];
    const static uint8_t palettes[][17];
#endif
    extern uint8_t key_map[128]; // All zeros
#if 0
    const static uint16_t fibs_17[];
#endif
    extern uint16_t jump_height_table[18];
    extern uint16_t spritejump_1, spritejump_2;

    extern char game_ext[4];
    extern const uint32_t pitAltCounter;

    // BSS 
    extern uint32_t ticks_sync;
    extern uint16_t sprite_sync;
    extern int16_t god_mode, keen_invincible;
    extern int16_t level_finished;
    // TODO: Using this in "modern" code is uncommon today...
    extern char string_buf[0x50];

    extern int16_t joy_detect;
    extern int16_t pass_keys_to_bios;
    //uint16_t pass_keys_to_bios;
    extern uint16_t key_code, key_scane;

    extern uint16_t sound_disabled;
    extern int16_t rnd;
    extern uint16_t tiledraws_c, bmpdraws_c, spritedraws_c;
    // Same for bmpdraws and more
    extern SpriteDraw_T spritedraws[0x1f4];
    extern BmpDraw_T bmpdraws[0xa];
    extern TileDraw_T tiledraws[0x64];
    extern uint16_t screentiles[0x258];
    // TODO: bmpdraws_i (bmpdraws_p) doesn't seem to actually be used...
    extern int bmpdraws_i, tiledraws_i, screentiles_i, spritedraws_i;

    extern uint16_t want_sound;

    extern int16_t mouse_ctrl_1;
    // Original reference names to the four joystick arrays:
    // word_2B2BA, word_2B2C0, word_2B2C6, word_2B2CC
    extern int16_t joystick_ctrl[4][3];

    extern uint16_t *TILEINFO_Anim;
    extern int16_t *TILEINFO_Type;
    extern int16_t *TILEINFO_UEdge, *TILEINFO_REdge, *TILEINFO_DEdge, *TILEINFO_LEdge;
    
    /* NOTE: This is a two-dimensional array of a variable size, depending on
     * the episode. Each episode has its own static "implementation" of the array.
     * We could actually use anim_plane, but there are references to
     * anim_frame_tiles in the game code...
     */
    extern uint16_t * anim_frame_tiles[4];

    extern uint16_t * anim_plane[4];
    extern int anim_plane_i;

    extern uint8_t *sound_data;

    extern uint16_t ATR[2][0x14A];

    extern uint8_t demo_actions_including_level_num[5001];
    extern uint8_t *demo_action_ptr, *end_of_demo_ptr, *demo_after_last_byte_char_offset;
    extern int16_t demo_status;

    extern int16_t map_data[0x10000 / 2], *map_data_tiles, *map_data_sprites;
    extern int16_t map_width_T, map_height_T, map_width_B;
    extern uint16_t screen_wrap_single;
    //extern uint16_t screen_wrap;
    extern int32_t map_width, map_height;
    extern int32_t scrollX, scrollY, scrollX_min, scrollY_min, scrollX_max, scrollY_max, ceilingX, ceilingY;
    extern int16_t scrollX_T, scrollY_T;
    extern uint16_t cursorX, cursorX_save, cursorY;

    extern uint32_t cursorX_b, cursorY_b; // Cursor position in map units

    extern uint16_t textbox_x_pos, textbox_width, textbox_y_pos, textbox_height;
    extern uint16_t textbox_offs_x, textbox_offs_y;

    extern int16_t keen_tileX, keen_tileY;

    extern Sprite_T sprites[0x50], temp_sprite;
    extern Body_T bodies[0x10];

    extern GameInput_T input_new, input_old;

    extern int16_t num_sprites, num_bodies;

    extern int16_t spark_counter;

    extern uint16_t quit_to_title;

    extern int16_t wmap_sprite_on;

    extern int32_t extra_life_pts;

    extern MaskedSpriteEntry_T temp_MSE;

    extern uint16_t anim_speed;

    extern uint16_t ctrl_type[3];
    extern uint8_t sc_dir[8], sc_but1, sc_but2;

    extern uint16_t lights;

    extern uint8_t *previews_txt, *story_text, *help_text, *end_text;

    extern HighScoresTable_T high_scores_table;
    extern Sprite_T keen_map[2];
    extern GameProfile_T keen_gp;

    extern uint16_t on_world_map;
    extern uint16_t resuming_saved_game;
    extern uint16_t current_level;

    extern uint16_t reshow_scroll_up;

    extern uint16_t wmap_col;

    extern int16_t keen_facing;

    extern uint16_t apogee_bmp_height, intro_complete;

    extern uint32_t keen_wmap_x_pos, keen_wmap_y_pos;
    extern uint32_t wmap_scrollX, wmap_scrollY;
    extern Maplevel_T wmaplevels[0x10];

    typedef void (*DrawFunction_T)();
    //typedef void (CVorticons::*DrawFunction_T)();
    extern DrawFunction_T draw_func;

    extern uint16_t text_viewer_top_pos, text_viewer_height, text_viewer_bottom;
    extern uint8_t *text_ptr;

    extern int16_t text_viewer_buffer[0x190], *text_viewer_buffer_ptr;

    extern int16_t keen_switch;

    extern uint16_t messie_mounted, messie_frame, messie_time_to_climb, messie_move_tics;
    extern uint16_t messie_x_T, messie_y_T;
    extern int32_t messie_xpos, messie_ypos, messie_delX, messie_delY;

    extern Teleporter_T teleporters[16];
//};

#endif
