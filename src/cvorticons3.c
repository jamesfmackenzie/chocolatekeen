#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <vector>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "cvorticons.h"
#include "cvorticons3.h"

/******************************************************************************
WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

Sprite think and contact functions, as well as body think functions, should
often be casted to CVorticons function types,
such as <Sprite_T::SpriteThinkFunction_T>. These are the functions that are
implemented only here (and not as virtual functions).
As long as the casts are done ONLY HERE (i.e. in CVorticons3 member functions),
they should be theoretically safe.
******************************************************************************/

void CVort3_show_pause_menu() {
    uint32_t origTicks = CVort_ptr_engine_getTicks();
    CVort_clear_keys();
    CVort_draw_box_opening_main(0x1C, 0xB);
    uint16_t origCursorX = cursorX, origCursorY = cursorY;

    CVort_draw_string_80("    SCORE     EXTRA KEEN AT \n");
    cursorX = origCursorX + 0xC;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80("    KEENS            PISTOL \n");
    cursorX = origCursorX + 0x13;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0x13;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0x13;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80(" ANKH TIME      KEY CARDS   \n");
    cursorX = origCursorX + 0xB;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0xB;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0xB;
    CVort_draw_string_80(" \n");
    CVort_draw_string_80("     PLEASE PRESS A KEY     ");

    sprintf(string_buf, "%" PRId32, keen_gp.score);
    cursorX = origCursorX + 0xA - strlen(string_buf);
    cursorY = origCursorY + 1;
    CVort_draw_string(string_buf);

    sprintf(string_buf, "%" PRId32, extra_life_pts + 20000);
    cursorX = origCursorX + 0x1A - strlen(string_buf);
    CVort_draw_string(string_buf);

    for (int16_t currKeen = 0; (currKeen < keen_gp.lives) && (currKeen < 9); currKeen++)
        CVort_engine_drawSprite((currKeen << 1) + origCursorX + 1, (origCursorY + 3) << 3, 0);

    CVort_engine_drawTile(origCursorX + 0x15, ((origCursorY + 3) << 3) + 4, 0xD8);

    cursorX = origCursorX + 0x18;
    cursorY = origCursorY + 0x4;
    CVort_draw_number_word(keen_gp.ammo);

    CVort_engine_drawTile(origCursorX + 3, ((origCursorY + 7) << 3) + 4, 0xD6);

    cursorX = origCursorX + 7;
    cursorY = origCursorY + 8;
    CVort_draw_number_word(keen_invincible / 0x90);

    cursorX = origCursorX + 0xD;
    cursorY = origCursorY + 8;

    if (keen_gp.stuff[5])
        CVort_engine_drawTile(cursorX, (cursorY << 3) - 4, 0xD9);
    if (keen_gp.stuff[6])
        CVort_engine_drawTile(cursorX + 4, (cursorY << 3) - 4, 0xDA);
    if (keen_gp.stuff[7])
        CVort_engine_drawTile(cursorX + 8, (cursorY << 3) - 4, 0xDB);
    if (keen_gp.stuff[8])
        CVort_engine_drawTile(cursorX + 12, (cursorY << 3) - 4, 0xDC);

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
void CVort3_init_level(uint16_t levelnum) {
    int16_t currSprite;
    CVort_load_level_data(levelnum);
    for (int16_t currY = 0, currX; currY < map_height_T; currY++)
        for (currX = 0; currX < map_width_T; currX++) {
            currSprite = map_data_sprites[currY * map_width_T + currX];

            switch (currSprite) {
                case 1:
                    CVort_add_sprite_vorticon(currX, currY);
                    break;

                case 2:
                    CVort_add_sprite_youth(currX, currY);
                    break;

                case 3:
                    CVort3_add_sprite_vortimom(currX, currY);
                    break;

                case 4:
                    CVort3_add_sprite_meep(currX, currY);
                    break;

                case 5:
                    CVort3_add_sprite_vortininja(currX, currY);
                    break;

                case 6:
                    CVort3_add_sprite_foob(currX, currY);
                    break;

                case 7:
                    CVort3_add_sprite_ball(currX, currY);
                    break;

                case 8:
                    CVort3_add_sprite_jack(currX, currY);
                    break;

                case 9:
                    CVort3_add_sprite_platform_h(currX, currY);
                    break;

                case 10:
                    CVort3_add_sprite_platform_v(currX, currY);
                    break;

                case 11:
                    CVort_add_sprite_vorticon(currX, currY);
                    break;

                case 12:
                    CVort3_add_sprite_spark(currX, currY);
                    break;

                case 13:
                    CVort3_add_sprite_heart(currX, currY);
                    break;

                case 14:
                    CVort3_add_body_enemygun_right(currX, currY);
                    break;

                case 15:
                    CVort3_add_body_enemygun_down(currX, currY);
                    break;

                case 16:
                    CVort3_add_body_mangling_arm(currX, currY);
                    break;

                case 17:
                    CVort3_add_body_mangling_leg(currX, currY, -1);
                    break;

                case 18:
                    CVort3_add_body_mangling_leg(currX, currY, 1);
                    break;

                case 0xFF:
                    sprites[0].posX = currX << 12;
                    sprites[0].posY = (currY << 12) + 0x800;
                    break;
                default:
                    break;
            }
        }
}

#if 0
void CVort3_add_sprite_vorticon(int16_t tileX, int16_t tileY) {
}
#endif

#if 0
void CVort3_add_sprite_youth(int16_t tileX, int16_t tileY) {
}
#endif

void CVort3_add_sprite_vortimom(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_vortimom;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_vortimom_walk;
    sprites[sprIndex].contact = &CVort3_contact_vortimom;
    sprites[sprIndex].health = 5;
    sprites[sprIndex].velX = (sprites[sprIndex].posX > sprites[0].posX) ? -50 : 50;
    sprites[sprIndex].frame = CVort3_spr_momleft1;
}

void CVort3_add_sprite_meep(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_meep;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = (tileY << 12) + 0x800;
    sprites[sprIndex].think = &CVort3_think_meep_walk;
    sprites[sprIndex].contact = &CVort3_contact_meep;
    sprites[sprIndex].health = 1;
    sprites[sprIndex].velX = (sprites[sprIndex].posX > sprites[0].posX) ? -65 : 65;
    sprites[sprIndex].frame = CVort3_spr_meepl1;
}

void CVort3_add_sprite_vortininja(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_vortininja;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_vortininja_stand;
    sprites[sprIndex].contact = &CVort3_contact_vortininja;
    sprites[sprIndex].health = 3; // In practice, 4 shots are needed to end one
    sprites[sprIndex].velX = (sprites[sprIndex].posX > sprites[0].posX) ? -1 : 1;
    sprites[sprIndex].frame = CVort3_spr_ninjal1;
}

void CVort3_add_sprite_foob(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_foob;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_foob_walk;
    sprites[sprIndex].contact = &CVort3_contact_foob;
    sprites[sprIndex].health = 1;
    sprites[sprIndex].velX = (sprites[sprIndex].posX > sprites[0].posX) ? -50 : 50;
    sprites[sprIndex].frame = CVort3_spr_foobl1;
}

void CVort3_add_sprite_ball(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_ball;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_ball;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].health = 1;
    sprites[sprIndex].velX = (sprites[sprIndex].posX > sprites[0].posX) ? -400 : 400;
    sprites[sprIndex].velY = (sprites[sprIndex].posY > sprites[0].posY) ? -400 : 400;
    sprites[sprIndex].frame = CVort3_spr_ball;
}

void CVort3_add_sprite_jack(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_jack;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_jack;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].health = 1;
    sprites[sprIndex].velX = (sprites[sprIndex].posX > sprites[0].posX) ? -400 : 400;
    sprites[sprIndex].velY = (sprites[sprIndex].posY > sprites[0].posY) ? -400 : 400;
    sprites[sprIndex].frame = CVort3_spr_jack1;
}

void CVort3_add_sprite_platform_h(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_platform;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = (tileY << 12) - 0x400;
    sprites[sprIndex].think = &CVort3_think_platform_move;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].frame = CVort3_spr_platform1;
    sprites[sprIndex].velX = 75;
}

