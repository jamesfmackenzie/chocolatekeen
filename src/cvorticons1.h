/* 
 * File:   cvorticons1.h (renamed from chocolate_keen1.h)
 * Author: Owen
 *
 * Created on March 30, 2013, 10:17 PM
 */

#ifndef _CHOCOLATE_KEEN_VORTICONS1_
#define	_CHOCOLATE_KEEN_VORTICONS1_

#include "cvorticons.h"

//#define VORTICONS_KEEN1
//#define VORTICONS_EPISODE 1

#if 0
#define GAMEEXT "CK1"
#define EXENAME "KEEN1.EXE"
//#define EXE_COMPRESSED_SIZE 51190
//#define EXE_DECOMPRESSED_SIZE

// Offsets in the EXE Image of some data files
// A value of zero means it wasn't linked in the EXE
#define TILEINFO_OFFSET 0x130F8
#define TRANS_MAP_OFFSET 0x18100
#define RND_VALS_OFFSET 0x181A9
#define FIBS17_OFFSET 0x182D1
#define POINTS_TABLE_OFFSET 0x15076
#define PALETTES_OFFSET 0x15558
#define CHAR_MAP_OFFSET 0x1549E
#define UPPERCASE_TABLE_OFFSET 0x1831F
#define ANSI_QUIT_SCREEN_OFFSET 0x12080 

#define SOUNDS_OFFSET 0
#define PREVIEWS_TEXT_OFFSET 0
#define STORY_TEXT_OFFSET 0
#define HELP_TEXT_OFFSET 0
#define END_TEXT_OFFSET 0


// Values
#define TILENUM 611

// Some strings
#define LOADING_STRING
#endif

