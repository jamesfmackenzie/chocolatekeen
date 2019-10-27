#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "cvorticons1.h"

/******************************************************************************
WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

Sprite think and contact functions, as well as body think functions, should
often be casted to CVorticons function types,
such as <Sprite_T::SpriteThinkFunction_T>. These are the functions that are
implemented only here (and not as virtual functions).
As long as the casts are done ONLY HERE (i.e. in CVorticons1 member functions),
they should be theoretically safe.
******************************************************************************/

void CVort1_show_pause_menu() {
    uint32_t origTicks = CVort_ptr_engine_getTicks();
    CVort_clear_keys();
    CVort_draw_box_opening_main(0x1C, 0xD);
    uint16_t origCursorX = cursorX, origCursorY = cursorY;

    CVort_draw_string_80("    SCORE     EXTRA KEEN AT \n");
    cursorX = origCursorX + 0xC;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80("    KEENS       SHIP PARTS  \n");
    cursorX = origCursorX + 0xE;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0xE;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0xE;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80(" RAYGUN   POGO    KEYCARDS  \n");
    cursorX = origCursorX + 8;
    CVort_draw_string_80(" ");
    cursorX += 6;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 8;
    CVort_draw_string_80(" ");
    cursorX += 6;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 8;
    CVort_draw_string_80(" ");
    cursorX += 6;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80(" CHARGE  ");
    cursorX += 6;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 8;
    CVort_draw_string_80(" ");
    cursorX += 6;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80("     PLEASE PRESS A KEY     ");

    sprintf(string_buf, "%" PRId32, keen_gp.score);
    cursorX = origCursorX + 0xA - strlen(string_buf);
    cursorY = origCursorY + 1;
    CVort_draw_string(string_buf);

    sprintf(string_buf, "%" PRId32, extra_life_pts + 20000);
    cursorX = origCursorX + 0x1A - strlen(string_buf);
    CVort_draw_string(string_buf);

    for (int16_t currKeen = 0; (currKeen < keen_gp.lives) && (currKeen < 6); currKeen++)
        CVort_engine_drawSprite((currKeen << 1) + origCursorX + 1, (origCursorY + 3) << 3, 0);

    cursorX = origCursorX + 0x10;
    cursorY = origCursorY + 3;
    if (keen_gp.stuff[0])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x1C0);
    else
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x141);

    cursorX += 3;
    if (keen_gp.stuff[4])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x1C1);
    else
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x142);

    cursorX += 3;
    if (keen_gp.stuff[1])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x1C2);
    else
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x143);

    cursorX += 3;
    if (keen_gp.stuff[2])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x1C3);
    else
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 4, 0x144);

    cursorX = origCursorX + 0x13;
    cursorY = origCursorY + 7;
    if (keen_gp.stuff[5])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 3, 0x1A8);
    if (keen_gp.stuff[6])
        CVort_engine_drawTile(cursorX + 4, (cursorY << 3) + 3, 0x1A9);
    if (keen_gp.stuff[7])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 0x15, 0x1AA);
    if (keen_gp.stuff[8])
        CVort_engine_drawTile(cursorX + 4, (cursorY << 3) + 0x15, 0x1AB);

    CVort_engine_drawTile(origCursorX + 3, ((origCursorY + 7) << 3) + 4, 0x19E);

    cursorX = origCursorX + 3;
    cursorY = origCursorY + 0xB;
    CVort_draw_number_word(keen_gp.ammo);

    if (keen_gp.stuff[3])
        CVort_engine_drawTile(origCursorX + 0xB, ((origCursorY + 8) << 3) + 4, 0x19F);

    CVort_clear_keys();
    //engine_updateActualDisplay(); // THIS IS IMPORTANT!
    CVort_translate_key(0);
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    CVort_clear_keys();
    CVort_ptr_engine_setTicks(origTicks);
}

/*
 * Level Stuff
 */
void CVort1_init_level(uint16_t levelnum) {
    int16_t currSprite;
    CVort_load_level_data(levelnum);
    for (int16_t currY = 0, currX; currY < map_height_T; currY++)
        for (currX = 0; currX < map_width_T; currX++) {
            currSprite = map_data_sprites[currY * map_width_T + currX];
            if (currSprite == 6)
                CVort1_add_body_cannon(currX, currY, 0);
            else if (currSprite > 6) {
                if (currSprite == 9)
                    CVort1_add_body_cannon(currX, currY, 3);
                else if (currSprite > 9) {
                    if (currSprite == 0xA)
                        CVort1_add_sprite_chain(currX, currY);
                    else if (currSprite == 0xFF) // Keen
                    {
                        sprites[0].posX = currX << 12;
                        sprites[0].posY = (currY << 12) + 0x800;
                    }
                } else if (currSprite == 7)
                    CVort1_add_body_cannon(currX, currY, 1);
                else if (currSprite == 8)
                    CVort1_add_body_cannon(currX, currY, 2);
            } else switch (currSprite) {
                    case 1:
                        CVort1_add_sprite_yorp(currX, currY);
                        break;
                    case 2:
                        CVort1_add_sprite_garg(currX, currY);
                        break;
                    case 3:
                        CVort_add_sprite_vorticon(currX, currY);
                        break;
                    case 4:
                        CVort1_add_sprite_butler(currX, currY);
                        break;
                    case 5:
                        CVort1_add_sprite_tankbot(currX, currY);
                        break;
                    default:
                        break;
                }
        }
}

void CVort1_think_keen_frozen() {
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 5)&1) + 0x1C;
    temp_sprite.time -= sprite_sync;
    if (temp_sprite.time < 0) {
        temp_sprite.frame = 0x1F;
        if (temp_sprite.time < -50)
            temp_sprite.think = &CVort_think_keen_ground;
    }
    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();
}

void CVort1_contact_keen(Sprite_T *keen, Sprite_T *contacted) {
    switch (contacted->type_) {
        case 2: // Yorp
            if (contacted->think == &CVort1_think_yorp_stunned)
                return;
            // Currently, the Yorp is not stunned
            if ((keen->velY <= 0) || (keen->posY + 0x800 > contacted->posY)) { // Yorp pushes
                keen->velY = 0;
                if (contacted->velX > 0) // Yorp pushes right
                    keen->velX = 0xF0;
                else
                    keen->velX = -0xF0;
                CVort_engine_setCurSound(0x1D);
                return;
            }
            // Yorp gets stunned
            contacted->think = &CVort1_think_yorp_stunned;
            contacted->time = 0;
            keen->velY = 0;
            keen->think = &CVort_think_keen_ground;
            CVort_engine_setCurSound(0x1F);
            return;
        case 5: // Push Keen
        case 6:
            keen->velY = 0;
            if (contacted->velX > 0)
                keen->velX = 240;
            else
                keen->velX = -240;
            CVort_engine_setCurSound(0x1D);
            return;
        case 3: // Kill Keen
        case 4:
        case 11:
            if ((contacted->type_ == 11) && (keen->think == &CVort1_think_keen_frozen))
                keen->time = 0;
            else
                CVort_kill_keen();
            return;
        case 15: // Ice cube
            keen->think = &CVort1_think_keen_frozen;
            keen->velX = contacted->velX;
            keen->velY = contacted->velY;
            keen->time = 800;
            CVort_engine_setCurSound(0x28);
            return;
        default:
            return;
    }
}

