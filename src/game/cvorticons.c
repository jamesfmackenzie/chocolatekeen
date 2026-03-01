#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "core/cvorticons.h"
#include "episodes/cvorticons1.h"
#include "episodes/cvorticons2.h"
#include "episodes/cvorticons3.h"
#include "third_party/cgenius/fileio/compression/CRLE.h"

#include "episodes/cvorticons_episode_macros.h"

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
    if (g_input.key_map[0x2E] && g_input.key_map[0x14] && g_input.key_map[0x39])
        CVort_start_cheating();
    if (g_input.key_map[0x22] && g_input.key_map[0x18] && g_input.key_map[0x20]) {
        CVort_clear_keys();
        CVort_draw_box_opening_main(0x14, 1);
        g_game.god_mode ^= 1;
        if (g_game.god_mode)
            CVort_draw_string("God mode enabled");
        else
            CVort_draw_string("God mode disabled");
        CVort_read_char_with_echo();
        CVort_engine_clearOverlay();
    } else if (g_input.key_map[0x39]) {
        CVort_engine_saveCurSound();
        CVort_ptr_show_pause_menu();
        CVort_engine_restoreCurSound();
    }
}

void CVort_main() {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        snprintf(game_ext, sizeof(game_ext), "CK1");
        CVort_engine_puts("Loading Commander Keen part 1... (version 1.31 1/23/90)");
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        snprintf(game_ext, sizeof(game_ext), "CK2");
        CVort_engine_puts("Loading Commander Keen part 2... (version 1.31 1/23/90)");
    } else if (engine_gameVersion == GAMEVER_KEEN3) {
        snprintf(game_ext, sizeof(game_ext), "CK3");
        CVort_engine_puts("Loading Commander Keen part 3... (version 1.31 1/23/90)");
    }

    if (CVort_engine_isJoystickInUse()) {
        CVort_engine_puts("Joystick detected");
        g_input.joy_detect = 1; // Actually never used on vanilla Keen...
    } else {
        CVort_engine_puts("Joystick not detected");
        g_input.joy_detect = 0;
    }
    // If we pass keystrokes to BIOS or not, it doesn't matter us.
    // But we simulate the message anyway... "cheating" though.
    g_input.pass_keys_to_bios = 1;
    if (!engine_arguments.passKeysToBios) {
        g_input.pass_keys_to_bios = 0;
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
        g_input.ctrl_type[1] = 0;
        break;
    case EMU_INITIAL_INPUT_MOUSE:
        g_input.ctrl_type[1] = 1;
        break;
    case EMU_INITIAL_INPUT_JOY1:
        g_input.ctrl_type[1] = 2;
        break;
    case EMU_INITIAL_INPUT_JOY2:
        g_input.ctrl_type[1] = 3;
        break;
    default: ; // EMU_INITIAL_INPUT_LAST
    }

    // We don't need to allocate memory for level data; It's done
    // statically: An array of 10000h bytes in the stack.
    textbox_offs_x = 0x17;
    g_game.anim_speed = 7;
    CVort_ptr_engine_setTicksSync(0);
    CVort_ptr_engine_setTicks(0);
    CVort_main_loop();
}

int16_t CVort_add_sprite() {
    int16_t spriteNum;
    for (spriteNum = 1; g_entities.sprites[spriteNum].type_ && (spriteNum < g_entities.num_sprites); spriteNum++);
    if (spriteNum >= g_entities.num_sprites)
        g_entities.num_sprites++;
    memset(&g_entities.sprites[spriteNum], 0, sizeof (g_entities.sprites[spriteNum]));
    g_entities.sprites[spriteNum].think = &CVort_default_think;
    g_entities.sprites[spriteNum].contact = &CVort_default_contact;
    g_entities.sprites[spriteNum].active = 1;
    return spriteNum;
}

int16_t CVort_add_body() {
    int16_t bodyNum;
    for (bodyNum = 0; g_entities.bodies[bodyNum].type_ && (bodyNum < g_entities.num_bodies); bodyNum++);
    if (bodyNum >= g_entities.num_bodies)
        g_entities.num_bodies++;
    memset(&g_entities.bodies[bodyNum], 0, sizeof (g_entities.bodies[bodyNum]));
    // FIXME: The casting is potentially DANGEROUS, but the default_think
    // function is also used for g_entities.sprites...
    g_entities.bodies[bodyNum].think_ptr = (BodyThinkFunction_T)(&CVort_default_think);
    return bodyNum;
}

