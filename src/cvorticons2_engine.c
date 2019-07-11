#include "cvorticons2.h"

void CVort2_engine_processEXE() {
    /**********************************************************
    Remember that all values are stored in Little-Endian forms!
    **********************************************************/
    size_t loopVar;

    exeFields.trans_map = exeImage + CVort2_TRANS_MAP_OFFSET;
    exeFields.rnd_vals = exeImage + CVort2_RND_VALS_OFFSET;

    // This is related to the jump table (and actually copied to one)
    exeFields.fibs_17 = (uint16_t *)(exeImage + CVort2_FIBS17_OFFSET);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    //Swap on Big-Endians platforms
    for (loopVar = 0; loopVar < 17; loopVar++) {
        exeFields.fibs_17[loopVar] = SDL_Swap16(exeFields.fibs_17[loopVar]);
    }
#endif
    exeFields.points_tbl = (int16_t *)(exeImage + CVort2_POINTS_TABLE_OFFSET);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for (loopVar = 0; loopVar < 5; loopVar++) {
        exeFields.points_tbl[loopVar] = SDL_Swap16(exeFields.points_tbl[loopVar]);
    }
#endif
    // The following are single-byte arrays, so no swap is required.
    exeFields.palettes[0] = exeImage + CVort2_PALETTES_OFFSET;
    exeFields.palettes[1] = exeImage + CVort2_PALETTES_OFFSET + PALETTE_NUM_OF_ENTRIES;
    exeFields.palettes[2] = exeImage + CVort2_PALETTES_OFFSET + PALETTE_NUM_OF_ENTRIES*2;
    exeFields.palettes[3] = exeImage + CVort2_PALETTES_OFFSET + PALETTE_NUM_OF_ENTRIES*3;

    exeFields.char_map = exeImage + CVort2_CHAR_MAP_OFFSET;

    exeFields.uppercase_table = exeImage + CVort2_UPPERCASE_TABLE_OFFSET;

    exeFields.endScreen = exeImage + CVort2_ANSI_QUIT_SCREEN_OFFSET;

    exeFieldsEp2.earth_expl3 = (uint16_t *)(exeImage + 0x19ED6);
    exeFieldsEp2.earth_explX = (uint16_t *)(exeImage + 0x19F74);
    exeFieldsEp2.earth_explY = (uint16_t *)(exeImage + 0x19F86);
    exeFieldsEp2.earth_expl  = (uint16_t *)(exeImage + 0x19F98);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // These all consist of one single continous buffer, so we
    // simply loop over ther four arrays' contents at once (SORT OF A HACK)
    for (loopVar = 0; loopVar < 118; loopVar++)
        exeFieldsEp2.earth_expl3[loopVar] = SDL_Swap16(exeFieldsEp2.earth_expl3[loopVar]);
#endif

    // TODO: Maybe we can remove "exeFields" from all things in general?
    // Or do a different thing...

    // In this section we are initializing a whole bunch of variables
    // to whatever value was present in the datasegment

    // Get the tileinfo
    // TODO: In some games (e.g., Slordax, Tileinfo is a separate file
    // Add support for TILEINFO.XXX
    TILEINFO_Anim = (uint16_t *) (exeImage + CVort2_TILEINFO_OFFSET);
    TILEINFO_Type = (int16_t *) (exeImage + CVort2_TILEINFO_OFFSET) + CVort2_TILENUM;
    TILEINFO_UEdge = (int16_t *) (exeImage + CVort2_TILEINFO_OFFSET) + CVort2_TILENUM * 2;
    TILEINFO_REdge = (int16_t *) (exeImage + CVort2_TILEINFO_OFFSET) + CVort2_TILENUM * 3;
    TILEINFO_DEdge = (int16_t *) (exeImage + CVort2_TILEINFO_OFFSET) + CVort2_TILENUM * 4;
    TILEINFO_LEdge = (int16_t *) (exeImage + CVort2_TILEINFO_OFFSET) + CVort2_TILENUM * 5;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // Byte swap again...
    for (loopVar = 0; loopVar < CVort2_TILENUM; loopVar++) {
        TILEINFO_Anim[loopVar] = SDL_Swap16(TILEINFO_Anim[loopVar]);
        TILEINFO_Type[loopVar] = SDL_Swap16(TILEINFO_Type[loopVar]);
        TILEINFO_UEdge[loopVar] = SDL_Swap16(TILEINFO_UEdge[loopVar]);
        TILEINFO_REdge[loopVar] = SDL_Swap16(TILEINFO_REdge[loopVar]);
        TILEINFO_DEdge[loopVar] = SDL_Swap16(TILEINFO_DEdge[loopVar]);
        TILEINFO_LEdge[loopVar] = SDL_Swap16(TILEINFO_LEdge[loopVar]);
    }
#endif


    // Initialize some odds and ends
    // TODO: Do we really need anim_plane *and* anim_frame_tiles?
    for (loopVar = 0; loopVar < 4; loopVar++)
        anim_plane[loopVar] = anim_frame_tiles[loopVar];

    textbox_offs_x = 0x13;
    textbox_offs_y = 0xB;
    reshow_scroll_up = 1;
    sound_limiter = 0;
    sound_disabled = 0;

    // TODO? These are apparently not done on vanilla Keen directly,
    // but maybe indirectly they are...
    // They are located in the BSS, so they are set to zero by default
    // god_mode = 0;
    // memset(sprites, 0, sizeof(sprites));
    // memset(bodies, 0, sizeof(bodies));

    // TODO: We can always fill more values...
}
