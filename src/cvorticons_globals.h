#ifndef _CHOCOLATE_KEEN_VORTICONS_GLOBALS_
#define _CHOCOLATE_KEEN_VORTICONS_GLOBALS_

#include <stdio.h>
#include "cvorticons_types.h"
#include "cvorticons_sprites.h"
#include "cvorticons_opengl.h"
#include "cvorticons_input.h"

/* ---------------------------------------------------------------
 * Grouped global state structs (Step 6 refactor)
 * --------------------------------------------------------------- */
typedef struct {
    int16_t god_mode, keen_invincible;
    int16_t level_finished;
    uint16_t current_level;
    uint16_t on_world_map, resuming_saved_game;
    uint16_t quit_to_title;
    int16_t keen_facing;
    uint16_t sprite_sync;
    int16_t rnd;
    uint16_t sound_disabled, want_sound;
    int32_t extra_life_pts;
    int16_t spark_counter;
    int16_t wmap_sprite_on;
    uint16_t reshow_scroll_up, wmap_col;
    uint16_t apogee_bmp_height, intro_complete;
    uint16_t anim_speed;
    uint16_t lights;
    int16_t demo_status;
    int16_t keen_switch;
    char string_buf[0x50];
} GameState_T;

typedef struct {
    Sprite_T sprites[0x50];
    Sprite_T temp_sprite;
    Body_T bodies[0x10];
    int16_t num_sprites, num_bodies;
    MaskedSpriteEntry_T temp_MSE;
} EntityManager_T;

typedef struct {
    uint8_t key_map[128];
    uint16_t key_code, key_scane;
    int16_t joy_detect;
    int16_t pass_keys_to_bios;
    int16_t mouse_ctrl_1;
    int16_t joystick_ctrl[4][3];
    uint16_t ctrl_type[3];
    uint8_t sc_dir[8], sc_but1, sc_but2;
    GameInput_T input_new, input_old;
} InputState_T;

    /*********************
    GUI/Launcher functions
    *********************/
    void CVort_gui_runLoop(void);
    /*****************************
    Engine (lower level) functions
    *****************************/
    bool CVort_engine_start();
    void CVort_engine_loadKeen(gameversion_T gameVer);
    void CVort_engine_shutdown(void);

    bool CVort_engine_isGameExeAvailable(gameversion_T gameVer);

    bool CVort_engine_processArguments(int argc, char **argv);

    extern void (*CVort_ptr_engine_processEXE)();

    void CVort_engine_setupInputMappings(void);
    void CVort_engine_setDefaultInputMappings(void);
    bool CVort_engine_tryToLoadInputMappings(void);
    bool CVort_engine_addInputMappingsFromFile(const char *fileBuffer, const char *tempBuffer, MappedInputEventList_T *pEventList);
    // Records a new mapper binding from the user (host key press / joystick motion / ...)
    // Returns NULL if timed out or there are already MAX_EMU_MAPPINGS_PER_HOST_ENTRY
    // bindings for a given host input, otherwise returns a pointer to a mapping
    // that may be changed directly (say to add modifiers)
    // Sets retInputT, retInputId and retInputVal to values representing
    // captured host input action
    // If there has already been a binding for the given action, it is chosen,
    // otherwise a new one is added
    MappedInputEvent_T *CVort_engine_recordNewInputMapping(EmulatedInput_T emuInput, int value, unsigned int mstimeout, HostInput_T *retInputT, int *retInputId, int *retInputVal);
    // Removes mapping if there's any
    void CVort_engine_deleteInputMapping(EmulatedInput_T emuInput, int emuValue, HostInput_T inputT, int inputId, int inputVal);
    // For a given emulated input event, gets the first mapping after the one
    // given as input, if there's any (possibly the exact same mapping).
    // If there's none, NULL is returned.
    // Otherwise a reference to the mapping that can be modified as returned.
    MappedInputEvent_T *CVort_engine_getNextInputMapping(
        EmulatedInput_T emuInput, int value,
        HostInput_T inputT, int inputId, int inputVal,
        HostInput_T *retInputT, int *retInputId, int *retInputVal
    );
    // Same as getNextInputMapping but traversing in the opposite direction
    MappedInputEvent_T *CVort_engine_getPrevInputMapping(
        EmulatedInput_T emuInput, int value,
        HostInput_T inputT, int inputId, int inputVal,
        HostInput_T *retInputT, int *retInputId, int *retInputVal
    );

    void CVort_engine_saveInputMappings();
    void CVort_engine_writeMappedHostInputs(FILE *fp, const MappedInputEvent_T *pTestEvent);
    void CVort_engine_writeEventModifiers(FILE *fp, int modMask);

    void CVort_engine_handleQuit();
    void CVort_engine_shutdownSDL();
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
    bool CVort_engine_preparegl();