void CVort1_add_sprite_tankbot(int16_t tileX, int16_t tileY) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = 6;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = (tileY << 12) + 0x800;
    sprites[sprIndex].velX = 90;
    if (sprites[sprIndex].posX < sprites[0].posX)
        sprites[sprIndex].velX = -sprites[sprIndex].velX;
    sprites[sprIndex].think = &CVort1_think_tankbot_spawn;
    sprites[sprIndex].contact = &CVort_contact_nop;
    sprites[sprIndex].frame = 0x6A;
}

void CVort1_add_sprite_butler(int16_t tileX, int16_t tileY) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = 5;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].velX = 90;
    if (sprites[sprIndex].posX < sprites[0].posX)
        sprites[sprIndex].velX = -sprites[sprIndex].velX;
    sprites[sprIndex].think = &CVort1_think_butler_walk;
    sprites[sprIndex].contact = &CVort1_contact_butler;
    sprites[sprIndex].frame = 0x60;
}

void CVort1_add_sprite_garg(int16_t tileX, int16_t tileY) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = 3;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort1_think_garg_look;
    sprites[sprIndex].contact = &CVort1_contact_garg;
    sprites[sprIndex].frame = 0x3C;
}

void CVort1_add_sprite_yorp(int16_t tileX, int16_t tileY) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = 2;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = (tileY << 12) + 0x800;
    sprites[sprIndex].think = &CVort1_think_yorp_walk;
    if (sprites[sprIndex].posX < sprites[0].posX)
        sprites[sprIndex].velX = 60;
    else
        sprites[sprIndex].velX = -60;
    sprites[sprIndex].contact = &CVort1_contact_yorp;
    sprites[sprIndex].frame = 0x30;
}

void CVort1_add_sprite_chain(int16_t tileX, int16_t tileY) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = 8;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;

    // FIXME FIXME!!!
    // This could be a pretty bad idea!!!!!! But let's try anyway...
    sprites[sprIndex].think = (SpriteThinkFunction_T)(&CVort_think_keengun);

    sprites[sprIndex].contact = &CVort1_contact_chain;
    sprites[sprIndex].frame = 0x72;
}
void CVort1_add_sprite_cube(int32_t posX, int32_t posY, int16_t cannon_type) {
    int16_t sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = 15;
    sprites[sprIndex].posX = posX;
    sprites[sprIndex].posY = posY;
    sprites[sprIndex].think = &CVort1_think_cube;
    switch (cannon_type) {
        case 0:
            sprites[sprIndex].posX += 0x1000;
            sprites[sprIndex].velY = -200;
            sprites[sprIndex].velX = 200;
            break;
        case 1:
            sprites[sprIndex].velY = -200;
            sprites[sprIndex].velX = 0;
            break;
        case 2:
            sprites[sprIndex].velY = 200;
            sprites[sprIndex].velX = 0;
            break;
        case 3:
            sprites[sprIndex].velY = -200;
            sprites[sprIndex].velX = -200;
            break;
        default:
            break;
    }
    sprites[sprIndex].contact = &CVort1_contact_cube;
    sprites[sprIndex].frame = 0x70;
}

void CVort1_add_body_cannon(int16_t posX, int16_t posY, int16_t variant) {
    int16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = 3;
    bodies[bodyIndex].think_ptr = &CVort1_body_ice_cannon;
    bodies[bodyIndex].variant = variant;
    bodies[bodyIndex].tile_x = posX;
    bodies[bodyIndex].tile_y = posY;
}

void CVort1_think_yorp_walk() {
    if (!temp_sprite.velY) {
        if ((unsigned)(CVort_get_random()) < sprite_sync)
            temp_sprite.velY = -CVort_calc_jump_height(0x80);
        else if ((unsigned)(CVort_get_random()) < sprite_sync) {
            temp_sprite.think = &CVort1_think_yorp_look;
            temp_sprite.time = 0;
        }
    }
    if (temp_sprite.velX > 0)
        temp_sprite.frame = 0x34;
    else
        temp_sprite.frame = 0x36;
    temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4)&1;
    CVort_do_fall();
    int16_t currDelta = CVort_compute_sprite_delta();
    if (currDelta & 4)
        temp_sprite.velX = -0x3C;
    if (currDelta & 1)
        temp_sprite.velX = 0x3C;
}

void CVort1_think_yorp_look() {
    temp_sprite.velX = 0;
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 5)&3) + 0x30;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time >= 200) {
        if (temp_sprite.posX < sprites[0].posX)
            temp_sprite.velX = 0x3C;
        else
            temp_sprite.velX = -0x3C;
        temp_sprite.think = &CVort1_think_yorp_walk;
    }
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort1_think_yorp_stunned() {
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 4)&1) + 0x38;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time >= 800) {
        temp_sprite.time = 0;
        temp_sprite.think = &CVort1_think_yorp_look;
    }
    temp_sprite.velX = 0;
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort1_contact_yorp(Sprite_T *yorp, Sprite_T *contacted) {
    if ((contacted->type_ != 10) && (contacted->type_ != 11))
        return;
    yorp->time = 0;
    yorp->varB = 2;
    yorp->frame = 0x3A;
    yorp->think = &CVort_think_kill_sprite;
    yorp->contact = &CVort_think_contact_nop;
    yorp->velY = -80;
    CVort_engine_setCurSound(0x22);
}

void CVort1_think_garg_move() {
    if (!temp_sprite.velY && (temp_sprite.velX > -220) && (temp_sprite.velX < 220)) {
        if (CVort_get_random() < sprite_sync) {
            temp_sprite.think = &CVort1_think_garg_look;
            temp_sprite.time = 0;
        }
    }
    if (temp_sprite.velX > 0)
        temp_sprite.frame = 0x40;
    else
        temp_sprite.frame = 0x42;
    temp_sprite.frame += ((CVort_ptr_engine_getTicks() >> 4)&1);
    CVort_do_fall();
    int16_t currDelta = CVort_compute_sprite_delta();
    if (currDelta & 4)
        temp_sprite.velX = -60;
    if (currDelta & 1)
        temp_sprite.velX = 60;
    if (currDelta & 2)
        temp_sprite.time = 0;
    else if (((temp_sprite.velX == 220) || (temp_sprite.velX == -220)) && !temp_sprite.time) {
        temp_sprite.time = 1;
        temp_sprite.velY = -200;
    }
}

