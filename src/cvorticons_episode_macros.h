#ifndef _CHOCOLATE_KEEN_EPISODE_MACROS_
#define _CHOCOLATE_KEEN_EPISODE_MACROS_

/* Episode-dispatch macros: resolve sprite/object/sound IDs to the correct
 * episode-specific constant at runtime, based on engine_gameVersion. */

#define VORTFRAME(a) ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_vort##a : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_vort##a : CVort3_spr_vort##a))
#define YOUTHFRAME(a) ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_youth##a : CVort3_spr_youth##a)

#define SPRKEENSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_keenshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_keenshot : CVort3_spr_keenshot))
#define SPRTANKSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_tankshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_tankshot : CVort3_spr_tankshot))
#define SPRSHOTSPLASHR ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_shotsplashr : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_shotsplashr : CVort3_spr_shotsplashr))
#define SPRSHOTSPLASHL ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_spr_shotsplashl : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_spr_shotsplashl : CVort3_spr_shotsplashl))

#define OBJNULL ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_null : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_null : CVort3_obj_null))
#define OBJDEAD ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_dead : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_dead : CVort3_obj_dead))
#define OBJZAPZOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_zapzot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_zapzot : CVort3_obj_zapzot))
#define OBJKEEN ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_keen : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_keen : CVort3_obj_keen))
#define OBJENEMYSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_enemyshot :  CVort2_obj_enemyshot)
#define OBJKEENSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_keenshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_keenshot : CVort3_obj_keenshot))
#define OBJONEBEFOREKEENSHOT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_onebeforekeenshot : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_onebeforekeenshot : CVort3_obj_onebeforekeenshot))
#define OBJVORTICON ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_obj_vorticon : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_vorticon : CVort3_obj_vorticon))
#define OBJYOUTH ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_obj_youth : CVort3_obj_youth)

#define SNDSHOTHIT ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_snd_shothit : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_snd_shothit : CVort3_snd_shothit))
#define SNDVORTSCREAM ((engine_gameVersion == GAMEVER_KEEN1) ? CVort1_snd_vortscream : ((engine_gameVersion == GAMEVER_KEEN2) ? CVort2_snd_vortscream : CVort3_snd_vortscream))

#endif
