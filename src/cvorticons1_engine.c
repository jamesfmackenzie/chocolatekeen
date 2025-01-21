#include "cvorticons1.h"

void CVort1_engine_processEXE() {
    /**********************************************************
    Remember that all values are stored in Little-Endian forms!
    **********************************************************/
    size_t loopVar;

    exeFields.trans_map = exeImage + CVort1_TRANS_MAP_OFFSET;
    exeFields.rnd_vals = exeImage + CVort1_RND_VALS_OFFSET;

    // This is related to the jump table (and actually copied to one)
    exeFields.fibs_17 = (uint16_t *)(exeImage + CVort1_FIBS17_OFFSET);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    //Swap on Big-Endians platforms
    for (loopVar = 0; loopVar < 17; loopVar++) {
        exeFields.fibs_17[loopVar] = SDL_Swap16(exeFields.fibs_17[loopVar]);
    }
#endif
    exeFields.points_tbl = (int16_t *)(exeImage + CVort1_POINTS_TABLE_OFFSET);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for (loopVar = 0; loopVar < 5; loopVar++) {
        exeFields.points_tbl[loopVar] = SDL_Swap16(exeFields.points_tbl[loopVar]);
    }
#endif
    // The following are single-byte arrays, so no swap is required.
    exeFields.palettes[0] = exeImage + CVort1_PALETTES_OFFSET;
    exeFields.palettes[1] = exeImage + CVort1_PALETTES_OFFSET + PALETTE_NUM_OF_ENTRIES;
    exeFields.palettes[2] = exeImage + CVort1_PALETTES_OFFSET + PALETTE_NUM_OF_ENTRIES*2;
    exeFields.palettes[3] = exeImage + CVort1_PALETTES_OFFSET + PALETTE_NUM_OF_ENTRIES*3;

    exeFields.char_map = exeImage + CVort1_CHAR_MAP_OFFSET;

    exeFields.uppercase_table = exeImage + CVort1_UPPERCASE_TABLE_OFFSET;

    exeFields.endScreen = exeImage + CVort1_ANSI_QUIT_SCREEN_OFFSET;

    exeFieldsEp1.ordering_sprites_x = (uint32_t *)(exeImage + 0x1615E);
    exeFieldsEp1.ordering_sprites_y = (uint32_t *)(exeImage + 0x16156);
    exeFieldsEp1.ordering_sprites_frames = (uint16_t *)(exeImage + 0x16152);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // We must byte swap...
    exeFieldsEp1.ordering_sprites_x[0] = SDL_Swap32(exeFieldsEp1.ordering_sprites_x[0]);
    exeFieldsEp1.ordering_sprites_x[1] = SDL_Swap32(exeFieldsEp1.ordering_sprites_x[1]);
    exeFieldsEp1.ordering_sprites_y[0] = SDL_Swap32(exeFieldsEp1.ordering_sprites_y[0]);
    exeFieldsEp1.ordering_sprites_y[1] = SDL_Swap32(exeFieldsEp1.ordering_sprites_y[1]);
    exeFieldsEp1.ordering_sprites_frames[0] = SDL_Swap16(exeFieldsEp1.ordering_sprites_frames[0]);
    exeFieldsEp1.ordering_sprites_frames[1] = SDL_Swap16(exeFieldsEp1.ordering_sprites_frames[1]);
#endif

    exeFieldsEp1.win1 = (int16_t *)(exeImage + 0x1597E);
    exeFieldsEp1.win2 = (int16_t *)(exeImage + 0x159BA);
    exeFieldsEp1.win3 = (int16_t *)(exeImage + 0x15A66);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // Byte-swap everything
    for (loopVar = 0; loopVar < 30; loopVar++) {
        exeFieldsEp1.win1[loopVar] = SDL_Swap16(exeFieldsEp1.win1[loopVar]);
    }
    for (loopVar = 0; loopVar < 86; loopVar++) {
        exeFieldsEp1.win2[loopVar] = SDL_Swap16(exeFieldsEp1.win2[loopVar]);
    }
    for (loopVar = 0; loopVar < 80; loopVar++) {
        exeFieldsEp1.win3[loopVar] = SDL_Swap16(exeFieldsEp1.win3[loopVar]);
    }
#endif

    // TODO: Maybe we can remove "exeFields" from all things in general?
    // Or do a different thing...

    // In this section we are initializing a whole bunch of variables
    // to whatever value was present in the datasegment

    // Get the tileinfo
    // TODO: In some games (e.g., Slordax, Tileinfo is a separate file
    // Add support for TILEINFO.XXX
    TILEINFO_Anim = (uint16_t *) (exeImage + CVort1_TILEINFO_OFFSET);
    TILEINFO_Type = (int16_t *) (exeImage + CVort1_TILEINFO_OFFSET) + CVort1_TILENUM;
    TILEINFO_UEdge = (int16_t *) (exeImage + CVort1_TILEINFO_OFFSET) + CVort1_TILENUM * 2;
    TILEINFO_REdge = (int16_t *) (exeImage + CVort1_TILEINFO_OFFSET) + CVort1_TILENUM * 3;
    TILEINFO_DEdge = (int16_t *) (exeImage + CVort1_TILEINFO_OFFSET) + CVort1_TILENUM * 4;
    TILEINFO_LEdge = (int16_t *) (exeImage + CVort1_TILEINFO_OFFSET) + CVort1_TILENUM * 5;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    // Byte swap again...
    for (loopVar = 0; loopVar < CVort1_TILENUM; loopVar++) {
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

    // A struct (or an array of structs) is better copied manually, rather
    // than have a pointer to a struct in the EXE, due to various ways the
    // compiler may actually align a struct in memory.
    teleporters[0].destX = SDL_SwapLE32(*(int32_t *)(exeImage + 0x158DE));
    teleporters[0].destY = SDL_SwapLE32(*(int32_t *)(exeImage + 0x158E2));
    teleporters[0].isOnSnow = SDL_SwapLE16(*(int16_t *)(exeImage + 0x158E6));
    teleporters[1].destX = SDL_SwapLE32(*(int32_t *)(exeImage + 0x158E8));
    teleporters[1].destY = SDL_SwapLE32(*(int32_t *)(exeImage + 0x158EC));
    teleporters[1].isOnSnow = SDL_SwapLE16(*(int16_t *)(exeImage + 0x158F0));
    // Secret map telepoter
    teleporters[2].destX = SDL_SwapLE32(*(int32_t *)(exeImage + 0x158F2));
    teleporters[2].destY = SDL_SwapLE32(*(int32_t *)(exeImage + 0x158F6));
    teleporters[2].isOnSnow = SDL_SwapLE16(*(int16_t *)(exeImage + 0x158FA));

    // TODO? These are apparently not done on vanilla Keen directly,
    // but maybe indirectly they are...
    // They are located in the BSS, so they are set to zero by default
    // god_mode = 0;
    // memset(sprites, 0, sizeof(sprites));
    // memset(bodies, 0, sizeof(bodies));

    // TODO: We can always fill more values...
}