void CVort3_add_sprite_platform_v(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_platform;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_platform_move;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].frame = CVort3_spr_platform1;
    sprites[sprIndex].velY = 75;
}

void CVort3_add_sprite_spark(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_spark;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_spark;
    sprites[sprIndex].contact = &CVort3_contact_spark;
    sprites[sprIndex].frame = CVort3_spr_spark1;
}

void CVort3_add_sprite_heart(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_heart;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_heart;
    sprites[sprIndex].contact = &CVort3_contact_heart;
    // Yes, that is an unnoticeable vanilla bug apparently!
    sprites[sprIndex].frame = CVort3_spr_spark1;
}

void CVort3_add_body_enemygun_right(int16_t tileX, int16_t tileY) {
    uint16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_enemyshot;
    // Yeah, the scaling IS done (used for the shots later).
    bodies[bodyIndex].tile_x = tileX << 12;
    bodies[bodyIndex].tile_y = tileY << 12;
    bodies[bodyIndex].think_ptr = &CVort3_think_enemygun_right;
}

void CVort3_add_body_enemygun_down(int16_t tileX, int16_t tileY) {
    uint16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_enemyshot;
    // Yeah, the scaling IS done (used for the shots later).
    bodies[bodyIndex].tile_x = tileX << 12;
    bodies[bodyIndex].tile_y = tileY << 12;
    bodies[bodyIndex].think_ptr = &CVort3_think_enemygun_down;
}

void CVort3_add_body_mangling_arm(int16_t tileX, int16_t tileY) {
    uint16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_mangling_arm;
    bodies[bodyIndex].tile_x = tileX;
    bodies[bodyIndex].tile_y = tileY;
    bodies[bodyIndex].think_ptr = &CVort3_think_mangling_arm;
    bodies[bodyIndex].field_C = 0;
    bodies[bodyIndex].variant = 1;
    bodies[bodyIndex].field_E = 15;
}

void CVort3_add_body_mangling_leg(int16_t tileX, int16_t tileY, int16_t left_right) {
    uint16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_mangling_leg;
    bodies[bodyIndex].tile_x = tileX;
    bodies[bodyIndex].tile_y = tileY;
    bodies[bodyIndex].field_E = bodies[bodyIndex].variant = left_right;
    bodies[bodyIndex].think_ptr = &CVort3_think_mangling_leg_moving;
}

void CVort3_contact_keen(Sprite_T *keen, Sprite_T *contacted) {
    switch (contacted->type_) {
        case 2:
        case 6:
        case 9:
        case 16:
        case 17: // Kill Keen
            CVort_kill_keen();
            break;

        case 3:
            if (keen->think == &CVort_think_keen_stunned)
                return;
            if (god_mode)
                return;
            if (keen_invincible)
                return;

            keen->think = &CVort_think_keen_stunned;
            keen->velX = contacted->velX;
            keen->velY = contacted->velY;
            keen->time = 400;
            break;

        case 4:
        case 5:
            CVort_push_keen(keen, contacted);
            break;

        case 8:
        case 10:
            CVort_carry_keen(keen, contacted);
            break;

        default: ;
    }
#if 0
    switch (contacted->type_) {
        case obj_grunt:
        case obj_elite:
        case obj_guardbot:
        case CVort3_obj_enemyshot:
            CVort_kill_keen();
            break;

        case obj_youth:
            if (keen->think == &CVort_think_keen_stunned)
                return;

            keen->think = &CVort_think_keen_stunned;
            keen->velX = contacted->velX;
            keen->velY = contacted->velY;
            keen->time = 400;
            break;

        case obj_scrub:
        case CVort3_obj_platform: // Push Keen
            CVort_carry_keen(keen, contacted);
            break;

    }
#endif
}

#if 0

void CVort3_add_sprite_youth(int16_t tileX, int16_t tileY) {

    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = obj_youth;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort3_think_youth_walk;
    sprites[sprIndex].contact = &CVort3_contact_youth;
    sprites[sprIndex].health = 1;
    sprites[sprIndex].velX = sprites[0].posX > sprites[sprIndex].posX ? 250 : -250;
    sprites[sprIndex].frame = spr_youthleft1;
}
#endif

#if 0
void CVort3_think_youth_walk() {
#if 0
    uint16_t blocking;

    temp_sprite.frame = temp_sprite.velX > 0 ? spr_youthright1 : spr_youthleft1;
    temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4)&3;
    if (CVort_get_random() < sprite_sync * 3) {
        temp_sprite.velY = -CVort_calc_jump_height(400);
        temp_sprite.think = &CVort3_think_youth_jump;
    }

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (!(blocking & 2)) {
        temp_sprite.think = &CVort3_think_youth_jump;
    }

    if (blocking & 4) {
        temp_sprite.velX = -250;
    }

    if (blocking & 1) {
        temp_sprite.velX = 250;
    }

#endif
}

void CVort3_think_youth_jump() {
#if 0
    uint16_t blocking;

    temp_sprite.frame = temp_sprite.velX > 0 ? spr_youthright4 : spr_youthleft4;

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (blocking & 2) {
        temp_sprite.think = &CVort3_think_youth_walk;
    }

    if (blocking & 4) {
        temp_sprite.velX = -250;
    }

    if (blocking & 1) {
        temp_sprite.velX = 250;
    }

#endif
}

void CVort3_contact_youth(Sprite_T *youth, Sprite_T *contacted) {
#if 0
    if (contacted->type_ == obj_keenshot || contacted->type_ == CVort3_obj_enemyshot) {
        CVort_engine_setCurSound(snd_vortscream);
        youth->time = 0;
        youth->varB = 2;
        youth->frame = spr_youthdie1;
        youth->contact = &CVort3_contact_nop;
        youth->think = &CVort3_think_kill_sprite;
    }

#endif
}
#endif

void CVort3_think_vortimom_walk() {
    int32_t currPosY;
    uint16_t blocking;

    if (temp_sprite.velX > 0) {
        temp_sprite.frame = CVort3_spr_momright1;
    } else {
        temp_sprite.frame = CVort3_spr_momleft1;
    }
    temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4) & 1;
    currPosY = temp_sprite.posY;
    if (CVort_get_random() < sprite_sync) {
        temp_sprite.varC = -temp_sprite.velX;
        temp_sprite.velX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        temp_sprite.think = &CVort3_think_vortimom_shoot;
    }
    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (!(blocking & 2)) {
        temp_sprite.posY = currPosY;
        temp_sprite.velX = -temp_sprite.velX;
        temp_sprite.posX += (temp_sprite.velX << 1);
    }
    if (blocking & 4) {
        temp_sprite.velX = -50;
    }
    if (blocking & 1) {
        temp_sprite.velX = 50;
    }
}

void CVort3_think_vortimom_shoot() {
    if (temp_sprite.varC < 0) {
        temp_sprite.frame = CVort3_spr_momattackl;
    } else {
        temp_sprite.frame = CVort3_spr_momattackr;
    }
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time < 30) {
        return;
    }
    if (!temp_sprite.varB) {
        temp_sprite.varB = 1;
        CVort_engine_setCurSound(0x26);
        if (temp_sprite.varC < 0) {
            CVort3_add_vortimomshot(temp_sprite.posX, temp_sprite.posY - 0x100, -150);
        } else {
            CVort3_add_vortimomshot(temp_sprite.posX, temp_sprite.posY - 0x100, 150);
        }
    }
    if (temp_sprite.time <= 50) {
        return;
    }
    temp_sprite.velX = temp_sprite.varC;
    temp_sprite.think = &CVort3_think_vortimom_walk;
}

