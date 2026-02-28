#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "cvorticons.h"
#include "cvorticons1.h"
#include "cvorticons2.h"
#include "cvorticons3.h"
#include "decompression/CGenius/fileio/compression/CRLE.h"

#include "cvorticons_episode_macros.h"

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
    if ((scrollX_T - ENGINE_VIEWPORT_ACTIVATE_MARGIN_X <= scaledX) && (scrollY_T - ENGINE_VIEWPORT_ACTIVATE_MARGIN_Y <= scaledY) && (scrollX_T + ENGINE_VIEWPORT_ACTIVATE_WIDTH_TILES >= scaledX) && (scrollY_T + ENGINE_VIEWPORT_ACTIVATE_HEIGHT_TILES >= scaledY))
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