#endif

    void CVort_engine_parseCalculatedEngineArguments(void);
    bool CVort_engine_prepareInstructionsScreen();
    void CVort_engine_displayCommandLineHelp();
    //bool CVort_engine_handleCmdLineHelpPageTransition();
    void CVort_engine_decompGraphics();

    int16_t CVort_engine_readGamePort();
    bool CVort_engine_isJoystickInUse();

    extern void (*CVort_ptr_engine_setTicks)(uint32_t currTicks);
    extern uint32_t (*CVort_ptr_engine_getTicks)();
    extern void (*CVort_ptr_engine_setTicksSync)(uint32_t currTicks);
    extern uint32_t (*CVort_ptr_engine_getTicksSync)();

    int64_t CVort_engine_convertMomentFromGameTicksToMicroSec(uint32_t momentInGameTicks);

    /* An amortized delay function, which also updates input status and
     * display contents in the background. In practice, the display
     * contents are updated if there is anything to update on screen and
     * there is also the need to wait.
     * The length is given, approximately, in emulated frames
     * (~70.086 for a second with the default of VGA). It may actually wait
     * a bit longer, depending on OS scheduling. Such lost time is
     * "returned" in later calls to engine_delay, hence the delay is
     * amortized.
     * Note that the latter may not apply if a "long term" sleep is
     * requested, using engine_shortSleep.
     * Further note that this may have an effect on later calls to
     * engine_getTicks(), so the game "thinks" that no time has been lost.
     */
    void CVort_engine_delay(int16_t length);

    /* A replacement for a busy loop that waits until the following condition
     * holds:
     * (engine_getTicks()&0xFFFF)-(ticks_sync&0xFFFF) >= length
     * Or equivalently (for small length):
     * engine_getTicks() - ticks_sync >= length
     * Like engine_delay, the implied delay is amortized and it may have a
     * similar effect on later calls to engine_getTicks().
     */
    void CVort_engine_delayInGameTicks(uint32_t startOfDelayMoment, uint16_t length);

    void CVort_engine_doWaitInterval();
    /* Refreshes the screen contents (even if there is no actual update),
     * sleeps for a bit and then resets internal counters used by
     * engine_delay and engine_delayInGameTicks.
     * This function is used when one may need to wait for something
     * (like reading scrollable text while not actually scrolling).
     */
    void CVort_engine_shortSleep();

    extern void (*CVort_engine_updateEgaGfxNonPalRect_ptr) (uint32_t offset, uint16_t width, uint16_t height);
    extern void (*CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr) (uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    extern void (*CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr) (uint16_t x, uint16_t y, bool isShown);
    extern void (*CVort_engine_doDrawEgaTxtCursor_ptr) (void);
    extern void (*CVort_engine_updateBorderedZoomedRectBuffer_ptr) (uint8_t *buffer, uint32_t egaOffset, uint32_t borderLineOffset);
    extern void (*CVort_engine_updateBorderedZoomedRectBufferBorder_ptr) (uint8_t *buffer);

    extern void (*CVort_engine_gui_drawFontChar_ptr) (int x, int y, int w, int h, const uint8_t *fontCharPtr, int colorNum, int ratio);
    extern void (*CVort_engine_gui_drawRoundedRectBorder_ptr) (int x, int y, int w, int h, int borderColorNum, int innerColorNum, int ratio);

    void CVort_engine_gui_clearScreen(void);
    void CVort_engine_gui_drawColoredLine(int lineNum, int lineLength, int colorNum);
    void CVort_engine_gui_drawColoredColumn(int columnNum, int columnLength, int colorNum);

    void CVort_engine_updateActualDisplay();
    void CVort_engine_setWindowTitleAndIcon();
    bool CVort_engine_prepareScreen(void);
    bool CVort_engine_resetWindow(void);
    bool CVort_engine_setVideoMode(int16_t vidMode);
    void CVort_engine_reactToWindowResize(int width, int height);
#if SDL_VERSION_ATLEAST(2,0,0)
    void CVort_engine_handleWindowSideChange(void);
#endif
    bool CVort_engine_isVerticalBlank();

    void CVort_engine_copyToTxtMemory(uint8_t *buffer);

    void CVort_engine_drawChar(uint16_t x, uint16_t y, uint16_t val);
    void CVort_engine_drawSprite(uint16_t x, uint16_t y, uint16_t num);
    void CVort_engine_drawTile(uint16_t x, uint16_t y, uint16_t num);
    void CVort_engine_drawBitmap(uint16_t x, uint16_t y, uint16_t num);

    uint16_t CVort_engine_drawSpriteAt(int32_t pos_x, int32_t pos_y, uint16_t frame);
    uint16_t CVort_engine_drawTileAt(int32_t pos_x, int32_t pos_y, uint16_t tilenum);

    void CVort_engine_clearOverlay();
    void CVort_engine_syncDrawing();
    void CVort_engine_drawScreen();
    void CVort_engine_doDrawing();
    void CVort_engine_blitTile(uint16_t num, uint32_t firstPos);
    void CVort_engine_adaptiveTileRefresh(uint16_t initTileIndex);
    void CVort_engine_egaPageFlip();

    void CVort_engine_setBorderColor(uint8_t color);
    void CVort_engine_setPaletteAndBorderColor(const uint8_t *palette);
    void CVort_engine_gotoXY(uint8_t x, uint8_t y);

    void CVort_engine_showImageFile(const char *filename);

    void CVort_engine_loadSounds();
    void CVort_engine_sndCallback(void *unused, uint8_t *stream, int len);
    void CVort_engine_setCurSound(uint16_t sound);
    void CVort_engine_saveCurSound();
    void CVort_engine_restoreCurSound();
    void CVort_engine_finishCurSound();

    void CVort_engine_updateInputStatus();
    void CVort_engine_handleEvent(const MappedInputEvent_T *pMappedEvent, int32_t action);
    void CVort_engine_toggleCursorLock(bool toggle);
    JoystickPoll_T CVort_engine_pollJoystick(int16_t joystickNum);

    /* Used for loading read-only vanilla Keen data */
    FILE *CVort_engine_cross_ro_data_fopen(const char *filename);
    /* Used for loading read/write vanilla Keen files (CTLPANEL and more) */
    FILE *CVort_engine_cross_rw_misc_fopen(const char *filename, const char *mode);
    /* Opens a file used by Chocolate Keen internally (not the "emulated" game)
    from a possibly different location.                                      */
    FILE *CVort_engine_configpath_fopen(const char *filename, const char *mode);
    void CVort_engine_loadConfigFile(void);
    void CVort_engine_saveConfigFile(void);
    /* Used for reading buffers of a specific type, assuming Little-Endian
    byte order in the file's data itself. It gets converted to native order. */
    size_t CVort_engine_cross_freadInt8LE(void *ptr, size_t count, FILE *stream);
    size_t CVort_engine_cross_freadInt16LE(void *ptr, size_t count, FILE *stream);
    size_t CVort_engine_cross_freadInt32LE(void *ptr, size_t count, FILE *stream);
    /* Used for writing buffers of a specific type, converting
    native byte order to Little-Endian order within the file. */
    size_t CVort_engine_cross_fwriteInt8LE(void *ptr, size_t count, FILE *stream);
    size_t CVort_engine_cross_fwriteInt16LE(void *ptr, size_t count, FILE *stream);
    size_t CVort_engine_cross_fwriteInt32LE(void *ptr, size_t count, FILE *stream);

    /* Writes a message to a log in case of some problem */
    void CVort_engine_cross_logMessage(CVort_Log_Message_Class_T msgClass, const char *format, ...);

    int16_t CVort_engine_signExtend8To16(int8_t inputVal);
    int32_t CVort_engine_signExtend16To32(int16_t inputVal);

    int16_t CVort_engine_toupper(int16_t c);
    void CVort_engine_puts(const char *str);

    /**********************
    Mostly ported functions
     **********************/
    void CVort_start_cheating();
    extern void (*CVort_ptr_show_pause_menu)();
    void CVort_handle_cheat_keys();
    void CVort_main();
    extern void (*CVort_ptr_init_level)(uint16_t levelnum);
    void CVort_main_loop();
    void CVort_update_sprite_hitbox();
    void CVort_update_sprite_hitbox_wmap(Sprite_T *sprite);

    /* Given a GameInput_T structure input, simulates a vanilla Keen do-while busy
     * loop waiting as long as the condition (input.but1jump || input.but2pogo)
     * applies The main difference is that a sleep is requested - but NOT on the
     * first iteration of the loop. So, we are actually using a "standard" while
     * loop here.
     * Since this task is repeated often (waiting for keyboard/controller button
     * release) and requires a bit of "special" handling regarding the sleeps, it
     * resides in its own function definition now.
     */
    void CVort_waitForNoGameButtonPress(GameInput_T *pInput);

    void CVort_draw_worldmap();
    void CVort_mark_cities_done();
    void CVort_erase_parents_text();
    void CVort_draw_string_finale(const char *str);
    extern void (*CVort_ptr_draw_win)();

    //extern void (*CVort_ptr_handle_secret_city)();
    //extern void (*CVort_ptr_draw_earth_explode)();

    void CVort_game_over();
    void CVort_show_keens_left();
    void CVort_place_keen_on_worldmap(uint32_t *pos_x, uint32_t *pos_y);
    void CVort_do_intro_and_menu();
    uint16_t CVort_draw_title();
    extern uint16_t (*CVort_ptr_continue_game)();
    void CVort_do_story();
    void CVort_do_help();
    void CVort_draw_string_sel(uint16_t type, uint16_t x_pos, uint16_t y_pos, const char *str);
    void CVort_draw_text_viewer_border();
    void CVort_do_text_viewer_short(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset);
    void CVort_do_text_viewer(uint8_t *text, uint16_t top_line_offset, uint16_t bottom_line_offset);
    void CVort_draw_menu();
    void CVort_do_start_menu();
    void CVort_do_previews();
    void CVort_show_about_us();
    void CVort_draw_mural();
    void CVort_do_draw_mural();
    void CVort_draw_about_us();
    void CVort_do_about_us();
    extern void (*CVort_ptr_draw_scores)();
    void CVort_do_scores();
    void CVort_show_scores();
    void CVort_save_game();
    void CVort_draw_apogee();
    void CVort_show_logo_text();
    void CVort_scroll_up_logo();
    extern void (*CVort_ptr_show_ordering)(uint16_t isTimed);
    extern void (*CVort_ptr_draw_ordering_info)();
    extern void (*CVort_ptr_do_ordering)();
    void CVort_draw_string_80(const char *str);
    int16_t CVort_check_world_map_col(Sprite_T *sprite);
    void CVort_move_worldmap(GameInput_T input, Sprite_T *spritedraw);
    extern int16_t (*CVort_ptr_worldmap_sprites)(int16_t map_sprite_standing, Sprite_T* spritedraw, int16_t pos_x, int16_t pos_y);
    void CVort_wait_for_key();

    extern void (*CVort_ptr_inlevel_message)();

    void CVort_add_sprite_keengun(int32_t pos_x, int32_t pos_y);
    void CVort_think_zapzot();
    void CVort_think_keengun();
    void CVort_contact_keengun(Sprite_T *keengun, Sprite_T * contacted);
    void CVort_add_sprite_tankshot(int32_t pos_x, int32_t pos_y, int16_t vel_x);
    void CVort_contact_tankshot(Sprite_T *keengun, Sprite_T * contacted);

    void CVort_add_sprite_vorticon(int16_t tileX, int16_t tileY);
    void CVort_add_sprite_youth(int16_t tileX, int16_t tileY);
    void CVort_think_vorticon_walk();
    void CVort_think_vorticon_jump();
    void CVort_think_vorticon_search();
    void CVort_contact_vorticon(Sprite_T *vorticon, Sprite_T *contacted);
    void CVort_think_youth_walk();
    void CVort_think_youth_jump();
    void CVort_contact_youth(Sprite_T *youth, Sprite_T *contacted);

    int16_t CVort_add_sprite();
    int16_t CVort_add_body();

    void CVort_think_keen_ground();
    void CVort_think_keen_jump_ground();
    void CVort_think_keen_jump_air();
    void CVort_think_keen_shoot();
    void CVort_think_keen_pogo_air();
    void CVort_think_keen_pogo_ground();
    void CVort_think_keen_exit();
    void CVort_think_keen_death();
    void CVort_think_keen_stunned();
    extern void (*CVort_ptr_contact_keen)(Sprite_T *keen, Sprite_T * contacted);
    void CVort_body_border_flash(Body_T * body);
    void CVort_body_bridge_extend(Body_T * bridge);
    void CVort_body_bridge_retract(Body_T * bridge);

    // FIXME? This is sorta more vanilla like but not really needed...
    void CVort_contact_nop(Sprite_T *curr, Sprite_T * other);
    void CVort_think_contact_nop(Sprite_T *curr, Sprite_T * other);
    void CVort_body_nop(Body_T * body);

    void CVort_think_dead_sprite();
    void CVort_think_kill_sprite();
    void CVort_think_remove_sprite();
    void CVort_default_think();
    void CVort_default_contact(Sprite_T *curr, Sprite_T * other);

    void CVort_kill_keen();
    void CVort_kill_keen_temp();
    void CVort_move_left_right(int16_t acceleration);
    void CVort_pogo_jump(int16_t max_height, int16_t diff);

    void CVort_check_ceiling();

    void CVort_body_slide_door(Body_T * body);
    void CVort_open_door(int16_t tileX, int16_t tileY);

    void CVort_do_fall();
    int16_t CVort_compute_sprite_delta();
    int16_t CVort_check_ground();
    void CVort_carry_keen(Sprite_T *keen, Sprite_T *contacted);
    void CVort_push_keen(Sprite_T *keen, Sprite_T *pusher);

    void CVort_do_scrolling();
    int16_t CVort_sprite_active_screen();
    int16_t CVort_detect_sprite_col(Sprite_T *spr_0, Sprite_T * spr_1);
    void CVort_keen_bgtile_col();

    int16_t CVort_handle_global_keys();
    void CVort_add_score(int16_t points);
    void CVort_load_level_data(uint16_t levelnum);
    void CVort_handle_quit();
    void CVort_clear_keys();
    void CVort_init_rnd(bool doRand);
    int16_t CVort_get_random();
    uint32_t CVort_filelength(FILE * fp);
    void CVort_setup_jump_heights(uint16_t seed);
    int16_t CVort_calc_jump_height(uint16_t max_height);
    void CVort_chg_vid_and_error(const char *msg);

    uint16_t CVort_draw_level(uint16_t levelnum);

    void CVort_calibrate_joystick(int16_t arg_0);
    void CVort_draw_keyname(int16_t key);
    void CVort_handle_redef_keys();

    void CVort_init_ctrls();
    void CVort_save_ctrls();
    GameInput_T CVort_engine_getKeybCtrlState();
    GameInput_T CVort_engine_getMouseCtrl();
    GameInput_T CVort_engine_getJoystickCtrl(int16_t joy_id);
    GameInput_T CVort_handle_ctrl(uint16_t input_type);
    uint16_t CVort_translate_key(uint16_t type);

    void CVort_draw_box2(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
    void CVort_draw_box(uint16_t width, uint16_t height);
    // Draw a rectangle of the same char
    void CVort_draw_char_rectangle(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t character);
    void CVort_draw_box_opening_main(uint16_t width, uint16_t height);
    void CVort_draw_box_opening_horiz(uint16_t width, uint16_t height);
    void CVort_draw_box_opening_vert(uint16_t width, uint16_t height);

    uint16_t CVort_read_char_with_echo();
    void CVort_draw_string(const char *str);
    void CVort_draw_number_word(uint16_t value);
    void CVort_draw_number_dword(uint32_t value);
    void CVort_draw_stringz(const char *str);
    uint16_t CVort_get_string_input(char *strBuffer, uint16_t len);

    // Vanilla Keen gets some pointer; Either to an external file, or to
    // a location in the EXE. Here we use this.
    void CVort_process_text_file(uint8_t * buffer);
    // Keen 1 specific: Loads text from file first, and then process.
    // In contrary to vanilla Keen 1, THIS function is responsible for
    // about a missing text file, rather than process_text_file.
    void CVort_load_and_process_text_file(const char *filename, uint8_t ** pBuffer);

    uint16_t CVort_draw_text_page(uint8_t *text_src_ptr, int16_t *text_viewer_buffer, int16_t arg_6, int16_t arg_8);
    void CVort_engine_scrollText(int16_t top_line_offs, int16_t bot_line_offs, int16_t direction);
    void CVort_text_viewer_bkgrd(uint16_t box_x_pos, uint16_t box_y_pos, uint8_t *pointer, int16_t *arg_8, int16_t arg_A);
    void CVort_load_high_scores_table();
    void CVort_save_high_scores_table();
    void CVort_fade_in();
    void CVort_fade_out();
    void CVort_lights_on();
    void CVort_lights_out();
    void CVort_toggle_switch();

    /***********
    Engine stuff
    ***********/
    extern EngineArguments_T engine_arguments;
    extern /*const*/ gameversion_T engine_gameVersion;
    extern bool engine_skipLauncher, engine_isLauncherFullscreen;
    extern bool engine_forceSpecificEpisode;

    extern InputMappingStruct_T engine_inputMappings;
    extern const EmulatedKey_T engine_emulatedKeysTable[];

    extern CVort_engine_screen_T engine_screen;
    // A permenantly constant table of colors
    // in a specific internal format (ARGB or XRGB)
    extern const uint32_t engine_egaRGBColorTable[];

    extern uint8_t engine_currPage;
    extern uint16_t pel_panning;
    extern uint32_t engine_currPageStart; // Acts like page_current_seg
    extern uint32_t engine_dstPage; // Acts like dstseg
    extern uint32_t engine_egaStart; // Acts like ega_regen_start_addr

    extern uint32_t engine_gameTicksStart, engine_gameTicksDelayOffset;
    extern int64_t engine_sdlMicroTicksStart, engine_sdlMicroTicksDelayOffset;
    // A moment in time to wait for, measured in micro seconds
    extern int64_t engine_momentToDelayInMicroSec;
    // The time of a single (emulated) refresh in micro seconds
    extern int64_t engine_refreshTimeInMicroSec;

    /* Use this to reset the time on a call to engine_delay if needed
     * (used on startup or after a call to engine_shortSleep)
     */
    extern bool engine_doResetMomentToDelay;

    extern bool engine_isFrameReadyToDisplay;

    extern uint32_t engine_lastDisplayUpdateTime;

    extern EGAHeadGeneralSection_T engine_egaHeadGeneral;
    extern EGAHeadUnmasked_T *engine_egaHeadUnmasked;
    //extern EGAHeadSprite_T *engine_egaHeadSprite;
    extern MaskedSpriteEntry_T *engine_maskedSpriteEntry;

    // Note: These are not in planar format but rather single-byte per pixel
    extern uint8_t *engine_egaBmpData, *engine_egaFontData, *engine_egaTileData, *engine_egaSpriteData;
    extern uint8_t **engine_egaBmps, **engine_egaFonts, **engine_egaTiles, **engine_egaSprites;

    extern uint16_t engine_sndCount;

    extern sndChunk_T *sndChunks;
    extern void *sndChunksData;

    extern uint8_t sound_limiter, *sndPriorities;
    extern int engine_currSoundLocation;
    extern int16_t engine_currSoundPlaying;

    extern uint8_t engine_lastScanCode;
    extern uint32_t engine_lastKeyTime;
    extern bool engine_isBeforeKeyDelay;

    extern bool engine_isCursorLocked;

    // EXE file data (the uncompressed EXE without the header)
    extern uint8_t *exeImage;

    extern CVort_EXE_FIELDS_T exeFields;

    /************************************
    Vanilla Keen variables; Well, mostly.
     ************************************/
    // Grouped state structs
    extern GameState_T g_game;
    extern EntityManager_T g_entities;
    extern InputState_T g_input;

    // Initialized Variables
    extern uint16_t jump_height_table[18];
    extern uint16_t spritejump_1, spritejump_2;

    extern char game_ext[4];
    extern const uint32_t pitAltCounter;

    // BSS
    extern uint32_t ticks_sync;
    extern uint16_t tiledraws_c, bmpdraws_c, spritedraws_c;
    // Same for bmpdraws and more
    extern SpriteDraw_T spritedraws[0x1f4];
    extern BmpDraw_T bmpdraws[0xa];
    extern TileDraw_T tiledraws[0x64];
    extern uint16_t screentiles[0x258];
    // TODO: bmpdraws_i (bmpdraws_p) doesn't seem to actually be used...
    extern int bmpdraws_i, tiledraws_i, screentiles_i, spritedraws_i;

    extern uint16_t *TILEINFO_Anim;
    extern int16_t *TILEINFO_Type;
    extern int16_t *TILEINFO_UEdge, *TILEINFO_REdge, *TILEINFO_DEdge, *TILEINFO_LEdge;

    /* NOTE: This is a two-dimensional array of a variable size, depending on
     * the episode. Each episode has its own static "implementation" of the array.
     * We could actually use anim_plane, but there are references to
     * anim_frame_tiles in the game code...
     */
    extern uint16_t * anim_frame_tiles[4];

    extern uint16_t * anim_plane[4];
    extern int anim_plane_i;

    extern uint8_t *sound_data;

    extern uint16_t ATR[2][0x14A];

    extern uint8_t demo_actions_including_level_num[5001];
    extern uint8_t *demo_action_ptr, *end_of_demo_ptr, *demo_after_last_byte_char_offset;

    extern int16_t map_data[0x10000 / 2], *map_data_tiles, *map_data_sprites;
    extern int16_t map_width_tile, map_height_tile, map_width_bytes;
    extern uint16_t screen_wrap_single;
    //extern uint16_t screen_wrap;
    extern int32_t map_width, map_height;
    extern int32_t scroll_x, scroll_y, scroll_x_min, scroll_y_min, scroll_x_max, scroll_y_max, ceiling_x, ceiling_y;
    extern int16_t scroll_x_tile, scroll_y_tile;
    extern uint16_t cursorX, cursorX_save, cursorY;

    extern uint32_t cursorX_b, cursorY_b; // Cursor position in map units

    extern uint16_t textbox_x_pos, textbox_width, textbox_y_pos, textbox_height;
    extern uint16_t textbox_offs_x, textbox_offs_y;

    extern int16_t keen_tileX, keen_tileY;

    extern uint8_t *previews_txt, *story_text, *help_text, *end_text;

    extern HighScoresTable_T high_scores_table;
    extern Sprite_T keen_map[2];
    extern GameProfile_T keen_gp;

    extern uint32_t keen_wmap_x_pos, keen_wmap_y_pos;
    extern uint32_t wmap_scroll_x, wmap_scroll_y;
    extern Maplevel_T wmaplevels[0x10];

    extern DrawFunction_T draw_func;

    extern uint16_t text_viewer_top_pos, text_viewer_height, text_viewer_bottom;
    extern uint8_t *text_ptr;

    extern int16_t text_viewer_buffer[0x190], *text_viewer_buffer_ptr;

    extern uint16_t messie_mounted, messie_frame, messie_time_to_climb, messie_move_tics;
    extern uint16_t messie_x_T, messie_y_T;
    extern int32_t messie_xpos, messie_ypos, messie_del_x, messie_del_y;

    extern Teleporter_T teleporters[16];

#endif
