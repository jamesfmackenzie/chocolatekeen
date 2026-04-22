/*
 * gba_sdl.h — SDL2 compatibility shim for the GBA port of Chocolate Keen.
 *
 * The GBA has no SDL implementation. Rather than rewrite the engine's
 * SDL call-sites, we provide this shim: a small subset of SDL types,
 * constants, and function declarations that the engine touches.
 *
 * - Types are concrete enough to be poked at (e.g. SDL_Event.window.data1).
 * - Functions are real symbols declared here and defined in gba_sdl.c as
 *   no-ops, returning the values the engine's success paths expect.
 * - The hot-path renderer, audio callback, and launcher UI are NOT handled
 *   through this shim; they are replaced wholesale by *_gba.c files that
 *   re-export the same CVort_* symbols.
 *
 * The header is force-included via `-include` from build/gba/Makefile so
 * that source files that `#include "SDL.h"` transparently pick it up.
 */

#ifndef CHOCOLATE_KEEN_GBA_SDL_H
#define CHOCOLATE_KEEN_GBA_SDL_H

#ifndef CHOCOLATE_KEEN_TARGET_GBA
#error "gba_sdl.h should only be force-included in the GBA build"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Re-route libc file I/O used by the engine through our in-ROM FILE shim.
 * The shim satisfies fopen/fread/fseek/ftell/fclose/fgetc/fprintf/fgets/
 * fputs/fwrite/feof/rewind, which covers every call site in the engine.
 */
#include "platform/gba_fileio.h"

/* ------------------------------------------------------------------
 * Version macros. SDL 2.0.0+ branches are the supported ones.
 * ------------------------------------------------------------------ */
#define SDL_MAJOR_VERSION 2
#define SDL_MINOR_VERSION 0
#define SDL_PATCHLEVEL    10
#define SDL_VERSIONNUM(X, Y, Z) ((X) * 1000 + (Y) * 100 + (Z))
#define SDL_COMPILEDVERSION SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL)
#define SDL_VERSION_ATLEAST(X, Y, Z) (SDL_COMPILEDVERSION >= SDL_VERSIONNUM(X, Y, Z))

/* ------------------------------------------------------------------
 * Core primitive types.
 * ------------------------------------------------------------------ */
typedef uint8_t  Uint8;
typedef int8_t   Sint8;
typedef uint16_t Uint16;
typedef int16_t  Sint16;
typedef uint32_t Uint32;
typedef int32_t  Sint32;
typedef uint64_t Uint64;
typedef int64_t  Sint64;
typedef int      SDL_bool;

#define SDL_TRUE  1
#define SDL_FALSE 0

/* Byte-order (GBA is little-endian). */
#define SDL_LIL_ENDIAN 1234
#define SDL_BIG_ENDIAN 4321
#define SDL_BYTEORDER  SDL_LIL_ENDIAN

static inline Uint16 SDL_Swap16(Uint16 x) { return (Uint16)((x << 8) | (x >> 8)); }
static inline Uint32 SDL_Swap32(Uint32 x) {
    return ((x & 0xFFu) << 24) | ((x & 0xFF00u) << 8) |
           ((x & 0xFF0000u) >> 8) | ((x & 0xFF000000u) >> 24);
}
#define SDL_SwapLE16(x) ((Uint16)(x))
#define SDL_SwapLE32(x) ((Uint32)(x))
#define SDL_SwapBE16(x) SDL_Swap16((Uint16)(x))
#define SDL_SwapBE32(x) SDL_Swap32((Uint32)(x))

/* ------------------------------------------------------------------
 * Init / subsystem flags.
 * ------------------------------------------------------------------ */
#define SDL_INIT_TIMER          0x00000001u
#define SDL_INIT_AUDIO          0x00000010u
#define SDL_INIT_VIDEO          0x00000020u
#define SDL_INIT_JOYSTICK       0x00000200u
#define SDL_INIT_HAPTIC         0x00001000u
#define SDL_INIT_GAMECONTROLLER 0x00002000u
#define SDL_INIT_EVENTS         0x00004000u
#define SDL_INIT_EVERYTHING     0x0000FFFFu

#define SDL_ENABLE  1
#define SDL_DISABLE 0
#define SDL_QUERY  -1
#define SDL_IGNORE  0
#define SDL_PRESSED 1
#define SDL_RELEASED 0