void CVort1_think_garg_look() {
    temp_sprite.velX = 0;
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 5)&3) + 0x3C;
    temp_sprite.time += sprite_sync;
    CVort_do_fall();
    CVort_compute_sprite_delta();
    if (temp_sprite.time < 80)
        return;
    if (temp_sprite.posY + 0x800 == sprites[0].posY)
        temp_sprite.velX = 220;
    else
        temp_sprite.velX = 60;
    if (temp_sprite.posX > sprites[0].posX)
        temp_sprite.velX = -temp_sprite.velX;
    temp_sprite.think = &CVort1_think_garg_move;
}

void CVort1_contact_garg(Sprite_T *garg, Sprite_T *contacted) {
    if ((contacted->type_ != 10) && (contacted->type_ != 11))
        return;
    garg->time = 0;
    garg->varB = 2;
    garg->frame = 0x44;
    garg->think = &CVort_think_kill_sprite;
    garg->contact = &CVort_think_contact_nop;
    garg->velY = -80;
    CVort_engine_setCurSound(0x23);
}

void CVort1_think_butler_walk() {
    if (temp_sprite.velX > 0)
        temp_sprite.frame = 0x58;
    else
        temp_sprite.frame = 0x5C;
    temp_sprite.frame += ((CVort_ptr_engine_getTicks() >> 5)&3);
    CVort_do_fall();
    int16_t currDelta = CVort_compute_sprite_delta();
    if (!(currDelta & 2)) // Don't fall down!
    {
        temp_sprite.delX = (-temp_sprite.delX) << 1;
        temp_sprite.delY = 0;
        temp_sprite.varB = -temp_sprite.velX;
        temp_sprite.think = &CVort1_think_butler_turn;
        temp_sprite.velX = 0;
        temp_sprite.time = 0;
    }
    if (currDelta & 4) {
        temp_sprite.varB = -90;
        temp_sprite.think = &CVort1_think_butler_turn;
        temp_sprite.velX = 0;
        temp_sprite.time = 0;
    }
    if (currDelta & 1) {
        temp_sprite.varB = 90;
        temp_sprite.think = &CVort1_think_butler_turn;
        temp_sprite.velX = 0;
        temp_sprite.time = 0;
    }
}

void CVort1_think_butler_turn() {
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time > 50) {
        temp_sprite.think = &CVort1_think_butler_walk;
        temp_sprite.velX = temp_sprite.varB;
    }
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 5)&1) + 0x60;
    CVort_do_fall();
    CVort_compute_sprite_delta();
}

void CVort1_contact_butler(Sprite_T *butler, Sprite_T *contacted) {
    if (contacted->type_ != 10)
        return;
    butler->time = 0;
}

void CVort1_think_tankbot_move() {
    if (temp_sprite.velX > 0)
        temp_sprite.frame = 0x62;
    else
        temp_sprite.frame = 0x66;
    temp_sprite.frame += ((CVort_ptr_engine_getTicks() >> 3)&3);
    if (CVort_get_random() < sprite_sync) {
        temp_sprite.think = &CVort1_think_tankbot_shoot;
        temp_sprite.time = 0;
        temp_sprite.varB = 0;
        temp_sprite.varC = temp_sprite.velX;
        temp_sprite.velX = 0;
    }
    CVort_do_fall();
    int16_t currDelta = CVort_compute_sprite_delta();
    temp_sprite.velY = 0;
    temp_sprite.delY = 0;
    if (!(currDelta & 2)) {
        if (temp_sprite.frame < 0x66) {
            temp_sprite.delX = (signed)sprite_sync * (-180);
            temp_sprite.varB = -90;
        } else {
            temp_sprite.delX = sprite_sync * 180;
            temp_sprite.varB = 90;
        }
    } else {
        if (currDelta & 4) {
            temp_sprite.varB = -90;
            temp_sprite.think = &CVort1_think_tankbot_turn;
            temp_sprite.velX = 0;
            temp_sprite.time = 0;
        }
        if (!(currDelta & 1))
            return;
        temp_sprite.varB = 90;
    }
    // TODO: This can probably be re-structured in some better way...
    temp_sprite.think = &CVort1_think_tankbot_turn;
    temp_sprite.velX = 0;
    temp_sprite.time = 0;
}

void CVort1_think_tankbot_spawn() {
    CVort_do_fall();
    if (CVort_compute_sprite_delta() & 2)
        temp_sprite.think = &CVort1_think_tankbot_move;
}

void CVort1_think_tankbot_turn() {
    temp_sprite.time += sprite_sync;
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 4)&1) + 0x6A;
    if (temp_sprite.time > 50) {
        temp_sprite.think = &CVort1_think_tankbot_move;
        temp_sprite.velX = temp_sprite.varB;
    }
}

void CVort1_think_tankbot_shoot() {
    temp_sprite.time += sprite_sync;
    if (!temp_sprite.varB && (temp_sprite.time > 80)) {
        CVort_engine_setCurSound(CVort1_snd_tankfire);
        if (temp_sprite.varC < 0)
            CVort_add_sprite_tankshot(temp_sprite.posX, temp_sprite.posY, -350);
        else
            CVort_add_sprite_tankshot(temp_sprite.posX, temp_sprite.posY, 350);
        temp_sprite.varB = 1;
    }
    if (temp_sprite.time <= 120)
        return;
    temp_sprite.varB = 90;
    if (temp_sprite.posX > sprites[0].posX)
        temp_sprite.varB = -90;
    temp_sprite.time = 0;
    temp_sprite.think = &CVort1_think_tankbot_turn;
}

void CVort1_body_ice_cannon(Body_T *cannon) {
    if (((CVort_ptr_engine_getTicks()&0xFFFF) >> 7) == cannon->field_C)
        return;
    cannon->field_C = ((CVort_ptr_engine_getTicks()&0xFFFF) >> 7);
    if (scrollX_T - 4 >= cannon->tile_x)
        return;
    if (scrollY_T - 4 >= cannon->tile_y)
        return;
    if (scrollX_T + 0x18 <= cannon->tile_x)
        return;
    if (scrollY_T + 0xE <= cannon->tile_y)
        return;
    CVort_engine_setCurSound(0x17);
    CVort1_add_sprite_cube(cannon->tile_x << 12, cannon->tile_y << 12, cannon->variant);
}

void CVort1_think_cubette_flight() {
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time > 60) {
        temp_sprite.type_ = 0;
        return;
    }
    CVort_do_fall();
    temp_sprite.delX += temp_sprite.velX*sprite_sync;
    temp_sprite.delY += temp_sprite.velY*sprite_sync;
}

