//#include <cstdio>
//#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <vector>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "cvorticons.h"
#include "cvorticons1.h"
#include "cvorticons2.h"
#include "cvorticons3.h"
#include "decompression/CGenius/fileio/compression/CRLE.h"

#define VORTFRAME(a) ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_vort##a : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_vort##a : CVort3_spr_vort##a))
#define YOUTHFRAME(a) ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_youth##a : CVort3_spr_youth##a)

#define SPRKEENSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_keenshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_keenshot : CVort3_spr_keenshot))
#define SPRTANKSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_tankshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_tankshot : CVort3_spr_tankshot))
#define SPRSHOTSPLASHR ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_shotsplashr : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_shotsplashr : CVort3_spr_shotsplashr))
#define SPRSHOTSPLASHL ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_shotsplashl : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_shotsplashl : CVort3_spr_shotsplashl))

#define OBJNULL ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_null : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_null : CVort3_obj_null))
#define OBJDEAD ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_dead : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_dead : CVort3_obj_dead))
#define OBJZAPZOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_zapzot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_zapzot : CVort3_obj_zapzot))
#define OBJKEEN ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_keen : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_keen : CVort3_obj_keen))
#define OBJENEMYSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_enemyshot :  CVort2_obj_enemyshot)
#define OBJKEENSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_keenshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_keenshot : CVort3_obj_keenshot))
#define OBJONEBEFOREKEENSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_onebeforekeenshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_onebeforekeenshot : CVort3_obj_onebeforekeenshot))
#define OBJVORTICON ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_vorticon : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_vorticon : CVort3_obj_vorticon))
#define OBJYOUTH ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_youth : CVort3_obj_youth)

#define SNDSHOTHIT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_snd_shothit : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_snd_shothit : CVort3_snd_shothit))
#define SNDVORTSCREAM ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_snd_vortscream : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_snd_vortscream : CVort3_snd_vortscream))

#if 0
// Random number table
const uint8_t CVort_rnd_vals[] = {
    0x0, 0x8, 0x6D, 0xDC, 0xDE, 0xF1, 0x95, 0x6B, 0x4B, 0xF8, 0xFE, 0x8C, 0x10, 0x42, 0x4A, 0x15,
    0xD3, 0x2F, 0x50, 0xF2, 0x9A, 0x1B, 0xCD, 0x80, 0xA1, 0x59, 0x4D, 0x24, 0x5F, 0x6E, 0x55, 0x30,
    0xD4, 0x8C, 0xD3, 0xF9, 0x16, 0x4F, 0xC8, 0x32, 0x1C, 0xBC, 0x34, 0x8C, 0xCA, 0x78, 0x44, 0x91,
    0x3E, 0x46, 0xB8, 0xBE, 0x5B, 0xC5, 0x98, 0xE0, 0x95, 0x68, 0x19, 0xB2, 0xFC, 0xB6, 0xCA, 0xB6,
    0x8D, 0xC5, 0x4, 0x51, 0xB5, 0xF2, 0x91, 0x2A, 0x27, 0xE3, 0x9C, 0xC6, 0xE1, 0xC1, 0xDB, 0x5D,
    0x7A, 0xAF, 0xF9, 0x0, 0xAF, 0x8F, 0x46, 0xEF, 0x2E, 0xF6, 0xA3, 0x35, 0xA3, 0x6D, 0xA8, 0x87,
    0x2, 0xEB, 0x19, 0x5C, 0x14, 0x91, 0x8A, 0x4D, 0x45, 0xA6, 0x4E, 0xB0, 0xAD, 0xD4, 0xA6, 0x71,
    0x5E, 0xA1, 0x29, 0x32, 0xEF, 0x31, 0x6F, 0xA4, 0x46, 0x3C, 0x2, 0x25, 0xAB, 0x4B, 0x88, 0x9C,
    0xB, 0x38, 0x2A, 0x92, 0x8A, 0xE5, 0x49, 0x92, 0x4D, 0x3D, 0x62, 0xC4, 0x87, 0x6A, 0x3F, 0xC5,
    0xC3, 0x56, 0x60, 0xCB, 0x71, 0x65, 0xAA, 0xF7, 0xB5, 0x71, 0x50, 0xFA, 0x6C, 0x7, 0xFF, 0xED,
    0x81, 0xE2, 0x4F, 0x6B, 0x70, 0xA6, 0x67, 0xF1, 0x18, 0xDF, 0xEF, 0x78, 0xC6, 0x3A, 0x3C, 0x52,
    0x80, 0x3, 0xB8, 0x42, 0x8F, 0xE0, 0x91, 0xE0, 0x51, 0xCE, 0xA3, 0x2D, 0x3F, 0x5A, 0xA8, 0x72,
    0x3B, 0x21, 0x9F, 0x5F, 0x1C, 0x8B, 0x7B, 0x62, 0x7D, 0xC4, 0xF, 0x46, 0xC2, 0xFD, 0x36, 0xE,
    0x6D, 0xE2, 0x47, 0x11, 0xA1, 0x5D, 0xBA, 0x57, 0xF4, 0x8A, 0x14, 0x34, 0x7B, 0xFB, 0x1A, 0x24,
    0x11, 0x2E, 0x34, 0xE7, 0xE8, 0x4C, 0x1F, 0xDD, 0x54, 0x25, 0xD8, 0xA5, 0xD4, 0x6A, 0xC5, 0xF2,
    0x62, 0x2B, 0x27, 0xAF, 0xFE, 0x91, 0xBE, 0x54, 0x76, 0xDE, 0xBB, 0x88, 0x78, 0xA3, 0xEC, 0xF9,
};

const uint8_t CVort_trans_map[] = {
    0x0, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x8, 0x9,
    0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6F, 0x70, 0x5B, 0x5D, 0x0D, 0x0, 0x61, 0x73,
    0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0x60, 0x0, 0x5C, 0x7A, 0x78, 0x63, 0x76,
    0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x0, 0x2A, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x2D, 0x1, 0x1, 0x1, 0x2B, 0x1,
    0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

const uint8_t CVort_char_map[] = {
    0x3F, 0x3F, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x2B, 0x3F, 0x3F,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49, 0x4F, 0x50, 0x5B, 0x5D, 0x7C, 0x3F, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3B, 0x22, 0x3F, 0x3F, 0x3F, 0x5A, 0x58, 0x43, 0x56,
    0x42, 0x4E, 0x4D, 0x2C, 0x2E, 0x2F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x0F, 0x3F, 0x2D, 0x15, 0x35, 0x11, 0x2B, 0x3F,
    0x13, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
    0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
};


const uint8_t CVort_uppercase_table[] = {
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x21, 0x21, 0x21, 0x21, 0x21, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x1, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
    0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x40, 0x40, 0x40, 0x40, 0x20,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

// Item values
const int16_t CVort_points_tbl[] = {
    500, 100, 200, 2000, 5000
};

// EGA Palettes
const uint8_t CVort_palettes[][17] = {
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3},
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x3},
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x3},
    {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x3},

};
#endif

void (*CVort_ptr_engine_processEXE)();
void (*CVort_ptr_engine_setTicks)(uint32_t currTicks);
uint32_t (*CVort_ptr_engine_getTicks)();
void (*CVort_ptr_engine_setTicksSync)(uint32_t currTicks);
uint32_t (*CVort_ptr_engine_getTicksSync)();
void (*CVort_ptr_show_pause_menu)();
void (*CVort_ptr_init_level)(uint16_t levelnum);
void (*CVort_ptr_draw_win)();
//void (*CVort_ptr_handle_secret_city)();
//void (*CVort_ptr_draw_earth_explode)();
uint16_t (*CVort_ptr_continue_game)();
void (*CVort_ptr_draw_scores)();
void (*CVort_ptr_show_ordering)(uint16_t isTimed);
void (*CVort_ptr_draw_ordering_info)();
void (*CVort_ptr_do_ordering)();
void (*CVort_ptr_inlevel_message)();
int16_t (*CVort_ptr_worldmap_sprites)(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y);
void (*CVort_ptr_contact_keen)(Sprite_T *keen, Sprite_T * contacted);

void CVort_start_cheating() {
    uint32_t origTicks = CVort_ptr_engine_getTicks();
    CVort_clear_keys();
    CVort_draw_box_opening_main(0x1A, 4);
    CVort_draw_string("You are now cheating!\n");
    if (engine_gameVersion == GAMEVER_KEEN1) {
        CVort_draw_string("You just got a pogo stick,\n");
        CVort_draw_string("all the key cards, and\n");
        CVort_draw_string("lots of ray gun charges.");
    } else { // GAMEVER_KEEN2, GAMEVER_KEEN3
        CVort_draw_string("You just got all the\n");
        CVort_draw_string("key cards, and lots of\n");
        CVort_draw_string("ray gun charges.");
    }
    keen_gp.stuff[3] = 1; // Done on Keen 2,3 as well
    keen_gp.ammo = 100;
    for (int16_t currKey = 0; currKey < 4; currKey++)
        keen_gp.stuff[currKey + 5] = 1;
    CVort_read_char_with_echo();
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    CVort_ptr_engine_setTicks(origTicks);
    CVort_clear_keys();
}


void CVort_handle_cheat_keys() {
    //CVort_engine_updateInputStatus();
    if (key_map[0x2E] && key_map[0x14] && key_map[0x39])
        CVort_start_cheating();
    if (key_map[0x22] && key_map[0x18] && key_map[0x20]) {
        CVort_clear_keys();
        CVort_draw_box_opening_main(0x14, 1);
        god_mode ^= 1;
        if (god_mode)
            CVort_draw_string("God mode enabled");
        else
            CVort_draw_string("God mode disabled");
        CVort_read_char_with_echo();
        CVort_engine_clearOverlay();
    } else if (key_map[0x39]) {
        CVort_engine_saveCurSound();
        CVort_ptr_show_pause_menu();
        CVort_engine_restoreCurSound();
    }
}

void CVort_main() {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        strcpy(game_ext, "CK1");
        CVort_engine_puts("Loading Commander Keen part 1... (version 1.31 1/23/90)");
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        strcpy(game_ext, "CK2");
        CVort_engine_puts("Loading Commander Keen part 2... (version 1.31 1/23/90)");
    } else if (engine_gameVersion == GAMEVER_KEEN3) {
        strcpy(game_ext, "CK3");
        CVort_engine_puts("Loading Commander Keen part 3... (version 1.31 1/23/90)");
    }

    if (CVort_engine_isJoystickInUse()) {
        CVort_engine_puts("Joystick detected");
        joy_detect = 1; // Actually never used on vanilla Keen...
    } else {
        CVort_engine_puts("Joystick not detected");
        joy_detect = 0;
    }
    // If we pass keystrokes to BIOS or not, it doesn't matter us.
    // But we simulate the message anyway... "cheating" though.
    pass_keys_to_bios = 1;
    if (!engine_arguments.passKeysToBios) {
        pass_keys_to_bios = 0;
        CVort_engine_puts("Keystrokes will not be passed to bios");
    }

    // We also don't need to detect EGA/VGA here.
    // But to preserve something...
    if (engine_arguments.isEmulatedGfxCardVga)
        CVort_engine_puts("VGA card detected");
    else
        CVort_engine_puts("EGA card detected");

    // CVort_clear_keys is called now only if EGA/VGA is not detected...
    //CVort_clear_keys();
    //CVort_translate_key(0);

    /* FIXME? This is a real hack that forces the display of the above
     * messages for a little bit. Otherwise these are simply not seen on
     * sufficiently fast machines (even if they can be seen with DOSEMU).
     */
    if (engine_gameVersion == GAMEVER_KEEN1) {
        CVort_engine_puts("Decompressing graphics, this may take some time...");
        // It takes a bit longer on vanilla Keen 1, due to graphics uncompression
        for (int loopVar = 0; loopVar < 80; loopVar++) {
            CVort_engine_shortSleep();
        }
    } else {
        for (int loopVar = 0; loopVar < 3; loopVar++) {
            CVort_engine_shortSleep();
        }
    }

    CVort_engine_decompGraphics();

    // TODO: Since tiles are now measured in *BYTE PER PIXEL*,
    // we add multiples of 256 rather than 32. Or that
    // we don't multiple at all(?), since indexes are all that matter now.
    const int scaleConst = 1;

    // animsetup
    for (uint16_t loopVar = 0, scaledLoopVar = 0; loopVar < engine_egaHeadGeneral.tileNum; loopVar++, scaledLoopVar += scaleConst) {
        switch (TILEINFO_Anim[loopVar]) {
            case 1: // One frame
                anim_frame_tiles[3][loopVar] = anim_frame_tiles[2][loopVar] = anim_frame_tiles[1][loopVar] = anim_frame_tiles[0][loopVar] = scaledLoopVar;
                break;
            case 2: // Two frames
                anim_frame_tiles[2][loopVar] = anim_frame_tiles[0][loopVar] = scaledLoopVar;
                anim_frame_tiles[3][loopVar] = anim_frame_tiles[1][loopVar] = scaledLoopVar + scaleConst;
                loopVar++;
                scaledLoopVar += scaleConst;
                anim_frame_tiles[2][loopVar] = anim_frame_tiles[0][loopVar] = scaledLoopVar;
                anim_frame_tiles[3][loopVar] = anim_frame_tiles[1][loopVar] = scaledLoopVar - scaleConst;
                break;
            case 4: // Four frames
                anim_frame_tiles[0][loopVar] = scaledLoopVar;
                anim_frame_tiles[1][loopVar] = scaledLoopVar + scaleConst;
                anim_frame_tiles[2][loopVar] = scaledLoopVar + 2 * scaleConst;
                anim_frame_tiles[3][loopVar] = scaledLoopVar + 3 * scaleConst;
                loopVar++;
                scaledLoopVar += scaleConst;
                anim_frame_tiles[0][loopVar] = scaledLoopVar;
                anim_frame_tiles[1][loopVar] = scaledLoopVar + scaleConst;
                anim_frame_tiles[2][loopVar] = scaledLoopVar + 2 * scaleConst;
                anim_frame_tiles[3][loopVar] = scaledLoopVar - scaleConst;
                loopVar++;
                scaledLoopVar += scaleConst;
                anim_frame_tiles[0][loopVar] = scaledLoopVar;
                anim_frame_tiles[1][loopVar] = scaledLoopVar + scaleConst;
                anim_frame_tiles[2][loopVar] = scaledLoopVar - 2 * scaleConst;
                anim_frame_tiles[3][loopVar] = scaledLoopVar - scaleConst;
                loopVar++;
                scaledLoopVar += scaleConst;
                anim_frame_tiles[0][loopVar] = scaledLoopVar;
                anim_frame_tiles[1][loopVar] = scaledLoopVar - 3 * scaleConst;
                anim_frame_tiles[2][loopVar] = scaledLoopVar - 2 * scaleConst;
                anim_frame_tiles[3][loopVar] = scaledLoopVar - scaleConst;
                break;
            default: // Invalid animation
                break;
        }
    }

    //sndsetup
    CVort_engine_loadSounds();

    CVort_init_rnd(true);
    CVort_setup_jump_heights(1);

    CVort_init_ctrls();
    // By default, keyboard input is FORCED on startup
    switch (engine_arguments.extras.initialEmulatedInputDevice) {
    case EMU_INITIAL_INPUT_KEYBOARD: // The one picked in vanilla Keen 1-3
        ctrl_type[1] = 0;
        break;
    case EMU_INITIAL_INPUT_MOUSE:
        ctrl_type[1] = 1;
        break;
    case EMU_INITIAL_INPUT_JOY1:
        ctrl_type[1] = 2;
        break;
    case EMU_INITIAL_INPUT_JOY2:
        ctrl_type[1] = 3;
        break;
    default: ; // EMU_INITIAL_INPUT_LAST
    }

    // We don't need to allocate memory for level data; It's done
    // statically: An array of 10000h bytes in the stack.
    textbox_offs_x = 0x17;
    anim_speed = 7;
    CVort_ptr_engine_setTicksSync(0);
    CVort_ptr_engine_setTicks(0);
    CVort_main_loop();
}

int16_t CVort_add_sprite() {
    int16_t spriteNum;
    for (spriteNum = 1; sprites[spriteNum].type_ && (spriteNum < num_sprites); spriteNum++);
    if (spriteNum >= num_sprites)
        num_sprites++;
    memset(&sprites[spriteNum], 0, sizeof (sprites[spriteNum]));
    sprites[spriteNum].think = &CVort_default_think;
    sprites[spriteNum].contact = &CVort_default_contact;
    sprites[spriteNum].active = 1;
    return spriteNum;
}

int16_t CVort_add_body() {
    int16_t bodyNum;
    for (bodyNum = 0; bodies[bodyNum].type_ && (bodyNum < num_bodies); bodyNum++);
    if (bodyNum >= num_bodies)
        num_bodies++;
    memset(&bodies[bodyNum], 0, sizeof (bodies[bodyNum]));
    // FIXME: The casting is potentially DANGEROUS, but the default_think
    // function is also used for sprites...
    bodies[bodyNum].think_ptr = (BodyThinkFunction_T)(&CVort_default_think);
    return bodyNum;
}

/*
 * In Vanilla keen, the text files are either linked into the .exe or they are
 * present externally.
 * Chocolate Keen will first look for an external file and then in the .exe
 */
void CVort_main_loop() {

    // The file names ARE constructed on vanilla Keen 2, although never used
    static char helptext[14] = "HELPTEXT.";
    static char endtext[14] = "ENDTEXT.";
    static char storytext[14] = "STORYTXT.";
    static char previewtext[14] = "PREVIEWS.";
    strcat((char*) helptext, game_ext);
    strcat((char*) endtext, game_ext);
    strcat((char*) storytext, game_ext);
    strcat((char*) previewtext, game_ext);

    if (engine_gameVersion == GAMEVER_KEEN1) {
        CVort_load_and_process_text_file(helptext, &help_text);
        CVort_load_and_process_text_file(storytext, &story_text);
        CVort_load_and_process_text_file(endtext, &end_text);
        CVort_load_and_process_text_file(previewtext, &previews_txt);
    } else if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {

        if (engine_gameVersion == GAMEVER_KEEN2)
        {
        // FIXME: Move these to engine exe processing?
            help_text = (uint8_t*) exeImage + CVort2_HELP_TEXT_OFFSET;
            story_text = (uint8_t*) exeImage + CVort2_STORY_TEXT_OFFSET;
            end_text = (uint8_t*) exeImage + CVort2_END_TEXT_OFFSET;
            previews_txt = (uint8_t*) exeImage + CVort2_PREVIEWS_TEXT_OFFSET;
        } else if (engine_gameVersion == GAMEVER_KEEN3) {
            help_text = (uint8_t*) exeImage + CVort3_HELP_TEXT_OFFSET;
            story_text = (uint8_t*) exeImage + CVort3_STORY_TEXT_OFFSET;
            end_text = (uint8_t*) exeImage + CVort3_END_TEXT_OFFSET;
            previews_txt = (uint8_t*) exeImage + CVort3_PREVIEWS_TEXT_OFFSET;

        }

        CVort_process_text_file(help_text);
        CVort_process_text_file(story_text);
        CVort_process_text_file(end_text);
        CVort_process_text_file(previews_txt);
    }

    on_world_map = 0;
    CVort_load_high_scores_table();
    CVort_fade_out();
    while (true) {
        CVort_do_intro_and_menu();
        CVort_draw_worldmap();
    }
}

void CVort_update_sprite_hitbox(void) {
    //update_sprite_hitbox_wmap(&temp_sprite)?
    int16_t tempNum = (temp_sprite.frame << 2) + (temp_sprite.posX >> 9) % 4;
    temp_MSE = engine_maskedSpriteEntry[tempNum];
    temp_sprite.boxX1 = temp_sprite.posX + temp_MSE.hitbox_l;
    temp_sprite.boxX2 = temp_sprite.posX + temp_MSE.hitbox_r;
    temp_sprite.boxY1 = temp_sprite.posY + temp_MSE.hitbox_u;
    temp_sprite.boxY2 = temp_sprite.posY + temp_MSE.hitbox_b;
}

void CVort_update_sprite_hitbox_wmap(Sprite_T *sprite) {
    int16_t tempNum = (sprite->frame << 2) + (sprite->posX >> 9) % 4;
    temp_MSE = engine_maskedSpriteEntry[tempNum];
    sprite->boxX1 = sprite->posX + temp_MSE.hitbox_l;
    sprite->boxX2 = sprite->posX + temp_MSE.hitbox_r;
    sprite->boxY1 = sprite->posY + temp_MSE.hitbox_u;
    sprite->boxY2 = sprite->posY + temp_MSE.hitbox_b;
}

/* Given a GameInput_T structure input, simulates a vanilla Keen do-while busy
 * loop waiting as long as the condition (input.but1jump || input.but2pogo)
 * applies. The main difference is that a sleep is requested - but NOT on the
 * first iteration of the loop. So, we are actually using a "standard" while
 * loop here.
 * Since this task is repeated often (waiting for keyboard/controller button
 * release) and requires a bit of "special" handling regarding the sleeps, it
 * resides in its own function definition now.
 */
inline void CVort_waitForNoGameButtonPress(GameInput_T *pInput) {
    *pInput = CVort_handle_ctrl(1);
    while (pInput->but1jump || pInput->but2pogo) {
        CVort_engine_shortSleep();
        *pInput = CVort_handle_ctrl(1);
    };
}

