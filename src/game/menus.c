// menus.c: Implements menus for the game subsystem.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "core/globals.h"
#include "input/actions.h"
#include "core/io_helpers.h"
#include "core/path_helpers.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"
#include "third_party/cgenius/fileio/compression/CRLE.h"

#include "episodes/episode_macros.h"

static bool CVort_has_ui_action_press(void) {
    return CK_Action_IsHeld(CK_ACTION_MENU_CONFIRM) ||
           CK_Action_IsHeld(CK_ACTION_MENU_BACK) ||
           CK_Action_IsHeld(CK_ACTION_STATUS);
}

void CVort_demo_toggle_reset_player_partial_state_before();
void CVort_demo_toggle_reset_player_partial_state_after();

static bool CVort_private_read_saved_game_payload(FILE *fp) {
    bool loadOk = true;
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(keen_gp.stuff, 9, fp), 9);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(keen_gp.levels, 16, fp), 16);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(&keen_gp.lives, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(&keen_gp.ammo, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt32LE(&keen_gp.score, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt32LE(&keen_gp.mapX, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt32LE(&keen_gp.mapY, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt32LE(&keen_gp.screenX, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt32LE(&keen_gp.screenY, 1, fp), 1);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(keen_gp.targets, 8, fp), 8);
    CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(&keen_gp.unknown, 1, fp), 1);
    return loadOk;
}

static bool CVort_private_write_saved_game_payload(FILE *fp) {
    bool saveOk = true;
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(keen_gp.stuff, 9, fp), 9);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(keen_gp.levels, 16, fp), 16);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(&keen_gp.lives, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(&keen_gp.ammo, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt32LE(&keen_gp.score, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt32LE(&keen_gp.mapX, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt32LE(&keen_gp.mapY, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt32LE(&keen_gp.screenX, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt32LE(&keen_gp.screenY, 1, fp), 1);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(keen_gp.targets, 8, fp), 8);
    CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(&keen_gp.unknown, 1, fp), 1);
    return saveOk;
}

static bool CVort_private_read_high_scores_table(FILE *fp) {
    bool loadOk = true;
    int entryCounter, partCounter;
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt32LE(high_scores_table.scores + entryCounter, 1, fp), 1);
    for (partCounter = 0; partCounter < 4; partCounter++)
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(&high_scores_table.parts[entryCounter][partCounter], 1, fp), 1);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(high_scores_table.targets + entryCounter, 1, fp), 1);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(loadOk, CVort_engine_cross_freadInt16LE(high_scores_table.unknown + entryCounter, 1, fp), 1);
    for (entryCounter = 0; entryCounter < 7; entryCounter++) {
        if (fread(high_scores_table.names[entryCounter], sizeof(high_scores_table.names[entryCounter]), 1, fp) != 1) {
            loadOk = false;
            break;
        }
    }
    return loadOk;
}

static bool CVort_private_write_high_scores_table(FILE *fp) {
    bool saveOk = true;
    int entryCounter, partCounter;
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt32LE(high_scores_table.scores + entryCounter, 1, fp), 1);
    for (partCounter = 0; partCounter < 4; partCounter++)
        for (entryCounter = 0; entryCounter < 7; entryCounter++)
            CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(&high_scores_table.parts[entryCounter][partCounter], 1, fp), 1);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(high_scores_table.targets + entryCounter, 1, fp), 1);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(saveOk, CVort_engine_cross_fwriteInt16LE(high_scores_table.unknown + entryCounter, 1, fp), 1);
    for (entryCounter = 0; entryCounter < 7; entryCounter++)
        CK_IO_EXPECT(saveOk, fwrite(high_scores_table.names[entryCounter], sizeof(high_scores_table.names[entryCounter]), 1, fp), 1);
    return saveOk;
}

void CVort_record_demo(int16_t demo_number) {
    demo_actions_including_level_num[0] = g_game.current_level;
    demo_action_ptr = 1 + demo_actions_including_level_num;
    g_game.demo_status = DEMO_RECORD;
}

