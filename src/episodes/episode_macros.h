// episode_macros.h: Declares episode macros interfaces for the episodes subsystem.

#ifndef _CHOCOLATE_KEEN_EPISODE_MACROS_
#define _CHOCOLATE_KEEN_EPISODE_MACROS_

#include <stdint.h>

/* Per-episode constants table — one row per episode. */
typedef struct {
    /* sprite frame IDs used by VORTFRAME() */
    uint16_t spr_vortstand1;
    uint16_t spr_vortright1;
    uint16_t spr_vortleft1;
    uint16_t spr_vortjumpl;
    uint16_t spr_vortjumpr;
    uint16_t spr_vortdie1;
    /* sprite frame IDs used by YOUTHFRAME() — 0 for Ep1 (no youth) */
    uint16_t spr_youthleft1;
    uint16_t spr_youthright1;
    uint16_t spr_youthleft4;
    uint16_t spr_youthright4;
    uint16_t spr_youthdie1;
    /* other sprite frame IDs */
    uint16_t spr_keenshot;
    uint16_t spr_tankshot;
    uint16_t spr_shotsplashr;
    uint16_t spr_shotsplashl;
    /* object type IDs */
    uint16_t obj_null;
    uint16_t obj_dead;
    uint16_t obj_zapzot;
    uint16_t obj_keen;
    uint16_t obj_enemyshot;   /* Ep3 reuses CVort2_obj_enemyshot */
    uint16_t obj_keenshot;
    uint16_t obj_onebeforekeenshot;
    uint16_t obj_vorticon;
    uint16_t obj_youth;       /* 0 for Ep1 (no youth) */
    /* sound IDs */
    uint16_t snd_shothit;
    uint16_t snd_vortscream;
} EpisodeConstants_T;

extern const EpisodeConstants_T ep_constants[GAMEVER_TOTALAMOUNT];

/* Episode-dispatch macros: resolve sprite/object/sound IDs to the correct
 * episode-specific constant at runtime, based on engine_gameVersion. */

#define VORTFRAME(a)          (ep_constants[engine_gameVersion].spr_vort##a)
#define YOUTHFRAME(a)         (ep_constants[engine_gameVersion].spr_youth##a)
#define SPRKEENSHOT           (ep_constants[engine_gameVersion].spr_keenshot)
#define SPRTANKSHOT           (ep_constants[engine_gameVersion].spr_tankshot)
#define SPRSHOTSPLASHR        (ep_constants[engine_gameVersion].spr_shotsplashr)
#define SPRSHOTSPLASHL        (ep_constants[engine_gameVersion].spr_shotsplashl)
#define OBJNULL               (ep_constants[engine_gameVersion].obj_null)
#define OBJDEAD               (ep_constants[engine_gameVersion].obj_dead)
#define OBJZAPZOT             (ep_constants[engine_gameVersion].obj_zapzot)
#define OBJKEEN               (ep_constants[engine_gameVersion].obj_keen)
#define OBJENEMYSHOT          (ep_constants[engine_gameVersion].obj_enemyshot)
#define OBJKEENSHOT           (ep_constants[engine_gameVersion].obj_keenshot)
#define OBJONEBEFOREKEENSHOT  (ep_constants[engine_gameVersion].obj_onebeforekeenshot)
#define OBJVORTICON           (ep_constants[engine_gameVersion].obj_vorticon)
#define OBJYOUTH              (ep_constants[engine_gameVersion].obj_youth)
#define SNDSHOTHIT            (ep_constants[engine_gameVersion].snd_shothit)
#define SNDVORTSCREAM         (ep_constants[engine_gameVersion].snd_vortscream)

#endif