void CVort3_add_vortimomshot(int32_t posX, int32_t posY, int16_t velocity) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort3_obj_enemyshot;
    sprites[sprIndex].posX = posX;
    sprites[sprIndex].posY = posY + 0xD00;
    sprites[sprIndex].think = &CVort3_think_vortimomshot;
    sprites[sprIndex].velX = velocity;
    sprites[sprIndex].contact = &CVort3_contact_vortimomshot;

    // TODO: Complete!
    if (velocity >= 0) {
        // Yes, initially time == frame!
        sprites[sprIndex].time = sprites[sprIndex].frame = CVort3_spr_momfirer1;
        if (!TILEINFO_REdge[map_data_tiles[((posY >> 12) + 1) * map_width_T + (posX >> 12) + 1]])
            return;
        CVort_engine_setCurSound(0x25);
        sprites[sprIndex].type_ = CVort3_obj_zapzot;
        sprites[sprIndex].think = &CVort_think_zapzot;
        sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            sprites[sprIndex].frame = CVort3_spr_shotsplashr;
        } else {
            sprites[sprIndex].frame = CVort3_spr_shotsplashl;
        }
    } else {
        // Again, initially time == frame!
        sprites[sprIndex].time = sprites[sprIndex].frame = CVort3_spr_momfirel1;
        if (!TILEINFO_LEdge[map_data_tiles[((posY >> 12) + 1) * map_width_T + (posX >> 12)]])
            return;
        CVort_engine_setCurSound(0x25);
        sprites[sprIndex].type_ = CVort3_obj_zapzot;
        sprites[sprIndex].think = &CVort_think_zapzot;
        sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            sprites[sprIndex].frame = CVort3_spr_shotsplashr;
        } else {
            sprites[sprIndex].frame = CVort3_spr_shotsplashl;
        }
    }
}

void CVort3_think_vortimomshot() {
    temp_sprite.frame = temp_sprite.time + ((CVort_ptr_engine_getTicks() >> 3) & 1);
    if (!CVort_compute_sprite_delta()) {
        return;
    }
    CVort_engine_setCurSound(0x25);
    temp_sprite.type_ = CVort3_obj_zapzot;
    temp_sprite.think = &CVort_think_zapzot;
    temp_sprite.time = 0;
    if (CVort_get_random() > 0x80) {
        temp_sprite.frame = CVort3_spr_shotsplashr;
    } else {
        temp_sprite.frame = CVort3_spr_shotsplashl;
    }
}

void CVort3_contact_vortimomshot(Sprite_T *shot, Sprite_T *contacted) {
    if (contacted->type_ == CVort3_obj_vortimom || contacted->type_ == CVort3_obj_dead) {
        return;
    }
    CVort_engine_setCurSound(0x25);
    shot->think = &CVort_think_zapzot;
    shot->contact = &CVort_think_contact_nop;
    shot->time = 0;
    if (CVort_get_random() > 0x80) {
        shot->frame = CVort3_spr_shotsplashr;
    } else {
        shot->frame = CVort3_spr_shotsplashl;
    }
}

void CVort3_contact_vortimom(Sprite_T *vortimom, Sprite_T *contacted) {
    if (contacted->type_ != CVort3_obj_keenshot)
        return;
    --(vortimom->health);
    if (vortimom->health) {
        return;
    }
    CVort_engine_setCurSound(0x27);
    vortimom->time = 0;
    vortimom->varB = 2;
    vortimom->frame = CVort3_spr_momdie1;
    vortimom->contact = &CVort_think_contact_nop;
    vortimom->think = &CVort_think_kill_sprite;
}

void CVort3_think_meep_walk() {
    uint16_t blocking;

    if (temp_sprite.velX > 0) {
        temp_sprite.frame = CVort3_spr_meepr1;
    } else {
        temp_sprite.frame = CVort3_spr_meepl1;
    }
    temp_sprite.frame += ((CVort_ptr_engine_getTicks() >> 4) & 1);
    if (CVort_get_random() < sprite_sync) {
        temp_sprite.varC = temp_sprite.velX; 
        temp_sprite.velX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        temp_sprite.think = &CVort3_think_meep_shoot;
    }
    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();
    // Do NOT fix! (An apparent vanilla bug)
    if (!(blocking & 2)) {
        temp_sprite.varC = temp_sprite.velX; 
        temp_sprite.velX = 0;
        temp_sprite.varB = 0;
        temp_sprite.time = 0;
        temp_sprite.think = &CVort3_think_meep_shoot;
    }
    if (blocking & 4) {
        temp_sprite.velX = -65;
    }
    if (blocking & 1) {
        temp_sprite.velX = 65;
    }
}

void CVort3_think_meep_shoot() {
    if (temp_sprite.varC < 0) {
        temp_sprite.frame = CVort3_spr_meepsingl;
    } else {
        temp_sprite.frame = CVort3_spr_meepsingr;
    }
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time < 60)
        return;
    if (!temp_sprite.varB) {
        int16_t shotIndex;
        temp_sprite.varB = 1;
        CVort_engine_setCurSound(0x2B);
        shotIndex = CVort_add_sprite();
        if (temp_sprite.varC < 0) {
            sprites[shotIndex].posX = temp_sprite.posX;
            // Yes, again time should be the same as the frame here!
            sprites[shotIndex].time = sprites[shotIndex].frame = CVort3_spr_meepwavel1;
            sprites[shotIndex].velX = -200;
        } else {
            sprites[shotIndex].posX = temp_sprite.posX + 0x1000;
            sprites[shotIndex].velX = 200;
            sprites[shotIndex].time = sprites[shotIndex].frame = CVort3_spr_meepwaver1;
        }
        sprites[shotIndex].posY = temp_sprite.posY + 0x400;
        sprites[shotIndex].think = &CVort3_think_meepshot;
        sprites[shotIndex].contact = &CVort_think_contact_nop;
        sprites[shotIndex].type_ = CVort3_obj_meepshot;
    }
    if (temp_sprite.time <= 80)
        return;
    temp_sprite.velX = temp_sprite.varC;
    temp_sprite.think = &CVort3_think_meep_walk;
}

void CVort3_contact_meep(Sprite_T *meep, Sprite_T *contacted) {
    if (contacted->type_ != CVort3_obj_keenshot)
        return;
    --(meep->health);
    if (meep->health) {
        return;
    }
    CVort_engine_setCurSound(0x27);
    meep->time = 0;
    meep->varB = 2;
    meep->frame = CVort3_spr_meepdie1;
    meep->contact = &CVort_think_contact_nop;
    meep->think = &CVort_think_kill_sprite;
}

void CVort3_think_meepshot() {
    temp_sprite.frame = temp_sprite.time + ((CVort_ptr_engine_getTicks() >> 3) & 1);
    temp_sprite.delX = temp_sprite.velX * sprite_sync;
}

void CVort3_think_vortininja_stand() {
    int32_t yPosDiff;
    if (temp_sprite.posX < sprites[0].posX) {
        temp_sprite.velX = 250;
        temp_sprite.frame = CVort3_spr_ninjar1;
    } else {
        temp_sprite.velX = -250;
        temp_sprite.frame = CVort3_spr_ninjal1;
    }
    temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 5) & 1;
    yPosDiff = sprites[0].posY - temp_sprite.posY;
    if ((yPosDiff > 20480) || (yPosDiff < -20480)) {
        return;
    }
    if (CVort_get_random() >= sprite_sync * 3) {
        return;
    }
   temp_sprite.velY = -CVort_calc_jump_height(350);
   temp_sprite.think = &CVort3_think_vortininja_jump;
}

void CVort3_think_vortininja_jump() {
    if (temp_sprite.velX > 0) {
        temp_sprite.frame = CVort3_spr_ninjajumpr;
    } else {
        temp_sprite.frame = CVort3_spr_ninjajumpl;
    }
    CVort_do_fall();
    if (CVort_compute_sprite_delta() & 2) {
        temp_sprite.think = &CVort3_think_vortininja_stand;
    }
}

void CVort3_contact_vortininja(Sprite_T *vortininja, Sprite_T *contacted) {
    if (contacted->type_ != CVort3_obj_keenshot)
        return;
    int16_t origHealth = vortininja->health;
    vortininja->health--;
    if (origHealth) {
        return;
    }
    CVort_engine_setCurSound(0x27);
    vortininja->time = 0;
    vortininja->varB = 2;
    vortininja->frame = CVort3_spr_ninjadie1;
    vortininja->contact = &CVort_think_contact_nop;
    vortininja->think = &CVort_think_kill_sprite;
}