void CVort1_think_cube() {
    if (!CVort_compute_sprite_delta())
        return;
    // A collision with a blocking tile has occurred,
    // so the cube splits into a few cubettes now.
    temp_sprite.type_ = 0;
    int16_t cubetteIndex = CVort_add_sprite(); // First, going down and right
    sprites[cubetteIndex].type_ = 12;
    sprites[cubetteIndex].think = &CVort1_think_cubette_flight;
    sprites[cubetteIndex].contact = &CVort_think_contact_nop;
    sprites[cubetteIndex].posX = temp_sprite.posX;
    sprites[cubetteIndex].posY = temp_sprite.posY;
    sprites[cubetteIndex].velX = 300;
    sprites[cubetteIndex].velY = 300;
    sprites[cubetteIndex].frame = 0x71;
    cubetteIndex = CVort_add_sprite();
    sprites[cubetteIndex].type_ = 12; // Second, going up and right
    sprites[cubetteIndex].think = &CVort1_think_cubette_flight;
    sprites[cubetteIndex].contact = &CVort_think_contact_nop;
    sprites[cubetteIndex].posX = temp_sprite.posX;
    sprites[cubetteIndex].posY = temp_sprite.posY;
    sprites[cubetteIndex].velX = 300;
    sprites[cubetteIndex].velY = -300;
    sprites[cubetteIndex].frame = 0x71;
    cubetteIndex = CVort_add_sprite();
    sprites[cubetteIndex].type_ = 12; // Third, going down and left
    sprites[cubetteIndex].think = &CVort1_think_cubette_flight;
    sprites[cubetteIndex].contact = &CVort_think_contact_nop;
    sprites[cubetteIndex].posX = temp_sprite.posX;
    sprites[cubetteIndex].posY = temp_sprite.posY;
    sprites[cubetteIndex].velX = -300;
    sprites[cubetteIndex].velY = 300;
    sprites[cubetteIndex].frame = 0x71;
    cubetteIndex = CVort_add_sprite();
    sprites[cubetteIndex].type_ = 12; // Fourth, going up and left
    sprites[cubetteIndex].think = &CVort1_think_cubette_flight;
    sprites[cubetteIndex].contact = &CVort_think_contact_nop;
    sprites[cubetteIndex].posX = temp_sprite.posX;
    sprites[cubetteIndex].posY = temp_sprite.posY;
    sprites[cubetteIndex].velX = -300;
    sprites[cubetteIndex].velY = -300;
    sprites[cubetteIndex].frame = 0x71;
    CVort_engine_setCurSound(0x13);
}

void CVort1_contact_cube(Sprite_T *cube, Sprite_T *contacted) {
    if (contacted->type_ != CVort1_obj_keen) // If it's not Keen to freeze...
        return;
    cube->think = &CVort_think_remove_sprite;
}

void CVort1_contact_chain(Sprite_T *chain, Sprite_T *contacted) {
    if (contacted->type_ != 10)
        return;
    // Turn the chain itself into zapzot
    chain->think = &CVort_think_zapzot;
    chain->time = 0;
    chain->contact = &CVort_think_contact_nop;
    if (CVort_get_random() > 0x80)
        chain->frame = CVort1_spr_shotsplashr;
    else
        chain->frame = CVort1_spr_shotsplashl;
    int16_t bodyChainIndex = CVort_add_body();
    bodies[bodyChainIndex].type_ = 4;
    bodies[bodyChainIndex].think_ptr = &CVort1_body_shot_chain;
    bodies[bodyChainIndex].tile_x = chain->posX >> 12;
    bodies[bodyChainIndex].tile_y = (chain->posY >> 12) + 1;
    bodies[bodyChainIndex].variant = 0;
    /* WARNING: This is the vanilla behavior, looking for the vorticon.
    If no vorticon is found, an INFINITE LOOP is the result!...
    ...Oh wait. Rather, there are great chances of an erroneous memory
    access, hence a crash.
    ANOTHER PROBLEM: What about sprite no. num_sprites? Is there any? */
    int16_t vorticonIndex;
    for (vorticonIndex = num_sprites; sprites[vorticonIndex].type_ != 4; vorticonIndex--);
    CVort_engine_setCurSound(CVort1_snd_vortscream);
    // Kill the vorticon
    sprites[vorticonIndex].time = 0;
    sprites[vorticonIndex].varB = 6;
    sprites[vorticonIndex].frame = 0x52;
    sprites[vorticonIndex].contact = &CVort_think_contact_nop;
    sprites[vorticonIndex].think = &CVort_think_kill_sprite;
}

void CVort1_body_shot_chain(Body_T *chain) {
    if (((CVort_ptr_engine_getTicks()&0xFFFF) >> 5) == chain->field_C)
        return;
    chain->field_C = ((CVort_ptr_engine_getTicks()&0xFFFF) >> 5);
    int16_t shiftedTilePos = (chain->tile_x & 0xFFFF) - 4;
    map_data_tiles[chain->tile_y * map_width_T + shiftedTilePos] = 0x9B;
    map_data_tiles[(chain->tile_y + 1) * map_width_T + shiftedTilePos] = 0x1DC;
    map_data_tiles[(chain->tile_y + 2) * map_width_T + shiftedTilePos] = 0x1E9;
    for (shiftedTilePos = (chain->tile_x & 0xFFFF) - 3; shiftedTilePos <= chain->tile_x + 3; shiftedTilePos++) {
        map_data_tiles[chain->tile_y * map_width_T + shiftedTilePos] = 0x9B;
        map_data_tiles[(chain->tile_y + 1) * map_width_T + shiftedTilePos] = 0x1DD;
        map_data_tiles[(chain->tile_y + 2) * map_width_T + shiftedTilePos] = 0x1EA;
    }
    shiftedTilePos = (chain->tile_x & 0xFFFF) + 4;
    map_data_tiles[chain->tile_y * map_width_T + shiftedTilePos] = 0x9B;
    map_data_tiles[(chain->tile_y + 1) * map_width_T + shiftedTilePos] = 0x1DE;
    map_data_tiles[(chain->tile_y + 2) * map_width_T + shiftedTilePos] = 0x1EB;

    chain->tile_y++;
    chain->variant++;
    if (chain->variant == 4) // Time to finish this
        chain->type_ = 0;
}

// Yorp statues

