#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <vector>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "cvorticons2.h"

/******************************************************************************
WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

Sprite think and contact functions, as well as body think functions, should
often be casted to CVorticons function types,
such as <Sprite_T::SpriteThinkFunction_T>. These are the functions that are
implemented only here (and not as virtual functions).
As long as the casts are done ONLY HERE (i.e. in CVorticons2 member functions),
they should be theoretically safe.
******************************************************************************/

void CVort2_show_pause_menu() {
    uint32_t origTicks = CVort_ptr_engine_getTicks();
    CVort_clear_keys();
    CVort_draw_box_opening_main(0x1C, 0xC);
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
    CVort_draw_string_80("   TARGETS SAVED      KEYS  \n");
    cursorX = origCursorX + 0x13;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0x13;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0x13;
    CVort_draw_string_80(" \n");
    cursorX = origCursorX + 0x13;
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

    CVort_engine_drawTile(origCursorX + 0x15, ((origCursorY + 3) << 3) + 4, 0x19E);

    cursorX = origCursorX + 0x18;
    cursorY = origCursorY + 0x4;
    CVort_draw_number_word(keen_gp.ammo);

    cursorX = origCursorX + 0x15;
    cursorY = origCursorY + 7;

    if (keen_gp.stuff[5])
        CVort_engine_drawTile(cursorX, (cursorY << 3), 0x1A8);
    if (keen_gp.stuff[6])
        CVort_engine_drawTile(cursorX + 4, (cursorY << 3), 0x1A9);
    if (keen_gp.stuff[7])
        CVort_engine_drawTile(cursorX, (cursorY << 3) + 0x10, 0x1AA);
    if (keen_gp.stuff[8])
        CVort_engine_drawTile(cursorX + 4, (cursorY << 3) + 0x10, 0x1AB);

    if (keen_gp.targets[0]) {
        cursorX = origCursorX;
        cursorY = origCursorY + 7;
        CVort_draw_string("London");
    }
    if (keen_gp.targets[1]) {
        cursorX = origCursorX;
        cursorY = origCursorY + 8;
        CVort_draw_string("Cairo");
    }
    if (keen_gp.targets[2]) {
        cursorX = origCursorX;
        cursorY = origCursorY + 9;
        CVort_draw_string("Sydney");
    }
    if (keen_gp.targets[3]) {
        cursorX = origCursorX;
        cursorY = origCursorY + 10;
        CVort_draw_string("New York");
    }
    if (keen_gp.targets[4]) {
        cursorX = origCursorX + 10;
        cursorY = origCursorY + 7;
        CVort_draw_string("Paris");
    }
    if (keen_gp.targets[5]) {
        cursorX = origCursorX + 10;
        cursorY = origCursorY + 8;
        CVort_draw_string("Rome");
    }
    if (keen_gp.targets[6]) {
        cursorX = origCursorX + 10;
        cursorY = origCursorY + 9;
        CVort_draw_string("Moscow");
    }
    if (keen_gp.targets[7]) {
        cursorX = origCursorX + 10;
        cursorY = origCursorY + 10;
        CVort_draw_string("Wash D.C.");
    }


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
void CVort2_init_level(uint16_t levelnum) {
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
                    CVort2_add_sprite_elite(currX, currY);
                    break;

                case 4:
                    CVort2_add_sprite_scrub(currX, currY);
                    break;

                case 5:
                    CVort2_add_sprite_guardbot(currX, currY);
                    break;

                case 6:
                    CVort2_add_sprite_platform(currX, currY);
                    break;

                case 7:
                    CVort2_add_sprite_tantalus(currX, currY);
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

void CVort2_contact_keen(Sprite_T *keen, Sprite_T *contacted) {
    switch (contacted->type_) {
        case CVort2_obj_vorticon:
        case CVort2_obj_elite:
        case CVort2_obj_guardbot:
        case CVort2_obj_enemyshot:
            CVort_kill_keen();
            break;

        case CVort2_obj_youth:
            if (keen->think == &CVort_think_keen_stunned)
                return;

            keen->think = &CVort_think_keen_stunned;
            keen->velX = contacted->velX;
            keen->velY = contacted->velY;
            keen->time = 400;
            break;

        case CVort2_obj_scrub:
        case CVort2_obj_platform: // Push Keen
            CVort_carry_keen(keen, contacted);
            break;

    }
}

void CVort2_add_sprite_elite(int16_t tileX, int16_t tileY) {

    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort2_obj_elite;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort2_think_elite_walk;
    sprites[sprIndex].contact = &CVort2_contact_elite;
    sprites[sprIndex].health = 2;
    sprites[sprIndex].velX = sprites[0].posX > sprites[sprIndex].posX ? 100 : -100;
    sprites[sprIndex].frame = CVort2_spr_eliteleft1;
}

void CVort2_think_elite_walk() {
    
    uint16_t blocking; 

    temp_sprite.frame = temp_sprite.velX > 0 ? CVort2_spr_eliteright1 : CVort2_spr_eliteleft1;
    temp_sprite.frame += (CVort_ptr_engine_getTicks()>>4)&3;

    if (CVort_get_random()<sprite_sync*2 && lights)
    {
        // Jump
        temp_sprite.velY = -CVort_calc_jump_height(300);
        temp_sprite.think = &CVort2_think_elite_jump;
    }
    else if (CVort_get_random() < sprite_sync*2)
    {
        // Charge at keen
        temp_sprite.velX = (temp_sprite.posY + 0x800 == sprites[0].posY)? 200 : 100;
        if (sprites[0].posX < temp_sprite.posX)
            temp_sprite.velX = -temp_sprite.velX;
    }
    else if (CVort_get_random() < sprite_sync * 2)
    {
        // Fire gun
        temp_sprite.think = &CVort2_think_elite_shoot;
        temp_sprite.varB = temp_sprite.time = 0;
    }

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (blocking & 4)
    {
        temp_sprite.velX = -100;
    }

    if (blocking & 1)
    {
        temp_sprite.velX = 100;
    }

    if (!(blocking&2))
    {
        temp_sprite.think = &CVort2_think_elite_jump;
    }
}

void CVort2_think_elite_shoot()
{
   temp_sprite.frame = temp_sprite.velX > 0 ? CVort2_spr_elitefirer : CVort2_spr_elitefirel;

   if ((temp_sprite.time+=sprite_sync) >= 30)
   {
       if (!temp_sprite.varB)
       {
           temp_sprite.varB = 1;
           CVort_engine_setCurSound(CVort2_snd_tankfire);
           CVort_add_sprite_tankshot(temp_sprite.posX, temp_sprite.posY - 0x100, temp_sprite.velX > 0 ? 350 : -350);
       }

       if (temp_sprite.time > 50)
       {
           temp_sprite.think = &CVort2_think_elite_walk;
       }
   }
}

void CVort2_think_elite_jump()
{
    uint16_t blocking;

    temp_sprite.frame = temp_sprite.velX > 0 ? CVort2_spr_elitejumpr : CVort2_spr_elitejumpl;

    CVort_do_fall();
    blocking = CVort_compute_sprite_delta();

    if (blocking & 2)
    {
        temp_sprite.think = &CVort2_think_elite_walk;
    }

    if (blocking & 4)
    {
        temp_sprite.velX = -100;
    }

    if (blocking & 1)
    {
        temp_sprite.velX = 100;
    }
}

void CVort2_contact_elite(Sprite_T *elite, Sprite_T *contacted)
{
    if (contacted->type_ == CVort2_obj_keenshot && elite->health-- == 0)
    {
        CVort_engine_setCurSound(CVort2_snd_vortscream);
        elite->time = 0;
        elite->varB = 2;
        elite->frame = CVort2_spr_elitedie1;
        elite->contact = &CVort_contact_nop;
        elite->think = &CVort_think_kill_sprite;
    }
}

void CVort2_add_sprite_guardbot(int16_t tileX, int16_t tileY) {

    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort2_obj_guardbot;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].velX = 100;
    sprites[sprIndex].think = &CVort2_think_guardbot_move;
    sprites[sprIndex].contact = &CVort2_contact_guardbot;
    sprites[sprIndex].health = 99;
    sprites[sprIndex].frame = CVort2_spr_guardright1;
}

void CVort2_think_guardbot_move(){

    uint16_t blocking;

    temp_sprite.frame = temp_sprite.velX > 0 ? CVort2_spr_guardright1 : CVort2_spr_guardleft1;
    temp_sprite.frame += (CVort_ptr_engine_getTicks()>>4)&3;

    if (CVort_get_random() < sprite_sync)
    {
        temp_sprite.think = &CVort2_think_guardbot_shoot;
        temp_sprite.varB = temp_sprite.time = 0;
    }

    blocking = CVort_compute_sprite_delta();

    if (blocking & 1)
    {
        temp_sprite.varB = 100;
        temp_sprite.time = 0;
        temp_sprite.think = &CVort2_think_guardbot_turn;
    }
    else if (blocking & 4)
    {
        temp_sprite.varB = -100;
        temp_sprite.time = 0;
        temp_sprite.think = &CVort2_think_guardbot_turn;
    }
}

void CVort2_think_guardbot_shoot()
{
    temp_sprite.frame = temp_sprite.velX > 0 ? CVort2_spr_guardright1 : CVort2_spr_guardleft1;
    temp_sprite.frame += (CVort_ptr_engine_getTicks()>>4)&3;
    if ((temp_sprite.time += sprite_sync)>=50)
    {
        if (temp_sprite.time > 150)
            temp_sprite.think = &CVort2_think_guardbot_move;


        if ((temp_sprite.varB+=sprite_sync)>20)
        {
            temp_sprite.varB = 0;
            CVort_engine_setCurSound(CVort2_snd_tankfire);
            CVort_add_sprite_tankshot(temp_sprite.posX, temp_sprite.posY-0x400, temp_sprite.velX > 0 ? 350 : -350);
        }
    }
}

void CVort2_think_guardbot_turn()
{
    if ((temp_sprite.time+=sprite_sync)>50)
    {
        temp_sprite.think = &CVort2_think_guardbot_move;
        temp_sprite.velX = temp_sprite.varB;
    }

    temp_sprite.frame = CVort2_spr_guardstand1 + ((CVort_ptr_engine_getTicks()>>4)&1);
}

void CVort2_contact_guardbot(Sprite_T *guardbot, Sprite_T *contacted)
{
    // Nothing here

}

void CVort2_add_sprite_scrub(int16_t tileX, int16_t tileY) {

    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort2_obj_scrub;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort2_think_scrub_walk_left;
    sprites[sprIndex].contact = &CVort2_contact_scrub;
    sprites[sprIndex].frame = CVort2_spr_scrubl1;
}

void CVort2_think_scrub_walk_left() {

    uint16_t blocking;

   temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&1) + CVort2_spr_scrubl1;
   temp_sprite.velX = -80;
   temp_sprite.velY = 80;
   if (temp_sprite.time == 0)
   {
       temp_sprite.delY += 0x400;
   }

   blocking = CVort_compute_sprite_delta();

   if (blocking & 1) 
   {
       temp_sprite.think = &CVort2_think_scrub_walk_up;
       temp_sprite.time = 0;
       return;
   }

   if (!(blocking & 2))
   {
       if (temp_sprite.time)
       {
       temp_sprite.think = &CVort2_think_scrub_walk_down;
       temp_sprite.time = 0;
       temp_sprite.posY += 0x100;
       return;
       }

       temp_sprite.think = CVort2_think_scrub_fall;
   }

   if (!temp_sprite.time)
       temp_sprite.time = 1;

} 

