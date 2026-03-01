#include "core/core.h"
#include "episodes/cvorticons1.h"
#include "episodes/cvorticons2.h"
#include "episodes/cvorticons3.h"
#include "episodes/episode_macros.h"

const EpisodeConstants_T ep_constants[GAMEVER_TOTALAMOUNT] = {

#if CHOCOLATE_KEEN_IS_EPISODE1_ENABLED
    [GAMEVER_KEEN1] = {
        .spr_vortstand1         = CVort1_spr_vortstand1,
        .spr_vortright1         = CVort1_spr_vortright1,
        .spr_vortleft1          = CVort1_spr_vortleft1,
        .spr_vortjumpl          = CVort1_spr_vortjumpl,
        .spr_vortjumpr          = CVort1_spr_vortjumpr,
        .spr_vortdie1           = CVort1_spr_vortdie1,
        /* no youth in Ep1 — spr_youth* and obj_youth remain 0 */
        .spr_keenshot           = CVort1_spr_keenshot,
        .spr_tankshot           = CVort1_spr_tankshot,
        .spr_shotsplashr        = CVort1_spr_shotsplashr,
        .spr_shotsplashl        = CVort1_spr_shotsplashl,
        .obj_null               = CVort1_obj_null,
        .obj_dead               = CVort1_obj_dead,
        .obj_zapzot             = CVort1_obj_zapzot,
        .obj_keen               = CVort1_obj_keen,
        .obj_enemyshot          = CVort1_obj_enemyshot,
        .obj_keenshot           = CVort1_obj_keenshot,
        .obj_onebeforekeenshot  = CVort1_obj_onebeforekeenshot,
        .obj_vorticon           = CVort1_obj_vorticon,
        /* obj_youth = 0 */
        .snd_shothit            = CVort1_snd_shothit,
        .snd_vortscream         = CVort1_snd_vortscream,
    },
#endif

#if CHOCOLATE_KEEN_IS_EPISODE2_ENABLED
    [GAMEVER_KEEN2] = {
        .spr_vortstand1         = CVort2_spr_vortstand1,
        .spr_vortright1         = CVort2_spr_vortright1,
        .spr_vortleft1          = CVort2_spr_vortleft1,
        .spr_vortjumpl          = CVort2_spr_vortjumpl,
        .spr_vortjumpr          = CVort2_spr_vortjumpr,
        .spr_vortdie1           = CVort2_spr_vortdie1,
        .spr_youthleft1         = CVort2_spr_youthleft1,
        .spr_youthright1        = CVort2_spr_youthright1,
        .spr_youthleft4         = CVort2_spr_youthleft4,
        .spr_youthright4        = CVort2_spr_youthright4,
        .spr_youthdie1          = CVort2_spr_youthdie1,
        .spr_keenshot           = CVort2_spr_keenshot,
        .spr_tankshot           = CVort2_spr_tankshot,
        .spr_shotsplashr        = CVort2_spr_shotsplashr,
        .spr_shotsplashl        = CVort2_spr_shotsplashl,
        .obj_null               = CVort2_obj_null,
        .obj_dead               = CVort2_obj_dead,
        .obj_zapzot             = CVort2_obj_zapzot,
        .obj_keen               = CVort2_obj_keen,
        .obj_enemyshot          = CVort2_obj_enemyshot,
        .obj_keenshot           = CVort2_obj_keenshot,
        .obj_onebeforekeenshot  = CVort2_obj_onebeforekeenshot,
        .obj_vorticon           = CVort2_obj_vorticon,
        .obj_youth              = CVort2_obj_youth,
        .snd_shothit            = CVort2_snd_shothit,
        .snd_vortscream         = CVort2_snd_vortscream,
    },
#endif

#if CHOCOLATE_KEEN_IS_EPISODE3_ENABLED
    [GAMEVER_KEEN3] = {
        .spr_vortstand1         = CVort3_spr_vortstand1,
        .spr_vortright1         = CVort3_spr_vortright1,
        .spr_vortleft1          = CVort3_spr_vortleft1,
        .spr_vortjumpl          = CVort3_spr_vortjumpl,
        .spr_vortjumpr          = CVort3_spr_vortjumpr,
        .spr_vortdie1           = CVort3_spr_vortdie1,
        .spr_youthleft1         = CVort3_spr_youthleft1,
        .spr_youthright1        = CVort3_spr_youthright1,
        .spr_youthleft4         = CVort3_spr_youthleft4,
        .spr_youthright4        = CVort3_spr_youthright4,
        .spr_youthdie1          = CVort3_spr_youthdie1,
        .spr_keenshot           = CVort3_spr_keenshot,
        .spr_tankshot           = CVort3_spr_tankshot,
        .spr_shotsplashr        = CVort3_spr_shotsplashr,
        .spr_shotsplashl        = CVort3_spr_shotsplashl,
        .obj_null               = CVort3_obj_null,
        .obj_dead               = CVort3_obj_dead,
        .obj_zapzot             = CVort3_obj_zapzot,
        .obj_keen               = CVort3_obj_keen,
        .obj_enemyshot          = CVort2_obj_enemyshot, /* Ep3 reuses Ep2 value */
        .obj_keenshot           = CVort3_obj_keenshot,
        .obj_onebeforekeenshot  = CVort3_obj_onebeforekeenshot,
        .obj_vorticon           = CVort3_obj_vorticon,
        .obj_youth              = CVort3_obj_youth,
        .snd_shothit            = CVort3_snd_shothit,
        .snd_vortscream         = CVort3_snd_vortscream,
    },
#endif

};
