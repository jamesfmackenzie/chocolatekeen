/* 
 * File:   cvorticons2.h (renamed from chocolate_keen2.h)
 * Author: Owen
 *
 * Created on March 31, 2013, 12:27 AM
 */

#ifndef _CHOCOLATE_KEEN_VORTICONS2_
#define	_CHOCOLATE_KEEN_VORTICONS2_

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

//class CVorticons2 : public CVorticons {
//public:

    // Here we could put in enumerated bmp names, sound names, etc

    enum CVort2_bmptype {
        CVort2_bmp_title,
        CVort2_bmp_idlogo,
        CVort2_bmp_f1help,
        CVort2_bmp_highscor,
        CVort2_bmp_name,
        CVort2_bmp_score,
        CVort2_bmp_citiessa,
        CVort2_bmp_gameover,
        CVort2_bmp_an,
        CVort2_bmp_present,
        CVort2_bmp_apogee,
        CVort2_bmp_keenship,
        CVort2_bmp_windon,
        CVort2_bmp_windoff,
        CVort2_bmp_onemomen,
        CVort2_bmp_ofan,
        CVort2_bmp_product,
        CVort2_bmp_idsoft,
    };

    enum {
        CVort2_spr_keenwalkr1,
        CVort2_spr_keenwalkr2,
        CVort2_spr_keenwalkr3,
        CVort2_spr_keenwalkr4,
        CVort2_spr_keenwalkl1,
        CVort2_spr_keenwalkl2,
        CVort2_spr_keenwalkl3,
        CVort2_spr_keenwalkl4,
        CVort2_spr_keenjumpr1,
        CVort2_spr_keenjumpr2,
        CVort2_spr_keenjumpr3,
        CVort2_spr_keenjumpr4,
        CVort2_spr_keenjumpr5,
        CVort2_spr_keenjumpr6,
        CVort2_spr_keenjumpl1,
        CVort2_spr_keenjumpl2,
        CVort2_spr_keenjumpl3,
        CVort2_spr_keenjumpl4,
        CVort2_spr_keenjumpl5,
        CVort2_spr_keenjumpl6,
        CVort2_spr_keenshootr,
        CVort2_spr_keenshootl,
        CVort2_spr_keenisdead1,
        CVort2_spr_keenisdead2,
        CVort2_spr_keenpogor1,
        CVort2_spr_keenpogor2,
        CVort2_spr_keenpogol1,
        CVort2_spr_keenpogol2,
        CVort2_spr_keensicle1,
        CVort2_spr_keensicle2,
        CVort2_spr_keensback, // 30
        CVort2_spr_keengetsup, // 31
        CVort2_spr_mapkeenr1, // 32
        CVort2_spr_mapkeenr2,
        CVort2_spr_mapkeenr3,
        CVort2_spr_mapkeenr4,
        CVort2_spr_mapkeend1, // 36
        CVort2_spr_mapkeend2,
        CVort2_spr_mapkeend3,
        CVort2_spr_mapkeend4,
        CVort2_spr_mapkeenl1,
        CVort2_spr_mapkeenl2,
        CVort2_spr_mapkeenl3,
        CVort2_spr_mapkeenl4,
        CVort2_spr_mapkeenu1,
        CVort2_spr_mapkeenu2,
        CVort2_spr_mapkeenu3,
        CVort2_spr_mapkeenu4,
        CVort2_spr_youthleft1, // 48
        CVort2_spr_youthleft2,
        CVort2_spr_youthleft3,
        CVort2_spr_youthleft4,
        CVort2_spr_youthright1,
        CVort2_spr_youthright2,
        CVort2_spr_youthright3,
        CVort2_spr_youthright4,
        CVort2_spr_youthdie1,
        CVort2_spr_youthdie2,
        CVort2_spr_tantalus1,
        CVort2_spr_tantalus2,
        CVort2_spr_firearth1,
        CVort2_spr_firearth2,
        CVort2_spr_firearth3,
        CVort2_spr_firearth4,
        CVort2_spr_earthchunk1,
        CVort2_spr_earthchunk2,
        CVort2_spr_earthchunk3,
        CVort2_spr_earthchunk4,
        CVort2_spr_lilchunk1,
        CVort2_spr_lilchunk2,
        CVort2_spr_lilchunk3,
        CVort2_spr_lilchunk4,
        CVort2_spr_lameship,
        CVort2_spr_yawn,
        CVort2_spr_vortleft1,
        CVort2_spr_vortleft2,
        CVort2_spr_vortleft3,
        CVort2_spr_vortleft4,
        CVort2_spr_vortright1,
        CVort2_spr_vortright2,
        CVort2_spr_vortright3,
        CVort2_spr_vortright4,
        CVort2_spr_vortstand1, // 82
        CVort2_spr_vortstand2,
        CVort2_spr_vortjumpl,
        CVort2_spr_vortjumpr,
        CVort2_spr_vortdie1,
        CVort2_spr_vortdie2,
        CVort2_spr_eliteleft1,
        CVort2_spr_eliteleft2,
        CVort2_spr_eliteleft3,
        CVort2_spr_eliteleft4,
        CVort2_spr_eliteright1,
        CVort2_spr_eliteright2,
        CVort2_spr_eliteright3,
        CVort2_spr_eliteright4,
        CVort2_spr_elitefirel,
        CVort2_spr_elitefirer,
        CVort2_spr_elitejumpr,
        CVort2_spr_elitejumpl,
        CVort2_spr_elitedie1,
        CVort2_spr_elitedie2,
        CVort2_spr_scrubl1,
        CVort2_spr_scrubl2,
        CVort2_spr_scrubu1,
        CVort2_spr_scrubu2,
        CVort2_spr_scrubr1,
        CVort2_spr_scrubr2,
        CVort2_spr_scrubd1,
        CVort2_spr_scrubd2,
        CVort2_spr_scrubshot,
        CVort2_spr_scrubdead,
        CVort2_spr_guardright1,
        CVort2_spr_guardright2,
        CVort2_spr_guardright3,
        CVort2_spr_guardright4,
        CVort2_spr_guardleft1,
        CVort2_spr_guardleft2,
        CVort2_spr_guardleft3,
        CVort2_spr_guardleft4,
        CVort2_spr_guardstand1,
        CVort2_spr_guardstand2,
        CVort2_spr_keenshot,
        CVort2_spr_tankshot,
        CVort2_spr_shotsplashr,
        CVort2_spr_shotsplashl,
        CVort2_spr_platform1,
        CVort2_spr_platform2,
        CVort2_spr_spark1,
        CVort2_spr_spark2,
        CVort2_spr_spark3,
        CVort2_spr_spark4,
        CVort2_spr_smallshipr,
        CVort2_spr_smallshipl,
    };

    enum CVort2_objtype {
        CVort2_obj_null = 0,
        CVort2_obj_keen = 1,
        CVort2_obj_vorticon = 2,
        CVort2_obj_youth = 3,
        CVort2_obj_elite,
        CVort2_obj_scrub,
        CVort2_obj_guardbot,
        CVort2_obj_platform,
        CVort2_obj_tantalus,

        CVort2_obj_keenshot = 10,
        CVort2_obj_onebeforekeenshot = CVort2_obj_keenshot-1, // Used by sprite_active_screen

        CVort2_obj_enemyshot = 11,
        CVort2_obj_zapzot = 12,
        CVort2_obj_dead = 13,
    };

    enum CVort2_bodytype {
        CVort2_bod_border_flasher = 4,
        CVort2_bod_tantalus_explosion = 5,
    };

    enum CVort2_sndtype {
        CVort2_snd_wldwalk = 1, // 0
        CVort2_snd_wldblock, // 1
        CVort2_snd_wldenter, // 2
        CVort2_snd_keenwalk, // 3
        CVort2_snd_keenblok, // 4
        CVort2_snd_keenjump, // 5
        CVort2_snd_keenland, // 6
        CVort2_snd_keendie, // 7
        CVort2_snd_gotbonus, // 8
        CVort2_snd_gotitem, // 9
        CVort2_snd_gotpart,
        CVort2_snd_keenfire,
        CVort2_snd_keenpogo,
        CVort2_snd_pogojump,
        CVort2_snd_lvldone,
        CVort2_snd_gameover,
        CVort2_snd_hiscore,
        CVort2_snd_teleport,
        CVort2_snd_chunksmash,
        CVort2_snd_goindoor,
        CVort2_snd_bumphead,
        CVort2_snd_usekey,
        CVort2_snd_cannonfire,
        CVort2_snd_slam,
        CVort2_snd_click,
        CVort2_snd_crystal,
        CVort2_snd_plummet,
        CVort2_snd_extraman,
        CVort2_snd_yorpbump,
        CVort2_snd_keenwlk2,
        CVort2_snd_yorpbop,
        CVort2_snd_getcard,
        CVort2_snd_dooropen,
        CVort2_snd_yorpscream,
        CVort2_snd_gargscream,
        CVort2_snd_gunclick,
        CVort2_snd_shothit,
        CVort2_snd_tankfire,
        CVort2_snd_vortscream,
        CVort2_snd_keencicle,
        CVort2_snd_keensleft,
        CVort2_snd_earthpow,
    };

    // Offsets in the EXE Image of some data files
    // A value of zero means it wasn't linked in the EXE

    enum CVort2_imageOffsets {
        CVort2_TILEINFO_OFFSET = 0x17828,
        CVort2_TRANS_MAP_OFFSET = 0x1C80C,
        CVort2_RND_VALS_OFFSET = 0x1C8B5,
        CVort2_FIBS17_OFFSET = 0x1C9DD,
        CVort2_POINTS_TABLE_OFFSET = 0x19FC2,
        CVort2_PALETTES_OFFSET = 0x19BE8,
        CVort2_CHAR_MAP_OFFSET = 0x19B2E,
        CVort2_UPPERCASE_TABLE_OFFSET = 0x1CA2B,
        CVort2_ANSI_QUIT_SCREEN_OFFSET = 0x11780,

        CVort2_SOUNDS_OFFSET = 0x12730,
        CVort2_PREVIEWS_TEXT_OFFSET = 0x163A0,
        CVort2_STORY_TEXT_OFFSET = 0x16AC0,
        CVort2_HELP_TEXT_OFFSET = 0x15BC0,
        CVort2_END_TEXT_OFFSET = 0x15840,
    };

    enum CVort2_constValues {
        CVort2_TILENUM = 689,
    };

    /*virtual*/ void CVort2_engine_processEXE();

    /*virtual*/ void CVort2_show_pause_menu();
    /*virtual*/ void CVort2_init_level(uint16_t levelnum);

    /*virtual*/ void CVort2_draw_earth_explode();
    /*virtual*/ void CVort2_inlevel_message();

    /*virtual*/ void CVort2_draw_scores();

    /*virtual*/ void CVort2_show_ordering(uint16_t isTimed);
    /*virtual*/ void CVort2_draw_ordering_info();
    /*virtual*/ void CVort2_do_ordering();

    //void CVort2_add_sprite_grunt(int16_t tileX, int16_t tileY);
    void CVort2_add_sprite_elite(int16_t tileX, int16_t tileY);
    void CVort2_add_sprite_scrub(int16_t tileX, int16_t tileY);
    void CVort2_add_sprite_guardbot(int16_t tileX, int16_t tileY);
    void CVort2_add_sprite_platform(int16_t tileX, int16_t tileY);
    void CVort2_add_sprite_tantalus(int16_t tileX, int16_t tileY);
    /*virtual*/ void CVort2_contact_keen(Sprite_T *keen, Sprite_T * contacted);
    void CVort2_think_elite_walk();
    void CVort2_think_elite_shoot();
    void CVort2_think_elite_jump();
    void CVort2_contact_elite(Sprite_T *youth, Sprite_T *contacted);
    void CVort2_think_guardbot_turn();
    void CVort2_think_guardbot_shoot();
    void CVort2_think_guardbot_move();
    void CVort2_contact_guardbot(Sprite_T *guardbot, Sprite_T * contacted);
    void CVort2_think_scrub_walk_left();
    void CVort2_think_scrub_walk_down();
    void CVort2_think_scrub_walk_right();
    void CVort2_think_scrub_walk_up();
    void CVort2_think_scrub_fall();
    void CVort2_contact_scrub(Sprite_T *scrub, Sprite_T * contacted);

    void CVort2_tantalus_explosion(uint16_t tileX, uint16_t tileY, uint16_t tilenum);
    void CVort2_body_destroy_tantalus(Body_T *tantalus);
    void CVort2_contact_tantalus(Sprite_T *tantalus, Sprite_T *contacted);
    void CVort2_think_tantalus();

    void CVort2_think_platform_turn();
    void CVort2_think_platform_move();

    void CVort2_draw_win_text();
    /*virtual*/ void CVort2_draw_win();

//protected:

    typedef struct {
        uint16_t *earth_expl3, *earth_explX, *earth_explY, *earth_expl;
    } CVort2_EXE_FIELDS_T;
    extern CVort2_EXE_FIELDS_T exeFieldsEp2;
//};

#endif	/* _CHOCOLATE_KEEN_VORTICONS2_ */

