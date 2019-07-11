/* 
 * File:   cvorticons3.h 
 * Author: Owen
 *
 * Created on April 13, 2013, 11:39 AM
 */

#ifndef _CHOCOLATE_KEEN_VORTICONS3_
#define	_CHOCOLATE_KEEN_VORTICONS3_

#include "cvorticons.h"

//#define VORTICONS_KEEN2
//#define VORTICONS_EPISODE 2

#if 0
#define GAMEEXT "CK2"
#define EXENAME "KEEN2.EXE"
//#define EXE_COMPRESSED_SIZE 58335
//#define EXE_DECOMPRESSED_SIZE

// Offsets in the EXE Image of some data files
// A value of zero means it wasn't linked in the EXE
#define TILEINFO_OFFSET 0x17828
#define TRANS_MAP_OFFSET 0x1C80C
#define RND_VALS_OFFSET 0x1C8B5
#define FIBS17_OFFSET 0x1C9DD
#define POINTS_TABLE_OFFSET 0x19FC2
#define PALETTES_OFFSET 0x19BE8
#define CHAR_MAP_OFFSET 0x19B2E
#define UPPERCASE_TABLE_OFFSET 0x1CA2B
#define ANSI_QUIT_SCREEN_OFFSET 0x11780

#define SOUNDS_OFFSET 0x12730
#define PREVIEWS_TEXT_OFFSET 0x163A0
#define STORY_TEXT_OFFSET 0x16AC0
#define HELP_TEXT_OFFSET 0x15BC0
#define END_TEXT_OFFSET 0x15840

// Values
#define TILENUM 689

// Some strings
#define LOADING_STRING
#endif

