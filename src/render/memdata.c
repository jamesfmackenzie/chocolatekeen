#include "chocolate-keen_config.h"
#include "core/globals.h"

    /* Contains MOST data declared in cvorticons.h at the moment */

    /***********
    Engine stuff
    ***********/
    EngineArguments_T engine_arguments;
    /*const*/ gameversion_T engine_gameVersion;
    bool engine_skipLauncher, engine_isLauncherFullscreen;
    bool engine_forceSpecificEpisode;

    char *engine_configFileLocation;

    InputMappingStruct_T engine_inputMappings;
    //const EmulatedKey_T engine_emulatedKeysTable[];

    //const uint32_t engine_egaRGBColorTable[];

    CVort_engine_screen_T engine_screen;

    uint8_t engine_currPage;
    uint16_t pel_panning;
    uint32_t engine_currPageStart; // Acts like page_current_seg
    uint32_t engine_dstPage; // Acts like dstseg
    uint32_t engine_egaStart; // Acts like ega_regen_start_addr

    uint32_t engine_gameTicksStart, engine_gameTicksDelayOffset;
    int64_t engine_sdlMicroTicksStart, engine_sdlMicroTicksDelayOffset;
    // A moment in time to wait for, measured in micro seconds
    int64_t engine_momentToDelayInMicroSec;
    // The time of a single (emulated) refresh in micro seconds
    int64_t engine_refreshTimeInMicroSec;

    /* Use this to reset the time on a call to engine_delay if needed
     * (used on startup or after a call to engine_shortSleep)
     */
    bool engine_doResetMomentToDelay;

    bool engine_isFrameReadyToDisplay;

    uint32_t engine_lastDisplayUpdateTime;

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    // Common (OpenGL 1.1 / ES 2.0 and compatible)
    glGetString_Func_T engine_glGetString;
    glClearColor_Func_T engine_glClearColor;
    glViewport_Func_T engine_glViewport;
    glGetIntegerv_Func_T engine_glGetIntegerv;
    glEnable_Func_T engine_glEnable;
    glGenTextures_Func_T engine_glGenTextures;
    glBindTexture_Func_T engine_glBindTexture;
    glTexParameteri_Func_T engine_glTexParameteri;
    glTexImage2D_Func_T engine_glTexImage2D;
    glClear_Func_T engine_glClear;
    glDisable_Func_T engine_glDisable;

    glDeleteTextures_Func_T engine_glDeleteTextures;

    glTexSubImage2D_Func_T engine_glTexSubImage2D;
    glCopyTexSubImage2D_Func_T engine_glCopyTexSubImage2D;
    glScissor_Func_T engine_glScissor;

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    // OpenGL 1.1 (fixed pipeline, not available in ES 2.0)
    glMatrixMode_Func_T engine_glMatrixMode;
    glLoadIdentity_Func_T engine_glLoadIdentity;
    //glScalef_Func_T engine_glScalef;
    glOrtho_Func_T engine_glOrtho;
    glGenLists_Func_T engine_glGenLists;
    glNewList_Func_T engine_glNewList;
    glBegin_Func_T engine_glBegin;
    glTexCoord2f_Func_T engine_glTexCoord2f;
    glVertex3f_Func_T engine_glVertex3f;
    glEnd_Func_T engine_glEnd;
    glEndList_Func_T engine_glEndList;
    glPushAttrib_Func_T engine_glPushAttrib;
    glPopAttrib_Func_T engine_glPopAttrib;
    glDeleteLists_Func_T engine_glDeleteLists;
    glCallList_Func_T engine_glCallList;

    // OpenGL 1.1 (misc. functions not available in ES 2.0)
    //glDrawBuffer_Func_T engine_glDrawBuffer;
    //glReadBuffer_Func_T engine_glReadBuffer;
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    // OpenGL 1.1 / ES 2.0 (used for GL 2.0 renderer only)
    glDrawElements_Func_T engine_glDrawElements;

    // OpenGL 1.3 / ES 2.0 (used for palette cycling, GL 2.0 renderer only)
    glActiveTexture_Func_T engine_glActiveTexture;

    // OpenGL 2.0 / ES 2.0 (shader related functions)
    glCreateShader_Func_T engine_glCreateShader;
    glShaderSource_Func_T engine_glShaderSource;
    glCompileShader_Func_T engine_glCompileShader;
    glGetShaderiv_Func_T engine_glGetShaderiv;
    glGetShaderInfoLog_Func_T engine_glGetShaderInfoLog;
    glDeleteShader_Func_T engine_glDeleteShader;

    glCreateProgram_Func_T engine_glCreateProgram;
    glAttachShader_Func_T engine_glAttachShader;
    glLinkProgram_Func_T engine_glLinkProgram;
    glGetProgramiv_Func_T engine_glGetProgramiv;
    glGetProgramInfoLog_Func_T engine_glGetProgramInfoLog;
    glDeleteProgram_Func_T engine_glDeleteProgram;
    glUseProgram_Func_T engine_glUseProgram;

    glGetAttribLocation_Func_T engine_glGetAttribLocation;
    glGetUniformLocation_Func_T engine_glGetUniformLocation;

    glVertexAttribPointer_Func_T engine_glVertexAttribPointer;
    glEnableVertexAttribArray_Func_T engine_glEnableVertexAttribArray;
    glUniform1i_Func_T engine_glUniform1i;