void CVort_draw_worldmap(void) {
    int16_t count_i, var4;
    int16_t level_return_code = 0; // draw_level return code; 0 == Died.
    uint16_t x_coord, y_coord;
    Sprite_T *keen_sp;
    GameInput_T input;
    CVort_clear_keys();
    CVort_engine_clearOverlay();
    CVort_load_level_data(80);

    if (engine_gameVersion == GAMEVER_KEEN3)
    {
        for (var4 = 0; var4 < map_height_T; var4++)
        {
            if (map_data_sprites[(map_width_T*var4)+map_width_T/2] == 0x2000)
            {
                messie_mounted = messie_time_to_climb = 0;
                messie_x_T = map_width_T/2;
                messie_y_T = var4;
                messie_delX = -0x200;
                messie_delY = 0;
                messie_xpos = messie_x_T << 12;
                messie_ypos = messie_y_T << 12;
                messie_frame = CVort3_spr_messield1;
                messie_move_tics = 8;
		break;
            }
        }

    }

    if (!resuming_saved_game) {
        CVort_place_keen_on_worldmap(&keen_gp.mapX, &keen_gp.mapY);
        // 32-bit additions (instead of two 16-bit ones with carry bit involved)
        keen_gp.screenX = keen_gp.mapX + 0xFFFF7000;
        keen_gp.screenY = keen_gp.mapY + 0xFFFFD000;
        keen_gp.score = 0;
    }
    wmap_scrollX = keen_gp.screenX;
    wmap_scrollY = keen_gp.screenY;
    keen_wmap_x_pos = keen_gp.mapX;
    keen_wmap_y_pos = keen_gp.mapY;
    wmap_col = 0x8000;
    extra_life_pts = (keen_gp.score / 20000)*20000;
    quit_to_title = 0;
    engine_currPage = 0;
    anim_speed = 3;
    do // World map loop
    {
        keen_sp = sprites;
        keen_sp->delX = keen_sp->delY = keen_sp->velX = keen_sp->velY = 0;
        if (engine_gameVersion == GAMEVER_KEEN1)
          keen_sp->frame = 0x24;//spr_mapkeend1;
        else if (engine_gameVersion == GAMEVER_KEEN2)
          keen_sp->frame = 0x24;//spr_mapkeend1;
        else if (engine_gameVersion == GAMEVER_KEEN3)
          keen_sp->frame = CVort3_spr_mapkeend1;//spr_mapkeend1;
        wmap_sprite_on = 0;
        CVort_load_level_data(80);
        CVort_mark_cities_done();

        for (count_i = 0; count_i < 0x10; count_i++) {
            // FIXME? Original code takes bx=count_i*6 and then
            // accesses wmaplevels.tx[bx].
            x_coord = wmaplevels[count_i].tx;
            y_coord = wmaplevels[count_i].ty;
            if (keen_gp.levels[count_i]) // Mark completed level?
            {
                // 1x1 sized entry
                if (wmaplevels[count_i].type_ == 0) {
                    map_data_sprites[y_coord * map_width_T + x_coord] = 0;
                    if (engine_gameVersion == GAMEVER_KEEN3) {
                        map_data_tiles[y_coord * map_width_T + x_coord] = 0x38;
                    } else {
                        map_data_tiles[y_coord * map_width_T + x_coord] = 0x4D;
                    }
                }// 2x2 sized entry
                else if (wmaplevels[count_i].type_ == 1) {
                    map_data_sprites[y_coord * map_width_T + x_coord] = 0;
                    map_data_sprites[y_coord * map_width_T + x_coord + 1] = 0;
                    map_data_sprites[(y_coord + 1) * map_width_T + x_coord] = 0;
                    map_data_sprites[(y_coord + 1) * map_width_T + x_coord + 1] = 0;
                    if (engine_gameVersion == GAMEVER_KEEN3) {
                        map_data_tiles[y_coord * map_width_T + x_coord] = 0x34;
                        map_data_tiles[y_coord * map_width_T + x_coord + 1] = 0x35;
                        map_data_tiles[(y_coord + 1) * map_width_T + x_coord] = 0x36;
                        map_data_tiles[(y_coord + 1) * map_width_T + x_coord + 1] = 0x37;
                    } else {
                        map_data_tiles[y_coord * map_width_T + x_coord] = 0x4E;
                        map_data_tiles[y_coord * map_width_T + x_coord + 1] = 0x4F;
                        map_data_tiles[(y_coord + 1) * map_width_T + x_coord] = 0x50;
                        map_data_tiles[(y_coord + 1) * map_width_T + x_coord + 1] = 0x51;
                    }
                }
            }
        }
        keen_sp->posX = keen_wmap_x_pos;
        keen_sp->posY = keen_wmap_y_pos;
        scrollX = wmap_scrollX;
        scrollY = wmap_scrollY;
        CVort_engine_syncDrawing();
        CVort_engine_drawScreen();
        CVort_engine_drawScreen();
        if (!level_return_code) // Keen died or new game started?
            CVort_show_keens_left();
        if (level_return_code < 2)
            CVort_fade_in();
        if (!level_return_code) {
            CVort_clear_keys();
            for (count_i = 0; count_i < 0x3C; count_i++) {
                CVort_engine_delay(1);
                input = CVort_handle_ctrl(1);
                if (input.but1jump || input.but2pogo || CVort_translate_key(1))
                    break;
            }
            CVort_waitForNoGameButtonPress(&input);
        }
        CVort_clear_keys();
        CVort_engine_clearOverlay();
        on_world_map = 1;

        do // Actual map loop
        {
            CVort_engine_syncDrawing();
#ifdef CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
            if (engine_gameVersion == GAMEVER_KEEN3)
                CVort3_do_messie(keen_sp);
                //(static_cast<CVorticons3 *>(this))->do_messie(keen_sp);
#endif
            CVort_move_worldmap(CVort_handle_ctrl(1), keen_sp);
            if (engine_gameVersion == GAMEVER_KEEN3)
                CVort_engine_drawSpriteAt(messie_xpos, messie_ypos, messie_frame + (y_coord&2)/2);

            CVort_engine_drawScreen();
            CVort_handle_cheat_keys();
            keen_wmap_x_pos = keen_sp->posX;
            keen_wmap_y_pos = keen_sp->posY;
            wmap_scrollX = scrollX;
            wmap_scrollY = scrollY;
            CVort_handle_global_keys();
            if (key_map[0x39])
                CVort_ptr_show_pause_menu();
            if (quit_to_title) {
                CVort_game_over();
                return;
            }
            y_coord++;  // This probably was not ycoord, but some new stack variable that was mistakenly not initialized
        } while (!wmap_sprite_on);

        on_world_map = 0;
        /* The original code takes wmap_sprite_on, a 16-bit value,
        shifts by 1 bit to the left and then uses this to access
        element number wmap_sprite_on in the array.
        With the G-O-D cheat it is easy to enter a place where 0x8000
        is added to the level number (in unsigned form). For instance,
        without the cheat wmap_sprite_on may be 7 for some level, but
        once you try to access the same level from a neighboring tile
        it can really become 0x8007.
        The left shift by 1 practically means there is no difference
        between 7 and 0x8007. So, we try to simulate that here.

        How? Well, we remove the most significant bit,
        or "and" with 0x7FFF.                                       */
        if (!keen_gp.levels[(wmap_sprite_on & 0x7FFF) - 1]) {
            CVort_engine_setCurSound(3);
            level_return_code = CVort_draw_level(wmap_sprite_on & 0xFF);
            if (quit_to_title) {
                quit_to_title = 0;
                CVort_game_over();
                return;
            }
            switch (level_return_code) {
                case LEVEL_END_DIE:
                    keen_gp.lives--;
                    continue; // Skip to the very end of the loop
                case LEVEL_END_EXIT:
                    keen_gp.levels[(wmap_sprite_on & 0x7FFF) - 1] = 1;
                    break;
#if CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
                case LEVEL_END_SECRET:
                    if (engine_gameVersion == GAMEVER_KEEN1) {
                        CVort1_handle_secret_city();
                    }
                    break;
#endif
#if CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
                case LEVEL_END_TANTALUS:
                    if (engine_gameVersion == GAMEVER_KEEN2) {
                        CVort_fade_out();
                        goto earthExplodes;
                    }
                    break;
#endif
                default:
                    break;
                    
            }
            if (engine_gameVersion == GAMEVER_KEEN1) {
                if (keen_gp.stuff[0] && keen_gp.stuff[4]
                    && keen_gp.stuff[1] && keen_gp.stuff[2]) {
                    CVort_ptr_draw_win();
                    CVort_game_over();
                    return;
                }
            } else if (engine_gameVersion == GAMEVER_KEEN2) {
                uint16_t loopVar, totalTargets;
                for (loopVar = 0, totalTargets=0; loopVar<8; loopVar++) {
                    totalTargets += keen_gp.targets[loopVar];
                }
                if (totalTargets == 8) {
                    CVort_ptr_draw_win();
                    CVort_game_over();
                    return;
                }
            } else {
                if ((wmap_sprite_on & 0xFF) == 16) {
                    CVort_ptr_draw_win();
                    CVort_game_over();
                    return;
                }
            }
        }
    } while (keen_gp.lives > -1);

    CVort_engine_finishCurSound();
#if CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
    if (engine_gameVersion == GAMEVER_KEEN2) {
earthExplodes:
        CVort2_draw_earth_explode();
    } else
#endif
    {
        CVort_fade_in();
    }
    CVort_game_over();
}

void CVort_mark_cities_done() {
    int16_t mapEntry, teleIndex, singleLoopVar;
    for (singleLoopVar = 0; singleLoopVar < 0x10; singleLoopVar++)
        wmaplevels[singleLoopVar].tx = 0;
    if (engine_gameVersion == GAMEVER_KEEN3) {
        for (singleLoopVar = 0; singleLoopVar < 0x10; singleLoopVar++)
            teleporters[singleLoopVar].isOnSnow = 0;
    }
    for (int16_t posY = 0, posX; posY < map_height_T; posY++)
        for (posX = 0; posX < map_width_T; posX++) {
            mapEntry = map_data_sprites[posY * map_width_T + posX]&0x7FFF;

            if (engine_gameVersion == GAMEVER_KEEN3)
            {
                // Check for teleporter
                if ((mapEntry&0xF00) == 0xF00)
                {
                    teleIndex = (mapEntry&0xF0)/16;
                    teleporters[teleIndex].isOnSnow = mapEntry&0xF; // Record the target tele
                    teleporters[teleIndex].destX = posX;
                    teleporters[teleIndex].destY = posY;
                }
            }
            /* TODO? For mapEntry>0x10 this results in erroneous
            memory accesses. This shall be avoided! Although,
            the vanilla way does accept 0x10<mapEntry<0x100...
            ...
            But the vanilla way, in this modern implementation, may
            have been a cause for crashes with OpenGL and video mode
            changed (between fullscreen and windowed).
            In fact, an inspection of the CVorticons class structure
            (as of version 0.3 or 0.4.0) reveals why.
            It really seemed to get out of the bounds of the
            CVorticons object in memory!!!!!!                     */
            if ((mapEntry <= 0) || (mapEntry > 0x10) || wmaplevels[mapEntry - 1].tx)
                //if ((mapEntry <= 0) || (mapEntry >= 0x100) || wmaplevels[mapEntry-1].tx)
                continue;
            wmaplevels[mapEntry - 1].tx = posX;
            wmaplevels[mapEntry - 1].ty = posY;
            wmaplevels[mapEntry - 1].type_ = 0;

            if (mapEntry != (map_data_sprites[posY * map_width_T + posX + 1]&0x7FFF))
                continue;
            posX++;
            wmaplevels[mapEntry - 1].type_ = 1;
        }
}

void CVort_erase_parents_text() {
    CVort_draw_char_rectangle(textbox_x_pos, textbox_y_pos, textbox_width, textbox_height, 0);
    //engine_updateActualDisplay();
}

void CVort_draw_string_finale(const char *str) {
    char shortBuffer[2];
    shortBuffer[1] = '\0';
    for (; *str; str++) {
        shortBuffer[0] = *str;
        CVort_draw_string(shortBuffer);
        //engine_updateActualDisplay();
        CVort_engine_delay(6);
    }
}

void CVort_game_over() {
    char ranknames[][7] = {"first", "second", "third", "fourth", "fifth", "sixth", "last"};
    char score_file[14], scorestr[10], citystr[10];
    int16_t oldX, oldY;
    strncpy(score_file, "SCORES.", 13);
    strcat(score_file, game_ext);
    int16_t scoreEntry = 0, loopVar;
    // Check if we have a new high score
    for (loopVar = 0; loopVar < 7; loopVar++)
        if (high_scores_table.scores[loopVar] < keen_gp.score) {
            scoreEntry = loopVar + 1;
            break;
        }
    scrollX &= 0xFFFFF000;
    scrollY &= 0xFFFFF000;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    CVort_engine_drawScreen();
    if (scoreEntry) // Got a new high score
    {
        CVort_draw_box_opening_main(0x20, 0xD);
        oldX = cursorX;
        oldY = cursorY;
    } else // No new high score
    {
        cursorX = 7;
        cursorY = 0xA;
    }
    CVort_engine_drawBitmap(cursorX + 9, cursorY << 3, (engine_gameVersion == GAMEVER_KEEN3) ? 6 : 7);
    CVort_engine_setCurSound(0x10);
    if (scoreEntry) {
        scoreEntry--;
        cursorX = cursorX_save = oldX + 1;
        cursorY = oldY + 4;
        CVort_draw_string("         SCORE:");
        sprintf(scorestr, "%" PRId32, keen_gp.score);
        CVort_draw_string(scorestr);
        cursorX = cursorX_save + 0xB;
        if (engine_gameVersion == GAMEVER_KEEN1) {
            if (keen_gp.stuff[0])
                CVort_engine_drawTile(cursorX, ((cursorY + 2) << 3) - 4, 0x1C0);
            if (keen_gp.stuff[4])
                CVort_engine_drawTile(cursorX + 2, ((cursorY + 2) << 3) - 4, 0x1C1);
            if (keen_gp.stuff[1])
                CVort_engine_drawTile(cursorX + 4, ((cursorY + 2) << 3) - 4, 0x1C2);
            if (keen_gp.stuff[2])
                CVort_engine_drawTile(cursorX + 6, ((cursorY + 2) << 3) - 4, 0x1C3);
        } else if (engine_gameVersion == GAMEVER_KEEN2) {
            uint16_t citiesSaved = 0;
            for (loopVar = 0; loopVar < 8; loopVar++)
                if (keen_gp.targets[loopVar])
                    citiesSaved++;

            cursorX = cursorX_save + 6;
            cursorY += 2;
            sprintf(citystr, "%" PRIu16, citiesSaved);
            CVort_draw_string("You saved ");
            if (!citiesSaved)
            {
                CVort_draw_string("no cities!");
            }
            else 
            {
                CVort_draw_string(citystr);
                if (citiesSaved == 1)
                    CVort_draw_string(" city!");
                else
                    CVort_draw_string(" cities!");
            }

            cursorY -= 2;
        }
          
        cursorX = cursorX_save;
        cursorY += 4;
        CVort_draw_string("       CONGRATULATIONS!\n");
        CVort_draw_string("     You got ");
        // A little "hack" used in game to quickly access a rank string
        CVort_draw_string(ranknames[scoreEntry]);
        CVort_draw_string(" place!\n\n\nEnter your name:");
        // Shift lower scores in the table
        for (loopVar = 5; loopVar >= scoreEntry; loopVar--) {
            high_scores_table.scores[loopVar + 1] = high_scores_table.scores[loopVar];
            high_scores_table.targets[loopVar + 1] = high_scores_table.targets[loopVar];
            high_scores_table.unknown[loopVar + 1] = high_scores_table.unknown[loopVar];
            high_scores_table.parts[loopVar + 1][0] = high_scores_table.parts[loopVar][0];
            high_scores_table.parts[loopVar + 1][1] = high_scores_table.parts[loopVar][1];
            // NOTE THE ORDER!!! (It may not matter HERE, but it DOES elsewhere!)
            high_scores_table.parts[loopVar + 1][3] = high_scores_table.parts[loopVar][3];
            high_scores_table.parts[loopVar + 1][2] = high_scores_table.parts[loopVar][2];
            strcpy(high_scores_table.names[loopVar + 1], high_scores_table.names[loopVar]);
        }
        CVort_clear_keys();
        CVort_get_string_input(high_scores_table.names[scoreEntry], 12);
        high_scores_table.scores[scoreEntry] = keen_gp.score;
        high_scores_table.parts[scoreEntry][0] = keen_gp.stuff[0];
        high_scores_table.parts[scoreEntry][1] = keen_gp.stuff[4];
        // NOTE THE ORDER HERE!!! THIS IS *IMPORTANT*!
        high_scores_table.parts[scoreEntry][3] = keen_gp.stuff[1];
        high_scores_table.parts[scoreEntry][2] = keen_gp.stuff[2];

        high_scores_table.targets[scoreEntry] = 0;
        for (loopVar = 0; loopVar < 8; loopVar++)
            high_scores_table.targets[scoreEntry] += keen_gp.targets[loopVar];

        high_scores_table.unknown[scoreEntry] = keen_gp.unknown;

        CVort_save_high_scores_table();
    }// THIS IS IMPORTANT! (For kind of a vanilla behavior...?)
    else
        scoreEntry--;

    GameInput_T input;
    CVort_waitForNoGameButtonPress(&input);
    CVort_clear_keys();
    if (scoreEntry < 0) {
        //engine_updateActualDisplay();
        for (loopVar = 0; loopVar < 360; loopVar++) {
            CVort_engine_delay(1);
            input = CVort_handle_ctrl(1);
            if (input.but1jump || input.but2pogo || CVort_translate_key(1))
                break;
        }
    }
    CVort_fade_out();
    CVort_clear_keys();
    CVort_load_level_data(90);
    CVort_do_scores();
    CVort_fade_in();
    int16_t timeLeft = 2400;
    on_world_map = 0;
    do {
        CVort_engine_syncDrawing();
        CVort_engine_drawScreen();
        input = CVort_handle_ctrl(1);
        if (CVort_handle_global_keys())
            CVort_do_scores();
        timeLeft -= sprite_sync;
        if (input.but1jump || input.but2pogo || CVort_translate_key(1))
            break;
    } while (!(input.but1jump || input.but2pogo || CVort_translate_key(1))
            && (timeLeft > 0));
    CVort_fade_out();
}

void CVort_show_keens_left() {
    uint16_t old_xpos;
    CVort_draw_box_opening_main(0x10, 4);
    old_xpos = cursorX;
    CVort_draw_string("   Keens Left");
    int16_t keensToDraw = keen_gp.lives;
    if (keensToDraw > 6)
        keensToDraw = 6;
    for (int16_t currKeen = 0; currKeen < keensToDraw; currKeen++)
        CVort_engine_drawSprite(old_xpos + (currKeen << 1) + 1, (cursorY << 3) + 0xB, engine_gameVersion == GAMEVER_KEEN1? 0x90 : engine_gameVersion == GAMEVER_KEEN2? 0x90 : CVort3_spr_mapkeend1*4);
    CVort_engine_setCurSound(0x29);
}

void CVort_place_keen_on_worldmap(uint32_t *posX, uint32_t *posY) {
    int16_t mapEntry;
    for (int16_t currY = 0, currX; currY < map_height_T; currY++)
        for (currX = 0; currX < map_width_T; currX++) {
            mapEntry = map_data_sprites[currY * map_width_T + currX];
            if (mapEntry != 0xFF)
                continue;
            *posX = ((int32_t)(currX)) << 12;
            *posY = ((int32_t)(currY)) << 12;
            return;
        }
}

void CVort_demo_toggle_reset_player_partial_state_before();
void CVort_demo_toggle_reset_player_partial_state_after();

void CVort_record_demo(int16_t demo_number) {
    demo_actions_including_level_num[0] = current_level;
    demo_action_ptr = 1 + demo_actions_including_level_num;
    demo_status = DEMO_RECORD;
}

void CVort_load_demo(int16_t demo_number) {
    FILE *fp;
    sprintf(string_buf, "DEMO%d.%s", demo_number, game_ext);
    fp = CVort_engine_cross_rw_misc_fopen(string_buf, "rb");
    uint32_t filesize = CVort_filelength(fp);
    fread(demo_actions_including_level_num, filesize, 1, fp);
    fclose(fp);
    current_level = demo_actions_including_level_num[0];
    demo_action_ptr = 1 + demo_actions_including_level_num;
    demo_status = DEMO_PLAY;
}

void CVort_save_demo(int16_t demo_number) {
    FILE *fp;
    sprintf(string_buf, "DEMO%d.%s", demo_number, game_ext);
    fp = CVort_engine_cross_rw_misc_fopen(string_buf, "wb");
    fwrite(demo_actions_including_level_num, demo_action_ptr-demo_actions_including_level_num, 1, fp);
    fclose(fp);
    demo_status = DEMO_OFF;
}

void CVort_do_intro_and_menu() {
    int16_t introTickCounter;
    uint16_t introCurrScreen;
    GameInput_T currInput;
    bool doHalt;
    FILE *fp;
    resuming_saved_game = 0;
    keen_gp.lives = 4;
    // ACTUALLY: Vanilla Keen 2 sets the ammo to 0, then stuff[3]
    // (pogo stick) to 1 and finally the ammo to 3 again! 
    if (engine_gameVersion == GAMEVER_KEEN1) {
        keen_gp.ammo = 0;
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        keen_gp.ammo = 3;
    } else if (engine_gameVersion == GAMEVER_KEEN3) {
        keen_gp.ammo = 5;
    }
    memset(keen_gp.stuff, 0, sizeof (keen_gp.stuff));
    memset(keen_gp.targets, 0, sizeof (keen_gp.targets));
    memset(keen_gp.levels, 0, sizeof (keen_gp.levels));
    if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {
        keen_gp.stuff[3] = 1;
    }
    // scrollup
    while (1) {
        engine_currPage = 0;
        anim_speed = 3;
        CVort_engine_clearOverlay();
        CVort_load_level_data(90);
        // Show Apogee logo
        if (reshow_scroll_up) // Uncomment this to replay scroll up upon demo restart
        {
            CVort_load_level_data(90); // FIXME: Again???
            scrollX = 0x68000;
            scrollY = 0x2000;
            CVort_engine_clearOverlay();
            CVort_engine_syncDrawing();
            CVort_engine_drawScreen();
            CVort_fade_in();
            introTickCounter = 2400;
            introCurrScreen = 4;
            CVort_scroll_up_logo();
            CVort_fade_out();
            reshow_scroll_up = 0; // Uncomment this line to replay scroll up
        }
        // Fade into menu
        //introTickCounter = 0xC0;
        introTickCounter = engine_arguments.extras.vorticonsDemoModeToggle ? 146 : 2400;
        introCurrScreen = 0;
        scrollX = scrollY = 0x2000;
        CVort_engine_clearOverlay();
        CVort_engine_syncDrawing();
        CVort_do_draw_mural();
        CVort_fade_in();
        CVort_clear_keys();

        // Menu
        while (1) {
            currInput = CVort_handle_ctrl(1);
            CVort_engine_syncDrawing();
            CVort_engine_drawScreen();
            if (engine_arguments.extras.vorticonsDemoModeToggle) {
                if (CVort_handle_global_keys()) {
                    CVort_do_draw_mural();
                }
                introTickCounter -= sprite_sync;
                if (introTickCounter <= 0) {
                    for (; introCurrScreen < 10; introCurrScreen++) {
                        sprintf(string_buf, "DEMO%d.%s", introCurrScreen, game_ext);
                        fp = CVort_engine_cross_rw_misc_fopen(string_buf, "rb");
                        if (!fp) {
                            continue;
                        }
                        end_of_demo_ptr = demo_actions_including_level_num + CVort_filelength(fp);
                        fclose(fp);
                        CVort_load_demo(introCurrScreen);
                        quit_to_title = 0;
                        rnd = 0;
                        CVort_setup_jump_heights(0);
                        memset(&input_old, 0, sizeof(input_old));
                        CVort_demo_toggle_reset_player_partial_state_before();
                        CVort_draw_level(current_level);
                        demo_status = DEMO_OFF;
                        CVort_demo_toggle_reset_player_partial_state_after();
                        // 1 - player halt. 2 - Demo halt (may also be 0).
                        if (quit_to_title == 1) {
                            break;
                        }
                    }
                    introCurrScreen = 0;
                    CVort_fade_out();
                    CVort_load_level_data(90);
                    introTickCounter = 146;
                    CVort_engine_syncDrawing();
                    CVort_engine_drawScreen();
                    scrollX = scrollY = 0x2000;
                    CVort_engine_clearOverlay();
                    CVort_do_draw_mural();
                    CVort_fade_in();
                    // PLAYER halt? (NOT demo, i.e. 2.)
                    if (quit_to_title == 1) {
                        break;
                    }
                    continue;
                }
            } else {
                if (CVort_handle_global_keys())
                    switch (introCurrScreen) {
                        case 0: CVort_do_draw_mural();
                            break;
                        case 1: CVort_do_about_us();
                            break;
                        case 2: CVort_do_scores();
                            break;
                        case 3:
                            CVort_ptr_do_ordering();
                            CVort_engine_drawScreen();
                            CVort_ptr_do_ordering();
                            break;
                        default: break;
                    }
                introTickCounter -= sprite_sync;
                if (introTickCounter <= 0) {
                    CVort_fade_out();
                    switch (introCurrScreen) {
                        case 0:
                            scrollX = 0x3F000;
                            scrollY = 0x1C00;
                            CVort_do_about_us();
                            doHalt = false;
                            break;
                        case 1:
                            scrollX = 0x54000;
                            scrollY = 0x2000;
                            CVort_do_scores();
                            doHalt = false;
                            break;
                        case 2:
                            introCurrScreen++;
                            CVort_ptr_show_ordering(1);
                            currInput = CVort_handle_ctrl(1);
                            doHalt = (CVort_translate_key(1) || currInput.but1jump || currInput.but2pogo);
                            if (doHalt) {
                                //introTickCounter = 0xC0;
                                introTickCounter = 2400;
                                break;
                            }
                            CVort_fade_out();
                            break;
                        default: break;
                    }
                    if (!doHalt) {
                        //introTickCounter = 0xC0;
                        introTickCounter = 2400;
                        CVort_engine_syncDrawing();
                        CVort_engine_drawScreen();
                        introCurrScreen++;
                        if (introCurrScreen == 4) {
                            introCurrScreen = 0;
                            scrollX = scrollY = 0x2000;
                            CVort_engine_clearOverlay();
                            CVort_do_draw_mural();
                        }
                        CVort_fade_in();
                    }
                }
            }
            if (CVort_translate_key(1) || currInput.but1jump || currInput.but2pogo || intro_complete)
                break;
        }
        intro_complete = 0;
        if ((scrollX >> 16) || ((scrollX & 0xFFFF) != 0x2000))
            CVort_fade_out();
        //if (CVort_draw_title(introTickCounter))
        if (CVort_draw_title())
            break;
    }
    CVort_clear_keys();
    CVort_fade_out();
}