//class CVorticons1 : public CVorticons {
//public:

    // Here we could put in enumerated bmp names, sound names, etc
    typedef enum CVort1_bmptype {
        CVort1_bmp_title,
        CVort1_bmp_idlogo,
        CVort1_bmp_f1help,
        CVort1_bmp_highscor,
        CVort1_bmp_name,
        CVort1_bmp_score,
        CVort1_bmp_parts,
        CVort1_bmp_gameover,
        CVort1_bmp_an,
        CVort1_bmp_present,
        CVort1_bmp_apogee,
        CVort1_bmp_keenship,
        CVort1_bmp_windon,
        CVort1_bmp_windoff,
        CVort1_bmp_onemomen,
        CVort1_bmp_ofan,
        CVort1_bmp_product,
        CVort1_bmp_idsoft,
    } CVort1_bmptype;

    enum {
        CVort1_spr_keenwalkr1, // 0
        CVort1_spr_keenwalkr2, // 1
        CVort1_spr_keenwalkr3, // 2
        CVort1_spr_keenwalkr4, // 3
        CVort1_spr_keenwalkl1, // 4
        CVort1_spr_keenwalkl2, // 5
        CVort1_spr_keenwalkl3, // 6
        CVort1_spr_keenwalkl4, // 7
        CVort1_spr_keenjumpr1, // 8
        CVort1_spr_keenjumpr2, // 9
        CVort1_spr_keenjumpr3, // 10
        CVort1_spr_keenjumpr4, // 11
        CVort1_spr_keenjumpr5, // 12
        CVort1_spr_keenjumpr6, // 13
        CVort1_spr_keenjumpl1, // 14
        CVort1_spr_keenjumpl2, // 15
        CVort1_spr_keenjumpl3, // 16
        CVort1_spr_keenjumpl4, // 17
        CVort1_spr_keenjumpl5, // 18
        CVort1_spr_keenjumpl6, // 19
        CVort1_spr_keenshootr, // 20
        CVort1_spr_keenshootl, // 21
        CVort1_spr_keenisdead1, // 22
        CVort1_spr_keenisdead2, // 23
        CVort1_spr_keenpogor1, // 24
        CVort1_spr_keenpogor2, // 25
        CVort1_spr_keenpogol1, // 26
        CVort1_spr_keenpogol2, // 27
        CVort1_spr_keensicle1, // 28
        CVort1_spr_keensicle2, // 29
        CVort1_spr_keensback, // 30
        CVort1_spr_keengetsup, // 31
        CVort1_spr_mapkeenr1, // 32
        CVort1_spr_mapkeenr2, // 33
        CVort1_spr_mapkeenr3, // 34
        CVort1_spr_mapkeenr4, // 35
        CVort1_spr_mapkeend1, // 36
        CVort1_spr_mapkeend2, // 37
        CVort1_spr_mapkeend3, // 38
        CVort1_spr_mapkeend4, // 39
        CVort1_spr_mapkeenl1, // 40
        CVort1_spr_mapkeenl2, // 41
        CVort1_spr_mapkeenl3, // 42
        CVort1_spr_mapkeenl4, // 43
        CVort1_spr_mapkeenu1, // 44
        CVort1_spr_mapkeenu2, // 45
        CVort1_spr_mapkeenu3, // 46
        CVort1_spr_mapkeenu4, // 47
        CVort1_spr_yorpstand1, // 48
        CVort1_spr_yorpstand2, // 49
        CVort1_spr_yorpstand3, // 50
        CVort1_spr_yorpstand4, // 51
        CVort1_spr_yorpright1, // 52
        CVort1_spr_yorpright2, // 53
        CVort1_spr_yorpleft1, // 54
        CVort1_spr_yorpleft2, // 55
        CVort1_spr_yorpstun1, // 56
        CVort1_spr_yorpstun2, // 57
        CVort1_spr_yorpcorpse1, // 58
        CVort1_spr_yorpcorpse2, // 59
        CVort1_spr_gargstand1, // 60
        CVort1_spr_gargstand2, // 61
        CVort1_spr_gargstand3, // 62
        CVort1_spr_gargstand4, // 63
        CVort1_spr_gargright1, // 64
        CVort1_spr_gargright2, // 65
        CVort1_spr_gargleft1, // 66
        CVort1_spr_gargleft2, // 67
        CVort1_spr_gargcorpse1, // 68
        CVort1_spr_gargcorpse2, // 69
        CVort1_spr_vortleft1, // 70
        CVort1_spr_vortleft2, // 71
        CVort1_spr_vortleft3, // 72
        CVort1_spr_vortleft4, // 73
        CVort1_spr_vortright1, // 74
        CVort1_spr_vortright2, // 75
        CVort1_spr_vortright3, // 76
        CVort1_spr_vortright4, // 77
        CVort1_spr_vortstand1, // 78
        CVort1_spr_vortstand2, // 79
        CVort1_spr_vortjumpl, // 80
        CVort1_spr_vortjumpr, // 81
        CVort1_spr_vortdie1, // 82
        CVort1_spr_vortdie2, // 83
        CVort1_spr_vortdie3, // 84
        CVort1_spr_vortdie4, // 85
        CVort1_spr_vortdie5, // 86
        CVort1_spr_vortdie6, // 87
        CVort1_spr_canleft1, // 88
        CVort1_spr_canleft2, // 89
        CVort1_spr_canleft3, // 90
        CVort1_spr_canleft4, // 91
        CVort1_spr_canright1, // 92
        CVort1_spr_canright2, // 93
        CVort1_spr_canright3, // 94
        CVort1_spr_canright4, // 95
        CVort1_spr_canstand1, // 96
        CVort1_spr_canstand2, // 97
        CVort1_spr_tankleft1, // 98
        CVort1_spr_tankleft2, // 99
        CVort1_spr_tankleft3, // 100
        CVort1_spr_tankleft4, // 101
        CVort1_spr_tankright1, // 102
        CVort1_spr_tankright2, // 103
        CVort1_spr_tankright3, // 104
        CVort1_spr_tankright4, // 105
        CVort1_spr_tankstand1, // 106
        CVort1_spr_tankstand2, // 107
        CVort1_spr_keenshot, // 108
        CVort1_spr_tankshot, // 109
        CVort1_spr_shotsplashr, // 110
        CVort1_spr_shotsplashl, // 111
        CVort1_spr_iceshot, // 112
        CVort1_spr_icechunk, // 113
        CVort1_spr_chain, // 114
        CVort1_spr_smallship1, // 115
        CVort1_spr_smallship2, // 116
        CVort1_spr_point, // 117
        CVort1_spr_mark, // 118
    };

    typedef enum CVort1_objtype {
        CVort1_obj_null = 0,
        CVort1_obj_keen = 1,
        CVort1_obj_2,
        CVort1_obj_vorticon = 4,

        CVort1_obj_keenshot = 10,
        CVort1_obj_onebeforekeenshot = CVort1_obj_keenshot-1, // Used by sprite_active_screen

        CVort1_obj_enemyshot = 11,
        CVort1_obj_zapzot = 13,
        CVort1_obj_dead = 14,
    } CVort1_objtype;

    typedef enum CVort1_sndtype {
        CVort1_snd_wldwalk = 1,
        CVort1_snd_wldblock,
        CVort1_snd_wldenter,
        CVort1_snd_keenwalk,
        CVort1_snd_keenblok,
        CVort1_snd_keenjump,
        CVort1_snd_keenland,
        CVort1_snd_keendie,
        CVort1_snd_gotbonus,
        CVort1_snd_gotitem,
        CVort1_snd_gotpart,
        CVort1_snd_keenfire,
        CVort1_snd_keenpogo,
        CVort1_snd_pogojump,
        CVort1_snd_lvldone,
        CVort1_snd_gameover,
        CVort1_snd_hiscore,
        CVort1_snd_teleport,
        CVort1_snd_chunksmash,
        CVort1_snd_goindoor,
        CVort1_snd_bumphead,
        CVort1_snd_usekey,
        CVort1_snd_cannonfire,
        CVort1_snd_slam,
        CVort1_snd_click,
        CVort1_snd_crystal,
        CVort1_snd_plummet,
        CVort1_snd_extraman,
        CVort1_snd_yorpbump,
        CVort1_snd_keenwlk2,
        CVort1_snd_yorpbop,
        CVort1_snd_getcard,
        CVort1_snd_dooropen,
        CVort1_snd_yorpscream,
        CVort1_snd_gargscream,
        CVort1_snd_gunclick,
        CVort1_snd_shothit,
        CVort1_snd_tankfire,
        CVort1_snd_vortscream,
        CVort1_snd_keencicle,
        CVort1_snd_keensleft,
    } CVort1_sndtype;

    // Offsets in the EXE Image of some data files
    // A value of zero means it wasn't linked in the EXE

    typedef enum CVort1_imageOffsets {
        CVort1_TILEINFO_OFFSET = 0x130F8,
        CVort1_TRANS_MAP_OFFSET = 0x18100,
        CVort1_RND_VALS_OFFSET = 0x181A9,
        CVort1_FIBS17_OFFSET = 0x182D1,
        CVort1_POINTS_TABLE_OFFSET = 0x15076,
        CVort1_PALETTES_OFFSET = 0x15558,
        CVort1_CHAR_MAP_OFFSET = 0x1549E,
        CVort1_UPPERCASE_TABLE_OFFSET = 0x1831F,
        CVort1_ANSI_QUIT_SCREEN_OFFSET = 0x12080,

        CVort1_SOUNDS_OFFSET = 0,
        CVort1_PREVIEWS_TEXT_OFFSET = 0,
        CVort1_STORY_TEXT_OFFSET = 0,
        CVort1_HELP_TEXT_OFFSET = 0,
        CVort1_END_TEXT_OFFSET = 0,
    } CVort1_imageOffsets;

    typedef enum CVort1_constValues {
        CVort1_TILENUM = 611,
    } CVort1_constValues;

    /*virtual*/ void CVort1_engine_processEXE();

    /*virtual*/ void CVort1_show_pause_menu();
    /*virtual*/ void CVort1_init_level(uint16_t levelnum);

    /*virtual*/ void CVort1_handle_secret_city();
    /*virtual*/ void CVort1_inlevel_message();

    /*virtual*/ void CVort1_draw_scores();

    /*virtual*/ void CVort1_show_ordering(uint16_t isTimed);
    /*virtual*/ void CVort1_draw_ordering_info();
    /*virtual*/ void CVort1_do_ordering();

    /*virtual*/ int16_t CVort1_worldmap_sprites(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y);

    void CVort1_add_sprite_tankbot(int16_t tileX, int16_t tileY);
    void CVort1_add_sprite_butler(int16_t tileX, int16_t tileY);
    void CVort1_add_sprite_garg(int16_t tileX, int16_t tileY);
    void CVort1_add_sprite_yorp(int16_t tileX, int16_t tileY);
    void CVort1_add_sprite_chain(int16_t tileX, int16_t tileY);
    void CVort1_add_body_cannon(int16_t tileX, int16_t tileY, int16_t variant);
    // Note the various differing formats here!
    void CVort1_add_sprite_cube(int32_t posX, int32_t posY, int16_t cannon_type);

    void CVort1_think_keen_frozen();
    /*virtual*/ void CVort1_contact_keen(Sprite_T *keen, Sprite_T * contacted);

    void CVort1_think_yorp_walk();
    void CVort1_think_yorp_look();
    void CVort1_think_yorp_stunned();
    void CVort1_contact_yorp(Sprite_T *yorp, Sprite_T *contacted);
    void CVort1_think_garg_move();
    void CVort1_think_garg_look();
    void CVort1_contact_garg(Sprite_T *garg, Sprite_T *contacted);
    void CVort1_think_butler_walk();
    void CVort1_think_butler_turn();
    void CVort1_contact_butler(Sprite_T *butler, Sprite_T *contacted);
    void CVort1_think_tankbot_move();
    void CVort1_think_tankbot_spawn();
    void CVort1_think_tankbot_turn();
    void CVort1_think_tankbot_shoot();
    void CVort1_body_ice_cannon(Body_T *cannon);
    void CVort1_think_cubette_flight();
    void CVort1_think_cube();
    void CVort1_contact_cube(Sprite_T *cube, Sprite_T *contacted);
    void CVort1_contact_chain(Sprite_T *chain, Sprite_T *contacted);
    void CVort1_body_shot_chain(Body_T *chain);

    /*virtual*/ void CVort1_draw_win();

//protected:

    typedef struct {
        uint32_t *ordering_sprites_x, *ordering_sprites_y;
        uint16_t *ordering_sprites_frames;

        // Actually, win3 is used before win1.
        int16_t *win1, *win2, *win3;
    } CVort1_EXE_FIELDS_T;
    extern CVort1_EXE_FIELDS_T exeFieldsEp1;

    //Teleporter_T teleporters[3];
//};

#endif	/* _CHOCOLATE_KEEN_VORTICONS1_ */