void CVort1_inlevel_message() {
    uint32_t tickcount = CVort_ptr_engine_getTicks();
    CVort_draw_box(0x15, 0xA);
    switch (current_level) {
        case 2:
            CVort_draw_string("You hear in your    \n");
            CVort_draw_string("mind:               \n");
            CVort_draw_string("                    \n");
            CVort_draw_string("It is too bad that  \n");
            CVort_draw_string("you cannot read the \n");
            CVort_draw_string("Standard Galactic   \n");
            CVort_draw_string("Alphabet, human.    \n");
            break;
        case 6:
            CVort_draw_string("A message echoes in \n");
            CVort_draw_string("your head:          \n");
            CVort_draw_string("                    \n");
            CVort_draw_string("The teleporter in   \n");
            CVort_draw_string("the ice will send   \n");
            CVort_draw_string("you to the dark side\n");
            CVort_draw_string("of Mars.            \n");
            break;
        case 9:
            CVort_draw_string("A voice buzzes in   \n");
            CVort_draw_string("your mind:          \n");
            CVort_draw_string("                    \n");
            CVort_draw_string("There is a hidden   \n");
            CVort_draw_string("city. Look in the   \n");
            CVort_draw_string("dark area of the    \n");
            CVort_draw_string("city to the south.  \n");
            break;
        case 10:
            CVort_draw_string("You see these words \n");
            CVort_draw_string("in your head:       \n");
            CVort_draw_string("                    \n");
            CVort_draw_string("You will need a     \n");
            CVort_draw_string("raygun in the end,  \n");
            CVort_draw_string("but not to shoot the\n");
            CVort_draw_string("Vorticon...         \n");
            break;
        case 11:
            CVort_draw_string("You hear in your    \n");
            CVort_draw_string("mind:               \n");
            CVort_draw_string("                    \n");
            CVort_draw_string(" GAAARRRRRGG!       \n");
            break;
        case 12:
            CVort_draw_string("A Yorpish whisper   \n");
            CVort_draw_string("says:               \n");
            CVort_draw_string("                    \n");
            CVort_draw_string("Look for dark, hidden\n");
            CVort_draw_string("bricks. You can see \n");
            CVort_draw_string("naught but their    \n");
            CVort_draw_string("upper left corner.  \n");
            break;
        case 15:
            CVort_draw_string("A Yorpy mind-thought\n");
            CVort_draw_string("bellows:            \n");
            CVort_draw_string("                    \n");
            CVort_draw_string("You cannot kill the \n");
            CVort_draw_string("Vorticon Commander  \n");
            CVort_draw_string("directly.           \n");
            break;
        default:
            break;
    }
    cursorY = textbox_offs_y + 5;
    CVort_draw_stringz("Press ENTER:");
    CVort_clear_keys();
    while ((CVort_read_char_with_echo()&0xFF) != 0xD); // ENTER/Return key
    CVort_engine_clearOverlay();
    CVort_clear_keys();
    CVort_ptr_engine_setTicks(tickcount);
}