/* Here we need to take advantage of engine_updateActualDisplay(), since within
the vanilla code, nothing like page flipping and other tricks is done.
It should be used wisely so no "selector blanking" artifact is seen, though.

FIXME: What would occur if the host video card syncs to vertical retrace at a
rate of less than 70Hz? Well, slowdowns are expected...                       */
uint16_t CVort_draw_title() {
    uint16_t sel, bg, var_6, var_8;
    int16_t loopVar, currCharOffset;
    GameInput_T input;
loc_19297:
    if ((scrollX >> 16) || ((scrollX & 0xFFFF) != 0x2000)) {
        CVort_do_start_menu();
        CVort_fade_in();
    } else
        CVort_do_start_menu();
loc_192B1:
    CVort_clear_keys();
    sel = 0;
    currCharOffset = 0;
    var_8 = 0x14;
loc_192C0:
    for (loopVar = 0; loopVar < 6; loopVar++) {
        CVort_engine_delay(1);
        input = CVort_handle_ctrl(1);
        if (input.direction != 8)
            break;
    }
loc_192F8:
    CVort_engine_drawChar(cursorX, cursorY << 3, currCharOffset + 9);
    //engine_updateActualDisplay();
    currCharOffset++;
    if (currCharOffset > 2)
        currCharOffset = 0;

    if (input.direction == 0) // Move up
    {
        if (!sel) // Already on top? Then we cycle to the bottom.
        {
            sel = 7;
            CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
        } else {
            sel--;
            for (loopVar = 0; loopVar < 8; loopVar++) {
                CVort_engine_drawChar(cursorX, (cursorY << 3) - loopVar, currCharOffset + 9);
                //engine_updateActualDisplay();
                CVort_engine_delay(1);
                CVort_engine_drawChar(cursorX, (cursorY << 3) - loopVar, ' ');
                currCharOffset++;
                if (currCharOffset > 2)
                    currCharOffset = 0;
            }
        }
        cursorY = cursorY_b + sel;
        CVort_engine_drawChar(cursorX, cursorY << 3, currCharOffset + 9);
        //engine_updateActualDisplay();

        while (1) {
            if (!var_8) {
                //var_8--;
                break;
            }
            var_8--;
            if (input.direction == 8)
                break;
            input = CVort_handle_ctrl(1);
            CVort_engine_delay(1);
        }
        var_8 = 0x14;
        CVort_clear_keys();
    } else if (input.direction == 4) // Move down
    {
        if (sel >= 7) // Already on bottom?
        {
            sel = 0;
            CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
        } else {
            sel++;
            for (loopVar = 0; loopVar < 8; loopVar++) {
                CVort_engine_drawChar(cursorX, (cursorY << 3) + loopVar, currCharOffset + 9);
                //engine_updateActualDisplay();
                CVort_engine_delay(1);
                CVort_engine_drawChar(cursorX, (cursorY << 3) + loopVar, ' ');
                currCharOffset++;
                if (currCharOffset > 2)
                    currCharOffset = 0;
            }
        }
        cursorY = cursorY_b + sel;
        CVort_engine_drawChar(cursorX, cursorY << 3, currCharOffset + 9);
        //engine_updateActualDisplay();

        while (1) {
            if (!var_8) {
                //var_8--;
                break;
            }
            var_8--;
            if (input.direction == 8)
                break;
            input = CVort_handle_ctrl(1);
            CVort_engine_delay(1);
        }
        var_8 = 0x14;
        CVort_clear_keys();
    } else // No move
        CVort_engine_delay(7);
select1:
    var_6 = CVort_translate_key(1)&0xFF;
    if (var_6 && ctrl_type[1])
        input.but1jump = 1;
    if (((var_6 == 0x20) || (var_6 == 0xD)) && (!ctrl_type[1]))
        input.but1jump = 1;
    if (CVort_handle_global_keys()) {
        CVort_do_start_menu();
        cursorY = sel + cursorY_b;
    }
    if (!input.but1jump && !input.but2pogo)
        goto loc_192C0; // FIXME FIXME FIXME FIXME FIXME FIXME GOTO?!!!

    CVort_clear_keys();
    CVort_engine_delay(8);
    switch (sel) {
        case 0:
            bg = 1;
            break;
        case 1:
            bg = CVort_ptr_continue_game();
            break;
        case 2:
            bg = 0;
            CVort_do_story();
            break;
        case 3:
            bg = 0;
            CVort_fade_out();
            CVort_show_about_us();
            CVort_fade_out();
            break;
        case 4:
            bg = 0;
            CVort_fade_out();
            CVort_show_scores();
            CVort_fade_out();
            break;
        case 5:
            bg = 0;
            CVort_fade_out();
            CVort_ptr_show_ordering(0);
            CVort_fade_out();
            break;
        case 6:
            bg = 0;
            CVort_fade_out();
            CVort_do_previews();
            CVort_fade_out();
            break;
        case 7:
            bg = 0;
            CVort_fade_out();
            break;
        default:
            break;
    }
    if ((sel != 7) && !bg)
        goto loc_19297; // FIXME FIXME FIXME FIXME FIXME FIXME FIXME
    return bg;
}

void CVort_draw_menu() {
    CVort_draw_box_opening_main(0x12, 0xA);
    cursorX_b = cursorX;
    cursorY_b = cursorY;
    CVort_draw_string("   New Game\n");
    if (engine_arguments.extras.vorticonsDemoModeToggle) {
        CVort_draw_string("   Record A Demo\n");
    } else {
        CVort_draw_string("   Continue Game\n");
    }
    CVort_draw_string("   Story\n");
    CVort_draw_string("   About ID...\n");
    CVort_draw_string("   High Scores\n");
    CVort_draw_string("   Ordering Info\n");
    CVort_draw_string("   Previews!\n");
    CVort_draw_string("   Restart Demo\n\n");
    CVort_draw_string("Use the ");

    switch (ctrl_type[1]) {
        case 0:
            CVort_draw_string("arrows");
            break;
        case 1:
            CVort_draw_string("mouse");
            break;
        case 2:
        case 3:
            CVort_draw_string("joystick");
            break;
        default:
            break;
    }
    cursorX = cursorX_b + 1;
    cursorY = cursorY_b;
}

void CVort_do_start_menu() {
    scrollX = scrollY = 0x2000;
    CVort_engine_clearOverlay();
    CVort_do_draw_mural();
    draw_func = &CVort_draw_menu;
    CVort_engine_drawScreen();
    draw_func = 0;
}

void CVort_do_previews() {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        CVort_engine_showImageFile("PREVIEW2.CK1");
        CVort_fade_in();
        CVort_engine_delay(0x12C);
        CVort_fade_out();
        CVort_engine_showImageFile("PREVIEW3.CK1");
        CVort_fade_in();
        CVort_load_level_data(90);
        CVort_engine_delay(0x12C);
    } else if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {
        CVort_do_text_viewer_short(previews_txt, 0, 0x16);
        CVort_fade_in();
    }
    CVort_do_text_viewer(previews_txt, 0, 0x16);
    if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {
        
        scrollX = 0x54000;
    }
}

void CVort_show_about_us() {
    GameInput_T input;
    scrollX = 0x3F000;
    scrollY = 0x1C00;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    CVort_do_about_us();
    CVort_fade_in();
    CVort_clear_keys();
    do {
        CVort_engine_syncDrawing();
        CVort_engine_drawScreen();
        if (CVort_handle_global_keys())
            CVort_do_about_us();
        input = CVort_handle_ctrl(1);
        // TODO: What should be done here (i.e. like in vanilla code)???
        // For now check for jump or pogo control action.
        // Also check for general keyboard key presses.
        if (input.but1jump || input.but2pogo || CVort_translate_key(1))
            return;
    } while (1);
}

void CVort_draw_mural() {
    CVort_engine_drawBitmap(8, 1, 0);
    CVort_engine_drawBitmap((engine_gameVersion == GAMEVER_KEEN3) ? 0x13 : 0x10, 0xb6, 2);
}

void CVort_do_draw_mural() {
    draw_func = &CVort_draw_mural;
    CVort_engine_syncDrawing();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = 0;
    CVort_clear_keys();
}

void CVort_draw_about_us() {
    char lines[][40] = {
        "We are a group of Software Artists\n",
        "whose goal is to bring commercial\n",
        "quality software to the public\n",
        "at shareware prices.\n\n",
        "Our effort is only possible with\n",
        "your support. Without it, we cannot\n",
        "continue to make this fine\n",
        "software so affordable.\n\n",
        "Thank you in advance for your\n",
        "contribution to the future of the\n",
        "growing shareware market.",
    };

    CVort_engine_drawBitmap(0x11, 0x17, 1);
    cursorX_save = cursorX = 7;
    cursorY = 9;
    for (uint16_t row = 0; row < 11; row++)
        CVort_draw_string_80(lines[row]);
}

void CVort_do_about_us() {
    draw_func = &CVort_draw_about_us;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = 0;
    CVort_clear_keys();
}


void CVort_do_scores() {
    scrollX = 0x54000;
    scrollY = 0x2000;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    draw_func = CVort_ptr_draw_scores; // NOT &CVort_ptr_draw_scores
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = 0;
}

void CVort_show_scores() {
    GameInput_T input;
    CVort_do_scores();
    CVort_fade_in();
    CVort_clear_keys();
    do {
        CVort_engine_syncDrawing();
        CVort_engine_drawScreen();
        if (CVort_handle_global_keys())
            CVort_do_scores();
        input = CVort_handle_ctrl(1);
        // TODO: What should be done here (i.e. like in vanilla code)???
        // For now check for jump or pogo control action.
        // Also check for general keyboard key presses.
        if (input.but1jump || input.but2pogo || CVort_translate_key(1))
            return;
    } while (1);
}

void CVort_save_game() {
    char path[13];
    FILE *fp;
    int8_t inputChar, confirmChar;
    // FIXME: Quite hackish but... more true to the original?
    strncpy(path, "SAVED?.", 13);
    strcat(path, game_ext);
    if (!on_world_map) {
        CVort_draw_box_opening_main(0x16, 3);
        CVort_draw_string("You can SAVE the game\n");
        CVort_draw_string("ONLY on the World Map!\n");
        CVort_draw_string("    press a key:");
        CVort_clear_keys();
        CVort_wait_for_key();
        return;
    }
    do {
        CVort_draw_box_opening_main(0x14, 3);
        CVort_draw_string("Which game position\n");
        CVort_draw_string("do you want to save?\n");
        CVort_draw_string("    1-9 or ESC:");
        do
            inputChar = CVort_read_char_with_echo()&0xFF; while (((inputChar < '1') || (inputChar > '9')) && (inputChar != 0x1B));
        if (inputChar == 0x1B)
            return;
        // FIXME: Seems a bit hackish but... well...
        path[5] = inputChar;
        // First check if file already exists
        fp = CVort_engine_cross_rw_misc_fopen(path, "rb");
        if (fp) {
            fclose(fp);
            CVort_draw_box_opening_main(0x14, 3);
            CVort_draw_string("That game position\n");
            CVort_draw_string("already exists!\n");
            CVort_draw_string("Overwrite it?:");
            do
                confirmChar = CVort_engine_toupper(CVort_read_char_with_echo()&0xFF); while ((confirmChar != 'Y') && (confirmChar != 'N') && (confirmChar != 0x1B));
            if (confirmChar == 0x1B)
                return;
            if (confirmChar == 'N')
                continue;
        }
        break;
    } while (1);

    keen_gp.screenX = wmap_scrollX;
    keen_gp.screenY = wmap_scrollY;
    keen_gp.mapX = keen_wmap_x_pos;
    keen_gp.mapY = keen_wmap_y_pos;

    fp = CVort_engine_cross_rw_misc_fopen(path, "wb");
    if (!fp) // FIXME: And what if it fails...
        return;
    CVort_engine_cross_fwriteInt16LE(keen_gp.stuff, 9, fp);
    CVort_engine_cross_fwriteInt16LE(keen_gp.levels, 16, fp);
    CVort_engine_cross_fwriteInt16LE(&keen_gp.lives, 1, fp);
    CVort_engine_cross_fwriteInt16LE(&keen_gp.ammo, 1, fp);
    CVort_engine_cross_fwriteInt32LE(&keen_gp.score, 1, fp);
    CVort_engine_cross_fwriteInt32LE(&keen_gp.mapX, 1, fp);
    CVort_engine_cross_fwriteInt32LE(&keen_gp.mapY, 1, fp);
    CVort_engine_cross_fwriteInt32LE(&keen_gp.screenX, 1, fp);
    CVort_engine_cross_fwriteInt32LE(&keen_gp.screenY, 1, fp);
    CVort_engine_cross_fwriteInt16LE(keen_gp.targets, 8, fp);
    CVort_engine_cross_fwriteInt16LE(&keen_gp.unknown, 1, fp);
    fclose(fp);

    CVort_draw_box_opening_main(0x1D, 3);
    CVort_draw_string("You can continue this game\n");
    CVort_draw_string("from the Main Menu next time\n");
    CVort_draw_string("you play. Press a key:");
    CVort_read_char_with_echo();
}

uint16_t CVort_private_continue_game() {
    char path[13];
    FILE *fp;
    int8_t inputChar;
    // FIXME: Again hackish but... same as CVort_save_game()...
    strncpy(path, "SAVED?.", 13);
    // Possibly a bit less vanilla, but actually works the proper
    // (and vanilla) way!
    strcat(path, game_ext);
    CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
    do {
        CVort_draw_box_opening_main(0x19, 2);
        cursorX_b = cursorX;
        cursorY_b = cursorY;
        CVort_draw_string("  Continue Which Game?\n");
        CVort_draw_string("    1-9 or ESC:");
        do
            inputChar = CVort_read_char_with_echo()&0xFF; while (((inputChar < '1') || (inputChar > '9')) && (inputChar != 0x1B));
        if (inputChar == 0x1B)
            return 0;
        // FIXME: Hackish just like in CVort_save_game()...
        path[5] = inputChar;
        // FIXME FIXME!!! Vanilla behavior concatenates the file
        // extension ".CK1" until we have a proper answer!!!!!!
        // Or so it seems.
        // Wait... no. We can't select, say, an empty saved slot, and
        // then select an actual one immediately. However, on vanilla
        // Keen it works. So... lets just do it the better way now.

        //strcat(path, game_ext);
        fp = CVort_engine_cross_rw_misc_fopen(path, "rb");
        if (!fp) {
            CVort_draw_box_opening_main(0x19, 2);
            CVort_draw_string("  That game hasn't\n");
            CVort_draw_string("  been saved yet!:");
            CVort_read_char_with_echo();
            continue;
        }
        if (CVort_filelength(fp) == 92) {
            CVort_engine_cross_freadInt16LE(keen_gp.stuff, 9, fp);
            CVort_engine_cross_freadInt16LE(keen_gp.levels, 16, fp);
            CVort_engine_cross_freadInt16LE(&keen_gp.lives, 1, fp);
            CVort_engine_cross_freadInt16LE(&keen_gp.ammo, 1, fp);
            CVort_engine_cross_freadInt32LE(&keen_gp.score, 1, fp);
            CVort_engine_cross_freadInt32LE(&keen_gp.mapX, 1, fp);
            CVort_engine_cross_freadInt32LE(&keen_gp.mapY, 1, fp);
            CVort_engine_cross_freadInt32LE(&keen_gp.screenX, 1, fp);
            CVort_engine_cross_freadInt32LE(&keen_gp.screenY, 1, fp);
            CVort_engine_cross_freadInt16LE(keen_gp.targets, 8, fp);
            CVort_engine_cross_freadInt16LE(&keen_gp.unknown, 1, fp);
            fclose(fp);
            resuming_saved_game = 1;
            return 1;
        } else {
            fclose(fp);
            CVort_draw_box_opening_main(0x19, 2);
            CVort_draw_string("That file is incompatible\n");
            CVort_draw_string("with this verion of CK:");
            CVort_read_char_with_echo();
        }
    } while (1);
}

uint16_t CVort_demo_toggle_prepare_to_record() {
	char demostr[3];
	CVort_draw_box_opening_main(0x1A, 1);
	CVort_draw_string("Choose a level (1-99): ");
	CVort_clear_keys();
	if (!CVort_get_string_input(demostr, 2)) {
		return 0;
	}
	current_level = atoi(demostr);
	CVort_record_demo(current_level);
	rnd = 0;
	CVort_setup_jump_heights(0);
	memset(&input_old, 0, sizeof(input_old));
	CVort_demo_toggle_reset_player_partial_state_before();
	quit_to_title = 0;
	CVort_draw_level(current_level);
	CVort_fade_in();
	CVort_clear_keys();
	CVort_draw_box_opening_main(0x17, 1);
	CVort_draw_string("Save as demo #(0-9): ");
	uint16_t inputChar = CVort_read_char_with_echo() & 0xFF;
	if ((inputChar >= '0') && (inputChar <= '9')) {
		CVort_save_demo(inputChar - '0');
	}
	demo_status = DEMO_OFF;
	CVort_fade_out();
	// HACK to force fade_in to the menu
	scrollX = 0x1000;
	// Load menu level
	CVort_load_level_data(90);
	CVort_demo_toggle_reset_player_partial_state_after();
	return 0;
}

void CVort_demo_toggle_reset_player_partial_state_before() {
	keen_gp.score = 0;
	extra_life_pts = 0;
	/* When a certain sprite is added to the level, like a creature,
	 * it is checked if the player is located somewhere. Problem is
	 * that the player may have not yet been founded...
	 */
	sprites[0].posX = 0;
	sprites[0].posY = 0;
}

void CVort_demo_toggle_reset_player_partial_state_after() {
	resuming_saved_game = 0;
	CVort_load_level_data(90);
	keen_gp.lives = 4;
	if (engine_gameVersion == GAMEVER_KEEN1) {
		keen_gp.ammo = 0;
	} else if (engine_gameVersion == GAMEVER_KEEN2) {
		keen_gp.ammo = 3;
	} else if (engine_gameVersion == GAMEVER_KEEN3) {
		keen_gp.ammo = 5;
	}
	memset(keen_gp.stuff, 0, sizeof (keen_gp.stuff));
	memset(keen_gp.levels, 0, sizeof (keen_gp.levels));
	if (engine_gameVersion == GAMEVER_KEEN2 || engine_gameVersion == GAMEVER_KEEN3) {
		keen_gp.stuff[3] = 1;
	}
	memset(keen_gp.targets, 0, sizeof (keen_gp.targets));
}

void CVort_do_story() {
    CVort_fade_out();
    scrollX = 0x2A000;
    scrollY = 0x2000;
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_fade_in();
    CVort_do_text_viewer(story_text, 0, 0x10);
    CVort_fade_out();
}

void CVort_do_help() {
    int32_t origScrollX = scrollX, origScrollY = scrollY;
    // Move to some other random place...?
    scrollX &= 0xFFFFF000;
    scrollY &= 0xFFFFF000;
    CVort_do_text_viewer(help_text, 1, 0x14);
    scrollX = origScrollX;
    scrollY = origScrollY;
}

void CVort_draw_string_sel(uint16_t type, uint16_t x_pos, uint16_t y_pos, const char *str) {
    uint16_t origCursorX = cursorX, origCursorY = cursorY;
    cursorX = x_pos;
    cursorY = y_pos;
    if (type)
        CVort_draw_string_80(str);
    else
        CVort_draw_string(str);
    cursorX = origCursorX;
    cursorY = origCursorY;
}

void CVort_draw_text_viewer_border() {
    CVort_draw_box2(4, text_viewer_top_pos, 0x2B, text_viewer_bottom);
    CVort_engine_drawChar(4, (text_viewer_bottom + 1) << 3, 4);
    CVort_engine_drawChar(0x2B, (text_viewer_bottom + 1) << 3, 4);
    CVort_engine_drawChar(4, (text_viewer_bottom + 2) << 3, 1);
    CVort_engine_drawChar(0x2B, (text_viewer_bottom + 2) << 3, 3);
    for (uint16_t loopChar = 5; loopChar < 0x2B; loopChar++)
        CVort_engine_drawChar(loopChar, (text_viewer_bottom + 2) << 3, 2);
    CVort_draw_string_sel(1, 5, text_viewer_bottom + 1, "       ESC to Exit /   to Read      ");
    CVort_text_viewer_bkgrd(apogee_bmp_height, text_viewer_top_pos + 1, text_ptr, text_viewer_buffer, text_viewer_height);
}


// This is just used to quickly draw the start of the text scroller
void CVort_do_text_viewer_short(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset) {

    int height =  bottom_line_offset - top_line_offset - 1;
    uint16_t tp = CVort_draw_text_page(text, text_viewer_buffer, 0x26, 0xC8);
    apogee_bmp_height = 5;
    text_viewer_top_pos = top_line_offset;
    text_ptr = text;
    text_viewer_buffer_ptr = text_viewer_buffer;
    text_viewer_height = height;
    text_viewer_bottom = bottom_line_offset;
    draw_func = &CVort_draw_text_viewer_border;
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = NULL;
}

void CVort_do_text_viewer(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset) {
    int16_t height = bottom_line_offset - top_line_offset - 1;
    uint16_t tp = CVort_draw_text_page(text, text_viewer_buffer, 0x26, 0xC8);
    GameInput_T in;
    apogee_bmp_height = 5;
    text_viewer_top_pos = top_line_offset;
    text_ptr = text;
    text_viewer_buffer_ptr = text_viewer_buffer;
    text_viewer_height = height;
    text_viewer_bottom = bottom_line_offset;
    draw_func = &CVort_draw_text_viewer_border;
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = NULL;
    int16_t currRow = 0;
    CVort_engine_delay(8);
    // Scroll
    do {
        in = CVort_handle_ctrl(1);
        if (key_map[0x48] || (in.direction == 0)) // Up
        {
            if (currRow > 0) {
                currRow--;
                CVort_engine_scrollText(top_line_offset + 1, bottom_line_offset - 1, 1);
                CVort_text_viewer_bkgrd(5, top_line_offset + 1, text, (currRow << 1) + text_viewer_buffer, 1);
                //engine_updateActualDisplay();
                CVort_engine_delay(2);
            }
            else // No busy loop, corrupted graphics... or a (timing related) hang!
              CVort_engine_shortSleep();
        } else if (key_map[0x50] || (in.direction == 4)) // Down
        {
            if ((tp - height >= currRow) && (0xC8 - height >= currRow)) {
                currRow++;
                CVort_engine_scrollText(top_line_offset + 1, bottom_line_offset - 1, 0);
                CVort_text_viewer_bkgrd(5, bottom_line_offset - 1, text, (currRow << 1)+(height << 1) + text_viewer_buffer - 2, 1);
                //engine_updateActualDisplay();
                CVort_engine_delay(2);
            }
            else // Same as for "Up": No busy loop and more
                CVort_engine_shortSleep();
        } else { // Avoid busy loop and corrupted graphics from moving windows
            CVort_engine_shortSleep();
            //engine_updateActualDisplay();
        }
        if (key_map[0x49]) // Page Up
        {
            if (currRow - height + 1 <= 0)
                currRow = 0;
            else
                currRow -= height - 1;
            CVort_text_viewer_bkgrd(5, top_line_offset + 1, text, (currRow << 1) + text_viewer_buffer, height);
            do {
                CVort_engine_shortSleep();
                //engine_updateActualDisplay();
            } while (key_map[0x49]);
        }
        if (key_map[0x51]) // Page Down
        {
            if (currRow + (height << 1) >= tp)
                currRow = tp - height + 1;
            else
                currRow += height - 1;
            CVort_text_viewer_bkgrd(5, top_line_offset + 1, text, (currRow << 1) + text_viewer_buffer, height);
            //engine_updateActualDisplay();
            do {
                CVort_engine_shortSleep();
                //engine_updateActualDisplay();
            } while (key_map[0x51]);
        }
    } while (!(key_map[1] || in.but1jump || in.but2pogo));

    CVort_waitForNoGameButtonPress(&in);
    CVort_clear_keys();
}

void CVort_draw_apogee() {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        CVort_engine_drawBitmap(0x10, apogee_bmp_height, CVort1_bmp_apogee);
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        CVort_engine_drawBitmap(0x10, apogee_bmp_height, CVort2_bmp_apogee);
    } else if (engine_gameVersion == GAMEVER_KEEN3) {
        CVort_engine_drawBitmap(0x10, apogee_bmp_height, CVort3_bmp_apogee);
    }
}