/* Grab-input enum (SDL1 legacy, only referenced in commented-out code). */
#define SDL_GRAB_OFF 0
#define SDL_GRAB_ON  1

/* ------------------------------------------------------------------
 * Minimal geometry/color/surface stubs.
 * ------------------------------------------------------------------ */
typedef struct { int x, y, w, h; } SDL_Rect;
typedef struct { Uint8 r, g, b, a; } SDL_Color;

typedef struct SDL_PixelFormat {
    Uint32 format;
    Uint8  BitsPerPixel;
    Uint8  BytesPerPixel;
    Uint32 Rmask, Gmask, Bmask, Amask;
} SDL_PixelFormat;

typedef struct SDL_Surface {
    Uint32 flags;
    SDL_PixelFormat *format;
    int    w, h, pitch;
    void  *pixels;
} SDL_Surface;

typedef void SDL_Window;
typedef void SDL_Renderer;
typedef void SDL_Texture;
typedef void SDL_Joystick;
typedef void SDL_GameController;
typedef void *SDL_GLContext;
typedef void SDL_Overlay;
typedef struct SDL_VideoInfo SDL_VideoInfo;
typedef Sint32 SDL_JoystickID;
typedef struct SDL_Cursor SDL_Cursor;

typedef struct SDL_RendererInfo {
    const char *name;
    Uint32 flags;
    Uint32 num_texture_formats;
    Uint32 texture_formats[16];
    int    max_texture_width;
    int    max_texture_height;
} SDL_RendererInfo;

typedef struct SDL_DisplayMode {
    Uint32 format;
    int w, h, refresh_rate;
    void *driverdata;
} SDL_DisplayMode;

typedef struct SDL_AudioSpec {
    int freq;
    Uint16 format;
    Uint8 channels;
    Uint8 silence;
    Uint16 samples;
    Uint32 size;
    void (*callback)(void *userdata, Uint8 *stream, int len);
    void *userdata;
} SDL_AudioSpec;

#define AUDIO_U8     0x0008
#define AUDIO_S16LSB 0x8010
#define AUDIO_S16SYS AUDIO_S16LSB

/* Audio status. */
typedef enum {
    SDL_AUDIO_STOPPED = 0,
    SDL_AUDIO_PLAYING,
    SDL_AUDIO_PAUSED
} SDL_AudioStatus;

/* ------------------------------------------------------------------
 * Events — only the fields the engine actually reads.
 * ------------------------------------------------------------------ */
typedef enum {
    SDL_FIRSTEVENT        = 0,
    SDL_QUIT              = 0x100,
    SDL_APP_TERMINATING,
    SDL_WINDOWEVENT       = 0x200,
    SDL_SYSWMEVENT,
    SDL_KEYDOWN           = 0x300,
    SDL_KEYUP,
    SDL_TEXTEDITING,
    SDL_TEXTINPUT,
    SDL_MOUSEMOTION       = 0x400,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEWHEEL,
    SDL_JOYAXISMOTION     = 0x600,
    SDL_JOYBALLMOTION,
    SDL_JOYHATMOTION,
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
    SDL_JOYDEVICEADDED,
    SDL_JOYDEVICEREMOVED,
    SDL_CONTROLLERAXISMOTION = 0x650,
    SDL_CONTROLLERBUTTONDOWN,
    SDL_CONTROLLERBUTTONUP,
    SDL_CONTROLLERDEVICEADDED,
    SDL_CONTROLLERDEVICEREMOVED,
    SDL_FINGERDOWN        = 0x700,
    SDL_FINGERUP,
    SDL_FINGERMOTION,
    SDL_USEREVENT         = 0x8000,
    /* SDL1 legacy token referenced in a preprocessor branch. */
    SDL_VIDEORESIZE       = 0x7FFE
} SDL_EventType;

