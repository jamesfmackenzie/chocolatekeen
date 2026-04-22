/*
 * engine_audio_gba.c — GBA-native PC-speaker emulation.
 *
 * The DOS original plays PC-speaker beep sequences. SOUNDS.CK1 is a small
 * (~9 KiB) table of square-wave "beep lists": a u16 array per sound,
 * each value a PIT period; 0 = silence tick, 0xFFFF = end of sound.
 *
 * On GBA we use DirectSound channel A:
 *   - Timer 0 overflows at ~18156 Hz (the effective sample rate).
 *   - DMA1 streams a 32-bit word from a circular buffer to FIFO_A on
 *     every FIFO-half-empty DRQ.
 *   - A VBlank IRQ hook refills the half of the buffer the DMA is not
 *     currently reading, generating fresh square-wave PCM from the
 *     active sound's beep list.
 *
 * Sample generation runs in IWRAM so the ISR is predictable; the sound
 * table itself stays in cart ROM (via ck_gba_lookup_rom) — no copy.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <gba_base.h>
#include <gba_dma.h>
#include <gba_sound.h>
#include <gba_timers.h>
#include <gba_interrupt.h>

#include "core/globals.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"
#include "platform/gba_data.h"

/* Effective sample rate = 16,777,216 / 924 ≈ 18156 Hz. One GBA frame is
 * 280,896 CPU cycles = 16.74 ms, so ~303 samples are consumed per frame.
 * Buffer is sized slightly larger to leave headroom between the sample
 * DMA actually reads and the one we refill each VBlank.
 *
 * Note: in sound-FIFO DMA mode the source pointer increments forever —
 * DMA_REPEAT does NOT wrap it. We work around that by tearing down
 * DMA1 at each VBlank, repointing SAD at the buffer base, and letting
 * the handler refill the whole buffer before the next frame. */
#define GBA_SND_SAMPLE_RATE    18157
#define GBA_SND_TIMER_RELOAD   ((uint16_t)(65536u - 924u))
#define GBA_SND_BUF_LEN        320

/* Square-wave amplitude. 8-bit signed; ±63 is audible without clipping
 * when Direct Sound is running at 100 % (SNDA_VOL_100). */
#define GBA_SND_WAVE_AMP       63

/* Cap on the number of sound slots we track. Keen 1 ships 60; 128 leaves
 * plenty of room and makes the owner tables cheap. */
#define GBA_SND_MAX_SLOTS      128

static int8_t s_gbaSndBuf[GBA_SND_BUF_LEN] __attribute__((aligned(4)));

static const uint8_t  *s_gbaSndBase = NULL;  /* ROM base of SOUNDS.CK<ep> */
static const uint16_t *s_gbaSndBeepLists[GBA_SND_MAX_SLOTS];
static uint8_t         s_gbaSndPriorityTab[GBA_SND_MAX_SLOTS];
static uint16_t        s_gbaSndLoadedCount = 0;
static bool            s_gbaSndHwInited = false;

/* Number of output samples per DOS-PIT-tick beep slot. pitAltCounter
 * (8192) PIT ticks = 1 beep slot; samplesPerBeep = 8192 * rate / PIT_RATE
 * = 8192 * 18157 / 1193182 ≈ 124.67. Integer-round to 125. */
#define GBA_SND_SAMPLES_PER_BEEP 125

/* Playback state touched by both the main thread (setCurSound / finish)
 * and the VBlank IRQ. We always mutate these with IRQs disabled. */
static volatile const uint16_t *s_curBeepPtr = NULL;
static volatile int32_t s_curBeepSamplesLeft = 0;
static volatile int32_t s_curBeepHalfCycle = 0;
static volatile int8_t  s_curBeepWave = GBA_SND_WAVE_AMP;

static inline void gba_snd_irqDisable(void) { REG_IME = 0; }
static inline void gba_snd_irqEnable(void)  { REG_IME = 1; }

/* --------------------------------------------------------------------
 * Sample generator. Runs from IWRAM; called by the VBlank ISR.
 * -------------------------------------------------------------------- */
