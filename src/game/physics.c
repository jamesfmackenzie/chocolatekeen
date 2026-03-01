#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "core/core.h"
#include "episodes/cvorticons1.h"
#include "episodes/cvorticons2.h"
#include "episodes/cvorticons3.h"
#include "third_party/cgenius/fileio/compression/CRLE.h"

#include "episodes/episode_macros.h"

void CVort_body_border_flash(Body_T *border) {
    border->variant += g_game.sprite_sync;
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
    bridge->variant += g_game.sprite_sync;
    if (bridge->variant < 12)
        return;
    bridge->variant -= 12;
    int16_t currActualX = bridge->tile_x + bridge->field_10;
    if (map_data_tiles[bridge->tile_y * map_width_tile + currActualX] != bridge->field_E) {
        bridge->think_ptr = &CVort_body_nop;
        return;
    }
    map_data_tiles[bridge->tile_y * map_width_tile + currActualX] = 0x10E;
    bridge->field_10 += bridge->field_C;
}

// FIXME? This is not well tested...

void CVort_body_bridge_retract(Body_T *bridge) {
    bridge->variant += g_game.sprite_sync;
    if (bridge->variant < 12)
        return;
    bridge->variant -= 12;
    bridge->field_10 -= bridge->field_C;
    int16_t currActualX = bridge->tile_x + bridge->field_10;
    if (map_data_tiles[bridge->tile_y * map_width_tile + currActualX] != 0x10E) {
        bridge->type_ = 0;
        return;
    }
    map_data_tiles[bridge->tile_y * map_width_tile + currActualX] = bridge->field_E;
}


// FIXME? This is sorta more vanilla like but not really needed...

void CVort_contact_nop(Sprite_T *curr, Sprite_T *other) {
}

void CVort_think_contact_nop(Sprite_T *curr, Sprite_T *other) {

}

// There is no "body_nop" in vanilla keen.
// think_contact_nop was originaly used for g_entities.bodies, but chocolate-keen crashes
// if we try that...
void CVort_body_nop(Body_T* body) {

}