void CVort2_think_scrub_walk_down(){

    uint16_t blocking;

   temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&1) + CVort2_spr_scrubd1;
   temp_sprite.velX = 80;
   temp_sprite.velY = 80;
   if (temp_sprite.time == 0)
   {
       temp_sprite.delX += 0x400;
   }

   blocking = CVort_compute_sprite_delta();

   if (blocking & 2) 
   {
       temp_sprite.think = &CVort2_think_scrub_walk_left;
       temp_sprite.time = 0;
       return;
   }

   if (!(blocking & 4))
   {
       if (temp_sprite.time)
       {
       temp_sprite.think = &CVort2_think_scrub_walk_right;
       temp_sprite.time = 0;
       temp_sprite.posX += 0x100;
       return;
       }

       temp_sprite.think = &CVort2_think_scrub_fall;
   }

   if (!temp_sprite.time)
       temp_sprite.time = 1;

}

void CVort2_think_scrub_walk_right(){

    uint16_t blocking;

   temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&1) + CVort2_spr_scrubr1;
   temp_sprite.velX = 80;
   temp_sprite.velY = -80;
   if (temp_sprite.time == 0)
   {
       temp_sprite.delY -= 0x400;
   }

   blocking = CVort_compute_sprite_delta();

   if (blocking & 4) 
   {
       temp_sprite.think = &CVort2_think_scrub_walk_down;
       temp_sprite.time = 0;
       return;
   }

   if (!(blocking & 8))
   {
       if (temp_sprite.time)
       {
       temp_sprite.think = &CVort2_think_scrub_walk_up;
       temp_sprite.time = 0;
       temp_sprite.posY -= 0x100;
       return;
       }

       temp_sprite.think = &CVort2_think_scrub_fall;
   }

   if (!temp_sprite.time)
       temp_sprite.time = 1;

}