IWRAM_CODE static void gba_snd_genSamples(int8_t *out, int n) {
    if (!s_curBeepPtr) {
        memset(out, 0, n);
        return;
    }
    while (n > 0) {
        if (s_curBeepSamplesLeft <= 0) {
            s_curBeepPtr++;
            uint16_t v = *s_curBeepPtr;
            if (v == 0xFFFFu) {
                s_curBeepPtr = NULL;
                engine_currSoundPlaying = -1;
                sound_limiter = 0;
                memset(out, 0, n);
                return;
            }
            s_curBeepSamplesLeft = GBA_SND_SAMPLES_PER_BEEP;
            s_curBeepHalfCycle = 0;
            s_curBeepWave = GBA_SND_WAVE_AMP;
        }
        uint16_t sndVal = *s_curBeepPtr;
        int take = (s_curBeepSamplesLeft < n) ? s_curBeepSamplesLeft : n;
        if (sndVal == 0) {
            /* Silence tick. */
            memset(out, 0, take);
        } else {
            /* Square wave: threshold = sampleRate * periodVal. Every
             * 2 * PIT_RATE counts, cross one half-cycle and flip sign. */
            int32_t threshold = (int32_t)GBA_SND_SAMPLE_RATE * (int32_t)sndVal;
            int32_t halfCycle = s_curBeepHalfCycle;
            int8_t wave = s_curBeepWave;
            for (int i = 0; i < take; i++) {
                out[i] = wave;
                halfCycle += 2 * (int32_t)PC_PIT_RATE;
                if (halfCycle >= threshold) {
                    halfCycle -= threshold;
                    wave = (int8_t)(-wave);
                }
            }
            s_curBeepHalfCycle = halfCycle;
            s_curBeepWave = wave;
        }
        out += take;
        s_curBeepSamplesLeft -= take;
        n -= take;
    }
}

IWRAM_CODE static void gba_snd_vblankHook(void) {
    /* Tear DMA1 down first so we can safely rewrite the whole buffer
     * without racing the read. FIFO still has ~16 samples queued, which
     * keeps the DAC fed while we regenerate. */
    REG_DMA1CNT = 0;
    gba_snd_genSamples(s_gbaSndBuf, GBA_SND_BUF_LEN);
    REG_DMA1SAD = (uint32_t)(uintptr_t)s_gbaSndBuf;
    REG_DMA1CNT = DMA_ENABLE | DMA_SPECIAL | DMA_REPEAT | DMA32
                | DMA_SRC_INC | DMA_DST_FIXED;
}

/* --------------------------------------------------------------------
 * Hardware setup. Idempotent — we may re-enter from loadSounds if the
 * episode changes or the file isn't present on the first try.
 * -------------------------------------------------------------------- */
static void gba_snd_hwInit(void) {
    if (s_gbaSndHwInited) return;
    memset(s_gbaSndBuf, 0, sizeof(s_gbaSndBuf));

    /* Master enable. */
    REG_SOUNDCNT_X = 0x0080;

    /* Direct Sound A at 100 %, both speakers, timer 0 as clock, reset
     * FIFO, DMG channels at 100 % of the mix (not used but harmless). */
    REG_SOUNDCNT_H = SNDA_VOL_100 | SNDA_L_ENABLE | SNDA_R_ENABLE
                   | SNDA_RESET_FIFO | 2 /* output ratio 100 % */;

    /* Timer 0 drives the sample clock. */
    REG_TM0CNT_L = GBA_SND_TIMER_RELOAD;
    REG_TM0CNT_H = TIMER_START;

    /* DMA1 feeds FIFO_A on half-empty DRQ. DMA_SPECIAL enables the sound
     * timing path; the hardware transfers 4 × 32-bit words per DRQ. */
    REG_DMA1SAD = (uint32_t)(uintptr_t)s_gbaSndBuf;
    REG_DMA1DAD = (uint32_t)(uintptr_t)&REG_FIFO_A;
    REG_DMA1CNT = DMA_ENABLE | DMA_SPECIAL | DMA_REPEAT | DMA32
                | DMA_SRC_INC | DMA_DST_FIXED;

    irqSet(IRQ_VBLANK, gba_snd_vblankHook);
    s_gbaSndHwInited = true;
}

/* --------------------------------------------------------------------
 * Sound table parsing. Layout (see engine_audio.c) is:
 *   [0..7]    header
 *   [8..9]    engine_sndCount  (u16 LE)
 *   For each i in [0..count):
 *     header entry at offset 16*(i+1):
 *       u16 @ +0 : offset of this sound's beep list, in bytes from [0]
 *       u8  @ +2 : priority
 * Beep lists are runs of u16 period values terminated by 0xFFFF.
 *
 * Keen 1 ships the table as the external SOUNDS.CK1 blob (looked up in
 * the baked ROM). Keen 2/3 instead embed it inside KEENn.EXE at a fixed
 * offset; we source it directly from exeImage so both paths read from
 * cart ROM with no copies. szBound caps the apparent blob size so a bad
 * offset field can't dereference out of bounds.
 * -------------------------------------------------------------------- */
