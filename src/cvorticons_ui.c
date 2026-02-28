#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "cvorticons.h"
#include "cvorticons1.h"
#include "cvorticons2.h"
#include "cvorticons3.h"
#include "decompression/CGenius/fileio/compression/CRLE.h"

#include "cvorticons_episode_macros.h"

void CVort_draw_keyname(int16_t key) {
    key &= 0x7F;
    if (key == 1) {
        CVort_draw_string("ESC");
        return;
    }
    if (key == 0xE) {
        CVort_draw_string("BKSP");
        return;
    }
    if (key == 0xF) {
        CVort_draw_string("TAB");
        return;
    }
    if (key == 0x1D) {
        CVort_draw_string("CTRL");
        return;
    }
    if (key == 0x2A) {
        CVort_draw_string("LSHIFT");
        return;
    }
    if (key == 0x39) {
        CVort_draw_string("SPACE");
        return;
    }
    if (key == 0x3A) {
        CVort_draw_string("CAPSLK");
        return;
    }
    if ((key >= 0x3B) && (key <= 0x44)) {
        CVort_draw_string("F");
        char fKeyNumAsStr[3];
        sprintf(fKeyNumAsStr, "%" PRId16, key - 0x3A);
        CVort_draw_string(fKeyNumAsStr);
        return;
    }
    if (key == 0x57) {
        CVort_draw_string("F11");
        return;
    }
    // Should really be 0x58, but vanilla Keen 1 checks for 0x59
    // and the result is a question mark displayed...
    if (key == 0x59) {
        CVort_draw_string("F12");
        return;
    }
    if (key == 0x46) {
        CVort_draw_string("SCRLLK");
        return;
    }
    if (key == 0x1C) {
        CVort_draw_string("ENTER");
        return;
    }
    if (key == 0x36) {
        CVort_draw_string("RSHIFT");
        return;
    }
    if (key == 0x37) {
        CVort_draw_string("PRTSC");
        return;
    }
    if (key == 0x38) {
        CVort_draw_string("ALT");
        return;
    }
    if (key == 0x47) {
        CVort_draw_string("HOME");
        return;
    }
    if (key == 0x49) {
        CVort_draw_string("PGUP");
        return;
    }
    if (key == 0x4F) {
        CVort_draw_string("END");
        return;
    }
    if (key == 0x51) {
        CVort_draw_string("PGDN");
        return;
    }
    if (key == 0x52) {
        CVort_draw_string("INS");
        return;
    }
    if (key == 0x53) {
        CVort_draw_string("DEL");
        return;
    }
    if (key == 0x45) {
        CVort_draw_string("NUMLK");
        return;
    }
    CVort_engine_drawChar(cursorX, cursorY << 3, exeFields.char_map[key]);
    cursorX++;
}

