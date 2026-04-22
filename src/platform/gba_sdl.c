/*
 * gba_sdl.c — runtime backing for the GBA SDL2 shim.
 *
 * Every function here is a deliberate no-op returning a value that the
 * engine's happy path treats as "success" or "nothing to do". Real
 * functionality that happens to share names with SDL (timing, input) is
 * implemented in sleep_gba.c / input_gba.c and ends up routed through
 * CK_Platform*; see platform/platform.h.
 */

#include "platform/gba_sdl.h"

#include <gba_base.h>
#include <gba_timers.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

/* Tick source for SDL_GetTicks. One of the hardware timers is cascaded
 * to give a millisecond-ish counter without needing interrupts. We set
 * it up lazily the first time SDL_GetTicks is invoked so the shim works
 * even before the engine calls SDL_Init. */
static int s_timersStarted = 0;

static void ck_start_tick_timers(void) {
    if (s_timersStarted) return;
    s_timersStarted = 1;
    /* Timer 2 counts CPU cycles (16.78 MHz) / 1024 ≈ 16.384 kHz.
     * Timer 3 is cascaded off timer 2 overflow to widen the counter.
     * A full overflow of the cascade wraps at ~4.55 hours, which is
     * more than adequate for SDL_GetTicks -style relative timing. */
    /* Prescaler values for TMxCNT_H bits 0-1: 0=/1, 1=/64, 2=/256, 3=/1024. */
    REG_TM2CNT_L = 0;
    REG_TM3CNT_L = 0;
    REG_TM2CNT_H = 0x3 /* /1024 */ | TIMER_START;
    REG_TM3CNT_H = TIMER_COUNT | TIMER_START;
}

int SDL_Init(Uint32 flags) { (void)flags; ck_start_tick_timers(); return 0; }
int SDL_InitSubSystem(Uint32 flags) { (void)flags; return 0; }
void SDL_Quit(void) {}
void SDL_QuitSubSystem(Uint32 flags) { (void)flags; }
const char *SDL_GetError(void) { return ""; }
void SDL_ClearError(void) {}
int SDL_SetError(const char *fmt, ...) { (void)fmt; return -1; }

Uint32 SDL_GetTicks(void) {
    ck_start_tick_timers();
    /* Snapshot the cascade. Read lo/hi twice to avoid a mid-tick seam. */
    Uint32 lo, hi, hi2;
    do {
        hi  = REG_TM3CNT_L;
        lo  = REG_TM2CNT_L;
        hi2 = REG_TM3CNT_L;
    } while (hi != hi2);
    Uint32 ticks16k = (hi << 16) | lo;
    /* 16.384 kHz → ms:  ms = ticks * 1000 / 16384 ≈ ticks * 61 / 1000 */
    return (Uint32)(((Uint64)ticks16k * 1000u) >> 14);
}

void SDL_Delay(Uint32 ms) {
    Uint32 start = SDL_GetTicks();
    while ((SDL_GetTicks() - start) < ms) {
        /* Halt the CPU until the next interrupt (VBlank fires at 60 Hz)
         * — this drops power draw significantly compared to a spin loop. */
        VBlankIntrWait();
    }
}

/* No windowing/event system on GBA. Polls always report "nothing". */
int SDL_PollEvent(SDL_Event *event) { (void)event; return 0; }
int SDL_PushEvent(SDL_Event *event) { (void)event; return 0; }
void SDL_PumpEvents(void) {}

/* Joysticks: report 0 and hand back NULL. Real input flows through
 * src/platform/input_gba.c, which sets g_input / inputMappings directly. */
int SDL_NumJoysticks(void) { return 0; }
SDL_Joystick *SDL_JoystickOpen(int index) { (void)index; return NULL; }
void SDL_JoystickClose(SDL_Joystick *joy) { (void)joy; }
const char *SDL_JoystickName(SDL_Joystick *joy) { (void)joy; return "GBA"; }
int SDL_JoystickEventState(int state) { (void)state; return 0; }
int SDL_JoystickNumAxes(SDL_Joystick *joy) { (void)joy; return 0; }
int SDL_JoystickNumButtons(SDL_Joystick *joy) { (void)joy; return 0; }
Sint16 SDL_JoystickGetAxis(SDL_Joystick *joy, int axis) { (void)joy; (void)axis; return 0; }
Uint8 SDL_JoystickGetButton(SDL_Joystick *joy, int button) { (void)joy; (void)button; return 0; }
int SDL_JoystickNumHats(SDL_Joystick *joy) { (void)joy; return 0; }
int SDL_JoystickNumBalls(SDL_Joystick *joy) { (void)joy; return 0; }
Uint8 SDL_JoystickGetHat(SDL_Joystick *joy, int hat) { (void)joy; (void)hat; return 0; }
SDL_JoystickID SDL_JoystickInstanceID(SDL_Joystick *joy) { (void)joy; return 0; }

/* Mouse/cursor: GBA has none. */
int SDL_ShowCursor(int toggle) { (void)toggle; return SDL_DISABLE; }
void SDL_SetRelativeMouseMode(SDL_bool enabled) { (void)enabled; }
Uint32 SDL_GetRelativeMouseState(int *x, int *y) {
    if (x) *x = 0;
    if (y) *y = 0;
    return 0;
}
Uint32 SDL_GetMouseState(int *x, int *y) {
    if (x) *x = 0;
    if (y) *y = 0;
    return 0;
}
void SDL_WM_GrabInput(int mode) { (void)mode; }

/* Renderer probing — pretend there's one "gba" driver. The caller only
 * reads .name to decide how to label things in a launcher menu. */
int SDL_GetNumRenderDrivers(void) { return 1; }
int SDL_GetRenderDriverInfo(int index, SDL_RendererInfo *info) {
    if (index != 0 || !info) return -1;
    static const char *name = "gba";
    info->name = name;
    info->flags = 0;
    info->num_texture_formats = 1;
    info->texture_formats[0] = SDL_PIXELFORMAT_RGB565;
    info->max_texture_width = 240;
    info->max_texture_height = 160;
    return 0;
}

/* Audio: the GBA build provides its own silent audio driver in
 * engine_audio_gba.c and never opens an SDL audio device. These stubs
 * exist because engine_loader.c still calls them on the failure path. */
int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
    if (obtained && desired) *obtained = *desired;
    return 0;
}
void SDL_CloseAudio(void) {}
void SDL_PauseAudio(int pause_on) { (void)pause_on; }
SDL_AudioStatus SDL_GetAudioStatus(void) { return SDL_AUDIO_STOPPED; }
void SDL_LockAudio(void) {}
void SDL_UnlockAudio(void) {}

SDL_bool SDL_SetHint(const char *name, const char *value) {
    (void)name; (void)value; return SDL_TRUE;
}
SDL_bool SDL_SetHintWithPriority(const char *name, const char *value, SDL_HintPriority prio) {
    (void)name; (void)value; (void)prio; return SDL_TRUE;
}