static bool gba_snd_loadFromBlob(const uint8_t *rom, size_t szBound) {
    if (!rom || szBound < 16) return false;
    s_gbaSndBase = rom;
    uint16_t count = (uint16_t)(rom[8] | (rom[9] << 8));
    if (count > GBA_SND_MAX_SLOTS) count = GBA_SND_MAX_SLOTS;
    s_gbaSndLoadedCount = count;
    for (uint16_t i = 0; i < count; i++) {
        const uint8_t *entry = rom + 16u * ((uint32_t)i + 1u);
        uint16_t off = (uint16_t)(entry[0] | (entry[1] << 8));
        s_gbaSndPriorityTab[i] = entry[2];
        s_gbaSndBeepLists[i] = (off < szBound) ? (const uint16_t *)(rom + off) : NULL;
    }
    engine_sndCount = count;
    return true;
}

static bool gba_snd_loadSoundTable(void) {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        char path[32];
        snprintf(path, sizeof(path), "SOUNDS.%s", game_ext);
        const uint8_t *rom = NULL;
        size_t sz = 0;
        if (ck_gba_lookup_rom(path, &rom, &sz) != 0) return false;
        return gba_snd_loadFromBlob(rom, sz);
    }
    if (!exeImage) return false;
    /* We don't know the EXE's decompressed length here, so use a bound
     * large enough to cover every valid beep-list offset in the table
     * (both Keen 2 and Keen 3 keep their sound data well inside the
     * first 64 KiB after the offset). */
    const size_t szBound = 0x10000;
    if (engine_gameVersion == GAMEVER_KEEN2) {
        return gba_snd_loadFromBlob(exeImage + CVort2_SOUNDS_OFFSET, szBound);
    }
    if (engine_gameVersion == GAMEVER_KEEN3) {
        return gba_snd_loadFromBlob(exeImage + CVort3_SOUNDS_OFFSET, szBound);
    }
    return false;
}

/* --------------------------------------------------------------------
 * Engine-facing API. Mirrors the SDL backend's contract closely enough
 * that gameplay.c / menus.c don't need GBA-specific call sites.
 * -------------------------------------------------------------------- */
void CVort_engine_loadSounds(void) {
    /* Mirror the SDL-path defaults so anyone reading these globals sees
     * sensible values whether or not the load succeeds. */
    sndChunks = NULL;
    sndChunksData = NULL;
    sndPriorities = NULL;
    sound_limiter = 0;
    engine_currSoundPlaying = -1;
    engine_currSoundLocation = 0;

    if (engine_arguments.disableSoundSystem) {
        engine_sndCount = 0;
        return;
    }

    if (!gba_snd_loadSoundTable()) {
        /* No SOUNDS.CK<ep> available — keep the backend silent but
         * leave hardware untouched so we don't clobber anything. */
        engine_sndCount = 0;
        return;
    }

    /* Expose the parsed priority table under the engine-wide name so
     * any shared code paths (cheats, test code) read the right bytes. */
    sndPriorities = s_gbaSndPriorityTab;

    gba_snd_hwInit();
}

void CVort_engine_sndCallback(void *unused, uint8_t *stream, int len) {
    /* Not used on GBA — DMA/IRQ drive playback. Kept for ABI parity. */
    (void)unused; (void)stream; (void)len;
}

void CVort_engine_setCurSound(uint16_t sound) {
    if (engine_arguments.disableSoundSystem) return;
    if (g_game.sound_disabled) return;
    if (sound == 0) return;
    sound--;  /* DOS indexes from 1. */
    if (sound >= s_gbaSndLoadedCount) return;
    const uint16_t *beeps = s_gbaSndBeepLists[sound];
    if (!beeps) return;

    /* Priority gate matches the DOS original: only preempt the active
     * sound if the new one's priority is >= the current sound_limiter. */
    if (engine_currSoundPlaying >= 0 &&
        s_gbaSndPriorityTab[sound] < sound_limiter) {
        return;
    }

    gba_snd_irqDisable();
    engine_currSoundPlaying = (int16_t)sound;
    engine_currSoundLocation = 0;
    sound_limiter = s_gbaSndPriorityTab[sound];
    s_curBeepPtr = beeps;
    s_curBeepSamplesLeft = GBA_SND_SAMPLES_PER_BEEP;
    s_curBeepHalfCycle = 0;
    s_curBeepWave = GBA_SND_WAVE_AMP;
    gba_snd_irqEnable();
}

void CVort_engine_saveCurSound(void) {
    /* SDL path pauses the audio device; on GBA we just stop generating
     * samples. DMA keeps spinning over a zeroed buffer. */
    if (engine_arguments.disableSoundSystem) return;
    gba_snd_irqDisable();
    s_curBeepPtr = NULL;
    gba_snd_irqEnable();
}

void CVort_engine_restoreCurSound(void) {
    /* No-op for the GBA backend: setCurSound will rearm playback when
     * the next call comes in. Kept for ABI parity. */
}

void CVort_engine_finishCurSound(void) {
    if (engine_arguments.disableSoundSystem) return;
    if (g_game.sound_disabled) return;
    while (engine_currSoundPlaying >= 0) {
        CVort_engine_shortSleep();
    }
}