void CVort_handle_redef_keys() {
    int8_t lastAnswer;
    int16_t shiftedAnswer, checkedKey;
    CVort_draw_box_opening_main(22, 14);
    CVort_draw_string("Keyboard Commands\n");
    CVort_draw_string("-----------------");
    CVort_draw_string("\n0 north    :");
    CVort_draw_string("\n1 northeast:");
    CVort_draw_string("\n2 east     :");
    CVort_draw_string("\n3 southeast:");
    CVort_draw_string("\n4 south    :");
    CVort_draw_string("\n5 southwest:");
    CVort_draw_string("\n6 west     :");
    CVort_draw_string("\n7 northwest:");
    CVort_draw_string("\n8 button1  :");
    CVort_draw_string("\n9 button2  :");
    CVort_draw_string("\nModify which action:");
    int16_t box_x_pos = cursorX, box_y_pos = cursorY;
    for (int16_t counter = 0; counter < 8; counter++) {
        cursorX = 0x1A;
        cursorY = counter + 7;
        CVort_draw_keyname(sc_dir[counter]);
    }
    cursorX = 0x1A;
    cursorY = 0xF;
    CVort_draw_keyname(sc_but1);
    cursorX = 0x1A;
    cursorY = 0x10;
    CVort_draw_keyname(sc_but2);
    //engine_updateActualDisplay();
    do {
        cursorX = box_x_pos;
        cursorY = box_y_pos;
        lastAnswer = CVort_read_char_with_echo() % 0x100;
        // Is the answer in the range '0'..'9'?
        if ((lastAnswer >= 0x30) && (lastAnswer <= 0x39)) {
            shiftedAnswer = (int16_t) (lastAnswer) - 0x30;
            CVort_engine_drawChar(cursorX, cursorY << 3, lastAnswer);
            // TODO: Vanilla code does it again!! Do it here, too?
            //shiftedAnswer = lastAnswer-0x30;
            CVort_draw_string("\n\rPress the new key:");
            //engine_updateActualDisplay();
            CVort_clear_keys();
            checkedKey = -1;
            do {
                checkedKey++;
                if (key_map[checkedKey])
                    break;
                if (checkedKey == 0x79) {
                    checkedKey = -1;
                    // If we sleep on EVERY loop iteration, it can take
                    // too long before we reach a key currently being held!
                    CVort_engine_shortSleep();
                }
                else if (checkedKey == 0x29)
                    checkedKey++;
            } while (1);
            CVort_clear_keys();
            CVort_draw_string("\r                  ");
            if (shiftedAnswer < 8)
                sc_dir[shiftedAnswer] = checkedKey;
            if (shiftedAnswer == 8)
                sc_but1 = checkedKey;
            if (shiftedAnswer == 9)
                sc_but2 = checkedKey;

            cursorY = shiftedAnswer + 7;
            cursorX = 0x1A;
            CVort_draw_string("        ");
            cursorX = 0x1A;
            CVort_draw_keyname(checkedKey);

            lastAnswer = 0x30;
        }
    } while ((lastAnswer >= 0x30) && (lastAnswer <= 0x39));
    ctrl_type[1] = 0;
}

void CVort_init_ctrls() {
    strcpy(string_buf, "CTLPANEL.");
    strcat(string_buf, game_ext);
    FILE *fp = CVort_engine_cross_rw_misc_fopen(string_buf, "rb");
    if (fp) {
        CVort_engine_cross_freadInt16LE(&want_sound, 1, fp);
        CVort_engine_cross_freadInt16LE(ctrl_type, 3, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[0], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[0] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[0] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[2], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[2] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[2] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[1], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[1] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[1] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[3], 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[3] + 1, 1, fp);
        CVort_engine_cross_freadInt16LE(joystick_ctrl[3] + 2, 1, fp);
        CVort_engine_cross_freadInt16LE(&mouse_ctrl_1, 1, fp);
        CVort_engine_cross_freadInt8LE(sc_dir, 8, fp);
        CVort_engine_cross_freadInt8LE(&sc_but1, 1, fp);
        CVort_engine_cross_freadInt8LE(&sc_but2, 1, fp);
        fclose(fp);
    } else {
        want_sound = 1;
        ctrl_type[1] = 0;
        ctrl_type[2] = 2;
        joystick_ctrl[0][1] = joystick_ctrl[0][2] = joystick_ctrl[2][1] = joystick_ctrl[2][2] = 20;
        joystick_ctrl[1][1] = joystick_ctrl[1][2] = joystick_ctrl[3][1] = joystick_ctrl[3][2] = 60;
        mouse_ctrl_1 = 5;

        sc_dir[0] = 0x48;
        sc_dir[1] = 0x49;
        sc_dir[2] = 0x4D;
        sc_dir[3] = 0x51;
        sc_dir[4] = 0x50;
        sc_dir[5] = 0x4F;
        sc_dir[6] = 0x4B;
        sc_dir[7] = 0x47;
        sc_but1 = 0x1D;
        sc_but2 = 0x38;
    }
}

void CVort_save_ctrls() {
    strcpy(string_buf, "CTLPANEL.");
    strcat(string_buf, game_ext);
    FILE *fp = CVort_engine_cross_rw_misc_fopen(string_buf, "wb");
    if (!fp)
        return;
    CVort_engine_cross_fwriteInt16LE(&want_sound, 1, fp);
    CVort_engine_cross_fwriteInt16LE(ctrl_type, 3, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[0], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[0] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[0] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[2], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[2] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[2] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[1], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[1] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[1] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[3], 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[3] + 1, 1, fp);
    CVort_engine_cross_fwriteInt16LE(joystick_ctrl[3] + 2, 1, fp);
    CVort_engine_cross_fwriteInt16LE(&mouse_ctrl_1, 1, fp);
    CVort_engine_cross_fwriteInt8LE(sc_dir, 8, fp);
    CVort_engine_cross_fwriteInt8LE(&sc_but1, 1, fp);
    CVort_engine_cross_fwriteInt8LE(&sc_but2, 1, fp);
    fclose(fp);
}