void CVort_think_dead_sprite() {
    g_entities.temp_sprite.time++;
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort_think_kill_sprite() {
    g_entities.temp_sprite.type_ = OBJDEAD;
    g_entities.temp_sprite.time += g_game.sprite_sync;
    if (g_entities.temp_sprite.time > 40) {
        g_entities.temp_sprite.time -= 40;
        g_entities.temp_sprite.frame++;
        g_entities.temp_sprite.varB--;
        if (g_entities.temp_sprite.varB == 1)
            g_entities.temp_sprite.think = &CVort_think_dead_sprite;
    }
    g_entities.temp_sprite.vel_x = 0;
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort_think_remove_sprite() {
    g_entities.temp_sprite.type_ = 0;
}

void CVort_default_think() {
    CVort_chg_vid_and_error("Bad think pointer!");
}

void CVort_default_contact(Sprite_T *curr, Sprite_T *other) {
    CVort_chg_vid_and_error("Bad contact pointer!");
}

void CVort_kill_keen() {
    if (g_game.god_mode || g_game.keen_invincible)
        return;
    g_entities.sprites[0].think = &CVort_think_keen_death;
    g_entities.sprites[0].contact = &CVort_think_contact_nop;
    g_entities.sprites[0].pos_y += 0x800;
    g_entities.sprites[0].time = g_entities.sprites[0].vel_x = g_entities.sprites[0].vel_y = 0;
    g_entities.sprites[0].frame = 0x16;
    CVort_engine_setCurSound(8);
}

void CVort_kill_keen_temp() {
    g_entities.temp_sprite.think = &CVort_think_keen_death;
    g_entities.temp_sprite.contact = &CVort_think_contact_nop;
    g_entities.temp_sprite.pos_y += 0x800;
    g_entities.temp_sprite.time = g_entities.sprites[0].vel_x = g_entities.sprites[0].vel_y = g_entities.temp_sprite.vel_x = 0;
    g_entities.temp_sprite.frame = 0x16;
    CVort_engine_setCurSound(8);
}



// acceleration: l/r speed acceleration

void CVort_move_left_right(int16_t acceleration) {
    for (uint16_t loopVar = 1; loopVar <= g_game.sprite_sync; loopVar++) {
        g_entities.temp_sprite.vel_x += acceleration;
        if (g_entities.temp_sprite.vel_x > 0x78)
            g_entities.temp_sprite.vel_x = 0x78;
        else if (g_entities.temp_sprite.vel_x < -0x78)
            g_entities.temp_sprite.vel_x = -0x78;
        if (loopVar != g_game.sprite_sync)
            g_entities.temp_sprite.del_x += g_entities.temp_sprite.vel_x;
    }
}

void CVort_pogo_jump(int16_t max_height, int16_t diff) {
    for (uint16_t loopVar = 1; loopVar <= g_game.sprite_sync; loopVar++) {
        g_entities.temp_sprite.vel_y += diff;
        if (g_entities.temp_sprite.vel_y > max_height)
            g_entities.temp_sprite.vel_y = max_height;
        else if (-max_height > g_entities.temp_sprite.vel_y)
            g_entities.temp_sprite.vel_y = -max_height;
        if (loopVar != g_game.sprite_sync)
            g_entities.temp_sprite.del_y += g_entities.temp_sprite.vel_y;
    }
}

void CVort_check_ceiling() {
    if (scroll_x_min + 8 > g_entities.temp_sprite.pos_x) {
        g_entities.temp_sprite.vel_x = g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.pos_x = scroll_x_min + 8;
    } else if (g_entities.temp_sprite.pos_x > ceiling_x) {
        g_entities.temp_sprite.vel_x = g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.pos_x = ceiling_x;
    }
    if (g_entities.temp_sprite.pos_y < scroll_y_min) {
        g_entities.temp_sprite.vel_y = g_entities.temp_sprite.del_y = 0;
        g_entities.temp_sprite.pos_y = scroll_y_min;
    } else if (g_entities.temp_sprite.pos_y > ceiling_y) {
        CVort_engine_setCurSound(0x1B);
        CVort_engine_finishCurSound();
        CVort_kill_keen_temp();
    }
}

void CVort_body_slide_door(Body_T *door) {
    door->variant += g_game.sprite_sync;
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
    int16_t tileType0 = TILEINFO_Type[map_data_tiles[tileY * map_width_tile + tileX]];
    // NOTE: tileType1 is never actually used, so there is no need for it...
    //int16_t tileType1 = TILEINFO_Type[map_data_tiles[(tileY-1)*map_width_tile+tileX]];
    if (tileType0)
        doorHeight = tileY;
    else
        doorHeight = tileY - 1;
    int16_t doorBodyIndex = CVort_add_body();
    g_entities.bodies[doorBodyIndex].type_ = 1;
    g_entities.bodies[doorBodyIndex].think_ptr = &CVort_body_slide_door;
    // We don't measure the location by tiles for this body,
    // which is a kind of an exception indeed.
    g_entities.bodies[doorBodyIndex].tile_x = tileX << 12;
    g_entities.bodies[doorBodyIndex].tile_y = doorHeight << 12;
    g_entities.bodies[doorBodyIndex].variant = 0;
    g_entities.bodies[doorBodyIndex].field_C = map_data_tiles[(doorHeight + 3) * map_width_tile + tileX];
    g_entities.bodies[doorBodyIndex].field_E = map_data_tiles[(doorHeight + 2) * map_width_tile + tileX];
    g_entities.bodies[doorBodyIndex].field_10 = map_data_tiles[doorHeight * map_width_tile + tileX];
    // FIXME? Looks like if Keen opens the location from an unusual location
    // (the bottom of it?) then it loses a DIFFERENT item, if any.
    // This is, more or less, a direct port of the original, though.
    keen_gp.stuff[3 + tileType0] = 0;
    // Now clear the door tiles (to be replaced with the body)
    if (engine_gameVersion == GAMEVER_KEEN3) {
        int16_t tileId = map_data_tiles[doorHeight * map_width_tile + tileX - 1];
        map_data_tiles[doorHeight * map_width_tile + tileX] = tileId;
        map_data_tiles[(doorHeight + 1) * map_width_tile + tileX] = tileId;
    } else {
        map_data_tiles[doorHeight * map_width_tile + tileX] = 0x8F;
        map_data_tiles[(doorHeight + 1) * map_width_tile + tileX] = 0x8F;
    }
}

void CVort_do_fall() {
    for (uint16_t loopVar = 1; loopVar <= g_game.sprite_sync; loopVar++) {
        g_entities.temp_sprite.vel_y += 3;
        if (g_entities.temp_sprite.vel_y > 200)
            g_entities.temp_sprite.vel_y = 200;
        else if (g_entities.temp_sprite.vel_y < -400)
            g_entities.temp_sprite.vel_y = -400;
        if (loopVar != g_game.sprite_sync)
            g_entities.temp_sprite.del_y += g_entities.temp_sprite.vel_y;
    }
}

int16_t CVort_compute_sprite_delta() {
    g_entities.temp_sprite.del_x += g_entities.temp_sprite.vel_x*g_game.sprite_sync;
    g_entities.temp_sprite.del_y += g_entities.temp_sprite.vel_y*g_game.sprite_sync;
    return CVort_check_ground();
}

int16_t CVort_check_ground() {
    int16_t result = 0;
    if (g_entities.temp_sprite.del_x > 0xF00)
        g_entities.temp_sprite.del_x = 0xF00;
    else if (g_entities.temp_sprite.del_x < -0xF00)
        g_entities.temp_sprite.del_x = -0xF00;
    if (g_entities.temp_sprite.del_y > 0xF00)
        g_entities.temp_sprite.del_y = 0xF00;
    else if (g_entities.temp_sprite.del_y < -0xF00)
        g_entities.temp_sprite.del_y = -0xF00;

    int16_t loopVar, tempNum, var_8, var_A;

    g_entities.temp_sprite.box_y2 += g_entities.temp_sprite.del_y;
    g_entities.temp_sprite.box_y1 += g_entities.temp_sprite.del_y;
    int16_t var_4 = g_entities.temp_sprite.box_x1 >> 12;
    int16_t var_6 = g_entities.temp_sprite.box_x2 >> 12;

    if (g_entities.temp_sprite.del_y > 0) {
        if (((g_entities.temp_sprite.box_y2 - g_entities.temp_sprite.del_y) >> 12) != (g_entities.temp_sprite.box_y2 >> 12)) {
            var_A = g_entities.temp_sprite.box_y2 >> 12;
            for (loopVar = var_4; loopVar <= var_6; loopVar++) {
                if (!TILEINFO_UEdge[map_data_tiles[var_A * map_width_tile + loopVar]])
                    continue; // No collision
                g_entities.temp_sprite.vel_y = 0;
                tempNum = (g_entities.temp_sprite.box_y2 + 1) % 0x1000;
                g_entities.temp_sprite.del_y -= tempNum;
                g_entities.temp_sprite.box_y1 -= tempNum;
                g_entities.temp_sprite.box_y2 -= tempNum;
                result = 2;
                break;
            }
        }
    } else if (g_entities.temp_sprite.del_y < 0) {
        if (((g_entities.temp_sprite.box_y1 - g_entities.temp_sprite.del_y) >> 12) != (g_entities.temp_sprite.box_y1 >> 12)) {
            var_8 = g_entities.temp_sprite.box_y1 >> 12;
            for (loopVar = var_4; loopVar <= var_6; loopVar++) {
                if (!TILEINFO_DEdge[map_data_tiles[var_8 * map_width_tile + loopVar]])
                    continue; // No collision
                g_entities.temp_sprite.vel_y = 0;
                tempNum = 0x1000 - g_entities.temp_sprite.box_y1 % 0x1000;
                g_entities.temp_sprite.del_y += tempNum;
                g_entities.temp_sprite.box_y1 += tempNum;
                g_entities.temp_sprite.box_y2 += tempNum;
                result = 8;
                break;
            }
        }
    }

    g_entities.temp_sprite.box_x1 += g_entities.temp_sprite.del_x;
    g_entities.temp_sprite.box_x2 += g_entities.temp_sprite.del_x;
    var_8 = g_entities.temp_sprite.box_y1 >> 12;
    var_A = g_entities.temp_sprite.box_y2 >> 12;

    if (g_entities.temp_sprite.del_x > 0) {
        if (((g_entities.temp_sprite.box_x2 - g_entities.temp_sprite.del_x) >> 12) != (g_entities.temp_sprite.box_x2 >> 12)) {
            var_6 = g_entities.temp_sprite.box_x2 >> 12;
            for (loopVar = var_8; loopVar <= var_A; loopVar++) {
                if (!TILEINFO_LEdge[map_data_tiles[loopVar * map_width_tile + var_6]])
                    continue;
                g_entities.temp_sprite.vel_x = 0;
                tempNum = (g_entities.temp_sprite.box_x2 + 1) % 0x1000;
                g_entities.temp_sprite.del_x -= tempNum;
                g_entities.temp_sprite.box_x1 -= tempNum;
                g_entities.temp_sprite.box_x2 -= tempNum;
                result |= 4;
                break;
            }
        }
    } else if (g_entities.temp_sprite.del_x < 0) {
        if (((g_entities.temp_sprite.box_x1 - g_entities.temp_sprite.del_x) >> 12) != (g_entities.temp_sprite.box_x1 >> 12)) {
            var_4 = g_entities.temp_sprite.box_x1 >> 12;
            for (loopVar = var_8; loopVar <= var_A; loopVar++) {
                if (!TILEINFO_REdge[map_data_tiles[loopVar * map_width_tile + var_4]])
                    continue;
                g_entities.temp_sprite.vel_x = 0;
                tempNum = 0x1000 - g_entities.temp_sprite.box_x1 % 0x1000;
                g_entities.temp_sprite.del_x += tempNum;
                g_entities.temp_sprite.box_x1 += tempNum;
                g_entities.temp_sprite.box_x2 += tempNum;
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
    int16_t del_x_dif, del_y_dif, boxX_dif, boxY_dif;

    memcpy(&g_entities.temp_sprite, keen, sizeof(Sprite_T));

    if (carrier->pos_x < g_entities.temp_sprite.pos_x)
    {
        del_x_dif = carrier->del_x - g_entities.temp_sprite.del_x + 2;
        boxX_dif = (carrier->box_x2&0xFFFF) - (g_entities.temp_sprite.box_x1&0xFFFF) + 1;
    }
    else
    {
        del_x_dif = g_entities.temp_sprite.del_x - carrier->del_x + 2;
        boxX_dif = (g_entities.temp_sprite.box_x2&0xFFFF) - (carrier->box_x1&0xFFFF) + 1;
    }

    if (carrier->pos_y < g_entities.temp_sprite.pos_y)
    {
        del_y_dif = carrier->del_y - g_entities.temp_sprite.del_y + 2;
        boxY_dif = (carrier->box_y2&0xFFFF) - (g_entities.temp_sprite.box_y1&0xFFFF) + 1;
    }
    else
    {
        del_y_dif = g_entities.temp_sprite.del_y - carrier->del_y + 2;
        boxY_dif = (g_entities.temp_sprite.box_y2&0xFFFF) - (carrier->box_y1&0xFFFF) + 1;
    }

    g_entities.temp_sprite.del_y = g_entities.temp_sprite.del_x = 0;

    if (del_y_dif < boxY_dif)
    {
        if (carrier->pos_x > g_entities.temp_sprite.pos_x)
        {
            g_entities.temp_sprite.del_x = -boxX_dif;
            if (carrier->vel_x < g_entities.temp_sprite.vel_x)
            {
                g_entities.temp_sprite.vel_x = carrier->vel_x;
            }
        }
        else
        {
            g_entities.temp_sprite.del_x = boxX_dif;
            if (carrier->vel_x > g_entities.temp_sprite.vel_x)
            {
                g_entities.temp_sprite.vel_x = carrier->vel_x;
            }
        }
    }
    else
    {
        if (carrier->pos_y > g_entities.temp_sprite.pos_y)
        {
            g_entities.temp_sprite.del_x = carrier->del_x;
            g_entities.temp_sprite.del_y = -boxY_dif-0x80;
            if (g_entities.temp_sprite.think == &CVort_think_keen_jump_air)
            {
                g_entities.temp_sprite.think = &CVort_think_keen_ground;
            }
            else if (g_entities.temp_sprite.think == &CVort_think_keen_pogo_air)
            {
                g_entities.temp_sprite.think = &CVort_think_keen_pogo_ground;
                g_entities.temp_sprite.time = 0;
                g_entities.temp_sprite.varB = g_entities.temp_sprite.vel_x;
                g_entities.temp_sprite.vel_x = 0;
            }

            g_entities.temp_sprite.varD = 1;
            if ((g_entities.temp_sprite.vel_y = carrier->vel_y) < 0)
            {
                g_entities.temp_sprite.vel_y /= 2;
            }
        }
        else
        {
            g_entities.temp_sprite.del_y = boxY_dif;

            // Don't get "lifted" off platform if it's falling quickly
            if (carrier->vel_y > g_entities.temp_sprite.vel_y)
                g_entities.temp_sprite.vel_y = carrier->vel_y;
        }
    }

    CVort_check_ground();
    g_entities.temp_sprite.pos_x += g_entities.temp_sprite.del_x;
    g_entities.temp_sprite.pos_y += g_entities.temp_sprite.del_y;
    CVort_update_sprite_hitbox();
    //*keen = g_entities.temp_sprite;
    memcpy(keen, &g_entities.temp_sprite, sizeof(Sprite_T));
    CVort_do_scrolling();

}

// For Keen 3, used for the Meep pushing Keen.
// For any other episode it is unused with no source modification.
void CVort_push_keen(Sprite_T *keen, Sprite_T *pusher) {
    memcpy(&g_entities.temp_sprite, keen, sizeof(g_entities.temp_sprite));

    if ((g_entities.temp_sprite.box_x2 + g_entities.temp_sprite.box_x1)/2 < (pusher->box_x2 + pusher->box_x1)/2) {
        g_entities.temp_sprite.del_x = -(g_entities.temp_sprite.box_x2 - pusher->box_x1 + 1);
        if (g_entities.temp_sprite.del_x > 120)
            g_entities.temp_sprite.del_y = 120;
    } else {
        g_entities.temp_sprite.del_x = pusher->box_x2 - g_entities.temp_sprite.box_x1 + 1;
        if (g_entities.temp_sprite.del_x < -120)
            g_entities.temp_sprite.del_y = -120;
    }
    CVort_check_ground();
    g_entities.temp_sprite.pos_x += g_entities.temp_sprite.del_x;
    g_entities.temp_sprite.pos_y += g_entities.temp_sprite.del_y;
    CVort_update_sprite_hitbox();
    //*keen = g_entities.temp_sprite;
    memcpy(keen, &g_entities.temp_sprite, sizeof(g_entities.temp_sprite));
    CVort_do_scrolling();
}

void CVort_do_scrolling() {
    if ((g_entities.sprites[0].think == &CVort_think_keen_exit) ||
            (g_entities.sprites[0].think == &CVort_think_keen_death))
        return;
    int32_t sprPosX = g_entities.sprites[0].pos_x, sprPosY = g_entities.sprites[0].pos_y, tempNum;
    int16_t sprDelX = g_entities.sprites[0].del_x, sprDelY = g_entities.sprites[0].del_y;

    if (sprDelX > 0) // XScrollHi
    {
        tempNum = sprPosX - scroll_x;
        if (((tempNum >> 16) > 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) > 0xB000))) {
            scroll_x += sprDelX;
            if (scroll_x > scroll_x_max)
                scroll_x = scroll_x_max;
        }
    } else if (sprDelX < 0) // XScrollLo
    {
        tempNum = sprPosX - scroll_x;
        if (((tempNum >> 16) < 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) < 0x9000))) {
            scroll_x += sprDelX;
            if (scroll_x < scroll_x_min)
                scroll_x = scroll_x_min;
        }
    }

    if (sprDelY > 0) // YScrollHi
    {
        tempNum = sprPosY - scroll_y;
        if (((tempNum >> 16) > 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) > 0x7000))) {
            scroll_y += sprDelY;
            if (scroll_y > scroll_y_max)
                scroll_y = scroll_y_max;
        }
    } else if (sprDelY < 0) // YScrollLo
    {
        tempNum = sprPosY - scroll_y;
        if (((tempNum >> 16) < 0) || (!(tempNum >> 16) && ((tempNum & 0xFFFF) < 0x3000))) {
            scroll_y += sprDelY;
            if (scroll_y < scroll_y_min)
                scroll_y = scroll_y_min;
        }
    }
}