void CVort_show_logo_text() {
    CVort_engine_drawBitmap(0x16, apogee_bmp_height - 0xa, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_an : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_an : CVort3_bmp_an ); // An
    CVort_engine_drawBitmap(0x10, apogee_bmp_height, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_apogee : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_apogee : CVort3_bmp_apogee ); // Apogee
    CVort_engine_drawBitmap(0x12, apogee_bmp_height + 0x20, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_present : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_present : CVort3_bmp_present ); // presentation
    CVort_engine_drawBitmap(0x15, 0x63, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_ofan : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_ofan : CVort3_bmp_ofan ); // of an
    CVort_engine_drawBitmap(0x13, 0x72,engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_idsoft : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_idsoft : CVort3_bmp_idsoft ); // ID Software
    CVort_engine_drawBitmap(0x13, 0x9f, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_product : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_product : CVort3_bmp_product ); // production
}

void CVort_scroll_up_logo() {
    scrollX = 0x68000;
    scrollY = 0x2000;
    uint16_t bmpHeight = 200, apogee = 0, currState = 0;
    CVort_engine_clearOverlay();
    CVort_clear_keys();
    intro_complete = 0;
    GameInput_T currInput;
    for (uint16_t timeLeft = 300; timeLeft > 0; timeLeft--) {
        CVort_engine_syncDrawing();
        apogee_bmp_height = bmpHeight;
        CVort_engine_drawScreen();
        switch (currState) {
            case 0: // No logo is shown, yet
                apogee++;
                if (apogee <= 30)
                    break;
                apogee = 0;
                currState++;
                draw_func = &CVort_draw_apogee;
                break;
            case 1: // Logo is raising
                if (bmpHeight <= 55) {
                    currState++;
                    draw_func = &CVort_show_logo_text;
                } else
                    bmpHeight--;
                break;
            default: // Logo is shown with some more text
                break;
        }
        currInput = CVort_handle_ctrl(1);
        if (currInput.but1jump || currInput.but2pogo || CVort_translate_key(1)) {
            intro_complete = 1;
            break;
        }
    }
    draw_func = 0;
}

// draw string on dark background (add 0x80 to each char)

void CVort_draw_string_80(const char *str) {
    while (*str) {
        if (*str == CHAR_LINEFEED) {
            cursorY++;
            cursorX = cursorX_save;
        } else if (*str == CHAR_CARRIAGE)
            cursorX = cursorX_save;
        else {
            CVort_engine_drawChar(cursorX, cursorY << 3, *str + 0x80);
            cursorX++;
        }
        str++;
    }
}

int16_t CVort_check_world_map_col(Sprite_T *sprite) {
    if (god_mode)
        return 0;
    sprite->delX += sprite->velX*sprite_sync;
    sprite->delY += sprite->velY*sprite_sync;
    keen_map[0] = *sprite;
    int16_t blocking = 0;
    int16_t x1_T, x2_T, y1_T, y2_T;

    // First we check the vertical move, if any
    keen_map[0].boxY2 += keen_map[0].delY;
    keen_map[0].boxY1 += keen_map[0].delY;
    x1_T = keen_map[0].boxX1 / 0x1000;
    x2_T = keen_map[0].boxX2 / 0x1000;

    int16_t tempNum, x, y;

    if (keen_map[0].delY > 0) // Down
    {
        if (keen_map[0].boxY2 / 0x1000 != (keen_map[0].boxY2 - keen_map[0].delY) / 0x1000) {
            y2_T = keen_map[0].boxY2 / 0x1000;
            for (x = x1_T; x <= x2_T; x++) {
                if (!TILEINFO_UEdge[map_data_tiles[y2_T * map_width_T + x]]) {
                    if (!(map_data_sprites[y2_T * map_width_T + x] & wmap_col))
                        continue;
                }
                sprite->velY = 0;
                tempNum = (keen_map[0].boxY2 + 1) % 0x1000;
                sprite->delY -= tempNum;
                keen_map[0].boxY1 -= tempNum;
                keen_map[0].boxY2 -= tempNum;
                blocking = 1;
                break;
            }
        }
    } else if (keen_map[0].delY < 0) // Up
    {
        if (keen_map[0].boxY1 / 0x1000 != (keen_map[0].boxY1 - keen_map[0].delY) / 0x1000) {
            y1_T = keen_map[0].boxY1 / 0x1000;
            for (x = x1_T; x <= x2_T; x++) {
                if (!TILEINFO_DEdge[map_data_tiles[y1_T * map_width_T + x]]) {
                    if (!(map_data_sprites[y1_T * map_width_T + x] & wmap_col))
                        continue;
                }
                sprite->velY = 0;
                tempNum = 0x1000 - keen_map[0].boxY1 % 0x1000;
                sprite->delY += tempNum;
                keen_map[0].boxY1 += tempNum;
                keen_map[0].boxY2 += tempNum;
                blocking = 1;
                break;
            }
        }
    }

    // Now horizontal
    keen_map[0].boxX1 += keen_map[0].delX;
    keen_map[0].boxX2 += keen_map[0].delX;
    y1_T = keen_map[0].boxY1 / 0x1000;
    y2_T = keen_map[0].boxY2 / 0x1000;

    if (keen_map[0].delX > 0) // Right
    {
        if (keen_map[0].boxX2 / 0x1000 != (keen_map[0].boxX2 - keen_map[0].delX) / 0x1000) {
            x2_T = keen_map[0].boxX2 / 0x1000;
            for (y = y1_T; y <= y2_T; y++) {
                if (!TILEINFO_LEdge[map_data_tiles[y * map_width_T + x2_T]]) {
                    if (!(map_data_sprites[y * map_width_T + x2_T] & wmap_col))
                        continue;
                }
                sprite->velX = 0;
                tempNum = (keen_map[0].boxX2 + 1) % 0x1000;
                sprite->delX -= tempNum;
                keen_map[0].boxX1 -= tempNum;
                keen_map[0].boxX2 -= tempNum;
                blocking = 1;
                break;
            }
        }
    } else if (keen_map[0].delX < 0) // Left
    {
        if (keen_map[0].boxX1 / 0x1000 != (keen_map[0].boxX1 - keen_map[0].delX) / 0x1000) {
            x1_T = keen_map[0].boxX1 / 0x1000;
            for (y = y1_T; y <= y2_T; y++) {
                if (!TILEINFO_REdge[map_data_tiles[y * map_width_T + x1_T]]) {
                    if (!(map_data_sprites[y * map_width_T + x1_T] & wmap_col))
                        continue;
                }
                sprite->velX = 0;
                tempNum = 0x1000 - keen_map[0].boxX1 % 0x1000;
                sprite->delX += tempNum;
                keen_map[0].boxX1 += tempNum;
                keen_map[0].boxX2 += tempNum;
                blocking |= 1; // THIS is how it's done in vanilla code here...
                break;
            }
        }
    }

    return blocking;
}

void CVort_move_worldmap(GameInput_T input, Sprite_T *spritedraw) {
    int16_t x, y, x1, y1, x2, y2, sprite_x, sprite_y, s, csd;
    uint16_t fr;

    if (engine_gameVersion == GAMEVER_KEEN3)
    {
        if (messie_mounted)
            return;
    }

    CVort_update_sprite_hitbox_wmap(spritedraw);
    if (input.but1jump || input.but2pogo) {
        x1 = spritedraw->boxX1 / 0x1000;
        x2 = spritedraw->boxX2 / 0x1000;
        y1 = spritedraw->boxY1 / 0x1000;
        y2 = spritedraw->boxY2 / 0x1000;
        for (x = x1; x <= x2; x++)
            for (y = y1; y <= y2; y++) {
                if (!map_data_sprites[y * map_width_T + x])
                    continue;
                sprite_x = x;
                sprite_y = y;
                wmap_sprite_on = map_data_sprites[y * map_width_T + x];
                if (wmap_sprite_on == 0xFF)
                    wmap_sprite_on = 0;
            }
    }
    spritedraw->delX = spritedraw->delY = 0;
    switch (input.direction) {
        case 7:
            spritedraw->delX = spritedraw->delY = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenu1;
            break;
        case 0:
            spritedraw->delY = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenu1;
            break;
        case 1:
            spritedraw->delY = -0x400;
            spritedraw->delX = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenu1;
            break;
        case 2:
            spritedraw->delX = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x20;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x20;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenr1;
            break;
        case 3:
            spritedraw->delY = spritedraw->delX = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeend1;
            break;
        case 4:
            spritedraw->delY = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeend1;
            break;
        case 5:
            spritedraw->delY = 0x400;
            spritedraw->delX = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeend1;
            break;
        case 6:
            spritedraw->delX = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x28;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x28;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenl1;
            break;
        default:
            break;
    }
    s = 0;
    if (spritedraw->delX | spritedraw->delY) {
        fr = ((CVort_ptr_engine_getTicks()&0xFFFF) >> 4)&3;
        s++;
    } else
        fr = 0;
    wmap_col = 0x8000;
    if (key_map[0xF] && key_map[0x2A])
        wmap_col = 0;
    csd = CVort_check_world_map_col(spritedraw);
    spritedraw->posX += spritedraw->delX;
    spritedraw->posY += spritedraw->delY;

    if (s && !(((CVort_ptr_engine_getTicks()&0xFFFF) >> 3)&3)) {
        if (csd)
            CVort_engine_setCurSound(2);
        else
            CVort_engine_setCurSound(1);
    }

    // Now we set the scrolling viewport
    //FIXME FIXME FIXME FIXME: Gotos?!!!!!!!
    //FIXME: Several of optimizations can be applied here, apparently.

    int32_t tempDiff;

scroll_x:
    if (spritedraw->delX <= 0)
        goto scroll_x_lo;
    tempDiff = spritedraw->posX - scrollX;
    if ((tempDiff >> 16) < 0)
        goto scroll_x_lo;
    if ((tempDiff >> 16) > 0)
        goto scroll_x_hi_fix;
    if ((tempDiff & 0xFFFF) <= 0xB000)
        goto scroll_x_lo;
scroll_x_hi_fix:
    scrollX += spritedraw->delX;
    if ((scrollX >> 16) < (scrollX_max >> 16))
        goto scroll_y;
    if ((scrollX >> 16) > (scrollX_max >> 16))
        goto scroll_x_hi_fix_limit;
    if ((scrollX & 0xFFFF) <= (scrollX_max & 0xFFFF))
        goto scroll_y;
scroll_x_hi_fix_limit:
    scrollX = scrollX_max;
    goto scroll_y;
scroll_x_lo:
    if (spritedraw->delX >= 0)
        goto scroll_y;
    tempDiff = spritedraw->posX - scrollX;
    if ((tempDiff >> 16) > 0)
        goto scroll_y;
    if ((tempDiff >> 16) < 0)
        goto scroll_x_lo_fix;
    if ((tempDiff & 0xFFFF) >= 0x9000)
        goto scroll_y;
scroll_x_lo_fix:
    scrollX += spritedraw->delX;
    if ((scrollX >> 16) > (scrollX_min >> 16))
        goto scroll_y;
    if ((scrollX >> 16) < (scrollX_min >> 16))
        goto scroll_x_lo_fix_limit;
    if ((scrollX & 0xFFFF) >= (scrollX_min & 0xFFFF))
        goto scroll_y;
scroll_x_lo_fix_limit:
    scrollX = scrollX_min;

scroll_y:
    if (spritedraw->delY <= 0)
        goto scroll_y_lo;
    tempDiff = spritedraw->posY - scrollY;
    if ((tempDiff >> 16) < 0)
        goto scroll_y_lo;
    if ((tempDiff >> 16) > 0)
        goto scroll_y_hi_fix;
    if ((tempDiff & 0xFFFF) <= 0x7000)
        goto scroll_y_lo;
scroll_y_hi_fix:
    scrollY += spritedraw->delY;
    if ((scrollY >> 16) < (scrollY_max >> 16))
        goto scroll_tiles;
    if ((scrollY >> 16) > (scrollY_max >> 16))
        goto scroll_y_hi_fix_limit;
    if ((scrollY & 0xFFFF) <= (scrollY_max & 0xFFFF))
        goto scroll_tiles;
scroll_y_hi_fix_limit:
    scrollY = scrollY_max;
    goto scroll_tiles;
scroll_y_lo:
    if (spritedraw->delY >= 0)
        goto scroll_tiles;
    tempDiff = spritedraw->posY - scrollY;
    if ((tempDiff >> 16) > 0)
        goto scroll_tiles;
    if ((tempDiff >> 16) < 0)
        goto scroll_y_lo_fix;
    if ((tempDiff & 0xFFFF) >= 0x3000)
        goto scroll_tiles;
scroll_y_lo_fix:
    scrollY += spritedraw->delY;
    if ((scrollY >> 16) > (scrollY_min >> 16))
        goto scroll_tiles;
    if ((scrollY >> 16) < (scrollY_min >> 16))
        goto scroll_y_lo_fix_limit;
    if ((scrollY & 0xFFFF) >= (scrollY_min & 0xFFFF))
        goto scroll_tiles;
scroll_y_lo_fix_limit:
    scrollY = scrollY_min;

scroll_tiles:
    scrollX_T = scrollX >> 12;
    scrollY_T = scrollY >> 12;
    CVort_engine_drawSpriteAt(spritedraw->posX, spritedraw->posY, spritedraw->frame + fr);

    if (engine_gameVersion == GAMEVER_KEEN3) {
        uint16_t messieW, messieH;

        // Check for Messie Contact
        if (!messie_mounted)
        {
            if (!messie_time_to_climb)
            {
                messieW = engine_maskedSpriteEntry[CVort3_spr_messield1*4].width;
                messieH = engine_maskedSpriteEntry[CVort3_spr_messield1*4].height;
                if (spritedraw->posX >= messie_xpos && spritedraw->posX <= messie_xpos + (messieW<<3<<8) && spritedraw->posY >= messie_ypos && spritedraw->posY <= messie_ypos + (messieH<<8))
                {
                    CVort_engine_setCurSound(CVort3_snd_crystal);
                    messie_mounted++;
                    messie_time_to_climb = 30;
                }
            }
            else
            {
                messie_time_to_climb--;
            }
        }
    }

    if (!wmap_sprite_on)
        return;

    if (engine_gameVersion == GAMEVER_KEEN1 || engine_gameVersion == GAMEVER_KEEN3) {
        if (CVort_ptr_worldmap_sprites(wmap_sprite_on, spritedraw, sprite_x, sprite_y))
            wmap_sprite_on = 0;
    }

}

void CVort_wait_for_key() {
    GameInput_T input;
    int16_t currCharOffset = 0, isKeyPressed = 0, delayLoopVar;
    do {
        CVort_engine_drawChar(cursorX, cursorY << 3, currCharOffset + 9);
        //engine_updateActualDisplay();
        for (delayLoopVar = 0; delayLoopVar < 6; delayLoopVar++) {
            CVort_engine_delay(1);
            input = CVort_handle_ctrl(1);
            if (input.but1jump || input.but2pogo || CVort_translate_key(1)) {
                isKeyPressed++;
                break;
            }
        }
        currCharOffset++;
        if (currCharOffset > 4)
            currCharOffset = 0;
    } while (!isKeyPressed);

    CVort_waitForNoGameButtonPress(&input);
    CVort_clear_keys();
}

void CVort_body_border_flash(Body_T *border) {
    border->variant += sprite_sync;
    if (border->variant > 300) // Finish the border flashing
    {
        CVort_engine_setBorderColor(3);
        border->type_ = 0;
        return;
    }
    if ((CVort_ptr_engine_getTicks() >> 6)&1)
        CVort_engine_setBorderColor(14);
    else
        CVort_engine_setBorderColor(3);
}

// FIXME? This is not well tested...

void CVort_body_bridge_extend(Body_T *bridge) {
    bridge->variant += sprite_sync;
    if (bridge->variant < 12)
        return;
    bridge->variant -= 12;
    int16_t currActualX = bridge->tile_x + bridge->field_10;
    if (map_data_tiles[bridge->tile_y * map_width_T + currActualX] != bridge->field_E) {
        bridge->think_ptr = &CVort_body_nop;
        return;
    }
    map_data_tiles[bridge->tile_y * map_width_T + currActualX] = 0x10E;
    bridge->field_10 += bridge->field_C;
}

// FIXME? This is not well tested...

void CVort_body_bridge_retract(Body_T *bridge) {
    bridge->variant += sprite_sync;
    if (bridge->variant < 12)
        return;
    bridge->variant -= 12;
    bridge->field_10 -= bridge->field_C;
    int16_t currActualX = bridge->tile_x + bridge->field_10;
    if (map_data_tiles[bridge->tile_y * map_width_T + currActualX] != 0x10E) {
        bridge->type_ = 0;
        return;
    }
    map_data_tiles[bridge->tile_y * map_width_T + currActualX] = bridge->field_E;
}


// FIXME? This is sorta more vanilla like but not really needed...

void CVort_contact_nop(Sprite_T *curr, Sprite_T *other) {
}

void CVort_think_contact_nop(Sprite_T *curr, Sprite_T *other) {

}

// There is no "body_nop" in vanilla keen.
// think_contact_nop was originaly used for bodies, but chocolate-keen crashes
// if we try that...
void CVort_body_nop(Body_T* body) {

}