void CVort2_think_scrub_walk_up(){

    uint16_t blocking;

   temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&1) + CVort2_spr_scrubu1;
   temp_sprite.velX = -80;
   temp_sprite.velY = -80;
   if (temp_sprite.time == 0)
   {
       temp_sprite.delX -= 0x400;
   }

   blocking = CVort_compute_sprite_delta();

   if (blocking & 8) 
   {
       temp_sprite.think = &CVort2_think_scrub_walk_right;
       temp_sprite.time = 0;
       return;
   }

   if (!(blocking & 1))
   {
       if (temp_sprite.time)
       {
       temp_sprite.think = &CVort2_think_scrub_walk_left;
       temp_sprite.time = 0;
       temp_sprite.posX -= 0x100;
       return;
       }

       temp_sprite.think = &CVort2_think_scrub_fall;
   }

   if (!temp_sprite.time)
       temp_sprite.time = 1;

}

void CVort2_think_scrub_fall()
{
    uint16_t blocking;
    
   temp_sprite.frame = ((CVort_ptr_engine_getTicks()>>5)&1) + CVort2_spr_scrubl1;
   temp_sprite.velX = 0;
   CVort_do_fall();
   blocking  = CVort_compute_sprite_delta();

   if (blocking & 2)
   {
       temp_sprite.think = &CVort2_think_scrub_walk_left;
       temp_sprite.time = 0;
   }
}