#endif

    // OpenGL 2.0 (not available in ES 2.0)
    //glDrawBuffers_Func_T engine_glDrawBuffers;

    // Framebuffer object (EXT_framebuffer_object in 2.0, core in ES 2.0)
    glBindFramebufferEXT_Func_T engine_glBindFramebuffer_CoreEXT;
    glDeleteFramebuffersEXT_Func_T engine_glDeleteFramebuffers_CoreEXT;
    glGenFramebuffersEXT_Func_T engine_glGenFramebuffers_CoreEXT;
    glCheckFramebufferStatusEXT_Func_T engine_glCheckFramebufferStatus_CoreEXT;
    glFramebufferTexture2DEXT_Func_T engine_glFramebufferTexture2D_CoreEXT;

    // Framebuffer blitting (EXT_framebuffer_blit, requires ES 3.0 or other ES extension)
    glBlitFramebufferEXT_Func_T engine_glBlitFramebufferEXT;
#endif
    EGAHeadGeneralSection_T engine_egaHeadGeneral;
    EGAHeadUnmasked_T *engine_egaHeadUnmasked;
    //EGAHeadSprite_T *engine_egaHeadSprite;
    MaskedSpriteEntry_T *engine_maskedSpriteEntry;

    // Note: These are not in planar format but rather single-byte per pixel
    uint8_t *engine_egaBmpData, *engine_egaFontData, *engine_egaTileData, *engine_egaSpriteData;
    uint8_t **engine_egaBmps, **engine_egaFonts, **engine_egaTiles, **engine_egaSprites;

    uint16_t engine_sndCount;

    sndChunk_T *sndChunks;
    void *sndChunksData;

    uint8_t sound_limiter, *sndPriorities;
    int engine_currSoundLocation;
    int16_t engine_currSoundPlaying;

    // The 16 EGA colors used when graphics mode 0x0D is set
    uint32_t engine_egaSDLColorTable[16];

    uint32_t engine_mappedEgaSDLColorTable[16];

    uint8_t engine_lastScanCode;
    uint32_t engine_lastKeyTime;
    bool engine_isBeforeKeyDelay;

    bool engine_isCursorLocked;
 
    // EXE file data (the uncompressed EXE without the header)
    uint8_t *exeImage;

    // Things that get loaded from the original EXE
    CVort_EXE_FIELDS_T exeFields;

    /************************************
    Vanilla Keen variables; Well, mostly.
     ************************************/
    // Grouped state structs
    GameState_T g_game;
    EntityManager_T g_entities;
    InputState_T g_input;

    // Initialized Variables
    uint16_t jump_height_table[18];
    uint16_t spritejump_1, spritejump_2;

    char game_ext[4];
    //const uint32_t pitAltCounter;

    // BSS
    uint32_t ticks_sync;
    uint16_t tiledraws_c, bmpdraws_c, spritedraws_c;
    // Same for bmpdraws and more
    SpriteDraw_T spritedraws[0x1f4];
    BmpDraw_T bmpdraws[0xa];
    TileDraw_T tiledraws[0x64];
    uint16_t screentiles[0x258];
    // Legacy counters are kept for compatibility with original draw pipeline flow.
    int bmpdraws_i, tiledraws_i, screentiles_i, spritedraws_i;

    uint16_t *TILEINFO_Anim;
    int16_t *TILEINFO_Type;
    int16_t *TILEINFO_UEdge, *TILEINFO_REdge, *TILEINFO_DEdge, *TILEINFO_LEdge;
    
    /* NOTE: This is a two-dimensional array of a variable size, depending on
     * the episode. Each episode has its own static "implementation" of the array.
     * We could actually use anim_plane, but there are references to
     * anim_frame_tiles in the game code...
     */
    uint16_t * anim_frame_tiles[4];

    uint16_t * anim_plane[4];
    int anim_plane_i;

    uint8_t *sound_data;

    uint16_t ATR[2][0x14A];

    uint8_t demo_actions_including_level_num[5001];
    uint8_t *demo_action_ptr, *end_of_demo_ptr, *demo_after_last_byte_char_offset = demo_actions_including_level_num + sizeof(demo_actions_including_level_num);

    int16_t map_data[0x10000 / 2], *map_data_tiles, *map_data_sprites;
    int16_t map_width_tile, map_height_tile, map_width_bytes;
    uint16_t screen_wrap_single;
    //uint16_t screen_wrap;
    int32_t map_width, map_height;
    int32_t scroll_x, scroll_y, scroll_x_min, scroll_y_min, scroll_x_max, scroll_y_max, ceiling_x, ceiling_y;
    int16_t scroll_x_tile, scroll_y_tile;
    uint16_t cursorX, cursorX_save, cursorY;

    uint32_t cursorX_b, cursorY_b; // Cursor position in map units

    uint16_t textbox_x_pos, textbox_width, textbox_y_pos, textbox_height;
    uint16_t textbox_offs_x, textbox_offs_y;

    int16_t keen_tileX, keen_tileY;

    uint8_t *previews_txt, *story_text, *help_text, *end_text;

    HighScoresTable_T high_scores_table;
    Sprite_T keen_map[2];
    GameProfile_T keen_gp;

    uint32_t keen_wmap_x_pos, keen_wmap_y_pos;
    uint32_t wmap_scroll_x, wmap_scroll_y;
    Maplevel_T wmaplevels[0x10];

    DrawFunction_T draw_func;

    uint16_t text_viewer_top_pos, text_viewer_height, text_viewer_bottom;
    uint8_t *text_ptr;

    int16_t text_viewer_buffer[0x190], *text_viewer_buffer_ptr;

    uint16_t messie_mounted, messie_frame, messie_time_to_climb, messie_move_tics;
    uint16_t messie_x_T, messie_y_T;
    int32_t messie_xpos, messie_ypos, messie_del_x, messie_del_y;

    Teleporter_T teleporters[16];