void CVort_think_dead_sprite() {
    temp_sprite.time++;
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort_think_kill_sprite() {
    temp_sprite.type_ = OBJDEAD;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time > 40) {
        temp_sprite.time -= 40;
        temp_sprite.frame++;
        temp_sprite.varB--;
        if (temp_sprite.varB == 1)
            temp_sprite.think = &CVort_think_dead_sprite;
    }
    temp_sprite.velX = 0;
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort_think_remove_sprite() {
    temp_sprite.type_ = 0;
}

void CVort_default_think() {
    CVort_chg_vid_and_error("Bad think pointer!");
}

void CVort_default_contact(Sprite_T *curr, Sprite_T *other) {
    CVort_chg_vid_and_error("Bad contact pointer!");
}

void CVort_kill_keen() {
    if (god_mode || keen_invincible)
        return;
    sprites[0].think = &CVort_think_keen_death;
    sprites[0].contact = &CVort_think_contact_nop;
    sprites[0].posY += 0x800;
    sprites[0].time = sprites[0].velX = sprites[0].velY = 0;
    sprites[0].frame = 0x16;
    CVort_engine_setCurSound(8);
}

void CVort_kill_keen_temp() {
    temp_sprite.think = &CVort_think_keen_death;
    temp_sprite.contact = &CVort_think_contact_nop;
    temp_sprite.posY += 0x800;
    temp_sprite.time = sprites[0].velX = sprites[0].velY = temp_sprite.velX = 0;
    temp_sprite.frame = 0x16;
    CVort_engine_setCurSound(8);
}



// acceleration: l/r speed acceleration

void CVort_move_left_right(int16_t acceleration) {
    for (uint16_t loopVar = 1; loopVar <= sprite_sync; loopVar++) {
        temp_sprite.velX += acceleration;
        if (temp_sprite.velX > 0x78)
            temp_sprite.velX = 0x78;
        else if (temp_sprite.velX < -0x78)
            temp_sprite.velX = -0x78;
        if (loopVar != sprite_sync)
            temp_sprite.delX += temp_sprite.velX;
    }
}

void CVort_pogo_jump(int16_t max_height, int16_t diff) {
    for (uint16_t loopVar = 1; loopVar <= sprite_sync; loopVar++) {
        temp_sprite.velY += diff;
        if (temp_sprite.velY > max_height)
            temp_sprite.velY = max_height;
        else if (-max_height > temp_sprite.velY)
            temp_sprite.velY = -max_height;
        if (loopVar != sprite_sync)
            temp_sprite.delY += temp_sprite.velY;
    }
}

void CVort_check_ceiling() {
    if (scrollX_min + 8 > temp_sprite.posX) {
        temp_sprite.velX = temp_sprite.delX = 0;
        temp_sprite.posX = scrollX_min + 8;
    } else if (temp_sprite.posX > ceilingX) {
        temp_sprite.velX = temp_sprite.delX = 0;
        temp_sprite.posX = ceilingX;
    }
    if (temp_sprite.posY < scrollY_min) {
        temp_sprite.velY = temp_sprite.delY = 0;
        temp_sprite.posY = scrollY_min;
    } else if (temp_sprite.posY > ceilingY) {
        CVort_engine_setCurSound(0x1B);
        CVort_engine_finishCurSound();
        CVort_kill_keen_temp();
    }
}

void CVort_body_slide_door(Body_T *door) {
    door->variant += sprite_sync;
    int16_t scaledTime = door->variant / 5;
    if (scaledTime > 32) {
        scaledTime = 32;
        door->type_ = 0;
    }
    CVort_engine_drawTileAt(door->tile_x, door->tile_y + (scaledTime << 8), door->field_10);
    CVort_engine_drawTileAt(door->tile_x, door->tile_y + (scaledTime << 8) + 0x1000, door->field_10 + 1);
    CVort_engine_drawTileAt(door->tile_x, door->tile_y + 0x2000, door->field_E);
    CVort_engine_drawTileAt(door->tile_x, door->tile_y + 0x3000, door->field_C);
}

void CVort_open_door(int16_t tileX, int16_t tileY) {
    int16_t doorHeight;
    CVort_engine_setCurSound(0x21);

    // Determine where the door is located (relatively to Keen)
    int16_t tileType0 = TILEINFO_Type[map_data_tiles[tileY * map_width_T + tileX]];
    // NOTE: tileType1 is never actually used, so there is no need for it...
    //int16_t tileType1 = TILEINFO_Type[map_data_tiles[(tileY-1)*map_width_T+tileX]];
    if (tileType0)
        doorHeight = tileY;
    else
        doorHeight = tileY - 1;
    int16_t doorBodyIndex = CVort_add_body();
    bodies[doorBodyIndex].type_ = 1;
    bodies[doorBodyIndex].think_ptr = &CVort_body_slide_door;
    // We don't measure the location by tiles for this body,
    // which is a kind of an exception indeed.
    bodies[doorBodyIndex].tile_x = tileX << 12;
    bodies[doorBodyIndex].tile_y = doorHeight << 12;
    bodies[doorBodyIndex].variant = 0;
    bodies[doorBodyIndex].field_C = map_data_tiles[(doorHeight + 3) * map_width_T + tileX];
    bodies[doorBodyIndex].field_E = map_data_tiles[(doorHeight + 2) * map_width_T + tileX];
    bodies[doorBodyIndex].field_10 = map_data_tiles[doorHeight * map_width_T + tileX];
    // FIXME? Looks like if Keen opens the location from an unusual location
    // (the bottom of it?) then it loses a DIFFERENT item, if any.
    // This is, more or less, a direct port of the original, though.
    keen_gp.stuff[3 + tileType0] = 0;
    // Now clear the door tiles (to be replaced with the body)
    if (engine_gameVersion == GAMEVER_KEEN3) {
        int16_t tileId = map_data_tiles[doorHeight * map_width_T + tileX - 1];
        map_data_tiles[doorHeight * map_width_T + tileX] = tileId;
        map_data_tiles[(doorHeight + 1) * map_width_T + tileX] = tileId;
    } else {
        map_data_tiles[doorHeight * map_width_T + tileX] = 0x8F;
        map_data_tiles[(doorHeight + 1) * map_width_T + tileX] = 0x8F;
    }
}

void CVort_do_fall() {
    for (uint16_t loopVar = 1; loopVar <= sprite_sync; loopVar++) {
        temp_sprite.velY += 3;
        if (temp_sprite.velY > 200)
            temp_sprite.velY = 200;
        else if (temp_sprite.velY < -400)
            temp_sprite.velY = -400;
        if (loopVar != sprite_sync)
            temp_sprite.delY += temp_sprite.velY;
    }
}

int16_t CVort_compute_sprite_delta() {
    temp_sprite.delX += temp_sprite.velX*sprite_sync;
    temp_sprite.delY += temp_sprite.velY*sprite_sync;
    return CVort_check_ground();
}

int16_t CVort_check_ground() {
    int16_t result = 0;
    if (temp_sprite.delX > 0xF00)
        temp_sprite.delX = 0xF00;
    else if (temp_sprite.delX < -0xF00)
        temp_sprite.delX = -0xF00;
    if (temp_sprite.delY > 0xF00)
        temp_sprite.delY = 0xF00;
    else if (temp_sprite.delY < -0xF00)
        temp_sprite.delY = -0xF00;

    int16_t loopVar, tempNum, var_8, var_A;

    temp_sprite.boxY2 += temp_sprite.delY;
    temp_sprite.boxY1 += temp_sprite.delY;
    int16_t var_4 = temp_sprite.boxX1 >> 12;
    int16_t var_6 = temp_sprite.boxX2 >> 12;

    if (temp_sprite.delY > 0) {
        if (((temp_sprite.boxY2 - temp_sprite.delY) >> 12) != (temp_sprite.boxY2 >> 12)) {
            var_A = temp_sprite.boxY2 >> 12;
            for (loopVar = var_4; loopVar <= var_6; loopVar++) {
                if (!TILEINFO_UEdge[map_data_tiles[var_A * map_width_T + loopVar]])
                    continue; // No collision
                temp_sprite.velY = 0;
                tempNum = (temp_sprite.boxY2 + 1) % 0x1000;
                temp_sprite.delY -= tempNum;
                temp_sprite.boxY1 -= tempNum;
                temp_sprite.boxY2 -= tempNum;
                result = 2;
                break;
            }
        }
    } else if (temp_sprite.delY < 0) {
        if (((temp_sprite.boxY1 - temp_sprite.delY) >> 12) != (temp_sprite.boxY1 >> 12)) {
            var_8 = temp_sprite.boxY1 >> 12;
            for (loopVar = var_4; loopVar <= var_6; loopVar++) {
                if (!TILEINFO_DEdge[map_data_tiles[var_8 * map_width_T + loopVar]])
                    continue; // No collision
                temp_sprite.velY = 0;
                tempNum = 0x1000 - temp_sprite.boxY1 % 0x1000;
                temp_sprite.delY += tempNum;
                temp_sprite.boxY1 += tempNum;
                temp_sprite.boxY2 += tempNum;
                result = 8;
                break;
            }
        }
    }

    temp_sprite.boxX1 += temp_sprite.delX;
    temp_sprite.boxX2 += temp_sprite.delX;
    var_8 = temp_sprite.boxY1 >> 12;
    var_A = temp_sprite.boxY2 >> 12;

    if (temp_sprite.delX > 0) {
        if (((temp_sprite.boxX2 - temp_sprite.delX) >> 12) != (temp_sprite.boxX2 >> 12)) {
            var_6 = temp_sprite.boxX2 >> 12;
            for (loopVar = var_8; loopVar <= var_A; loopVar++) {
                if (!TILEINFO_LEdge[map_data_tiles[loopVar * map_width_T + var_6]])
                    continue;
                temp_sprite.velX = 0;
                tempNum = (temp_sprite.boxX2 + 1) % 0x1000;
                temp_sprite.delX -= tempNum;
                temp_sprite.boxX1 -= tempNum;
                temp_sprite.boxX2 -= tempNum;
                result |= 4;
                break;
            }
        }
    } else if (temp_sprite.delX < 0) {
        if (((temp_sprite.boxX1 - temp_sprite.delX) >> 12) != (temp_sprite.boxX1 >> 12)) {
            var_4 = temp_sprite.boxX1 >> 12;
            for (loopVar = var_8; loopVar <= var_A; loopVar++) {
                if (!TILEINFO_REdge[map_data_tiles[loopVar * map_width_T + var_4]])
                    continue;
                temp_sprite.velX = 0;
                tempNum = 0x1000 - temp_sprite.boxX1 % 0x1000;
                temp_sprite.delX += tempNum;
                temp_sprite.boxX1 += tempNum;
                temp_sprite.boxX2 += tempNum;
                result |= 1;
                break;
            }
        }
    }

    return result;
}

// Can't find any errors here...
void CVort_carry_keen(Sprite_T *keen, Sprite_T *carrier)
{
    int16_t delX_dif, delY_dif, boxX_dif, boxY_dif;

    memcpy(&temp_sprite, keen, sizeof(Sprite_T));

    if (carrier->posX < temp_sprite.posX)
    {
        delX_dif = carrier->delX - temp_sprite.delX + 2;
        boxX_dif = (carrier->boxX2&0xFFFF) - (temp_sprite.boxX1&0xFFFF) + 1;
    }
    else
    {
        delX_dif = temp_sprite.delX - carrier->delX + 2;
        boxX_dif = (temp_sprite.boxX2&0xFFFF) - (carrier->boxX1&0xFFFF) + 1;
    }

    if (carrier->posY < temp_sprite.posY)
    {
        delY_dif = carrier->delY - temp_sprite.delY + 2;
        boxY_dif = (carrier->boxY2&0xFFFF) - (temp_sprite.boxY1&0xFFFF) + 1;
    }
    else
    {
        delY_dif = temp_sprite.delY - carrier->delY + 2;
        boxY_dif = (temp_sprite.boxY2&0xFFFF) - (carrier->boxY1&0xFFFF) + 1;
    }

    temp_sprite.delY = temp_sprite.delX = 0;

    if (delY_dif < boxY_dif)
    {
        if (carrier->posX > temp_sprite.posX)
        {
            temp_sprite.delX = -boxX_dif;
            if (carrier->velX < temp_sprite.velX)
            {
                temp_sprite.velX = carrier->velX;
            }
        }
        else
        {
            temp_sprite.delX = boxX_dif;
            if (carrier->velX > temp_sprite.velX)
            {
                temp_sprite.velX = carrier->velX;
            }
        }
    }
    else 
    {
        if (carrier->posY > temp_sprite.posY)
        {
            temp_sprite.delX = carrier->delX;
            temp_sprite.delY = -boxY_dif-0x80;
            if (temp_sprite.think == &CVort_think_keen_jump_air)
            {
                temp_sprite.think = &CVort_think_keen_ground;
            }
            else if (temp_sprite.think == &CVort_think_keen_pogo_air)
            {
                temp_sprite.think = &CVort_think_keen_pogo_ground;
                temp_sprite.time = 0;
                temp_sprite.varB = temp_sprite.velX;
                temp_sprite.velX = 0;
            }

            temp_sprite.varD = 1;
            if ((temp_sprite.velY = carrier->velY) < 0)
            {
                temp_sprite.velY /= 2;
            }
        }
        else
        {
            temp_sprite.delY = boxY_dif;

            // Don't get "lifted" off platform if it's falling quickly
            if (carrier->velY > temp_sprite.velY)
                temp_sprite.velY = carrier->velY;
        }
    }

    CVort_check_ground();
    temp_sprite.posX += temp_sprite.delX;
    temp_sprite.posY += temp_sprite.delY;
    CVort_update_sprite_hitbox();
    //*keen = temp_sprite;
    memcpy(keen, &temp_sprite, sizeof(Sprite_T));
    CVort_do_scrolling();

}

// For Keen 3, used for the Meep pushing Keen.
// For any other episode it is unused with no source modification.
void CVort_push_keen(Sprite_T *keen, Sprite_T *pusher) {
    memcpy(&temp_sprite, keen, sizeof(temp_sprite));

    if ((temp_sprite.boxX2 + temp_sprite.boxX1)/2 < (pusher->boxX2 + pusher->boxX1)/2) {
        temp_sprite.delX = -(temp_sprite.boxX2 - pusher->boxX1 + 1);
        if (temp_sprite.delX > 120)
            temp_sprite.delY = 120;
    } else {
        temp_sprite.delX = pusher->boxX2 - temp_sprite.boxX1 + 1;
        if (temp_sprite.delX < -120)
            temp_sprite.delY = -120;
    }
    CVort_check_ground();
    temp_sprite.posX += temp_sprite.delX;
    temp_sprite.posY += temp_sprite.delY;
    CVort_update_sprite_hitbox();
    //*keen = temp_sprite;
    memcpy(keen, &temp_sprite, sizeof(temp_sprite));
    CVort_do_scrolling();
}

void CVort_do_scrolling() {
    if ((sprites[0].think == &CVort_think_keen_exit) ||
            (sprites[0].think == &CVort_think_keen_death))
        return;
    int32_t sprPosX = sprites[0].posX, sprPosY = sprites[0].posY, tempNum;
    int16_t sprDelX = sprites[0].delX, sprDelY = sprites[0].delY;

    if (sprDelX > 0) // XScrollHi
    {
        tempNum = sprPosX - scrollX;
        if (((tempNum >> 16) > 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) > 0xB000))) {
            scrollX += sprDelX;
            if (scrollX > scrollX_max)
                scrollX = scrollX_max;
        }
    } else if (sprDelX < 0) // XScrollLo
    {
        tempNum = sprPosX - scrollX;
        if (((tempNum >> 16) < 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) < 0x9000))) {
            scrollX += sprDelX;
            if (scrollX < scrollX_min)
                scrollX = scrollX_min;
        }
    }

    if (sprDelY > 0) // YScrollHi
    {
        tempNum = sprPosY - scrollY;
        if (((tempNum >> 16) > 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) > 0x7000))) {
            scrollY += sprDelY;
            if (scrollY > scrollY_max)
                scrollY = scrollY_max;
        }
    } else if (sprDelY < 0) // YScrollLo
    {
        tempNum = sprPosY - scrollY;
        if (((tempNum >> 16) < 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) < 0x3000))) {
            scrollY += sprDelY;
            if (scrollY < scrollY_min)
                scrollY = scrollY_min;
        }
    }
}

// Returns 0 if sprite is to be updated, 1 if not

int16_t CVort_sprite_active_screen() {
    int16_t scaledX = temp_sprite.posX >> 12, scaledY = temp_sprite.posY >> 12;
    if (temp_sprite.posY < 0)
        temp_sprite.posY = 0;
    if ((temp_sprite.posX > map_width) || (temp_sprite.posX < 0) || (temp_sprite.posY > map_height)) {
        temp_sprite.type_ = 0;
        return 1;
    }
    if ((scrollX_T - 8 <= scaledX) && (scrollY_T - 8 <= scaledY) && (scrollX_T + 0x1C >= scaledX) && (scrollY_T + 0x12 >= scaledY))
        return 0;
    if (temp_sprite.type_ >= OBJONEBEFOREKEENSHOT) {
        temp_sprite.type_ = 0;
        return 1;
    }
    temp_sprite.active = 0;
    return 1;
}

// Returns 1 if there is a collision, and 0 otherwise

int16_t CVort_detect_sprite_col(Sprite_T *spr_0, Sprite_T *spr_1) {
    if (!spr_0->boxX1 || !spr_1->boxX1)
        return 0;
    if (spr_0->boxX2 < spr_1->boxX1)
        return 0;
    if (spr_0->boxY2 < spr_1->boxY1)
        return 0;
    if (spr_0->boxX1 > spr_1->boxX2)
        return 0;
    if (spr_0->boxY1 > spr_1->boxY2)
        return 0;
    return 1;
}

// Handles Keen's collision with various kinds of tiles

void CVort_keen_bgtile_col() {
    if (sprites[0].think == &CVort_think_keen_death)
        return;
    keen_switch = 0;
    int16_t tileleft = sprites[0].boxX1 >> 12;
    int16_t tileright = sprites[0].boxX2 >> 12;
    int16_t tiletop = sprites[0].boxY1 >> 12;
    int16_t tilebottom = sprites[0].boxY2 >> 12;
    int16_t currTilePos, currTileType;

    for (int16_t currX = tileleft, currY; currX <= tileright; currX++)
        for (currY = tiletop; currY <= tilebottom; currY++) {
            currTilePos = currY * map_width_T + currX;
            currTileType = TILEINFO_Type[map_data_tiles[currTilePos]];
            if (!currTileType)
                continue;
            switch (currTileType) {
                case 1: // A hazardous tile that kills Keen immediately
                    CVort_kill_keen();
                    break;
                case 2: // A door
                case 3:
                case 4:
                case 5:
                    if (keen_gp.stuff[3 + currTileType])
                        CVort_open_door(currX, currY);
                    else if (sprites[0].delX > 0)
                        sprites[0].posX &= 0xFFFFF000;
                    else
                        sprites[0].posX = (sprites[0].posX + 0x1000)&0xFFFFF000;
                    break;
                case 6: // Some item that add points
                case 7:
                case 8:
                case 9:
                case 10:
                    CVort_add_score(exeFields.points_tbl[currTileType - 6]);
                    CVort_engine_setCurSound(9);
                    if (engine_gameVersion == GAMEVER_KEEN3) {
                        map_data_tiles[currTilePos] = (map_data_tiles[currTilePos] / 0xD) * 0xD;
                    } else {
                    if (map_data_tiles[currTilePos] < 0x131)
                        map_data_tiles[currTilePos] = 0x8F;
                    else
                        map_data_tiles[currTilePos] = 0x114;
                    }
                    break;
		// If episode is Keen 1 ONLY
                case 11: // One of the four BwB parts
                case 12:
                case 13:
                case 14:
                    if (engine_gameVersion != GAMEVER_KEEN1)
                        break;
                    if (currTileType == 11)
                        keen_gp.stuff[0] = 1;
                    if (currTileType == 12)
                        keen_gp.stuff[4] = 1;
                    if (currTileType == 13)
                        keen_gp.stuff[1] = 1;
                    if (currTileType == 14)
                        keen_gp.stuff[2] = 1;
                    CVort_add_score(10000);
                    CVort_engine_setCurSound(0xB);
                    map_data_tiles[currTilePos] = 0x8F;
                    break;
                case 15: // Ammo or...
                case 16: // ...pogo
                    if (currTileType == 15)
                        keen_gp.ammo += 5;
                    if (currTileType == 16)
                        keen_gp.stuff[3] = 1;
                    CVort_engine_setCurSound(0xA);
                    if (engine_gameVersion == GAMEVER_KEEN1) {
                        map_data_tiles[currTilePos] = 0x8F;
                    } else if (engine_gameVersion == GAMEVER_KEEN2) {
                        if (map_data_tiles[currTilePos] < 0x131)
                            map_data_tiles[currTilePos] = 0x8F;
                        else
                            map_data_tiles[currTilePos] = 0x114;
                    } else { // KEEN 3
                        map_data_tiles[currTilePos] = (map_data_tiles[currTilePos] / 0xD) * 0xD;
                    }
                    
                    break;
                case 17: // Exit door
                    if (sprites[0].think != &CVort_think_keen_ground)
                        break;
                    CVort_engine_setCurSound(0xF);
                    sprites[0].think = &CVort_think_keen_exit;
                    sprites[0].contact = &CVort_think_contact_nop;
                    sprites[0].time = currX + 2;
                    sprites[0].varB = currY;
                    break;
                case 18: // A key for some door
                case 19:
                case 20:
                case 21:
                    keen_gp.stuff[currTileType - 13] = 1;
                    CVort_engine_setCurSound(0x20);
                    if (engine_gameVersion == GAMEVER_KEEN3) {
                        map_data_tiles[currTilePos] = (map_data_tiles[currTilePos] / 0xD) * 0xD;
                    } else {
                        map_data_tiles[currTilePos] = 0x8F;
                    }
                    break;
                case 22:
                    // A statue (K1) or elder (K2) that should "tell" a message
                    // or a do-nothing function (K3)
                    CVort_ptr_inlevel_message();
                    if (engine_gameVersion == GAMEVER_KEEN1) {
                        if (current_level == 0xB)
                            map_data_tiles[currTilePos] = 0x1B2;
                        else
                            map_data_tiles[currTilePos] = 0x13B;
                    } else { // Keen 2, 3
                        map_data_tiles[currTilePos] = 0x8F;
                    }
                    break;
                case 23: // A switch
                case 25:
                case 26:
                    keen_switch = 1;
                    keen_tileX = currX;
                    keen_tileY = currY;
                    break;
                case 24: // Secret level teleporter
                    level_finished = LEVEL_END_SECRET;
                    break;
                // NOTE: The following cases should be CHECKED for in Keen 3 only
                case 27: // Ankh
                    if (engine_gameVersion != GAMEVER_KEEN3)
                        break;
                    keen_invincible += 1400;
                    CVort_engine_setCurSound(0x2A);
                    map_data_tiles[currTilePos] = (map_data_tiles[currTilePos] / 0xD) * 0xD;
                    break;
                case 28: // Ammo
                    if (engine_gameVersion != GAMEVER_KEEN3)
                        break;
                    keen_gp.ammo++;
                    map_data_tiles[currTilePos] = (map_data_tiles[currTilePos] / 0xD) * 0xD;
                    CVort_engine_setCurSound(0xA);
                default:
                    break;
            }
        }
}

int16_t CVort_handle_global_keys() {
    int16_t result = 0;
    CVort_translate_key(1);
    // Original debugging code?
    // if (0x82EA == 0) return 0;

    uint32_t saved_tickcount = CVort_ptr_engine_getTicks();

    int16_t filteredKeyScan = key_scane & 0x7F;
    switch (filteredKeyScan) {
        case 0x3D:
            CVort_engine_saveCurSound();
            CVort_clear_keys();
            CVort_handle_redef_keys();
            result++;
            break;
        case 0x3E:
            CVort_engine_saveCurSound();
            CVort_clear_keys();
            CVort_calibrate_joystick(1);
            result++;
            break;
        case 0x3F:
            CVort_engine_saveCurSound();
            CVort_save_game();
            result++;
            break;
        case 1:
            CVort_engine_saveCurSound();
            CVort_handle_quit();
            result++;
            break;
        case 0x3B:
            CVort_engine_saveCurSound();
            CVort_clear_keys();
            CVort_do_help();
            result++;
            break;
        case 0x3C:
            CVort_engine_saveCurSound();
            CVort_clear_keys();
            CVort_draw_box_opening_main(13, 1);
            CVort_draw_string("Sound (Y/N)?");
            switch (CVort_engine_toupper(CVort_read_char_with_echo()) & 0xFF) {
                case 0x4E: // 'N'
                    want_sound = 0;
                    break;
                case 0x59: // 'Y'
                    want_sound = 1;
                    break;
                default:;
            }
            result++;
            break;
        default:
            return 0;
    }
    CVort_ptr_engine_setTicks(saved_tickcount);
    CVort_clear_keys();
    CVort_engine_restoreCurSound();
    CVort_engine_clearOverlay();
    return 1;
}

void CVort_add_score(int16_t points) {
    keen_gp.score += points;
    if (keen_gp.score - extra_life_pts < 20000)
        return;
    CVort_engine_setCurSound(0x1C);
    extra_life_pts = (keen_gp.score / 20000)*20000;
    keen_gp.lives++;
}

void CVort_load_level_data(uint16_t levelnum) {
    current_level = levelnum;
    char filename[13];
    sprintf(filename, "%s%02" PRIu16 ".%s", "LEVEL", levelnum, game_ext);
    FILE *fp = CVort_engine_cross_ro_data_fopen(filename);
    /* TODO? We could apply random map generation if file is not found */
    /* (e.g. if Keen enters a "random" map from the worldmap while a   */
    /* cheat code is in use). However, most chances are that it would  */
    /* look vastly different than the one on vanilla Keen.             */
    if (!fp)
        return;
    uint32_t len = CVort_filelength(fp);
    uint8_t compressedMapData[len];
    fread(compressedMapData, len, 1, fp);
    fclose(fp);

    // Here, code from Commander Genius is used as-is
    CRLE_expandSwapped((word*)map_data, compressedMapData, 0xFEFE);
    // We remove the initial DWORD of uncompressed data size
    memmove(map_data, map_data+1, sizeof(map_data)/sizeof(int16_t)-2);
#if 0
    CRLE RLE;
    std::vector<uint16_t> planeitems;
    RLE.expandSwapped(planeitems, vecCompressedMapData, 0xFEFE);
    // We remove the initial DWORD of uncompressed data size
    std::copy(planeitems.begin() + 1, planeitems.end(), map_data);
#endif
    // FIXME: Are byteswaps required for the Big-Endian architectures?
    // (Apparently not)

    map_data_tiles = map_data + 16;
    map_data_sprites = map_data + 16 + map_data[7] / 2; // map_data[7] == Plane size

    // FIXME: Should we swap map_data[0] and map_data[1] here - and later?
    map_width_T = map_data[0];
    map_height_T = map_data[1];
    map_width_B = map_width_T << 1;
    //screen_wrap = map_width_B-0x2A; // map width in tiles x2 MINUS tiles displayed on screen x2
    screen_wrap_single = map_width_T - 0x15; // map width in tiles MINUS tiles displayed on screen

    scrollX_min = scrollY_min = 0x2000;

    // TODO: Are the shifts done correctly?
    scrollX_max = (map_data[0] - 0x16) << 12;
    map_width = (map_data[0] - 2) << 12;
    map_height = map_data[1] << 12;
    scrollY_max = ((map_data[1] - 0xF) << 12) + 0x800;
    ceilingX = (map_data[0] - 3) << 12;
    ceilingY = map_data[1] << 12;
}

void CVort_handle_quit() {
    CVort_clear_keys();
    if (current_level == 90) {
        CVort_draw_box_opening_main(0xC, 1);
        CVort_draw_string("Quit (Y/N)?");
        if (CVort_engine_toupper(CVort_read_char_with_echo()) != 'Y')
            return;
        CVort_chg_vid_and_error("");
        return; // Actually, that never happens...
    }
    CVort_draw_box_opening_main(0x14, 2);
    CVort_draw_string("Quit to (D)os or\n");
    CVort_draw_string("(T)itle:");
    switch (CVort_engine_toupper(CVort_read_char_with_echo())) // A trick to avoid an extra var...
    {
        case 'D':
            CVort_chg_vid_and_error("");
            break;
        case 'T':
            quit_to_title = 1;
            break;
        default:
            break;
    }
}

/* The way the text is proccessed here appears to be a bit complicated, mainly
when it comes to new lines. Here is what appears to be done:
- Byte no. 0x1A is used as the end-of-text mark.
- When 0x1F is encountered, it is replaced with 0xD.
- If an occurrence of 0xD appears ("carriage return"), though, then it should
be followed by 0xA ("line feed"). Now there are two possibilities:
--- There is a following occurrence of 0xD. Looks like it is assumed that 0xD
is always PAIRED with 0xA. These pairs are simply skipped with no change done.
--- There is no folowing occurrence of 0xD. In that case, the current single
pair of 0xD and 0xA is replaced with a space character (0x20). Pratically, 0xD
is replaced with 0x20 while the rest of the text is SHIFTED by one character
to the left, hence getting the 0xA character removed.

So, to summarize, what is done is as follows:
- 0x1F is replaced with 0xD.
- Multiple successive occurrences of 0xD 0xA are skipped.
- A lone occurrence of 0xD 0xA is replaced with a space.
- 0x1A marks the end of text.                                               */

/* Finally, in Keen1 there is only load_and_process_text_file, and in
 * keen 2/3 there is only CVort_process_text_file, because textfiles are linked
 * in to the exe
 */
void CVort_process_text_file(uint8_t *buffer) {
    // Now process text file.
    // TODO: Is this the correct way?
    uint16_t numOfChars = 0, charCounter = 0;
    while (buffer[numOfChars] != 0x1A)
        numOfChars++;

    while (buffer[charCounter] != 0x1A) {
        if (buffer[charCounter] == 0x1F) {
            buffer[charCounter] = 0xD;
            charCounter++;
        } else if (buffer[charCounter] == 0xD) {
            if (buffer[charCounter + 2] == 0xD)
                do {
                    charCounter += 2;
                } while (buffer[charCounter] == 0xD);
            else {
                buffer[charCounter] = 0x20;
                /* Shift portion of the text by one char.
                TODO? Vanilla code does this but with a total
                of numOfChars-charCounter characters, which
                implies an ERRONEOUS memory access, one byte
                after the ending 0x1A character.           */
                memmove(buffer + charCounter + 1, buffer + charCounter + 2, numOfChars - charCounter - 1);
                numOfChars--;
                charCounter++;
            }
        } else
            charCounter++;
    }
}

void CVort_load_and_process_text_file(const char *filename, uint8_t ** pBuffer) {
    FILE *fp = CVort_engine_cross_ro_data_fopen(filename);
    if (!fp)
        CVort_chg_vid_and_error("Missing a text file!");
    uint32_t len = CVort_filelength(fp);
    *pBuffer = (uint8_t *) malloc(len);
    // If we are out of memory then we are in a problem...
    fread(*pBuffer, len, 1, fp);
    fclose(fp);

    CVort_process_text_file(*pBuffer);
}