typedef enum {
    SDL_WINDOWEVENT_NONE,
    SDL_WINDOWEVENT_SHOWN,
    SDL_WINDOWEVENT_HIDDEN,
    SDL_WINDOWEVENT_EXPOSED,
    SDL_WINDOWEVENT_MOVED,
    SDL_WINDOWEVENT_RESIZED,
    SDL_WINDOWEVENT_SIZE_CHANGED,
    SDL_WINDOWEVENT_MINIMIZED,
    SDL_WINDOWEVENT_MAXIMIZED,
    SDL_WINDOWEVENT_RESTORED,
    SDL_WINDOWEVENT_ENTER,
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_GAINED,
    SDL_WINDOWEVENT_FOCUS_LOST,
    SDL_WINDOWEVENT_CLOSE
} SDL_WindowEventID;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint8  event;
    Uint8  padding1, padding2, padding3;
    Sint32 data1;
    Sint32 data2;
} SDL_WindowEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    int    w, h;
} SDL_ResizeEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint8  state;
    Uint8  repeat;
    Uint8  padding2, padding3;
    struct {
        Sint32 scancode;
        Sint32 sym;
        Uint16 mod;
        Uint32 unused;
    } keysym;
} SDL_KeyboardEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint32 which;
    Uint8  button;
    Uint8  state;
    Uint8  clicks;
    Uint8  padding1;
    Sint32 x, y;
} SDL_MouseButtonEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    SDL_JoystickID which;
    Uint8  button;
    Uint8  state;
    Uint8  padding1, padding2;
} SDL_JoyButtonEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    SDL_JoystickID which;
    Uint8  axis;
    Uint8  padding1, padding2, padding3;
    Sint16 value;
    Uint16 padding4;
} SDL_JoyAxisEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    SDL_JoystickID which;
    Uint8  hat;
    Uint8  value;
    Uint8  padding1, padding2;
} SDL_JoyHatEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    SDL_JoystickID which;
    Uint8  axis;
    Uint8  padding1, padding2, padding3;
    Sint16 value;
    Uint16 padding4;
} SDL_ControllerAxisEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    SDL_JoystickID which;
    Uint8  button;
    Uint8  state;
    Uint8  padding1, padding2;
} SDL_ControllerButtonEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    Sint32 which;
} SDL_ControllerDeviceEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    Sint32 which;
} SDL_JoyDeviceEvent;

typedef struct {
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint32 which;
    Uint32 state;
    Sint32 x, y;
    Sint32 xrel, yrel;
} SDL_MouseMotionEvent;

#define SDL_BUTTON_LEFT   1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT  3
#define SDL_BUTTON_X1     4
#define SDL_BUTTON_X2     5

#define SDL_HAT_CENTERED  0x00
#define SDL_HAT_UP        0x01
#define SDL_HAT_RIGHT     0x02
#define SDL_HAT_DOWN      0x04
#define SDL_HAT_LEFT      0x08
#define SDL_HAT_RIGHTUP   (SDL_HAT_RIGHT | SDL_HAT_UP)
#define SDL_HAT_RIGHTDOWN (SDL_HAT_RIGHT | SDL_HAT_DOWN)
#define SDL_HAT_LEFTUP    (SDL_HAT_LEFT  | SDL_HAT_UP)
#define SDL_HAT_LEFTDOWN  (SDL_HAT_LEFT  | SDL_HAT_DOWN)

typedef union {
    Uint32 type;
    SDL_WindowEvent      window;
    SDL_KeyboardEvent    key;
    SDL_MouseButtonEvent button;
    SDL_JoyButtonEvent   jbutton;
    SDL_JoyAxisEvent     jaxis;
    SDL_JoyHatEvent      jhat;
    SDL_JoyDeviceEvent   jdevice;
    SDL_ControllerAxisEvent   caxis;
    SDL_ControllerButtonEvent cbutton;
    SDL_ControllerDeviceEvent cdevice;
    SDL_MouseMotionEvent motion;
    SDL_ResizeEvent      resize;
    Uint8 padding[64];
} SDL_Event;

/* ------------------------------------------------------------------
 * Scancode / Keycode minimal tables. The mapper consumes hundreds of
 * these names; providing them as an enum is cheap and keeps the
 * existing default-mapping tables compiling unchanged.
 * ------------------------------------------------------------------ */