void CVort_load_demo(int16_t demo_number) {
    FILE *fp;
    CK_Path_FormatDemo(g_game.string_buf, sizeof(g_game.string_buf), demo_number, game_ext);
    fp = CVort_engine_cross_rw_misc_fopen(g_game.string_buf, "rb");
    if (!fp) {
        CK_IO_WARN_ON_OPEN_FAIL_FOR_READ(g_game.string_buf);
        return;
    }
    uint32_t filesize = CVort_filelength(fp);
    bool loadOk = (fread(demo_actions_including_level_num, filesize, 1, fp) == 1);
    if (!loadOk) {
        fclose(fp);
        CK_IO_WARN_ON_READ_FAIL(loadOk, g_game.string_buf);
        return;
    }
    fclose(fp);
    g_game.current_level = demo_actions_including_level_num[0];
    demo_action_ptr = 1 + demo_actions_including_level_num;
    g_game.demo_status = DEMO_PLAY;
}

void CVort_save_demo(int16_t demo_number) {
    FILE *fp;
    CK_Path_FormatDemo(g_game.string_buf, sizeof(g_game.string_buf), demo_number, game_ext);
    fp = CVort_engine_cross_rw_misc_fopen(g_game.string_buf, "wb");
    CK_IO_RETURN_IF_OPEN_FAIL_FOR_WRITE(fp, g_game.string_buf);
    bool saveOk = true;
    CK_IO_EXPECT(saveOk, fwrite(demo_actions_including_level_num, demo_action_ptr-demo_actions_including_level_num, 1, fp), 1);
    fclose(fp);
    CK_IO_WARN_ON_WRITE_FAIL(saveOk, g_game.string_buf);
    g_game.demo_status = DEMO_OFF;
}

void CVort_do_intro_and_menu() {
    int16_t introTickCounter;
    uint16_t introCurrScreen;
    GameInput_T currInput;
    bool doHalt;
    FILE *fp;
    g_game.resuming_saved_game = 0;
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
        g_game.anim_speed = 3;
        CVort_engine_clearOverlay();
        CVort_load_level_data(90);
        // Show Apogee logo
        if (g_game.reshow_scroll_up) // Uncomment this to replay scroll up upon demo restart
        {
            CVort_load_level_data(90); // NOTE: Again???
            scroll_x = 0x68000;
            scroll_y = 0x2000;
            CVort_engine_clearOverlay();
            CVort_engine_syncDrawing();
            CVort_engine_drawScreen();
            CVort_fade_in();
            introTickCounter = 2400;
            introCurrScreen = 4;
            CVort_scroll_up_logo();
            CVort_fade_out();
            g_game.reshow_scroll_up = 0; // Uncomment this line to replay scroll up
        }
        // Fade into menu
        //introTickCounter = 0xC0;
        introTickCounter = engine_arguments.extras.vorticonsDemoModeToggle ? 146 : 2400;
        introCurrScreen = 0;
        scroll_x = scroll_y = 0x2000;
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
                introTickCounter -= g_game.sprite_sync;
                if (introTickCounter <= 0) {
                    for (; introCurrScreen < 10; introCurrScreen++) {
                        CK_Path_FormatDemo(g_game.string_buf, sizeof(g_game.string_buf), introCurrScreen, game_ext);
                        fp = CVort_engine_cross_rw_misc_fopen(g_game.string_buf, "rb");
                        if (!fp) {
                            continue;
                        }
                        end_of_demo_ptr = demo_actions_including_level_num + CVort_filelength(fp);
                        fclose(fp);
                        CVort_load_demo(introCurrScreen);
                        g_game.quit_to_title = 0;
                        g_game.rnd = 0;
                        CVort_setup_jump_heights(0);
                        memset(&g_input.input_old, 0, sizeof(g_input.input_old));
                        CVort_demo_toggle_reset_player_partial_state_before();
                        CVort_draw_level(g_game.current_level);
                        g_game.demo_status = DEMO_OFF;
                        CVort_demo_toggle_reset_player_partial_state_after();
                        // 1 - player halt. 2 - Demo halt (may also be 0).
                        if (g_game.quit_to_title == 1) {
                            break;
                        }
                    }
                    introCurrScreen = 0;
                    CVort_fade_out();
                    CVort_load_level_data(90);
                    introTickCounter = 146;
                    CVort_engine_syncDrawing();
                    CVort_engine_drawScreen();
                    scroll_x = scroll_y = 0x2000;
                    CVort_engine_clearOverlay();
                    CVort_do_draw_mural();
                    CVort_fade_in();
                    // PLAYER halt? (NOT demo, i.e. 2.)
                    if (g_game.quit_to_title == 1) {
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
                introTickCounter -= g_game.sprite_sync;
                if (introTickCounter <= 0) {
                    CVort_fade_out();
                    switch (introCurrScreen) {
                        case 0:
                            scroll_x = 0x3F000;
                            scroll_y = 0x1C00;
                            CVort_do_about_us();
                            doHalt = false;
                            break;
                        case 1:
                            scroll_x = 0x54000;
                            scroll_y = 0x2000;
                            CVort_do_scores();
                            doHalt = false;
                            break;
                        case 2:
                            introCurrScreen++;
                            CVort_ptr_show_ordering(1);
                            currInput = CVort_handle_ctrl(1);
                            doHalt = (CVort_has_ui_action_press() || CVort_translate_key(1) || currInput.but1jump || currInput.but2pogo);
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
                            scroll_x = scroll_y = 0x2000;
                            CVort_engine_clearOverlay();
                            CVort_do_draw_mural();
                        }
                        CVort_fade_in();
                    }
                }
            }
            if (CVort_has_ui_action_press() || CVort_translate_key(1) || currInput.but1jump || currInput.but2pogo || g_game.intro_complete)
                break;
        }
        g_game.intro_complete = 0;
        if ((scroll_x >> 16) || ((scroll_x & 0xFFFF) != 0x2000))
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