GameInput_T CVort_handle_ctrl(uint16_t input_type) {
    GameInput_T input_ret;
    //CVort_engine_updateInputStatus();
    if ((demo_status == DEMO_OFF) || (demo_status == DEMO_RECORD)) {
        switch (ctrl_type[input_type]) {
            case CONTROL_KEYB:
                input_ret = CVort_engine_getKeybCtrlState();
                break;
            case CONTROL_MOUSE:
                input_ret = CVort_engine_getMouseCtrl();
                break;
            case CONTROL_JOY_1:
                input_ret = CVort_engine_getJoystickCtrl(1);
                break;
            case CONTROL_JOY_2:
                input_ret = CVort_engine_getJoystickCtrl(2);
                break;
            default:
                break;
        }
        if (demo_status == DEMO_RECORD) {
            if (engine_arguments.extras.vorticonsDemoModeToggle && (demo_action_ptr == demo_after_last_byte_char_offset)) {
                CVort_chg_vid_and_error("Demo buffer overflow");
            }
            *demo_action_ptr = (input_ret.direction << 2) | (input_ret.but2pogo << 1) | input_ret.but1jump;
            demo_action_ptr++;
        }
    } else {
        if (engine_arguments.extras.vorticonsDemoModeToggle && (demo_action_ptr == end_of_demo_ptr)) {
            quit_to_title = 2; // SPECIAL VALUE (HALT BY DEMO, *NOT* PLAYER)

            // FIXME: On a second thought that's malformed
            // behavior (what is input_ret??)... Oh well
            return input_ret;
        }
        input_ret.but1jump = (*demo_action_ptr) & 1;
        input_ret.but2pogo = ((*demo_action_ptr) & 2) >> 1;
        input_ret.direction = ((*demo_action_ptr) & 60) >> 2;
        demo_action_ptr++;
    }
    return input_ret;
}

uint16_t CVort_translate_key(uint16_t type) {
    //CVort_engine_updateInputStatus();
    if (type) {
        if (!(key_scane & 0x80))
            return 0;
        uint8_t returnHigh = key_scane & 0x7f;
        key_code = exeFields.trans_map[returnHigh];
        if (key_code)
            return (key_code | (returnHigh << 8));
        return 0;
    }
    do {
        if (key_scane & 0x80) // Should be ((int8_t)key_scane < 0) or so?
        {
            key_scane &= 0x7f;
            if (exeFields.trans_map[key_scane]) {
                key_code = exeFields.trans_map[key_scane];
                return (key_code | (key_scane << 8));
            }
        }
        // Rather than a busy loop, we wait for a new event here.
        //SDL_WaitEvent(NULL);
        /* ...or wait, this is *a bit* less true to the vanilla behavior
        in terms of continuous "Space" key presses while Pause menu is
        displayed. Perfect emulation may be harder and, in fact, even on
        on real DOS it may depend on currently set keyboard behavior,
        in terms of "CVort_engine_delay" and "repeat".
        Anyway... just do a short sleep for now.                      */

       // Without this, moving windows around can result in loss of
       // on-screen contents. It may further be a good chance to
       // update these, if there is anything to update.
       //engine_updateActualDisplay();
       CVort_engine_shortSleep();

    } while (1);
}

void CVort_draw_box2(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height) {
    textbox_x_pos = xpos;
    textbox_y_pos = ypos;
    textbox_width = width;
    textbox_height = height;
    CVort_engine_drawChar(xpos, ypos << 3, 1);
    uint16_t currX, currY;
    for (currX = xpos + 1; currX < width; currX++)
        CVort_engine_drawChar(currX, ypos << 3, 2);
    CVort_engine_drawChar(width, ypos << 3, 3);
    for (currY = ypos + 1; currY < height; currY++) {
        CVort_engine_drawChar(xpos, currY << 3, 4);
        for (currX = xpos + 1; currX < width; currX++)
            CVort_engine_drawChar(currX, currY << 3, 0x20);
        CVort_engine_drawChar(width, currY << 3, 5);
    }
    CVort_engine_drawChar(xpos, height << 3, 6);
    for (currX = xpos + 1; currX < width; currX++)
        CVort_engine_drawChar(currX, height << 3, 7);
    CVort_engine_drawChar(width, height << 3, 8);

    cursorX = cursorX_save = xpos + 1;
    cursorY = ypos + 1;
}