void CVort3_think_foob_walk() {
    int16_t currTileX, currTileY, blocking;
    if (temp_sprite.velX > 0) {
        temp_sprite.frame = CVort3_spr_foobr1;
    } else {
        temp_sprite.frame = CVort3_spr_foobl1;
    }
    temp_sprite.frame += ((CVort_ptr_engine_getTicks() >> 5) & 1);
    currTileX = temp_sprite.posX >> 12;
    currTileY = temp_sprite.posY >> 12;
    if ((currTileX > scrollX_T) && (currTileY > scrollY_T)
        && (scrollX_T + 0x13 > currTileX)) {
        int16_t currRand = (CVort_get_random() < (sprite_sync << 1)) ? 1 : 0,
                secondQuery = (scrollY_T + 0xC > currTileY) ? 1 : 0;
        if (currRand & secondQuery) {
            temp_sprite.time = 0;
            temp_sprite.think = &CVort3_think_foob_scared;
            CVort_engine_setCurSound(0x22);
        }
    }
    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();
    // Do NOT fix! (An apparent vanilla bug)
    if (blocking & 4) {
        temp_sprite.velX = -50;
    }
    if (blocking & 1) {
        temp_sprite.velX = 50;
    }
}

void CVort3_think_foob_run() {
    int16_t blocking;
    if (temp_sprite.velX > 0) {
        temp_sprite.frame = CVort3_spr_foobr1;
    } else {
        temp_sprite.frame = CVort3_spr_foobl1;
    }
    temp_sprite.frame += ((CVort_ptr_engine_getTicks() >> 3) & 1);
    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();
    // The foob is now "chasing" into a wall
    if (blocking & 1) {
        temp_sprite.velX = -1;
    }
    if (blocking & 4) {
        temp_sprite.velX = 1;
    }
}

void CVort3_think_foob_scared() {
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() / 0xA) & 1) + CVort3_spr_foobyell1;
    temp_sprite.time += sprite_sync;
    if (temp_sprite.time <= 24)
        return;
    if (temp_sprite.posX > sprites[0].posX) {
        temp_sprite.velX = 250;
    } else {
        temp_sprite.velX = -250;
    }
    temp_sprite.think = &CVort3_think_foob_run;
}

void CVort3_contact_foob(Sprite_T *foob, Sprite_T *contacted) {
    if ((contacted->type_ != CVort3_obj_keen)
        && (contacted->type_ != CVort3_obj_keenshot)
        && (contacted->type_ != CVort3_obj_enemyshot))
        return;
    CVort_engine_setCurSound(0x22);
    foob->time = 0;
    foob->varB = 3;
    foob->frame = CVort3_spr_foobdie1;
    foob->contact = &CVort_think_contact_nop;
    foob->think = &CVort_think_kill_sprite;
}

void CVort3_think_jack() {
    int16_t blocking;
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 3) & 3) + CVort3_spr_jack1;
    blocking = CVort_compute_sprite_delta();
    if (blocking & 4) {
        temp_sprite.velX = -400;
    }
    if (blocking & 1) {
        temp_sprite.velX = 400;
    }
    if (blocking & 2) {
        temp_sprite.velY = -400;
    }
    if (blocking & 8) {
        temp_sprite.velY = 400;
    }
}

void CVort3_think_ball() {
    int16_t blocking = CVort_compute_sprite_delta();
    if (blocking & 4) {
        temp_sprite.velX = -400;
    }
    if (blocking & 1) {
        temp_sprite.velX = 400;
    }
    if (blocking & 2) {
        temp_sprite.velY = -400;
    }
    if (blocking & 8) {
        temp_sprite.velY = 400;
    }
}

void CVort3_think_platform_move() {
    uint16_t blocking;

    temp_sprite.frame = CVort3_spr_platform1 + ((CVort_ptr_engine_getTicks() >> 5)&1);

    blocking = CVort_compute_sprite_delta();

    if (blocking & 1 || blocking & 4) {
        if (blocking & 1) {
            temp_sprite.varB = 75;
        } else /*if (blocking & 4)*/ {
            temp_sprite.varB = -75;
        }
        temp_sprite.velX = temp_sprite.varC = temp_sprite.time = 0;
        temp_sprite.think = &CVort3_think_platform_turn;
    }
    if (blocking & 8) {
        temp_sprite.varC = 75;
        temp_sprite.velY = temp_sprite.varB = temp_sprite.time = 0;
        temp_sprite.think = &CVort3_think_platform_turn;
    } else if (blocking & 2) {
        temp_sprite.velY = -75; // A quick bump
    }
}

void CVort3_think_platform_turn() {

    if ((temp_sprite.time += sprite_sync) > 75) {
        temp_sprite.velX = temp_sprite.varB;
        temp_sprite.velY = temp_sprite.varC;
        temp_sprite.think = &CVort3_think_platform_move;
    }
}

void CVort3_think_enemyshot() {
    if (!CVort_compute_sprite_delta()) {
        return;
    }
    CVort_engine_setCurSound(0x25);
    temp_sprite.type_ = CVort3_obj_zapzot;
    temp_sprite.think = &CVort_think_zapzot;
    temp_sprite.time = 0;
    if (CVort_get_random() > 0x80) {
        temp_sprite.frame = CVort3_spr_shotsplashr;
    } else {
        temp_sprite.frame = CVort3_spr_shotsplashl;
    }
}

void CVort3_think_enemygun_right(Body_T * enemygun) {
    uint16_t sprIndex;

    enemygun->variant += sprite_sync;
    if (enemygun->variant < 400) {
        return;
    }
    enemygun->variant -= 400;
    CVort_engine_setCurSound(0x26);

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].frame = CVort3_spr_tankshot;
    sprites[sprIndex].velX = 400;
    sprites[sprIndex].posX = enemygun->tile_x;
    sprites[sprIndex].posY = enemygun->tile_y + 0x300;
    sprites[sprIndex].think = &CVort3_think_enemyshot;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].type_ = CVort3_obj_enemyshot;
}

void CVort3_think_enemygun_down(Body_T * enemygun) {
    uint16_t sprIndex;

    enemygun->variant += sprite_sync;
    if (enemygun->variant < 400) {
        return;
    }
    enemygun->variant -= 400;
    CVort_engine_setCurSound(0x26);

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].frame = CVort3_spr_tankshotv;
    sprites[sprIndex].velY = 400;
    sprites[sprIndex].posX = enemygun->tile_x + 0x400;
    sprites[sprIndex].posY = enemygun->tile_y;
    sprites[sprIndex].think = &CVort3_think_enemyshot;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].type_ = CVort3_obj_enemyshot;
}

void CVort3_think_spark() {
    temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 3) & 3) + CVort3_spr_spark1;
}

void CVort3_contact_spark(Sprite_T *spark, Sprite_T *contacted) {
    int16_t bodyIndex;
    if (contacted->type_ != CVort3_obj_keenshot)
        return;
    spark->type_ = CVort3_obj_null;
    bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_border_flasher;
    bodies[bodyIndex].think_ptr = &CVort_body_border_flash;
    bodies[bodyIndex].variant = 0;
    CVort_add_score(1000);
    spark_counter++;
    if (spark_counter != 6)
        return;

    for (bodyIndex = 0; bodyIndex < num_bodies; bodyIndex++) {
        if (bodies[bodyIndex].type_ == CVort3_bod_mangling_arm) {
            bodies[bodyIndex].type_ = CVort3_bod_null;
        }
    }
    bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_mangling_arm_destruct;
    bodies[bodyIndex].think_ptr = &CVort3_think_mangling_arm_destruct;
    bodies[bodyIndex].tile_y = 6;
    bodies[bodyIndex].variant = bodies[bodyIndex].field_C = 0;
}

void CVort3_think_heart() {
    temp_sprite.frame = (((CVort_ptr_engine_getTicks() & 0xFF) / 24) & 1) + CVort3_spr_heart1;
}

