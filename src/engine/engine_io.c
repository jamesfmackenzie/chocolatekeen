#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "core/globals.h"
#include "engine/engine_io.h"

#define KEEN1_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN1/"
#define KEEN2_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN2/"
#define KEEN3_GAMEDATA_PREFIX_WITH_SLASH "GAMEDATA/KEEN3/"

static size_t engine_gameDataPrefixLen;
static char *engine_gameDataFullPathBuffer = NULL;

void CVort_engine_prepareGameDataFilePathBuffers(gameversion_T gameVer) {
    if (engine_gameDataFullPathBuffer) {
        free(engine_gameDataFullPathBuffer);
    }
    switch (gameVer) {
    case GAMEVER_KEEN1:
        engine_gameDataPrefixLen = strlen(KEEN1_GAMEDATA_PREFIX_WITH_SLASH);
        engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen + 13);
        snprintf(engine_gameDataFullPathBuffer, engine_gameDataPrefixLen + 13, "%s", KEEN1_GAMEDATA_PREFIX_WITH_SLASH);
        break;
    case GAMEVER_KEEN2:
        engine_gameDataPrefixLen = strlen(KEEN2_GAMEDATA_PREFIX_WITH_SLASH);
        engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen + 13);
        snprintf(engine_gameDataFullPathBuffer, engine_gameDataPrefixLen + 13, "%s", KEEN2_GAMEDATA_PREFIX_WITH_SLASH);
        break;
    case GAMEVER_KEEN3:
        engine_gameDataPrefixLen = strlen(KEEN3_GAMEDATA_PREFIX_WITH_SLASH);
        engine_gameDataFullPathBuffer = (char *)malloc(engine_gameDataPrefixLen + 13);
        snprintf(engine_gameDataFullPathBuffer, engine_gameDataPrefixLen + 13, "%s", KEEN3_GAMEDATA_PREFIX_WITH_SLASH);
        break;
    default:
        engine_gameDataPrefixLen = 0;
        engine_gameDataFullPathBuffer = NULL;
        break;
    }
}

FILE *CVort_engine_cross_ro_data_fopen(const char *filename) {
    // Data-file lookup currently assumes uppercase DOS-style short names.
    char *ptr = engine_gameDataFullPathBuffer + engine_gameDataPrefixLen;
    strncpy(ptr, filename, 13);
    for (size_t loopVar = 0; loopVar < strlen(ptr); loopVar++) {
        ptr[loopVar] = toupper(ptr[loopVar]);
    }
    return fopen(engine_gameDataFullPathBuffer, "rb");
}

FILE *CVort_engine_cross_rw_misc_fopen(const char *filename, const char *mode) {
    // Misc RW files currently use the provided path unchanged.
    return fopen(filename, mode);
}

FILE *CVort_engine_configpath_fopen(const char *filename, const char *mode) {
    // NOTE For now it simply opens the file given by the name, as-is...
    return fopen(filename, mode);
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
