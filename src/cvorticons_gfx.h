#ifndef _CHOCOLATE_KEEN_GFX_
#define _CHOCOLATE_KEEN_GFX_

#include <stdint.h>

/*********
Some entry
*********/

typedef struct
{
	uint16_t width, height;
	uint16_t loc_offset, location;
	uint16_t hitbox_l, hitbox_u, hitbox_r, hitbox_b;
	char name[12];
	uint32_t h_v_off; // Unused?
} MaskedSpriteEntry_T;

/************
EGAHead stuff
************/

typedef struct
{
	uint32_t latchPlaneSize, sprPlaneSize, imgDataStart, sprDataStart;
	uint16_t fontNum;
	uint32_t fontLoc;
	uint16_t unkNum; // Unused
	uint32_t unkLoc; // Unused
	uint16_t tileNum;
	uint32_t tileLoc;
	uint16_t bmpNum;
	uint32_t bmpLoc;
	uint16_t spriteNum;
	uint32_t spriteLoc;
	uint16_t compression;
} EGAHeadGeneralSection_T; // Section 1

typedef struct
{
	uint16_t h, v;
	uint32_t loc;
	char name[8];
} EGAHeadUnmasked_T; // Section 2

#if 0
typedef struct
{
	MaskedSpriteEntry_T entry;
	uint8_t copies[3*32]; // Unused?
} EGAHeadSprite_T; // Section 3
#endif

/* EGDHeadSprite_T is a special case here: Each sprite consists of one 32-byte
sized entry, along with 3 extra entries for shifts of the same sprite.
For compatibility with original code, we actually treat each such shift as its
own unique sprite. So, we really use MaskedSpriteEntry_T.
For reference, though a definition is given here.                           */
typedef struct
{
	MaskedSpriteEntry_T entries[4];
} EGAHeadSprite_T;

/*************
Things to draw
*************/

typedef struct
{
	uint16_t x_byte;  // 2 bytes per tile, screen x byte
	uint16_t y_line;  // line on screen (buffer of 32 lines = two tiles)
	uint16_t tile_id; // tileID number
} TileDraw_T;

typedef struct
{
	uint16_t x, y;
	uint16_t bmp_loc; // offset of bmp location in the plane
} BmpDraw_T;

typedef struct
{
	uint16_t x_byte;      // 2 bytes per tile (4 byte left hand buffer)
	uint16_t y_row;       // 16 rows per tile (32 row buffer)
	uint16_t sprite_copy; // masked sprite entry number
} SpriteDraw_T;

#endif
