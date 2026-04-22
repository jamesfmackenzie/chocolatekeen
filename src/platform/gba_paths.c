/*
 * gba_paths.c — CK_PlatformBuild*Path / auxiliary hooks.
 *
 * There is no filesystem, so path builders just forward the relative
 * name; the FILE* shim in gba_fileio.c resolves opens by basename against
 * the incbin'd ROM table. CK_PlatformBuildRwMiscPath always "succeeds"
 * into a path that fopen will reject — the engine's config/save paths
 * therefore no-op gracefully.
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "platform/platform.h"
#include "platform/gba_sdl.h"

void CK_PlatformEnsureRwMiscDir(void) {
    /* Nothing to do — no writable storage on stock GBA carts. */
}

bool CK_PlatformBuildRoDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    if (!relativePath || !outPath || outPathSize == 0) return false;
    size_t len = strlen(relativePath);
    if (len + 1 > outPathSize) return false;
    memcpy(outPath, relativePath, len + 1);
    return true;
}

bool CK_PlatformBuildExternalGameDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    /* No external storage on GBA — report "no external data available"
     * so the engine only consults the in-ROM bundle. */
    (void)relativePath; (void)outPath; (void)outPathSize;
    return false;
}

bool CK_PlatformBuildRwMiscPath(const char *filename, char *outPath, size_t outPathSize) {
    if (!filename || !outPath || outPathSize == 0) return false;
    size_t len = strlen(filename);
    if (len + 1 > outPathSize) return false;
    memcpy(outPath, filename, len + 1);
    /* The path is syntactically valid but fopen will fail on it because
     * the name is not in the ROM table; config + save code gracefully
     * falls back to in-memory defaults. */
    return true;
}

bool CK_PlatformShouldAutoShowLauncher(int availableEpisodes) {
    /* The GBA build is pinned to Episode 1 and has no launcher, so the
     * auto-launcher policy is always false. */
    (void)availableEpisodes;
    return false;
}
