// constants.h: Declares constants interfaces for the core subsystem.

#ifndef _CHOCOLATE_KEEN_VORTICONS_CONSTANTS_
#define _CHOCOLATE_KEEN_VORTICONS_CONSTANTS_

#define PC_PIT_RATE 1193182 // It is actually 1193181.8181...Hz
// The refresh rate is approximately 70.086Hz
#define ENGINE_VGA_REFRESHRATE_SCALED 70086
#define ENGINE_EGA_REFRESHRATE_SCALED 59920
#define ENGINE_VGA_CURSOR_BLINK_VERT_FRAME_RATE 8
#define ENGINE_EGA_CURSOR_BLINK_VERT_FRAME_RATE 8
#define ENGINE_VGA_TEXT_BLINK_VERT_FRAME_RATE 16
#define ENGINE_EGA_TEXT_BLINK_VERT_FRAME_RATE 16
#define ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR 1000
#define ENGINE_EGA_GFX_WIDTH 320
#define ENGINE_EGA_GFX_HEIGHT 200
#define ENGINE_EGACGA_TXT_PIXEL_WIDTH 640
#define ENGINE_EGACGA_TXT_PIXEL_HEIGHT 200
#define ENGINE_EGA_TXT_PIXEL_WIDTH 640
#define ENGINE_EGA_TXT_PIXEL_HEIGHT 350
#define ENGINE_VGA_TXT_PIXEL_WIDTH 720
#define ENGINE_VGA_TXT_PIXEL_HEIGHT 400
#define ENGINE_EGAVGA_TXT_COLS_NUM 80
#define ENGINE_EGAVGA_TXT_ROWS_NUM 25

#define ENGINE_GUI_WIDTH 320
#define ENGINE_GUI_HEIGHT 240

// Video modes used by the engine for launcher, text, and graphics output.
#define CVORT_VIDEO_MODE_LAUNCHER     (-1)
#define CVORT_VIDEO_MODE_TEXT         3
// BIOS mode 0x0D is the graphics-mode identifier used here; VGA vs EGA
// behavior is configured separately via the emulated adapter settings.
#define CVORT_VIDEO_MODE_GRAPHICS     0xD

// A scanline width of 320x2*32=384. This is the length of a scanline used in
// vanilla Keen (where only 320 pixels are actually seen).
// Note that the vanilla code itself sets this by sending the number 24
// to a CRTC register. Apparently, it is obtained by: 24 = (384/8/2).

#define ENGINE_EGA_GFX_SCANLINE_LEN 384
//#define ENGINE_EGA_SCANLINEEXTRA 32 // Leads to a width of 320+2*32=384

// Overscan border dimensions (for each side of the screen)
// For the textual case on the CGA (the adapter), see one of the posts here:
// vogons.zetafleet.com/viewtopic.php?t=9814&postdays=0&postorder=asc&start=60
#define ENGINE_EGA_GFX_OVERSCAN_LEFT 16
#define ENGINE_EGA_GFX_OVERSCAN_RIGHT 40
#define ENGINE_EGA_GFX_OVERSCAN_TOP 22
#define ENGINE_EGA_GFX_OVERSCAN_BOTTOM 24
// EGA textual mode, CGA monitor
#define ENGINE_EGACGA_TXT_OVERSCAN_LEFT 24
#define ENGINE_EGACGA_TXT_OVERSCAN_RIGHT 96
#define ENGINE_EGACGA_TXT_OVERSCAN_TOP 22
#define ENGINE_EGACGA_TXT_OVERSCAN_BOTTOM 24
// EGA textual mode, EGA monitor
#define ENGINE_EGA_TXT_OVERSCAN_LEFT 48
#define ENGINE_EGA_TXT_OVERSCAN_RIGHT 24
#define ENGINE_EGA_TXT_OVERSCAN_TOP 4
#define ENGINE_EGA_TXT_OVERSCAN_BOTTOM 0
// VGA
#define ENGINE_VGA_GFX_OVERSCAN_LEFT 8
#define ENGINE_VGA_GFX_OVERSCAN_RIGHT 8
#define ENGINE_VGA_GFX_OVERSCAN_TOP_AFTER_DOUBLING 7    // 200-line doubling
#define ENGINE_VGA_GFX_OVERSCAN_BOTTOM_AFTER_DOUBLING 7 // 200-line doubling
#define ENGINE_VGA_TXT_OVERSCAN_LEFT 8
#define ENGINE_VGA_TXT_OVERSCAN_RIGHT 8
#define ENGINE_VGA_TXT_OVERSCAN_TOP 7
#define ENGINE_VGA_TXT_OVERSCAN_BOTTOM 7
// Some more relevant info, for the simulation of vertical retrace status
// check; All values are measured in scanlines.
#define ENGINE_EGACGA_VERTICAL_RETRACE_LEN 3
#define ENGINE_EGACGA_TOTAL_SCANLINE_COUNT 262

