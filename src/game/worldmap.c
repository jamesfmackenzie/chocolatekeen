// worldmap.c: Implements worldmap for the game subsystem.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "core/globals.h"
#include "input/actions.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"
#include "third_party/cgenius/fileio/compression/CRLE.h"

#include "episodes/episode_macros.h"

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
        for (var4 = 0; var4 < map_height_tile; var4++)
        {
            if (map_data_sprites[(map_width_tile*var4)+map_width_tile/2] == 0x2000)
            {
                messie_mounted = messie_time_to_climb = 0;
                messie_x_T = map_width_tile/2;
                messie_y_T = var4;
                messie_del_x = -0x200;
                messie_del_y = 0;
                messie_xpos = messie_x_T << 12;
                messie_ypos = messie_y_T << 12;
                messie_frame = CVort3_spr_messield1;
                messie_move_tics = 8;
		break;
            }
        }

    }

    if (!g_game.resuming_saved_game) {
        CVort_place_keen_on_worldmap(&keen_gp.mapX, &keen_gp.mapY);
        // 32-bit additions (instead of two 16-bit ones with carry bit involved)
        keen_gp.screenX = keen_gp.mapX + 0xFFFF7000;
        keen_gp.screenY = keen_gp.mapY + 0xFFFFD000;
        keen_gp.score = 0;
    }
    wmap_scroll_x = keen_gp.screenX;
    wmap_scroll_y = keen_gp.screenY;
    keen_wmap_x_pos = keen_gp.mapX;
    keen_wmap_y_pos = keen_gp.mapY;
    g_game.wmap_col = 0x8000;
    g_game.extra_life_pts = (keen_gp.score / 20000)*20000;
    g_game.quit_to_title = 0;
    engine_currPage = 0;
    g_game.anim_speed = 3;
    do // World map loop
    {
        keen_sp = g_entities.sprites;
        keen_sp->del_x = keen_sp->del_y = keen_sp->vel_x = keen_sp->vel_y = 0;
        if (engine_gameVersion == GAMEVER_KEEN1)
          keen_sp->frame = 0x24;//spr_mapkeend1;
        else if (engine_gameVersion == GAMEVER_KEEN2)
          keen_sp->frame = 0x24;//spr_mapkeend1;
        else if (engine_gameVersion == GAMEVER_KEEN3)
          keen_sp->frame = CVort3_spr_mapkeend1;//spr_mapkeend1;
        g_game.wmap_sprite_on = 0;
        CVort_load_level_data(80);
        CVort_mark_cities_done();

        for (count_i = 0; count_i < 0x10; count_i++) {
            // NOTE? Original code takes bx=count_i*6 and then
            // accesses wmaplevels.tx[bx].
            x_coord = wmaplevels[count_i].tx;
            y_coord = wmaplevels[count_i].ty;
            if (keen_gp.levels[count_i]) // Mark completed level?
            {
                // 1x1 sized entry
                if (wmaplevels[count_i].type_ == 0) {
                    map_data_sprites[y_coord * map_width_tile + x_coord] = 0;
                    if (engine_gameVersion == GAMEVER_KEEN3) {
                        map_data_tiles[y_coord * map_width_tile + x_coord] = 0x38;
                    } else {
                        map_data_tiles[y_coord * map_width_tile + x_coord] = 0x4D;
                    }
                }// 2x2 sized entry
                else if (wmaplevels[count_i].type_ == 1) {
                    map_data_sprites[y_coord * map_width_tile + x_coord] = 0;
                    map_data_sprites[y_coord * map_width_tile + x_coord + 1] = 0;
                    map_data_sprites[(y_coord + 1) * map_width_tile + x_coord] = 0;
                    map_data_sprites[(y_coord + 1) * map_width_tile + x_coord + 1] = 0;
                    if (engine_gameVersion == GAMEVER_KEEN3) {
                        map_data_tiles[y_coord * map_width_tile + x_coord] = 0x34;
                        map_data_tiles[y_coord * map_width_tile + x_coord + 1] = 0x35;
                        map_data_tiles[(y_coord + 1) * map_width_tile + x_coord] = 0x36;
                        map_data_tiles[(y_coord + 1) * map_width_tile + x_coord + 1] = 0x37;
                    } else {
                        map_data_tiles[y_coord * map_width_tile + x_coord] = 0x4E;
                        map_data_tiles[y_coord * map_width_tile + x_coord + 1] = 0x4F;
                        map_data_tiles[(y_coord + 1) * map_width_tile + x_coord] = 0x50;
                        map_data_tiles[(y_coord + 1) * map_width_tile + x_coord + 1] = 0x51;
                    }
                }
            }
        }
        keen_sp->pos_x = keen_wmap_x_pos;
        keen_sp->pos_y = keen_wmap_y_pos;
        scroll_x = wmap_scroll_x;
        scroll_y = wmap_scroll_y;
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
                if (input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1))
                    break;
            }
            CVort_waitForNoGameButtonPress(&input);
        }
        CVort_clear_keys();
        CVort_engine_clearOverlay();
        g_game.on_world_map = 1;

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
            keen_wmap_x_pos = keen_sp->pos_x;
            keen_wmap_y_pos = keen_sp->pos_y;
            wmap_scroll_x = scroll_x;
            wmap_scroll_y = scroll_y;
            CVort_handle_global_keys();
            if (CK_Action_IsHeld(CK_ACTION_STATUS) || g_input.key_map[0x39])
                CVort_ptr_show_pause_menu();
            if (g_game.quit_to_title) {
                CVort_game_over();
                return;
            }
            y_coord++;  // This probably was not ycoord, but some new stack variable that was mistakenly not initialized
        } while (!g_game.wmap_sprite_on);

        g_game.on_world_map = 0;
        /* The original code takes g_game.wmap_sprite_on, a 16-bit value,
        shifts by 1 bit to the left and then uses this to access
        element number g_game.wmap_sprite_on in the array.
        With the G-O-D cheat it is easy to enter a place where 0x8000
        is added to the level number (in unsigned form). For instance,
        without the cheat g_game.wmap_sprite_on may be 7 for some level, but
        once you try to access the same level from a neighboring tile
        it can really become 0x8007.
        The left shift by 1 practically means there is no difference
        between 7 and 0x8007. So, we try to simulate that here.

        How? Well, we remove the most significant bit,
        or "and" with 0x7FFF.                                       */
        if (!keen_gp.levels[(g_game.wmap_sprite_on & 0x7FFF) - 1]) {
            CVort_engine_setCurSound(3);
            level_return_code = CVort_draw_level(g_game.wmap_sprite_on & 0xFF);
            if (g_game.quit_to_title) {
                g_game.quit_to_title = 0;
                CVort_game_over();
                return;
            }
            switch (level_return_code) {
                case LEVEL_END_DIE:
                    keen_gp.lives--;
                    continue; // Skip to the very end of the loop
                case LEVEL_END_EXIT:
                    keen_gp.levels[(g_game.wmap_sprite_on & 0x7FFF) - 1] = 1;
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
                if ((g_game.wmap_sprite_on & 0xFF) == 16) {
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
    for (int16_t pos_y = 0, pos_x; pos_y < map_height_tile; pos_y++)
        for (pos_x = 0; pos_x < map_width_tile; pos_x++) {
            mapEntry = map_data_sprites[pos_y * map_width_tile + pos_x]&0x7FFF;

            if (engine_gameVersion == GAMEVER_KEEN3)
            {
                // Check for teleporter
                if ((mapEntry&0xF00) == 0xF00)
                {
                    teleIndex = (mapEntry&0xF0)/16;
                    teleporters[teleIndex].isOnSnow = mapEntry&0xF; // Record the target tele
                    teleporters[teleIndex].destX = pos_x;
                    teleporters[teleIndex].destY = pos_y;
                }
            }
            /* NOTE? For mapEntry>0x10 this results in erroneous
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
            wmaplevels[mapEntry - 1].tx = pos_x;
            wmaplevels[mapEntry - 1].ty = pos_y;
            wmaplevels[mapEntry - 1].type_ = 0;

            if (mapEntry != (map_data_sprites[pos_y * map_width_tile + pos_x + 1]&0x7FFF))
                continue;
            pos_x++;
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
    snprintf(score_file, sizeof(score_file), "SCORES.%s", game_ext);
    int16_t scoreEntry = 0, loopVar;
    // Check if we have a new high score
    for (loopVar = 0; loopVar < 7; loopVar++)
        if (high_scores_table.scores[loopVar] < keen_gp.score) {
            scoreEntry = loopVar + 1;
            break;
        }
    scroll_x &= 0xFFFFF000;
    scroll_y &= 0xFFFFF000;
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
        snprintf(scorestr, sizeof(scorestr), "%" PRId32, keen_gp.score);
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
            snprintf(citystr, sizeof(citystr), "%" PRIu16, citiesSaved);
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
            strncpy(high_scores_table.names[loopVar + 1], high_scores_table.names[loopVar], sizeof(high_scores_table.names[loopVar]));
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
            if (input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1))
                break;
        }
    }
    CVort_fade_out();
    CVort_clear_keys();
    CVort_load_level_data(90);
    CVort_do_scores();
    CVort_fade_in();
    int16_t timeLeft = 2400;
    g_game.on_world_map = 0;
    do {
        CVort_engine_syncDrawing();
        CVort_engine_drawScreen();
        input = CVort_handle_ctrl(1);
        if (CVort_handle_global_keys())
            CVort_do_scores();
        timeLeft -= g_game.sprite_sync;
        if (input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1))
            break;
    } while (!(input.but1jump || input.but2pogo || CVort_has_ui_action_press() || CVort_translate_key(1))
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

void CVort_place_keen_on_worldmap(uint32_t *pos_x, uint32_t *pos_y) {
    int16_t mapEntry;
    for (int16_t currY = 0, currX; currY < map_height_tile; currY++)
        for (currX = 0; currX < map_width_tile; currX++) {
            mapEntry = map_data_sprites[currY * map_width_tile + currX];
            if (mapEntry != 0xFF)
                continue;
            *pos_x = ((int32_t)(currX)) << 12;
            *pos_y = ((int32_t)(currY)) << 12;
            return;
        }
}

int16_t CVort_check_world_map_col(Sprite_T *sprite) {
    if (g_game.god_mode)
        return 0;
    sprite->del_x += sprite->vel_x*g_game.sprite_sync;
    sprite->del_y += sprite->vel_y*g_game.sprite_sync;
    keen_map[0] = *sprite;
    int16_t blocking = 0;
    int16_t x1_tile, x2_tile, y1_tile, y2_tile;

    // First we check the vertical move, if any
    keen_map[0].box_y2 += keen_map[0].del_y;
    keen_map[0].box_y1 += keen_map[0].del_y;
    x1_tile = keen_map[0].box_x1 / 0x1000;
    x2_tile = keen_map[0].box_x2 / 0x1000;

    int16_t tempNum, x, y;

    if (keen_map[0].del_y > 0) // Down
    {
        if (keen_map[0].box_y2 / 0x1000 != (keen_map[0].box_y2 - keen_map[0].del_y) / 0x1000) {
            y2_tile = keen_map[0].box_y2 / 0x1000;
            for (x = x1_tile; x <= x2_tile; x++) {
                if (!TILEINFO_UEdge[map_data_tiles[y2_tile * map_width_tile + x]]) {
                    if (!(map_data_sprites[y2_tile * map_width_tile + x] & g_game.wmap_col))
                        continue;
                }
                sprite->vel_y = 0;
                tempNum = (keen_map[0].box_y2 + 1) % 0x1000;
                sprite->del_y -= tempNum;
                keen_map[0].box_y1 -= tempNum;
                keen_map[0].box_y2 -= tempNum;
                blocking = 1;
                break;
            }
        }
    } else if (keen_map[0].del_y < 0) // Up
    {
        if (keen_map[0].box_y1 / 0x1000 != (keen_map[0].box_y1 - keen_map[0].del_y) / 0x1000) {
            y1_tile = keen_map[0].box_y1 / 0x1000;
            for (x = x1_tile; x <= x2_tile; x++) {
                if (!TILEINFO_DEdge[map_data_tiles[y1_tile * map_width_tile + x]]) {
                    if (!(map_data_sprites[y1_tile * map_width_tile + x] & g_game.wmap_col))
                        continue;
                }
                sprite->vel_y = 0;
                tempNum = 0x1000 - keen_map[0].box_y1 % 0x1000;
                sprite->del_y += tempNum;
                keen_map[0].box_y1 += tempNum;
                keen_map[0].box_y2 += tempNum;
                blocking = 1;
                break;
            }
        }
    }

    // Now horizontal
    keen_map[0].box_x1 += keen_map[0].del_x;
    keen_map[0].box_x2 += keen_map[0].del_x;
    y1_tile = keen_map[0].box_y1 / 0x1000;
    y2_tile = keen_map[0].box_y2 / 0x1000;

    if (keen_map[0].del_x > 0) // Right
    {
        if (keen_map[0].box_x2 / 0x1000 != (keen_map[0].box_x2 - keen_map[0].del_x) / 0x1000) {
            x2_tile = keen_map[0].box_x2 / 0x1000;
            for (y = y1_tile; y <= y2_tile; y++) {
                if (!TILEINFO_LEdge[map_data_tiles[y * map_width_tile + x2_tile]]) {
                    if (!(map_data_sprites[y * map_width_tile + x2_tile] & g_game.wmap_col))
                        continue;
                }
                sprite->vel_x = 0;
                tempNum = (keen_map[0].box_x2 + 1) % 0x1000;
                sprite->del_x -= tempNum;
                keen_map[0].box_x1 -= tempNum;
                keen_map[0].box_x2 -= tempNum;
                blocking = 1;
                break;
            }
        }
    } else if (keen_map[0].del_x < 0) // Left
    {
        if (keen_map[0].box_x1 / 0x1000 != (keen_map[0].box_x1 - keen_map[0].del_x) / 0x1000) {
            x1_tile = keen_map[0].box_x1 / 0x1000;
            for (y = y1_tile; y <= y2_tile; y++) {
                if (!TILEINFO_REdge[map_data_tiles[y * map_width_tile + x1_tile]]) {
                    if (!(map_data_sprites[y * map_width_tile + x1_tile] & g_game.wmap_col))
                        continue;
                }
                sprite->vel_x = 0;
                tempNum = 0x1000 - keen_map[0].box_x1 % 0x1000;
                sprite->del_x += tempNum;
                keen_map[0].box_x1 += tempNum;
                keen_map[0].box_x2 += tempNum;
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
        x1 = spritedraw->box_x1 / 0x1000;
        x2 = spritedraw->box_x2 / 0x1000;
        y1 = spritedraw->box_y1 / 0x1000;
        y2 = spritedraw->box_y2 / 0x1000;
        for (x = x1; x <= x2; x++)
            for (y = y1; y <= y2; y++) {
                if (!map_data_sprites[y * map_width_tile + x])
                    continue;
                sprite_x = x;
                sprite_y = y;
                g_game.wmap_sprite_on = map_data_sprites[y * map_width_tile + x];
                if (g_game.wmap_sprite_on == 0xFF)
                    g_game.wmap_sprite_on = 0;
            }
    }
    spritedraw->del_x = spritedraw->del_y = 0;
    switch (input.direction) {
        case 7:
            spritedraw->del_x = spritedraw->del_y = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenu1;
            break;
        case 0:
            spritedraw->del_y = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenu1;
            break;
        case 1:
            spritedraw->del_y = -0x400;
            spritedraw->del_x = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x2C;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenu1;
            break;
        case 2:
            spritedraw->del_x = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x20;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x20;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeenr1;
            break;
        case 3:
            spritedraw->del_y = spritedraw->del_x = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeend1;
            break;
        case 4:
            spritedraw->del_y = 0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeend1;
            break;
        case 5:
            spritedraw->del_y = 0x400;
            spritedraw->del_x = -0x400;
            if (engine_gameVersion == GAMEVER_KEEN1)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN2)
              spritedraw->frame = 0x24;
            if (engine_gameVersion == GAMEVER_KEEN3)
              spritedraw->frame = CVort3_spr_mapkeend1;
            break;
        case 6:
            spritedraw->del_x = -0x400;
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
    if (spritedraw->del_x | spritedraw->del_y) {
        fr = ((CVort_ptr_engine_getTicks()&0xFFFF) >> 4)&3;
        s++;
    } else
        fr = 0;
    g_game.wmap_col = 0x8000;
    if (g_input.key_map[0xF] && g_input.key_map[0x2A])
        g_game.wmap_col = 0;
    csd = CVort_check_world_map_col(spritedraw);
    spritedraw->pos_x += spritedraw->del_x;
    spritedraw->pos_y += spritedraw->del_y;

    if (s && !(((CVort_ptr_engine_getTicks()&0xFFFF) >> 3)&3)) {
        if (csd)
            CVort_engine_setCurSound(2);
        else
            CVort_engine_setCurSound(1);
    }

    // Now we set the scrolling viewport
    //NOTE NOTE NOTE NOTE: Gotos?!!!!!!!
    //NOTE: Several of optimizations can be applied here, apparently.

    int32_t tempDiff;

scroll_x:
    if (spritedraw->del_x <= 0)
        goto scroll_x_lo;
    tempDiff = spritedraw->pos_x - scroll_x;
    if ((tempDiff >> 16) < 0)
        goto scroll_x_lo;
    if ((tempDiff >> 16) > 0)
        goto scroll_x_hi_fix;
    if ((tempDiff & 0xFFFF) <= 0xB000)
        goto scroll_x_lo;
scroll_x_hi_fix:
    scroll_x += spritedraw->del_x;
    if ((scroll_x >> 16) < (scroll_x_max >> 16))
        goto scroll_y;
    if ((scroll_x >> 16) > (scroll_x_max >> 16))
        goto scroll_x_hi_fix_limit;
    if ((scroll_x & 0xFFFF) <= (scroll_x_max & 0xFFFF))
        goto scroll_y;
scroll_x_hi_fix_limit:
    scroll_x = scroll_x_max;
    goto scroll_y;
scroll_x_lo:
    if (spritedraw->del_x >= 0)
        goto scroll_y;
    tempDiff = spritedraw->pos_x - scroll_x;
    if ((tempDiff >> 16) > 0)
        goto scroll_y;
    if ((tempDiff >> 16) < 0)
        goto scroll_x_lo_fix;
    if ((tempDiff & 0xFFFF) >= 0x9000)
        goto scroll_y;
scroll_x_lo_fix:
    scroll_x += spritedraw->del_x;
    if ((scroll_x >> 16) > (scroll_x_min >> 16))
        goto scroll_y;
    if ((scroll_x >> 16) < (scroll_x_min >> 16))
        goto scroll_x_lo_fix_limit;
    if ((scroll_x & 0xFFFF) >= (scroll_x_min & 0xFFFF))
        goto scroll_y;
scroll_x_lo_fix_limit:
    scroll_x = scroll_x_min;

scroll_y:
    if (spritedraw->del_y <= 0)
        goto scroll_y_lo;
    tempDiff = spritedraw->pos_y - scroll_y;
    if ((tempDiff >> 16) < 0)
        goto scroll_y_lo;
    if ((tempDiff >> 16) > 0)
        goto scroll_y_hi_fix;
    if ((tempDiff & 0xFFFF) <= 0x7000)
        goto scroll_y_lo;
scroll_y_hi_fix:
    scroll_y += spritedraw->del_y;
    if ((scroll_y >> 16) < (scroll_y_max >> 16))
        goto scroll_tiles;
    if ((scroll_y >> 16) > (scroll_y_max >> 16))
        goto scroll_y_hi_fix_limit;
    if ((scroll_y & 0xFFFF) <= (scroll_y_max & 0xFFFF))
        goto scroll_tiles;
scroll_y_hi_fix_limit:
    scroll_y = scroll_y_max;
    goto scroll_tiles;
scroll_y_lo:
    if (spritedraw->del_y >= 0)
        goto scroll_tiles;
    tempDiff = spritedraw->pos_y - scroll_y;
    if ((tempDiff >> 16) > 0)
        goto scroll_tiles;
    if ((tempDiff >> 16) < 0)
        goto scroll_y_lo_fix;
    if ((tempDiff & 0xFFFF) >= 0x3000)
        goto scroll_tiles;
scroll_y_lo_fix:
    scroll_y += spritedraw->del_y;
    if ((scroll_y >> 16) > (scroll_y_min >> 16))
        goto scroll_tiles;
    if ((scroll_y >> 16) < (scroll_y_min >> 16))
        goto scroll_y_lo_fix_limit;
    if ((scroll_y & 0xFFFF) >= (scroll_y_min & 0xFFFF))
        goto scroll_tiles;
scroll_y_lo_fix_limit:
    scroll_y = scroll_y_min;

scroll_tiles:
    scroll_x_tile = scroll_x >> 12;
    scroll_y_tile = scroll_y >> 12;
    CVort_engine_drawSpriteAt(spritedraw->pos_x, spritedraw->pos_y, spritedraw->frame + fr);

    if (engine_gameVersion == GAMEVER_KEEN3) {
        uint16_t messieW, messieH;

        // Check for Messie Contact
        if (!messie_mounted)
        {
            if (!messie_time_to_climb)
            {
                messieW = engine_maskedSpriteEntry[CVort3_spr_messield1*4].width;
                messieH = engine_maskedSpriteEntry[CVort3_spr_messield1*4].height;
                if (spritedraw->pos_x >= messie_xpos && spritedraw->pos_x <= messie_xpos + (messieW<<3<<8) && spritedraw->pos_y >= messie_ypos && spritedraw->pos_y <= messie_ypos + (messieH<<8))
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

    if (!g_game.wmap_sprite_on)
        return;

    if (engine_gameVersion == GAMEVER_KEEN1 || engine_gameVersion == GAMEVER_KEEN3) {
        if (CVort_ptr_worldmap_sprites(g_game.wmap_sprite_on, spritedraw, sprite_x, sprite_y))
            g_game.wmap_sprite_on = 0;
    }

}
