#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "core/globals.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"
#include "third_party/cgenius/fileio/compression/CRLE.h"

#include "episodes/episode_macros.h"

void CVort_add_sprite_vorticon(int16_t tileX, int16_t tileY)
{
	int16_t sprIndex = CVort_add_sprite();
	g_entities.sprites[sprIndex].type_ = OBJVORTICON;
	g_entities.sprites[sprIndex].pos_x = tileX<<12;
	g_entities.sprites[sprIndex].pos_y = tileY<<12;
	g_entities.sprites[sprIndex].think = &CVort_think_vorticon_walk;
	g_entities.sprites[sprIndex].contact = &CVort_contact_vorticon;

	if (engine_gameVersion == GAMEVER_KEEN1) {
		g_entities.sprites[sprIndex].health = 3;
		if (g_game.current_level == 16)
			g_entities.sprites[sprIndex].health = 104;
	} else {
		g_entities.sprites[sprIndex].health = 1;
	}

	if (g_entities.sprites[sprIndex].pos_x > g_entities.sprites[0].pos_x)
		g_entities.sprites[sprIndex].vel_x = -90;
	else
		g_entities.sprites[sprIndex].vel_x = 90;
	g_entities.sprites[sprIndex].frame = VORTFRAME(stand1);
}

void CVort_add_sprite_tankshot(int32_t pos_x, int32_t pos_y, int16_t vel_x) {
    int16_t sprIndex = CVort_add_sprite();
    g_entities.sprites[sprIndex].type_ = OBJENEMYSHOT;
    g_entities.sprites[sprIndex].pos_x = pos_x;
    if (engine_gameVersion == GAMEVER_KEEN1) {
        g_entities.sprites[sprIndex].pos_y = pos_y + 0x500;
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        g_entities.sprites[sprIndex].pos_y = pos_y + 0x900;
    }
    g_entities.sprites[sprIndex].think = &CVort_think_keengun;
    g_entities.sprites[sprIndex].vel_x = vel_x;
    g_entities.sprites[sprIndex].contact = &CVort_contact_tankshot;
    g_entities.sprites[sprIndex].frame = SPRTANKSHOT;
    if (vel_x >= 0) {
        if (!TILEINFO_REdge[map_data_tiles[(pos_x >> 12)+((pos_y >> 12) + 1) * map_width_tile + 1]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        g_entities.sprites[sprIndex].type_ = OBJZAPZOT;
        g_entities.sprites[sprIndex].think = &CVort_think_zapzot;
        g_entities.sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHL;
        } else {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHR;
        }
    } else {
        if (!TILEINFO_LEdge[map_data_tiles[((pos_y >> 12) + 1) * map_width_tile + (pos_x >> 12)]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        g_entities.sprites[sprIndex].type_ = OBJZAPZOT;
        g_entities.sprites[sprIndex].think = &CVort_think_zapzot;
        g_entities.sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHL;
        } else {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHR;
        }
    }
}

void CVort_add_sprite_youth(int16_t tileX, int16_t tileY) {
    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    g_entities.sprites[sprIndex].type_ = OBJYOUTH;
    g_entities.sprites[sprIndex].pos_x = tileX << 12;
    g_entities.sprites[sprIndex].pos_y = tileY << 12;
    g_entities.sprites[sprIndex].think = &CVort_think_youth_walk;
    g_entities.sprites[sprIndex].contact = &CVort_contact_youth;
    g_entities.sprites[sprIndex].health = 1;
    g_entities.sprites[sprIndex].vel_x = g_entities.sprites[0].pos_x > g_entities.sprites[sprIndex].pos_x? 250 : -250;
    g_entities.sprites[sprIndex].frame = YOUTHFRAME(left1);
}

void CVort_think_vorticon_walk()
{
	if (g_entities.temp_sprite.vel_x > 0)
		g_entities.temp_sprite.frame = VORTFRAME(right1);
	else
		g_entities.temp_sprite.frame = VORTFRAME(left1);
	g_entities.temp_sprite.frame += ((CVort_ptr_engine_getTicks()>>4)&3);
	if (CVort_get_random() < (g_game.sprite_sync<<1)
	    && ((engine_gameVersion == GAMEVER_KEEN1) || g_game.lights))
	{
		g_entities.temp_sprite.vel_y = -CVort_calc_jump_height(300);
		g_entities.temp_sprite.think = &CVort_think_vorticon_jump;
	}
	else if (CVort_get_random() < (g_game.sprite_sync<<1))
	{
		if (g_entities.temp_sprite.vel_x == 90)
			g_entities.temp_sprite.vel_x = 120;
		else if (g_entities.temp_sprite.vel_x > 90)
		{
			if (g_entities.temp_sprite.vel_x == 120)
				g_entities.temp_sprite.vel_x = 90;
		}
		else if (g_entities.temp_sprite.vel_x == -120)
			g_entities.temp_sprite.vel_x = -90;
		else if (g_entities.temp_sprite.vel_x == -90)
			g_entities.temp_sprite.vel_x = -120;
	}
	CVort_do_fall();
	int16_t currDelta = CVort_compute_sprite_delta();
	if (currDelta & 4)
		g_entities.temp_sprite.vel_x = -90;
	if (currDelta & 1)
		g_entities.temp_sprite.vel_x = 90;
        if ((engine_gameVersion != GAMEVER_KEEN1)
	    && !(currDelta & 2))
		g_entities.temp_sprite.think = &CVort_think_vorticon_jump;
}

void CVort_think_vorticon_jump()
{
	if (g_entities.temp_sprite.vel_x > 0)
		g_entities.temp_sprite.frame = VORTFRAME(jumpl);
	else
		g_entities.temp_sprite.frame = VORTFRAME(jumpr);
	CVort_do_fall();
	int16_t currDelta = CVort_compute_sprite_delta();
	if (currDelta & 2)
	{
		g_entities.temp_sprite.think = &CVort_think_vorticon_search;
		g_entities.temp_sprite.time = 0;
	}
	if (currDelta & 4)
		g_entities.temp_sprite.vel_x = -90;
	if (currDelta & 1)
		g_entities.temp_sprite.vel_x = 90;
}

void CVort_think_vorticon_search()
{
	g_entities.temp_sprite.vel_x = 0;
	g_entities.temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&3)+VORTFRAME(stand1);
	g_entities.temp_sprite.time += g_game.sprite_sync;
	if (g_entities.temp_sprite.time >= 80)
	{
		g_entities.temp_sprite.vel_x = 90;
		if (g_entities.temp_sprite.pos_x > g_entities.sprites[0].pos_x)
			g_entities.temp_sprite.vel_x = -g_entities.temp_sprite.vel_x;
		g_entities.temp_sprite.think = &CVort_think_vorticon_walk;
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
		g_entities.bodies[bodyBorderFlashIndex].type_ = 5;
		g_entities.bodies[bodyBorderFlashIndex].think_ptr = &CVort_body_border_flash;
		g_entities.bodies[bodyBorderFlashIndex].variant = 0;
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

    g_entities.temp_sprite.frame = g_entities.temp_sprite.vel_x > 0? YOUTHFRAME(right1) : YOUTHFRAME(left1);
    g_entities.temp_sprite.frame += (CVort_ptr_engine_getTicks()>>4)&3;
    if (CVort_get_random()<g_game.sprite_sync*3)
    {
       g_entities.temp_sprite.vel_y = -CVort_calc_jump_height(400);
       g_entities.temp_sprite.think = &CVort_think_youth_jump;
    }

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (!(blocking & 2))
    {
        g_entities.temp_sprite.think = &CVort_think_youth_jump;
    }

    if (blocking & 4)
    {
        g_entities.temp_sprite.vel_x = -250;
    }

    if (blocking & 1)
    {
        g_entities.temp_sprite.vel_x = 250;
    }

}

void CVort_think_youth_jump()
{
    uint16_t blocking;

    g_entities.temp_sprite.frame = g_entities.temp_sprite.vel_x > 0? YOUTHFRAME(right4) : YOUTHFRAME(left4);

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (blocking&2)
    {
        g_entities.temp_sprite.think = &CVort_think_youth_walk;
    }

    if (blocking & 4)
    {
        g_entities.temp_sprite.vel_x = -250;
    }

    if (blocking & 1)
    {
        g_entities.temp_sprite.vel_x = 250;
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

void CVort_add_sprite_keengun(int32_t pos_x, int32_t pos_y) {
    int16_t sprIndex = CVort_add_sprite();
    g_entities.sprites[sprIndex].type_ = OBJKEENSHOT;
    g_entities.sprites[sprIndex].pos_x = pos_x;
    g_entities.sprites[sprIndex].pos_y = pos_y + 0x900;
    g_entities.sprites[sprIndex].think = &CVort_think_keengun;
    g_entities.sprites[sprIndex].vel_y = 0;
    g_entities.sprites[sprIndex].contact = &CVort_contact_keengun;
    g_entities.sprites[sprIndex].frame = SPRKEENSHOT;
    if (g_game.keen_facing >= 0) {
        g_entities.sprites[sprIndex].vel_x = 400;
        if (!TILEINFO_REdge[map_data_tiles[(pos_x >> 12)+((pos_y >> 12) + 1) * map_width_tile + 1]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        g_entities.sprites[sprIndex].type_ = OBJZAPZOT;
        g_entities.sprites[sprIndex].think = &CVort_think_zapzot;
        g_entities.sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHR;
        } else {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHL;
        }
    } else {
        g_entities.sprites[sprIndex].vel_x = -400;
        if (!TILEINFO_LEdge[map_data_tiles[((pos_y >> 12) + 1) * map_width_tile + (pos_x >> 12)]])
            return;
        CVort_engine_setCurSound(SNDSHOTHIT);
        g_entities.sprites[sprIndex].type_ = OBJZAPZOT;
        g_entities.sprites[sprIndex].think = &CVort_think_zapzot;
        g_entities.sprites[sprIndex].time = 0;
        if (CVort_get_random() > 0x80) {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHR;
        } else {
            g_entities.sprites[sprIndex].frame = SPRSHOTSPLASHL;
        }
    }
}

void CVort_think_keen_ground() {
    int16_t tile_standingon_type, currtile_standingon_type, tile_collision, map_tile_left, map_tile_right, map_tile_standingon;
    if (g_entities.temp_sprite.varD)
        tile_standingon_type = 1;
    else {
        map_tile_left = g_entities.temp_sprite.box_x1 >> 12;
        map_tile_right = g_entities.temp_sprite.box_x2 >> 12;
        map_tile_standingon = (g_entities.temp_sprite.box_y2 >> 12) + 1;
        tile_standingon_type = 1;

        for (int16_t currX = map_tile_left; currX <= map_tile_right; currX++) {
            currtile_standingon_type = TILEINFO_UEdge[map_data_tiles[map_tile_standingon * map_width_tile + currX]];
            if (currtile_standingon_type > 1)
                tile_standingon_type = currtile_standingon_type;
        }
    }
    if (g_input.input_new.but1jump) {
        g_entities.temp_sprite.varC = g_entities.temp_sprite.vel_x;
        g_entities.temp_sprite.vel_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        if (g_game.keen_facing >= 0)
            g_entities.temp_sprite.varA = 8;
        else
            g_entities.temp_sprite.varA = 0xE;
        g_entities.temp_sprite.think = &CVort_think_keen_jump_ground;
    }
    if (tile_standingon_type < 3)
        switch (g_input.input_new.direction) {
            case 1:
            case 2:
            case 3:
                CVort_move_left_right(2);
                if (g_entities.temp_sprite.vel_x < 0)
                    g_input.input_new.direction = 8;
                break;
            case 5:
            case 6:
            case 7:
                CVort_move_left_right(-2);
                if (g_entities.temp_sprite.vel_x > 0)
                    g_input.input_new.direction = 8;
                break;
            default:
                break;
        }
    if ((tile_standingon_type == 1) && (g_input.input_new.direction == 8)) {
        if (g_entities.temp_sprite.vel_x > 0) {
            CVort_move_left_right(-3);
            if (g_entities.temp_sprite.vel_x < 0)
                g_entities.temp_sprite.vel_x = 0;
        } else if (g_entities.temp_sprite.vel_x < 0) {
            CVort_move_left_right(3);
            if (g_entities.temp_sprite.vel_x > 0)
                g_entities.temp_sprite.vel_x = 0;
        }
    }
    if (tile_standingon_type == 3) {
        if (g_game.keen_facing > 0)
            g_entities.temp_sprite.vel_x = 180;
        else if (g_game.keen_facing < 0)
            g_entities.temp_sprite.vel_x = -180;
    }
    if (!g_entities.temp_sprite.vel_x) {
        if (g_game.keen_facing >= 0)
            g_entities.temp_sprite.frame = 0;
        else
            g_entities.temp_sprite.frame = 4;
    } else {
        if (g_entities.temp_sprite.vel_x > 0) {
            g_entities.temp_sprite.frame = 0;
            if (tile_standingon_type < 3)
                g_entities.temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4)&3;

        } else {
            g_entities.temp_sprite.frame = 4;
            if (tile_standingon_type < 3)
                g_entities.temp_sprite.frame += (CVort_ptr_engine_getTicks() >> 4)&3;
        }
        g_game.keen_facing = g_entities.temp_sprite.vel_x;
    }
    if (g_entities.temp_sprite.vel_x && ((CVort_ptr_engine_getTicks() >> 4)&1) && (tile_standingon_type < 3)) {
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
    if (!(tile_collision & 2) && !g_entities.temp_sprite.varD) {
        g_entities.temp_sprite.think = &CVort_think_keen_jump_air;
        CVort_engine_setCurSound(0x1B);
        return;
    }
    if (g_input.input_new.but1jump) {
        g_entities.temp_sprite.varC = g_entities.temp_sprite.vel_x;
        g_entities.temp_sprite.vel_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        if (g_game.keen_facing >= 0)
            g_entities.temp_sprite.varA = 8;
        else
            g_entities.temp_sprite.varA = 0xE;
        g_entities.temp_sprite.think = &CVort_think_keen_jump_ground;
    }
    if (g_input.input_new.but2pogo && !g_input.input_old.but2pogo) {
        if (g_game.keen_switch)
            CVort_toggle_switch();
        else {
            g_entities.temp_sprite.time = 0;
            g_entities.temp_sprite.varB = g_entities.temp_sprite.vel_x;
            g_entities.temp_sprite.vel_x = 0;
            if (keen_gp.stuff[3])
                g_entities.temp_sprite.think = &CVort_think_keen_pogo_ground;
        }
    }
    if (g_input.input_new.but1jump && g_input.input_new.but2pogo && // Two-button firing
            !g_input.input_old.but1jump && !g_input.input_old.but2pogo) {
        g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        //Again???
        //g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        if (g_game.keen_facing > 0)
            g_entities.temp_sprite.frame = 0x14;
        else
            g_entities.temp_sprite.frame = 0x15;
    }
    if (g_entities.temp_sprite.varD)
        g_entities.temp_sprite.varD--;
}

void CVort_think_keen_jump_ground() {
    g_entities.temp_sprite.frame = g_entities.temp_sprite.varA + g_entities.temp_sprite.time / 6;
    if (g_input.input_new.but1jump)
        g_entities.temp_sprite.varB += g_game.sprite_sync * 6;
    else if (g_entities.temp_sprite.time < 12)
        g_entities.temp_sprite.time = 24 - g_entities.temp_sprite.time;

    switch (g_input.input_new.direction) {
        case 1: // Right (possibly diagonal)
        case 2:
        case 3:
            g_entities.temp_sprite.varC += (g_game.sprite_sync << 1);
            if (g_entities.temp_sprite.varC > 0x78)
                g_entities.temp_sprite.varC = 0x78;
            break;
        case 5: // Left (maybe diagonal)
        case 6:
        case 7:
            g_entities.temp_sprite.varC -= (g_game.sprite_sync << 1);
            if (g_entities.temp_sprite.varC < -0x78)
                g_entities.temp_sprite.varC = -0x78;
            break;
        default:
            break;
    }
    g_entities.temp_sprite.vel_x = 0;
    g_entities.temp_sprite.time += g_game.sprite_sync;
    if (g_entities.temp_sprite.time >= 36) {
        g_entities.temp_sprite.think = &CVort_think_keen_jump_air;
        g_entities.temp_sprite.vel_y -= g_entities.temp_sprite.varB;
        g_entities.temp_sprite.vel_x = g_entities.temp_sprite.varC;
        CVort_engine_setCurSound(6);
    }
    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();

    if (g_input.input_new.but1jump && g_input.input_new.but2pogo) // Two-button firing
    {
        g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        //Again... for another time???
        //g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        if (g_game.keen_facing > 0)
            g_entities.temp_sprite.frame = 0x14;
        else
            g_entities.temp_sprite.frame = 0x15;
    }
}

void CVort_think_keen_jump_air() {
    switch (g_input.input_new.direction) {
        case 1:
        case 2:
        case 3:
            CVort_move_left_right(2);
            if (g_entities.temp_sprite.vel_x < 0)
                g_input.input_new.direction = 8;
            break;
        case 5:
        case 6:
        case 7:
            CVort_move_left_right(-2);
            if (g_entities.temp_sprite.vel_x > 0)
                g_input.input_new.direction = 8;
            break;
        default:
            break;
    }
    if (g_input.input_new.direction == 8) {
        if (g_entities.temp_sprite.vel_x > 0) {
            CVort_move_left_right(-1);
            if (g_entities.temp_sprite.vel_x < 0)
                g_entities.temp_sprite.vel_x = 0;
        } else if (g_entities.temp_sprite.vel_x < 0) {
            CVort_move_left_right(1);
            if (g_entities.temp_sprite.vel_x > 0)
                g_entities.temp_sprite.vel_x = 0;
        }
    }
    if (g_game.keen_facing > 0)
        g_entities.temp_sprite.frame = 0xD;
    else
        g_entities.temp_sprite.frame = 0x13;
    if (g_entities.temp_sprite.vel_x)
        g_game.keen_facing = g_entities.temp_sprite.vel_x;

    CVort_do_fall();
    int16_t lastDelta = CVort_compute_sprite_delta();
    if (((lastDelta & 4) || (lastDelta & 1)) && ((CVort_ptr_engine_getTicks() >> 4) & 1))
        CVort_engine_setCurSound(5);
    if (lastDelta & 2) {
        g_entities.temp_sprite.think = &CVort_think_keen_ground;
        CVort_engine_setCurSound(7);
        return;
    }
    if (lastDelta & 8)
        CVort_engine_setCurSound(0x15);

    CVort_check_ceiling();
    if (g_input.input_new.but2pogo && !g_input.input_old.but2pogo) {
        if (g_game.keen_switch)
            CVort_toggle_switch();
        else if (keen_gp.stuff[3]) // Keen has gotten a pogo stick?
            g_entities.temp_sprite.think = &CVort_think_keen_pogo_air;
    }
    if (g_input.input_new.but1jump && g_input.input_new.but2pogo && // Two-button firing
            !g_input.input_old.but1jump && !g_input.input_old.but2pogo) {
        g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        //Yet again...
        //g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        if (g_game.keen_facing > 0)
            g_entities.temp_sprite.frame = 0x14;
        else
            g_entities.temp_sprite.frame = 0x15;
    }
}

void CVort_think_keen_shoot() {
    g_entities.temp_sprite.time += g_game.sprite_sync;
    if (!g_entities.temp_sprite.varB && (g_entities.temp_sprite.time > 1)) {
        if (keen_gp.ammo) {
            CVort_engine_setCurSound(0xC);
            keen_gp.ammo--;
            CVort_add_sprite_keengun(g_entities.temp_sprite.pos_x, g_entities.temp_sprite.pos_y);
        } else
            CVort_engine_setCurSound(0x24);
        g_entities.temp_sprite.varB = 1;
    }
    if ((g_entities.temp_sprite.time > 30) && !g_input.input_new.but1jump && !g_input.input_new.but2pogo)
        g_entities.temp_sprite.think = &CVort_think_keen_ground;
    if (g_entities.temp_sprite.vel_x > 0) {
        CVort_move_left_right(-1);
        if (g_entities.temp_sprite.vel_x < 0)
            g_entities.temp_sprite.vel_x = 0;
    } else if (g_entities.temp_sprite.vel_x < 0) {
        CVort_move_left_right(1);
        if (g_entities.temp_sprite.vel_x > 0)
            g_entities.temp_sprite.vel_x = 0;
    }
    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();
}

void CVort_think_keen_pogo_air() {
    g_input.input_new = CVort_handle_ctrl(1);
    switch (g_input.input_new.direction) {
        case 1:
        case 2:
        case 3:
            CVort_move_left_right(1);
            if (g_entities.temp_sprite.vel_x < 0)
                g_input.input_new.direction = 8;
            break;
        case 5:
        case 6:
        case 7:
            CVort_move_left_right(-1);
            if (g_entities.temp_sprite.vel_x > 0)
                g_input.input_new.direction = 8;
            break;
        default:
            break;
    }
    if (g_input.input_new.but1jump && (g_entities.temp_sprite.vel_y < 0))
        CVort_pogo_jump(200, -1);
    if (g_game.god_mode && g_input.input_new.but1jump) // Cheat in effect!
        g_entities.temp_sprite.vel_y = -200;
    if (g_entities.temp_sprite.vel_x)
        g_game.keen_facing = g_entities.temp_sprite.vel_x;
    if (g_game.keen_facing >= 0)
        g_entities.temp_sprite.varA = 0x18;
    else
        g_entities.temp_sprite.varA = 0x1A;
    g_entities.temp_sprite.frame = g_entities.temp_sprite.varA;
    CVort_do_fall();
    int16_t currDelta = CVort_compute_sprite_delta();
    if (((currDelta & 4) || (currDelta & 1)) && ((CVort_ptr_engine_getTicks() >> 4)&1))
        CVort_engine_setCurSound(5);
    if (currDelta & 2) {
        g_entities.temp_sprite.think = &CVort_think_keen_pogo_ground;
        g_entities.temp_sprite.time = 0;
        g_entities.temp_sprite.varB = g_entities.temp_sprite.vel_x;
        g_entities.temp_sprite.vel_x = 0;
    }
    if (currDelta & 8)
        CVort_engine_setCurSound(0x15);
    CVort_check_ceiling();
    if (g_input.input_new.but2pogo && !g_input.input_old.but2pogo) {
        if (g_game.keen_switch)
            CVort_toggle_switch();
        else
            g_entities.temp_sprite.think = &CVort_think_keen_jump_air;
    }
    if (g_input.input_new.but1jump && g_input.input_new.but2pogo) // Two-button firing
    {
        g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        //And again...
        //g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        if (g_game.keen_facing > 0)
            g_entities.temp_sprite.frame = 0x14;
        else
            g_entities.temp_sprite.frame = 0x15;
    }
    if (g_entities.temp_sprite.varD)
        g_entities.temp_sprite.varD--;
}

void CVort_think_keen_pogo_ground() {
    g_entities.temp_sprite.del_x = g_entities.temp_sprite.vel_x = 0;
    g_entities.temp_sprite.frame = g_entities.temp_sprite.varA + 1;
    g_entities.temp_sprite.time += g_game.sprite_sync;
    if (g_entities.temp_sprite.time > 22) {
        g_entities.temp_sprite.think = &CVort_think_keen_pogo_air;
        g_entities.temp_sprite.varD = 0;
        g_entities.temp_sprite.vel_y -= 200;
        g_entities.temp_sprite.vel_x = g_entities.temp_sprite.varB;
        CVort_engine_setCurSound(6);
    }
    if (g_input.input_new.but2pogo && !g_input.input_old.but2pogo)
        g_entities.temp_sprite.think = &CVort_think_keen_ground;
    if (g_input.input_new.but1jump && g_input.input_new.but2pogo) // Two-button firing
    {
        g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        g_entities.temp_sprite.del_x = 0;
        g_entities.temp_sprite.varB = 0;
        g_entities.temp_sprite.time = 0;
        //Aaand again...
        //g_entities.temp_sprite.think = &CVort_think_keen_shoot;
        if (g_game.keen_facing > 0)
            g_entities.temp_sprite.frame = 0x14;
        else
            g_entities.temp_sprite.frame = 0x15;
    }
    CVort_compute_sprite_delta();
    g_entities.temp_sprite.varD = 0;
}

void CVort_think_keen_exit() {
    int32_t time = g_entities.temp_sprite.time, time2 = g_entities.temp_sprite.varB;
    g_entities.temp_sprite.del_x = g_game.sprite_sync * 60;
    g_entities.temp_sprite.frame = (CVort_ptr_engine_getTicks() >> 4) & 3;
    g_game.keen_facing = g_entities.temp_sprite.vel_x;
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
    if ((time << 12) <= g_entities.temp_sprite.pos_x) {
        g_entities.temp_sprite.type_ = 0;
        g_game.level_finished = LEVEL_END_EXIT;
    }
}

void CVort_think_keen_death() {
    g_entities.temp_sprite.time += g_game.sprite_sync;
    if (g_entities.temp_sprite.time >= 200) {
        g_entities.temp_sprite.time = -999;
        g_entities.temp_sprite.vel_x = CVort_get_random() - 0x80;
        g_entities.temp_sprite.vel_y = -0x190;
    }
    g_entities.temp_sprite.frame = ((CVort_ptr_engine_getTicks() >> 4)&1) + 0x16;
    g_entities.temp_sprite.del_x = g_entities.temp_sprite.vel_x * g_game.sprite_sync;
    g_entities.temp_sprite.del_y = g_entities.temp_sprite.vel_y * g_game.sprite_sync;
    if (g_entities.temp_sprite.box_y2 < scroll_y)
        g_entities.temp_sprite.type_ = 0;
}

void CVort_think_keen_stunned() {

    // NOTE: We don't need to handle Keen 1? (Apparently this shared code with think_keen_frozen)
    if (engine_gameVersion == GAMEVER_KEEN1) {
        g_entities.temp_sprite.frame = CVort1_spr_keensicle1 + ((CVort_ptr_engine_getTicks()>>5)&1);
    } else if (engine_gameVersion == GAMEVER_KEEN2) {
        g_entities.temp_sprite.frame = CVort2_spr_keensicle1 + ((CVort_ptr_engine_getTicks()>>5)&1);
    } else {
        g_entities.temp_sprite.frame = CVort3_spr_keensicle1 + ((CVort_ptr_engine_getTicks()>>5)&1);
    }
    if ((g_entities.temp_sprite.time-=g_game.sprite_sync) < 0)
    {
        if (engine_gameVersion == GAMEVER_KEEN1) {
            g_entities.temp_sprite.frame = CVort1_spr_keengetsup;
        } else if (engine_gameVersion == GAMEVER_KEEN2) {
            g_entities.temp_sprite.frame = CVort2_spr_keengetsup;
        } else {
            g_entities.temp_sprite.frame = CVort3_spr_keengetsup;
        }
        if (g_entities.temp_sprite.time < -40)
        {
            g_entities.temp_sprite.think = &CVort_think_keen_ground;
        }
    }

    if (g_entities.temp_sprite.vel_x > 0)
    {
        CVort_move_left_right(-3);
        if (g_entities.temp_sprite.vel_x < 0)
        {
            g_entities.temp_sprite.vel_x = 0;
        }
    }
    else if (g_entities.temp_sprite.vel_x < 0)
    {
        CVort_move_left_right(3);
        if (g_entities.temp_sprite.vel_x > 0)
        {
            g_entities.temp_sprite.vel_x = 0;
        }
    }

    CVort_do_fall();
    CVort_compute_sprite_delta();
    CVort_check_ceiling();
}

void CVort_think_zapzot() {
    g_entities.temp_sprite.type_ = OBJDEAD;
    g_entities.temp_sprite.time += g_game.sprite_sync;
    if (g_entities.temp_sprite.time > 20) {
        g_entities.temp_sprite.type_ = OBJNULL;
    }
}

void CVort_think_keengun() {
    int16_t currDelta = CVort_compute_sprite_delta();
    if (!currDelta)
        return;
    CVort_engine_setCurSound(SNDSHOTHIT);
    g_entities.temp_sprite.type_ = OBJZAPZOT;
    g_entities.temp_sprite.think = &CVort_think_zapzot;
    g_entities.temp_sprite.time = 0;
    if (CVort_get_random() > 0x80) {
        g_entities.temp_sprite.frame = SPRSHOTSPLASHR;
    } else {
        g_entities.temp_sprite.frame = SPRSHOTSPLASHL;
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
