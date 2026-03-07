// engine_timing.c: Implements engine timing for the engine subsystem.

#include "SDL.h"

#include "core/globals.h"
#include "engine/engine_timing.h"
#include "platform/platform.h"

const uint32_t pitAltCounter = 0x2000;

int64_t CVort_engine_convertMomentFromGameTicksToMicroSec(uint32_t momentInGameTicks) {
    return (((int64_t)(momentInGameTicks - engine_gameTicksStart)) * ((int64_t)1000000) * ((int64_t)pitAltCounter)) / ((int64_t)PC_PIT_RATE) + engine_sdlMicroTicksStart;
}

void CVort_private_engine_setTicks(uint32_t currTicks) {
    engine_gameTicksStart = currTicks - (CVort_private_engine_getTicks() - engine_gameTicksStart);
}

uint32_t CVort_private_engine_getTicks(void) {
    return engine_gameTicksStart - engine_gameTicksDelayOffset + (uint32_t)((((int64_t)(((int64_t)1000) * ((int64_t)SDL_GetTicks()) - (int64_t)(engine_sdlMicroTicksStart) - (int64_t)(engine_sdlMicroTicksDelayOffset))) * (int64_t)(PC_PIT_RATE)) / (((int64_t)1000000) * ((int64_t)pitAltCounter)));
}

static uint32_t CVort_private_engine_getActualTicks(void) {
    return engine_gameTicksStart + (uint32_t)((((int64_t)(((int64_t)1000) * ((int64_t)SDL_GetTicks()) - (int64_t)(engine_sdlMicroTicksStart))) * (int64_t)(PC_PIT_RATE)) / (((int64_t)1000000) * ((int64_t)pitAltCounter)));
}

void CVort_private_engine_setTicksSync(uint32_t currTicks) {
    ticks_sync = currTicks;
}

uint32_t CVort_private_engine_getTicksSync(void) {
    return ticks_sync;
}

void CVort_engine_delay(int16_t length) {
    if (engine_doResetMomentToDelay) {
        engine_doResetMomentToDelay = false;
        /* We are RIGHT after a call to engine_shortSleep, if not on
         * application startup, so better update the display contents
         * so nothing gets missed when it's unexpected.
         */
        CVort_engine_updateActualDisplay();
        engine_sdlMicroTicksDelayOffset = 0;
        //engine_gameTicksDelayOffset = 0;
        engine_momentToDelayInMicroSec = 1000 * ((int64_t)SDL_GetTicks());
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
    while ((int64_t)(1000 * ((int64_t)SDL_GetTicks()) - engine_momentToDelayInMicroSec) < 0) {
        CVort_engine_doWaitInterval();
        CVort_engine_updateInputStatus();
    }

    /* The delay may have taken longer than intended (due to OS scheduling),
     * but the game should better "think" the passed time is really what's
     * expected. So we (re)set an offset for calls to CVort_private_engine_getTicks(),
     * measured in microseconds.
     */
    engine_sdlMicroTicksDelayOffset = 1000 * ((int64_t)SDL_GetTicks()) - engine_momentToDelayInMicroSec;
    engine_gameTicksDelayOffset = 0;
}

void CVort_engine_delayInGameTicks(uint32_t startOfDelayMoment, uint16_t length) {
    /* WARNING: Without this check, upon returning to the main menu from the
     * help text, the game is expected to HANG for long!
     * (Actually it still checks the input so you may unlock cursor and quit.)
     */
    if ((CVort_private_engine_getTicks() & 0xFFFF) - (startOfDelayMoment & 0xFFFF) >= length) {
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
        engine_momentToDelayInMicroSec = 1000 * ((int64_t)SDL_GetTicks());
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
    while ((int64_t)(1000 * ((int64_t)SDL_GetTicks()) - engine_momentToDelayInMicroSec) < 0) {
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
        CK_PlatformSleepMs(1);
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
    CK_PlatformSleepMs(1);
    CVort_engine_updateInputStatus();
    engine_doResetMomentToDelay = true;
}