NOTE: What would occur if the host video card syncs to vertical retrace at a
rate of less than 70Hz? Well, slowdowns are expected...                       */
uint16_t CVort_draw_title() {
    uint16_t sel, bg, var_6, var_8;
    int16_t loopVar, currCharOffset;
    GameInput_T input;
loc_19297:
    if ((scroll_x >> 16) || ((scroll_x & 0xFFFF) != 0x2000)) {
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
    if (var_6 && g_input.ctrl_type[1])
        input.but1jump = 1;
    if ((!g_input.ctrl_type[1]) &&
        ((var_6 == 0x20) || (var_6 == 0xD) ||
         CK_Action_IsHeld(CK_ACTION_STATUS) ||
         CK_Action_IsHeld(CK_ACTION_MENU_CONFIRM)))
        input.but1jump = 1;
    if (CVort_handle_global_keys()) {
        CVort_do_start_menu();
        cursorY = sel + cursorY_b;
    }
    if (!input.but1jump && !input.but2pogo)
        goto loc_192C0; // NOTE NOTE NOTE NOTE NOTE NOTE GOTO?!!!

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
        goto loc_19297; // NOTE NOTE NOTE NOTE NOTE NOTE NOTE
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

    switch (g_input.ctrl_type[1]) {
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
    scroll_x = scroll_y = 0x2000;
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

        scroll_x = 0x54000;
    }
}

void CVort_show_about_us() {
    GameInput_T input;
    scroll_x = 0x3F000;
    scroll_y = 0x1C00;
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
        // NOTE: What should be done here (i.e. like in vanilla code)???
        // For now check for jump or pogo control action.
        // Also check for general keyboard key presses.
        if (input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1))
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
    scroll_x = 0x54000;
    scroll_y = 0x2000;
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
        // NOTE: What should be done here (i.e. like in vanilla code)???
        // For now check for jump or pogo control action.
        // Also check for general keyboard key presses.
        if (input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1))
            return;
    } while (1);
}