uint16_t CVort_draw_text_page(uint8_t *text_src_ptr, int16_t *text_viewer_buffer, int16_t arg_6, int16_t arg_8) {
    int16_t var_2 = 0;
    uint16_t var_4 = 0, dLoopVar = 0, sLoopVar = 0;
    do {
        var_4 = 0;
        text_viewer_buffer[dLoopVar << 1] = var_2;
        for (sLoopVar = var_2; (uint16_t) (var_2 + arg_6) > sLoopVar; sLoopVar++) {
            if (text_src_ptr[sLoopVar] == CHAR_EOF) {
                var_4 = 2;
                text_viewer_buffer[(dLoopVar << 1) + 1] = sLoopVar - var_2;
                text_viewer_buffer[(dLoopVar + 1) << 1] = -1;
                text_viewer_buffer[((dLoopVar + 1) << 1) + 1] = -1;
                break;
            }
            if (text_src_ptr[sLoopVar] == CHAR_CARRIAGE) {
                text_viewer_buffer[(dLoopVar << 1) + 1] = sLoopVar - var_2 + 1;
                dLoopVar++;
                var_2 = sLoopVar + 2;
                var_4++;
                break;
            }
        }
        if (var_4 == 0) {
            for (; sLoopVar > (uint16_t) (var_2); sLoopVar--)
                if (text_src_ptr[sLoopVar] == CHAR_SPACE) {
                    text_viewer_buffer[(dLoopVar << 1) + 1] = sLoopVar - var_2;
                    dLoopVar++;
                    var_2 = sLoopVar + 1;
                    break;
                }
            if (sLoopVar == var_2) {
                text_viewer_buffer[(dLoopVar << 1) + 1] = arg_6;
                dLoopVar++;
                sLoopVar += arg_6;
                var_2 += arg_6;
            }
        }
        if (dLoopVar == arg_8) {
            text_viewer_buffer[(dLoopVar - 1) << 1] = -1;
            text_viewer_buffer[((dLoopVar - 1) << 1) + 1] = -1;
            var_4 = 2;
        }
    } while (var_4 < 2);
    return dLoopVar;
}

void CVort_text_viewer_bkgrd(uint16_t box_x_pos, uint16_t box_y_pos, uint8_t *pointer, int16_t *arg_8, int16_t arg_A) {
    int16_t var_4, var_6;
    uint16_t secondVal, innerLoopVar;
    cursorX = box_x_pos;
    cursorY = box_y_pos;
    for (int16_t var_2 = 0; var_2 < arg_A; var_2++) {
        var_4 = arg_8[var_2 << 1];
        if (var_4 == -1)
            return;
        secondVal = arg_8[(var_2 << 1) + 1];
        if (pointer[var_4 + secondVal - 1] == CHAR_CARRIAGE)
            secondVal--;
        if (pointer[var_4] == 0x7E) {
            var_6 = 0x80;
            var_4++;
            secondVal--;
        } else
            var_6 = 0;
        for (innerLoopVar = 0; innerLoopVar < secondVal; innerLoopVar++) {
            CVort_engine_drawChar(cursorX, cursorY << 3, (pointer[var_4 + innerLoopVar] + var_6)&0xFF);
            cursorX++;
        }
        if (cursorX < 0x2B)
            CVort_draw_char_rectangle(cursorX, cursorY, 0x2A, cursorY, var_6 + 0x20);
        cursorY++;
        cursorX = box_x_pos;
    }
}

void CVort_load_high_scores_table() {
    // FIXME: This is a wrong way to do it, but...... more vanilla!
    static char default_names[][15] = {"Yorpy", "Gargile", "Zzapp!"};
    char path[14];
    strcpy(path, "SCORES.");
    strcat(path, game_ext);

    int entryCounter, partCounter;

    FILE *fp = CVort_engine_cross_rw_misc_fopen(path, "rb");
    if (fp) {
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            CVort_engine_cross_freadInt32LE(high_scores_table.scores + entryCounter, 1, fp);
        for (partCounter = 0; partCounter < 4; partCounter++)
            for (entryCounter = 0; entryCounter < 7; entryCounter++)
                CVort_engine_cross_freadInt16LE(&high_scores_table.parts[entryCounter][partCounter], 1, fp);
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            CVort_engine_cross_freadInt16LE(high_scores_table.targets + entryCounter, 1, fp);
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            CVort_engine_cross_freadInt16LE(high_scores_table.unknown + entryCounter, 1, fp);
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            fread(high_scores_table.names[entryCounter], sizeof (high_scores_table.names[entryCounter]), 1, fp);
        fclose(fp);
    } else {
        for (entryCounter = 0; entryCounter < 7; entryCounter++) {
            high_scores_table.scores[entryCounter] = 100;
            strcpy(high_scores_table.names[entryCounter], default_names[entryCounter % 3]);
        }
        memset(high_scores_table.unknown, 0, sizeof (high_scores_table.unknown));
        memset(high_scores_table.targets, 0, sizeof (high_scores_table.targets));
        memset(high_scores_table.parts, 0, sizeof (high_scores_table.parts));
        CVort_save_high_scores_table();
    }
}

void CVort_save_high_scores_table() {
    // FIXME: This is a wrong way to do it, but...... more vanilla!
    // Or rather, vanilla on the *loading* side...
    char path[14];
    strcpy(path, "SCORES.");
    strcat(path, game_ext);

    FILE *fp = CVort_engine_cross_rw_misc_fopen(path, "wb");
    if (!fp) // TODO: ???
        return;

    int entryCounter, partCounter;
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CVort_engine_cross_fwriteInt32LE(high_scores_table.scores + entryCounter, 1, fp);
    for (partCounter = 0; partCounter < 4; partCounter++)
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            CVort_engine_cross_fwriteInt16LE(&high_scores_table.parts[entryCounter][partCounter], 1, fp);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CVort_engine_cross_fwriteInt16LE(high_scores_table.targets + entryCounter, 1, fp);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CVort_engine_cross_fwriteInt16LE(high_scores_table.unknown + entryCounter, 1, fp);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        fwrite(high_scores_table.names[entryCounter], sizeof (high_scores_table.names[entryCounter]), 1, fp);
    fclose(fp);
}

void CVort_clear_keys() {
    key_code = key_scane = 0;
    memset(key_map, 0, 0x80);
}

// TODO: The original code simply sets rnd to the amount of hundredths of
// seconds in the current time.
// For now we use the seconds elapsed...

void CVort_init_rnd(bool doRand) {
    if (doRand)
        rnd = time(NULL) & 0xFF;
    else
        rnd = 0;
}

int16_t CVort_get_random() {
    rnd++;
    rnd &= 0xFF;
    return (int16_t)(exeFields.rnd_vals[rnd]);
}

uint32_t CVort_filelength(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    uint32_t len = ftell(fp);
    rewind(fp);
    return len;
}

// TODO: This function should not be used, due to endianness issues
// which vary from file to file.
#if 0
// A VARIANT OF alloc_and_map_file. Opens file, allocates memory for it and
// returns a pointer to the memory chunk.
// IMPORTANT: This should be free'd using standard "free" function.

void *CVort_alloc_file(char *filename) {
    FILE *fp = CVort_engine_cross_fopen(filename, "rb");
    if (!fp)
        return NULL;
    uint32_t len = CVort_filelength(fp);
    void *memChunk = malloc(len);
    if (!memChunk) {
        fclose(fp);
        return NULL;
    }
    fread(memChunk, len, 1, fp);
    return memChunk;
}
#endif

#if 0
/* Fibonacci-ish number table used for jump height calculation */
const uint16_t CVort_fibs_17[] = {
    0x1, 0x1, 0x2, 0x3, 0x5, 0x8, 0xD, 0x15, 0x36, 0x4B, 0x81, 0xCC, 0x143, 0x20F, 0x352, 0x561, 0x8B3
};
#endif

// I think "spritejump3" never existed in the first place
// It works better if jump_height_table is moved back by two bytes so that
// spritejump_3 becomes the start of that array

void CVort_setup_jump_heights(uint16_t seed) {
    memcpy(jump_height_table + 1, exeFields.fibs_17, 17 * sizeof (uint16_t));
    jump_height_table[0] = 0;
    spritejump_1 = 0x22;
    spritejump_2 = 0xa;
    if (seed) {
        // TODO the original code does this... it gets current time as:
        // CH = hours, CL = minutes, DH = seconds, DL = 1/100th of sec.
        // Then:
        // jump_height_table[16] = DX
        // jump_height_table[4] = DX xor CX
        // For now, using plain portable time function,
        // a different approach is used here.
        time_t curTime = time(NULL);
        jump_height_table[17] = curTime & 65535;
        jump_height_table[5] = (curTime^(curTime >> 16))&65535;
    }
    CVort_calc_jump_height(0xffff);
}

// TODO: Make this work as expected...

int16_t CVort_calc_jump_height(uint16_t max_height) {
    uint16_t loopVar1 = max_height, loopVar2 = 0xFFFF, result;
    /* The original code flow works like this:
     * - Shift left loopVar1 by one bit.
     * - If there is a carry as a result, abort loop.
     * - Shift right loopVar2 by one bit.
     * - Go back to the beginning of the loop.
     * After the loop, a bit of math is done, using the 'adc' instrument which
     * involves the carry. The carry's value is *always* 1 at this stage.
     */
    while (!(loopVar1 & 0x8000)) {
        loopVar1 <<= 1;
        loopVar2 >>= 1;
    }
    // TODO: What should be done here??? This? We add the last carry anyway.
    result = jump_height_table[(spritejump_1) >> 1] + jump_height_table[(spritejump_2) >> 1] + 1;
    jump_height_table[(spritejump_1) >> 1] = result;
    result += jump_height_table[0];
    jump_height_table[0] = result;
    /*
     *spritejump_1 -= 2;
            if (!(*spritejump_1))
     *spritejump_1 = 0x22;
     */
    if (spritejump_1 == 2)
        spritejump_1 = 0x22;
    else
        spritejump_1 -= 2;
    if (spritejump_2 == 2)
        spritejump_2 = 0x22;
    else
        spritejump_2 -= 2;

    result &= loopVar2;
    if (result > max_height)
        result >>= 1;
    return result;
}

void CVort_chg_vid_and_error(const char *msg) {
    CVort_engine_setVideoMode(3);
    if (!(*msg)) {
        CVort_save_ctrls();
        // NOTE: The 7 offset is the vanilla way.
        CVort_engine_copyToTxtMemory(exeFields.endScreen + 7);
    } else
        CVort_engine_puts(msg);
    CVort_engine_gotoXY(1, 23);
    CVort_engine_handleQuit();
}

uint16_t CVort_draw_level(uint16_t levelnum) {
    int16_t about_to_encounter_mortimer, var_4, var_6, sprite_counter, var_A;

    sprites[0].type_ = 1;
    sprites[0].think = &CVort_think_keen_ground;
    sprites[0].contact = CVort_ptr_contact_keen; // NOT &CVort_ptr_contact_keen
    sprites[0].velX = sprites[0].velY = 0;
    sprites[0].frame = 4;
    sprites[0].active = 1;
    num_sprites = 1;
    num_bodies = 0;
    about_to_encounter_mortimer = engine_arguments.extras.vorticonsDemoModeToggle ? 0 : 1;
    keen_facing = 1;
    if (!engine_arguments.extras.vorticonsDemoModeToggle) {
        sprite_sync = 0;
        CVort_ptr_engine_setTicksSync(0);
        CVort_ptr_engine_setTicks(0);
    }
    level_finished = god_mode = keen_invincible = 0;
    CVort_ptr_init_level(levelnum);
    scrollX = sprites[0].posX + 0xFFFF6000;
    scrollY = sprites[0].posY + 0xFFFFB000;
    // Some scrolling stuff...
    if (scrollX < scrollX_min)
        scrollX = scrollX_min;
    if (scrollY < scrollY_min)
        scrollY = scrollY_min;
    if (scrollX > scrollX_max)
        scrollX = scrollX_max;
    if (scrollY > scrollY_max)
        scrollY = scrollY_max;

    CVort_engine_syncDrawing();
    CVort_fade_out();
    lights = 1;
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    CVort_fade_in();
    scrollX_T = scrollX >> 12;
    scrollY_T = scrollY >> 12;

    // TEST TEST
    uint32_t frame_counter_start_time = SDL_GetTicks();

    if (engine_arguments.extras.vorticonsDemoModeToggle) {
        sprite_sync = 15;
        CVort_ptr_engine_setTicksSync(0);
        CVort_ptr_engine_setTicks(0);
        // FIXME? We use goto... but that just seems too simple...
        goto right_after_drawing_sync;
    }
    do {
        /*********************************************
        Note: combining function pointers with classes
        seems to be the cause of NIGHTMARES!
        NOTE 2: Got back to C since then... (C99)
        *********************************************/
        CVort_engine_syncDrawing();
right_after_drawing_sync:
        input_new = CVort_handle_ctrl(1);
        // Apply sprites behaviors if relevant
        for (sprite_counter = 0; sprite_counter < num_sprites; sprite_counter++) {
            if (!sprites[sprite_counter].type_)
                continue;
            temp_sprite = sprites[sprite_counter];
            if (!temp_sprite.active) {
                var_4 = temp_sprite.posX >> 12;
                var_6 = temp_sprite.posY >> 12;
                if ((scrollX_T - 2 < var_4) && (scrollY_T - 2 < var_6) && (scrollX_T + 0x17 > var_4) && (scrollY_T + 0xC > var_6)) {
                    temp_sprite.active = 1;
#if CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
                    if (engine_gameVersion == GAMEVER_KEEN1) { 
                        if (temp_sprite.think == &CVort1_think_yorp_stunned) {
                            temp_sprite.think = &CVort1_think_yorp_look;
                            temp_sprite.time = 0;
                        }
                    }
#endif
#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
                    else
#endif
#if CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
                      if (engine_gameVersion == GAMEVER_KEEN3) {
                        if (temp_sprite.think == &CVort3_think_foob_run) {
                            temp_sprite.think = &CVort3_think_foob_walk;
                            if (temp_sprite.posX > sprites[0].posX) {
                                temp_sprite.velX = 50;
                            } else {
                                temp_sprite.velX = -50;
                            }
                        }
                    }
#endif
                    CVort_update_sprite_hitbox();
                    temp_sprite.delX = temp_sprite.delY = 0;
                    /* GUESSED(?!) EXPLANATION (while using C++ with classes):
                     * 1. The "think" function can only operate on some object
                     * of class CVorticons, thus the first mention of *this.
                     * 2. It is also stored in temp_sprite, a member of such
                     * a specific object again. Hence, the second mention.
                     * 3. Finally the function should be prefixed with
                     * an asterisk (*).
                     */
                    (temp_sprite.think)();
                    //((*this).*(*this).temp_sprite.think)();
                    temp_sprite.posX += temp_sprite.delX;
                    temp_sprite.posY += temp_sprite.delY;
                    CVort_update_sprite_hitbox();
                }
            } else if (!sprite_counter
            // Platforms always move
            || ((engine_gameVersion == GAMEVER_KEEN2) && (temp_sprite.type_ == CVort2_obj_platform))
            || ((engine_gameVersion == GAMEVER_KEEN3) && (temp_sprite.type_ == 10))
            || !CVort_sprite_active_screen() ) {
                CVort_update_sprite_hitbox();
                temp_sprite.delX = temp_sprite.delY = 0;
                (temp_sprite.think)();
                //((*this).*(*this).temp_sprite.think)();
                temp_sprite.posX += temp_sprite.delX;
                temp_sprite.posY += temp_sprite.delY;
                CVort_update_sprite_hitbox();
            }
            sprites[sprite_counter] = temp_sprite;
        }
        CVort_do_scrolling();
        // Seems like collision code
        for (sprite_counter = 0; sprite_counter < num_sprites; sprite_counter++) {
            if (!sprites[sprite_counter].type_ || !sprites[sprite_counter].active)
                continue;
            for (var_A = sprite_counter + 1; var_A < num_sprites; var_A++) {
                if (!sprites[var_A].type_ || !sprites[var_A].active)
                    continue;
                if (!CVort_detect_sprite_col(&sprites[sprite_counter], &sprites[var_A]))
                    continue;
                //*this.*(sprites[sprite_counter].contact)(&sprites[sprite_counter], &sprites[var_A]);
                //*this.*(sprites[var_A].contact)(&sprites[var_A], &sprites[sprite_counter]);
                (sprites[sprite_counter].contact)(&sprites[sprite_counter], &sprites[var_A]);
                (sprites[var_A].contact)(&sprites[var_A], &sprites[sprite_counter]);
                //((*this).*(*this).sprites[sprite_counter].contact)(&sprites[sprite_counter], &sprites[var_A]);
                //((*this).*(*this).sprites[var_A].contact)(&sprites[var_A], &sprites[sprite_counter]);
            }
        }
        scrollX_T = scrollX >> 12;
        scrollY_T = scrollY >> 12;
        // Remember to draw sprites; Here, we scan in REVERSED order.
        for (sprite_counter = num_sprites - 1; sprite_counter >= 0; sprite_counter--)
            if (sprites[sprite_counter].type_ && sprites[sprite_counter].active)
                CVort_engine_drawSpriteAt(sprites[sprite_counter].posX, sprites[sprite_counter].posY, sprites[sprite_counter].frame);

        if (engine_gameVersion == GAMEVER_KEEN3) {
            if (keen_invincible || god_mode) {
                if (keen_invincible > 250) {
                    CVort_engine_drawSpriteAt(sprites[0].posX - 0x800, sprites[0].posY - 0x800, ((CVort_ptr_engine_getTicks() >> 4) & 1) + 0x3D);
                } else if ((CVort_ptr_engine_getTicks() >> 4) & 1) {
                    CVort_engine_drawSpriteAt(sprites[0].posX - 0x800, sprites[0].posY - 0x800, ((CVort_ptr_engine_getTicks() >> 5) & 1) + 0x3D);
                }
                keen_invincible -= sprite_sync;
                if (keen_invincible < 0) {
                    keen_invincible = 0;
                }
            }
        }
        CVort_keen_bgtile_col();
        // Now call body "think" functions
        for (sprite_counter = 0; sprite_counter < num_bodies; sprite_counter++)
            if (bodies[sprite_counter].type_)
                (bodies[sprite_counter].think_ptr)(&bodies[sprite_counter]);
                //((*this).*(*this).bodies[sprite_counter].think_ptr)(&bodies[sprite_counter]);
        // Alright, let's update what is to be seen!
        CVort_engine_drawScreen();
        input_old = input_new;
#if CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
        if (engine_gameVersion == GAMEVER_KEEN3) {
            if (about_to_encounter_mortimer && (levelnum == 16)) {
                CVort3_handle_grand_intellect();
                //(static_cast<CVorticons3 *>(this))->handle_grand_intellect();
                about_to_encounter_mortimer = 0;
            }
        }
#endif
        CVort_handle_cheat_keys();
        if (CVort_handle_global_keys())
            CVort_ptr_engine_setTicksSync(CVort_ptr_engine_getTicks());
        if (quit_to_title)
            return 0;
    } while ((level_finished == LEVEL_END_DIE) && (sprites[0].type_));

    // TODO: comment this out if sound running, otherwise the below will never finish
    CVort_engine_finishCurSound();
    CVort_fade_out();
    for (var_4 = 0; var_4 < 4; var_4++)
        keen_gp.stuff[var_4 + 5] = 0; // Set unused "stuff" vals to 0
    if (level_finished != LEVEL_END_DIE)
        return level_finished;

    if (engine_gameVersion == GAMEVER_KEEN1) {
        // Since Keen has just died, If a new item for the BWB has been
        // collected, we should basically FORGET of that item for now.
        switch (levelnum) {
            case 8:
                keen_gp.stuff[1] = 0;
                break;
            case 16:
                keen_gp.stuff[2] = 0;
                break;
            case 3:
                keen_gp.stuff[4] = 0;
                break;
            case 4:
                keen_gp.stuff[0] = 0;
                break;
            default:
                break;
        }
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        // Unrescue a city if it was rescued
        switch (levelnum) {
            case 4:
                keen_gp.targets[0] = 0;
                break;
            case 6:
                keen_gp.targets[1] = 0;
                break;
            case 7:
                keen_gp.targets[2] = 0;
                break;
            case 9:
                keen_gp.targets[3] = 0;
                break;
            case 11:
                keen_gp.targets[4] = 0;
                break;
            case 13:
                keen_gp.targets[5] = 0;
                break;
            case 15:
                keen_gp.targets[6] = 0;
                break;
            case 16:
                keen_gp.targets[7] = 0;
                break;
         }
    }
    return level_finished;
}

void CVort_calibrate_joystick(int16_t joy_id) {
    CVort_draw_box_opening_main(28, 9);
    int16_t var_2 = 0, var_4 = 0;
    JoystickPoll_T currPollInput = CVort_engine_pollJoystick(1);
    if (currPollInput.xPoll < 500)
        var_2 = 1;
    currPollInput = CVort_engine_pollJoystick(2);
    if (currPollInput.yPoll < 500)
        var_2 = 2;
    CVort_draw_string("  Joystick Configuration\n\r");
    CVort_draw_string("  ----------------------\n\r");
    CVort_draw_string("Hold the joystick in the\n\r");
    CVort_draw_string("upper left\n\r");
    CVort_draw_string("corner and press button 1:");

    int16_t currOnScreenArrowChar = 9;
    JoystickPoll_T topLeftPollInput, bottomRightPollInput;
    GameInput_T currInput;
    do {
        CVort_engine_drawChar(cursorX, cursorY << 3, currOnScreenArrowChar);
        //engine_updateActualDisplay();
        CVort_engine_delay(3);
        currOnScreenArrowChar++;
        if (currOnScreenArrowChar == 12)
            currOnScreenArrowChar = 9;
        topLeftPollInput = CVort_engine_pollJoystick(joy_id);
        currInput = CVort_engine_getJoystickCtrl(joy_id);
        //CVort_engine_updateInputStatus();
        if (key_map[1]) // ESC pressed
        {
            CVort_clear_keys();
            return;
        }
    } while (currInput.but1jump != 1);
    CVort_engine_drawChar(cursorX, cursorY << 3, 0x20);
    do {
        currInput = CVort_engine_getJoystickCtrl(joy_id);
        //engine_updateActualDisplay();
        CVort_engine_shortSleep();
    } while (currInput.but1jump != 0);
    CVort_engine_delay(2);
    CVort_draw_string("\n\n\rHold the joystick in the\n\r");
    CVort_draw_string("lower right\n\r");
    CVort_draw_string("corner and press button 1:");

    do {
        CVort_engine_drawChar(cursorX, cursorY << 3, currOnScreenArrowChar);
        //engine_updateActualDisplay();
        CVort_engine_delay(3);
        currOnScreenArrowChar++;
        if (currOnScreenArrowChar == 12)
            currOnScreenArrowChar = 9;
        bottomRightPollInput = CVort_engine_pollJoystick(joy_id);
        currInput = CVort_engine_getJoystickCtrl(joy_id);
        CVort_engine_updateInputStatus();
        if (key_map[1]) // ESC pressed
        {
            CVort_clear_keys();
            return;
        }
    } while (currInput.but1jump != 1);
    CVort_engine_drawChar(cursorX, cursorY << 3, 0x20);
    do {
        currInput = CVort_engine_getJoystickCtrl(joy_id);
        //engine_updateActualDisplay();
        CVort_engine_shortSleep();
    } while (currInput.but1jump != 0);

    int16_t var_A = (bottomRightPollInput.xPoll - topLeftPollInput.xPoll) / 4;
    int16_t var_C = (bottomRightPollInput.yPoll - topLeftPollInput.yPoll) / 4;
    joystick_ctrl[0][joy_id] = topLeftPollInput.xPoll + var_A;
    joystick_ctrl[1][joy_id] = bottomRightPollInput.xPoll - var_A;
    joystick_ctrl[2][joy_id] = topLeftPollInput.yPoll + var_C;
    joystick_ctrl[3][joy_id] = bottomRightPollInput.yPoll - var_C;
    ctrl_type[1] = 2;
    CVort_clear_keys();
}