void CVort1_draw_win() {
    // Actually it's 1, then 3, then 2.
#if 0
    int16_t win1[30] = {
        0x0, -3, 0x0, -3, 0x0, -3, 0x0, -3,
        0x0, -3, 0x0, -3, 0x0, -3, 0x0, -3,
        0x0, -3, 0x0, -3, 0x3, -3, 0x3, -2,
        0x3, -1, 0x3, 0x0, 0x0, 0x0,
    };
    int16_t win2[86] = {
        -3, -3, -3, -3, -3, -3, -3, -3,
        -3, -3, -3, -3, -3, -3, -3, -3,
        -3, -2, -3, -2, -3, -2, -3, -2,
        -3, -2, -3, -2, -3, -2, -3, -2,
        -3, -1, -3, -1, -3, -1, -3, -1,
        -3, -1, -3, -1, -3, -1, -3, -1,
        -3, 0, -3, 0, -3, 0, -3, 0,
        -3, 0, -3, 0, -3, 0, -3, 1,
        -3, 1, -3, 2, -3, 2, -4, 2,
        -4, 2, -4, 2, -4, 2, -4, 3,
        -3, 3, -2, 3, 0, 0,
    };

    int16_t win3[80] = {
        3, -1, 3, -1, 3, -2, 3, -2,
        3, -2, 3, -2, 3, -2, 3, -2,
        3, 0, 3, 0, 3, 0, 3, 0,
        3, 0, 3, 0, 3, 0, 3, 1,
        3, 1, 3, 1, 3, 1, 3, 1,
        3, 1, 3, 1, 3, 1, 3, 2,
        3, 2, 3, 2, 3, 2, 3, 2,
        3, 2, 3, 2, 3, 2, 2, 2,
        1, 2, 1, 2, 1, 2, 1, 2,
        1, 2, 1, 2, 1, 2, 0, 0,
    };
#endif

    // These memcpys would have been generated by the compiler
    // memcpy(win1, exeFields.win_array_1, 60);
    // memcpy(win2, exeFields.win_array_2, 172);
    // memcpy(win3, exeFields.win_array_3, 160);
    CVort_clear_keys();

    CVort_load_level_data(80);
    scrollX = 0x3000;
    scrollY = 0x23000;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    CVort_engine_drawSpriteAt(0xD000, 0x27000, 0x28);
    CVort_engine_drawScreen();
    CVort_engine_delay(300);
    CVort_draw_box2(5, 15, 42, 20);
    CVort_fade_in();
    CVort_draw_string_finale("Commander Keen returns to the Bean-\n");
    CVort_draw_string_finale("with-Bacon Megarocket and quickly\n");
    CVort_draw_string_finale("replaces the missing parts. He must\n");
    CVort_draw_string_finale("get home before his parents do!\n");
    CVort_load_level_data(81);
    //engine_updateActualDisplay();
    CVort_engine_delay(240);
    CVort_fade_out();

    scrollX = scrollY = 0;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();

    CVort_fade_in();
    int32_t pos_x = 0x6400, pos_y = 0x5000;
    int16_t sCounter = 0;
    do // The BWB moving in one direction; First stage
    {
        pos_x += (exeFieldsEp1.win1[sCounter << 1] << 8);
        pos_y += (exeFieldsEp1.win1[(sCounter << 1) + 1] << 8);
        CVort_engine_syncDrawing();
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
        CVort_engine_drawScreen();
        CVort_engine_delay(4);
        if (!(exeFieldsEp1.win1[sCounter << 1] + exeFieldsEp1.win1[(sCounter << 1) + 1])) {
            sCounter++; // Vanilla behavior...
            break;
        }
        sCounter++;
    } while (1);
    for (sCounter = 0; sCounter <= 20; sCounter++) // Second stage
    {
        pos_x += 0x300;
        pos_y += 0x100;
        CVort_engine_syncDrawing();
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
        CVort_engine_drawScreen();
    }
    // Third stage (like the second but with scrolling)
    for (sCounter = 0; sCounter <= 200; sCounter++) {
        scrollX += 0x300;
        scrollY += 0x100;
        pos_x += 0x300;
        pos_y += 0x100;
        CVort_engine_syncDrawing();
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
        CVort_engine_drawScreen();
    }
    for (sCounter = 0; sCounter <= 60; sCounter++) // Fourth, moving "down"
    {
        pos_y += 0x100;
        if (sCounter < 20)
            scrollY += 0x300;
        CVort_engine_syncDrawing();
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
        CVort_engine_drawScreen();
    }
    CVort_engine_delay(20);
    for (sCounter = 0; sCounter <= 30; sCounter++) // Fifth; What is the...
    {
        CVort_engine_syncDrawing();
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
        CVort_engine_drawSpriteAt(pos_x + 0x600, pos_y - 0xE00, 0x76);
        CVort_engine_drawScreen();
    }
    // Sixth, something's wrong...
    for (sCounter = 0; sCounter <= 50; sCounter++) {
        CVort_engine_syncDrawing();
        pos_x -= 0x200;
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x74);
        CVort_engine_drawScreen();
    }
    // Seventh; Now that's pretty bad!
    for (sCounter = 0; sCounter <= 30; sCounter++) {
        CVort_engine_syncDrawing();
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x74);
        CVort_engine_drawSpriteAt(pos_x + 0x600, pos_y - 0xE00, 0x75);
        CVort_engine_drawScreen();
    }
    sCounter = 0;
    do // Final stage
    {
        CVort_engine_syncDrawing();
        pos_x += (exeFieldsEp1.win3[sCounter << 1] << 8);
        pos_y += (exeFieldsEp1.win3[(sCounter << 1) + 1] << 8);
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
        CVort_engine_drawScreen();
        if (!(exeFieldsEp1.win3[sCounter << 1] + exeFieldsEp1.win3[(sCounter << 1) + 1])) {
            sCounter++; // Vanilla behavior...
            break;
        }
        sCounter++;
    } while (1);

    CVort_engine_delay(60);
    CVort_fade_out();
    int32_t save_scroll_x = scrollX;
    int32_t save_scroll_y = scrollY;
    CVort_engine_showImageFile("FINALE.CK1");
    CVort_draw_box2(4, 15, 27, 19);
    CVort_fade_in();
    CVort_draw_string_finale("Keen makes it home and\n");
    CVort_draw_string_finale("rushes to beat his\n");
    CVort_draw_string_finale("parents upstairs.");
    CVort_engine_delay(120);
    CVort_erase_parents_text();
    CVort_engine_delay(60);
    CVort_engine_drawBitmap(0xE, 0, 0xC);
    //engine_updateActualDisplay();
    CVort_engine_setCurSound(0x19);
    CVort_engine_delay(120);
    CVort_engine_drawBitmap(0xE, 0, 0xD);
    //engine_updateActualDisplay();
    CVort_engine_setCurSound(0x19);
    CVort_engine_delay(300);
    CVort_draw_box2(4, 15, 27, 19);
    CVort_draw_string_finale("Shhh, honey...let's\n");
    CVort_draw_string_finale("see if little Billy\n");
    CVort_draw_string_finale("is asleep.");
    CVort_engine_delay(180);
    CVort_erase_parents_text();
    CVort_engine_delay(60);
    CVort_engine_drawBitmap(0xE, 0, 0xC);
    CVort_engine_setCurSound(0x19);
    CVort_draw_box2(4, 15, 27, 20);
    CVort_draw_string_finale("Billy...? Are you a--\n");
    CVort_draw_string("WHAT IS THIS ONE-EYED\n");
    CVort_draw_string("GREEN THING IN YOUR\n");
    CVort_draw_string("ROOM!!!!???\n");
    //engine_updateActualDisplay();
    CVort_engine_delay(240);
    CVort_erase_parents_text();
    CVort_draw_box2(4, 15, 27, 18);
    CVort_draw_string_finale("Aw, Mom, can't I\n");
    CVort_draw_string_finale("keep him?");
    CVort_engine_delay(180);
    CVort_erase_parents_text();
    CVort_draw_box2(4, 15, 29, 19);
    CVort_draw_string_finale("Well, we'll talk about\n");
    CVort_draw_string_finale("that in the morning,\n");
    CVort_draw_string_finale("son. You get some rest.");
    CVort_engine_delay(180);
    CVort_erase_parents_text();
    CVort_draw_box2(4, 15, 27, 17);
    CVort_draw_string_finale("OK, Mom. Goodnight.");
    CVort_engine_delay(180);
    CVort_draw_box2(4, 15, 27, 17);
    CVort_draw_string_finale("Goodnight, dear.");
    CVort_engine_delay(180);
    CVort_erase_parents_text();
    CVort_engine_delay(60);
    CVort_engine_drawBitmap(0xE, 0, 0xD);
    //engine_updateActualDisplay();
    CVort_engine_setCurSound(0x19);
    CVort_engine_delay(300);
    CVort_engine_drawBitmap(0xE, 0, 0xC);
    CVort_draw_box2(4, 15, 29, 21);
    CVort_engine_setCurSound(0x19);
    CVort_draw_string_finale("But there is no sleep\n");
    CVort_draw_string_finale("for Commander Keen! The\n");
    CVort_draw_string_finale("Vorticon Mothership\n");
    CVort_draw_string_finale("looms above, ready to\n");
    CVort_draw_string_finale("destroy Earth!");
    CVort_engine_delay(300);
    CVort_fade_out();

    CVort_load_level_data(81);
    scrollX = save_scroll_x;
    scrollY = save_scroll_y;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    CVort_engine_drawSpriteAt(pos_x, pos_y, 0x74);
    CVort_engine_drawScreen();
    CVort_fade_in();
    CVort_engine_delay(30);

    sCounter = 0;
    do {
        CVort_engine_syncDrawing();
        pos_x += (exeFieldsEp1.win2[sCounter << 1] << 8);
        pos_y += (exeFieldsEp1.win2[(sCounter << 1) + 1] << 8);
        CVort_engine_drawSpriteAt(pos_x, pos_y, 0x74);
        CVort_engine_drawScreen();
        if (!(exeFieldsEp1.win2[sCounter << 1] + exeFieldsEp1.win2[(sCounter << 1) + 1])) {
            sCounter++; // Vanilla behavior...
            break;
        }
        sCounter++;
    } while (1);
    CVort_engine_syncDrawing();
    CVort_engine_drawSpriteAt(pos_x, pos_y, 0x73);
    CVort_engine_drawScreen();
    CVort_engine_delay(60);
    CVort_draw_box2(12, 3, 32, 5);
    CVort_draw_string_finale("TO BE CONTINUED....");
    CVort_engine_delay(400);
    scrollX &= 0xFFFFF000;
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    CVort_engine_setCurSound(0x29);
    CVort_do_text_viewer(end_text, 0, 0x16);
}

// Note the different formats in the following sprite addition functions!
// Map stuff