void CVort3_contact_heart(Sprite_T *heart, Sprite_T *contacted) {
    int16_t bodyIndex;
    if (contacted->type_ != CVort3_obj_keenshot)
        return;
    heart->type_ = CVort3_obj_null;
    bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_border_flasher;
    bodies[bodyIndex].think_ptr = &CVort_body_border_flash;
    bodies[bodyIndex].variant = 0;
    CVort_add_score(1000);

    for (bodyIndex = 0; bodyIndex < num_bodies; bodyIndex++) {
        if (bodies[bodyIndex].type_ == CVort3_bod_mangling_leg) {
            bodies[bodyIndex].type_ = CVort3_bod_null;
        }
    }
    bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = CVort3_bod_mangling_destruct;
    bodies[bodyIndex].think_ptr = &CVort3_think_mangling_destruct;
    bodies[bodyIndex].tile_y = 2;
    bodies[bodyIndex].variant = bodies[bodyIndex].field_C = 0;
    num_sprites = 0;
}

void CVort3_destory_mangling_tile(int16_t tileX, int16_t tileY, int16_t tileId) {
    int16_t spriteIndex;
    CVort_engine_setCurSound(0x25);
    spriteIndex = CVort_add_sprite();
    sprites[spriteIndex].think = &CVort_think_zapzot;
    sprites[spriteIndex].type_ = CVort3_obj_dead; // Yeah, NOT CVort3_obj_zapzot
    sprites[spriteIndex].posX = tileX << 12;
    sprites[spriteIndex].posY = tileY << 12;
    sprites[spriteIndex].time = 0;
    sprites[spriteIndex].contact = &CVort_think_contact_nop;
    if (CVort_get_random() > 0x80) {
        sprites[spriteIndex].frame = CVort3_spr_shotsplashr;
    } else {
        sprites[spriteIndex].frame = CVort3_spr_shotsplashl;
    }
    map_data_tiles[tileY * map_width_T + tileX] = tileId;
}

void CVort3_think_mangling_arm_destruct(Body_T* body) {
    body->variant += sprite_sync;
    if (body->variant < 30)
        return;
    body->variant -= 30;
    body->tile_y++;
    if (body->tile_y == 19) {
        body->type_ = 0;
        return;
    }
    int16_t currTileY = body->tile_y & 0xFF, currTileX;
    for (currTileX = 5; currTileX <= 7; currTileX++) {
        if (map_data_tiles[currTileY * map_width_T + currTileX] != 0xA9) {
            CVort3_destory_mangling_tile(currTileX, currTileY, 0xA9);
        }
    }
    for (currTileX = 17; currTileX <= 19; currTileX++) {
        if (map_data_tiles[currTileY * map_width_T + currTileX] != 0xA9) {
            CVort3_destory_mangling_tile(currTileX, currTileY, 0xA9);
        }
    }
}

void CVort3_think_mangling_destruct(Body_T* body) {
    body->variant += sprite_sync;
    if (body->variant < 20)
        return;
    body->variant -= 20;
    body->tile_y++;
    if (body->tile_y == 24) {
        level_finished = LEVEL_END_EXIT;
        body->type_ = 0;
        return;
    }
    if (body->tile_y >= 19)
        return;

    int16_t currTileY = body->tile_y & 0xFF, currTileX;
    for (currTileX = 8; currTileX <= 16; currTileX++) {
        if (map_data_tiles[currTileY * map_width_T + currTileX] != 0xA9) {
            CVort3_destory_mangling_tile(currTileX, currTileY, 0xA9);
        }
    }
}

void CVort3_think_mangling_arm(Body_T *arm) {
    // WARNING: Each of the body's tile fields 32-bit long,
    // while we store truncated 16-bit forms of them!
    int16_t currTileX = arm->tile_x & 0xFFFF, currTileY = arm->tile_y & 0xFFFF;
    arm->field_C += sprite_sync;
    if (arm->field_C < arm->field_E) {
        return;
    }
    arm->field_C -= arm->field_E;
    map_data_tiles[currTileY * map_width_T + currTileX] = 0xA9;
    map_data_tiles[currTileY * map_width_T + currTileX - 1] = 0xA9;
    map_data_tiles[(currTileY + 1) * map_width_T + currTileX - 1] = 0xA9;
    map_data_tiles[(currTileY + 2) * map_width_T + currTileX - 1] = 0xA9;
    map_data_tiles[currTileY * map_width_T + currTileX + 1] = 0xA9;
    map_data_tiles[(currTileY + 1) * map_width_T + currTileX + 1] = 0xA9;
    map_data_tiles[(currTileY + 2) * map_width_T + currTileX + 1] = 0xA9;

    if (arm->variant == -1) {
        if (map_data_tiles[(currTileY - 1) * map_width_T + currTileX] != 0x255) {
            arm->variant = 1;
            arm->field_E = 15;
        } else {
            currTileY--;
            arm->tile_y--;
        }
    } else {
        if (map_data_tiles[(currTileY + 3) * map_width_T + currTileX] == 0x1D7) {
            arm->variant = -1;
            arm->field_E = 40;
        } else {
            map_data_tiles[currTileY * map_width_T + currTileX] = 0x255;
            currTileY++;
            arm->tile_y++;
        }
    }
    map_data_tiles[currTileY * map_width_T + currTileX] = 0x255;
    map_data_tiles[currTileY * map_width_T + currTileX - 1] = 0x26A;
    map_data_tiles[(currTileY + 1) * map_width_T + currTileX - 1] = 0x26C;
    map_data_tiles[(currTileY + 2) * map_width_T + currTileX - 1] = 0x26B;
    map_data_tiles[currTileY * map_width_T + currTileX + 1] = 0x26A;
    map_data_tiles[(currTileY + 1) * map_width_T + currTileX + 1] = 0x26C;
    map_data_tiles[(currTileY + 2) * map_width_T + currTileX + 1] = 0x26B;
}

void CVort3_think_mangling_leg_moving(Body_T *leg) {
    // WARNING: Each of the body's tile fields 32-bit long,
    // while we store truncated 16-bit forms of them!
    int16_t currTileX = leg->tile_x & 0xFFFF, currTileY = leg->tile_y & 0xFFFF;
    leg->field_C += sprite_sync;
    if (leg->field_C < 35) {
        return;
    }
    leg->field_C -= 35;
    map_data_tiles[currTileY * map_width_T + currTileX] = 0xA9;

    if (leg->field_E == -1) {
        map_data_tiles[currTileY * map_width_T + currTileX - 1] = 0xA9;
        map_data_tiles[currTileY * map_width_T + currTileX - 2] = 0xA9;
        map_data_tiles[currTileY * map_width_T + currTileX - 3] = 0xA9;
    } else {
        map_data_tiles[currTileY * map_width_T + currTileX + 1] = 0xA9;
        map_data_tiles[currTileY * map_width_T + currTileX + 2] = 0xA9;
        map_data_tiles[currTileY * map_width_T + currTileX + 3] = 0xA9;
    }
    if (leg->variant == -1) {
        if (map_data_tiles[(currTileY - 1) * map_width_T + currTileX] != 0x255) {
            leg->variant = 1;
        } else {
            currTileY--;
            leg->tile_y--;
        }
    } else {
        if (map_data_tiles[(currTileY + 1) * map_width_T + currTileX] == 0x1AE) {
            leg->variant = -1;
            CVort_engine_setCurSound(0x2D);
            leg->think_ptr = &CVort3_think_mangling_leg_awaiting;
            leg->field_C = 0;
        } else {
            map_data_tiles[currTileY * map_width_T + currTileX] = 0x255;
            currTileY++;
            leg->tile_y++;
        }
    }
    if (leg->field_E == -1) {
        map_data_tiles[currTileY * map_width_T + currTileX] = 0x26E;
        map_data_tiles[currTileY * map_width_T + currTileX - 1] = 0x26F;
        map_data_tiles[currTileY * map_width_T + currTileX - 2] = 0x26F;
        map_data_tiles[currTileY * map_width_T + currTileX - 3] = 0x26D;
    } else {
        map_data_tiles[currTileY * map_width_T + currTileX] = 0x26D;
        map_data_tiles[currTileY * map_width_T + currTileX + 1] = 0x26F;
        map_data_tiles[currTileY * map_width_T + currTileX + 2] = 0x26F;
        map_data_tiles[currTileY * map_width_T + currTileX + 3] = 0x26E;
    }
}

