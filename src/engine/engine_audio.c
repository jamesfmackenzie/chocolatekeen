#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "core/globals.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"

void CVort_engine_loadSounds(void) {
    if (engine_arguments.disableSoundSystem) {
        return;
    }
    // For Keen 2 and 3, the sounds are stored in the EXE,
    // while for Keen 1 they are stored in an external file (SOUNDS.CK1).

    uint32_t len;
    uint8_t *soundDataPtr;

    if (engine_gameVersion == GAMEVER_KEEN1) {
        snprintf(g_game.string_buf, sizeof(g_game.string_buf), "SOUNDS.%s", game_ext);
        FILE *fp = CVort_engine_cross_ro_data_fopen(g_game.string_buf);
        // TODO: What if we fail to load the file?
        if (fp) {
            len = CVort_filelength(fp);
            soundDataPtr = (uint8_t *)malloc(len);
            if (!soundDataPtr || (fread(soundDataPtr, len, 1, fp) != 1)) {
                if (soundDataPtr)
                    free(soundDataPtr);
                fclose(fp);
                CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Failed reading %s.\n", g_game.string_buf);
                return;
            }
            fclose(fp);
        } else {
            return;
        }
    } else if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {
        soundDataPtr = (uint8_t *)exeImage + (engine_gameVersion == GAMEVER_KEEN2 ? CVort2_SOUNDS_OFFSET : CVort3_SOUNDS_OFFSET);
        len = SDL_SwapLE16(*(uint16_t *)(soundDataPtr + 4));
    } else {
        // Couldn't find anything
        return;
    }
    // Note: The data has been loaded from the file as-is,
    // in LITTLE-ENDIAN order wherever relevant.
    // Shouldn't this be soundDataPtr + 8?
    engine_sndCount = SDL_SwapLE16(*(uint16_t *)(soundDataPtr + 8));
    sndChunks = (sndChunk_T *)malloc(engine_sndCount * sizeof(sndChunk_T));
    // Sound priorities
    sndPriorities = (uint8_t *)malloc(engine_sndCount);

    uint32_t samplesPerBeep = pitAltCounter * engine_arguments.sndSampleRate / PC_PIT_RATE;
    // Allocate data for all potential beeps. We consider the
    // file's contents without the header or sound definitions.
    sndChunksData = malloc(ENGINE_SNDBYTEDEPTH * samplesPerBeep * (len - 16 * (engine_sndCount + 1)));

    // Now fill the data.
    // TODO: Again we assume 16-bit depth and a single channel.
    uint16_t *currSndChunkPtr = (uint16_t *)sndChunksData;
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
        sndPtr = (uint16_t *)(soundDataPtr + SDL_SwapLE16(*(uint16_t *)(soundDataPtr + 16 * (loopVar + 1))));
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
                        beepHalfCycleCounter %= engine_arguments.sndSampleRate * sndVal;
                        beepWaveVal = -beepWaveVal;
                    }
                    currSndChunkPtr++;
                }
            } else { // Silence
                memset(currSndChunkPtr, 0, 2 * samplesPerBeep);
                currSndChunkPtr += samplesPerBeep;
            }
            numOfBeeps++;
            sndPtr++;
        }
        sndChunks[loopVar].len = (uint8_t *)currSndChunkPtr - (uint8_t *)sndChunks[loopVar].chunk;
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

    if (g_game.want_sound & 0xFF) { // As done on Vanilla Keen...sort of
        memcpy(stream, (uint8_t *)sndChunks[currSoundPlaying].chunk + engine_currSoundLocation, amount);
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

    if (!g_game.sound_disabled && ((engine_currSoundPlaying < 0) || (sndPriorities[sound] >= sound_limiter))) {
        SDL_PauseAudio(1);
        engine_currSoundPlaying = sound;
        sound_limiter = sndPriorities[sound];
        engine_currSoundLocation = 0;
        SDL_PauseAudio(0);
    }
}

void CVort_engine_saveCurSound(void) {
    if (engine_arguments.disableSoundSystem)
        return;
    if (!g_game.sound_disabled && (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING))
        SDL_PauseAudio(1);
}

void CVort_engine_restoreCurSound(void) {
    if (engine_arguments.disableSoundSystem)
        return;
    if (!g_game.sound_disabled && (SDL_GetAudioStatus() == SDL_AUDIO_PAUSED))
        SDL_PauseAudio(0);
}

void CVort_engine_finishCurSound(void) {
    if (engine_arguments.disableSoundSystem)
        return;
    if (!g_game.sound_disabled)
        while (engine_currSoundPlaying >= 0) {
            CVort_engine_shortSleep();
            //CVort_engine_updateActualDisplay();
        }
}