void CVort_save_game() {
    char path[13];
    FILE *fp;
    int8_t inputChar, confirmChar;
    // NOTE: Quite hackish but... more true to the original?
    CK_Path_FormatSavedSlot(path, sizeof(path), '?', game_ext);
    if (!g_game.on_world_map) {
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
        // NOTE: Seems a bit hackish but... well...
        CK_Path_FormatSavedSlot(path, sizeof(path), inputChar, game_ext);
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

    keen_gp.screenX = wmap_scroll_x;
    keen_gp.screenY = wmap_scroll_y;
    keen_gp.mapX = keen_wmap_x_pos;
    keen_gp.mapY = keen_wmap_y_pos;

    fp = CVort_engine_cross_rw_misc_fopen(path, "wb");
    CK_IO_RETURN_IF_OPEN_FAIL_FOR_WRITE(fp, path);
    bool saveOk = CVort_private_write_saved_game_payload(fp);
    fclose(fp);
    CK_IO_WARN_ON_WRITE_FAIL(saveOk, path);

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
    // NOTE: Again hackish but... same as CVort_save_game()...
    // Possibly a bit less vanilla, but actually works the proper
    // (and vanilla) way!
    CK_Path_FormatSavedSlot(path, sizeof(path), '?', game_ext);
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
        // NOTE: Hackish just like in CVort_save_game()...
        CK_Path_FormatSavedSlot(path, sizeof(path), inputChar, game_ext);
        // NOTE NOTE!!! Vanilla behavior concatenates the file
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
            bool loadOk = CVort_private_read_saved_game_payload(fp);
            fclose(fp);
            if (!loadOk)
                continue;
            g_game.resuming_saved_game = 1;
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
	g_game.current_level = atoi(demostr);
	CVort_record_demo(g_game.current_level);
	g_game.rnd = 0;
	CVort_setup_jump_heights(0);
	memset(&g_input.input_old, 0, sizeof(g_input.input_old));
	CVort_demo_toggle_reset_player_partial_state_before();
	g_game.quit_to_title = 0;
	CVort_draw_level(g_game.current_level);
	CVort_fade_in();
	CVort_clear_keys();
	CVort_draw_box_opening_main(0x17, 1);
	CVort_draw_string("Save as demo #(0-9): ");
	uint16_t inputChar = CVort_read_char_with_echo() & 0xFF;
	if ((inputChar >= '0') && (inputChar <= '9')) {
		CVort_save_demo(inputChar - '0');
	}
	g_game.demo_status = DEMO_OFF;
	CVort_fade_out();
	// WORKAROUND to force fade_in to the menu
	scroll_x = 0x1000;
	// Load menu level
	CVort_load_level_data(90);
	CVort_demo_toggle_reset_player_partial_state_after();
	return 0;
}

void CVort_demo_toggle_reset_player_partial_state_before() {
	keen_gp.score = 0;
	g_game.extra_life_pts = 0;
	/* When a certain sprite is added to the level, like a creature,
	 * it is checked if the player is located somewhere. Problem is
	 * that the player may have not yet been founded...
	 */
	g_entities.sprites[0].pos_x = 0;
	g_entities.sprites[0].pos_y = 0;
}

void CVort_demo_toggle_reset_player_partial_state_after() {
	g_game.resuming_saved_game = 0;
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
    scroll_x = 0x2A000;
    scroll_y = 0x2000;
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_fade_in();
    CVort_do_text_viewer(story_text, 0, 0x10);
    CVort_fade_out();
}

void CVort_do_help() {
    int32_t origScrollX = scroll_x, origScrollY = scroll_y;
    // Move to some other random place...?
    scroll_x &= 0xFFFFF000;
    scroll_y &= 0xFFFFF000;
    CVort_do_text_viewer(help_text, 1, 0x14);
    scroll_x = origScrollX;
    scroll_y = origScrollY;
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
    CVort_draw_string_sel(1, 5, text_viewer_bottom + 1, "       ESC to Exit /   to Read      ");
    CVort_text_viewer_bkgrd(g_game.apogee_bmp_height, text_viewer_top_pos + 1, text_ptr, text_viewer_buffer, text_viewer_height);
}


// This is just used to quickly draw the start of the text scroller
void CVort_do_text_viewer_short(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset) {

    int height =  bottom_line_offset - top_line_offset - 1;
    uint16_t tp = CVort_draw_text_page(text, text_viewer_buffer, 0x26, 0xC8);
    g_game.apogee_bmp_height = 5;
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
    g_game.apogee_bmp_height = 5;
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
        if (g_input.key_map[0x48] || (in.direction == 0)) // Up
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
        } else if (g_input.key_map[0x50] || (in.direction == 4)) // Down
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
        if (CK_Action_IsHeld(CK_ACTION_PAGE_UP) || g_input.key_map[0x49]) // Page Up
        {
            if (currRow - height + 1 <= 0)
                currRow = 0;
            else
                currRow -= height - 1;
            CVort_text_viewer_bkgrd(5, top_line_offset + 1, text, (currRow << 1) + text_viewer_buffer, height);
            do {
                CVort_engine_shortSleep();
                //engine_updateActualDisplay();
            } while (CK_Action_IsHeld(CK_ACTION_PAGE_UP) || g_input.key_map[0x49]);
        }
        if (CK_Action_IsHeld(CK_ACTION_PAGE_DOWN) || g_input.key_map[0x51]) // Page Down
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
            } while (CK_Action_IsHeld(CK_ACTION_PAGE_DOWN) || g_input.key_map[0x51]);
        }
    } while (!((g_input.key_map[1] || CK_Action_IsHeld(CK_ACTION_MENU_BACK)) || in.but1jump || in.but2pogo));

    CVort_waitForNoGameButtonPress(&in);
    CVort_clear_keys();
}