void CVort3_think_mangling_leg_awaiting(Body_T *leg) {
    leg->field_C += sprite_sync;
    if (leg->field_C > 200) {
        leg->field_C = 0;
        leg->think_ptr = &CVort3_think_mangling_leg_moving;
    }
}

// Does nothing for this episode
void CVort3_inlevel_message() {
}

// A variant of draw_box
void CVort_draw_box_grand_intellect_1(int16_t width, int16_t height) {
    int16_t x, y;
    x = textbox_offs_x - width / 2;
    y = 20 - height / 2;
    CVort_draw_box2(x, y, x + width + 1, y + height + 1);
}

// Another such variant of draw_box
void CVort_draw_box_grand_intellect_2(int16_t width, int16_t height) {
    int16_t x, y;
    x = textbox_offs_x - width / 2;
    y = 18 - height / 2;
    CVort_draw_box2(x, y, x + width + 1, y + height + 1);
}

int16_t CVort3_delay_grand_intellect(int16_t length) {
    uint16_t currKey;
    for (int16_t loopVar = 0; loopVar < length; loopVar++) {
        CVort_engine_delay(1);
        currKey = CVort_translate_key(1) & 0xFF;
        if ((currKey == 0x1B) || (currKey == 0x20)
            || CVort_engine_readGamePort()) {
            CVort_clear_keys();
            input_old.but1jump = input_old.but2pogo = 1;
            return 1;
        }
    }
    return 0;
}

// Same as CVort3_draw_string_win, but also checks for user input
int16_t CVort3_draw_string_grand_intellect(const char *str) {
    char charBuffer[2];
    uint16_t currKey;
    charBuffer[1] = 0;
    for (uint16_t currChar = 0; strlen(str) > currChar; currChar++) {
        charBuffer[0] = str[currChar];
        CVort_draw_string(charBuffer);
        CVort_engine_delay(6);
        currKey = CVort_translate_key(1) & 0xFF;
        if ((currKey == 0x1B) || (currKey == 0x20)
            || CVort_engine_readGamePort()) {
            CVort_clear_keys();
            return 1;
        }
    }
    return 0;
}