typedef Sint32 SDL_Keycode;
typedef enum {
    SDL_SCANCODE_UNKNOWN = 0,
    SDL_SCANCODE_A = 4, SDL_SCANCODE_B, SDL_SCANCODE_C, SDL_SCANCODE_D,
    SDL_SCANCODE_E, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H,
    SDL_SCANCODE_I, SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_L,
    SDL_SCANCODE_M, SDL_SCANCODE_N, SDL_SCANCODE_O, SDL_SCANCODE_P,
    SDL_SCANCODE_Q, SDL_SCANCODE_R, SDL_SCANCODE_S, SDL_SCANCODE_T,
    SDL_SCANCODE_U, SDL_SCANCODE_V, SDL_SCANCODE_W, SDL_SCANCODE_X,
    SDL_SCANCODE_Y, SDL_SCANCODE_Z,
    SDL_SCANCODE_1 = 30, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
    SDL_SCANCODE_9, SDL_SCANCODE_0,
    SDL_SCANCODE_RETURN = 40, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_TAB, SDL_SCANCODE_SPACE, SDL_SCANCODE_MINUS, SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_LEFTBRACKET, SDL_SCANCODE_RIGHTBRACKET, SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_NONUSHASH, SDL_SCANCODE_SEMICOLON, SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_GRAVE, SDL_SCANCODE_COMMA, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_F1 = 58, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4,
    SDL_SCANCODE_F5, SDL_SCANCODE_F6, SDL_SCANCODE_F7, SDL_SCANCODE_F8,
    SDL_SCANCODE_F9, SDL_SCANCODE_F10, SDL_SCANCODE_F11, SDL_SCANCODE_F12,
    SDL_SCANCODE_PRINTSCREEN, SDL_SCANCODE_SCROLLLOCK, SDL_SCANCODE_PAUSE,
    SDL_SCANCODE_INSERT, SDL_SCANCODE_HOME, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_DELETE,
    SDL_SCANCODE_END, SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_RIGHT = 79, SDL_SCANCODE_LEFT, SDL_SCANCODE_DOWN, SDL_SCANCODE_UP,
    SDL_SCANCODE_NUMLOCKCLEAR, SDL_SCANCODE_KP_DIVIDE, SDL_SCANCODE_KP_MULTIPLY,
    SDL_SCANCODE_KP_MINUS, SDL_SCANCODE_KP_PLUS, SDL_SCANCODE_KP_ENTER,
    SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_3, SDL_SCANCODE_KP_4,
    SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8,
    SDL_SCANCODE_KP_9, SDL_SCANCODE_KP_0, SDL_SCANCODE_KP_PERIOD,
    SDL_SCANCODE_LCTRL = 224, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_LALT, SDL_SCANCODE_LGUI,
    SDL_SCANCODE_RCTRL, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_RALT, SDL_SCANCODE_RGUI,
    /* Default SDL2 reserves 512 scancode slots. The highest we actually
     * emit is SDL_SCANCODE_RGUI (231); trimming to 232 saves tens of KiB
     * in InputMappingStruct_T.keyMappings, which matters on the GBA. */
    SDL_NUM_SCANCODES = 232
} SDL_Scancode;

#define SDLK_SCANCODE_MASK (1<<30)
#define SDL_SCANCODE_TO_KEYCODE(X) ((SDL_Keycode)(X) | SDLK_SCANCODE_MASK)
#define SDLK_UNKNOWN    0
#define SDLK_RETURN     '\r'
#define SDLK_ESCAPE     '\x1B'
#define SDLK_BACKSPACE  '\b'
#define SDLK_TAB        '\t'
#define SDLK_SPACE      ' '
#define SDLK_F1         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1)
#define SDLK_F2         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2)
#define SDLK_F3         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3)
#define SDLK_F4         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4)
#define SDLK_UP         SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP)
#define SDLK_DOWN       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN)
#define SDLK_LEFT       SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT)
#define SDLK_RIGHT      SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT)

#define KMOD_NONE   0x0000
#define KMOD_LSHIFT 0x0001
#define KMOD_RSHIFT 0x0002
#define KMOD_LCTRL  0x0040
#define KMOD_RCTRL  0x0080
#define KMOD_LALT   0x0100
#define KMOD_RALT   0x0200
#define KMOD_SHIFT  (KMOD_LSHIFT|KMOD_RSHIFT)
#define KMOD_CTRL   (KMOD_LCTRL|KMOD_RCTRL)
#define KMOD_ALT    (KMOD_LALT|KMOD_RALT)

/* ------------------------------------------------------------------
 * Pixel format FourCC-style IDs the engine_config probe references.
 * ------------------------------------------------------------------ */