void CVort_draw_box(uint16_t width, uint16_t height) {
    uint16_t x, y;
    x = textbox_offs_x - width / 2;
    y = textbox_offs_y - height / 2;
    CVort_draw_box2(x, y, x + width + 1, y + height + 1);
}

// Draw a rectangle of the same char

void CVort_draw_char_rectangle(uint16_t x_pos, uint16_t y_pos, uint16_t width, uint16_t height, uint16_t character) {
    for (uint16_t currY = y_pos, currX; currY <= height; currY++)
        for (currX = x_pos; currX <= width; currX++)
            CVort_engine_drawChar(currX, currY << 3, character);
}

//void CVort_draw_box_opening2(uint16_t width, uint16_t height)

void CVort_draw_box_opening_main(uint16_t width, uint16_t height) {
    if (width <= 2) {
        if (height > 2)
            CVort_draw_box_opening_vert(width, height - 2);
    } else if (height <= 2)
        CVort_draw_box_opening_horiz(width - 2, height);
    else
        CVort_draw_box_opening_main(width - 2, height - 2);
    CVort_engine_delay(1);
    CVort_draw_box(width, height);
    //engine_updateActualDisplay();
}

//void CVort_draw_box_opening(uint16_t width, uint16_t height)

void CVort_draw_box_opening_horiz(uint16_t width, uint16_t height) {
    if (width > 2)
        CVort_draw_box_opening_horiz(width - 2, height);
    CVort_engine_delay(1);
    CVort_draw_box(width, height);
    //engine_updateActualDisplay();
}

//void CVort_sub_1614B(uint16_t width, uint16_t height)

void CVort_draw_box_opening_vert(uint16_t width, uint16_t height) {
    if (height > 2)
        CVort_draw_box_opening_vert(width, height - 2);
    CVort_engine_delay(1);
    CVort_draw_box(width, height);
    //engine_updateActualDisplay();
}

uint16_t CVort_read_char_with_echo() {
    uint16_t currOnScreenCh = 9, currCh;
    do {
        currCh = CVort_translate_key(1)&0xFF;
        if (!currCh && (currOnScreenCh < 0xD)) {
            CVort_engine_drawChar(cursorX, cursorY << 3, currOnScreenCh);
            currOnScreenCh++;
            //engine_updateActualDisplay();
            CVort_engine_delay(5);
            continue;
        }
        if (currCh)
            break;
        currOnScreenCh = 9;
    } while (1);
    CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
    return CVort_translate_key(0);
}

void CVort_draw_string(const char *str) {
    while (*str) {
        if (*str == CHAR_LINEFEED) {
            cursorY++;
            cursorX = cursorX_save;
        } else if (*str == CHAR_CARRIAGE)
            cursorX = cursorX_save;
        else {
            CVort_engine_drawChar(cursorX, cursorY << 3, *str);
            cursorX++;
        }
        str++;
    }
}

void CVort_draw_number_word(uint16_t value) {
    sprintf(string_buf, "%" PRIu16, value);
    CVort_draw_string(string_buf);
}

void CVort_draw_number_dword(uint32_t value) {
    sprintf(string_buf, "%" PRIu32, value);
    CVort_draw_string(string_buf);
}

void CVort_draw_stringz(const char *str) {
    cursorX = textbox_offs_x + 1 - (strlen(str) >> 1);
    CVort_draw_string(str);
}

uint16_t CVort_get_string_input(char *strBuffer, uint16_t len) {
    char lastCharRead;
    uint16_t currChIndex = 0;
    do {
        lastCharRead = CVort_engine_toupper(CVort_read_char_with_echo()&0xFF);
        // A key like Backspace or Delete pressed?
        if (((lastCharRead == 0x7F) || (lastCharRead == 8)) && (currChIndex > 0)) {
            currChIndex--;
            CVort_engine_drawChar(cursorX, cursorY << 3, ' ');
            cursorX--;
        }// A standard character (out of most of the ASCII chars)?
        else if ((lastCharRead >= 0x20) && (lastCharRead <= 0x7A) && (currChIndex < len)) {
            strBuffer[currChIndex] = lastCharRead;
            currChIndex++;
            CVort_engine_drawChar(cursorX, cursorY << 3, lastCharRead);
            cursorX++;
        }
    } while ((lastCharRead != 0x1B) && (lastCharRead != 0xD));
    // Set the rest of the characters to NULL (this is vanilla behavior).
    for (; currChIndex < len; currChIndex++)
        strBuffer[currChIndex] = '\0';
    if (lastCharRead == 0xD) // Carriage return
        return 1;
    return 0; // Escape
}