void CVort2_contact_scrub(Sprite_T *scrub, Sprite_T *contacted)
{
    if (contacted->type_ == CVort2_obj_keenshot || contacted->type_ == CVort2_obj_enemyshot)
    {
        scrub->time = 0;
        scrub->varB = 2;
        scrub->frame = CVort2_spr_scrubshot;
        scrub->contact = &CVort_contact_nop;
        scrub->think = &CVort_think_kill_sprite;
    }
}


void CVort2_add_sprite_platform(int16_t tileX, int16_t tileY) {

    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort2_obj_platform;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = (tileY << 12) - 0x400;
    sprites[sprIndex].think = &CVort2_think_platform_move;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].frame = CVort2_spr_platform1;
    sprites[sprIndex].velX = 75;
}

void CVort2_think_platform_move()
{
    uint16_t blocking;

    temp_sprite.frame = CVort2_spr_platform1 + ((CVort_ptr_engine_getTicks()>>5)&1);

    blocking = CVort_compute_sprite_delta();

    if (blocking&1 || blocking&4) {
        if (blocking&1) {
            temp_sprite.varB = 75;
        } else /* if (blocking&4) */ {
            temp_sprite.varB = -75;
        }
        
        temp_sprite.velX = temp_sprite.time = 0;
        temp_sprite.think = &CVort2_think_platform_turn;
    }
}

void CVort2_think_platform_turn() {
    
    if ((temp_sprite.time+=sprite_sync)>75) {
        temp_sprite.velX = temp_sprite.varB;
        temp_sprite.think = &CVort2_think_platform_move;
    }
}

void CVort2_add_sprite_tantalus(int16_t tileX, int16_t tileY) {

    uint16_t sprIndex;

    sprIndex = CVort_add_sprite();
    sprites[sprIndex].type_ = CVort2_obj_tantalus;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].think = &CVort2_think_tantalus;
    sprites[sprIndex].contact = &CVort2_contact_tantalus;
    sprites[sprIndex].frame = CVort2_spr_spark1;
}

void CVort2_think_tantalus() {

    uint16_t blocking;

    temp_sprite.frame = CVort2_spr_spark1 + ((CVort_ptr_engine_getTicks()>>3)&3);

    blocking = CVort_compute_sprite_delta();

    // Odd ... they don't move anyways
    if (blocking & 1)
    {
        temp_sprite.velX = 75;
    }
    else if (blocking & 4)
    {
        temp_sprite.velX = -75;
    }
}

void CVort2_contact_tantalus(Sprite_T *tantalus, Sprite_T *contacted)
{
    // This is unexpected... ANYTHING touching the tantalus will save the city!
    switch (current_level)
    {
        case 4:
            keen_gp.targets[0] = 1;
            break;
        case 6:
            keen_gp.targets[1] = 1;
            break;
        case 7:
            keen_gp.targets[2] = 1;
            break;
        case 9:
            keen_gp.targets[3] = 1;
            break;
        case 11:
            keen_gp.targets[4] = 1;
            break;
        case 13:
            keen_gp.targets[5] = 1;
            break;
        case 15:
            keen_gp.targets[6] = 1;
            break;
        case 16:
            keen_gp.targets[7] = 1;
            break;
    }

    // But only keen's shot will cause an explosion
    if (contacted->type_ == CVort2_obj_keenshot)
    {
        Body_T *explosion, *flasher;
        uint16_t bodyNum;

        tantalus->type_ = CVort2_obj_null;
        CVort_add_score(10000);
        bodyNum = CVort_add_body();
        bodies[bodyNum].type_ = CVort2_bod_tantalus_explosion;
        bodies[bodyNum].think_ptr = &CVort2_body_destroy_tantalus;
        bodies[bodyNum].tile_x = tantalus->posX >> 12;
        bodies[bodyNum].tile_y = tantalus->posY >> 12;
        bodies[bodyNum].field_C = bodies[bodyNum].variant = 0;

        bodyNum = CVort_add_body();
        bodies[bodyNum].type_ = CVort2_bod_border_flasher;
        bodies[bodyNum].think_ptr = &CVort_body_border_flash;
        bodies[bodyNum].variant = 0;
    }
}