#define SDL_PIXELFORMAT_UNKNOWN     0
#define SDL_PIXELFORMAT_INDEX8      0x13000001u
#define SDL_PIXELFORMAT_RGB24       0x17101803u
#define SDL_PIXELFORMAT_BGR24       0x17401803u
#define SDL_PIXELFORMAT_RGB888      0x16161804u
#define SDL_PIXELFORMAT_RGBA8888    0x16462004u
#define SDL_PIXELFORMAT_ARGB8888    0x16362004u
#define SDL_PIXELFORMAT_RGB565      0x15151002u
#define SDL_PIXELFORMAT_BGR565      0x15551002u

/* ------------------------------------------------------------------
 * Hint / log / mouse constants.
 * ------------------------------------------------------------------ */
#define SDL_HINT_RENDER_DRIVER "SDL_RENDER_DRIVER"

typedef enum {
    SDL_HINT_DEFAULT = 0,
    SDL_HINT_NORMAL,
    SDL_HINT_OVERRIDE
} SDL_HintPriority;

/* ------------------------------------------------------------------
 * Function shim declarations. Real definitions live in gba_sdl.c.
 * ------------------------------------------------------------------ */
#ifdef __cplusplus
extern "C" {
#endif

int        SDL_Init(Uint32 flags);
int        SDL_InitSubSystem(Uint32 flags);
void       SDL_Quit(void);
void       SDL_QuitSubSystem(Uint32 flags);
const char *SDL_GetError(void);
void       SDL_ClearError(void);
int        SDL_SetError(const char *fmt, ...);
Uint32     SDL_GetTicks(void);
void       SDL_Delay(Uint32 ms);

int        SDL_PollEvent(SDL_Event *event);
int        SDL_PushEvent(SDL_Event *event);
void       SDL_PumpEvents(void);

int        SDL_NumJoysticks(void);
SDL_Joystick *SDL_JoystickOpen(int index);
void       SDL_JoystickClose(SDL_Joystick *joy);
const char *SDL_JoystickName(SDL_Joystick *joy);
int        SDL_JoystickEventState(int state);
int        SDL_JoystickNumAxes(SDL_Joystick *joy);
int        SDL_JoystickNumButtons(SDL_Joystick *joy);
int        SDL_JoystickNumHats(SDL_Joystick *joy);
int        SDL_JoystickNumBalls(SDL_Joystick *joy);
SDL_JoystickID SDL_JoystickInstanceID(SDL_Joystick *joy);
Uint8      SDL_JoystickGetHat(SDL_Joystick *joy, int hat);
Sint16     SDL_JoystickGetAxis(SDL_Joystick *joy, int axis);
Uint8      SDL_JoystickGetButton(SDL_Joystick *joy, int button);

int        SDL_ShowCursor(int toggle);
void       SDL_SetRelativeMouseMode(SDL_bool enabled);
Uint32     SDL_GetRelativeMouseState(int *x, int *y);
Uint32     SDL_GetMouseState(int *x, int *y);
void       SDL_WM_GrabInput(int mode);

int        SDL_GetNumRenderDrivers(void);
int        SDL_GetRenderDriverInfo(int index, SDL_RendererInfo *info);

int        SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
void       SDL_CloseAudio(void);
void       SDL_PauseAudio(int pause_on);
SDL_AudioStatus SDL_GetAudioStatus(void);
void       SDL_LockAudio(void);
void       SDL_UnlockAudio(void);

SDL_bool   SDL_SetHint(const char *name, const char *value);
SDL_bool   SDL_SetHintWithPriority(const char *name, const char *value, SDL_HintPriority priority);

/* Memory helpers — SDL_malloc/free are sometimes used by legacy code. */
#define SDL_malloc  malloc
#define SDL_calloc  calloc
#define SDL_realloc realloc
#define SDL_free    free
#define SDL_memset  memset
#define SDL_memcpy  memcpy
#define SDL_memcmp  memcmp
#define SDL_strlen  strlen
#define SDL_strcmp  strcmp
#define SDL_strncmp strncmp
#define SDL_strcpy  strcpy
#define SDL_strncpy strncpy

#ifdef __cplusplus
}
#endif

#endif /* CHOCOLATE_KEEN_GBA_SDL_H */
