#include "chocolate-keen_config.h"
#include "cvorticons.h"

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

#if 0
    // EGA color table, in RGBA/ABGR format. To be more specific,
    // it is RGBA on Big-Endian platforms and ABGR on Little-Endian ones.
    // TODO: Make this more cross-platform and work with other color depths?
    const uint32_t engine_egaABGRColorTableLE[] = {
        0X00000000, 0x0000aa00, 0x00aa0000, 0x00aaaa00, 0xaa000000, 0xaa00aa00, 0xaaaa0000, 0xaaaaaa00,
        0x00005500, 0x0000ff00, 0x00aa5500, 0x00aaff00, 0xaa005500, 0xaa00ff00, 0xaaaa5500, 0xaaaaff00,
        0x00550000, 0x0055aa00, 0x00ff0000, 0x00ffaa00, 0xaa550000, 0xaa55aa00, 0xaaff0000, 0xaaffaa00,
        0x00555500, 0x0055ff00, 0x00ff5500, 0x00ffff00, 0xaa555500, 0xaa55ff00, 0xaaff5500, 0xaaffff00,
        0x55000000, 0x5500aa00, 0x55aa0000, 0x55aaaa00, 0xff000000, 0xff00aa00, 0xffaa0000, 0xffaaaa00,
        0x55005500, 0x5500ff00, 0x55aa5500, 0x55aaff00, 0xff005500, 0xff00ff00, 0xffaa5500, 0xffaaff00,
        0x55550000, 0x5555aa00, 0x55ff0000, 0x55ffaa00, 0xff550000, 0xff55aa00, 0xffff0000, 0xffffaa00,
        0x55555500, 0x5555ff00, 0x55ff5500, 0x55ffff00, 0xff555500, 0xff55ff00, 0xffff5500, 0xffffff00
    };
#endif
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

#if 0
#if VORTICONS_EPISODE == 1
    struct {
        uint32_t *ordering_sprites_x, *ordering_sprites_y;
        uint16_t *ordering_sprites_frames;

	// Actually, win3 is used before win1.
        int16_t *win1, *win2, *win3;
    } exeFieldsEp1;
#elif VORTICONS_EPISODE == 2
    struct {
        uint16_t *earth_expl3, *earth_explX, *earth_explY, *earth_expl;
    } exeFieldsEp2;
#endif
#endif

    /************************************
    Vanilla Keen variables; Well, mostly.
     ************************************/
    // Initialized Variables
#if 0
    const static uint8_t rnd_vals[];
    const static uint8_t trans_map[];
    const static uint8_t char_map[];
    const static uint8_t uppercase_table[];
    const static int16_t points_tbl[];
    const static uint8_t palettes[][17];
#endif
    uint8_t key_map[128]; // All zeros
#if 0
    const static uint16_t fibs_17[];
#endif
    uint16_t jump_height_table[18];
    uint16_t spritejump_1, spritejump_2;

    char game_ext[4];
    //const uint32_t pitAltCounter;

    // BSS 
    uint32_t ticks_sync;
    uint16_t sprite_sync;
    int16_t god_mode, keen_invincible;
    int16_t level_finished;
    // TODO: Using this in "modern" code is uncommon today...
    char string_buf[0x50];

    int16_t joy_detect;
    int16_t pass_keys_to_bios;
    //uint16_t pass_keys_to_bios;
    uint16_t key_code, key_scane;

    uint16_t sound_disabled;
    int16_t rnd;
    uint16_t tiledraws_c, bmpdraws_c, spritedraws_c;
    // Same for bmpdraws and more
    SpriteDraw_T spritedraws[0x1f4];
    BmpDraw_T bmpdraws[0xa];
    TileDraw_T tiledraws[0x64];
    uint16_t screentiles[0x258];
    // TODO: bmpdraws_i (bmpdraws_p) doesn't seem to actually be used...
    int bmpdraws_i, tiledraws_i, screentiles_i, spritedraws_i;

    uint16_t want_sound;

    int16_t mouse_ctrl_1;
    // Original reference names to the four joystick arrays:
    // word_2B2BA, word_2B2C0, word_2B2C6, word_2B2CC
    int16_t joystick_ctrl[4][3];

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
    int16_t demo_status;

    int16_t map_data[0x10000 / 2], *map_data_tiles, *map_data_sprites;
    int16_t map_width_T, map_height_T, map_width_B;
    uint16_t screen_wrap_single;
    //uint16_t screen_wrap;
    int32_t map_width, map_height;
    int32_t scrollX, scrollY, scrollX_min, scrollY_min, scrollX_max, scrollY_max, ceilingX, ceilingY;
    int16_t scrollX_T, scrollY_T;
    uint16_t cursorX, cursorX_save, cursorY;

    uint32_t cursorX_b, cursorY_b; // Cursor position in map units

    uint16_t textbox_x_pos, textbox_width, textbox_y_pos, textbox_height;
    uint16_t textbox_offs_x, textbox_offs_y;

    int16_t keen_tileX, keen_tileY;

    Sprite_T sprites[0x50], temp_sprite;
    Body_T bodies[0x10];

    GameInput_T input_new, input_old;

    int16_t num_sprites, num_bodies;

    int16_t spark_counter;

    uint16_t quit_to_title;

    int16_t wmap_sprite_on;

    int32_t extra_life_pts;

    MaskedSpriteEntry_T temp_MSE;

    uint16_t anim_speed;

    uint16_t ctrl_type[3];
    uint8_t sc_dir[8], sc_but1, sc_but2;

    uint16_t lights;

    uint8_t *previews_txt, *story_text, *help_text, *end_text;

    HighScoresTable_T high_scores_table;
    Sprite_T keen_map[2];
    GameProfile_T keen_gp;

    uint16_t on_world_map;
    uint16_t resuming_saved_game;
    uint16_t current_level;

    uint16_t reshow_scroll_up;

    uint16_t wmap_col;

    int16_t keen_facing;

    uint16_t apogee_bmp_height, intro_complete;

    uint32_t keen_wmap_x_pos, keen_wmap_y_pos;
    uint32_t wmap_scrollX, wmap_scrollY;
    Maplevel_T wmaplevels[0x10];

    DrawFunction_T draw_func;

    uint16_t text_viewer_top_pos, text_viewer_height, text_viewer_bottom;
    uint8_t *text_ptr;

    int16_t text_viewer_buffer[0x190], *text_viewer_buffer_ptr;

    int16_t keen_switch;

    uint16_t messie_mounted, messie_frame, messie_time_to_climb, messie_move_tics;
    uint16_t messie_x_T, messie_y_T;
    int32_t messie_xpos, messie_ypos, messie_delX, messie_delY;

    Teleporter_T teleporters[16];