void CVort_draw_keyname(int16_t key) {
    key &= 0x7F;
    if (key == 1) {
        CVort_draw_string("ESC");
        return;
    }
    if (key == 0xE) {
        CVort_draw_string("BKSP");
        return;
    }
    if (key == 0xF) {
        CVort_draw_string("TAB");
        return;
    }
    if (key == 0x1D) {
        CVort_draw_string("CTRL");
        return;
    }
    if (key == 0x2A) {
        CVort_draw_string("LSHIFT");
        return;
    }
    if (key == 0x39) {
        CVort_draw_string("SPACE");
        return;
    }
    if (key == 0x3A) {
        CVort_draw_string("CAPSLK");
        return;
    }
    if ((key >= 0x3B) && (key <= 0x44)) {
        CVort_draw_string("F");
        char fKeyNumAsStr[3];
        sprintf(fKeyNumAsStr, "%" PRId16, key - 0x3A);
        CVort_draw_string(fKeyNumAsStr);
        return;
    }
    if (key == 0x57) {
        CVort_draw_string("F11");
        return;
    }
    // Should really be 0x58, but vanilla Keen 1 checks for 0x59
    // and the result is a question mark displayed...
    if (key == 0x59) {
        CVort_draw_string("F12");
        return;
    }
    if (key == 0x46) {
        CVort_draw_string("SCRLLK");
        return;
    }
    if (key == 0x1C) {
        CVort_draw_string("ENTER");
        return;
    }
    if (key == 0x36) {
        CVort_draw_string("RSHIFT");
        return;
    }
    if (key == 0x37) {
        CVort_draw_string("PRTSC");
        return;
    }
    if (key == 0x38) {
        CVort_draw_string("ALT");
        return;
    }
    if (key == 0x47) {
        CVort_draw_string("HOME");
        return;
    }
    if (key == 0x49) {
        CVort_draw_string("PGUP");
        return;
    }
    if (key == 0x4F) {
        CVort_draw_string("END");
        return;
    }
    if (key == 0x51) {
        CVort_draw_string("PGDN");
        return;
    }
    if (key == 0x52) {
        CVort_draw_string("INS");
        return;
    }
    if (key == 0x53) {
        CVort_draw_string("DEL");
        return;
    }
    if (key == 0x45) {
        CVort_draw_string("NUMLK");
        return;
    }
    CVort_engine_drawChar(cursorX, cursorY << 3, exeFields.char_map[key]);
    cursorX++;
}

void CVort_handle_redef_keys() {
    int8_t lastAnswer;
    int16_t shiftedAnswer, checkedKey;
    CVort_draw_box_opening_main(22, 14);
    CVort_draw_string("Keyboard Commands\n");
    CVort_draw_string("-----------------");
    CVort_draw_string("\n0 north    :");
    CVort_draw_string("\n1 northeast:");
    CVort_draw_string("\n2 east     :");
    CVort_draw_string("\n3 southeast:");
    CVort_draw_string("\n4 south    :");
    CVort_draw_string("\n5 southwest:");
    CVort_draw_string("\n6 west     :");
    CVort_draw_string("\n7 northwest:");
    CVort_draw_string("\n8 button1  :");
    CVort_draw_string("\n9 button2  :");
    CVort_draw_string("\nModify which action:");
    int16_t box_x_pos = cursorX, box_y_pos = cursorY;
    for (int16_t counter = 0; counter < 8; counter++) {
        cursorX = 0x1A;
        cursorY = counter + 7;
        CVort_draw_keyname(sc_dir[counter]);
    }
    cursorX = 0x1A;
    cursorY = 0xF;
    CVort_draw_keyname(sc_but1);
    cursorX = 0x1A;
    cursorY = 0x10;
    CVort_draw_keyname(sc_but2);
    //engine_updateActualDisplay();
    do {
        cursorX = box_x_pos;
        cursorY = box_y_pos;
        lastAnswer = CVort_read_char_with_echo() % 0x100;
        // Is the answer in the range '0'..'9'?
        if ((lastAnswer >= 0x30) && (lastAnswer <= 0x39)) {
            shiftedAnswer = (int16_t) (lastAnswer) - 0x30;
            CVort_engine_drawChar(cursorX, cursorY << 3, lastAnswer);
            // TODO: Vanilla code does it again!! Do it here, too?
            //shiftedAnswer = lastAnswer-0x30;
            CVort_draw_string("\n\rPress the new key:");
            //engine_updateActualDisplay();
            CVort_clear_keys();
            checkedKey = -1;
            do {
                checkedKey++;
                if (key_map[checkedKey])
                    break;
                if (checkedKey == 0x79) {
                    checkedKey = -1;
                    // If we sleep on EVERY loop iteration, it can take
                    // too long before we reach a key currently being held!
                    CVort_engine_shortSleep();
                }
                else if (checkedKey == 0x29)
                    checkedKey++;
            } while (1);
            CVort_clear_keys();
            CVort_draw_string("\r                  ");
            if (shiftedAnswer < 8)
                sc_dir[shiftedAnswer] = checkedKey;
            if (shiftedAnswer == 8)
                sc_but1 = checkedKey;
            if (shiftedAnswer == 9)
                sc_but2 = checkedKey;

            cursorY = shiftedAnswer + 7;
            cursorX = 0x1A;
            CVort_draw_string("        ");
            cursorX = 0x1A;
            CVort_draw_keyname(checkedKey);

            lastAnswer = 0x30;
        }
    } while ((lastAnswer >= 0x30) && (lastAnswer <= 0x39));
    ctrl_type[1] = 0;
}

void CVort_init_ctrls() {
    strcpy(string_buf, "CTLPANEL.");
    strcat(string_buf, game_ext);
    FILE *fp = CVort_engine_cross_rw_misc_fopen(string_buf, "rb");
    if (fp) {
        CVort_engine_cross_freadInt16LE(&want_sound, 1, fp);
        CVort_engine_cross_freadInt16LE(ctrl_type, 3, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[0], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[0] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[0] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[2], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[2] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[2] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[1], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[1] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[1] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[3], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[3] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[3] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(&mouse_ctrl_1, 1, fp);
        CVort_engine_cross_freadInt8LE(sc_dir, 8, fp);
        CVort_engine_cross_freadInt8LE(&sc_but1, 1, fp);
        CVort_engine_cross_freadInt8LE(&sc_but2, 1, fp);
        fclose(fp);
    } else {
        want_sound = 1;
        ctrl_type[1] = 0;
        ctrl_type[2] = 2;
        joystick_ctrl[0][1] = joystick_ctrl[0][2] = joystick_ctrl[2][1] = joystick_ctrl[2][2] = 20;
        joystick_ctrl[1][1] = joystick_ctrl[1][2] = joystick_ctrl[3][1] = joystick_ctrl[3][2] = 60;
        mouse_ctrl_1 = 5;

        sc_dir[0] = 0x48;
        sc_dir[1] = 0x49;
        sc_dir[2] = 0x4D;
        sc_dir[3] = 0x51;
        sc_dir[4] = 0x50;
        sc_dir[5] = 0x4F;
        sc_dir[6] = 0x4B;
        sc_dir[7] = 0x47;
        sc_but1 = 0x1D;
        sc_but2 = 0x38;
    }
}

void CVort_save_ctrls() {
    strcpy(string_buf, "CTLPANEL.");
    strcat(string_buf, game_ext);
    FILE *fp = CVort_engine_cross_rw_misc_fopen(string_buf, "wb");
    if (!fp)
        return;
    CVort_engine_cross_fwriteInt16LE(&want_sound, 1, fp);
    CVort_engine_cross_fwriteInt16LE(ctrl_type, 3, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[0], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[0] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[0] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[2], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[2] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[2] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[1], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[1] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[1] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[3], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[3] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[3] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(&mouse_ctrl_1, 1, fp);
    CVort_engine_cross_fwriteInt8LE(sc_dir, 8, fp);
    CVort_engine_cross_fwriteInt8LE(&sc_but1, 1, fp);
    CVort_engine_cross_fwriteInt8LE(&sc_but2, 1, fp);
    fclose(fp);
}

GameInput_T CVort_handle_ctrl(uint16_t input_type) {
    GameInput_T input_ret;
    //CVort_engine_updateInputStatus();
    if ((demo_status == DEMO_OFF) || (demo_status == DEMO_RECORD)) {
        switch (ctrl_type[input_type]) {
            case CONTROL_KEYB:
                input_ret = CVort_engine_getKeybCtrlState();
                break;
            case CONTROL_MOUSE:
                input_ret = CVort_engine_getMouseCtrl();
                break;
            case CONTROL_JOY_1:
                input_ret = CVort_engine_getJoystickCtrl(1);
                break;
            case CONTROL_JOY_2:
                input_ret = CVort_engine_getJoystickCtrl(2);
                break;
            default:
                break;
        }
        if (demo_status == DEMO_RECORD) {
            if (engine_arguments.extras.vorticonsDemoModeToggle && (demo_action_ptr == demo_after_last_byte_char_offset)) {
                CVort_chg_vid_and_error("Demo buffer overflow");
            }
            *demo_action_ptr = (input_ret.direction << 2) | (input_ret.but2pogo << 1) | input_ret.but1jump;
            demo_action_ptr++;
        }
    } else {
        if (engine_arguments.extras.vorticonsDemoModeToggle && (demo_action_ptr == end_of_demo_ptr)) {
            quit_to_title = 2; // SPECIAL VALUE (HALT BY DEMO, *NOT* PLAYER)

            // FIXME: On a second thought that's malformed
            // behavior (what is input_ret??)... Oh well
            return input_ret;
        }
        input_ret.but1jump = (*demo_action_ptr) & 1;
        input_ret.but2pogo = ((*demo_action_ptr) & 2) >> 1;
        input_ret.direction = ((*demo_action_ptr) & 60) >> 2;
        demo_action_ptr++;
    }
    return input_ret;
}

uint16_t CVort_translate_key(uint16_t type) {
    //CVort_engine_updateInputStatus();
    if (type) {
        if (!(key_scane & 0x80))
            return 0;
        uint8_t returnHigh = key_scane & 0x7f;
        key_code = exeFields.trans_map[returnHigh];
        if (key_code)
            return (key_code | (returnHigh << 8));
        return 0;
    }
    do {
        if (key_scane & 0x80) // Should be ((int8_t)key_scane < 0) or so?
        {
            key_scane &= 0x7f;
            if (exeFields.trans_map[key_scane]) {
                key_code = exeFields.trans_map[key_scane];
                return (key_code | (key_scane << 8));
            }
        }
        // Rather than a busy loop, we wait for a new event here.
        //SDL_WaitEvent(NULL);
        /* ...or wait, this is *a bit* less true to the vanilla behavior
        in terms of continuous "Space" key presses while Pause menu is
        displayed. Perfect emulation may be harder and, in fact, even on
        on real DOS it may depend on currently set keyboard behavior,
        in terms of "CVort_engine_delay" and "repeat".
        Anyway... just do a short sleep for now.                      */

       // Without this, moving windows around can result in loss of
       // on-screen contents. It may further be a good chance to
       // update these, if there is anything to update.
       //engine_updateActualDisplay();
       CVort_engine_shortSleep();

    } while (1);
}

void CVort_draw_box2(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height) {
    textbox_x_pos = xpos;
    textbox_y_pos = ypos;
    textbox_width = width;
    textbox_height = height;
    CVort_engine_drawChar(xpos, ypos << 3, 1);
    uint16_t currX, currY;
    for (currX = xpos + 1; currX < width; currX++)
        CVort_engine_drawChar(currX, ypos << 3, 2);
    CVort_engine_drawChar(width, ypos << 3, 3);
    for (currY = ypos + 1; currY < height; currY++) {
        CVort_engine_drawChar(xpos, currY << 3, 4);
        for (currX = xpos + 1; currX < width; currX++)
            CVort_engine_drawChar(currX, currY << 3, 0x20);
        CVort_engine_drawChar(width, currY << 3, 5);
    }
    CVort_engine_drawChar(xpos, height << 3, 6);
    for (currX = xpos + 1; currX < width; currX++)
        CVort_engine_drawChar(currX, height << 3, 7);
    CVort_engine_drawChar(width, height << 3, 8);

    cursorX = cursorX_save = xpos + 1;
    cursorY = ypos + 1;
}

void CVort_draw_box(uint16_t width, uint16_t height) {
    uint16_t x, y;
    x = textbox_offs_x - width / 2;
    y = textbox_offs_y - height / 2;
    CVort_draw_box2(x, y, x + width + 1, y + height + 1);
}

// Draw a rectangle of the same char

void CVort_draw_char_rectangle(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t character) {
    for (uint16_t currY = y_pos, currX; currY <= height; currY++)
        for (currX = x_pos; currX <= width; currX++)
            CVort_engine_drawChar(currX, currY << 3, character);
}

//void CVort_draw_box_opening2(uint16_t width, uint16_t height)

void CVort_draw_box_opening_main(uint16_t width, uint16_t height) {
    if (width <= 2) {
        if (height > 2)
            CVort_draw_box_opening_vert(width, height - 2);
    } else if (height <= 2)
        CVort_draw_box_opening_horiz(width - 2, height);
    else
        CVort_draw_box_opening_main(width - 2, height - 2);
    CVort_engine_delay(1);
    CVort_draw_box(width, height);
    //engine_updateActualDisplay();
}

//void CVort_draw_box_opening(uint16_t width, uint16_t height)

void CVort_draw_box_opening_horiz(uint16_t width, uint16_t height) {
    if (width > 2)
        CVort_draw_box_opening_horiz(width - 2, height);
    CVort_engine_delay(1);
    CVort_draw_box(width, height);
    //engine_updateActualDisplay();
}

//void CVort_sub_1614B(uint16_t width, uint16_t height)

void CVort_draw_box_opening_vert(uint16_t width, uint16_t height) {
    if (height > 2)
        CVort_draw_box_opening_vert(width, height - 2);
    CVort_engine_delay(1);
    CVort_draw_box(width, height);
    //engine_updateActualDisplay();
}

uint16_t CVort_read_char_with_echo() {
    uint16_t currOnScreenCh = 9, currCh;
    do {
        currCh = CVort_translate_key(1)&0xFF;
        if (!currCh && (currOnScreenCh < 0xD)) {
            CVort_engine_drawChar(cursorX, cursorY << 3, currOnScreenCh);
            currOnScreenCh++;
            //engine_updateActualDisplay();
            CVort_engine_delay(5);
            continue;
        }
        if (currCh)
            break;
        currOnScreenCh = 9;
    } while (1);
    CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
    return CVort_translate_key(0);
}

void CVort_draw_string(const char *str) {
    while (*str) {
        if (*str == CHAR_LINEFEED) {
            cursorY++;
            cursorX = cursorX_save;
        } else if (*str == CHAR_CARRIAGE)
            cursorX = cursorX_save;
        else {
            CVort_engine_drawChar(cursorX, cursorY << 3, *str);
            cursorX++;
        }
        str++;
    }
}

void CVort_draw_number_word(uint16_t value) {
    sprintf(string_buf, "%" PRIu16, value);
    CVort_draw_string(string_buf);
}

void CVort_draw_number_dword(uint32_t value) {
    sprintf(string_buf, "%" PRIu32, value);
    CVort_draw_string(string_buf);
}

#if 0

void CVort_draw_char_as_hex(UInt8 ch) {
    char hexStr[2];
    hexStr[1] = '\0';
    hexStr[0] = hex_digits[ch >> 4];
    CVort_draw_string(hexStr);
    hexStr[0] = hex_digits[ch & 0xF];
    CVort_draw_string(hexStr);
}

void CVort_draw_word_as_hex(uint16_t value) {
    CVort_draw_string("$");
    draw_char_as_hex(value >> 8);
    draw_char_as_hex(value & 0xFF);
}

void CVort_draw_word_as_bin(uint16_t value) {
    CVort_draw_string("%");
    for (uint16_t currBit = 0; currBit < 0x10; currBit++)
        CVort_draw_string((value >> (0xF - currBit)) ? "1" : "0");
    draw_char_as_hex(value >> 8);
    draw_char_as_hex(value & 0xFF);
}
#endif

void CVort_draw_stringz(const char *str) {
    cursorX = textbox_offs_x + 1 - (strlen(str) >> 1);
    CVort_draw_string(str);
}

/* get_numeric_input is a lot of work and, apparently, */
/* is never ever used... So give up for now.           */
#if 0

uint16_t get_numeric_input() {
}
#endif

uint16_t CVort_get_string_input(char *strBuffer, uint16_t len) {
    char lastCharRead;
    uint16_t currChIndex = 0;
    do {
        lastCharRead = CVort_engine_toupper(CVort_read_char_with_echo()&0xFF);
        // A key like Backspace or Delete pressed?
        if (((lastCharRead == 0x7F) || (lastCharRead == 8)) && (currChIndex > 0)) {
            currChIndex--;
            CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
            cursorX--;
        }// A standard character (out of most of the ASCII chars)?
        else if ((lastCharRead >= 0x20) && (lastCharRead <= 0x7A) && (currChIndex < len)) {
            strBuffer[currChIndex] = lastCharRead;
            currChIndex++;
            CVort_engine_drawChar(cursorX, cursorY << 3, lastCharRead);
            cursorX++;
        }
    } while ((lastCharRead != 0x1B) && (lastCharRead != 0xD));
    // Set the rest of the characters to NULL (this is vanilla behavior).
    for (; currChIndex < len; currChIndex++)
        strBuffer[currChIndex] = '\0';
    if (lastCharRead == 0xD) // Carriage return
        return 1;
    return 0; // Escape
}

void CVort_fade_in() {
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[0]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[1]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
}

void CVort_fade_out() {
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[2]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[1]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[0]);
}

// FACT: Out of all unmodified games, these are actually used in Keen 2 only.
// Custom levels for vanilla Keen1 with light switches are possible, though.

void CVort_lights_on() {
    lights = 1;
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
}

void CVort_lights_out() {
    lights = 0;
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[1]);
}

void CVort_toggle_switch() {
    int16_t loopVar;
    CVort_engine_setCurSound(0x19);
    // Looks like a tantalus ray has been activated... oops!
    if (map_data_tiles[(keen_tileY + 5) * map_width_T + keen_tileX + 3] == 0x1DF) {
        int32_t origScrollX = scrollX, origScrollY = scrollY;
        for (loopVar = 1; loopVar < 80; loopVar++) {
            CVort_engine_syncDrawing();
            scrollX = origScrollX - ((CVort_calc_jump_height(0x40) - 0x20) << 8);
            scrollY = origScrollY - ((CVort_calc_jump_height(0x40) - 0x20) << 8);
            CVort_engine_drawSpriteAt(temp_sprite.posX, temp_sprite.posY, temp_sprite.frame);
            CVort_engine_drawScreen();
        }
        CVort_draw_box_opening_main(5, 1);
        CVort_draw_string("Oops.");
        //engine_updateActualDisplay();
        CVort_engine_delay(100);
        level_finished = LEVEL_END_TANTALUS;
        return;
    }
    switch (map_data_tiles[keen_tileY * map_width_T + keen_tileX]) {
        case 0x10F:
            if (lights)
                CVort_lights_out();
            else
                CVort_lights_on();
            return;
        case 0x1E0:
            map_data_tiles[keen_tileY * map_width_T + keen_tileX] = 0x1ED;
            break;
        case 0x1ED:
            map_data_tiles[keen_tileY * map_width_T + keen_tileX] = 0x1E0;
            break;
        default:;
    }
    int16_t var_2 = map_data_sprites[keen_tileY * map_width_T + keen_tileX];
    int16_t var_A = CVort_engine_signExtend8To16(var_2 & 0xFF);
    int16_t var_6 = keen_tileX + var_A;
    int16_t var_8 = keen_tileY + CVort_engine_signExtend8To16((var_2 >> 8) & 0xFF);
    // Going over the bodies
    for (loopVar = 0; loopVar < num_bodies; loopVar++) {
        if (!bodies[loopVar].type_)
            continue;
        if (bodies[loopVar].tile_x != var_6)
            continue;
        if (bodies[loopVar].tile_y == var_8)
            break;
    }
    if (loopVar < num_bodies) {
        if (bodies[loopVar].think_ptr == &CVort_body_bridge_retract)
            (bodies[loopVar].think_ptr = &CVort_body_bridge_extend);
        else
            (bodies[loopVar].think_ptr = &CVort_body_bridge_retract);
        return;
    }
    int16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = 2;
    bodies[bodyIndex].think_ptr = &CVort_body_bridge_extend;
    bodies[bodyIndex].tile_x = var_6;
    bodies[bodyIndex].tile_y = var_8;
    bodies[bodyIndex].variant = 0;
    if (TILEINFO_LEdge[map_data_tiles[var_8 * map_width_T + var_6 + 1]])
        bodies[bodyIndex].field_C = -1;
    else
        bodies[bodyIndex].field_C = 1;
    bodies[bodyIndex].field_E = map_data_tiles[var_8 * map_width_T + var_6];
    bodies[bodyIndex].field_10 = 0;
}


/*
 * Enemy Behaviours
 * Put common stuff in the "base file"
 * If a function really needs "overriding", rewrite it in the episode file
 * If an enemy only appears in one episode, put it in the episode file
 */

// Perhaps Vorticons code should be copied three times
void CVort_add_sprite_vorticon(int16_t tileX, int16_t tileY)
{
	int16_t sprIndex = CVort_add_sprite();
	sprites[sprIndex].type_ = OBJVORTICON;
	sprites[sprIndex].posX = tileX<<12;
	sprites[sprIndex].posY = tileY<<12;
	sprites[sprIndex].think = &CVort_think_vorticon_walk;
	sprites[sprIndex].contact = &CVort_contact_vorticon;

	if (engine_gameVersion == GAMEVER_KEEN1) {
		sprites[sprIndex].health = 3;
		if (current_level == 16)
			sprites[sprIndex].health = 104;
	} else {
		sprites[sprIndex].health = 1;
	}

	if (sprites[sprIndex].posX > sprites[0].posX)
		sprites[sprIndex].velX = -90;
	else
		sprites[sprIndex].velX = 90;
	sprites[sprIndex].frame = VORTFRAME(stand1);
}

void CVort_add_sprite_tankshot(int32_t posX, int32_t posY, int16_t velX) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = OBJENEMYSHOT;
    sprites[sprIndex].posX = posX;
    if (engine_gameVersion == GAMEVER_KEEN1) {
        sprites[sprIndex].posY = posY + 0x500;
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        sprites[sprIndex].posY = posY + 0x900;
    }
    sprites[sprIndex].think = &CVort_think_keengun;
    sprites[sprIndex].velX = velX;
    sprites[sprIndex].contact = &CVort_contact_tankshot;
    sprites[sprIndex].frame = SPRTANKSHOT;
    if (velX >= 0) {
        if (!TILEINFO_REdge[map_data_tiles[(posX >> 12)+((posY >> 12) + 1) * map_width_T + 1]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        sprites[sprIndex].type_ = OBJZAPZOT;
        sprites[sprIndex].think = &CVort_think_zapzot;
        sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            sprites[sprIndex].frame = SPRSHOTSPLASHL;
        } else {
            sprites[sprIndex].frame = SPRSHOTSPLASHR;
        }
    } else {
        if (!TILEINFO_LEdge[map_data_tiles[((posY >> 12) + 1) * map_width_T + (posX >> 12)]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        sprites[sprIndex].type_ = OBJZAPZOT;
        sprites[sprIndex].think = &CVort_think_zapzot;
        sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            sprites[sprIndex].frame = SPRSHOTSPLASHL;
        } else {
            sprites[sprIndex].frame = SPRSHOTSPLASHR;
        }
    }
}

void CVort_add_sprite_youth(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = OBJYOUTH;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort_think_youth_walk;
    sprites[sprIndex].contact = &CVort_contact_youth;
    sprites[sprIndex].health = 1;
    sprites[sprIndex].velX = sprites[0].posX > sprites[sprIndex].posX? 250 : -250;
    sprites[sprIndex].frame = YOUTHFRAME(left1);
}

void CVort_think_vorticon_walk()
{
	if (temp_sprite.velX > 0)
		temp_sprite.frame = VORTFRAME(right1);
	else
		temp_sprite.frame = VORTFRAME(left1);
	temp_sprite.frame += ((CVort_ptr_engine_getTicks()>>4)&3);
	if (CVort_get_random() < (sprite_sync<<1)
	    && ((engine_gameVersion == GAMEVER_KEEN1) || lights))
	{
		temp_sprite.velY = -CVort_calc_jump_height(300);
		temp_sprite.think = &CVort_think_vorticon_jump;
	}
	else if (CVort_get_random() < (sprite_sync<<1))
	{
		if (temp_sprite.velX == 90)
			temp_sprite.velX = 120;
		else if (temp_sprite.velX > 90)
		{
			if (temp_sprite.velX == 120)
				temp_sprite.velX = 90;
		}
		else if (temp_sprite.velX == -120)
			temp_sprite.velX = -90;
		else if (temp_sprite.velX == -90)
			temp_sprite.velX = -120;
	}
	CVort_do_fall();
	int16_t currDelta = CVort_compute_sprite_delta();
	if (currDelta & 4)
		temp_sprite.velX = -90;
	if (currDelta & 1)
		temp_sprite.velX = 90;
        if ((engine_gameVersion != GAMEVER_KEEN1)
	    && !(currDelta & 2))
		temp_sprite.think = &CVort_think_vorticon_jump;
}

void CVort_think_vorticon_jump()
{
	if (temp_sprite.velX > 0)
		temp_sprite.frame = VORTFRAME(jumpl);
	else
		temp_sprite.frame = VORTFRAME(jumpr);
	CVort_do_fall();
	int16_t currDelta = CVort_compute_sprite_delta();
	if (currDelta & 2)
	{
		temp_sprite.think = &CVort_think_vorticon_search;
		temp_sprite.time = 0;
	}
	if (currDelta & 4)
		temp_sprite.velX = -90;
	if (currDelta & 1)
		temp_sprite.velX = 90;
}

void CVort_think_vorticon_search()
{
	temp_sprite.velX = 0;
	temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&3)+VORTFRAME(stand1);
	temp_sprite.time += sprite_sync;
	if (temp_sprite.time >= 80)
	{
		temp_sprite.velX = 90;
		if (temp_sprite.posX > sprites[0].posX)
			temp_sprite.velX = -temp_sprite.velX;
		temp_sprite.think = &CVort_think_vorticon_walk;
	}
	CVort_do_fall();
	CVort_compute_sprite_delta();
}