void CVort2_body_destroy_tantalus(Body_T *tantalus)
{
    uint16_t var4, var_si;

    // Wait 40 ticks
    if ((tantalus->variant+=sprite_sync)<40)
       return; 

    // And then do the next step
    tantalus->variant -= 40;

    switch (tantalus->field_C++)
    {
        case 0:
        map_data_tiles[(tantalus->tile_y - 1) * map_width_T + tantalus->tile_x - 1] = 0x8F;
        map_data_tiles[(tantalus->tile_y - 1) * map_width_T + tantalus->tile_x] = 0x8F;
        map_data_tiles[(tantalus->tile_y - 1) * map_width_T + tantalus->tile_x + 1] = 0x8F;
        map_data_tiles[(tantalus->tile_y) * map_width_T + tantalus->tile_x - 1] = 0x222;
        map_data_tiles[(tantalus->tile_y) * map_width_T + tantalus->tile_x] = 0x223;
        map_data_tiles[(tantalus->tile_y) * map_width_T + tantalus->tile_x + 1] = 0x224;
        map_data_tiles[(tantalus->tile_y+4) * map_width_T + tantalus->tile_x - 3] = 0x1FA;
        return;

        case 1:
            CVort2_tantalus_explosion(tantalus->tile_x-2, tantalus->tile_y, 0x1EC);
            return;
          
        case 2:
            CVort2_tantalus_explosion(tantalus->tile_x+2, tantalus->tile_y, 0x1EC);
            return;

        case 3:
        case 4:
        case 5:
            CVort2_tantalus_explosion(tantalus->tile_x, tantalus->tile_y+tantalus->field_C-1, 0x1F9);
            return;

        case 6:
        case 7:
        case 8:
        case 9:
            for (var_si = tantalus->tile_y+3;var_si<tantalus->tile_y+6; var_si++)
                CVort2_tantalus_explosion(tantalus->field_C-4+tantalus->tile_x, var_si, 0x225);

            return;

        default:
            for (var4 = tantalus->tile_x-7; var4<tantalus->tile_x-4; var4++)
            {
                for (var_si = tantalus->tile_y+2; var_si<tantalus->tile_y+5; var_si++)
                {
                  map_data_tiles[(var_si) * map_width_T + var4] = 0x215;
                }

            }

            tantalus->type_ = 0;
            return;
    }
     
}

void CVort2_tantalus_explosion(uint16_t tileX, uint16_t tileY, uint16_t tilenum)
{
    uint16_t sprIndex;

    CVort_engine_setCurSound(CVort2_snd_shothit);

    sprIndex = CVort_add_sprite();

    sprites[sprIndex].think = &CVort_think_zapzot;
    sprites[sprIndex].type_ = CVort2_obj_dead;
    sprites[sprIndex].posX = tileX << 12;
    sprites[sprIndex].posY = tileY << 12;
    sprites[sprIndex].time = 0;
    sprites[sprIndex].contact = &CVort_think_contact_nop;
    sprites[sprIndex].frame = CVort_get_random()>0x80 ? CVort2_spr_shotsplashl : CVort2_spr_shotsplashr;

    map_data_tiles[(tileY) * map_width_T + tileX] = tilenum;
}


void CVort2_inlevel_message() {
    uint32_t tickcount = CVort_ptr_engine_getTicks();
    CVort_draw_box(0x16, 0xD);
    switch (current_level) {
        case 8:
            CVort_draw_string("The Elder Vorticon    \n");
            CVort_draw_string("in the stasis field   \n");
            CVort_draw_string("says:                 \n");
            CVort_draw_string("The wise Vorticon     \n");
            CVort_draw_string("never jumps in the    \n");
            CVort_draw_string("dark.  In fact, even  \n");
            CVort_draw_string("unwise Vorticons will \n");
            CVort_draw_string("not jump in darkness. \n");
            break;
        case 10:
            CVort_draw_string("The Vorticon Elder    \n");
            CVort_draw_string("says through the      \n");
            CVort_draw_string("stasis field:         \n");
            CVort_draw_string("The Grand Intellect   \n");
            CVort_draw_string("is not from Vorticon  \n");
            CVort_draw_string("VI--he is from the    \n");
            CVort_draw_string("planet Earth. His evil\n");
            CVort_draw_string("Mind-Belts control    \n");
            CVort_draw_string("their minds.  They are\n");
            CVort_draw_string("not evil. Please do   \n");
            CVort_draw_string("not shoot them, human.\n");
            break;
        default:
            break;
    }
    cursorY = textbox_offs_y + 7;
    CVort_draw_stringz("Press ENTER:");
    CVort_clear_keys();
    while ((CVort_read_char_with_echo()&0xFF) != 0xD); // ENTER/Return key
    CVort_engine_clearOverlay();
    CVort_clear_keys();
    CVort_ptr_engine_setTicks(tickcount);
}