//class CVorticons3 : public CVorticons {
//public:

    // Here we could put in enumerated bmp names, sound names, etc

    enum CVort3_bmptype {
        CVort3_bmp_title,
        CVort3_bmp_idlogo,
        CVort3_bmp_f1help,
        CVort3_bmp_highscor,
        CVort3_bmp_name,
        CVort3_bmp_score,
        CVort3_bmp_gameover,
        CVort3_bmp_an,
        CVort3_bmp_present,
        CVort3_bmp_apogee,
        CVort3_bmp_onemomen,
        CVort3_bmp_ofan,
        CVort3_bmp_product,
        CVort3_bmp_idsoft,
    };

    /****** WARNING WARNING WARNING WARNING WARNING ******/
    /** spr_keensback is NOT present in vanilla Keen 3! **/

    enum {
        CVort3_spr_keenwalkr1,
        CVort3_spr_keenwalkr2,
        CVort3_spr_keenwalkr3,
        CVort3_spr_keenwalkr4,
        CVort3_spr_keenwalkl1,
        CVort3_spr_keenwalkl2,
        CVort3_spr_keenwalkl3,
        CVort3_spr_keenwalkl4,
        CVort3_spr_keenjumpr1,
        CVort3_spr_keenjumpr2,
        CVort3_spr_keenjumpr3,
        CVort3_spr_keenjumpr4,
        CVort3_spr_keenjumpr5,
        CVort3_spr_keenjumpr6,
        CVort3_spr_keenjumpl1,
        CVort3_spr_keenjumpl2,
        CVort3_spr_keenjumpl3,
        CVort3_spr_keenjumpl4,
        CVort3_spr_keenjumpl5,
        CVort3_spr_keenjumpl6,
        CVort3_spr_keenshootr,
        CVort3_spr_keenshootl,
        CVort3_spr_keenisdead1,
        CVort3_spr_keenisdead2,
        CVort3_spr_keenpogor1,
        CVort3_spr_keenpogor2,
        CVort3_spr_keenpogol1,
        CVort3_spr_keenpogol2,
        CVort3_spr_keensicle1,
        CVort3_spr_keensicle2,
        CVort3_spr_keengetsup, // 30
        CVort3_spr_mapkeenr1,  // 31 (no spr_keensback frame)
        CVort3_spr_mapkeenr2,
        CVort3_spr_mapkeenr3,
        CVort3_spr_mapkeenr4,
        CVort3_spr_mapkeend1, // 35
        CVort3_spr_mapkeend2,
        CVort3_spr_mapkeend3,
        CVort3_spr_mapkeend4,
        CVort3_spr_mapkeenl1,
        CVort3_spr_mapkeenl2,
        CVort3_spr_mapkeenl3,
        CVort3_spr_mapkeenl4,
        CVort3_spr_mapkeenu1,
        CVort3_spr_mapkeenu2,
        CVort3_spr_mapkeenu3,
        CVort3_spr_mapkeenu4,
        CVort3_spr_youthleft1, // 47
        CVort3_spr_youthleft2,
        CVort3_spr_youthleft3,
        CVort3_spr_youthleft4,
        CVort3_spr_youthright1,
        CVort3_spr_youthright2,
        CVort3_spr_youthright3,
        CVort3_spr_youthright4,
        CVort3_spr_youthdie1,
        CVort3_spr_youthdie2,
        CVort3_spr_momfirel1,
        CVort3_spr_momfirel2,
        CVort3_spr_momfirer1, // 59
        CVort3_spr_momfirer2, // 60
        CVort3_spr_godhalo,
        CVort3_spr_byehalo,
        CVort3_spr_vortleft1,
        CVort3_spr_vortleft2,
        CVort3_spr_vortleft3,
        CVort3_spr_vortleft4,
        CVort3_spr_vortright1,
        CVort3_spr_vortright2,
        CVort3_spr_vortright3,
        CVort3_spr_vortright4,
        CVort3_spr_vortstand1, // 71
        CVort3_spr_vortstand2,
        CVort3_spr_vortjumpl,
        CVort3_spr_vortjumpr,
        CVort3_spr_vortdie1,
        CVort3_spr_vortdie2,
        CVort3_spr_ninjal1,
        CVort3_spr_ninjal2,
        CVort3_spr_ninjar1,
        CVort3_spr_ninjar2,
        CVort3_spr_ninjajumpl,
        CVort3_spr_ninjajumpr,
        CVort3_spr_ninjadie1,
        CVort3_spr_ninjadie2,
        CVort3_spr_momleft1, // 85
        CVort3_spr_momleft2,
        CVort3_spr_momright1,
        CVort3_spr_momright2,
        CVort3_spr_momattackl,
        CVort3_spr_momattackr,
        CVort3_spr_momdie1,
        CVort3_spr_momdie2,
        CVort3_spr_foobl1,
        CVort3_spr_foobl2,
        CVort3_spr_foobr1,
        CVort3_spr_foobr2,
        CVort3_spr_foobyell1,
        CVort3_spr_foobyell2,
        CVort3_spr_foobdie1, // 99
        CVort3_spr_foobdie2,
        CVort3_spr_foobdie3,
        CVort3_spr_keenshot,
        CVort3_spr_tankshot,
        CVort3_spr_tankshotv,
        CVort3_spr_shotsplashr, // 105
        CVort3_spr_shotsplashl,
        CVort3_spr_platform1,
        CVort3_spr_platform2,
        CVort3_spr_ball,
        CVort3_spr_jack1, // 110
        CVort3_spr_jack2,
        CVort3_spr_jack3,
        CVort3_spr_jack4,
        CVort3_spr_spark1,
        CVort3_spr_spark2,
        CVort3_spr_spark3,
        CVort3_spr_spark4,
        CVort3_spr_meepr1,
        CVort3_spr_meepr2,
        CVort3_spr_meepl1, // 120
        CVort3_spr_meepl2,
        CVort3_spr_meepsingr,
        CVort3_spr_meepsingl,
        CVort3_spr_meepdie1,
        CVort3_spr_meepdie2,
        CVort3_spr_meepwaver1,
        CVort3_spr_meepwaver2,
        CVort3_spr_meepwavel1,
        CVort3_spr_meepwavel2,
        CVort3_spr_messield1, // 130
        CVort3_spr_messield2,
        CVort3_spr_messierd1,
        CVort3_spr_messierd2,
        CVort3_spr_messieur1,
        CVort3_spr_messieur2,
        CVort3_spr_messieul1,
        CVort3_spr_messieul2,
        CVort3_spr_kessield1,
        CVort3_spr_kessield2,
        CVort3_spr_kessierd1,
        CVort3_spr_kessierd2,
        CVort3_spr_kessieur1,
        CVort3_spr_kessieur2,
        CVort3_spr_kessieul1,
        CVort3_spr_kessieul2,
        CVort3_spr_heart1,
        CVort3_spr_heart2,
    };

    enum CVort3_objtype {
        CVort3_obj_null = 0,
        CVort3_obj_keen = 1,
        CVort3_obj_vorticon = 2,
        CVort3_obj_youth = 3,
        CVort3_obj_vortimom = 4,
        CVort3_obj_meep = 5,
        CVort3_obj_vortininja = 6,
        CVort3_obj_foob = 7,
        CVort3_obj_ball = 8,
        CVort3_obj_jack = 9,
        CVort3_obj_platform = 10,
        CVort3_obj_heart = 12,
        CVort3_obj_spark = 13,

        CVort3_obj_keenshot = 15,
        CVort3_obj_onebeforekeenshot = CVort3_obj_keenshot-1, // Used by sprite_active_screen

        CVort3_obj_enemyshot = 16,
        CVort3_obj_meepshot = 17,
        CVort3_obj_zapzot = 18,
        CVort3_obj_dead = 19
    };

    enum CVort3_bodytype {
        CVort3_bod_null = 0,
        CVort3_bod_border_flasher = 4,
        CVort3_bod_enemyshot = 5,
        CVort3_bod_mangling_arm = 6,
        CVort3_bod_mangling_leg = 7,
        CVort3_bod_mangling_arm_destruct = 8,
        CVort3_bod_mangling_destruct = 9
    };

    enum CVort3_sndtype {
        CVort3_snd_wldwalk = 1, // 0 (technically 1 due to a header)
        CVort3_snd_wldblock, // 1
        CVort3_snd_wldenter, // 2
        CVort3_snd_keenwalk, // 3
        CVort3_snd_keenblok, // 4
        CVort3_snd_keenjump, // 5
        CVort3_snd_keenland, // 6
        CVort3_snd_keendie, // 7
        CVort3_snd_gotbonus, // 8
        CVort3_snd_gotitem, // 9
        CVort3_snd_gotpart,
        CVort3_snd_keenfire,
        CVort3_snd_keenpogo,
        CVort3_snd_pogojump,
        CVort3_snd_lvldone,
        CVort3_snd_gameover,
        CVort3_snd_hiscore,
        CVort3_snd_teleport,
        CVort3_snd_chunksmash,
        CVort3_snd_goindoor,
        CVort3_snd_bumphead,
        CVort3_snd_usekey,
        CVort3_snd_cannonfire,
        CVort3_snd_slam,
        CVort3_snd_click,
        CVort3_snd_crystal,
        CVort3_snd_plummet,
        CVort3_snd_extraman,
        CVort3_snd_yorpbump,
        CVort3_snd_keenwlk2,
        CVort3_snd_yorpbop,
        CVort3_snd_getcard,
        CVort3_snd_dooropen,
        CVort3_snd_yorpscream,
        CVort3_snd_gargscream,
        CVort3_snd_gunclick,
        CVort3_snd_shothit,
        CVort3_snd_tankfire,
        CVort3_snd_vortscream,
        CVort3_snd_keencicle,
        CVort3_snd_keensleft,
        CVort3_snd_ankh,
        CVort3_snd_meep,
        CVort3_snd_mortimer,
        CVort3_snd_footslam,
    };

    // Offsets in the EXE Image of some data files
    // A value of zero means it wasn't linked in the EXE

    enum CVort3_imageOffsets {
        // Arrays in DSEG
        CVort3_TILEINFO_OFFSET = 0x198C8,
        CVort3_TRANS_MAP_OFFSET = 0x1EAFC,
        CVort3_RND_VALS_OFFSET = 0x1EBA5,
        CVort3_FIBS17_OFFSET = 0x1ECCD,
        CVort3_POINTS_TABLE_OFFSET = 0x1C083,
        CVort3_PALETTES_OFFSET = 0x1BD84,
        CVort3_CHAR_MAP_OFFSET = 0x1BCCA,
        CVort3_UPPERCASE_TABLE_OFFSET = 0x1ED1B,

        // Segments
        CVort3_ANSI_QUIT_SCREEN_OFFSET = 0x12AC0,
        CVort3_SOUNDS_OFFSET = 0x13A70,
        CVort3_HELP_TEXT_OFFSET = 0x179D0,
        CVort3_END_TEXT_OFFSET = 0x181A0,
        CVort3_PREVIEWS_TEXT_OFFSET = 0x184E0,
        CVort3_STORY_TEXT_OFFSET = 0x18BD0,
    };

    enum CVort3_constValues {
        // The Tileinfo arrays have room for 715, but there are only 624 tiles
        CVort3_TILENUM = 715,
    };

    /*virtual*/ void CVort3_engine_processEXE();

    /*virtual*/ void CVort3_show_pause_menu();
    /*virtual*/ void CVort3_init_level(uint16_t levelnum);

    /*virtual*/ void CVort3_inlevel_message();

    /*virtual*/ void CVort3_draw_scores();

    /*virtual*/ void CVort3_show_ordering(uint16_t isTimed);
    /*virtual*/ void CVort3_draw_ordering_info();
    /*virtual*/ void CVort3_do_ordering();

    //void CVort3_add_sprite_vorticon(int16_t tileX, int16_t tileY);
    //void CVort3_add_sprite_youth(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_vortimom(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_meep(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_vortininja(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_foob(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_ball(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_jack(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_platform_h(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_platform_v(int16_t tileX, int16_t tileY);
    //void CVort3_add_sprite_grunt(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_spark(int16_t tileX, int16_t tileY);
    void CVort3_add_sprite_heart(int16_t tileX, int16_t tileY);
    void CVort3_add_body_enemygun_right(int16_t tileX, int16_t tileY);
    void CVort3_add_body_enemygun_down(int16_t tileX, int16_t tileY);
    void CVort3_add_body_mangling_arm(int16_t tileX, int16_t tileY);
    void CVort3_add_body_mangling_leg(int16_t tileX, int16_t tileY, int16_t left_right);

    /*virtual*/ void CVort3_contact_keen(Sprite_T *keen, Sprite_T * contacted);
    //void CVort3_think_youth_walk();
    //void CVort3_think_youth_jump();
    //void CVort3_contact_youth(Sprite_T *youth, Sprite_T *contacted);

    void CVort3_think_vortimom_walk();
    void CVort3_think_vortimom_shoot();
    void CVort3_add_vortimomshot(int32_t posX, int32_t posY, int16_t velocity);
    void CVort3_think_vortimomshot();
    void CVort3_contact_vortimomshot(Sprite_T *shot, Sprite_T *contacted);
    void CVort3_contact_vortimom(Sprite_T *vortimom, Sprite_T *contacted);

    void CVort3_think_meep_walk();
    void CVort3_think_meep_shoot();
    void CVort3_contact_meep(Sprite_T *meep, Sprite_T *contacted);
    void CVort3_think_meepshot();

    void CVort3_think_vortininja_stand();
    void CVort3_think_vortininja_jump();
    void CVort3_contact_vortininja(Sprite_T *vortininja, Sprite_T *contacted);

    void CVort3_think_foob_walk();
    void CVort3_think_foob_run();
    void CVort3_think_foob_scared();
    void CVort3_contact_foob(Sprite_T *foob, Sprite_T *contacted);

    void CVort3_think_jack();
    void CVort3_think_ball();

    void CVort3_think_platform_move();
    void CVort3_think_platform_turn();

    void CVort3_think_enemyshot();
    void CVort3_think_enemygun_right(Body_T * enemygun);
    void CVort3_think_enemygun_down(Body_T * enemygun);

    void CVort3_think_spark();
    void CVort3_contact_spark(Sprite_T *spark, Sprite_T *contacted);

    void CVort3_think_heart();
    void CVort3_contact_heart(Sprite_T *heart, Sprite_T *contacted);

    void CVort3_destory_mangling_tile(int16_t tileX, int16_t tileY, int16_t tileId);
    void CVort3_think_mangling_arm_destruct(Body_T* body);
    void CVort3_think_mangling_destruct(Body_T* body);

    void CVort3_think_mangling_arm(Body_T *arm);
    void CVort3_think_mangling_leg_moving(Body_T *leg);
    void CVort3_think_mangling_leg_awaiting(Body_T *leg);

    // A variant of draw_box
    void CVort3_draw_box_grand_intellect_1(int16_t width, int16_t height);
    // Another such variant of draw_box
    void CVort3_draw_box_grand_intellect_2(int16_t width, int16_t height);
    int16_t CVort3_delay_grand_intellect(int16_t length);
    int16_t CVort3_draw_string_grand_intellect(const char *str);
    void CVort3_handle_grand_intellect();

    void CVort3_draw_string_win(const char *str);
    void CVort3_blink_camera_flash();
    /*virtual*/ void CVort3_draw_win();

    void CVort3_do_messie(Sprite_T* keen);
    /*virtual*/ int16_t CVort3_worldmap_sprites(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y);

//protected:

    typedef struct {
        uint8_t *palette_camFlash;
    } CVort3_EXE_FIELDS_T;
    extern CVort3_EXE_FIELDS_T exeFieldsEp3;

    //Teleporter_T CVort3_teleporters[16];
//};

#endif	/* _CHOCOLATE_KEEN_VORTICONS3_ */