void CVort_contact_vorticon(Sprite_T *vorticon, Sprite_T *contacted)
{
	if (engine_gameVersion == GAMEVER_KEEN3) {
		if ((contacted->type_ != 15) && (contacted->type_ != 16))
			return;
	} else {
		if ((contacted->type_ != 10) && (contacted->type_ != 11))
			return;
	}
	if (engine_gameVersion == GAMEVER_KEEN1) {
		int16_t currHealth = vorticon->health;
		vorticon->health--;
		if (currHealth)
			return;
        } else {
		vorticon->health--;
	        if (vorticon->health)
			return;
	}
	CVort_engine_setCurSound(0x27);
	vorticon->time = 0;
	if (engine_gameVersion == GAMEVER_KEEN1) {
		vorticon->varB = 6;
	} else {
	        vorticon->varB = 2;
	}
	vorticon->frame = VORTFRAME(die1);
	vorticon->contact = &CVort_think_contact_nop;
	vorticon->think = &CVort_think_kill_sprite;
	if (engine_gameVersion == GAMEVER_KEEN1) {
		int16_t bodyBorderFlashIndex = CVort_add_body();
		bodies[bodyBorderFlashIndex].type_ = 5;
		bodies[bodyBorderFlashIndex].think_ptr = &CVort_body_border_flash;
		bodies[bodyBorderFlashIndex].variant = 0;
	}
}

void CVort_contact_tankshot(Sprite_T *tankshot, Sprite_T *contacted) {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        if ((contacted->type_ == 6) || (contacted->type_ == 14))
            return;
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        if ((contacted->type_ == CVort2_obj_guardbot) || (contacted->type_ == CVort2_obj_dead) || (contacted->type_ == CVort2_obj_elite))
            return;
    }
    CVort_engine_setCurSound(SNDSHOTHIT);
    tankshot->think = &CVort_think_zapzot;
    tankshot->time = 0;
    tankshot->contact = &CVort_think_contact_nop;
    if (CVort_get_random() > 0x80) {
        tankshot->frame = SPRSHOTSPLASHR;
    } else {
        tankshot->frame = SPRSHOTSPLASHL;
    }
}

void CVort_think_youth_walk()
{
    uint16_t blocking;

    temp_sprite.frame = temp_sprite.velX > 0? YOUTHFRAME(right1) : YOUTHFRAME(left1);
    temp_sprite.frame += (CVort_ptr_engine_getTicks()>>4)&3;
    if (CVort_get_random()<sprite_sync*3)
    {
       temp_sprite.velY = -CVort_calc_jump_height(400); 
       temp_sprite.think = &CVort_think_youth_jump;
    }

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (!(blocking & 2))
    {
        temp_sprite.think = &CVort_think_youth_jump;
    }

    if (blocking & 4)
    {
        temp_sprite.velX = -250;
    }

    if (blocking & 1)
    {
        temp_sprite.velX = 250;
    }

}

void CVort_think_youth_jump()
{
    uint16_t blocking;

    temp_sprite.frame = temp_sprite.velX > 0? YOUTHFRAME(right4) : YOUTHFRAME(left4);

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (blocking&2)
    {
        temp_sprite.think = &CVort_think_youth_walk;
    }

    if (blocking & 4)
    {
        temp_sprite.velX = -250;
    }

    if (blocking & 1)
    {
        temp_sprite.velX = 250;
    }

}

void CVort_contact_youth(Sprite_T *youth, Sprite_T *contacted)
{
    if (contacted->type_ == OBJKEENSHOT || ((engine_gameVersion == GAMEVER_KEEN2) && contacted->type_ == OBJENEMYSHOT)) {
        CVort_engine_setCurSound(SNDVORTSCREAM);
        youth->time = 0;
        youth->varB = 2;
        youth->frame = YOUTHFRAME(die1);
        youth->contact = &CVort_contact_nop;
        youth->think = &CVort_think_kill_sprite;
    }

}

void CVort_add_sprite_keengun(int32_t posX, int32_t posY) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = OBJKEENSHOT;
    sprites[sprIndex].posX = posX;
    sprites[sprIndex].posY = posY + 0x900;
    sprites[sprIndex].think = &CVort_think_keengun;
    sprites[sprIndex].velY = 0;
    sprites[sprIndex].contact = &CVort_contact_keengun;
    sprites[sprIndex].frame = SPRKEENSHOT;
    if (keen_facing >= 0) {
        sprites[sprIndex].velX = 400;
        if (!TILEINFO_REdge[map_data_tiles[(posX >> 12)+((posY >> 12) + 1) * map_width_T + 1]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        sprites[sprIndex].type_ = OBJZAPZOT;
        sprites[sprIndex].think = &CVort_think_zapzot;
        sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            sprites[sprIndex].frame = SPRSHOTSPLASHR;
        } else {
            sprites[sprIndex].frame = SPRSHOTSPLASHL;
        }
    } else {
        sprites[sprIndex].velX = -400;
        if (!TILEINFO_LEdge[map_data_tiles[((posY >> 12) + 1) * map_width_T + (posX >> 12)]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        sprites[sprIndex].type_ = OBJZAPZOT;
        sprites[sprIndex].think = &CVort_think_zapzot;
        sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            sprites[sprIndex].frame = SPRSHOTSPLASHR;
        } else {
            sprites[sprIndex].frame = SPRSHOTSPLASHL;
        }
    }
}

void CVort_think_keen_ground() {
    int16_t tile_standingon_type, currtile_standingon_type, tile_collision, map_tile_left, map_tile_right, map_tile_standingon;
    if (temp_sprite.varD)
        tile_standingon_type = 1;
    else {
        map_tile_left = temp_sprite.boxX1 >> 12;
        map_tile_right = temp_sprite.boxX2 >> 12;
        map_tile_standingon = (temp_sprite.boxY2 >> 12) + 1;
        tile_standingon_type = 1;

        for (int16_t currX = map_tile_left; currX <= map_tile_right; currX++) {
            currtile_standingon_type = TILEINFO_UEdge[map_data_tiles[map_tile_standingon * map_width_T + currX]];
            if (currtile_standingon_type > 1)
                tile_standingon_type = currtile_standingon_type;
        }
    }
    if (input_new.but1jump) {
        temp_sprite.varC = temp_sprite.velX;
        temp_sprite.velX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        if (keen_facing >= 0)
            temp_sprite.varA = 8;
        else
            temp_sprite.varA = 0xE;
        temp_sprite.think = &CVort_think_keen_jump_ground;
    }
    if (tile_standingon_type < 3)
        switch (input_new.direction) {
            case 1:
            case 2:
            case 3:
                CVort_move_left_right(2);
                if (temp_sprite.velX < 0)
                    input_new.direction = 8;
                break;
            case 5:
            case 6:
            case 7:
                CVort_move_left_right(-2);
                if (temp_sprite.velX > 0)
                    input_new.direction = 8;
                break;
            default:
                break;
        }
    if ((tile_standingon_type == 1) && (input_new.direction == 8)) {
        if (temp_sprite.velX > 0) {
            CVort_move_left_right(-3);
            if (temp_sprite.velX < 0)
                temp_sprite.velX = 0;
        } else if (temp_sprite.velX < 0) {
            CVort_move_left_right(3);
            if (temp_sprite.velX > 0)
                temp_sprite.velX = 0;
        }
    }
    if (tile_standingon_type == 3) {
        if (keen_facing > 0)
            temp_sprite.velX = 180;
        else if (keen_facing < 0)
            temp_sprite.velX = -180;
    }
    if (!temp_sprite.velX) {
        if (keen_facing >= 0)
            temp_sprite.frame = 0;
        else
            temp_sprite.frame = 4;
    } else {
        if (temp_sprite.velX > 0) {
            temp_sprite.frame = 0;
            if (tile_standingon_type < 3)
                temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4)&3;

        } else {
            temp_sprite.frame = 4;
            if (tile_standingon_type < 3)
                temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4)&3;
        }
        keen_facing = temp_sprite.velX;
    }
    if (temp_sprite.velX && ((CVort_ptr_engine_getTicks() >> 4)&1) && (tile_standingon_type < 3)) {
        if ((CVort_ptr_engine_getTicks() >> 5)&1)
            CVort_engine_setCurSound(0x1E);
        else
            CVort_engine_setCurSound(4);
    }
    CVort_do_fall();
    tile_collision = CVort_compute_sprite_delta();
    CVort_check_ceiling();
    if (((tile_collision & 4) || (tile_collision & 1)) && ((CVort_ptr_engine_getTicks() >> 4)&1))
        CVort_engine_setCurSound(5);
    if (!(tile_collision & 2) && !temp_sprite.varD) {
        temp_sprite.think = &CVort_think_keen_jump_air;
        CVort_engine_setCurSound(0x1B);
        return;
    }
    if (input_new.but1jump) {
        temp_sprite.varC = temp_sprite.velX;
        temp_sprite.velX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        if (keen_facing >= 0)
            temp_sprite.varA = 8;
        else
            temp_sprite.varA = 0xE;
        temp_sprite.think = &CVort_think_keen_jump_ground;
    }
    if (input_new.but2pogo && !input_old.but2pogo) {
        if (keen_switch)
            CVort_toggle_switch();
        else {
            temp_sprite.time = 0;
            temp_sprite.varB = temp_sprite.velX;
            temp_sprite.velX = 0;
            if (keen_gp.stuff[3])
                temp_sprite.think = &CVort_think_keen_pogo_ground;
        }
    }
    if (input_new.but1jump && input_new.but2pogo && // Two-button firing
            !input_old.but1jump && !input_old.but2pogo) {
        temp_sprite.think = &CVort_think_keen_shoot;
        temp_sprite.delX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        //Again???
        //temp_sprite.think = &CVort_think_keen_shoot;
        if (keen_facing > 0)
            temp_sprite.frame = 0x14;
        else
            temp_sprite.frame = 0x15;
    }
    if (temp_sprite.varD)
        temp_sprite.varD--;
}

void CVort_think_keen_jump_ground() {
    temp_sprite.frame = temp_sprite.varA + temp_sprite.time / 6;
    if (input_new.but1jump)
        temp_sprite.varB += sprite_sync * 6;
    else if (temp_sprite.time < 12)
        temp_sprite.time = 24 - temp_sprite.time;

    switch (input_new.direction) {
        case 1: // Right (possibly diagonal)
        case 2:
        case 3:
            temp_sprite.varC += (sprite_sync << 1);
            if (temp_sprite.varC > 0x78)
                temp_sprite.varC = 0x78;
            break;
        case 5: // Left (maybe diagonal)
        case 6:
        case 7:
            temp_sprite.varC -= (sprite_sync << 1);
            if (temp_sprite.varC < -0x78)
                temp_sprite.varC = -0x78;
            break;
        default:
            break;
    }
    temp_sprite.velX = 0;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time >= 36) {
        temp_sprite.think = &CVort_think_keen_jump_air;
        temp_sprite.velY -= temp_sprite.varB;
        temp_sprite.velX = temp_sprite.varC;
        CVort_engine_setCurSound(6);
    }
    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();

    if (input_new.but1jump && input_new.but2pogo) // Two-button firing
    {
        temp_sprite.think = &CVort_think_keen_shoot;
        temp_sprite.delX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        //Again... for another time???
        //temp_sprite.think = &CVort_think_keen_shoot;
        if (keen_facing > 0)
            temp_sprite.frame = 0x14;
        else
            temp_sprite.frame = 0x15;
    }
}

void CVort_think_keen_jump_air() {
    switch (input_new.direction) {
        case 1:
        case 2:
        case 3:
            CVort_move_left_right(2);
            if (temp_sprite.velX < 0)
                input_new.direction = 8;
            break;
        case 5:
        case 6:
        case 7:
            CVort_move_left_right(-2);
            if (temp_sprite.velX > 0)
                input_new.direction = 8;
            break;
        default:
            break;
    }
    if (input_new.direction == 8) {
        if (temp_sprite.velX > 0) {
            CVort_move_left_right(-1);
            if (temp_sprite.velX < 0)
                temp_sprite.velX = 0;
        } else if (temp_sprite.velX < 0) {
            CVort_move_left_right(1);
            if (temp_sprite.velX > 0)
                temp_sprite.velX = 0;
        }
    }
    if (keen_facing > 0)
        temp_sprite.frame = 0xD;
    else
        temp_sprite.frame = 0x13;
    if (temp_sprite.velX)
        keen_facing = temp_sprite.velX;

    CVort_do_fall();
    int16_t lastDelta = CVort_compute_sprite_delta();
    if (((lastDelta & 4) || (lastDelta & 1)) && ((CVort_ptr_engine_getTicks() >> 4) & 1))
        CVort_engine_setCurSound(5);
    if (lastDelta & 2) {
        temp_sprite.think = &CVort_think_keen_ground;
        CVort_engine_setCurSound(7);
        return;
    }
    if (lastDelta & 8)
        CVort_engine_setCurSound(0x15);

    CVort_check_ceiling();
    if (input_new.but2pogo && !input_old.but2pogo) {
        if (keen_switch)
            CVort_toggle_switch();
        else if (keen_gp.stuff[3]) // Keen has gotten a pogo stick?
            temp_sprite.think = &CVort_think_keen_pogo_air;
    }
    if (input_new.but1jump && input_new.but2pogo && // Two-button firing
            !input_old.but1jump && !input_old.but2pogo) {
        temp_sprite.think = &CVort_think_keen_shoot;
        temp_sprite.delX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        //Yet again...
        //temp_sprite.think = &CVort_think_keen_shoot;
        if (keen_facing > 0)
            temp_sprite.frame = 0x14;
        else
            temp_sprite.frame = 0x15;
    }
}

void CVort_think_keen_shoot() {
    temp_sprite.time += sprite_sync;
    if (!temp_sprite.varB && (temp_sprite.time > 1)) {
        if (keen_gp.ammo) {
            CVort_engine_setCurSound(0xC);
            keen_gp.ammo--;
            CVort_add_sprite_keengun(temp_sprite.posX, temp_sprite.posY);
        } else
            CVort_engine_setCurSound(0x24);
        temp_sprite.varB = 1;
    }
    if ((temp_sprite.time > 30) && !input_new.but1jump && !input_new.but2pogo)
        temp_sprite.think = &CVort_think_keen_ground;
    if (temp_sprite.velX > 0) {
        CVort_move_left_right(-1);
        if (temp_sprite.velX < 0)
            temp_sprite.velX = 0;
    } else if (temp_sprite.velX < 0) {
        CVort_move_left_right(1);
        if (temp_sprite.velX > 0)
            temp_sprite.velX = 0;
    }
    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();
}

void CVort_think_keen_pogo_air() {
    input_new = CVort_handle_ctrl(1);
    switch (input_new.direction) {
        case 1:
        case 2:
        case 3:
            CVort_move_left_right(1);
            if (temp_sprite.velX < 0)
                input_new.direction = 8;
            break;
        case 5:
        case 6:
        case 7:
            CVort_move_left_right(-1);
            if (temp_sprite.velX > 0)
                input_new.direction = 8;
            break;
        default:
            break;
    }
    if (input_new.but1jump && (temp_sprite.velY < 0))
        CVort_pogo_jump(200, -1);
    if (god_mode && input_new.but1jump) // Cheat in effect!
        temp_sprite.velY = -200;
    if (temp_sprite.velX)
        keen_facing = temp_sprite.velX;
    if (keen_facing >= 0)
        temp_sprite.varA = 0x18;
    else
        temp_sprite.varA = 0x1A;
    temp_sprite.frame = temp_sprite.varA;
    CVort_do_fall();
    int16_t currDelta = CVort_compute_sprite_delta();
    if (((currDelta & 4) || (currDelta & 1)) && ((CVort_ptr_engine_getTicks() >> 4)&1))
        CVort_engine_setCurSound(5);
    if (currDelta & 2) {
        temp_sprite.think = &CVort_think_keen_pogo_ground;
        temp_sprite.time = 0;
        temp_sprite.varB = temp_sprite.velX;
        temp_sprite.velX = 0;
    }
    if (currDelta & 8)
        CVort_engine_setCurSound(0x15);
    CVort_check_ceiling();
    if (input_new.but2pogo && !input_old.but2pogo) {
        if (keen_switch)
            CVort_toggle_switch();
        else
            temp_sprite.think = &CVort_think_keen_jump_air;
    }
    if (input_new.but1jump && input_new.but2pogo) // Two-button firing
    {
        temp_sprite.think = &CVort_think_keen_shoot;
        temp_sprite.delX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        //And again...
        //temp_sprite.think = &CVort_think_keen_shoot;
        if (keen_facing > 0)
            temp_sprite.frame = 0x14;
        else
            temp_sprite.frame = 0x15;
    }
    if (temp_sprite.varD)
        temp_sprite.varD--;
}

void CVort_think_keen_pogo_ground() {
    temp_sprite.delX = temp_sprite.velX = 0;
    temp_sprite.frame = temp_sprite.varA + 1;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time > 22) {
        temp_sprite.think = &CVort_think_keen_pogo_air;
        temp_sprite.varD = 0;
        temp_sprite.velY -= 200;
        temp_sprite.velX = temp_sprite.varB;
        CVort_engine_setCurSound(6);
    }
    if (input_new.but2pogo && !input_old.but2pogo)
        temp_sprite.think = &CVort_think_keen_ground;
    if (input_new.but1jump && input_new.but2pogo) // Two-button firing
    {
        temp_sprite.think = &CVort_think_keen_shoot;
        temp_sprite.delX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        //Aaand again...
        //temp_sprite.think = &CVort_think_keen_shoot;
        if (keen_facing > 0)
            temp_sprite.frame = 0x14;
        else
            temp_sprite.frame = 0x15;
    }
    CVort_compute_sprite_delta();
    temp_sprite.varD = 0;
}

void CVort_think_keen_exit() {
    int32_t time = temp_sprite.time, time2 = temp_sprite.varB;
    temp_sprite.delX = sprite_sync * 60;
    temp_sprite.frame = (CVort_ptr_engine_getTicks() >> 4) & 3;
    keen_facing = temp_sprite.velX;
    // Draw exit door boundry tiles so they effectively hide Keen
    if (engine_gameVersion == GAMEVER_KEEN3) {
        CVort_engine_drawTileAt(time << 12, time2 << 12, 0x101);
        CVort_engine_drawTileAt(time << 12, (time2 + 1) << 12, 0x101);
        CVort_engine_drawTileAt((time + 1) << 12, time2 << 12, 0xF6);
        CVort_engine_drawTileAt((time + 1) << 12, (time2 + 1) << 12, 0xF6);
    } else {
        CVort_engine_drawTileAt(time << 12, time2 << 12, 0xA0);
        CVort_engine_drawTileAt(time << 12, (time2 + 1) << 12, 0xA0);
        CVort_engine_drawTileAt((time + 1) << 12, time2 << 12, 0x8F);
        CVort_engine_drawTileAt((time + 1) << 12, (time2 + 1) << 12, 0x8F);
    }
    if ((time << 12) <= temp_sprite.posX) {
        temp_sprite.type_ = 0;
        level_finished = LEVEL_END_EXIT;   
    }
}

void CVort_think_keen_death() {
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time >= 200) {
        temp_sprite.time = -999;
        temp_sprite.velX = CVort_get_random() - 0x80;
        temp_sprite.velY = -0x190;
    }
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 4)&1) + 0x16;
    temp_sprite.delX = temp_sprite.velX * sprite_sync;
    temp_sprite.delY = temp_sprite.velY * sprite_sync;
    if (temp_sprite.boxY2 < scrollY)
        temp_sprite.type_ = 0;
}

void CVort_think_keen_stunned() {

    // TODO: We don't need to handle Keen 1? (Apparently this shared code with think_keen_frozen)
    if (engine_gameVersion == GAMEVER_KEEN1) {
        temp_sprite.frame = CVort1_spr_keensicle1 + ((CVort_ptr_engine_getTicks()>>5)&1);
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        temp_sprite.frame = CVort2_spr_keensicle1 + ((CVort_ptr_engine_getTicks()>>5)&1);
    } else {
        temp_sprite.frame = CVort3_spr_keensicle1 + ((CVort_ptr_engine_getTicks()>>5)&1);
    }
    if ((temp_sprite.time-=sprite_sync) < 0)
    {
        if (engine_gameVersion == GAMEVER_KEEN1) {
            temp_sprite.frame = CVort1_spr_keengetsup;
        } else if (engine_gameVersion == GAMEVER_KEEN2) {
            temp_sprite.frame = CVort2_spr_keengetsup;
        } else {
            temp_sprite.frame = CVort3_spr_keengetsup;
        }
        if (temp_sprite.time < -40)
        {
            temp_sprite.think = &CVort_think_keen_ground;
        }
    }

    if (temp_sprite.velX > 0)
    {
        CVort_move_left_right(-3);
        if (temp_sprite.velX < 0)
        {
            temp_sprite.velX = 0;
        }
    }
    else if (temp_sprite.velX < 0)
    {
        CVort_move_left_right(3);
        if (temp_sprite.velX > 0)
        {
            temp_sprite.velX = 0;
        }
    }

    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();
}

void CVort_think_zapzot() {
    temp_sprite.type_ = OBJDEAD;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time > 20) {
        temp_sprite.type_ = OBJNULL;
    }
}

void CVort_think_keengun() {
    int16_t currDelta = CVort_compute_sprite_delta();
    if (!currDelta)
        return;
    CVort_engine_setCurSound(SNDSHOTHIT);
    temp_sprite.type_ = OBJZAPZOT;
    temp_sprite.think = &CVort_think_zapzot;
    temp_sprite.time = 0;
    if (CVort_get_random() > 0x80) {
        temp_sprite.frame = SPRSHOTSPLASHR;
    } else {
        temp_sprite.frame = SPRSHOTSPLASHL;
    }
}

void CVort_contact_keengun(Sprite_T *keengun, Sprite_T *contacted) {
    if ((contacted->type_ == OBJKEEN) || (contacted->type_ == OBJDEAD))
        return;

    CVort_engine_setCurSound(SNDSHOTHIT);
    keengun->think = &CVort_think_zapzot;
    keengun->contact = &CVort_think_contact_nop;
    keengun->time = 0;
    if (CVort_get_random() > 0x80) {
        keengun->frame = SPRSHOTSPLASHR;
    } else {
        keengun->frame = SPRSHOTSPLASHL;
    }
}
