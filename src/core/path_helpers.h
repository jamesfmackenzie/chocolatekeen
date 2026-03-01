#ifndef CK_PATH_HELPERS_H
#define CK_PATH_HELPERS_H

#include <stdint.h>
#include <stdio.h>

static inline void CK_Path_FormatDemo(char *path, size_t path_size, int16_t demo_number, const char *game_ext) {
    snprintf(path, path_size, "DEMO%d.%s", demo_number, game_ext);
}

static inline void CK_Path_FormatSavedSlot(char *path, size_t path_size, char slot_char, const char *game_ext) {
    snprintf(path, path_size, "SAVED%c.%s", slot_char, game_ext);
}

static inline void CK_Path_FormatScores(char *path, size_t path_size, const char *game_ext) {
    snprintf(path, path_size, "SCORES.%s", game_ext);
}

static inline void CK_Path_FormatCtrlPanel(char *path, size_t path_size, const char *game_ext) {
    snprintf(path, path_size, "CTLPANEL.%s", game_ext);
}

#endif