void CVort1_handle_secret_city() {
    CVort_load_level_data(80);
    CVort_engine_setCurSound(TELEPORTSND);
    sprites[0].delX = sprites[0].delY = sprites[0].velX = sprites[0].velY = 0;
    sprites[0].frame = 0x24;
    sprites[0].posX = cursorX_b = keen_wmap_x_pos = teleporters[2].destX;
    sprites[0].posY = cursorY_b = keen_wmap_y_pos = teleporters[2].destY;
    wmap_scrollX = scrollX = sprites[0].posX + 0xFFFF7000;
    wmap_scrollY = scrollY = sprites[0].posY + 0xFFFFD000;
    CVort_engine_syncDrawing();
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_fade_in();
    int16_t tile_x = sprites[0].posX / 0x1000, tile_y = sprites[0].posY / 0x1000;
    int16_t teleport_tile = TILES_TELEDIRT0;
    if (teleporters[2].isOnSnow)
        teleport_tile = TILES_TELESNOW0;
    // TODO: Should really be 0 (or anything, say, >= 0),
    // although the original code doesn't(?) seem to initialize it...
    int16_t tileOffset = 0;
    for (int16_t teleportCounter = 0; teleportCounter < 0x10; teleportCounter++) {
        CVort_engine_syncDrawing();
        CVort_engine_clearOverlay();
        if (!(teleportCounter % 2)) {
            tileOffset++;
            if (tileOffset > 3)
                tileOffset = 0;
        }
        map_data_tiles[tile_y * map_width_T + tile_x] = tileOffset + teleport_tile;
        CVort_engine_drawScreen();
    }
    teleport_tile = TILES_TELEDIRT;
    if (teleporters[2].isOnSnow)
        teleport_tile = TILES_TELESNOW;
    map_data_tiles[tile_y * map_width_T + tile_x] = teleport_tile;
    // Time to draw Keen on the map
    GameInput_T input;
    do {
        CVort_engine_syncDrawing();
        input = CVort_handle_ctrl(1);
        CVort_engine_drawSpriteAt(sprites[0].posX, sprites[0].posY, sprites[0].frame);
        CVort_engine_drawScreen();
        CVort_handle_cheat_keys();
        CVort_handle_global_keys();
    } while (input.but1jump || input.but2pogo);
    // FIXME? This is already done...
    wmap_scrollX = scrollX;
    wmap_scrollY = scrollY;
}

/*
 * Ordering info (yorp/garg under spotlights)
 */

void CVort1_show_ordering(uint16_t isTimed) {
#if 0
    static uint32_t ordering_sprites_x[2] = {0x9000, 0x8D00};
    static uint32_t ordering_sprites_y[2] = {0x9700, 0x8F00};
    static uint16_t ordering_sprites_frames[2] = {0x30, 0x3C};
#endif

    uint16_t frameOffset = 0; // di
    uint16_t creatureCounter = 0; // var_1C
    uint16_t currCreatureId = 0; // si
    int16_t timeforFrame = 0; // var_20
    CVort_ptr_do_ordering();
    CVort_fade_in();
    CVort_clear_keys();
    int16_t ticksCounter = 2400; // var_1E
    GameInput_T currInput;

    // FIXME Debug!
    //for (uint16_t frame = 0; frame < 119; frame++)
    //{
    //	CVort_engine_drawSpriteAt(0x8D00+scrollX, 0x8F00+scrollY, frame);
    //	CVort_engine_drawScreen();
    //	CVort_engine_delay(4);
    //}

    do {
        currInput = CVort_handle_ctrl(1);
        CVort_engine_syncDrawing();
        if (!(timeforFrame % 6)) {
            frameOffset = frameOffset + CVort_calc_jump_height(3) - 2;
            // TODO: Use signed variable types more often...
            if (((signed)frameOffset > 7) || ((signed)frameOffset < 0))
                frameOffset = CVort_calc_jump_height(49) / 7;
        }
        creatureCounter++;
        if (creatureCounter > 500) {
            currCreatureId ^= 1;
            creatureCounter = 0;
        }
        CVort_engine_drawSpriteAt(exeFieldsEp1.ordering_sprites_x[currCreatureId] + scrollX,
                exeFieldsEp1.ordering_sprites_y[currCreatureId] + scrollY,
                exeFieldsEp1.ordering_sprites_frames[currCreatureId] + frameOffset);
        CVort_engine_drawScreen();
        timeforFrame++;
        if (isTimed)
            ticksCounter -= sprite_sync;
        if (CVort_handle_global_keys()) {
            CVort_clear_keys();
            CVort1_do_ordering();
        }
        if (currInput.but1jump || currInput.but2pogo || CVort_translate_key(1))
            ticksCounter = 0;
    } while (ticksCounter > 0);
}

void CVort1_draw_ordering_info() {
    cursorX = cursorX_save = 0xc;
    cursorY = 4;
    CVort_draw_string_80("Commander Keen: Invasion\n");
    CVort_draw_string_80("of the Vorticons consists\n");
    CVort_draw_string_80("   of three unique and\n");
    CVort_draw_string_80("  challenging episodes:\n");
    CVort_draw_string_80("1. Marooned on Mars   $15\n");
    CVort_draw_string_80("2. The Earth Explodes $15\n");
    CVort_draw_string_80("3. Keen Must Die!     $15\n");
    cursorX = cursorX_save = 5;
    CVort_draw_string_80(" Order the trilogy for $30 and you get\n");
    cursorX--;
    cursorX_save = cursorX;
    CVort_draw_string_80("  * The \"Secret Hints & Tricks\" sheet\n");
    CVort_draw_string_80("  * The special \"cheat mode\" password\n");
    CVort_draw_string_80("  * The latest version of each game\n");
    CVort_draw_string_80("  * SEVERAL FREE BONUS GAMES!\n\n");
    CVort_draw_string_80("                       Mail orders to:\n");
    CVort_draw_string_80("(U.S. funds only       Apogee Software\n");
    CVort_draw_string_80("checks or M/O's        P.O. Box 476389\n");
    CVort_draw_string_80("include $2 P&H)        Garland, TX 75047\n\n\n");
    CVort_draw_string_80("Specify 5.25/3.5 disk size when ordering\n");
    cursorX = 0;
    CVort_draw_string_80("       Or order toll free: 1-800-852-5659    \n");
}

void CVort1_do_ordering() {
    scrollX = 0x16000;
    scrollY = 0x2000;
    // TODO: Is this a long shift left indeed?
    scrollX_T = scrollX << 12;
    scrollY_T = scrollY << 12;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    draw_func = &CVort1_draw_ordering_info;
    //draw_func = static_cast<CVorticons::DrawFunction_T>(&CVort1_draw_ordering_info);
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = 0;
    CVort_clear_keys();
}