#define ENGINE_EGA_TXT_VERTICAL_RETRACE_LEN 13
#define ENGINE_EGA_TXT_TOTAL_SCANLINE_COUNT 366

#define ENGINE_VGA_VERTICAL_RETRACE_LEN 2
#define ENGINE_VGA_TOTAL_SCANLINE_COUNT 449

// Viewport dimensions in tiles (how many tiles are visible on screen)
#define ENGINE_VIEWPORT_WIDTH_TILES  23  // 0x17 tiles wide
#define ENGINE_VIEWPORT_HEIGHT_TILES 12  // 0xC tiles high
#define ENGINE_VIEWPORT_MAX_X_TILE   21  // 0x15 - max tile X coordinate for screen drawing
#define ENGINE_VIEWPORT_MAX_Y_TILE   19  // 0x13 - max tile Y coordinate for episode 3

// Extended viewport bounds for sprite activation (viewport + margin)
#define ENGINE_VIEWPORT_ACTIVATE_WIDTH_TILES  28  // 0x1C
#define ENGINE_VIEWPORT_ACTIVATE_HEIGHT_TILES 18  // 0x12
#define ENGINE_VIEWPORT_ACTIVATE_MARGIN_X     8   // margin for activation check
#define ENGINE_VIEWPORT_ACTIVATE_MARGIN_Y     8   // margin for activation check

// Sprite visibility margins (tiles from viewport edge)
#define ENGINE_SPRITE_MARGIN_MIN  2   // minimum margin from viewport left/top
#define ENGINE_SPRITE_MARGIN_CANNON 4 // margin for cannon firing checks
#define ENGINE_SPRITE_MARGIN_LARGE 8 // large margin for sprite active check

// Extended viewport for cannon checks
#define ENGINE_VIEWPORT_CANNON_EXTRA_X 24 // 0x18
#define ENGINE_VIEWPORT_CANNON_EXTRA_Y 14 // 0xE

// Sprite rendering/clipping bounds (for tile-based sprite drawing)
#define ENGINE_SPRITE_CLIP_MAX_X       21  // 0x15 - max tile X for sprite drawing
#define ENGINE_SPRITE_CLIP_MAX_X2      20  // 0x14 - max tile X for sprite end in some functions
#define ENGINE_SPRITE_CLIP_MAX_Y1      14  // 0xE - max tile Y for sprite start check
#define ENGINE_SPRITE_CLIP_MAX_Y2      13  // 0xD - max tile Y for sprite end check
#define ENGINE_SCREEN_TILE_ROW_WIDTH   21  // 0x15 - number of tiles per row in screen buffer

// In the original game, 4 planes of EGA memory are used. For each such plane,
// 6000h=24576 bytes can be used (or 3000h=12288 per page for the page flipping
// feature). This consists of a total of 18000h=98304 bytes. In our case,
// we want to use one such area of memory - and in bytes. That's done
// since in our case, we store one byte per pixel (rather than 4 bits).
// So we want double the area. That is, 30000h bytes.
//
// ...HOWEVER, with just 0x30000 bytes, out-of-bounds write attempts are
// expected. The Apogee logo seen on startup (well, drawn but not yet seen) is
// such an example. So we... double the memory amount for now.
//
// The GBA build overrides this to something much smaller (see build/gba/
// Makefile) because 384 KB does not fit in 256 KB EWRAM. Out-of-bounds
// writes on the overridden size are tolerated for boot purposes only.
#ifndef ENGINE_EGA_MEMORY_IN_BITS
#define ENGINE_EGA_MEMORY_IN_BITS 0x60000
#endif
//#define ENGINE_EGA_MEMORY_IN_BITS 0x30000

// Memory for 80x25 text mode, including color attributes
#define ENGINE_EGA_TEXTUAL_MEMORY_IN_BYTES (ENGINE_EGAVGA_TXT_COLS_NUM*ENGINE_EGAVGA_TXT_ROWS_NUM*2)

#define ENGINE_SNDDEFAULTSAMPLERATE 48000
#define ENGINE_SNDBYTEDEPTH 2

#define PALETTE_NUM_OF_ENTRIES 17

// NOTE? Should better be a whole enum of tiles...although less useful for MODs
#define TILES_TELESNOW 0x63
#define TILES_JOYSTICK 0xDD
#define TILES_TELEDIRT 0x145
#define TILES_TELEDIRT0 0x152
#define TILES_TELESNOW0 0x156
// NOTE? Same but for sounds
#define TELEPORTSND 0x12

#endif