void CVort_draw_apogee() {
    if (engine_gameVersion == GAMEVER_KEEN1) {
        CVort_engine_drawBitmap(0x10, g_game.apogee_bmp_height, CVort1_bmp_apogee);
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        CVort_engine_drawBitmap(0x10, g_game.apogee_bmp_height, CVort2_bmp_apogee);
    } else if (engine_gameVersion == GAMEVER_KEEN3) {
        CVort_engine_drawBitmap(0x10, g_game.apogee_bmp_height, CVort3_bmp_apogee);
    }
}

void CVort_show_logo_text() {
    CVort_engine_drawBitmap(0x16, g_game.apogee_bmp_height - 0xa, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_an : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_an : CVort3_bmp_an ); // An
    CVort_engine_drawBitmap(0x10, g_game.apogee_bmp_height, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_apogee : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_apogee : CVort3_bmp_apogee ); // Apogee
    CVort_engine_drawBitmap(0x12, g_game.apogee_bmp_height + 0x20, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_present : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_present : CVort3_bmp_present ); // presentation
    CVort_engine_drawBitmap(0x15, 0x63, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_ofan : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_ofan : CVort3_bmp_ofan ); // of an
    CVort_engine_drawBitmap(0x13, 0x72,engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_idsoft : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_idsoft : CVort3_bmp_idsoft ); // ID Software
    CVort_engine_drawBitmap(0x13, 0x9f, engine_gameVersion == GAMEVER_KEEN1? CVort1_bmp_product : engine_gameVersion == GAMEVER_KEEN2? CVort2_bmp_product : CVort3_bmp_product ); // production
}

void CVort_scroll_up_logo() {
    scroll_x = 0x68000;
    scroll_y = 0x2000;
    uint16_t bmpHeight = 200, apogee = 0, currState = 0;
    CVort_engine_clearOverlay();
    CVort_clear_keys();
    g_game.intro_complete = 0;
    GameInput_T currInput;
    for (uint16_t timeLeft = 300; timeLeft > 0; timeLeft--) {
        CVort_engine_syncDrawing();
        g_game.apogee_bmp_height = bmpHeight;
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
        if (currInput.but1jump || currInput.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1)) {
            g_game.intro_complete = 1;
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

void CVort_wait_for_key() {
    GameInput_T input;
    int16_t currCharOffset = 0, isKeyPressed = 0, delayLoopVar;
    do {
        CVort_engine_drawChar(cursorX, cursorY << 3, currCharOffset + 9);
        //engine_updateActualDisplay();
        for (delayLoopVar = 0; delayLoopVar < 6; delayLoopVar++) {
            CVort_engine_delay(1);
            input = CVort_handle_ctrl(1);
            if (input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1)) {
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

void CVort_process_text_file(uint8_t *buffer) {
    // Now process text file.
    // NOTE: Is this the correct way?
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
                NOTE? Vanilla code does this but with a total
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
    if (!(*pBuffer) || (fread(*pBuffer, len, 1, fp) != 1)) {
        if (*pBuffer) {
            free(*pBuffer);
            *pBuffer = NULL;
        }
        fclose(fp);
        CVort_chg_vid_and_error("Failed to read a text file!");
    }
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
    // NOTE: This is a wrong way to do it, but...... more vanilla!
    static char default_names[][15] = {"Yorpy", "Gargile", "Zzapp!"};
    char path[14];
    CK_Path_FormatScores(path, sizeof(path), game_ext);

    int entryCounter, partCounter;

    FILE *fp = CVort_engine_cross_rw_misc_fopen(path, "rb");
    if (fp) {
        bool loadOk = CVort_private_read_high_scores_table(fp);
        fclose(fp);
        if (loadOk)
            return;
        CK_IO_WARN_ON_READ_FAIL(loadOk, path);
    }
    {
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
    // NOTE: This is a wrong way to do it, but...... more vanilla!
    // Or rather, vanilla on the *loading* side...
    char path[14];
    CK_Path_FormatScores(path, sizeof(path), game_ext);

    FILE *fp = CVort_engine_cross_rw_misc_fopen(path, "wb");
    CK_IO_RETURN_IF_OPEN_FAIL_FOR_WRITE(fp, path);

    bool saveOk = CVort_private_write_high_scores_table(fp);
    fclose(fp);
    CK_IO_WARN_ON_WRITE_FAIL(saveOk, path);
}