void CVort2_draw_earth_explode() {

    uint32_t var14, var18;
    uint32_t earthY, earthX;
    uint16_t di, explodeTick;
    uint32_t var1C;

    CVort_load_level_data(81);
    scrollX = scrollY = 0;
    CVort_engine_syncDrawing();
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_fade_in();
    CVort_engine_delay(120);
    CVort_engine_setCurSound(CVort2_snd_tankfire);
    var14 = 0x6000;
    var18 = 0x5000;

    // Tantalus flies southeast
    for (di = 0; di <= 20; di++) {
        // if (call_keyb_int(1)&0xFF==0x1B) { call_keyb_int(0); return; }
        // HACK (sort of): Check if the last key pressed is Escape
        if (engine_lastScanCode == 1)
            return;

        CVort_engine_syncDrawing();

        // Move the tantalus in 20 equal steps
        var14 = 0x6000 + ((0x29000 * di) / 20);
        var18 = 0x5000 + ((0x11000 * di) / 20);

        scrollX = var14 - 0x6000;
        scrollY = var18 - 0x5000;

        CVort_engine_drawSpriteAt(var14, var18, CVort2_spr_tantalus1 + (di % 2));
        CVort_engine_drawScreen();
    }


    // The Earth Explodes!
    CVort_engine_setCurSound(CVort2_snd_earthpow);
    earthX = 0x2F000;
    earthY = 0x16000;

    for (explodeTick = 0; explodeTick < 60; explodeTick++) {
        CVort_engine_syncDrawing();

        // Draw over the Earth tiles
        if (explodeTick == 20) {
            uint16_t var2, si;

            for (var2 = 0; var2 <= 2; var2++) {
                for (si = 0; si <= 2; si++) {
                    map_data_tiles[map_width_T * (0x16 + var2) + 0x2F + si] = 0x9B;
                }
            }

        }

        // Draw the eight little chunks
        if (explodeTick >= 15) {
            var1C = (explodeTick - 15)*0xA00;
            CVort_engine_drawSpriteAt(earthX - var1C / 2, earthY - var1C, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);
            CVort_engine_drawSpriteAt(earthX + var1C / 2 + 0x1000, earthY - var1C, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);
            // etc etc
            CVort_engine_drawSpriteAt(earthX - var1C / 2, earthY + var1C + 0x1000, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);
            CVort_engine_drawSpriteAt(earthX + var1C / 2 + 0x1000, earthY + var1C + 0x1000, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);

            //
            CVort_engine_drawSpriteAt(earthX - var1C, earthY - var1C / 2, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);
            CVort_engine_drawSpriteAt(earthX + var1C + 0x1000, earthY - var1C / 2, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);
            CVort_engine_drawSpriteAt(earthX - var1C, earthY + var1C / 2 + 0x1000, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);
            CVort_engine_drawSpriteAt(earthX + var1C + 0x1000, earthY + var1C / 2 + 0x1000, explodeTick / 2 % 4 + CVort2_spr_lilchunk1);


        }

        // Draw the four big earth chunks
        if (explodeTick >= 20) {
            var1C = ((((uint32_t) explodeTick - 20) << 3) << 8);
            CVort_engine_drawSpriteAt(earthX - var1C, earthY - var1C, explodeTick / 2 % 4 + CVort2_spr_earthchunk1);
            CVort_engine_drawSpriteAt(earthX + var1C + 0x1000, earthY - var1C, explodeTick / 2 % 4 + CVort2_spr_earthchunk1);
            CVort_engine_drawSpriteAt(earthX - var1C, earthY + var1C + 0x1000, explodeTick / 2 % 4 + CVort2_spr_earthchunk1);
            CVort_engine_drawSpriteAt(earthX + var1C + 0x1000, earthY + var1C + 0x1000, explodeTick / 2 % 4 + CVort2_spr_earthchunk1);


        }
#if 0
        static uint16_t earth_explX[] = {2, 1, 3, 1, 3, 0, 2, 2, 4};
        static uint16_t earth_explY[] = {2, 1, 1, 3, 3, 2, 0, 4, 2};
        static uint16_t earth_expl[] = {0, 2, 4, 6, 8, 10, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40};
        static uint16_t earth_expl3[] = {0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 1, 1, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
#endif

        // Draw the nine earth explosions
        for (di = 0; di < 9; di++) {
            int16_t var6, var8;

            if ((var8 = explodeTick - exeFieldsEp2.earth_expl[di]) < 0)
                continue;

            if ((var6 = exeFieldsEp2.earth_expl3[var8]) == -1)
                continue;

            CVort_engine_drawSpriteAt(
                    ((uint32_t) (exeFieldsEp2.earth_explX[di] + 0x5E) << 12) / 2,
                    ((uint32_t) (exeFieldsEp2.earth_explY[di] + 0x2C) << 12) / 2,
                    CVort2_spr_firearth1 + var6
                    );
        }


        CVort_engine_drawScreen();
        CVort_engine_delay(3);

    }


}

void CVort2_draw_win_text()
{
    CVort_draw_char_rectangle(textbox_x_pos, textbox_y_pos, textbox_width, textbox_height, 0x20);
}

void CVort2_draw_win()
{
    uint32_t mothershipX, mothershipY;
    uint32_t bwbX, bwbY;
    uint16_t si;

    CVort_load_level_data(81);

    scrollX = scrollY = 0;
    CVort_engine_syncDrawing();
    CVort_engine_clearOverlay();
    
    bwbX = 0x7000;
    bwbY = 0x3000;

    CVort_engine_drawSpriteAt(bwbX,bwbY,CVort2_spr_smallshipr);
    CVort_engine_drawScreen();

    CVort_draw_box2(4, 0x11, 0x24, 0x18);
    
    CVort_fade_in();

    CVort_draw_string_finale("After disabling the weaponry of\n");
    CVort_draw_string_finale("the Vorticon Mothership, Billy\n");
    CVort_draw_string_finale("heads for earth.  Even great\n");
    CVort_draw_string_finale("space heroes need a nap after\n");
    CVort_draw_string_finale("defeating a vicious horde\n");
    CVort_draw_string_finale("of violence-bent aliens!");

    CVort_engine_delay(300);

    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();

    // Draw the BWB flying back to earth
    for (si = 0; si < 120; si++)
    {
        // if (call_keyb_int(1)&0xFF==0x1B) { call_keyb_int(0); return; }
        // HACK (sort of): Check if the last key pressed is Escape
        if (engine_lastScanCode == 1)
            return;

        CVort_engine_syncDrawing();

        bwbX = ((uint32_t)si*0x28800)/120+0x7000;;
        bwbY = ((uint32_t)si*0x13800)/120+0x3000;
        scrollX = bwbX - 0x7000;
        scrollY = bwbY - 0x3000;
        CVort_engine_drawSpriteAt(bwbX, bwbY, CVort2_spr_smallshipr);
        CVort_engine_drawScreen();
        
    }

    CVort_engine_delay(120);
    CVort_fade_out();


    // Mothership limps back to Vorticon VI
    CVort_engine_syncDrawing();

    scrollX = 0;
    scrollY = 0xC000;

    mothershipX = 0xA000;
    mothershipY = 0x11000;
    CVort_engine_drawSpriteAt(mothershipX, mothershipY, CVort2_spr_lameship);

    CVort_engine_drawScreen();
    CVort_fade_in();

    CVort_draw_box2(4, 0x11, 0x24, 0x18);
    CVort_draw_string_finale("The Vorticon ship limps back\n");
    CVort_draw_string_finale("toward Vorticon VI to tell of  \n");
    CVort_draw_string_finale("their defeat at the hands of\n");
    CVort_draw_string_finale("Commander Keen.  The Grand\n");
    CVort_draw_string_finale("Intellect will not be pleased.\n");

    CVort_engine_delay(120);
    CVort_engine_clearOverlay();
    
    for (si = 0; si < 120; si++)
    {
        CVort_engine_syncDrawing();
        mothershipX -= 0x100;
        mothershipY -= 0x100;
        CVort_engine_drawSpriteAt(mothershipX, mothershipY, CVort2_spr_lameship);
        CVort_engine_drawScreen();
    }

    CVort_fade_out();


    // Back on earth, snow day!

    scrollX &= 0xFFFF000; // This is a mistake in the original code
    // It should be 0xFFFFF000; But this is Chocolate Keen, so we don't fix it!
    CVort_engine_drawScreen();
    CVort_engine_showImageFile("FINALE.CK2");
    CVort_load_level_data(81);
    CVort_fade_in();

    CVort_draw_box2(4, 0x12, 0x1D, 0x16);
    CVort_draw_string_finale("Wake up, Billy.  It\n");
    CVort_draw_string_finale("Snowed last night!\n");
    CVort_draw_string_finale("There's no school!");
    CVort_engine_delay(120);

    CVort2_draw_win_text();
    CVort_draw_box2(4, 0x10, 0x1D, 0x18);
    CVort_draw_string_finale("Wonderful, Mother.  That\n");
    CVort_draw_string_finale("will give me time to rid\n");
    CVort_draw_string_finale("the Galaxy of the\n");
    CVort_draw_string_finale("Vorticon menace and\n");
    CVort_draw_string_finale("discover the secret of\n");
    CVort_draw_string_finale("the mysterious Grand\n");
    CVort_draw_string_finale("intellect!\n");
    CVort_engine_delay(120);

    CVort2_draw_win_text();
    CVort_draw_box2(0x4, 0x12, 0x1D, 0x17);
    CVort_draw_string_finale("Ok, hon, but you'd\n");
    CVort_draw_string_finale("better have a nourishing\n");
    CVort_draw_string_finale("vitamin fortified bowl\n");
    CVort_draw_string_finale("of Sugar Stoopies first.\n");
    CVort_engine_delay(120);

    CVort2_draw_win_text();
    CVort_draw_box2(0x4, 0x12, 0x10, 0x14);
    CVort_draw_string_finale("Ok, mom...");
    CVort_engine_delay(120);


    CVort2_draw_win_text();
    CVort_draw_box2(0xC, 0x3, 0x20, 0x5);
    CVort_draw_string_finale("TO BE CONTINUED....");
    CVort_engine_delay(400);

    scrollX &= 0xFFFFF000;
    CVort_engine_clearOverlay();
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();

    CVort_do_text_viewer_short(end_text, 0, 0x16);
    CVort_engine_setCurSound(CVort2_snd_keensleft);
    CVort_engine_finishCurSound();
    CVort_do_text_viewer(end_text, 0, 0x16);

    return;
}

/*
 * Ordering info (Scrub going around)
 */
void CVort2_show_ordering(uint16_t isTimed) {
    uint16_t frames[] = {CVort2_spr_scrubr1, CVort2_spr_scrubd1, CVort2_spr_scrubl1, CVort2_spr_scrubu1};

    int32_t scrub_vel_x = 2, scrub_vel_y = 0;
    uint32_t scrub_pos_x = 0, scrub_pos_y = 4;
    uint16_t leg_raised = 0;
    uint16_t var22, var24 = 0; // Dunno what these were for
    uint16_t frame, frameTicker = 0;

    CVort2_do_ordering();
    CVort_fade_in();
    CVort_clear_keys();
    int16_t ticksCounter = 2400; // DI

    GameInput_T currInput;

    do {
        currInput = CVort_handle_ctrl(1);
        CVort_engine_syncDrawing();

        // Move the scrub
        if (scrub_vel_x > 0) {
            //9DA8
            frame = frames[0];
            scrub_pos_x += scrub_vel_x * 256;
            if (scrub_pos_x > 0x12E00) {
                scrub_vel_x = 0;
                scrub_vel_y = 2;
            }
        }

        if (scrub_vel_x < 0) {
            frame = frames[2];
            scrub_pos_x += scrub_vel_x * 256;
            if (scrub_pos_x < 2) {
                scrub_vel_x = 0;
                scrub_vel_y = -2;
            }
        }

        if (scrub_vel_y > 0) {
            frame = frames[1];
            scrub_pos_y += scrub_vel_y * 256;
            if (scrub_pos_y > 0xB800) {
                scrub_vel_x = -2;
                scrub_vel_y = 0;
            }
        }

        if (scrub_vel_y < 0) {
            frame = frames[3];
            scrub_pos_y += scrub_vel_y * 256;
            if (scrub_pos_y < 6) {
                scrub_vel_x = 2;
                scrub_vel_y = 0;
            }
        }

        // Toggle the walking
        if (frameTicker % 4 == 0)
            leg_raised ^= 1;

        CVort_engine_drawSpriteAt(scrub_pos_x + scrollX, scrub_pos_y + scrollY, frame + leg_raised);

        CVort_engine_drawScreen();
        frameTicker++;

        if (isTimed)
            ticksCounter -= sprite_sync;
        if (CVort_handle_global_keys()) {
            CVort_clear_keys();
            CVort2_do_ordering();
        }
        if (currInput.but1jump || currInput.but2pogo || CVort_translate_key(1))
            ticksCounter = 0;
    } while (ticksCounter > 0);
}

void CVort2_draw_ordering_info() {
    cursorX = cursorX_save = 6;
    cursorY = 3;
    CVort_draw_string_80("   Commander Keen: Invasion of the  \n");
    CVort_draw_string_80(" Vorticons consists of three unique \n");
    CVort_draw_string_80("      and challenging episodes:\n\n");
    CVort_draw_string_80(" Order the trilogy for $30 and get:\n");
    CVort_draw_string_80("* The \"Secret Hints & Tricks\" sheet\n");
    CVort_draw_string_80("* The special \"cheat mode\" password\n");
    CVort_draw_string_80("* The latest version of each game\n");
    CVort_draw_string_80("* SEVERAL FREE BONUS GAMES!\n\n");
    CVort_draw_string_80("          Mail orders to:\n");
    CVort_draw_string_80("          Apogee Software\n");
    CVort_draw_string_80("          P.O. Box 476389\n");
    CVort_draw_string_80("          Garland, TX 75047\n\n");
    CVort_draw_string_80("  U.S. funds only; checks or M/O's\n");
    CVort_draw_string_80("   Include $2 postage & handling\n");
    CVort_draw_string_80("Specify 5.25/3.5 disk when ordering.\n");
    CVort_draw_string_80(" Or order toll free: 1-800-852-5659 ");
}

void CVort2_do_ordering() {
    scrollX = 0x16000;
    scrollY = 0x2000;
    // TODO: Is this a long shift left indeed?
    scrollX_T = scrollX << 12;
    scrollY_T = scrollY << 12;
    scrollY -= 0x400;
    CVort_engine_clearOverlay();
    CVort_engine_syncDrawing();
    draw_func = &CVort2_draw_ordering_info;
    //draw_func = static_cast<CVorticons::DrawFunction_T>(&CVort2_draw_ordering_info);
    CVort_engine_drawScreen();
    CVort_engine_drawScreen();
    draw_func = 0;
    CVort_clear_keys();
}

void CVort2_draw_scores() {
    uint16_t var_2, var_4, extra_var;
    char radixStr[10];
    CVort_engine_drawBitmap(0xf, 7, 3);
    CVort_engine_drawBitmap(9, 0x2c, 4);
    CVort_engine_drawBitmap(0x17, 0x2c, 5);
    CVort_engine_drawBitmap(0x21, 0x28, 6);  

    for (uint16_t currEntry = 0; currEntry < 7; currEntry++) {
        // FIXME: Are the correct "long divisions" done here?
        var_2 = (scrollX / 0x1000) & 0xFFFF;
        var_4 = (scrollY / 0x1000) & 0xFFFF;
        extra_var = currEntry % 4;
        cursorY = (currEntry << 1) + 8;
        cursorX = 0x24;
        sprintf(radixStr, "%" PRIu16, high_scores_table.targets[currEntry]);
        CVort_draw_string_80(radixStr);
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