/*
 * In Vanilla keen, the text files are either linked into the .exe or they are
 * present externally.
 * Chocolate Keen will first look for an external file and then in the .exe
 */
void CVort_main_loop() {

    // The file names ARE constructed on vanilla Keen 2, although never used
    char helptext[14],    endtext[14],
         storytext[14],  previewtext[14];
    snprintf(helptext,    sizeof(helptext),    "HELPTEXT.%s",  game_ext);
    snprintf(endtext,     sizeof(endtext),     "ENDTEXT.%s",   game_ext);
    snprintf(storytext,   sizeof(storytext),   "STORYTXT.%s",  game_ext);
    snprintf(previewtext, sizeof(previewtext), "PREVIEWS.%s",  game_ext);

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

    g_game.on_world_map = 0;
    CVort_load_high_scores_table();
    CVort_fade_out();
    while (true) {
        CVort_do_intro_and_menu();
        CVort_draw_worldmap();
    }
}

void CVort_update_sprite_hitbox(void) {
    //update_sprite_hitbox_wmap(&g_entities.temp_sprite)?
    int16_t tempNum = (g_entities.temp_sprite.frame << 2) + (g_entities.temp_sprite.pos_x >> 9) % 4;
    g_entities.temp_MSE = engine_maskedSpriteEntry[tempNum];
    g_entities.temp_sprite.box_x1 = g_entities.temp_sprite.pos_x + g_entities.temp_MSE.hitbox_l;
    g_entities.temp_sprite.box_x2 = g_entities.temp_sprite.pos_x + g_entities.temp_MSE.hitbox_r;
    g_entities.temp_sprite.box_y1 = g_entities.temp_sprite.pos_y + g_entities.temp_MSE.hitbox_u;
    g_entities.temp_sprite.box_y2 = g_entities.temp_sprite.pos_y + g_entities.temp_MSE.hitbox_b;
}

