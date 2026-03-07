// input_names.c: Implements input names for the input subsystem.

#include "input/input_names.h"

const char *const mapperClientJoyButtonNames[] = {
    "jbutton_0_0", "jbutton_0_1", "jbutton_1_0", "jbutton_1_1",
    // EXTRAS (e.g. "jbutton_0_2" is gameport-equivalent to "jbutton_1_0")
    "jbutton_0_2", "jbutton_0_3"
};

const char *const mapperClientJoyAxisNames[] = {
    "jaxis_0_0-", "jaxis_0_0+", "jaxis_0_1-", "jaxis_0_1+",
    "jaxis_1_0-", "jaxis_1_0+", "jaxis_1_1-", "jaxis_1_1+",
};

const char *const mapperClientMouseButtonNames[] = {
    "mbutton_left", "mbutton_right", "mbutton_middle"
};

const char *const mapperClientMouseAxisNames[] = {
    "maxis_horiz-", "maxis_horiz+", "maxis_vert-", "maxis_vert+"
};

const char *const mapperClientHandlerNames[] = {
    "hand_shutdown", "hand_capcursor", "hand_fullscr",
    /*"hand_savemap"*/
};

const char *const mapperClientModNames[] = {
    "mod_1", "mod_2", "mod_3"
};