// Returns 0 if sprite is to be updated, 1 if not

int16_t CVort_sprite_active_screen() {
    int16_t scaledX = g_entities.temp_sprite.pos_x >> 12, scaledY = g_entities.temp_sprite.pos_y >> 12;
    if (g_entities.temp_sprite.pos_y < 0)
        g_entities.temp_sprite.pos_y = 0;
    if ((g_entities.temp_sprite.pos_x > map_width) || (g_entities.temp_sprite.pos_x < 0) || (g_entities.temp_sprite.pos_y > map_height)) {
        g_entities.temp_sprite.type_ = 0;
        return 1;
    }
    if ((scroll_x_tile - ENGINE_VIEWPORT_ACTIVATE_MARGIN_X <= scaledX) && (scroll_y_tile - ENGINE_VIEWPORT_ACTIVATE_MARGIN_Y <= scaledY) && (scroll_x_tile + ENGINE_VIEWPORT_ACTIVATE_WIDTH_TILES >= scaledX) && (scroll_y_tile + ENGINE_VIEWPORT_ACTIVATE_HEIGHT_TILES >= scaledY))
        return 0;
    if (g_entities.temp_sprite.type_ >= OBJONEBEFOREKEENSHOT) {
        g_entities.temp_sprite.type_ = 0;
        return 1;
    }
    g_entities.temp_sprite.active = 0;
    return 1;
}