void CVort_update_sprite_hitbox_wmap(Sprite_T *sprite) {
    int16_t tempNum = (sprite->frame << 2) + (sprite->pos_x >> 9) % 4;
    g_entities.temp_MSE = engine_maskedSpriteEntry[tempNum];
    sprite->box_x1 = sprite->pos_x + g_entities.temp_MSE.hitbox_l;
    sprite->box_x2 = sprite->pos_x + g_entities.temp_MSE.hitbox_r;
    sprite->box_y1 = sprite->pos_y + g_entities.temp_MSE.hitbox_u;
    sprite->box_y2 = sprite->pos_y + g_entities.temp_MSE.hitbox_b;
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

int16_t CVort_handle_global_keys() {
    int16_t result = 0;
    CVort_translate_key(1);
    // Original debugging code?
    // if (0x82EA == 0) return 0;

    uint32_t saved_tickcount = CVort_ptr_engine_getTicks();

    int16_t filteredKeyScan = g_input.key_scane & 0x7F;
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
                    g_game.want_sound = 0;
                    break;
                case 0x59: // 'Y'
                    g_game.want_sound = 1;
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
    if (keen_gp.score - g_game.extra_life_pts < 20000)
        return;
    CVort_engine_setCurSound(0x1C);
    g_game.extra_life_pts = (keen_gp.score / 20000)*20000;
    keen_gp.lives++;
}

void CVort_load_level_data(uint16_t levelnum) {
    g_game.current_level = levelnum;
    char filename[16];
    snprintf(filename, sizeof(filename), "%s%02" PRIu16 ".%s", "LEVEL", levelnum, game_ext);
    FILE *fp = CVort_engine_cross_ro_data_fopen(filename);
    /* TODO? We could apply random map generation if file is not found */
    /* (e.g. if Keen enters a "random" map from the worldmap while a   */
    /* cheat code is in use). However, most chances are that it would  */
    /* look vastly different than the one on vanilla Keen.             */
    if (!fp)
        return;
    uint32_t len = CVort_filelength(fp);
    uint8_t *compressedMapData = (uint8_t*)malloc(sizeof(uint8_t) * len);
    fread(compressedMapData, len, 1, fp);
    fclose(fp);

    // Here, code from Commander Genius is used as-is
    CRLE_expandSwapped((word*)map_data, compressedMapData, 0xFEFE);
    // We remove the initial DWORD of uncompressed data size
    memmove(map_data, map_data+1, sizeof(map_data)/sizeof(int16_t)-2);
    // FIXME: Are byteswaps required for the Big-Endian architectures?
    // (Apparently not)

    map_data_tiles = map_data + 16;
    map_data_sprites = map_data + 16 + map_data[7] / 2; // map_data[7] == Plane size

    // FIXME: Should we swap map_data[0] and map_data[1] here - and later?
    map_width_tile = map_data[0];
    map_height_tile = map_data[1];
    map_width_bytes = map_width_tile << 1;
    //screen_wrap = map_width_bytes-0x2A; // map width in tiles x2 MINUS tiles displayed on screen x2
    screen_wrap_single = map_width_tile - ENGINE_VIEWPORT_MAX_X_TILE; // map width in tiles MINUS tiles displayed on screen

    scroll_x_min = scroll_y_min = 0x2000;

    // TODO: Are the shifts done correctly?
    scroll_x_max = (map_data[0] - 0x16) << 12;
    map_width = (map_data[0] - 2) << 12;
    map_height = map_data[1] << 12;
    scroll_y_max = ((map_data[1] - 0xF) << 12) + 0x800;
    ceiling_x = (map_data[0] - 3) << 12;
    ceiling_y = map_data[1] << 12;
}

void CVort_handle_quit() {
    CVort_clear_keys();
    if (g_game.current_level == 90) {
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
            g_game.quit_to_title = 1;
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
void CVort_clear_keys() {
    g_input.key_code = g_input.key_scane = 0;
    memset(g_input.key_map, 0, 0x80);
}

// TODO: The original code simply sets g_game.rnd to the amount of hundredths of
// seconds in the current time.
// For now we use the seconds elapsed...

void CVort_init_rnd(bool doRand) {
    if (doRand)
        g_game.rnd = time(NULL) & 0xFF;
    else
        g_game.rnd = 0;
}

int16_t CVort_get_random() {
    g_game.rnd++;
    g_game.rnd &= 0xFF;
    return (int16_t)(exeFields.rnd_vals[g_game.rnd]);
}

uint32_t CVort_filelength(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    uint32_t len = ftell(fp);
    rewind(fp);
    return len;
}

// TODO: This function should not be used, due to endianness issues
// which vary from file to file.

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

static void CVort_level_init(void) {
    g_entities.sprites[0].type_ = 1;
    g_entities.sprites[0].think = &CVort_think_keen_ground;
    g_entities.sprites[0].contact = CVort_ptr_contact_keen; // NOT &CVort_ptr_contact_keen
    g_entities.sprites[0].vel_x = g_entities.sprites[0].vel_y = 0;
    g_entities.sprites[0].frame = 4;
    g_entities.sprites[0].active = 1;
    g_entities.num_sprites = 1;
    g_entities.num_bodies = 0;
    g_game.keen_facing = 1;
    g_game.level_finished = g_game.god_mode = g_game.keen_invincible = 0;
    if (!engine_arguments.extras.vorticonsDemoModeToggle) {
        g_game.sprite_sync = 0;
        CVort_ptr_engine_setTicksSync(0);
        CVort_ptr_engine_setTicks(0);
    }
}

static void CVort_level_setup_camera(void) {
    scroll_x = g_entities.sprites[0].pos_x + 0xFFFF6000;
    scroll_y = g_entities.sprites[0].pos_y + 0xFFFFB000;
    // Some scrolling stuff...
    if (scroll_x < scroll_x_min)
        scroll_x = scroll_x_min;
    if (scroll_y < scroll_y_min)
        scroll_y = scroll_y_min;
    if (scroll_x > scroll_x_max)
        scroll_x = scroll_x_max;
    if (scroll_y > scroll_y_max)
        scroll_y = scroll_y_max;
}

static void CVort_level_init_screen(void) {
    CVort_engine_syncDrawing();
    CVort_fade_out();
    g_game.lights = 1;
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    CVort_fade_in();
    scroll_x_tile = scroll_x >> 12;
    scroll_y_tile = scroll_y >> 12;
}

static void CVort_run_sprite_think(void) {
    CVort_update_sprite_hitbox();
    g_entities.temp_sprite.del_x = g_entities.temp_sprite.del_y = 0;
    (g_entities.temp_sprite.think)();
    g_entities.temp_sprite.pos_x += g_entities.temp_sprite.del_x;
    g_entities.temp_sprite.pos_y += g_entities.temp_sprite.del_y;
    CVort_update_sprite_hitbox();
}

/* Activate an inactive sprite that has entered the viewport (applying
 * episode-specific on-wake state adjustments), or tick an already-active
 * sprite that is still in range. Operates on g_entities.temp_sprite.
 */
static void CVort_activate_and_think_sprite(int16_t sprite_counter) {
    int16_t var_4, var_6;
    if (!g_entities.temp_sprite.active) {
        var_4 = g_entities.temp_sprite.pos_x >> 12;
        var_6 = g_entities.temp_sprite.pos_y >> 12;
        if ((scroll_x_tile - ENGINE_SPRITE_MARGIN_MIN < var_4) && (scroll_y_tile - ENGINE_SPRITE_MARGIN_MIN < var_6) && (scroll_x_tile + ENGINE_VIEWPORT_WIDTH_TILES > var_4) && (scroll_y_tile + ENGINE_VIEWPORT_HEIGHT_TILES > var_6)) {
            g_entities.temp_sprite.active = 1;
#if CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
            if (engine_gameVersion == GAMEVER_KEEN1) {
                if (g_entities.temp_sprite.think == &CVort1_think_yorp_stunned) {
                    g_entities.temp_sprite.think = &CVort1_think_yorp_look;
                    g_entities.temp_sprite.time = 0;
                }
            }
#endif
#ifndef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
            else
#endif
#if CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
              if (engine_gameVersion == GAMEVER_KEEN3) {
                if (g_entities.temp_sprite.think == &CVort3_think_foob_run) {
                    g_entities.temp_sprite.think = &CVort3_think_foob_walk;
                    if (g_entities.temp_sprite.pos_x > g_entities.sprites[0].pos_x) {
                        g_entities.temp_sprite.vel_x = 50;
                    } else {
                        g_entities.temp_sprite.vel_x = -50;
                    }
                }
            }
#endif
            CVort_run_sprite_think();
        }
    } else if (!sprite_counter
    // Platforms always move
    || ((engine_gameVersion == GAMEVER_KEEN2) && (g_entities.temp_sprite.type_ == CVort2_obj_platform))
    || ((engine_gameVersion == GAMEVER_KEEN3) && (g_entities.temp_sprite.type_ == 10))
    || !CVort_sprite_active_screen() ) {
        CVort_run_sprite_think();
    }
}

static void CVort_level_update_sprites(void) {
    int16_t sprite_counter;
    for (sprite_counter = 0; sprite_counter < g_entities.num_sprites; sprite_counter++) {
        if (!g_entities.sprites[sprite_counter].type_)
            continue;
        g_entities.temp_sprite = g_entities.sprites[sprite_counter];
        CVort_activate_and_think_sprite(sprite_counter);
        g_entities.sprites[sprite_counter] = g_entities.temp_sprite;
    }
}

static void CVort_level_detect_collisions(void) {
    int16_t sprite_counter, var_A;
    // Seems like collision code
    for (sprite_counter = 0; sprite_counter < g_entities.num_sprites; sprite_counter++) {
        if (!g_entities.sprites[sprite_counter].type_ || !g_entities.sprites[sprite_counter].active)
            continue;
        for (var_A = sprite_counter + 1; var_A < g_entities.num_sprites; var_A++) {
            if (!g_entities.sprites[var_A].type_ || !g_entities.sprites[var_A].active)
                continue;
            if (!CVort_detect_sprite_col(&g_entities.sprites[sprite_counter], &g_entities.sprites[var_A]))
                continue;
            (g_entities.sprites[sprite_counter].contact)(&g_entities.sprites[sprite_counter], &g_entities.sprites[var_A]);
            (g_entities.sprites[var_A].contact)(&g_entities.sprites[var_A], &g_entities.sprites[sprite_counter]);
        }
    }
}

static void CVort_level_draw_sprites(void) {
    int16_t sprite_counter;
    // Remember to draw g_entities.sprites; Here, we scan in REVERSED order.
    for (sprite_counter = g_entities.num_sprites - 1; sprite_counter >= 0; sprite_counter--)
        if (g_entities.sprites[sprite_counter].type_ && g_entities.sprites[sprite_counter].active)
            CVort_engine_drawSpriteAt(g_entities.sprites[sprite_counter].pos_x, g_entities.sprites[sprite_counter].pos_y, g_entities.sprites[sprite_counter].frame);
}

#if CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
static void CVort_level_draw_invincibility(void) {
    if (engine_gameVersion == GAMEVER_KEEN3) {
        if (g_game.keen_invincible || g_game.god_mode) {
            if (g_game.keen_invincible > 250) {
                CVort_engine_drawSpriteAt(g_entities.sprites[0].pos_x - 0x800, g_entities.sprites[0].pos_y - 0x800, ((CVort_ptr_engine_getTicks() >> 4) & 1) + 0x3D);
            } else if ((CVort_ptr_engine_getTicks() >> 4) & 1) {
                CVort_engine_drawSpriteAt(g_entities.sprites[0].pos_x - 0x800, g_entities.sprites[0].pos_y - 0x800, ((CVort_ptr_engine_getTicks() >> 5) & 1) + 0x3D);
            }
            g_game.keen_invincible -= g_game.sprite_sync;
            if (g_game.keen_invincible < 0) {
                g_game.keen_invincible = 0;
            }
        }
    }
}
#endif

static void CVort_level_update_bodies(void) {
    int16_t sprite_counter;
    // Now call body "think" functions
    for (sprite_counter = 0; sprite_counter < g_entities.num_bodies; sprite_counter++)
        if (g_entities.bodies[sprite_counter].type_)
            (g_entities.bodies[sprite_counter].think_ptr)(&g_entities.bodies[sprite_counter]);
            //((*this).*(*this).g_entities.bodies[sprite_counter].think_ptr)(&g_entities.bodies[sprite_counter]);
}

static void CVort_level_handle_death(uint16_t levelnum) {
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
}

uint16_t CVort_draw_level(uint16_t levelnum) {
    int16_t about_to_encounter_mortimer;

    CVort_level_init();
    about_to_encounter_mortimer =
        engine_arguments.extras.vorticonsDemoModeToggle ? 0 : 1;
    CVort_ptr_init_level(levelnum);
    CVort_level_setup_camera();
    CVort_level_init_screen();

    // TEST TEST
    uint32_t frame_counter_start_time = SDL_GetTicks();

    if (engine_arguments.extras.vorticonsDemoModeToggle) {
        g_game.sprite_sync = 15;
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
        g_input.input_new = CVort_handle_ctrl(1);
        CVort_level_update_sprites();
        CVort_do_scrolling();
        CVort_level_detect_collisions();
        scroll_x_tile = scroll_x >> 12;
        scroll_y_tile = scroll_y >> 12;
        CVort_level_draw_sprites();
#if CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
        CVort_level_draw_invincibility();
#endif
        CVort_keen_bgtile_col();
        CVort_level_update_bodies();
        // Alright, let's update what is to be seen!
        CVort_engine_drawScreen();
        g_input.input_old = g_input.input_new;
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
        if (g_game.quit_to_title)
            return 0;
    } while ((g_game.level_finished == LEVEL_END_DIE) && (g_entities.sprites[0].type_));

    CVort_engine_finishCurSound();
    CVort_fade_out();
    for (int16_t i = 0; i < 4; i++)
        keen_gp.stuff[i + 5] = 0; // Set unused "stuff" vals to 0
    if (g_game.level_finished != LEVEL_END_DIE)
        return g_game.level_finished;

    CVort_level_handle_death(levelnum);
    return g_game.level_finished;
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
        if (g_input.key_map[1]) // ESC pressed
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
        if (g_input.key_map[1]) // ESC pressed
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
    g_input.joystick_ctrl[0][joy_id] = topLeftPollInput.xPoll + var_A;
    g_input.joystick_ctrl[1][joy_id] = bottomRightPollInput.xPoll - var_A;
    g_input.joystick_ctrl[2][joy_id] = topLeftPollInput.yPoll + var_C;
    g_input.joystick_ctrl[3][joy_id] = bottomRightPollInput.yPoll - var_C;
    g_input.ctrl_type[1] = 2;
    CVort_clear_keys();
}
