// engine_io.c: Implements engine io for the engine subsystem.

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "core/globals.h"
#include "engine/engine_io.h"
#include "platform/platform.h"

#ifndef KEEN1_GAMEDATA_PREFIX_WITH_SLASH
#define KEEN1_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN1/"
#define KEEN2_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN2/"
#define KEEN3_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN3/"
#endif

static size_t engine_gameDataPrefixLen;
static char *engine_gameDataFullPathBuffer = NULL;

#ifdef CHOCOLATE_KEEN_TARGET_GBA
/* Longest prefix is "GAMEDATA/KEENx/" (15) + 13 for the 8.3 name = 28.
 * Using a fixed EWRAM BSS buffer eliminates three per-session mallocs. */
static char s_gbaGameDataPathBuffer[32] __attribute__((section(".sbss")));
#endif

#ifdef __APPLE__
extern const char *getBundlePath(const char *suffix);
#endif

void CVort_engine_prepareGameDataFilePathBuffers(gameversion_T gameVer) {
    const char *prefix = NULL;
    switch (gameVer) {
#ifdef __APPLE__
    case GAMEVER_KEEN1: prefix = getBundlePath(KEEN1_GAMEDATA_PREFIX_WITH_SLASH); break;
    case GAMEVER_KEEN2: prefix = getBundlePath(KEEN2_GAMEDATA_PREFIX_WITH_SLASH); break;
    case GAMEVER_KEEN3: prefix = getBundlePath(KEEN3_GAMEDATA_PREFIX_WITH_SLASH); break;
#else
    case GAMEVER_KEEN1: prefix = KEEN1_GAMEDATA_PREFIX_WITH_SLASH; break;
    case GAMEVER_KEEN2: prefix = KEEN2_GAMEDATA_PREFIX_WITH_SLASH; break;
    case GAMEVER_KEEN3: prefix = KEEN3_GAMEDATA_PREFIX_WITH_SLASH; break;
#endif

    default:
        engine_gameDataPrefixLen = 0;
#ifndef CHOCOLATE_KEEN_TARGET_GBA
        if (engine_gameDataFullPathBuffer) { free(engine_gameDataFullPathBuffer); }
#endif
        engine_gameDataFullPathBuffer = NULL;
        return;
    }
    engine_gameDataPrefixLen = strlen(prefix);
#ifdef CHOCOLATE_KEEN_TARGET_GBA
    engine_gameDataFullPathBuffer = s_gbaGameDataPathBuffer;
    snprintf(engine_gameDataFullPathBuffer, sizeof(s_gbaGameDataPathBuffer), "%s", prefix);
#else
    if (engine_gameDataFullPathBuffer) { free(engine_gameDataFullPathBuffer); }
    engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen + 13);
    snprintf(engine_gameDataFullPathBuffer, engine_gameDataPrefixLen + 13, "%s", prefix);
#endif
}

FILE *CVort_engine_cross_ro_data_fopen(const char *filename) {
    char platformPath[1024];
    char externalPlatformPath[1024];
    FILE *fp;

    if (!engine_gameDataFullPathBuffer) {
        return NULL;
    }
    // Data-file lookup currently assumes uppercase DOS-style short names.
    char *ptr = engine_gameDataFullPathBuffer + engine_gameDataPrefixLen;
    strncpy(ptr, filename, 13);
    for (size_t loopVar = 0; loopVar < strlen(ptr); loopVar++) {
        ptr[loopVar] = toupper(ptr[loopVar]);
    }
    if (CK_PlatformBuildExternalGameDataPath(engine_gameDataFullPathBuffer, externalPlatformPath, sizeof(externalPlatformPath))) {
        fp = fopen(externalPlatformPath, "rb");
        if (fp) {
            return fp;
        }
    }
    if (!CK_PlatformBuildRoDataPath(engine_gameDataFullPathBuffer, platformPath, sizeof(platformPath))) {
        return NULL;
    }
    return fopen(platformPath, "rb");
}

FILE *CVort_engine_cross_rw_misc_fopen(const char *filename, const char *mode) {
    char platformPath[1024];

    CK_PlatformEnsureRwMiscDir();
    if (!CK_PlatformBuildRwMiscPath(filename, platformPath, sizeof(platformPath))) {
        return NULL;
    }
    return fopen(platformPath, mode);
}

FILE *CVort_engine_configpath_fopen(const char *filename, const char *mode) {
    char platformPath[1024];

    CK_PlatformEnsureRwMiscDir();
    if (!CK_PlatformBuildRwMiscPath(filename, platformPath, sizeof(platformPath))) {
        return NULL;
    }
    return fopen(platformPath, mode);
}

size_t CVort_engine_cross_freadInt8LE(void *ptr, size_t count, FILE *stream) {
    size_t actualCount = fread(ptr, 1, count, stream);
    if (actualCount < count) {
        memset((uint8_t *)ptr + actualCount, 0, count - actualCount);
    }
    return actualCount;
}

size_t CVort_engine_cross_freadInt16LE(void *ptr, size_t count, FILE *stream) {
    size_t actualCount = fread(ptr, 2, count, stream);
    if (actualCount < count) {
        memset((uint8_t *)ptr + 2 * actualCount, 0, 2 * (count - actualCount));
    }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for (size_t loopVar = 0; loopVar < actualCount; loopVar++, ((uint16_t *)ptr)++)
        *(uint16_t *)ptr = SDL_Swap16(*(uint16_t *)ptr);
#endif
    return actualCount;
}

size_t CVort_engine_cross_freadInt32LE(void *ptr, size_t count, FILE *stream) {
    size_t actualCount = fread(ptr, 4, count, stream);
    if (actualCount < count) {
        memset((uint8_t *)ptr + 4 * actualCount, 0, 4 * (count - actualCount));
    }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for (size_t loopVar = 0; loopVar < actualCount; loopVar++, ((uint32_t *)ptr)++)
        *(uint32_t *)ptr = SDL_Swap32(*(uint32_t *)ptr);
#endif
    return actualCount;
}

size_t CVort_engine_cross_fwriteInt8LE(void *ptr, size_t count, FILE *stream) {
    return fwrite(ptr, 1, count, stream);
}

size_t CVort_engine_cross_fwriteInt16LE(void *ptr, size_t count, FILE *stream) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    return fwrite(ptr, 2, count, stream);
#else
    uint16_t val;
    size_t actualCount = 0;
    for (size_t loopVar = 0; loopVar < count; loopVar++, ((uint16_t *)ptr)++) {
        val = SDL_Swap16(*(uint16_t *)ptr);
        actualCount += fwrite(&val, 2, 1, stream);
    }
    return actualCount;
#endif
}

size_t CVort_engine_cross_fwriteInt32LE(void *ptr, size_t count, FILE *stream) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    return fwrite(ptr, 4, count, stream);
#else
    uint32_t val;
    size_t actualCount = 0;
    for (size_t loopVar = 0; loopVar < count; loopVar++, ((uint32_t *)ptr)++) {
        val = SDL_Swap32(*(uint32_t *)ptr);
        actualCount += fwrite(&val, 4, 1, stream);
    }
    return actualCount;
#endif
}