void CVort_fade_in() {
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[0]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[1]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
}

void CVort_fade_out() {
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[2]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[1]);
    CVort_engine_delay(8);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[0]);
}

// FACT: Out of all unmodified games, these are actually used in Keen 2 only.
// Custom levels for vanilla Keen1 with light switches are possible, though.

void CVort_lights_on() {
    lights = 1;
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[3]);
}

void CVort_lights_out() {
    lights = 0;
    CVort_engine_delay(1);
    CVort_engine_setPaletteAndBorderColor(exeFields.palettes[1]);
}

void CVort_toggle_switch() {
    int16_t loopVar;
    CVort_engine_setCurSound(0x19);
    // Looks like a tantalus ray has been activated... oops!
    if (map_data_tiles[(keen_tileY + 5) * map_width_T + keen_tileX + 3] == 0x1DF) {
        int32_t origScrollX = scrollX, origScrollY = scrollY;
        for (loopVar = 1; loopVar < 80; loopVar++) {
            CVort_engine_syncDrawing();
            scrollX = origScrollX - ((CVort_calc_jump_height(0x40) - 0x20) << 8);
            scrollY = origScrollY - ((CVort_calc_jump_height(0x40) - 0x20) << 8);
            CVort_engine_drawSpriteAt(temp_sprite.posX, temp_sprite.posY, temp_sprite.frame);
            CVort_engine_drawScreen();
        }
        CVort_draw_box_opening_main(5, 1);
        CVort_draw_string("Oops.");
        //engine_updateActualDisplay();
        CVort_engine_delay(100);
        level_finished = LEVEL_END_TANTALUS;
        return;
    }
    switch (map_data_tiles[keen_tileY * map_width_T + keen_tileX]) {
        case 0x10F:
            if (lights)
                CVort_lights_out();
            else
                CVort_lights_on();
            return;
        case 0x1E0:
            map_data_tiles[keen_tileY * map_width_T + keen_tileX] = 0x1ED;
            break;
        case 0x1ED:
            map_data_tiles[keen_tileY * map_width_T + keen_tileX] = 0x1E0;
            break;
        default:;
    }
    int16_t var_2 = map_data_sprites[keen_tileY * map_width_T + keen_tileX];
    int16_t var_A = CVort_engine_signExtend8To16(var_2 & 0xFF);
    int16_t var_6 = keen_tileX + var_A;
    int16_t var_8 = keen_tileY + CVort_engine_signExtend8To16((var_2 >> 8) & 0xFF);
    // Going over the bodies
    for (loopVar = 0; loopVar < num_bodies; loopVar++) {
        if (!bodies[loopVar].type_)
            continue;
        if (bodies[loopVar].tile_x != var_6)
            continue;
        if (bodies[loopVar].tile_y == var_8)
            break;
    }
    if (loopVar < num_bodies) {
        if (bodies[loopVar].think_ptr == &CVort_body_bridge_retract)
            (bodies[loopVar].think_ptr = &CVort_body_bridge_extend);
        else
            (bodies[loopVar].think_ptr = &CVort_body_bridge_retract);
        return;
    }
    int16_t bodyIndex = CVort_add_body();
    bodies[bodyIndex].type_ = 2;
    bodies[bodyIndex].think_ptr = &CVort_body_bridge_extend;
    bodies[bodyIndex].tile_x = var_6;
    bodies[bodyIndex].tile_y = var_8;
    bodies[bodyIndex].variant = 0;
    if (TILEINFO_LEdge[map_data_tiles[var_8 * map_width_T + var_6 + 1]])
        bodies[bodyIndex].field_C = -1;
    else
        bodies[bodyIndex].field_C = 1;
    bodies[bodyIndex].field_E = map_data_tiles[var_8 * map_width_T + var_6];
    bodies[bodyIndex].field_10 = 0;
}
