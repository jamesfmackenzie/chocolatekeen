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