void CVort3_handle_grand_intellect() {
    spark_counter = 0;
    CVort_engine_setCurSound(0x2C);
    CVort_engine_setCurSound(0x21);
    CVort_engine_finishCurSound();
    CVort_draw_box_grand_intellect_1(5, 1);
    CVort_engine_clearOverlay();
    if (CVort3_draw_string_grand_intellect("No...")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(180)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_1(12, 1);
    if (CVort3_draw_string_grand_intellect("It can't be!")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(180)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_1(18, 1);
    if (CVort3_draw_string_grand_intellect("MORTIMER McMIRE!!!")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(240)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_1(38, 5);
    if (CVort3_draw_string_grand_intellect("Mortimer has been a thorn in your side\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("for as long as you can remember.  Your\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("IQ test score was 314--Mortimer's was\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("315.  He always held that over you,\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("never letting you forget for one day.\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(240)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_1(38, 5);
    if (CVort3_draw_string_grand_intellect("All the practical jokes, the mental\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("cruelty, the swirlies--each memory\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("makes your teeth grit harder.  And now\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("he's out to destroy earth!  You have\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("had enough!\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(240)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_1(31, 2);
    if (CVort3_draw_string_grand_intellect("\"ALL RIGHT MORTIMER, WHAT'S THE\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("PROBLEM?  WHY DESTROY EARTH?\"\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(180)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_2(35, 4);
    if (CVort3_draw_string_grand_intellect("\"You and all those mental wimps\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("deserve to die!  I'm the smartest\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("person in the galaxy.  Aren't I,\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("mister THREE FOURTEEN! Ah ha ha!\"")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(180)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_1(34, 1);
    if (CVort3_draw_string_grand_intellect("\"I'll get you for that, Mortimer!\"")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(180)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
    CVort_engine_clearOverlay();
    CVort_draw_box_grand_intellect_2(38, 3);
    if (CVort3_draw_string_grand_intellect("\"Come and try!  You'll never get past\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("my hideous Mangling Machine!  Prepare\n")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_draw_string_grand_intellect("to die, Commander Clown!\"")) {
        CVort_engine_drawScreen();
        return;
    }
    if (CVort3_delay_grand_intellect(240)) {
        CVort_engine_drawScreen();
        return;
    }
    CVort_engine_drawScreen();
}

void CVort3_blink_camera_flash() {
    CVort_engine_setPaletteAndBorderColor(exeFieldsEp3.palette_camFlash);
    CVort_engine_setCurSound(0x19);
    CVort_engine_finishCurSound();
    CVort_engine_delay(2);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
}

void CVort3_draw_win() {
    CVort_load_level_data(81);
    scrollX = scrollY = 0x2000;
    CVort_engine_syncDrawing();
    CVort_engine_clearOverlay();
    CVort_engine_drawSpriteAt(0xE000, 0x6000, 0x43);
    CVort_engine_drawSpriteAt(0xF800, 0x6800, 0);
    CVort_engine_drawScreen();
    CVort_fade_in();
    CVort_draw_box2(4, 18, 43, 23);
    CVort3_draw_string_win("With Mortimer McMire out of the       \n");
    CVort3_draw_string_win("picture, and his brain-wave belts no\n");
    CVort3_draw_string_win("longer controlling them, the Vorticons\n");
    CVort3_draw_string_win("are freed of their mental enslavement.\n");
    CVort_engine_delay(120);
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_draw_box2(4, 18, 43, 22);
    CVort3_draw_string_win("\"Commander Keen, in honor of your     \n");
    CVort3_draw_string_win("meritorious service in freeing us from\n");
    CVort3_draw_string_win("the Grand Intellect's mental chains,\"\n");
    CVort_engine_delay(120);
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_draw_box2(4, 18, 43, 22);
    CVort3_draw_string_win("\"I and the other Vorticons you haven't\n");
    CVort3_draw_string_win("slaughtered want to award you the     \n");
    CVort3_draw_string_win("Big V, our highest honor.\"\n");
    CVort_engine_delay(120);
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_draw_box2(17, 18, 30, 20);
    CVort3_draw_string_win("\"Thank you!\"\n");
    CVort_engine_delay(120);

    map_data_tiles[5 * map_width_T + 0x13] = 0x3A;
    CVort_engine_drawScreen();
    CVort3_blink_camera_flash();
    map_data_tiles[5 * map_width_T + 0x13] = 0x48;
    CVort_engine_drawScreen();
    CVort_engine_delay(20);
    map_data_tiles[(map_width_T << 3) + 3] = 0x3C;
    CVort_engine_drawScreen();
    CVort3_blink_camera_flash();
    map_data_tiles[(map_width_T << 3) + 3] = 0x3D;
    CVort_engine_drawScreen();
    CVort_engine_delay(10);
    map_data_tiles[(map_width_T << 3) + 5] = 0x3C;
    CVort_engine_drawScreen();
    CVort3_blink_camera_flash();
    map_data_tiles[(map_width_T << 3) + 5] = 0x3D;
    CVort_engine_drawScreen();
    CVort_engine_delay(5);
    map_data_tiles[5 * map_width_T + 0x14] = 0x3A;
    CVort_engine_drawScreen();
    CVort3_blink_camera_flash();
    map_data_tiles[5 * map_width_T + 0x14] = 0x48;
    CVort_engine_drawScreen();
    CVort_engine_delay(10);
    map_data_tiles[(map_width_T << 3) + 8] = 0x3C;
    CVort_engine_drawScreen();
    CVort3_blink_camera_flash();
    map_data_tiles[(map_width_T << 3) + 8] = 0x3D;
    CVort_engine_drawScreen();
    CVort_engine_delay(15);

    CVort_engine_delay(120);
    CVort_fade_out();
    scrollX &= 0x0FFFF000;
    CVort_engine_drawScreen();
    // FIXME? This is a LOWERCASE file name!!! (which IS the vanilla way...)
    CVort_engine_showImageFile("finale.ck3");
    //
    CVort_fade_in();
    CVort_load_level_data(81);
    scrollX = scrollY = 0x2000;
    CVort_engine_delay(400);
    CVort_draw_box2(18, 20, 29, 24);
    CVort3_draw_string_win("The End...\n For now!\n");
    // Yes, a NORMAL CVort_draw_string function call with no delays!
    CVort_draw_string("    ");
    CVort_clear_keys();
    CVort_read_char_with_echo();
    scrollX &= 0xFFFFF000;
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    CVort_do_text_viewer_short(end_text, 0, 22);
    CVort_engine_setCurSound(0x29);
    CVort_engine_finishCurSound();
    CVort_engine_setCurSound(0x2C);
    CVort_do_text_viewer(end_text, 0, 22);
}

// Same as CVort3_draw_string_grand_intellect, but does NOT check for user input
void CVort3_draw_string_win(const char *str) {
    char charBuffer[2];
    uint16_t currKey;
    charBuffer[1] = 0;
    for (uint16_t currChar = 0; strlen(str) > currChar; currChar++) {
        charBuffer[0] = str[currChar];
        CVort_draw_string(charBuffer);
        CVort_engine_delay(6);
    }
}

/*
 * Ordering info (Keen scaring a Foob away)
 */
void CVort3_show_ordering(uint16_t isTimed) {

    uint16_t di;
    uint16_t foobTicker;
    int16_t ticksCounter;
    int32_t foobX, foobVelX;
    uint32_t var10, var14; // Don't seem to have any use
    uint16_t foobFrame;

    di = 100;
    foobTicker = 0;
    var10 = var14 = 0;

    CVort3_do_ordering();
    CVort_fade_in();
    CVort_clear_keys();

    ticksCounter = 2400;
    GameInput_T currInput;

    do {
        currInput = CVort_handle_ctrl(1);
        CVort_engine_syncDrawing();

        // Draw Keen
        if (foobTicker == 0 || foobTicker > 10) {
            CVort_engine_drawSpriteAt(scrollX + 0x800, scrollY + 0xB000, CVort3_spr_keenwalkr1);

        } else {
            if (foobVelX > 0) {
                CVort_engine_drawSpriteAt(scrollX + 0x800, scrollY + 0xB000, CVort3_spr_keenwalkr1);
            } else {
                CVort_engine_drawSpriteAt(scrollX + 0x800, scrollY + 0xB000, CVort3_spr_keenshootr);
            }
        }

        if (di) {
            if (!--di) {
                foobX = 0x15400;
                foobVelX = -0x100;
                foobTicker = CVort_calc_jump_height(175) + 100;
            }
        }

        // Draw the foob
        if (foobTicker) {
            foobX += foobVelX;
            if (foobVelX > 0) {
                CVort_engine_drawSpriteAt(scrollX + foobX, scrollY + 0xB800, 0x5F + (foobFrame & 2) / 2);

            } else {
                CVort_engine_drawSpriteAt(scrollX + foobX, scrollY + 0xB800, 0x5D + (foobFrame & 2) / 2);
            }

            if (!--foobTicker) {
                if (foobVelX > 0) {
                    di = 10 + CVort_calc_jump_height(50);
                    foobVelX = 0;
                } else {
                    // Draw Foob freakout
                    CVort_engine_setCurSound(0x22);
                    CVort_engine_syncDrawing();
                    CVort_engine_drawSpriteAt(scrollX + 0x800, scrollY + 0xB000, 0x14);
                    CVort_engine_drawSpriteAt(scrollX + foobX, scrollY + 0xB800, 0x61);
                    CVort_engine_drawScreen();
                    CVort_engine_drawScreen();
                    CVort_engine_syncDrawing();
                    CVort_engine_drawSpriteAt(scrollX + 0x800, scrollY + 0xB000, 0x14);
                    CVort_engine_drawSpriteAt(scrollX + foobX, scrollY + 0xB800, 0x62);
                    CVort_engine_drawScreen();
                    CVort_engine_drawScreen();

                    foobVelX = 0x600;
                    foobTicker = (0x15400 - foobX) / 0x600;
                }
            }
        }

        // seg000:ADE4
        CVort_engine_drawScreen();
        foobFrame++;

        if (isTimed)
            ticksCounter -= sprite_sync;

        if (CVort_handle_global_keys()) {
            CVort_clear_keys();
            CVort3_do_ordering();
        }

        if (currInput.but1jump || currInput.but2pogo || CVort_translate_key(1))
            ticksCounter = 0;
    } while (ticksCounter > 0);
}

void CVort3_draw_ordering_info() {
    cursorX = cursorX_save = 4;
    cursorY = 4;
    CVort_draw_string_80("    Commander Keen: Invasion of the\n");
    CVort_draw_string_80("   Vorticons consists of three unique\n");
    CVort_draw_string_80("        and challenging episodes:\n\n");
    CVort_draw_string_80("   Order the trilogy for $30 and get:\n");
    CVort_draw_string_80("  * The \"Secret Hints & Tricks\" sheet\n");
    CVort_draw_string_80("  * The special \"cheat mode\" password\n");
    CVort_draw_string_80("  * The latest version of each game\n");
    CVort_draw_string_80("  * SEVERAL FREE BONUS GAMES!\n\n");
    CVort_draw_string_80(" Mail orders to:     U.S funds only;\n");
    CVort_draw_string_80(" Apogee Software     checks or M/O's.\n");
    CVort_draw_string_80(" P.O. Box 476389     Include $2 postage\n");
    CVort_draw_string_80(" Garland, TX 75047   and handling.\n\n");
    CVort_draw_string_80("Specify 5.25/3.5 disk size when ordering\n");
    CVort_draw_string_80("   Or order toll free: 1-800-852-5659 ");
}

void CVort3_do_ordering() {
    scrollX = 0x16000;
    scrollY = 0x2000;
    // TODO: Is this a long shift left indeed?
    scrollX_T = scrollX >> 12;
    scrollY_T = scrollY >> 12;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    draw_func = &CVort3_draw_ordering_info;
    //draw_func = static_cast<CVort_DrawFunction_T> (&CVort3_draw_ordering_info);
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = 0;
    CVort_clear_keys();
}

void CVort3_draw_scores() {
    uint16_t var_2, var_4, extra_var;
    char radixStr[10];
    CVort_engine_drawBitmap(0xf, 7, 3);
    CVort_engine_drawBitmap(0xe, 0x2c, 4);
    CVort_engine_drawBitmap(0x1c, 0x2c, 5);

    for (uint16_t currEntry = 0; currEntry < 7; currEntry++) {
        // FIXME: Are the correct "long divisions" done here?
        var_2 = (scrollX / 0x1000) & 0xFFFF;
        var_4 = (scrollY / 0x1000) & 0xFFFF;
        extra_var = currEntry % 4;
        cursorY = (currEntry << 1) + 8;

        sprintf(radixStr, "%" PRId32, high_scores_table.scores[currEntry]);
        cursorX = 0x22 - strlen(radixStr);
        CVort_draw_string_80(radixStr);

        // Name
        cursorX = 0xE;
        CVort_draw_string_80(high_scores_table.names[currEntry]);
    }
}

// Handle infoplane stuff 

int16_t CVort3_worldmap_sprites(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y) {

    int16_t si, di, var2, var4;

    if (map_sprite_standing == 0x14) // Show BWB text
    {
        uint16_t bwb_msg = CVort_calc_jump_height(3);
        CVort_draw_box_opening_main(0x20, 6);
        si = cursorY;

        switch (bwb_msg) {
            case 0:
                CVort_draw_string("You enter your ship, sit around\n");
                CVort_draw_string("for a while, get bored, then\n");
                CVort_draw_string("remember that you have to find\n");
                CVort_draw_string("the Grand Intellect!");
                break;
            case 1:
                CVort_draw_string("Into the ship you journey, lie\n");
                CVort_draw_string("about a bit, then resume your\n");
                CVort_draw_string("quest for the Grand Intellect!");
                break;
            case 2:
                CVort_draw_string("You feel like entering the ship\n");
                CVort_draw_string("and taking a rest, but the\n");
                CVort_draw_string("mystery of the Grand Intellect\'s\n");
                CVort_draw_string("identity changes your mind.");
                break;
            case 3:
                CVort_draw_string("Entering the ship might be a\n");
                CVort_draw_string("fun thing to do, but right now,\n");
                CVort_draw_string("you need to find the Grand\n");
                CVort_draw_string("Intellect and vanquish him!");
                break;
        }
        cursorY = si + 4;
        CVort_draw_string("\n         press a ");
        if (ctrl_type[1])
            CVort_draw_string("button:");
        else
            CVort_draw_string("key:");
        //CVort_engine_delay(15);
        CVort_clear_keys();
        CVort_wait_for_key();
        CVort_clear_keys();
        CVort_engine_clearOverlay();
        return 1;
    }

    // Check for.. messie shore tiles?  
    if (((map_sprite_standing & 0xFF00) >= 0x2000) && ((map_sprite_standing & 0xFF00) <= 0x2200))
        return 1;

    // Check for teleporters
    if ((map_sprite_standing & 0xF00) != 0xF00)
        return 0;

    // Get the starting teleporter
    si = (map_sprite_standing & 0xF0) / 16;

    // Animate the teleporter twice
    for (var2 = 0; var2 < 2; var2++) {
        CVort_engine_setCurSound(CVort3_snd_teleport);

        for (di = 130; di < 134; di++) {
            map_data_tiles[map_width_T * teleporters[si].destY + teleporters[si].destX] = di;
            CVort_engine_syncDrawing();

            // Now we draw the messie while the teleporter is going
            CVort_engine_drawSpriteAt(messie_xpos, messie_ypos, messie_frame + (var4 & 2) / 2);
            CVort_engine_drawScreen();
            CVort_engine_delay(4);
        }
    }

    map_data_tiles[map_width_T * teleporters[si].destY + teleporters[si].destX] = 0x86;

    si = map_sprite_standing & 0xF; // This is the "dest"

    // Move the screen
    if (teleporters[si].destX < 10)
        scrollX = 0x2000;
    else
        scrollX = (teleporters[si].destX - 10) << 12;

    if (teleporters[si].destY < 6)
        scrollY = 0x2000;
    else
        scrollY = (teleporters[si].destY - 6) << 12;

    // Now repeat the animation sequence for the destination teleporter
    for (var2 = 0; var2 < 2; var2++) {
        CVort_engine_setCurSound(CVort3_snd_teleport);

        for (di = 130; di < 134; di++) {
            map_data_tiles[map_width_T * teleporters[si].destY + teleporters[si].destX] = di;
            CVort_engine_syncDrawing();

            // Now we draw the messie while the teleporter is going
            CVort_engine_drawSpriteAt(messie_xpos, messie_ypos, messie_frame + (var4 & 2) / 2);
            CVort_engine_drawScreen();
            CVort_engine_delay(4);
        }
    }

    // Finally, replace the destination teleporter tile and put keen there
    map_data_tiles[map_width_T * teleporters[si].destY + teleporters[si].destX] = 0x86;
    spritedraw->posX = teleporters[si].destX << 12;
    spritedraw->posY = teleporters[si].destY << 12;
    return 1;
}

void CVort3_do_messie(Sprite_T *keen) {
    int16_t si, di, var2, var4, var6, var8;

    int16_t messie_dir_h[] = {-2, 0, 2, -2, 0, 2, -2, 0, 2};
    int16_t messie_dir_v[] = {-2, -2, -2, 0, 0, 0, 2, 2, 2};
    uint16_t messie_frames[] = {0x88, 0x88, 0x86, 0x82, 0x82, 0x84, 0x82, 0x84, 0x84};
    uint16_t kessie_frames[] = {0x90, 0x90, 0x8E, 0x8A, 0x8A, 0x8C, 0x8A, 0x8C, 0x8C};

    if (messie_move_tics <= 8)
    {
        messie_xpos += messie_delX;
        messie_ypos += messie_delY;
    }

    // Update the messie frame
    for (si = 0; si < 9; si++)
        if ((uint32_t) (messie_dir_h[si] << 8) == messie_delX && (uint32_t) (messie_dir_v[si] << 8) == messie_delY)
            break;
    messie_frame = messie_mounted ? kessie_frames[si] : messie_frames[si];

    // Move the messie
    if (!--messie_move_tics) {

        messie_move_tics = 8;
        var2 = messie_xpos / 0x1000;
        var4 = messie_ypos / 0x1000;

        for (si = var2 - 1; var2 + 2 > si; si++) {
            var8 = 0;
            for (di = var4 - 1; var4 + 2 > di; di++) {
                var6 = map_data_sprites[di * map_width_T + si];

                if (var6 != 0x2100 && var6 != 0x2000)
                    continue;

                if (si == var2 && di == var4)
                    continue;

                if (si == messie_x_T && di == messie_y_T)
                    continue;

                if (si < var2)
                    messie_delX = -0x200;
                else if (si > var2)
                    messie_delX = 0x200;
                else
                    messie_delX = 0;

                if (di < var4)
                    messie_delY = -0x200;
                else if (di > var4)
                    messie_delY = 0x200;
                else
                    messie_delY = 0;

                messie_x_T = var2;
                messie_y_T = var4;

                // Nibbling on reeds
                if (var6 == 0x2100)
                    messie_move_tics = 130;

                var8++;
		break;
            }

            if (var8)
                goto messieUnmount;
        }
    }

messieUnmount:
    // seg000:902A
    // Handle messie unmounting
    if (messie_mounted) {
        //int16_t messie_1_x[] = {0, 1, 1, 1, 0, -1, -1, -1};
        //int16_t messie_1_y[] = {-1, -1, 0, 1, 1, 1, 0, -1};

        /* NOTE NOTE NOTE */
        /* If no direction key/button is used, currInput.direction is evaluated
         * to the value of 8, resulting in out-of-bounds (!!!) attempts to
         * access arrays messie_1_x and messie_1_y.
         * So, they'd really be a bit LONGER than intended.
         */
        int16_t messie_1_x[] = {0, 1, 1, 1, 0, -1, -1, -1, /* Extra */ -1 };
        int16_t messie_1_y[] = {-1, -1, 0, 1, 1, 1, 0, -1, /* Extra */ 0x4353 };

        GameInput_T currInput = CVort_handle_ctrl(1);

        // If keen is on a shore tile, unmount the messie if there's controller input
        if (map_data_sprites[((((messie_ypos / 0x1000) + messie_1_y[currInput.direction]) * map_width_T)&0xFFFF)+(messie_xpos / 0x1000) + messie_1_x[currInput.direction]] == 0x2200) {
            messie_mounted = 0;
            keen->posX = messie_xpos + (messie_1_x[currInput.direction] << 12);
            keen->posY = messie_ypos + (messie_1_y[currInput.direction] << 12);
            messie_time_to_climb = 30;
        }

        // Now move the screen
        if (messie_delX > 0 && messie_xpos - scrollX > 0xB000) {
            scrollX += messie_delX;
            if (scrollX > scrollX_max)
                scrollX = scrollX_max;
        } else if (messie_delX < 0 && messie_xpos - scrollX < 0x9000) {
            scrollX += messie_delX;
            if (scrollX < scrollX_min)
                scrollX = scrollX_min;
        }

        if (messie_delY > 0 && messie_ypos - scrollY > 0x7000) {
            scrollY += messie_delY;
            if (scrollY > scrollY_max)
                scrollY = scrollY_max;
        } else if (messie_delY < 0 && messie_ypos - scrollY < 0x3000) {
            scrollY += messie_delY;
            if (scrollY < scrollY_min)
                scrollY = scrollY_min;
        }
    }
}