// Returns 1 if there is a collision, and 0 otherwise

int16_t CVort_detect_sprite_col(Sprite_T *spr_0, Sprite_T *spr_1) {
    if (!spr_0->box_x1 || !spr_1->box_x1)
        return 0;
    if (spr_0->box_x2 < spr_1->box_x1)
        return 0;
    if (spr_0->box_y2 < spr_1->box_y1)
        return 0;
    if (spr_0->box_x1 > spr_1->box_x2)
        return 0;
    if (spr_0->box_y1 > spr_1->box_y2)
        return 0;
    return 1;
}

// Handles Keen's collision with various kinds of tiles

void CVort_keen_bgtile_col() {
    if (g_entities.sprites[0].think == &CVort_think_keen_death)
        return;
    g_game.keen_switch = 0;
    int16_t tileleft = g_entities.sprites[0].box_x1 >> 12;
    int16_t tileright = g_entities.sprites[0].box_x2 >> 12;
    int16_t tiletop = g_entities.sprites[0].box_y1 >> 12;
    int16_t tilebottom = g_entities.sprites[0].box_y2 >> 12;
    int16_t currTilePos, currTileType;

    for (int16_t currX = tileleft, currY; currX <= tileright; currX++)
        for (currY = tiletop; currY <= tilebottom; currY++) {
            currTilePos = currY * map_width_tile + currX;
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
                    else if (g_entities.sprites[0].del_x > 0)
                        g_entities.sprites[0].pos_x &= 0xFFFFF000;
                    else
                        g_entities.sprites[0].pos_x = (g_entities.sprites[0].pos_x + 0x1000)&0xFFFFF000;
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
                    if (g_entities.sprites[0].think != &CVort_think_keen_ground)
                        break;
                    CVort_engine_setCurSound(0xF);
                    g_entities.sprites[0].think = &CVort_think_keen_exit;
                    g_entities.sprites[0].contact = &CVort_think_contact_nop;
                    g_entities.sprites[0].time = currX + 2;
                    g_entities.sprites[0].varB = currY;
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
                        if (g_game.current_level == 0xB)
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
                    g_game.keen_switch = 1;
                    keen_tileX = currX;
                    keen_tileY = currY;
                    break;
                case 24: // Secret level teleporter
                    g_game.level_finished = LEVEL_END_SECRET;
                    break;
                // NOTE: The following cases should be CHECKED for in Keen 3 only
                case 27: // Ankh
                    if (engine_gameVersion != GAMEVER_KEEN3)
                        break;
                    g_game.keen_invincible += 1400;
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