void CVort1_draw_scores() {
    uint16_t var_2, var_4, extra_var;
    char radixStr[10];
    CVort_engine_drawBitmap(0xf, 7, 3);
    CVort_engine_drawBitmap(9, 0x2c, 4);
    CVort_engine_drawBitmap(0x17, 0x2c, 5);
    CVort_engine_drawBitmap(0x21, 0x2c, 6);

    for (uint16_t currEntry = 0; currEntry < 7; currEntry++) {
        // FIXME: Are the correct "long divisions" done here?
        var_2 = (scrollX / 0x1000) & 0xFFFF;
        var_4 = (scrollY / 0x1000) & 0xFFFF;
        extra_var = currEntry % 4;
        cursorY = (currEntry << 1) + 8;
        // Joystick
        if (high_scores_table.parts[currEntry][0])
            map_data_tiles[var_2 + (cursorY / 2 + var_4) * map_width_T + 0xE] = extra_var + 0xDD;
        // Battery
        if (high_scores_table.parts[currEntry][1])
            map_data_tiles[var_2 + (cursorY / 2 + var_4) * map_width_T + 0xF] = extra_var + 0xED;
        // Vacuum (NOTICE THE ORDER)
        if (high_scores_table.parts[currEntry][3])
            map_data_tiles[var_2 + (cursorY / 2 + var_4) * map_width_T + 0x10] = extra_var + 0xF1;
        // Booze
        if (high_scores_table.parts[currEntry][2])
            map_data_tiles[var_2 + (cursorY / 2 + var_4) * map_width_T + 0x11] = extra_var + 0xF5;
        // Score
        // FIXME: This is dangerous, but (more) true to the original...

        sprintf(radixStr, "%" PRId32, high_scores_table.scores[currEntry]);
        cursorX = 0x1d - strlen(radixStr);
        CVort_draw_string_80(radixStr);

        // Name
        cursorX = 9;
        CVort_draw_string_80(high_scores_table.names[currEntry]);
    }

}

// Ship parts dialog and teleporters
int16_t CVort1_worldmap_sprites(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y) {
    if (map_sprite_standing == 0x14) // Show BWB text
    {
        CVort_draw_box_opening_main(0x14, 8);
        CVort_draw_string("Your ship is missing\nthese parts:\n\n\n\n\n");
        CVort_draw_string("    GO GET THEM!\n");
        CVort_draw_string("   press a ");
        if (ctrl_type[1])
            CVort_draw_string("button:");
        else
            CVort_draw_string("key:");
        if (!keen_gp.stuff[0]) // Joystick
            CVort_engine_drawTile(cursorX_save + 3, (cursorY - 4) << 3, 0x141);
        if (!keen_gp.stuff[4]) // Battery
            CVort_engine_drawTile(cursorX_save + 7, (cursorY - 4) << 3, 0x142);
        if (!keen_gp.stuff[1]) // Vacuum
            CVort_engine_drawTile(cursorX_save + 11, (cursorY - 4) << 3, 0x143);
        if (!keen_gp.stuff[2]) // Booze
            CVort_engine_drawTile(cursorX_save + 15, (cursorY - 4) << 3, 0x144);
        //engine_updateActualDisplay();
        CVort_engine_delay(15);
        CVort_wait_for_key();
        CVort_engine_clearOverlay();
        return 1;
    }
    // It is a teleporter
    int16_t dstTeleportId, srcTeleportId, tileOffset, tile_x, tile_y, teleport_tile;
    // TODO: Vanilla code doesn't seem to do that directly,
    // but it's better we set this anyway.
    tileOffset = 0;
    for (int16_t teleportCounter = 0; teleportCounter < 3; teleportCounter++) {
        if ((map_sprite_standing & 0x20) != 0x20)
            continue;
        dstTeleportId = (map_sprite_standing & 3) - 1;
        srcTeleportId = ((map_sprite_standing >> 2)&3) - 1;
        tile_x = pos_x;
        tile_y = pos_y;
        teleport_tile = TILES_TELEDIRT0;
        if (teleporters[srcTeleportId].isOnSnow)
            teleport_tile = TILES_TELESNOW0;
        CVort_engine_setCurSound(TELEPORTSND);
        // Yes, the SAME loop variable is used.

        // Teleporter entrace...
        for (teleportCounter = 0; teleportCounter < 0x10; teleportCounter++) {
            CVort_engine_syncDrawing();
            CVort_engine_clearOverlay();
            if (!(teleportCounter % 2)) {
                tileOffset++;
                if (tileOffset > 3)
                    tileOffset = 0;
            }
            map_data_tiles[tile_y * map_width_T + tile_x] = tileOffset + teleport_tile;
            CVort_engine_drawScreen();
        }
        // Teleporting now...
        teleport_tile = TILES_TELEDIRT;
        if (teleporters[srcTeleportId].isOnSnow)
            teleport_tile = TILES_TELESNOW;
        map_data_tiles[tile_y * map_width_T + tile_x] = teleport_tile;
        spritedraw->posX = cursorX_b = keen_wmap_x_pos = teleporters[dstTeleportId].destX;
        spritedraw->posY = cursorY_b = keen_wmap_y_pos = teleporters[dstTeleportId].destY;
        scrollX = spritedraw->posX + 0xFFFF7000;
        scrollY = spritedraw->posY + 0xFFFFD000;
        tile_x = spritedraw->posX / 0x1000;
        tile_y = spritedraw->posY / 0x1000;
        teleport_tile = 0x152;
        if (teleporters[dstTeleportId].isOnSnow)
            teleport_tile = 0x156;
        // Getting out of the second teleporter...
        for (teleportCounter = 0; teleportCounter < 0x10; teleportCounter++) {
            CVort_engine_syncDrawing();
            CVort_engine_clearOverlay();
            if (!(teleportCounter % 2)) {
                tileOffset++;
                if (tileOffset > 3)
                    tileOffset = 0;
            }
            map_data_tiles[tile_y * map_width_T + tile_x] = tileOffset + teleport_tile;
            CVort_engine_drawScreen();
        }
        teleport_tile = 0x145;
        if (teleporters[dstTeleportId].isOnSnow)
            teleport_tile = 0x63;
        map_data_tiles[tile_y * map_width_T + tile_x] = teleport_tile;
        // Time to draw Keen again
        GameInput_T input;
        do {
            CVort_engine_syncDrawing();
            input = CVort_handle_ctrl(1);
            CVort_engine_drawSpriteAt(spritedraw->posX, spritedraw->posY, spritedraw->frame);
            CVort_engine_drawScreen();
            CVort_handle_cheat_keys();
            CVort_handle_global_keys();
        } while (input.but1jump || input.but2pogo);
        return 1;
    }
    return 0;
}
