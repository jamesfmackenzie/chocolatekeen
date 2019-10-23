#include <assert.h>
#include "SDL.h"
#include "cvorticons.h"
#include "cvorticons1.h"
#include "cvorticons2.h"
#include "cvorticons3.h"
#include "decompression/imageRLE.h"
#include "decompression/CGenius/fileio/lz.h"
#include "../rsrc/chocolate-keen_icon.h"

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
#ifndef NO_SDL_GLEXT
#define NO_SDL_GLEXT
#endif
#include "SDL_opengl.h"
#include "GL/glext.h"
#endif

// Set in CVort_engine_resetWindow
struct {
	int fullWidth, fullHeight, windowWidth, windowHeight;
	gfxOutputSystem_T outputSystem;
	bool isFullscreen;
#if SDL_VERSION_ATLEAST(2,0,0)
	int rendererDriverIndex;
#endif
	gfxScaleType_T scaleType;
	int zoomLevel;
	bool vSync;
	bool bilinearInterpolation;
	bool offScreenRendering;
} engine_gfx_effective_arguments;

// TODO: The following implementation is currently...somewhat messy.
void CVort_engine_decompGraphics()
{
	/******************************
	First part: Load the EGA header
	******************************/
	strcpy(string_buf, "EGAHEAD.");
	strcat(string_buf, game_ext);
	FILE *fp = CVort_engine_cross_ro_data_fopen(string_buf);
	// TODO: What if we fail to load the file?
	if (!fp)
		return;
	/***********************************************************************
	WARNING: We should *NOT* read the EGA Header as-is and then simply treat
	parts of it as structs. There are two issues with it:
	- Endianness conversions may be required.
	- The actual size of a struct may be larger than what you expect,
	due to compiler optimizations.
	***********************************************************************/
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.latchPlaneSize, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.sprPlaneSize, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.imgDataStart, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.sprDataStart, 1, fp);
	CVort_engine_cross_freadInt16LE(&engine_egaHeadGeneral.fontNum, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.fontLoc, 1, fp);
	CVort_engine_cross_freadInt16LE(&engine_egaHeadGeneral.unkNum, 1, fp); // Unused
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.unkLoc, 1, fp); // Unused
	CVort_engine_cross_freadInt16LE(&engine_egaHeadGeneral.tileNum, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.tileLoc, 1, fp);
	CVort_engine_cross_freadInt16LE(&engine_egaHeadGeneral.bmpNum, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.bmpLoc, 1, fp);
	CVort_engine_cross_freadInt16LE(&engine_egaHeadGeneral.spriteNum, 1, fp);
	CVort_engine_cross_freadInt32LE(&engine_egaHeadGeneral.spriteLoc, 1, fp);
	CVort_engine_cross_freadInt16LE(&engine_egaHeadGeneral.compression, 1, fp);
	// Now we get to the second section
	engine_egaHeadUnmasked = (EGAHeadUnmasked_T *)malloc(engine_egaHeadGeneral.bmpNum*sizeof(EGAHeadUnmasked_T));
	uint16_t loopVar;
	uint32_t bmpTotalPixelCount = 0;
	fseek(fp, engine_egaHeadGeneral.imgDataStart, SEEK_SET);
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.bmpNum; loopVar++)
	{
		CVort_engine_cross_freadInt16LE(&engine_egaHeadUnmasked[loopVar].h, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_egaHeadUnmasked[loopVar].v, 1, fp);
		CVort_engine_cross_freadInt32LE(&engine_egaHeadUnmasked[loopVar].loc, 1, fp);
		fread(engine_egaHeadUnmasked[loopVar].name, 8, 1, fp);
		bmpTotalPixelCount += 8*engine_egaHeadUnmasked[loopVar].h*engine_egaHeadUnmasked[loopVar].v;
	}
	// We now jump to the third, although we treat each "copy" out of
	// 4 shifts of a sprite as four INDIVIDUAL entries.
	engine_maskedSpriteEntry = (MaskedSpriteEntry_T *)malloc(4*engine_egaHeadGeneral.spriteNum*sizeof(MaskedSpriteEntry_T));
	uint32_t spriteTotalPixelCount = 0;
	fseek(fp, engine_egaHeadGeneral.sprDataStart, SEEK_SET);
	for (loopVar = 0; loopVar < 4*engine_egaHeadGeneral.spriteNum; loopVar++)
	{
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].width, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].height, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].loc_offset, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].location, 1, fp);
		//CVort_engine_cross_freadInt32LE(&engine_maskedSpriteEntry[loopVar].hitbox_ul, 1, fp);
		//CVort_engine_cross_freadInt32LE(&engine_maskedSpriteEntry[loopVar].hitbox_br, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].hitbox_l, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].hitbox_u, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].hitbox_r, 1, fp);
		CVort_engine_cross_freadInt16LE(&engine_maskedSpriteEntry[loopVar].hitbox_b, 1, fp);
		fread(engine_maskedSpriteEntry[loopVar].name, 12, 1, fp);
		CVort_engine_cross_freadInt32LE(&engine_maskedSpriteEntry[loopVar].h_v_off, 1, fp);
		//fread(engine_maskedSpriteEntry[loopVar].copies, 32, 3, fp); // Unused

		// We don't duplicate copies of the same sprite in RAM, though.
		if (loopVar % 4 == 0)
			spriteTotalPixelCount += 8*engine_maskedSpriteEntry[loopVar].width*engine_maskedSpriteEntry[loopVar].height;
	}
	fclose(fp);
	/*************************************************************
	Second part: Load EGALATCH, uncompress if required and then 
	convert to single-byte format (instead of 4-plane structures).
	*************************************************************/
	strcpy(string_buf, "EGALATCH.");
	strcat(string_buf, game_ext);
	// TODO: What if we fail to load the file?
	fp = CVort_engine_cross_ro_data_fopen(string_buf);
	if (!fp)
		return;
	uint8_t *egaLatchData = (uint8_t *)malloc(engine_egaHeadGeneral.latchPlaneSize*4);
	// FIXME: Maybe the compression field is used incorrectly here.
	if (engine_egaHeadGeneral.compression & 2)
		lz_decompress(fp, egaLatchData);
	else
		fread(egaLatchData, engine_egaHeadGeneral.latchPlaneSize*4, 1, fp);
	fclose(fp);
	/********************************************************************
	FIXME? This is a real hack that forces the display of some textual
	messages (like "Decompressing graphics, this may take some time...")
	for a little bit. Otherwise these are simply not seen on sufficiently
	fast machines (even if they can be seen with DOSEMU).

	It further takes a bit(?) longer when uncompression is done.
	********************************************************************/
	CVort_engine_shortSleep();
        if (engine_egaHeadGeneral.compression & 2) {
	        for (int loopVar = 0; loopVar < 7; loopVar++) {
			CVort_engine_shortSleep();
		}
	}
	/*************************************
	"Internal" part: As in Vanilla Keen,
	we set the video mode to be graphical.
	**************************************/
	CVort_engine_setVideoMode(0xD);
	/**************************
	OK back to part 2 itself...
	**************************/

	// Load unmasked bitmaps in our format
	engine_egaBmpData = (uint8_t *)malloc(bmpTotalPixelCount);
	engine_egaBmps = (uint8_t **)malloc(engine_egaHeadGeneral.bmpNum*sizeof(uint8_t *));
	bmpTotalPixelCount = 0;
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.bmpNum; loopVar++)
	{
		engine_egaBmps[loopVar] = engine_egaBmpData+bmpTotalPixelCount;
		bmpTotalPixelCount += 8*engine_egaHeadUnmasked[loopVar].h*engine_egaHeadUnmasked[loopVar].v;
	}
	// Fill with data
	uint32_t currPixelIndex;
	uint8_t *pixelPtr = engine_egaBmpData, *vanillaPtr;
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.bmpNum; loopVar++)
	{
		bmpTotalPixelCount = 0;
		vanillaPtr = egaLatchData+engine_egaHeadGeneral.bmpLoc+engine_egaHeadUnmasked[loopVar].loc;
		for (currPixelIndex = 0; currPixelIndex < 8*engine_egaHeadUnmasked[loopVar].h*engine_egaHeadUnmasked[loopVar].v; currPixelIndex++)
		{
			*pixelPtr = 0;
			if (*(vanillaPtr+bmpTotalPixelCount/8)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 1; // Blue bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 2; // Green bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+2*engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 4; // Red bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+3*engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 8; // Intensity bit
			pixelPtr++;
			bmpTotalPixelCount++;
		}
	}
	// Load fonts in our format
	engine_egaFontData = (uint8_t *)malloc(engine_egaHeadGeneral.fontNum*64);
	engine_egaFonts = (uint8_t **)malloc(engine_egaHeadGeneral.fontNum*sizeof(uint8_t *));
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.fontNum; loopVar++)
		engine_egaFonts[loopVar] = engine_egaFontData+64*loopVar;
	// Fill with data
	bmpTotalPixelCount = 0;
	pixelPtr = engine_egaFontData;
	vanillaPtr = egaLatchData+engine_egaHeadGeneral.fontLoc;
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.fontNum; loopVar++)
		for (currPixelIndex = 0; currPixelIndex < 64; currPixelIndex++)
		{
			*pixelPtr = 0;
			if (*(vanillaPtr+bmpTotalPixelCount/8)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 1; // Blue bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 2; // Green bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+2*engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 4; // Red bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+3*engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 8; // Intensity bit
			pixelPtr++;
			bmpTotalPixelCount++;
		}
	// Load tiles in our format
	engine_egaTileData = (uint8_t *)malloc(engine_egaHeadGeneral.tileNum*256);
	engine_egaTiles = (uint8_t **)malloc(engine_egaHeadGeneral.tileNum*sizeof(uint8_t *));
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.tileNum; loopVar++)
		engine_egaTiles[loopVar] = engine_egaTileData+256*loopVar;
	// Fill with data
	bmpTotalPixelCount = 0;
	pixelPtr = engine_egaTileData;
	vanillaPtr = egaLatchData+engine_egaHeadGeneral.tileLoc;
	for (loopVar = 0; loopVar < engine_egaHeadGeneral.tileNum; loopVar++)
		for (currPixelIndex = 0; currPixelIndex < 256; currPixelIndex++)
		{
			*pixelPtr = 0;
			if (*(vanillaPtr+bmpTotalPixelCount/8)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 1; // Blue bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 2; // Green bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+2*engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 4; // Red bit
			if (*(vanillaPtr+bmpTotalPixelCount/8+3*engine_egaHeadGeneral.latchPlaneSize)&(1<<(bmpTotalPixelCount%8^7)))
				*pixelPtr |= 8; // Intensity bit
			pixelPtr++;
			bmpTotalPixelCount++;
		}
	free(egaLatchData);
	/******************************************************
	Part "2.5": As in Vanilla Keen, we set the border color
	to cyan and draw the "One moment" bitmap on screen.
	******************************************************/
	// TODO: Make these patchable?
	CVort_engine_setBorderColor(3);
	if (engine_gameVersion == GAMEVER_KEEN1) {
		CVort_engine_drawBitmap(0xf, 0x4c, CVort1_bmp_onemomen);
	} else if (engine_gameVersion == GAMEVER_KEEN2) {
		CVort_engine_drawBitmap(0xf, 0x4c, CVort2_bmp_onemomen);
	} else if (engine_gameVersion == GAMEVER_KEEN3) {
		CVort_engine_drawBitmap(0xf, 0x4c, CVort3_bmp_onemomen);
	}
	//engine_updateActualDisplay();
	/*************************************************************
	Third part: Load EGASPRIT, uncompress if required and then 
	convert to single-byte format (instead of 5-plane structures).
	*************************************************************/
	strcpy(string_buf, "EGASPRIT.");
	strcat(string_buf, game_ext);
	// TODO: What if we fail to load the file?
	fp = CVort_engine_cross_ro_data_fopen(string_buf);
	if (!fp)
		return;
	uint8_t *egaSpriteData = (uint8_t *)malloc(engine_egaHeadGeneral.sprPlaneSize*5);
	// FIXME: Maybe the compression field is used incorrectly here.
	if (engine_egaHeadGeneral.compression & 1)
		lz_decompress(fp, egaSpriteData);
	else
		fread(egaSpriteData, engine_egaHeadGeneral.sprPlaneSize*5, 1, fp);
	fclose(fp);
	// Load sprites in our format.
	// Do NOT make 4 copies of each sprite, though.
	engine_egaSpriteData = (uint8_t *)malloc(spriteTotalPixelCount);
	engine_egaSprites = (uint8_t **)malloc(engine_egaHeadGeneral.spriteNum*sizeof(uint8_t *));
	spriteTotalPixelCount = 0;
	for (loopVar = 0; loopVar < 4*engine_egaHeadGeneral.spriteNum; loopVar+=4)
	{
		engine_egaSprites[loopVar/4] = engine_egaSpriteData+spriteTotalPixelCount;
		spriteTotalPixelCount += 8*engine_maskedSpriteEntry[loopVar].width*engine_maskedSpriteEntry[loopVar].height;
	}
	// Fill with data
	// FIXME
	// FIXME
	// FIXME
	// We ignore the "offset" field of EGAHeadSprite_T, but what to do
	// with it anyway...
	pixelPtr = engine_egaSpriteData;
	for (loopVar = 0; loopVar < 4*engine_egaHeadGeneral.spriteNum; loopVar+=4)
	{
		spriteTotalPixelCount = 0;
		vanillaPtr = egaSpriteData+engine_egaHeadGeneral.spriteLoc+engine_maskedSpriteEntry[loopVar].loc_offset+16*engine_maskedSpriteEntry[loopVar].location;
		for (currPixelIndex = 0; currPixelIndex < 8*engine_maskedSpriteEntry[loopVar].width*engine_maskedSpriteEntry[loopVar].height; currPixelIndex++)
		{
			*pixelPtr = 0;
			if (*(vanillaPtr+spriteTotalPixelCount/8)&(1<<(spriteTotalPixelCount%8^7)))
				*pixelPtr |= 1; // Blue bit
			if (*(vanillaPtr+spriteTotalPixelCount/8+engine_egaHeadGeneral.sprPlaneSize)&(1<<(spriteTotalPixelCount%8^7)))
				*pixelPtr |= 2; // Green bit
			if (*(vanillaPtr+spriteTotalPixelCount/8+2*engine_egaHeadGeneral.sprPlaneSize)&(1<<(spriteTotalPixelCount%8^7)))
				*pixelPtr |= 4; // Red bit
			if (*(vanillaPtr+spriteTotalPixelCount/8+3*engine_egaHeadGeneral.sprPlaneSize)&(1<<(spriteTotalPixelCount%8^7)))
				*pixelPtr |= 8; // Intensity bit
			if (*(vanillaPtr+spriteTotalPixelCount/8+4*engine_egaHeadGeneral.sprPlaneSize)&(1<<(spriteTotalPixelCount%8^7)))
				*pixelPtr |= 16; // Mask bit
			pixelPtr++;
			spriteTotalPixelCount++;
		}
	}
	free(egaSpriteData);
}

void privResetEgaMemStartLocAndPanning(void) {
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_SURFACE) {
		if (engine_screen.sdl.secondarySurface) {
			SDL_FreeSurface(engine_screen.sdl.secondarySurface);
		}
		engine_screen.sdl.secondarySurface = SDL_CreateRGBSurfaceFrom(
		    (uint8_t *)engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_screen.client.currEgaStartAddr+engine_screen.client.currPanning),
		    engine_screen.dims.clientScanLineLength,
		    engine_screen.dims.clientRect.h,
		    engine_screen.sdl.windowSurface->format->BitsPerPixel, engine_screen.host.bytesPerPixel*engine_screen.dims.clientScanLineLength,
		    engine_screen.sdl.windowSurface->format->Rmask,
		    engine_screen.sdl.windowSurface->format->Gmask,
		    engine_screen.sdl.windowSurface->format->Bmask,
		    engine_screen.sdl.windowSurface->format->Amask
		);
	}
}

void CVort_private_engine_setEgaMemStartLocAndPanning(uint32_t egaStart, uint16_t panning) {
	engine_screen.client.currEgaStartAddr = egaStart;
	engine_screen.client.currPanning = panning;
	privResetEgaMemStartLocAndPanning();
}


/* Some code that fills rectangles with colors on the host side follows.
 * There are implementations for a few specific color depths, as well as
 * a general implementation for any other color depth.
 * To reduce code duplication, templates are used with the help of macros.
 *
 * Furthermore, 8-bit indexed color handling is added for text mode.
 */

#define CHOCOLATE_KEEN_HOST_VAL_TYPE_indexed8 uint8_t
#define CHOCOLATE_KEEN_HOST_VAL_TYPE_16 uint16_t
#define CHOCOLATE_KEEN_HOST_VAL_TYPE_32 uint32_t
#define CHOCOLATE_KEEN_HOST_VAL_TYPE_other uint8_t

#define CHOCOLATE_KEEN_PREPARE_HOST_EGA_indexed8(offset) uint8_t *hostEgaPtr = (uint8_t *)engine_screen.host.egaMemoryPtr+(offset)
#define CHOCOLATE_KEEN_PREPARE_HOST_EGA_16(offset) uint16_t *hostEgaPtr = (uint16_t *)engine_screen.host.egaMemoryPtr+(offset)
#define CHOCOLATE_KEEN_PREPARE_HOST_EGA_32(offset) uint32_t *hostEgaPtr = (uint32_t *)engine_screen.host.egaMemoryPtr+(offset)
#define CHOCOLATE_KEEN_PREPARE_HOST_EGA_other(offset) uint8_t *hostEgaPtr = (uint8_t *)engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(offset)
#define CHOCOLATE_KEEN_INC_HOST_EGA_indexed8(ptr) ptr++
#define CHOCOLATE_KEEN_INC_HOST_EGA_16(ptr) ptr++
#define CHOCOLATE_KEEN_INC_HOST_EGA_32(ptr) ptr++
#define CHOCOLATE_KEEN_INC_HOST_EGA_other(ptr) ptr+=engine_screen.host.bytesPerPixel
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_indexed8(index,ptr,table) *ptr = ((uint8_t *)(table))[index]
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_16(index,ptr,table) *ptr = ((uint16_t *)(table))[index]
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_32(index,ptr,table) *ptr = ((uint32_t *)(table))[index]
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_other(index,ptr,table) memcpy(ptr, (uint8_t *)(table) + (index) * engine_screen.host.bytesPerPixel, engine_screen.host.bytesPerPixel)
#define CHOCOLATE_KEEN_ADD_HOST_EGA_indexed8(ptr,val) ptr+=(val)
#define CHOCOLATE_KEEN_ADD_HOST_EGA_16(ptr,val) ptr+=(val)
#define CHOCOLATE_KEEN_ADD_HOST_EGA_32(ptr,val) ptr+=(val)
#define CHOCOLATE_KEEN_ADD_HOST_EGA_other(ptr,val) ptr+=(val)*engine_screen.host.bytesPerPixel
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_indexed8(index,ptr,table) ptr = (uint8_t *)(table) + (index)
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_16(index,ptr,table) ptr = (uint16_t *)(table) + (index)
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_32(index,ptr,table) ptr = (uint32_t *)(table) + (index)
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_other(index,ptr,table) ptr = (uint8_t *)(table) + (index) * engine_screen.host.bytesPerPixel
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_indexed8(dst,src) *dst = *(src)
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_16(dst,src) *dst = *(src)
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_32(dst,src) *dst = *(src)
#define CHOCOLATE_KEEN_ASSIGN_HOST_EGA_other(dst,src) memcpy(dst, src, engine_screen.host.bytesPerPixel);

// Used to updated the non-paletted contents of some rectangle
// in EGA memory for graphics mode or launcher UI, based on a
// recent update to the ("real") paletted contents.
#define CVort_engine_updateEgaGfxNonPalRect_TempImpl(T) \
void CVort_engine_updateEgaGfxNonPalRect_##T##bpp (uint32_t offset, uint16_t width, uint16_t height) { \
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+offset; \
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(offset); \
\
	uint16_t currY, currX; \
	for (currY = 0; currY < height; currY++) { \
		for (currX = 0; currX < width; currX++, byteEgaPtr++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_##T(*byteEgaPtr,hostEgaPtr,engine_screen.host.mappedEgaColorTable); \
		} \
		byteEgaPtr += engine_screen.dims.clientScanLineLength-width; \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr,engine_screen.dims.clientScanLineLength-width); \
	} \
}

CVort_engine_updateEgaGfxNonPalRect_TempImpl(16)
CVort_engine_updateEgaGfxNonPalRect_TempImpl(32)
CVort_engine_updateEgaGfxNonPalRect_TempImpl(other)
void (*CVort_engine_updateEgaGfxNonPalRect_ptr) (uint32_t offset, uint16_t width, uint16_t height);
void CVort_engine_updateEgaGfxNonPalRect_doNothing(uint32_t offset, uint16_t width, uint16_t height) {}

// Similar but for text mode. All input values are measured in chars.
#define CVort_engine_updateEgaNonBlinkingTxtNonPalRect_TempImpl(T) \
void CVort_engine_updateEgaNonBlinkingTxtNonPalRect_##T##bpp(uint16_t x, uint16_t y, uint16_t width, uint16_t height) { \
	uint16_t byteOffset = 2*(x+y*ENGINE_EGAVGA_TXT_COLS_NUM); \
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(x*engine_arguments.calc.txtCharPixWidth+y*engine_arguments.calc.txtCharPixHeight*ENGINE_EGAVGA_TXT_COLS_NUM*engine_arguments.calc.txtCharPixWidth); \
\
	uint8_t currChar; \
	const uint8_t *currCharFontPtr; \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T *currBackgroundColorPtr, *currCharColorPtr, *currPixColorPtr, *currScrPixelPtr; \
\
	uint16_t col, row; \
	uint16_t currX, currY; \
\
	for (row = 0; row < height; row++) { \
		for (col = 0; col < width; col++, CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr,engine_arguments.calc.txtCharPixWidth)) { \
			currChar = engine_screen.client.egaTxtMemory[byteOffset]; \
			/* Luckily, the product width*height is always divisible by 8... \
			 * Note that the actual width is always 8, \
			 * even in VGA mode. We convert to 9 while drawing. \
			 */ \
			currCharFontPtr = engine_arguments.calc.txtFontPtr + currChar*8*engine_arguments.calc.txtCharPixHeight; \
			byteOffset++; \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_##T((engine_screen.client.egaTxtMemory[byteOffset] >> 4) & 7, currBackgroundColorPtr, engine_screen.host.colorTable); \
			/* Don't draw blinking character for now. \
			 * engine_updateActualDisplay takes care of that. \
			 */ \
			if (engine_screen.client.egaTxtMemory[byteOffset] & 0x80) { \
				byteOffset++; \
				continue; \
			} \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_##T(engine_screen.client.egaTxtMemory[byteOffset] & 15, currCharColorPtr, engine_screen.host.colorTable); \
			byteOffset++; \
			currScrPixelPtr = hostEgaPtr; \
			for (currY = 0; currY < engine_arguments.calc.txtCharPixHeight; currY++) { \
				/* NOTE: The char width is actually \
				 * 8 for all fonts. In the VGA case, \
				 * the 9th pixel is determined according \
				 * to the 8th and char number. \
				 */ \
				for (currX = 0; currX < 8; currX++, currCharFontPtr++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(currScrPixelPtr)) { \
					if (*currCharFontPtr) { \
						currPixColorPtr = currCharColorPtr; \
					} else { \
						currPixColorPtr = currBackgroundColorPtr; \
					} \
					/* Store pixel */ \
					CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(currScrPixelPtr, currPixColorPtr); \
				} \
				/* Add an extra 9th column on VGA */ \
				if (engine_arguments.isEmulatedGfxCardVga) { \
					/* If the condition applies, the next \
					 * pixel's color is the last's. \
					 * Otherwise it has background color. \
					 */ \
					CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(currScrPixelPtr, ((currChar >= 192) && (currChar <= 223)) ? currPixColorPtr : currBackgroundColorPtr); \
					CHOCOLATE_KEEN_INC_HOST_EGA_##T(currScrPixelPtr); \
				} \
				CHOCOLATE_KEEN_ADD_HOST_EGA_##T(currScrPixelPtr, engine_screen.dims.clientRect.w-engine_arguments.calc.txtCharPixWidth); \
			} \
		} \
		/* Skip to the following row of characters */ \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, -engine_arguments.calc.txtCharPixWidth*width); \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, engine_screen.dims.clientRect.w*engine_arguments.calc.txtCharPixHeight); \
		byteOffset -= 2*width; \
		byteOffset += 2*ENGINE_EGAVGA_TXT_COLS_NUM; \
	} \
}

CVort_engine_updateEgaNonBlinkingTxtNonPalRect_TempImpl(indexed8)
CVort_engine_updateEgaNonBlinkingTxtNonPalRect_TempImpl(16)
CVort_engine_updateEgaNonBlinkingTxtNonPalRect_TempImpl(32)
CVort_engine_updateEgaNonBlinkingTxtNonPalRect_TempImpl(other)
void (*CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr) (uint16_t x, uint16_t y, uint16_t width, uint16_t height);

// Updates a single, blinking character
#define CVort_engine_updateEgaBlinkingTxtNonPalChar_TempImpl(T) \
void CVort_engine_updateEgaBlinkingTxtNonPalChar_##T##bpp(uint16_t x, uint16_t y, bool isShown) { \
	uint16_t byteOffset = 2*(x+y*ENGINE_EGAVGA_TXT_COLS_NUM); \
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(x*engine_arguments.calc.txtCharPixWidth+y*engine_arguments.calc.txtCharPixHeight*ENGINE_EGAVGA_TXT_COLS_NUM*engine_arguments.calc.txtCharPixWidth); \
\
	uint8_t currChar; \
	const uint8_t *currCharFontPtr; \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T *currBackgroundColorPtr, *currCharColorPtr, *currPixColorPtr, *currScrPixelPtr; \
\
	uint16_t col, row; \
	uint16_t currX, currY; \
\
	currChar = engine_screen.client.egaTxtMemory[byteOffset]; \
	/* Luckily, the product width*height is always divisible by 8... \
	 * Note that the actual width is always 8, \
	 * even in VGA mode. We convert to 9 while drawing. \
	 */ \
	currCharFontPtr = engine_arguments.calc.txtFontPtr + currChar*8*engine_arguments.calc.txtCharPixHeight; \
	byteOffset++; \
	CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_##T((engine_screen.client.egaTxtMemory[byteOffset] >> 4) & 7, currBackgroundColorPtr, engine_screen.host.colorTable); \
	/* Should the character be displayed at the moment (based on blinking)? */ \
	if (isShown) { \
		CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_##T(engine_screen.client.egaTxtMemory[byteOffset] & 15, currCharColorPtr, engine_screen.host.colorTable); \
	} else { \
		currCharColorPtr = currBackgroundColorPtr; \
	} \
\
	currScrPixelPtr = hostEgaPtr; \
	for (currY = 0; currY < engine_arguments.calc.txtCharPixHeight; currY++) { \
		/* NOTE: The char width is actually \
		8 for all fonts. In the VGA case, \
		the 9th pixel is determined according \
		to the 8th and char number.        */ \
		for (currX = 0; currX < 8; currX++, currCharFontPtr++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(currScrPixelPtr)) { \
			if (*currCharFontPtr) { \
				currPixColorPtr = currCharColorPtr; \
			} else { \
				currPixColorPtr = currBackgroundColorPtr; \
			} \
			/* Store pixel */ \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(currScrPixelPtr, currPixColorPtr); \
		} \
		/* Add an extra 9th column on VGA */ \
		if (engine_arguments.isEmulatedGfxCardVga) { \
			if ((currChar < 192) || (currChar > 223)) { \
				currPixColorPtr = currBackgroundColorPtr; \
			} \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(currScrPixelPtr, currPixColorPtr); \
			CHOCOLATE_KEEN_INC_HOST_EGA_##T(currScrPixelPtr); \
		} \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(currScrPixelPtr, engine_screen.dims.clientRect.w-engine_arguments.calc.txtCharPixWidth); \
	} \
}

CVort_engine_updateEgaBlinkingTxtNonPalChar_TempImpl(indexed8)
CVort_engine_updateEgaBlinkingTxtNonPalChar_TempImpl(16)
CVort_engine_updateEgaBlinkingTxtNonPalChar_TempImpl(32)
CVort_engine_updateEgaBlinkingTxtNonPalChar_TempImpl(other)
void (*CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr) (uint16_t x, uint16_t y, bool isShown);

// Draws the text mode cursor at its current location
#define CVort_engine_doDrawEgaTxtCursor_TempImpl(T) \
void CVort_engine_doDrawEgaTxtCursor_##T##bpp(void) { \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T *currCharColorPtr; \
	CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_##T(engine_screen.client.egaTxtMemory[1+((ENGINE_EGAVGA_TXT_COLS_NUM*engine_screen.client.txtCursorPosY+engine_screen.client.txtCursorPosX)<<1)] & 15, currCharColorPtr, engine_screen.host.colorTable); \
\
\
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(engine_screen.client.currEgaStartAddr + engine_screen.client.currPanning \
	                                    + engine_screen.client.txtCursorPosY*engine_arguments.calc.txtCharPixHeight*engine_screen.dims.clientRect.w \
	                                    + engine_screen.client.txtCursorPosX*engine_arguments.calc.txtCharPixWidth \
	                                    /* Out of 3 last scanlines of char, draw to the first 2. */ \
	                                    + (engine_arguments.calc.txtCharPixHeight-3)*engine_screen.dims.clientRect.w \
	); \
	for (uint16_t currY = 0, currX; currY < 2; currY++) { \
		for (currX = 0; currX < engine_arguments.calc.txtCharPixWidth; currX++, hostEgaPtr++) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, currCharColorPtr); \
		} \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, engine_screen.dims.clientRect.w-engine_arguments.calc.txtCharPixWidth); \
	} \
}

CVort_engine_doDrawEgaTxtCursor_TempImpl(indexed8)
CVort_engine_doDrawEgaTxtCursor_TempImpl(16)
CVort_engine_doDrawEgaTxtCursor_TempImpl(32)
CVort_engine_doDrawEgaTxtCursor_TempImpl(other)
void (*CVort_engine_doDrawEgaTxtCursor_ptr) (void);

// Used by updateActualDisplay: Updates a zoomed buffer of contents,
// *skipping* borders and possibly more.
// Here, the buffer *may* contain more than the bordered drawing (longer scanline).
#define CVort_engine_updateBorderedZoomedRectBuffer_TempImpl(T) \
void CVort_engine_updateBorderedZoomedRectBuffer_##T##bpp(uint8_t *buffer, uint32_t egaOffset, uint32_t borderLineOffset) { \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T *screenPixelPtr; \
	CHOCOLATE_KEEN_ASSIGN_HOST_EGA_PTR_FROM_TABLE_##T(egaOffset, screenPixelPtr, buffer); \
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(engine_screen.client.currEgaStartAddr+engine_screen.client.currPanning); \
	if (engine_screen.dims.zoomFactor > 1) { \
		uint32_t numOfZoomedPixelsPerLine = (engine_screen.dims.clientRect.w+borderLineOffset)*engine_screen.dims.zoomFactor; \
		for (uint16_t currY = 0, currX, copyIndex; currY < engine_screen.dims.clientRect.h; currY++) { \
			/* Draw one zoomed line */ \
			for (currX = 0; currX < engine_screen.dims.clientRect.w; currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) \
				for (copyIndex = 0; copyIndex < engine_screen.dims.zoomFactor; copyIndex++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(screenPixelPtr)) \
					CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(screenPixelPtr, hostEgaPtr); \
			CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, engine_screen.dims.clientScanLineLength-engine_screen.dims.clientRect.w); \
			CHOCOLATE_KEEN_ADD_HOST_EGA_##T(screenPixelPtr, engine_screen.dims.zoomFactor*borderLineOffset); \
			/* Now repeat the same line for vertical zooming */ \
			for (copyIndex = 1; copyIndex < engine_screen.dims.zoomFactor; copyIndex++, CHOCOLATE_KEEN_ADD_HOST_EGA_##T(screenPixelPtr, numOfZoomedPixelsPerLine)) \
				memcpy(screenPixelPtr, (uint8_t *)screenPixelPtr-engine_screen.host.bytesPerPixel*numOfZoomedPixelsPerLine, engine_screen.host.bytesPerPixel*engine_screen.dims.zoomFactor*engine_screen.dims.clientRect.w); \
		} \
	} else { \
		uint32_t numOfPixelsPerLine = engine_screen.dims.clientRect.w+borderLineOffset; \
		for (uint16_t currY = 0; currY < engine_screen.dims.clientRect.h; \
		     currY++, CHOCOLATE_KEEN_ADD_HOST_EGA_##T(screenPixelPtr, numOfPixelsPerLine), CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, engine_screen.dims.clientScanLineLength)) { \
			memcpy(screenPixelPtr, hostEgaPtr, engine_screen.host.bytesPerPixel*engine_screen.dims.clientRect.w); \
		} \
	} \
}

CVort_engine_updateBorderedZoomedRectBuffer_TempImpl(16)
CVort_engine_updateBorderedZoomedRectBuffer_TempImpl(32)
CVort_engine_updateBorderedZoomedRectBuffer_TempImpl(other)
void (*CVort_engine_updateBorderedZoomedRectBuffer_ptr) (uint8_t *buffer, uint32_t egaOffset, uint32_t borderLineOffset);

// Used by setBorderColor: Updates overscan color in zoomed buffer of contents.
// Here, the buffer should *not* contain more than the bordered drawing.
#define CVort_engine_updateBorderedZoomedRectBufferBorder_TempImpl(T) \
void CVort_engine_updateBorderedZoomedRectBufferBorder_##T##bpp(uint8_t *buffer) { \
	/* FIXME: This can be more efficient!!! */ \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T *screenPixelPtr = (CHOCOLATE_KEEN_HOST_VAL_TYPE_##T *)buffer; \
	for (uint16_t currY = 0, currX; currY < engine_screen.dims.clientZoomedBorderedHeight; currY++) { \
		for (currX = 0; currX < engine_screen.dims.clientZoomedBorderedWidth; currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(screenPixelPtr))  {\
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_##T(engine_screen.client.currParsedBorderColor, screenPixelPtr, engine_screen.host.colorTable); \
		} \
	} \
}

CVort_engine_updateBorderedZoomedRectBufferBorder_TempImpl(16)
CVort_engine_updateBorderedZoomedRectBufferBorder_TempImpl(32)
CVort_engine_updateBorderedZoomedRectBufferBorder_TempImpl(other)
void (*CVort_engine_updateBorderedZoomedRectBufferBorder_ptr) (uint8_t *buffer);

// Templates for launcher UI, always drawing to hostEgaPtr, paletted or not

#define CVort_engine_gui_drawFontChar_TempImpl(T) \
void CVort_engine_gui_drawFontChar_##T##bpp(int x, int y, int w, int h, const uint8_t *fontCharPtr, int colorNum, int ratio) { \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T color; \
	CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_##T(colorNum,&color,engine_screen.host.colorTable); \
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(x + y*ENGINE_GUI_WIDTH); \
	const uint8_t *currRowFontCharPtr = fontCharPtr; \
	int copyIndexX, copyIndexY; \
	for (int currY = 0, currX; currY < h; currY++) { \
		for (copyIndexY = 0; copyIndexY < ratio; copyIndexY++) { \
			for (currX = 0; currX < w; currX++, fontCharPtr++) { \
				for (copyIndexX = 0; copyIndexX < ratio; copyIndexX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
					if (*fontCharPtr) { \
						CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &color); \
					} \
				} \
			} \
			CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, ENGINE_GUI_WIDTH-ratio*w); \
			fontCharPtr = currRowFontCharPtr; \
		} \
		fontCharPtr += w; \
		currRowFontCharPtr += w; \
	} \
}

CVort_engine_gui_drawFontChar_TempImpl(indexed8)
CVort_engine_gui_drawFontChar_TempImpl(16)
CVort_engine_gui_drawFontChar_TempImpl(32)
CVort_engine_gui_drawFontChar_TempImpl(other)
void (*CVort_engine_gui_drawFontChar_ptr) (int x, int y, int w, int h, const uint8_t *fontCharPtr, int colorNum, int ratio);

#define CVort_engine_gui_drawRoundedRectBorder_TempImpl(T) \
void CVort_engine_gui_drawRoundedRectBorder_##T##bpp(int x, int y, int w, int h, int borderColorNum, int innerColorNum, int ratio) { \
	CHOCOLATE_KEEN_HOST_VAL_TYPE_##T borderColor, innerColor; \
	CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_##T(borderColorNum,&borderColor,engine_screen.host.colorTable); \
	CHOCOLATE_KEEN_ASSIGN_HOST_EGA_FROM_TABLE_##T(innerColorNum,&innerColor,engine_screen.host.colorTable); \
	CHOCOLATE_KEEN_PREPARE_HOST_EGA_##T(x + y*ENGINE_GUI_WIDTH); \
	int copyIndex, currY, currX; \
	/* Draw top border's first row, an almost filled line (ex. corners) */ \
	CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr); \
	for (currX = 1; currX < ratio*w - 1; currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
		CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &borderColor); \
	} \
	/* Draw the rest of the top border's row as full lines */ \
	CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, ENGINE_GUI_WIDTH-ratio*w+1); \
	for (copyIndex = 1; copyIndex < ratio; copyIndex++) { \
		for (currX = 0; currX < ratio*w; currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &borderColor); \
		} \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, ENGINE_GUI_WIDTH-ratio*w); \
	} \
	/* Draw most of the left and right borders (excluding top+bottom),
	 * plus the internal portion.
	 */ \
	for (currY = ratio; currY < ratio*(h-1); currY++) { \
		/* Left */ \
		for (copyIndex = 0; copyIndex < ratio; copyIndex++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &borderColor); \
		} \
		/* Inner */ \
		for (currX = 0; currX < ratio*(w-2); currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &innerColor); \
		} \
		/* Right */ \
		for (copyIndex = 0; copyIndex < ratio; copyIndex++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &borderColor); \
		} \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, ENGINE_GUI_WIDTH-ratio*w); \
	} \
	/* Draw bottom border with full lines, but without the very last line */ \
	for (copyIndex = 1; copyIndex < ratio; copyIndex++) { \
		for (currX = 0; currX < ratio*w; currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
			CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &borderColor); \
		} \
		CHOCOLATE_KEEN_ADD_HOST_EGA_##T(hostEgaPtr, ENGINE_GUI_WIDTH-ratio*w); \
	} \
	/* Last line left without corners */ \
	CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr); \
	for (currX = 1; currX < ratio*w - 1; currX++, CHOCOLATE_KEEN_INC_HOST_EGA_##T(hostEgaPtr)) { \
		CHOCOLATE_KEEN_ASSIGN_HOST_EGA_##T(hostEgaPtr, &borderColor); \
	} \
}

CVort_engine_gui_drawRoundedRectBorder_TempImpl(indexed8)
CVort_engine_gui_drawRoundedRectBorder_TempImpl(16)
CVort_engine_gui_drawRoundedRectBorder_TempImpl(32)
CVort_engine_gui_drawRoundedRectBorder_TempImpl(other)
void (*CVort_engine_gui_drawRoundedRectBorder_ptr) (int x, int y, int w, int h, int borderColorNum, int innerColorNum, int ratio);

void CVort_engine_gui_clearScreen(void) {
	memset(engine_screen.client.byteEgaMemory, 0, sizeof(engine_screen.client.byteEgaMemory));
	// Convert to non-zero just in case (say a YUV format)
	CVort_engine_updateEgaGfxNonPalRect_ptr(0, engine_screen.dims.clientRect.w, engine_screen.dims.clientRect.h);
}

void CVort_engine_gui_drawColoredLine(int lineNum, int lineLength, int colorNum) {
	memset(engine_screen.client.byteEgaMemory + lineNum*engine_screen.dims.clientRect.w, colorNum, lineLength);
	CVort_engine_updateEgaGfxNonPalRect_ptr(lineNum*engine_screen.dims.clientRect.w, lineLength, 1);
}

void CVort_engine_gui_drawColoredColumn(int columnNum, int columnLength, int colorNum) {
	uint8_t *pixelPtr = engine_screen.client.byteEgaMemory + columnNum;
	for (int i = 0; i < columnLength; i++, pixelPtr+=engine_screen.dims.clientRect.w) {
		*pixelPtr = colorNum;
	}
	CVort_engine_updateEgaGfxNonPalRect_ptr(columnNum, 1, columnLength);
}

void CVort_engine_updateActualDisplay(void) {
	// Text mode handling
	if (engine_screen.client.currVidMode == 3) {
		static bool haveBlinkingCharsShownLast = true, hasCursorShownLast = true;
		bool areBlinkingCharsShown = (((Uint64)(engine_arguments.calc.scaledRefreshRate*SDL_GetTicks()/ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR)/(1000*engine_arguments.calc.txtBlinkRate)) % 2),
		     isCursorShown = (((Uint64)(engine_arguments.calc.scaledRefreshRate*SDL_GetTicks()/ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR)/(1000*engine_arguments.calc.txtCursorBlinkRate)) % 2);
		// Possibly refresh blinking characters
		if (engine_isFrameReadyToDisplay || (haveBlinkingCharsShownLast != areBlinkingCharsShown)) {
			uint16_t txtByteCounter = 1;
			engine_isFrameReadyToDisplay = true;
			for (uint16_t row = 0, col; row < ENGINE_EGAVGA_TXT_ROWS_NUM; row++) {
				for (col = 0; col < ENGINE_EGAVGA_TXT_COLS_NUM; col++, txtByteCounter += 2) {
					if (engine_screen.client.egaTxtMemory[txtByteCounter] & 0x80) {
						CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr(col, row, areBlinkingCharsShown);
					}
				}
			}
		}
		// Update cursor status if required
		if (engine_isFrameReadyToDisplay || (hasCursorShownLast != isCursorShown)) {
			engine_isFrameReadyToDisplay = true;
			if (isCursorShown) {
				CVort_engine_doDrawEgaTxtCursor_ptr();
			} else { // Simply re-draw the whole char (if not blinking)
				CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr(engine_screen.client.txtCursorPosX, engine_screen.client.txtCursorPosY, 1, 1);
			}
		}
		// Do NOT forget these!
		haveBlinkingCharsShownLast = areBlinkingCharsShown;
		hasCursorShownLast = isCursorShown;
		if (!engine_isFrameReadyToDisplay) {
			return; /* Decrease CPU/GPU usage */
		}
	}
	// Draw
	switch (engine_gfx_effective_arguments.outputSystem) {
#if SDL_VERSION_ATLEAST(2,0,0)
		case OUTPUTSYS_TEXTURE:
		{
			SDL_RenderClear(engine_screen.sdl.renderer);
			if (engine_screen.sdl.renderTexture) {
				SDL_UpdateTexture(engine_screen.sdl.texture, NULL,
				                  engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_screen.client.currEgaStartAddr+engine_screen.client.currPanning),
				                  engine_screen.host.texPitch
				);
				SDL_SetRenderTarget(engine_screen.sdl.renderer, engine_screen.sdl.renderTexture);
				SDL_RenderCopy(engine_screen.sdl.renderer,
				               engine_screen.sdl.texture,
				               &engine_screen.dims.clientRect,
				               &engine_screen.dims.clientOffsettedZoomedRect
				);
				SDL_SetRenderTarget(engine_screen.sdl.renderer, NULL);
				SDL_RenderCopy(engine_screen.sdl.renderer,
				               engine_screen.sdl.renderTexture,
				               NULL,
				               &engine_screen.dims.borderedViewportRect
				);
				SDL_RenderPresent(engine_screen.sdl.renderer);
				break;
			}
			if (engine_screen.host.bilinearInterpolation) {
				void *buffer;
				int pitch;
				SDL_LockTexture(engine_screen.sdl.texture, NULL, &buffer, &pitch);
				CVort_engine_updateBorderedZoomedRectBuffer_ptr((uint8_t *)buffer, engine_screen.dims.clientOffsettedZoomedRect.x+engine_screen.dims.clientOffsettedZoomedRect.y*engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientBorderedWidth-engine_screen.dims.clientRect.w);
				SDL_UnlockTexture(engine_screen.sdl.texture);
				// This also copies and scalers the border
				SDL_RenderCopy(engine_screen.sdl.renderer,
				               engine_screen.sdl.texture,
				               NULL,
				               &engine_screen.dims.borderedViewportRect
				);
			} else {
				// (Re)set overscan border color separately
				SDL_SetRenderDrawColor(engine_screen.sdl.renderer,
				                       engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 16,
				                       (engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 8) & 0xFF,
				                       engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] & 0xFF,
				                       0xFF);
				SDL_RenderFillRect(engine_screen.sdl.renderer,
				                   &engine_screen.dims.borderedViewportRect
				);
				SDL_UpdateTexture(engine_screen.sdl.texture, NULL,
				                  engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_screen.client.currEgaStartAddr+engine_screen.client.currPanning),
				                  engine_screen.host.texPitch
				);
				// Notice this does NOT contain the border now
				SDL_RenderCopy(engine_screen.sdl.renderer,
				               engine_screen.sdl.texture,
				               &engine_screen.dims.clientRect,
				               &engine_screen.dims.viewportRect
				);
				// Don't forget this!!
				SDL_SetRenderDrawColor(engine_screen.sdl.renderer, 0, 0, 0, 0xFF);
			}
			SDL_RenderPresent(engine_screen.sdl.renderer);
			break;
		}
#else
		case OUTPUTSYS_OVERLAY:
		{
			SDL_LockYUVOverlay(engine_screen.sdl.overlay);
			CVort_engine_updateBorderedZoomedRectBuffer_32bpp(*engine_screen.sdl.overlay->pixels, engine_screen.dims.clientOffsettedZoomedRect.x+engine_screen.dims.clientOffsettedZoomedRect.y*engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientBorderedWidth-engine_screen.dims.clientRect.w);
			SDL_UnlockYUVOverlay(engine_screen.sdl.overlay);
			SDL_DisplayYUVOverlay(engine_screen.sdl.overlay, &engine_screen.dims.borderedViewportRect);
			break;
		}
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
		case OUTPUTSYS_OPENGL:
		{
			// THIS SHOULD BE STATIC!!!
			// Or else, expect troubles...
			static GLushort triangleIndices[] = {0, 1, 2, 1, 2, 3};
			if (engine_screen.host.bilinearInterpolation && engine_screen.gl.offScreenRendering) {
				// Here, the texture's width already takes
				// the whole scanline into consideration.
				// Furthermore, there are NO overscan borders.
				engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[0]);
				engine_glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
				                       engine_screen.dims.clientScanLineLength,
				                       engine_screen.dims.clientRect.h,
				                       engine_screen.gl.texturePixFormat, GL_UNSIGNED_BYTE,
				                       (uint8_t *)engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_screen.client.currEgaStartAddr+engine_screen.client.currPanning)
				);
				/* First we render to secondary texture */
				engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, engine_screen.gl.frameBufferName);
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
				if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
					// No need to explicitly call glViewport;
					// The call list takes care of that.
					engine_glCallList(engine_screen.gl.outputCallListName);
				}
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
				if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
					engine_glViewport(0, 0, engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientZoomedBorderedHeight);
					// NOTE: This should invert the image here
					engine_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, triangleIndices);
					engine_glViewport(engine_screen.dims.borderedViewportRect.x,
					                  engine_screen.dims.borderedViewportRect.y,
					                  engine_screen.dims.borderedViewportRect.w,
					                  engine_screen.dims.borderedViewportRect.h
					);
				}
#endif
				if (engine_screen.gl.haveFramebufferBlit) {
					/* Now render to screen */
					/*engine_glBindFramebuffer_CoreEXT(ENGINE_GL_READ_FRAMEBUFFER_COREEXT, engine_screen.gl.frameBufferName);*/
					engine_glBindFramebuffer_CoreEXT(ENGINE_GL_DRAW_FRAMEBUFFER_COREEXT, 0);
					engine_glClear(GL_COLOR_BUFFER_BIT);
					engine_glBlitFramebufferEXT(
						0, 0,
						engine_screen.dims.clientZoomedBorderedWidth,
						engine_screen.dims.clientZoomedBorderedHeight,
						engine_screen.dims.borderedViewportRect.x,
						engine_screen.dims.borderedViewportRect.y,
						engine_screen.dims.borderedViewportRect.x+engine_screen.dims.borderedViewportRect.w,
						engine_screen.dims.borderedViewportRect.y+engine_screen.dims.borderedViewportRect.h,
						GL_COLOR_BUFFER_BIT,
						GL_LINEAR
					);
					engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, 0);
				} else {
					/* Now render to screen */
					engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, 0);
					engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[1]);
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
					if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
						// No need to explicitly call glClear;
						// The call list takes care of that.
						engine_glCallList(1+engine_screen.gl.outputCallListName);
					}
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
					if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
						// Clear window
						engine_glClear(GL_COLOR_BUFFER_BIT);
						// Temporarily change buffer
						engine_glVertexAttribPointer(
						    engine_screen.gl.positionLoc, 3, GL_FLOAT, 
						    GL_FALSE, 5*sizeof(GLfloat), engine_screen.gl.secondaryPositionsBuffer
						);
						engine_glVertexAttribPointer(
						    engine_screen.gl.texCoordLoc, 2, GL_FLOAT,
						    GL_FALSE, 5*sizeof(GLfloat), &engine_screen.gl.secondaryPositionsBuffer[3]
						);
						// Draw, while temporarily changing fragment shader behaviors if required
						if (engine_screen.gl.gpuPaletteCycling)
							engine_glUniform1i(engine_screen.gl.palCyclingIntLoc, 0);
						engine_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, triangleIndices);
						if (engine_screen.gl.gpuPaletteCycling)
							engine_glUniform1i(engine_screen.gl.palCyclingIntLoc, 1);
						// Then restore
						engine_glVertexAttribPointer(
						    engine_screen.gl.positionLoc, 3, GL_FLOAT, 
						    GL_FALSE, 5*sizeof(GLfloat), engine_screen.gl.positionsBuffer
						);
						engine_glVertexAttribPointer(
						    engine_screen.gl.texCoordLoc, 2, GL_FLOAT,
						    GL_FALSE, 5*sizeof(GLfloat), &engine_screen.gl.positionsBuffer[3]
						);
					}
#endif
				}
#if SDL_VERSION_ATLEAST(2,0,0)
				SDL_GL_SwapWindow(engine_screen.sdl.window);
#else
				SDL_GL_SwapBuffers();
#endif
				break;
			}
#if 0
			if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
				engine_glActiveTexture(GL_TEXTURE0);
			}
#endif
			engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[0]);
			if (engine_screen.host.bilinearInterpolation) {
				// No GPU palette cycling is done here,
				// so it's always 32-bit for now.
				CVort_engine_updateBorderedZoomedRectBuffer_32bpp((uint8_t *)engine_screen.sdl.secondarySurface->pixels, 0, 0);
				engine_glTexSubImage2D(GL_TEXTURE_2D, 0,
				                       engine_screen.dims.clientOffsettedZoomedRect.x,
				                       engine_screen.dims.clientOffsettedZoomedRect.y,
				                       engine_screen.dims.clientOffsettedZoomedRect.w,
				                       engine_screen.dims.clientOffsettedZoomedRect.h,
				                       GL_RGBA/*engine_screen.gl.texturePixFormat*/, GL_UNSIGNED_BYTE,
				                       engine_screen.sdl.secondarySurface->pixels
				);
			} else {
				// First we clear the viewport ON OUR OWN,
				// i.e. not with the help of the display list.
				engine_glClear(GL_COLOR_BUFFER_BIT);
				// Next we reset overscan border color
				engine_glEnable(GL_SCISSOR_TEST);
				engine_glClearColor(((GLclampf)(engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 16)) / 255.0f,
				                    ((GLclampf)((engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 8) & 0xFF)) / 255.0f,
				                    ((GLclampf)(engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] & 0xFF)) / 255.0f,
				                    1.0f
				);
				engine_glClear(GL_COLOR_BUFFER_BIT);
				engine_glDisable(GL_SCISSOR_TEST);
				// Don't forget this!!!
				engine_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				// We can now blit the texture to the viewport,
				// ignoring the overscan borders.
				engine_glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
				                       engine_screen.dims.clientScanLineLength,
				                       engine_screen.dims.clientRect.h,
				                       engine_screen.gl.texturePixFormat, GL_UNSIGNED_BYTE,
				                       (uint8_t *)engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_screen.client.currEgaStartAddr+engine_screen.client.currPanning)
				);
			}
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
			if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
				// No need to explicitly call glClear;
				// The call list takes care of that.
				engine_glCallList(engine_screen.gl.outputCallListName);
			}
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
			if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
				// Do NOT clear otherwise, since we need to apply TWO clears
				// (one for the whole viewport and the other for the overscan border.)
				if (engine_screen.host.bilinearInterpolation) {
					engine_glClear(GL_COLOR_BUFFER_BIT);
				}
				engine_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, triangleIndices);
			}
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
			SDL_GL_SwapWindow(engine_screen.sdl.window);
#else
			SDL_GL_SwapBuffers();
#endif
			break;
		}
#endif
		case OUTPUTSYS_SURFACE:
			if ((engine_screen.dims.clientRect.w != engine_screen.dims.viewportRect.w)
			    || (engine_screen.dims.clientRect.h != engine_screen.dims.viewportRect.h)) {
				SDL_SoftStretch(engine_screen.sdl.secondarySurface,
				                &engine_screen.dims.clientRect,
				                engine_screen.sdl.windowSurface,
				                &engine_screen.dims.viewportRect);
			} else {
				SDL_BlitSurface(engine_screen.sdl.secondarySurface,
				                &engine_screen.dims.clientRect,
				                engine_screen.sdl.windowSurface,
				                &engine_screen.dims.viewportRect);
			}
#if SDL_VERSION_ATLEAST(2,0,0)
			SDL_UpdateWindowSurface(engine_screen.sdl.window);
#else
			SDL_Flip(engine_screen.sdl.windowSurface);
#endif
			break;
	}
	engine_isFrameReadyToDisplay = false;
}

void CVort_engine_setWindowTitleAndIcon() {
	static SDL_Surface *iconSurface = NULL;
	if (!iconSurface) {
		iconSurface = SDL_CreateRGBSurfaceFrom(
		                           ChocolateKeen_Icon, 32, 32, 8, 32,
		                           0, 0, 0, 0);
#if SDL_VERSION_ATLEAST(2,0,0)
		SDL_SetPaletteColors(iconSurface->format->palette, ChocolateKeen_Palette, CHOCOLATE_KEEN_PALETTE_START, CHOCOLATE_KEEN_PALETTE_SIZE);
		SDL_SetColorKey(iconSurface, SDL_TRUE, SDL_MapRGB(iconSurface->format, 0xCC, 0xFF, 0xCC));
#else
		SDL_SetPalette(iconSurface, SDL_LOGPAL, ChocolateKeen_Palette, CHOCOLATE_KEEN_PALETTE_START, CHOCOLATE_KEEN_PALETTE_SIZE);
		SDL_SetColorKey(iconSurface, SDL_SRCCOLORKEY, SDL_MapRGB(iconSurface->format, 0xCC, 0xFF, 0xCC));
#endif
	}
#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_SetWindowTitle(engine_screen.sdl.window, "Chocolate Keen");
	SDL_SetWindowIcon(engine_screen.sdl.window, iconSurface);
#else
	SDL_WM_SetCaption("Chocolate Keen", NULL);
	SDL_WM_SetIcon(iconSurface, NULL);
#endif
}

void CVort_engine_prepareWindowRects(bool doBoxing) {
#if SDL_VERSION_ATLEAST(2,0,0)
	int windowWidth, windowHeight;
	SDL_GetWindowSize(engine_screen.sdl.window, &windowWidth, &windowHeight);
#else
	int windowWidth = engine_screen.sdl.windowSurface->w, windowHeight = engine_screen.sdl.windowSurface->h;
#endif
	// REMINDER: borderedRatioHoriz, borderedRatioVert are both int64_t

	// There is no need to aspect correct or prepare a box if the
	// window is not a full screen - it has already been corrected!
	// Same if -forcecutfullscreen is specified for a full screen.
	//
	// EXCEPTION: Window resize

	if (doBoxing) {
		if (engine_gfx_effective_arguments.scaleType == GFX_SCALE_ASPECT) {
			/* If we want the area WITHOUT THE OVERSCAN BORDERS
			 * to have the aspect ratio of 4:3, we should do the
			 * correct calculations. Taking the screen contents in
			 * square pixels, the height should be multipled by 6/5.
			 * Hence we use borderedRatioHoriz and borderedRatioVert.
			 */
			if (engine_screen.dims.borderedRatioVert*windowWidth >= engine_screen.dims.borderedRatioHoriz*windowHeight) { // Wider than 4:3
				engine_screen.dims.borderedViewportRect.w = engine_screen.dims.borderedRatioHoriz*windowHeight/engine_screen.dims.borderedRatioVert;
				engine_screen.dims.borderedViewportRect.h = windowHeight;
				engine_screen.dims.borderedViewportRect.x = (windowWidth-engine_screen.dims.borderedViewportRect.w)/2;
				engine_screen.dims.borderedViewportRect.y = 0;
			} else { // Thinner than 4:3
				engine_screen.dims.borderedViewportRect.w = windowWidth;
				engine_screen.dims.borderedViewportRect.h = engine_screen.dims.borderedRatioVert*windowWidth/engine_screen.dims.borderedRatioHoriz;
				engine_screen.dims.borderedViewportRect.x = 0;
				engine_screen.dims.borderedViewportRect.y = (windowHeight-engine_screen.dims.borderedViewportRect.h)/2;
			}
		} else { // Fullscreen but GFX_SCALE_BOXED
			if (engine_screen.dims.clientZoomedBorderedWidth <= windowWidth) {
				engine_screen.dims.borderedViewportRect.w = engine_screen.dims.clientZoomedBorderedWidth;
				engine_screen.dims.borderedViewportRect.x = (windowWidth-engine_screen.dims.clientZoomedBorderedWidth)/2;
			} else {
				engine_screen.dims.borderedViewportRect.w = windowWidth;
				engine_screen.dims.borderedViewportRect.x = 0;
			}
			if (engine_screen.dims.clientZoomedBorderedHeight <= windowHeight) {
				engine_screen.dims.borderedViewportRect.h = engine_screen.dims.clientZoomedBorderedHeight;
				engine_screen.dims.borderedViewportRect.y = (windowHeight-engine_screen.dims.clientZoomedBorderedHeight)/2;
			} else {
				engine_screen.dims.borderedViewportRect.h = windowHeight;
				engine_screen.dims.borderedViewportRect.y = 0;
			}
		}
	} else {
		engine_screen.dims.borderedViewportRect.w = windowWidth;
		engine_screen.dims.borderedViewportRect.h = windowHeight;
		engine_screen.dims.borderedViewportRect.x = 0;
		engine_screen.dims.borderedViewportRect.y = 0;
	}

	// Based on borderedViewportRect we next construct viewportRect, which
	// does NOT contain the overscan border. Useful for software rendering.

	// Begin with a simple copy
	engine_screen.dims.viewportRect = engine_screen.dims.borderedViewportRect;
	// Next, correct the dimensions
	// taking VGA 200-line mode double scanning into consideration
	engine_screen.dims.viewportRect.w = engine_screen.dims.viewportRect.w*engine_screen.dims.clientOffsettedZoomedRect.w/engine_screen.dims.clientZoomedBorderedWidth;
	engine_screen.dims.viewportRect.h = engine_screen.dims.viewportRect.h*engine_screen.dims.clientOffsettedZoomedRect.h/engine_screen.dims.clientZoomedBorderedHeight;
	// Finally correct the offsets,
	// again taking VGA 200-line mode double scanning into consideration
	if (engine_screen.client.currVidMode == 0xD) { // Graphical
		engine_screen.dims.viewportRect.x += engine_arguments.calc.overscanGfxLeft * engine_screen.dims.viewportRect.w / engine_screen.dims.clientRect.w;
		engine_screen.dims.viewportRect.y += engine_arguments.calc.overscanGfxTop * engine_screen.dims.zoomFactor * engine_screen.dims.viewportRect.h / (engine_screen.dims.clientOffsettedZoomedRect.h * engine_arguments.calc.gfxHeightScalingFactor);
	} else if (engine_screen.client.currVidMode == 3) { // Textual
		engine_screen.dims.viewportRect.x += engine_arguments.calc.overscanTxtLeft * engine_screen.dims.viewportRect.w / engine_screen.dims.clientRect.w;
		engine_screen.dims.viewportRect.y += engine_arguments.calc.overscanTxtTop * engine_screen.dims.viewportRect.h / engine_screen.dims.clientRect.h;
	} // For GUI/Launcher there's no overscan border so we do nothing

	/*************************
	Some output specific setup
	*************************/
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OPENGL) {
#if 0
		if (engine_screen.host.bilinearInterpolation && engine_screen.gl.offScreenRendering && engine_screen.gl.haveFramebufferBlit) {
			/* We call glBlitFramebufferEXT so use whole window */
			engine_glViewport(0, 0, windowWidth, windowHeight);
		} else {
#endif
			/* Otherwise discard black bars */
			engine_glViewport(engine_screen.dims.borderedViewportRect.x,
			                  engine_screen.dims.borderedViewportRect.y,
			                  engine_screen.dims.borderedViewportRect.w,
			                  engine_screen.dims.borderedViewportRect.h
			);
#if 0
		}
#endif
		if (!engine_screen.host.bilinearInterpolation) {
		        // We further need this! (Note: It's not yet enabled.)
		        engine_glScissor(engine_screen.dims.borderedViewportRect.x,
		                         engine_screen.dims.borderedViewportRect.y,
		                         engine_screen.dims.borderedViewportRect.w,
		                         engine_screen.dims.borderedViewportRect.h
		        );
		}
	}
}

void privResetBorderColor(void);
void privResetPalette(void);

void CVort_engine_reactToWindowResize(int width, int height)
{
	// Actually, better do nothing if this is SDL 1.2
#if SDL_VERSION_ATLEAST(2,0,0)
	/* Do NOT store new fullscreen dimensions:
	 * They may come from temporary window snapping or so.
	 *
	 * For now we also don't store new non-fullscreen window dimensions.
	 */
#if 0
	if (engine_gfx_effective_arguments.isFullscreen) {
		// Do NOT - this may come from window snapping or so
		engine_screen.host.fullWidth = width;
		engine_screen.host.fullHeight = height;
	} else {
		// For now it's also better to not set this
		engine_screen.host.winWidth = width;
		engine_screen.host.winHeight = height;
	}
#endif
#if 0
	/*************************
	Some output specific setup
	*************************/
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_SURFACE) {
		engine_screen.sdl.windowSurface = SDL_GetWindowSurface(engine_screen.sdl.window);
	}
#endif
	CVort_engine_prepareWindowRects((engine_gfx_effective_arguments.scaleType != GFX_SCALE_FILL));
	// Reset current border color (at least one case covered)
	privResetBorderColor();

	//CVort_engine_setVideoMode(engine_screen.client.currVidMode);
#endif
}


#if SDL_VERSION_ATLEAST(2,0,0)
void CVort_engine_handleWindowSideChange(void) {
	/*************************
	Some output specific setup
	*************************/
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_SURFACE) {
		engine_screen.sdl.windowSurface = SDL_GetWindowSurface(engine_screen.sdl.window);
		// Maybe a resized sub-window of some kind is used...
		if (!engine_arguments.doForceCutFullScreen && engine_gfx_effective_arguments.isFullscreen) {
			SDL_FillRect(engine_screen.sdl.windowSurface, NULL, SDL_MapRGB(engine_screen.sdl.windowSurface->format, 0, 0, 0));
		}
		privResetBorderColor();
	}
}
#endif


bool CVort_engine_prepareScreen() {
	engine_screen.sdl.windowSurface = NULL;
	engine_screen.sdl.secondarySurface = NULL;

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL
	memset(engine_screen.gl.textureNames, 0, sizeof(engine_screen.gl.textureNames));
	engine_screen.gl.frameBufferName = 0;
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
	engine_screen.gl.outputCallListName = 0;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	engine_screen.gl.palTextureName = NULL;
	engine_screen.gl.vertexShader = engine_screen.gl.fragmentShader = 0;
	engine_screen.gl.shaderProgram = 0;
	engine_screen.gl.positionLoc = engine_screen.gl.texCoordLoc = 0;
	engine_screen.gl.palSamplerLoc = engine_screen.gl.samplerLoc = 0;
        engine_screen.gl.palCyclingIntLoc = 0;
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
	engine_screen.sdl.glContext = NULL;
#endif
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
	engine_screen.sdl.window = NULL;
	engine_screen.sdl.renderer = NULL;
	engine_screen.sdl.texture = NULL;
	engine_screen.sdl.textureFormat = SDL_PIXELFORMAT_UNKNOWN;
	engine_screen.sdl.renderTexture = NULL;
#else
	engine_screen.sdl.overlay = NULL;
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_DisplayMode dispMode;
	SDL_GetDesktopDisplayMode(engine_arguments.displayNumber, &dispMode);
	engine_screen.host.desktopWidth = dispMode.w;
	engine_screen.host.desktopHeight = dispMode.h;
	engine_screen.sdl.desktopPixelFormat = dispMode.format;
#else
	engine_screen.sdl.videoInfo = SDL_GetVideoInfo();
	engine_screen.host.desktopWidth = engine_screen.sdl.videoInfo->current_w;
	engine_screen.host.desktopHeight = engine_screen.sdl.videoInfo->current_h;
#endif
#if 0
	if (!engine_gfx_effective_arguments.windowWidth || !engine_gfx_effective_arguments.windowHeight) {
		// Is there really a need to involve
		// the golden ratio here? Hmm...
		engine_screen.host.winWidth = engine_screen.host.desktopWidth * 309 / 500;
		engine_screen.host.winHeight = engine_screen.host.desktopHeight * 319 / 500;
	} else {
		engine_screen.host.winWidth = engine_gfx_effective_arguments.windowWidth;
		engine_screen.host.winHeight = engine_gfx_effective_arguments.windowHeight;
	}
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
	// Don't set this now. Should be set BEFORE the SDL video subssystem
	// is initialized, along with SDL_HINT_FRAMEBUFFER_ACCELERATION.
#if 0
	if (engine_arguments.rendererDriver) {
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, engine_arguments.rendererDriver);
	}
#endif
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	if (engine_arguments.outputSystem == OUTPUTSYS_OPENGL) {
		// TODO: If we fail loading this, no problem.
		SDL_GL_LoadLibrary(NULL);
#if 0
		if (SDL_GL_LoadLibrary(NULL) < 0) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Cannot load the OpenGL library. Is it installed?\n"
			                                                   "More details: %s\n", SDL_GetError());
			CVort_engine_shutdownSDL();
			return false;
		}
#endif
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 2);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 2);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 2);
		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#if SDL_VERSION_ATLEAST(2,0,0)
#if 0
		switch (engine_arguments.outputGLVersion) {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
		case OUTPUTGL_1_1:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			break;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
		case OUTPUTGL_2_0:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			break;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
		case OUTPUTGL_ES_2_0:
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			break;
#endif
		}
#endif
		// That is done AFTER the GL context is (re)created, and by
		// accessing engine_gfx_effective_arguments.vSync instead.
		//SDL_GL_SetSwapInterval(engine_arguments.vSync ? 1 : 0);
#else // SDL 1.2
		// Yeah, do NOT access engine_gfx_effective_arguments.vSync
		// here - this is CVort_engine_prepareScreen.
		SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, engine_arguments.vSync ? 1 : 0);
#endif
	}
#endif

	engine_screen.host.colorTable = NULL;
	engine_currPage = pel_panning = 0;
	engine_currPageStart = engine_dstPage = engine_egaStart = 0;

	return true;
}

// Modified from the function CheckExtension given in:
// http://www.mesa3d.org/brianp/sig97/exten.htm
bool CVort_engine_isGLExtensionAvailable(const char *extName) {
	/* Naive usage of strstr can yield unexpected results */
	char *extPtr = (char *)engine_glGetString(GL_EXTENSIONS);
	char *extEnd = extPtr + strlen(extPtr);
	int extNameLen = strlen(extName), n;

	while (extPtr < extEnd) {
		n = strcspn(extPtr, " ");
		if ((extNameLen == n) && (strncmp(extName, extPtr, n) == 0)) {
			return true;
		}
		extPtr += (n + 1);
	}
	return false;
}

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
void privClearGLShaderResources(void) {
	if (engine_screen.gl.shaderProgram) {
		// For SDL 1.2 with a GL 2.0+ compatibility context (not reset)
		engine_glUseProgram(0);

		engine_glDeleteProgram(engine_screen.gl.shaderProgram);
		engine_screen.gl.shaderProgram = 0;
	}
	if (engine_screen.gl.fragmentShader) {
		engine_glDeleteShader(engine_screen.gl.fragmentShader);
		engine_screen.gl.fragmentShader = 0;
	}
	if (engine_screen.gl.vertexShader) {
		engine_glDeleteShader(engine_screen.gl.vertexShader);
		engine_screen.gl.vertexShader = 0;
	}
}

#endif

void privDestroyWindowResources(void) {
#if SDL_VERSION_ATLEAST(2,0,0)
	if (engine_screen.sdl.renderTexture) {
		SDL_DestroyTexture(engine_screen.sdl.renderTexture);
		engine_screen.sdl.renderTexture = NULL;
	}
	if (engine_screen.sdl.texture) {
		SDL_DestroyTexture(engine_screen.sdl.texture);
		engine_screen.sdl.texture = NULL;
	}
#else
	if (engine_screen.sdl.overlay) {
		SDL_FreeYUVOverlay(engine_screen.sdl.overlay);
		engine_screen.sdl.overlay = NULL;
	}
#endif
	if (engine_screen.sdl.secondarySurface) {
		SDL_FreeSurface(engine_screen.sdl.secondarySurface);
		engine_screen.sdl.secondarySurface = NULL;
	}
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	if (engine_screen.gl.frameBufferName) {
		engine_glDeleteFramebuffers_CoreEXT(1, &engine_screen.gl.frameBufferName);
		engine_screen.gl.frameBufferName = 0;
	}
	if (engine_screen.gl.textureNames[0]) {
		engine_glDeleteTextures(1, &engine_screen.gl.textureNames[0]);
		engine_screen.gl.textureNames[0] = 0;
	}
	if (engine_screen.gl.textureNames[1]) {
		engine_glDeleteTextures(1, &engine_screen.gl.textureNames[1]);
		engine_screen.gl.textureNames[1] = 0;
	}
	if (engine_screen.gl.textureNames[2]) {
		engine_glDeleteTextures(1, &engine_screen.gl.textureNames[2]);
		engine_screen.gl.textureNames[2] = 0;
	}
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	engine_screen.gl.palTextureName = 0;
	privClearGLShaderResources();
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
	if (engine_screen.gl.outputCallListName) {
		engine_glDeleteLists(engine_screen.gl.outputCallListName, engine_screen.gl.numOfCallLists);
		engine_screen.gl.outputCallListName = 0;
	}
#endif
#endif	// _CHOCOLATE_KEEN_ENABLE_OPENGL_
}

bool privCreateWindowResources(void);
void privDestroyHostWindow(void);
bool privCreateHostWindow(void);

bool privCreateWindowResourcesFailSafe(void) {
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_SURFACE)
		return false;
	privDestroyWindowResources();
	privDestroyHostWindow();
	engine_gfx_effective_arguments.outputSystem = OUTPUTSYS_SURFACE;
	return (privCreateHostWindow() && privCreateWindowResources()); // Second chance
}

// Creates resources for window (but NOT the context/renderer)
bool privCreateWindowResources(void) {
	//engine_screen.host.isIndexedColorFormatted = false;
	switch (engine_gfx_effective_arguments.outputSystem) {
#if SDL_VERSION_ATLEAST(2,0,0)
		case OUTPUTSYS_TEXTURE:
			/* Before we create a texture, check if we REALLY need
			 * to create yet another texture for a render target:
			 * We do not apply two-step scaling (say with Normal2x)
			 * if bilinear interpolation is not in use. It is
			 * clearly the case if software rendering is in effect.
			 */
			if (engine_gfx_effective_arguments.bilinearInterpolation && !(engine_screen.sdl.rendererInfo.flags & SDL_RENDERER_SOFTWARE)) {
				engine_screen.host.bilinearInterpolation = true;
				if (engine_gfx_effective_arguments.offScreenRendering && (engine_screen.sdl.rendererInfo.flags & SDL_RENDERER_TARGETTEXTURE)) {
					SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
					engine_screen.sdl.renderTexture = SDL_CreateTexture(engine_screen.sdl.renderer, engine_screen.sdl.textureFormat, SDL_TEXTUREACCESS_TARGET, engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientZoomedBorderedHeight);
					if (!engine_screen.sdl.renderTexture) {
						CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create rendering texture: %s\n"
						                                                   "Disabling off-screen rendering.\n", SDL_GetError());
					}
				}
				if (engine_screen.sdl.renderTexture) {
					SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
					engine_screen.sdl.texture = SDL_CreateTexture(
					                                engine_screen.sdl.renderer,
					                                engine_screen.sdl.textureFormat,
					                                SDL_TEXTUREACCESS_STREAMING,
					                                engine_screen.dims.clientScanLineLength,
					                                engine_screen.dims.clientRect.h
					);
					/* We soon check for failure */
				} else {
					SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
					engine_screen.sdl.texture = SDL_CreateTexture(engine_screen.sdl.renderer, engine_screen.sdl.textureFormat, SDL_TEXTUREACCESS_STREAMING, engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientZoomedBorderedHeight);
					/* We soon check for failure */
				}
			} else {
				engine_screen.host.bilinearInterpolation = false;
				SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
				engine_screen.sdl.texture = SDL_CreateTexture(
				                                engine_screen.sdl.renderer,
				                                engine_screen.sdl.textureFormat,
				                                SDL_TEXTUREACCESS_STREAMING,
				                                engine_screen.dims.clientScanLineLength,
				                                engine_screen.dims.clientRect.h
				);
			}
			if (!engine_screen.sdl.texture) {
				CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create main texture: %s\n"
				                                                   "Reverting to software surface.\n", SDL_GetError());
				return privCreateWindowResourcesFailSafe();
			}
			//engine_screen.host.bytesPerPixel = SDL_BYTESPERPIXEL(engine_screen.sdl.textureFormat);
			engine_screen.host.texPitch = engine_screen.host.bytesPerPixel*engine_screen.dims.clientScanLineLength;
			break;
#else	// SDL 1.2
		case OUTPUTSYS_OVERLAY:
			// Double the width and use packed UYVY format
			engine_screen.sdl.overlay = SDL_CreateYUVOverlay(2*engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientZoomedBorderedHeight, SDL_UYVY_OVERLAY, engine_screen.sdl.windowSurface);
			if (!engine_screen.sdl.overlay) {
				CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create screen overlay: %s\n"
				                                                   "Reverting to software surface.\n", SDL_GetError());
				return privCreateWindowResourcesFailSafe();
			}
			//engine_screen.host.bytesPerPixel = 4;
			break;
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
		case OUTPUTSYS_OPENGL:
#if 0
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
			if ((engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0)) {
	 			// Consider this
				// FIXME: Is accessing engine_arguments here ok?
				engine_screen.gl.gpuPaletteCycling = engine_arguments.gpuPaletteCycling;
				// Don't, if offscreen rendering is disabled and we want bilinear interpolation
				engine_screen.gl.gpuPaletteCycling &= (engine_screen.gl.offScreenRendering || !engine_screen.host.bilinearInterpolation);
				// Based on that...
				engine_screen.host.bytesPerPixel = engine_screen.gl.gpuPaletteCycling ? 1 : 4;
				engine_screen.host.isIndexedColorFormatted = engine_screen.gl.gpuPaletteCycling;
			} else {
				engine_screen.gl.gpuPaletteCycling = false;
				engine_screen.host.bytesPerPixel = 4;
			}
#else
			engine_screen.host.bytesPerPixel = 4;
#endif
#endif
			if (engine_screen.host.bilinearInterpolation && !engine_screen.gl.offScreenRendering) {
				engine_screen.sdl.secondarySurface = SDL_CreateRGBSurface(
				    0, engine_screen.dims.clientOffsettedZoomedRect.w,
				    engine_screen.dims.clientOffsettedZoomedRect.h, 8*engine_screen.host.bytesPerPixel,
				    0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
				);
				if (!engine_screen.sdl.secondarySurface) {
					CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create secondary screen surface: %s\n"
					                                                   "Reverting to software surface.\n", SDL_GetError());
					return privCreateWindowResourcesFailSafe();
				}
			}
			if (!CVort_engine_preparegl()) {
				return privCreateWindowResourcesFailSafe();
			}
			break;
#endif
		case OUTPUTSYS_SURFACE:
#if 0
#if SDL_VERSION_ATLEAST(2,0,0)
			engine_screen.sdl.windowSurface = SDL_GetWindowSurface(engine_screen.sdl.window);
			if (!engine_screen.sdl.windowSurface) {
				CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't retrieve window surface: %s\n", SDL_GetError());
				CVort_engine_shutdownSDL();
				return false;
			}
#endif
			engine_screen.host.bytesPerPixel = engine_screen.sdl.windowSurface->format->BytesPerPixel;
			// Maybe a resized sub-window of some kind is used...
			if (!engine_arguments.doForceCutFullScreen && engine_gfx_effective_arguments.isFullscreen) {
				SDL_FillRect(engine_screen.sdl.windowSurface, NULL, SDL_MapRGB(engine_screen.sdl.windowSurface->format, 0, 0, 0));
			}
#endif
			// FIXME: Prepare a secondary surface based on existing system RAM
			break;
	}
	return true;
}

// Destroys window and renderer/context ("Do nothing" in SDL 1.2)
void privDestroyHostWindow(void) {
#if SDL_VERSION_ATLEAST(2,0,0)
	if (engine_screen.sdl.renderer) {
		SDL_DestroyRenderer(engine_screen.sdl.renderer);
		engine_screen.sdl.renderer = NULL;
	}
#if _CHOCOLATE_KEEN_ENABLE_OPENGL_
	if (engine_screen.sdl.glContext) {
		SDL_GL_DeleteContext(engine_screen.sdl.glContext);
		engine_screen.sdl.glContext = NULL;
	}
#endif
	if (engine_screen.sdl.window) {
		SDL_DestroyWindow(engine_screen.sdl.window);
		engine_screen.sdl.window = NULL;
	}
#endif // SDL_VERSION_ATLEAST(2,0,0)
}

bool privCreateHostWindowFailSafe(void) {
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_SURFACE)
		return false;
	privDestroyHostWindow();
	engine_gfx_effective_arguments.outputSystem = OUTPUTSYS_SURFACE;
	return privCreateHostWindow(); // Second chance
}

bool privCreateHostWindow(void) {

	int screenWidth = engine_gfx_effective_arguments.isFullscreen ? engine_screen.host.fullWidth : engine_screen.host.winWidth;
	int screenHeight = engine_gfx_effective_arguments.isFullscreen ? engine_screen.host.fullHeight : engine_screen.host.winHeight;

#if SDL_VERSION_ATLEAST(2,0,0)

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	switch (engine_arguments.outputGLVersion) {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
	case OUTPUTGL_1_1:
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		break;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
	case OUTPUTGL_2_0:
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		break;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
	case OUTPUTGL_ES_2_0:
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		break;
#endif
	}

	// Still under the assumption _CHOCOLATE_KEEN_ENABLE_OPENGL_ is defined;
	// Make window resizable ONLY if it should have the launcher UI
	//engine_screen.sdl.window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED_DISPLAY(engine_arguments.displayNumber), SDL_WINDOWPOS_UNDEFINED_DISPLAY(engine_arguments.displayNumber), screenWidth, screenHeight, (engine_gfx_effective_arguments.isFullscreen ? (engine_screen.host.useFullDesktopDims ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN) : 0) | ((engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OPENGL) ? SDL_WINDOW_OPENGL : 0) | ((engine_screen.client.currVidMode == -1) ? SDL_WINDOW_RESIZABLE : 0));
	engine_screen.sdl.window = SDL_CreateWindow("", 0, 0, 640, 400, 0);
#else // Not OpenGL
	// Make window resizable ONLY if it should have the launcher UI
	//engine_screen.sdl.window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED_DISPLAY(engine_arguments.displayNumber), SDL_WINDOWPOS_UNDEFINED_DISPLAY(engine_arguments.displayNumber), screenWidth, screenHeight, (engine_gfx_effective_arguments.isFullscreen ? (engine_screen.host.useFullDesktopDims ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN) : 0) | ((engine_screen.client.currVidMode == -1) ? SDL_WINDOW_RESIZABLE : 0));
	engine_screen.sdl.window = SDL_CreateWindow("", 0, 0, 640, 400, 0);
#endif // GL
	if (!engine_screen.sdl.window) {
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
		if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OPENGL) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create window: %s\n"
			                                                   "Reverting to software surface.\n", SDL_GetError());
			return privCreateHostWindowFailSafe();
		}
#endif
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create window: %s\n", SDL_GetError());
		CVort_engine_shutdownSDL();
		return false;
	}
	if (engine_gfx_effective_arguments.isFullscreen) {
		SDL_GetWindowSize(engine_screen.sdl.window, &screenWidth, &screenHeight);
		if ((screenWidth != engine_screen.host.fullWidth) || (screenHeight != engine_screen.host.fullHeight)) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Requested fullscreen resolution %dx%d, got %dx%d.\n", engine_screen.host.fullWidth, engine_screen.host.fullHeight, screenWidth, screenHeight);	
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Disabling fullscreen.\n");
			engine_gfx_effective_arguments.isFullscreen = false;
			privDestroyHostWindow();
			return privCreateHostWindow();
		}
	}

#else // SDL 1.2

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	engine_screen.sdl.windowSurface = SDL_SetVideoMode(screenWidth, screenHeight, engine_screen.sdl.videoInfo->vfmt->BitsPerPixel, ((engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OPENGL) ? SDL_OPENGL : (engine_gfx_effective_arguments.vSync ? (SDL_DOUBLEBUF | SDL_HWSURFACE) : 0)) | (engine_gfx_effective_arguments.isFullscreen ? SDL_FULLSCREEN : 0));
#else
	engine_screen.sdl.windowSurface = SDL_SetVideoMode(screenWidth, screenHeight, engine_screen.sdl.videoInfo->vfmt->BitsPerPixel, (engine_gfx_effective_arguments.vSync ? (SDL_DOUBLEBUF | SDL_HWSURFACE) : 0) | (engine_gfx_effective_arguments.isFullscreen ? SDL_FULLSCREEN : 0));
#endif	// GL
	if (!engine_screen.sdl.windowSurface) {
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
		if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OPENGL) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't set video mode: %s\n"
			                                                   "Reverting to software surface.\n", SDL_GetError());
			return privCreateHostWindowFailSafe();
		}
#endif
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't set video mode: %s\n", SDL_GetError());
		CVort_engine_shutdownSDL();
		return false;
	}
#endif // SDL 1.2/2.0

	CVort_engine_setWindowTitleAndIcon();

	/* Once the window is ready, some more things are required */

	engine_screen.host.isIndexedColorFormatted = false; // Usually

	switch (engine_gfx_effective_arguments.outputSystem) {
#if SDL_VERSION_ATLEAST(2,0,0)
	case OUTPUTSYS_TEXTURE:
		engine_screen.sdl.renderer = SDL_CreateRenderer(engine_screen.sdl.window, engine_gfx_effective_arguments.rendererDriverIndex, SDL_RENDERER_ACCELERATED | (engine_gfx_effective_arguments.vSync ? SDL_RENDERER_PRESENTVSYNC : 0));
		if (!engine_screen.sdl.renderer) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create renderer: %s\n"
			                                                   "Reverting to software surface.\n", SDL_GetError());
			return privCreateHostWindowFailSafe();
		}
		SDL_GetRendererInfo(engine_screen.sdl.renderer, &engine_screen.sdl.rendererInfo);
		if (engine_screen.sdl.textureFormat == SDL_PIXELFORMAT_UNKNOWN) {
			if (engine_screen.sdl.rendererInfo.flags & SDL_RENDERER_SOFTWARE) {
				// Check if the desktop format is in the array.
				int texFormatLoopVar;
				for (texFormatLoopVar = 0; texFormatLoopVar < engine_screen.sdl.rendererInfo.num_texture_formats; texFormatLoopVar++) {
					if (engine_screen.sdl.rendererInfo.texture_formats[texFormatLoopVar] == engine_screen.sdl.desktopPixelFormat
) {
						engine_screen.sdl.textureFormat = engine_screen.sdl.desktopPixelFormat;
						break;
					}
				}
				if (texFormatLoopVar == engine_screen.sdl.rendererInfo.num_texture_formats) {
					// Use this as a fallback
					engine_screen.sdl.textureFormat = engine_screen.sdl.rendererInfo.texture_formats[0];
				}
			} else { // Not a software renderer
				engine_screen.sdl.textureFormat = engine_screen.sdl.rendererInfo.texture_formats[0];
			}
		}
		engine_screen.host.bytesPerPixel = SDL_BYTESPERPIXEL(engine_screen.sdl.textureFormat);
		break;
#else // SDL 1.2
	case OUTPUTSYS_OVERLAY:
		// We don't create the overlay here (considered a "resource")
		// but the pixel format is known at this point.
		engine_screen.host.bytesPerPixel = 4;
		break;
#endif // SDL 1.2/2.0
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	case OUTPUTSYS_OPENGL:
	{
#if SDL_VERSION_ATLEAST(2,0,0)
		engine_screen.sdl.glContext = SDL_GL_CreateContext(engine_screen.sdl.window);
		if (!engine_screen.sdl.glContext) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create OpenGL context: %s\n"
			                                                   "Reverting to software surface.\n", SDL_GetError());
			return privCreateHostWindowFailSafe();
		}
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "Created OpenGL context.\n");
		SDL_GL_SetSwapInterval(engine_gfx_effective_arguments.vSync ? 1 : 0);
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "%s sync to vblank.\n", engine_gfx_effective_arguments.vSync ? "Enabling" : "Disabling");
#else
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "OpenGL context should be ready.\n"); // SDL_SetVideoMode may use an existing context
#endif
		engine_screen.host.bilinearInterpolation = engine_gfx_effective_arguments.bilinearInterpolation;
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "%s bilinear interpolation.\n", engine_screen.host.bilinearInterpolation ? "Enabling" : "Disabling");

		/* Bad idea to use names like glMajorVer
		 * (conflicts with a GL standard may arise)
		 */
		int ourGLmajorVer, ourGLminorVer;
		bool isEmbedded, foundCompatibleGLVersion = false;
		engine_glGetString = (glGetString_Func_T)SDL_GL_GetProcAddress("glGetString");
		// If engine_glGetString is set to NULL, this is
		// a quite unsupported case (OpenGL 1.0 for desktops??)
		if (!engine_glGetString) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't fetch OpenGL version for context.\n"
			                                                   "Reverting to software surface.\n");
			/* NOTE: The actual fallback is done very soon */
		} else {
			const char* glVersionStr = (const char *)engine_glGetString(GL_VERSION);
			if (!glVersionStr) {
				CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't fetch OpenGL version for context.\n"
				                                                   "Reverting to software surface.\n");
			} else {
				CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "GL version string: %s\n", glVersionStr);
				// First check for desktop version
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_DESKTOP_ANY_
				if (sscanf(glVersionStr, "%d.%d", &ourGLmajorVer, &ourGLminorVer) == 2) {
					isEmbedded = false;
					switch (engine_arguments.outputGLVersion) {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
					case OUTPUTGL_1_1:
						if ((ourGLmajorVer > 1) || ((ourGLmajorVer == 1) && (ourGLminorVer >= 1))) {
							engine_screen.gl.outputGLClass = OUTPUTGL_CLASS_1_1;
							foundCompatibleGLVersion = true;
							CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "Using OpenGL 1.1 renderer.\n");
						}
						break;
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_
					case OUTPUTGL_2_0:
						if ((ourGLmajorVer > 2) || ((ourGLmajorVer == 2) && (ourGLminorVer >= 0))) {
							engine_screen.gl.outputGLClass = OUTPUTGL_CLASS_2_0;
							foundCompatibleGLVersion = true;
							CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "Using OpenGL 2.0 / ES 2.0 renderer.\n");
						}
						break;
#endif
					}
				}
#endif // _CHOCOLATE_KEEN_HAVE_OPENGL_DESKTOP_ANY_
				// Try a case like "OpenGL ES 2.0"
				// (OpenGL ES 1.0/1.1 has a different syntax!)
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_ES_2_0_
				if (sscanf(glVersionStr, "OpenGL ES %d.%d", &ourGLmajorVer, &ourGLminorVer) == 2) {
					isEmbedded = true;
					if ((engine_arguments.outputGLVersion == OUTPUTGL_ES_2_0) &&
					    ((ourGLmajorVer > 2) || ((ourGLmajorVer == 2) && (ourGLminorVer >= 0)))
					   ) {
						engine_screen.gl.outputGLClass = OUTPUTGL_CLASS_2_0;
						foundCompatibleGLVersion = true;
						CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "Using OpenGL 2.0 / ES 2.0 renderer.\n");
					}
				}
#endif
				// If we haven't detected version so far...
				if (!foundCompatibleGLVersion) {
					CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Unsupported or unknown OpenGL version detected.\n"
					                                                   "Reverting to software surface.\n", ourGLmajorVer, ourGLminorVer);
					/* NOTE: The actual fallback is done very soon */
				}
			}
		}
		if (!foundCompatibleGLVersion) {
			return privCreateHostWindowFailSafe();
		}
		/* In OpenGL 1.1-3.0 for the desktop, and OpenGL ES 2.0,
		 * all of these should be available and usable.
		 */
		engine_glClearColor = (glClearColor_Func_T)SDL_GL_GetProcAddress("glClearColor");
		engine_glViewport = (glViewport_Func_T)SDL_GL_GetProcAddress("glViewport");
		engine_glGetIntegerv = (glGetIntegerv_Func_T)SDL_GL_GetProcAddress("glGetIntegerv");
		engine_glEnable = (glEnable_Func_T)SDL_GL_GetProcAddress("glEnable");
		engine_glGenTextures = (glGenTextures_Func_T)SDL_GL_GetProcAddress("glGenTextures");
		engine_glBindTexture = (glBindTexture_Func_T)SDL_GL_GetProcAddress("glBindTexture");
		engine_glTexParameteri = (glTexParameteri_Func_T)SDL_GL_GetProcAddress("glTexParameteri");
		engine_glTexImage2D = (glTexImage2D_Func_T)SDL_GL_GetProcAddress("glTexImage2D");
		engine_glClear = (glClear_Func_T)SDL_GL_GetProcAddress("glClear");
		engine_glDisable = (glDisable_Func_T)SDL_GL_GetProcAddress("glDisable");

		engine_glDeleteTextures = (glDeleteTextures_Func_T)SDL_GL_GetProcAddress("glDeleteTextures");

		engine_glTexSubImage2D = (glTexSubImage2D_Func_T)SDL_GL_GetProcAddress("glTexSubImage2D");
		engine_glCopyTexSubImage2D = (glCopyTexSubImage2D_Func_T)SDL_GL_GetProcAddress("glCopyTexSubImage2D");
		engine_glScissor = (glScissor_Func_T)SDL_GL_GetProcAddress("glScissor");
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
		if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
			engine_glMatrixMode = (glMatrixMode_Func_T)SDL_GL_GetProcAddress("glMatrixMode");
			engine_glLoadIdentity = (glLoadIdentity_Func_T)SDL_GL_GetProcAddress("glLoadIdentity");
			engine_glOrtho = (glOrtho_Func_T)SDL_GL_GetProcAddress("glOrtho");
			engine_glGenLists = (glGenLists_Func_T)SDL_GL_GetProcAddress("glGenLists");
			engine_glNewList = (glNewList_Func_T)SDL_GL_GetProcAddress("glNewList");
			engine_glBegin = (glBegin_Func_T)SDL_GL_GetProcAddress("glBegin");
			engine_glTexCoord2f = (glTexCoord2f_Func_T)SDL_GL_GetProcAddress("glTexCoord2f");
			engine_glVertex3f = (glVertex3f_Func_T)SDL_GL_GetProcAddress("glVertex3f");
			engine_glEnd = (glEnd_Func_T)SDL_GL_GetProcAddress("glEnd");
			engine_glEndList = (glEndList_Func_T)SDL_GL_GetProcAddress("glEndList");
			engine_glPushAttrib = (glPushAttrib_Func_T)SDL_GL_GetProcAddress("glPushAttrib");
			engine_glPopAttrib = (glPopAttrib_Func_T)SDL_GL_GetProcAddress("glPopAttrib");
			engine_glDeleteLists = (glDeleteLists_Func_T)SDL_GL_GetProcAddress("glDeleteLists");
			engine_glCallList = (glCallList_Func_T)SDL_GL_GetProcAddress("glCallList");
			//engine_glDrawBuffer = (glDrawBuffer_Func_T)SDL_GL_GetProcAddress("glDrawBuffer");
			//engine_glReadBuffer = (glReadBuffer_Func_T)SDL_GL_GetProcAddress("glReadBuffer");
		}
#endif
		/* For the sake of simplicity we don't bother
		 * with various extensions for the non-FBO
		 * methods and simply require OpenGL v2.0.
		 */
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
		if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
			// OpenGL 2.0 / ES 2.0:
			// Shader related functions
			engine_glCreateShader = (glCreateShader_Func_T)SDL_GL_GetProcAddress("glCreateShader");
			engine_glShaderSource = (glShaderSource_Func_T)SDL_GL_GetProcAddress("glShaderSource");
			engine_glCompileShader = (glCompileShader_Func_T)SDL_GL_GetProcAddress("glCompileShader");
			engine_glGetShaderiv = (glGetShaderiv_Func_T)SDL_GL_GetProcAddress("glGetShaderiv");
			engine_glGetShaderInfoLog = (glGetShaderInfoLog_Func_T)SDL_GL_GetProcAddress("glGetShaderInfoLog");
			engine_glDeleteShader = (glDeleteShader_Func_T)SDL_GL_GetProcAddress("glDeleteShader");

			engine_glCreateProgram = (glCreateProgram_Func_T)SDL_GL_GetProcAddress("glCreateProgram");
			engine_glAttachShader = (glAttachShader_Func_T)SDL_GL_GetProcAddress("glAttachShader");
			engine_glLinkProgram = (glLinkProgram_Func_T)SDL_GL_GetProcAddress("glLinkProgram");
			engine_glGetProgramiv = (glGetProgramiv_Func_T)SDL_GL_GetProcAddress("glGetProgramiv");
			engine_glGetProgramInfoLog = (glGetProgramInfoLog_Func_T)SDL_GL_GetProcAddress("glGetProgramInfoLog");
			engine_glDeleteProgram = (glDeleteProgram_Func_T)SDL_GL_GetProcAddress("glDeleteProgram");
			engine_glUseProgram = (glUseProgram_Func_T)SDL_GL_GetProcAddress("glUseProgram");

			engine_glGetAttribLocation = (glGetAttribLocation_Func_T)SDL_GL_GetProcAddress("glGetAttribLocation");
			engine_glGetUniformLocation = (glGetUniformLocation_Func_T)SDL_GL_GetProcAddress("glGetUniformLocation");

			engine_glVertexAttribPointer = (glVertexAttribPointer_Func_T)SDL_GL_GetProcAddress("glVertexAttribPointer");
			engine_glEnableVertexAttribArray = (glEnableVertexAttribArray_Func_T)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
			engine_glUniform1i = (glUniform1i_Func_T)SDL_GL_GetProcAddress("glUniform1i");
			// Actually a GL 1.1 (and ES 2.0) function
			engine_glDrawElements = (glDrawElements_Func_T)SDL_GL_GetProcAddress("glDrawElements");
			// Similarly a GL 1.3 (ES 2.0) function
			// (used for palette cycling)
			engine_glActiveTexture = (glActiveTexture_Func_T)SDL_GL_GetProcAddress("glActiveTexture");
		}
		/* glCopyTexSubImage2D seems to be unsupported
		 * or buggy here with OpenGL ES 2.0, while
		 * FBO functionality is supported.
		 * So always use an FBO in ES 2.0 if relevant.
		 *
		 * Otherwise, check for support.
		 */
		engine_screen.gl.offScreenRendering = isEmbedded || (engine_gfx_effective_arguments.offScreenRendering && CVort_engine_isGLExtensionAvailable("GL_EXT_framebuffer_object"));
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "%s offscreen rendering (via a framebuffer object)\nif bilinear interpolation is also enabled.\n", engine_screen.host.bilinearInterpolation ? "Enabling" : "Disabling");
		if (engine_screen.gl.offScreenRendering) {
			// We need glActiveTexture (GL 1.5)
			// UPDATE: Do we?
			//engine_glActiveTexture = (glActiveTexture_Func_T)SDL_GL_GetProcAddress("glActiveTexture");
			// Also glDrawBuffers (GL 2.0)
			// UPDATE: Nope, we can use glDrawBuffer (if necessary at all)
			//engine_glDrawBuffers = (glDrawBuffers_Func_T)SDL_GL_GetProcAddress("glDrawBuffers");

			// FBO methods come next
			// (Extension in GL pre-3.0, Core in GL ES 2.0)
			engine_glBindFramebuffer_CoreEXT = (glBindFramebufferEXT_Func_T)SDL_GL_GetProcAddress(isEmbedded ? "glBindFramebuffer" : "glBindFramebufferEXT");
			engine_glDeleteFramebuffers_CoreEXT = (glDeleteFramebuffersEXT_Func_T)SDL_GL_GetProcAddress(isEmbedded ? "glDeleteFramebuffers" : "glDeleteFramebuffersEXT");
			engine_glGenFramebuffers_CoreEXT = (glGenFramebuffersEXT_Func_T)SDL_GL_GetProcAddress(isEmbedded ? "glGenFramebuffers" : "glGenFramebuffersEXT");
			engine_glCheckFramebufferStatus_CoreEXT = (glCheckFramebufferStatusEXT_Func_T)SDL_GL_GetProcAddress(isEmbedded ? "glCheckFramebufferStatus" : "glCheckFramebufferStatusEXT");
			engine_glFramebufferTexture2D_CoreEXT = (glFramebufferTexture2DEXT_Func_T)SDL_GL_GetProcAddress(isEmbedded ? "glFramebufferTexture2D" : "glFramebufferTexture2DEXT");
			// Consider this (desktop GL only for now)
			engine_screen.gl.haveFramebufferBlit = CVort_engine_isGLExtensionAvailable("GL_EXT_framebuffer_blit");
			if (engine_screen.gl.haveFramebufferBlit) {
				CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "Framebuffer object blitting is supported.\n");
				engine_glBlitFramebufferEXT = (glBlitFramebufferEXT_Func_T)SDL_GL_GetProcAddress("glBlitFramebufferEXT");
			}
		}
#endif // _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
		if ((engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0)) {
	 		// Consider this
			// FIXME: Is accessing engine_arguments here ok?
			engine_screen.gl.gpuPaletteCycling = engine_arguments.gpuPaletteCycling;
			// Don't, if offscreen rendering is disabled and we want bilinear interpolation
			engine_screen.gl.gpuPaletteCycling &= (engine_screen.gl.offScreenRendering || !engine_screen.host.bilinearInterpolation);
			// Based on that...
			engine_screen.host.bytesPerPixel = engine_screen.gl.gpuPaletteCycling ? 1 : 4;
			engine_screen.host.isIndexedColorFormatted = engine_screen.gl.gpuPaletteCycling;
		} else {
			engine_screen.gl.gpuPaletteCycling = false;
			engine_screen.host.bytesPerPixel = 4;
		}
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_NORMAL, "%s GPU palette cycling.\n", engine_screen.gl.gpuPaletteCycling ? "Enabling" : "Disabling");
#else
		engine_screen.host.bytesPerPixel = 4;
#endif
		break;
	}
#endif // _CHOCOLATE_KEEN_ENABLE_OPENGL_
	case OUTPUTSYS_SURFACE:
#if SDL_VERSION_ATLEAST(2,0,0)
		// The window surface is currently not considered
		// a "resource" like a texture. Main reason is that
		// the surface is required to obtain bytesPerPixel.
		// In general it comes from SDL 1.2 style code, though.
		engine_screen.sdl.windowSurface = SDL_GetWindowSurface(engine_screen.sdl.window);
		if (!engine_screen.sdl.windowSurface) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't retrieve window surface: %s\n", SDL_GetError());
			CVort_engine_shutdownSDL();
			return false;
		}
#endif
		engine_screen.host.bytesPerPixel = engine_screen.sdl.windowSurface->format->BytesPerPixel;
#if 0
		// Maybe a resized sub-window of some kind is used...
		if (!engine_arguments.doForceCutFullScreen && engine_gfx_effective_arguments.isFullscreen) {
			SDL_FillRect(engine_screen.sdl.windowSurface, NULL, SDL_MapRGB(engine_screen.sdl.windowSurface->format, 0, 0, 0));
		}
#endif
		// FIXME: Prepare a secondary surface based on existing system RAM
		break;
	}

	/* Next, we initialize some internal (host) color palette,
	 * and similarly create storage for emulated EGA graphical
	 * contents using the exact same (host) pixel format.
	 */
	if (engine_screen.host.colorTable) {
		free(engine_screen.host.colorTable);
	}
	/* Allocate memory AT ONCE for egaMemoryPtr, colorTable and mappedEgaColorTable */
	/* HACK/Special: For indexed color data we simply point to byteEgaMemory */
	if (engine_screen.host.isIndexedColorFormatted) {
		engine_screen.host.colorTable = (uint8_t *)malloc(16); // Required for text mode and launcher UI
		engine_screen.host.egaMemoryPtr = engine_screen.client.byteEgaMemory;
	} else {
		engine_screen.host.colorTable = (uint8_t *)malloc(engine_screen.host.bytesPerPixel*(ENGINE_EGA_MEMORY_IN_BITS+32));
		engine_screen.host.mappedEgaColorTable = engine_screen.host.colorTable + engine_screen.host.bytesPerPixel*16;
		engine_screen.host.egaMemoryPtr = engine_screen.host.colorTable + engine_screen.host.bytesPerPixel*32;
	}

	/* Pick draw function pointers based on the color depth */
	switch (engine_screen.host.bytesPerPixel) {
	case 2:
		CVort_engine_updateEgaGfxNonPalRect_ptr = &CVort_engine_updateEgaGfxNonPalRect_16bpp;
		CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr = &CVort_engine_updateEgaNonBlinkingTxtNonPalRect_16bpp;
		CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr = &CVort_engine_updateEgaBlinkingTxtNonPalChar_16bpp;
		CVort_engine_doDrawEgaTxtCursor_ptr = &CVort_engine_doDrawEgaTxtCursor_16bpp;
		CVort_engine_updateBorderedZoomedRectBuffer_ptr = &CVort_engine_updateBorderedZoomedRectBuffer_16bpp;
		CVort_engine_updateBorderedZoomedRectBufferBorder_ptr = &CVort_engine_updateBorderedZoomedRectBufferBorder_16bpp;

		CVort_engine_gui_drawFontChar_ptr = &CVort_engine_gui_drawFontChar_16bpp;
		CVort_engine_gui_drawRoundedRectBorder_ptr = &CVort_engine_gui_drawRoundedRectBorder_16bpp;
		break;
	case 4:
		CVort_engine_updateEgaGfxNonPalRect_ptr = &CVort_engine_updateEgaGfxNonPalRect_32bpp;
		CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr = &CVort_engine_updateEgaNonBlinkingTxtNonPalRect_32bpp;
		CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr = &CVort_engine_updateEgaBlinkingTxtNonPalChar_32bpp;
		CVort_engine_doDrawEgaTxtCursor_ptr = &CVort_engine_doDrawEgaTxtCursor_32bpp;
		CVort_engine_updateBorderedZoomedRectBuffer_ptr = &CVort_engine_updateBorderedZoomedRectBuffer_32bpp;
		CVort_engine_updateBorderedZoomedRectBufferBorder_ptr = &CVort_engine_updateBorderedZoomedRectBufferBorder_32bpp;

		CVort_engine_gui_drawFontChar_ptr = &CVort_engine_gui_drawFontChar_32bpp;
		CVort_engine_gui_drawRoundedRectBorder_ptr = &CVort_engine_gui_drawRoundedRectBorder_32bpp;
		break;
	default:// Indexed color: Text mode and launcher UI
		// need methods, the rest don't (for now).
		if (engine_screen.host.isIndexedColorFormatted) {
			CVort_engine_updateEgaGfxNonPalRect_ptr = &CVort_engine_updateEgaGfxNonPalRect_doNothing;
			CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr = &CVort_engine_updateEgaNonBlinkingTxtNonPalRect_indexed8bpp;
			CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr = &CVort_engine_updateEgaBlinkingTxtNonPalChar_indexed8bpp;
			CVort_engine_doDrawEgaTxtCursor_ptr = &CVort_engine_doDrawEgaTxtCursor_indexed8bpp;
			CVort_engine_updateBorderedZoomedRectBuffer_ptr = 0;
			CVort_engine_updateBorderedZoomedRectBufferBorder_ptr = 0;

			CVort_engine_gui_drawFontChar_ptr = &CVort_engine_gui_drawFontChar_indexed8bpp;
			CVort_engine_gui_drawRoundedRectBorder_ptr = &CVort_engine_gui_drawRoundedRectBorder_indexed8bpp;
		} else {
			CVort_engine_updateEgaGfxNonPalRect_ptr = &CVort_engine_updateEgaGfxNonPalRect_otherbpp;
			CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr = &CVort_engine_updateEgaNonBlinkingTxtNonPalRect_otherbpp;
			CVort_engine_updateEgaBlinkingTxtNonPalChar_ptr = &CVort_engine_updateEgaBlinkingTxtNonPalChar_otherbpp;
			CVort_engine_doDrawEgaTxtCursor_ptr = &CVort_engine_doDrawEgaTxtCursor_otherbpp;
			CVort_engine_updateBorderedZoomedRectBuffer_ptr = &CVort_engine_updateBorderedZoomedRectBuffer_otherbpp;
			CVort_engine_updateBorderedZoomedRectBufferBorder_ptr = &CVort_engine_updateBorderedZoomedRectBufferBorder_otherbpp;

			CVort_engine_gui_drawFontChar_ptr = &CVort_engine_gui_drawFontChar_otherbpp;
			CVort_engine_gui_drawRoundedRectBorder_ptr = &CVort_engine_gui_drawRoundedRectBorder_otherbpp;
		}
	}
	/***************************************************
	Fill a table of a default EGA color palette, using
	relevant SDL values. The colors, in RGB format, are:
	0x000000, 0x0000aa, 0x00aa00, 0x00aaaa,
	0xaa0000, 0xaa00aa, 0xaa5500, 0xaaaaaa,
	0x555555, 0x5555ff, 0x55ff55, 0x55ffff,
	0xff5555, 0xff55ff, 0xffff55, 0xffffff.
	***************************************************/
	int colorLoopVar;
	switch (engine_gfx_effective_arguments.outputSystem) {
#if SDL_VERSION_ATLEAST(2,0,0)
	case OUTPUTSYS_TEXTURE:
	{
		SDL_PixelFormat *pixFormat = SDL_AllocFormat(engine_screen.sdl.textureFormat);
		uint32_t mappedRGBVal;
		for (colorLoopVar = 0; colorLoopVar < 16; colorLoopVar++) {
			mappedRGBVal = SDL_MapRGB(pixFormat, engine_egaRGBColorTable[colorLoopVar] >> 16, (engine_egaRGBColorTable[colorLoopVar] >> 8) & 0xFF, engine_egaRGBColorTable[colorLoopVar] & 0xFF);
			switch (engine_screen.host.bytesPerPixel) {
				case 2:
					((uint16_t *)engine_screen.host.colorTable)[colorLoopVar] = mappedRGBVal;
					break;
				case 4:
					((uint32_t *)engine_screen.host.colorTable)[colorLoopVar] = mappedRGBVal;
					break;
				default:
					if (engine_screen.host.bytesPerPixel < 4) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, ((uint8_t *)&mappedRGBVal)+4-engine_screen.host.bytesPerPixel, engine_screen.host.bytesPerPixel);
#else
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, &mappedRGBVal, engine_screen.host.bytesPerPixel);
#endif
					} else {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						memset(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, 0, engine_screen.host.bytesPerPixel-4);
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar+engine_screen.host.bytesPerPixel-4, &mappedRGBVal, 4);
#else
						memset(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar+4, 0, engine_screen.host.bytesPerPixel-4);
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, &mappedRGBVal, 4);
#endif
					}
			}
		}
		SDL_FreeFormat(pixFormat);
		break;
	}
#else	// SDL 1.2
	case OUTPUTSYS_OVERLAY:
	{
		/*********************************
		Here we use the equations:
		Y = 0.299R + 0.587G + 0.114B
		U = 0.564(B-Y) + CENTER
		V = 0.713(R-Y) + CENTER
		Based on info from:
		http://www.fourcc.org/fccyvrgb.php
		*********************************/
		int32_t colorR, colorG, colorB, colorY, colorU, colorV;
		// NOTE: We use SIGNED variables due
		// to signed multiplications involved!
		for (colorLoopVar = 0; colorLoopVar < 16; colorLoopVar++) {
			colorR = engine_egaRGBColorTable[colorLoopVar] >> 16;
			colorG = (engine_egaRGBColorTable[colorLoopVar] >> 8) & 0xFF;
			colorB = engine_egaRGBColorTable[colorLoopVar] & 0xFF;
			colorY = (299*colorR+587*colorG+114*colorB)/1000;
			colorU = 564*(colorB-colorY)/1000 + 128;
			colorV = 713*(colorR-colorY)/1000 + 128;
			// We now practically convert to uint32_t
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			((uint32_t *)engine_screen.host.colorTable)[colorLoopVar] = (colorU << 24) | (colorY << 16) | (colorV << 8) | colorY;
#else
			((uint32_t *)engine_screen.host.colorTable)[colorLoopVar] = colorU | (colorY << 8) | (colorV << 16) | (colorY << 24);
#endif
		}
		break;
	}
#endif	// SDL 1.2/2.0
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
	case OUTPUTSYS_OPENGL:
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
		if (engine_screen.host.isIndexedColorFormatted) { // Don't translate
			for (colorLoopVar = 0; colorLoopVar < 16; colorLoopVar++) {
				((uint8_t *)engine_screen.host.colorTable)[colorLoopVar] = colorLoopVar;
			}
		} else
#endif
			for (colorLoopVar = 0; colorLoopVar < 16; colorLoopVar++) {
				((uint32_t *)engine_screen.host.colorTable)[colorLoopVar] = SDL_SwapBE32(engine_egaRGBColorTable[colorLoopVar] << 8);
			}
		break;
#endif
	case OUTPUTSYS_SURFACE:
	{
		uint32_t mappedRGBVal;
		for (colorLoopVar = 0; colorLoopVar < 16; colorLoopVar++) {
			mappedRGBVal = SDL_MapRGB(engine_screen.sdl.windowSurface->format, engine_egaRGBColorTable[colorLoopVar] >> 16, (engine_egaRGBColorTable[colorLoopVar] >> 8) & 0xFF, engine_egaRGBColorTable[colorLoopVar] & 0xFF);
			switch (engine_screen.host.bytesPerPixel) {
				case 2:
					((uint16_t *)engine_screen.host.colorTable)[colorLoopVar] = mappedRGBVal;
					break;
				case 4:
					((uint32_t *)engine_screen.host.colorTable)[colorLoopVar] = mappedRGBVal;
					break;
				default:
					if (engine_screen.host.bytesPerPixel < 4) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, ((uint8_t *)&mappedRGBVal)+4-engine_screen.host.bytesPerPixel, engine_screen.host.bytesPerPixel);
#else
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, &mappedRGBVal, engine_screen.host.bytesPerPixel);
#endif
					} else {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						memset(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, 0, engine_screen.host.bytesPerPixel-4);
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar+engine_screen.host.bytesPerPixel-4, &mappedRGBVal, 4);
#else
						memset(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar+4, 0, engine_screen.host.bytesPerPixel-4);
						memcpy(engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*colorLoopVar, &mappedRGBVal, 4);
#endif
					}
			}
		}
		break;
	}
	}
	// Now copy to be set as the current palette
	// ... or don't. Whenever the EGA video mode is changed,
	// we set this (along with the border color) to defaults.
	//memcpy(engine_screen.host.mappedEgaColorTable, engine_screen.host.colorTable, sizeof(engine_screen.host.colorTable));

	return true;
}

void privSetVideoModeLow(int16_t vidMode) {
	assert((vidMode == -1) || (vidMode == 3) || (vidMode == 0xD));
#if 0
	// NOTE: It is possible that egaMemoryPtr == byteEgaMemory
	// (in the 8-bit case). The following code should work anyway.

	// Fill virtual EGA display with black, which in the
	// Overlay case is not just a plain zero value,
	// but rather: Y=0, U=128, V=128.
#if !SDL_VERSION_ATLEAST(2,0,0) // SDL 1.2 ONLY!
	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OVERLAY) {
		for (int currPix = 0; currPix < sizeof(engine_screen.client.byteEgaMemory); currPix++) {
			((uint32_t *)engine_screen.host.egaMemoryPtr)[currPix] = 0x00800080;
		}
	} else
#endif
		memset(engine_screen.host.egaMemoryPtr, 0, engine_screen.host.bytesPerPixel*ENGINE_EGA_MEMORY_IN_BITS);
#endif
	//Don't yet reset border color - we also reset the palette
	//CVort_engine_setBorderColor(0);

	// Set some client-side things
	if (vidMode == 0xD) { // Graphical
		engine_screen.client.totalScanHeight = engine_arguments.calc.gfxTotalScanHeight;
		engine_screen.client.vertRetraceLen = engine_arguments.calc.gfxVertRetraceLen;

		memset(engine_screen.client.byteEgaMemory, 0, sizeof(engine_screen.client.byteEgaMemory));
	} else if (vidMode == 3) { // Textual
		engine_screen.client.totalScanHeight = engine_arguments.calc.txtTotalScanHeight;
		engine_screen.client.vertRetraceLen = engine_arguments.calc.txtVertRetraceLen;
		// Set color attribute to gray and reset cursor position
		engine_screen.client.txtCursorPosX = engine_screen.client.txtCursorPosY = 0;
		for (int currTextMemByte = 0; currTextMemByte < sizeof(engine_screen.client.egaTxtMemory); currTextMemByte += 2) {
			engine_screen.client.egaTxtMemory[currTextMemByte] = 0x20;
			engine_screen.client.egaTxtMemory[currTextMemByte+1] = 7;
		}
	}
	engine_screen.client.mouseCursorPosX = ENGINE_EGA_GFX_WIDTH;
	engine_screen.client.mouseCursorPosY = ENGINE_EGA_GFX_HEIGHT/2;
	engine_screen.client.currVidMode = vidMode;

	engine_screen.client.currEgaStartAddr = 0;
	engine_screen.client.currPanning = 0;
	//CVort_private_engine_setEgaMemStartLocAndPanning(0, 0);
	//CVort_engine_setBorderColor(0); // Again, not yet.

	// Don't call CVort_private_engine_setPaletteAndBorderColor
	// either - maybe some host-side things are not ready.
	//CVort_private_engine_setPaletteAndBorderColor(...);

	for (int i = 0; i < 16; i++) {
		engine_screen.client.currParsedPalette[i] = i;
	}
	engine_screen.client.currParsedBorderColor = 0;
}

// This should be called AFTER video mode is set
void privUpdateClientWindowDims(void) {
	engine_screen.dims.clientRect.x = 0;
	engine_screen.dims.clientRect.y = 0;
	if (engine_screen.client.currVidMode == 0xD) { // Graphical
		engine_screen.dims.clientRect.w = ENGINE_EGA_GFX_WIDTH;
		engine_screen.dims.clientRect.h = ENGINE_EGA_GFX_HEIGHT;
		engine_screen.dims.clientBorderedWidth = ENGINE_EGA_GFX_WIDTH+engine_arguments.calc.overscanGfxLeft+engine_arguments.calc.overscanGfxRight;
		// Take VGA 200-line mode double scanning into consideration,
		// shrinking a bit each horizontal overscan border strip SEPARATELY if needed
		engine_screen.dims.clientBorderedHeight = ENGINE_EGA_GFX_HEIGHT+engine_arguments.calc.overscanGfxTop/engine_arguments.calc.gfxHeightScalingFactor+engine_arguments.calc.overscanGfxBottom/engine_arguments.calc.gfxHeightScalingFactor;
	} else if (engine_screen.client.currVidMode == 3) { // Textual
		engine_screen.dims.clientRect.w = engine_arguments.calc.txtPixelWidth;
		engine_screen.dims.clientRect.h = engine_arguments.calc.txtPixelHeight;
		engine_screen.dims.clientBorderedWidth = engine_arguments.calc.txtPixelWidth+engine_arguments.calc.overscanTxtLeft+engine_arguments.calc.overscanTxtRight;
		engine_screen.dims.clientBorderedHeight = engine_arguments.calc.txtPixelHeight+engine_arguments.calc.overscanTxtTop+engine_arguments.calc.overscanTxtBottom;
	} else { // GUI/Launcher
		engine_screen.dims.clientRect.w = ENGINE_GUI_WIDTH;
		engine_screen.dims.clientRect.h = ENGINE_GUI_HEIGHT;
		engine_screen.dims.clientBorderedWidth = ENGINE_GUI_WIDTH;
		engine_screen.dims.clientBorderedHeight = ENGINE_GUI_HEIGHT;
	}
	// Further set this
	engine_screen.dims.clientScanLineLength = (engine_screen.client.currVidMode == 0xD) ? ENGINE_EGA_GFX_SCANLINE_LEN : engine_screen.dims.clientRect.w;
	// For now, if no zoom level is manually specified, assume 2
	// for graphical/GUI mode or non-boxed scaling and 1 otherwise.
	engine_screen.dims.zoomFactor = engine_gfx_effective_arguments.zoomLevel ? engine_gfx_effective_arguments.zoomLevel
	                                : (((engine_screen.client.currVidMode != 3) || (engine_gfx_effective_arguments.scaleType != GFX_SCALE_BOXED)) ? 2 : 1);
	engine_screen.dims.clientOffsettedZoomedRect.x = engine_screen.dims.zoomFactor*((engine_screen.client.currVidMode == 0xD) ? engine_arguments.calc.overscanGfxLeft : (engine_screen.client.currVidMode == 3) ? engine_arguments.calc.overscanTxtLeft : 0);
	// Take VGA 200-line mode double scanning into consideration
	if (engine_screen.client.currVidMode == 0xD) { // Graphical
		engine_screen.dims.clientOffsettedZoomedRect.y = engine_screen.dims.zoomFactor*engine_arguments.calc.overscanGfxTop/engine_arguments.calc.gfxHeightScalingFactor;
	} else if (engine_screen.client.currVidMode == 3) { // Textual
		engine_screen.dims.clientOffsettedZoomedRect.y = engine_screen.dims.zoomFactor*engine_arguments.calc.overscanTxtTop;
	} else { // GUI/Launcher
		engine_screen.dims.clientOffsettedZoomedRect.y = 0;
	}
	engine_screen.dims.clientOffsettedZoomedRect.w = engine_screen.dims.zoomFactor*engine_screen.dims.clientRect.w;
	engine_screen.dims.clientOffsettedZoomedRect.h = engine_screen.dims.zoomFactor*engine_screen.dims.clientRect.h;
	engine_screen.dims.clientZoomedBorderedWidth = engine_screen.dims.zoomFactor*engine_screen.dims.clientBorderedWidth;
	// Take VGA 200-line mode double scanning into consideration,
	// shrinking a bit each horizontal overscan border strip SEPARATELY if needed
	if (engine_screen.client.currVidMode == 0xD) { // Graphical
		engine_screen.dims.clientZoomedBorderedHeight = engine_screen.dims.zoomFactor*ENGINE_EGA_GFX_HEIGHT+engine_screen.dims.zoomFactor*engine_arguments.calc.overscanGfxTop/engine_arguments.calc.gfxHeightScalingFactor+engine_screen.dims.zoomFactor*engine_arguments.calc.overscanGfxBottom/engine_arguments.calc.gfxHeightScalingFactor;
	} else { // Textual or GUI/Launcher
		engine_screen.dims.clientZoomedBorderedHeight = engine_screen.dims.zoomFactor*engine_screen.dims.clientBorderedHeight;
	}
	/* Used for aspect-correction to about 4:3:
	 * The exact ratio is probably different from 4:3, since the actual
	 * contents, *without* the overscan border, should have that ratio.
	 */
	engine_screen.dims.borderedRatioHoriz = 4*engine_screen.dims.clientRect.h*engine_screen.dims.clientZoomedBorderedWidth;
	engine_screen.dims.borderedRatioVert = 3*engine_screen.dims.clientRect.w*engine_screen.dims.clientZoomedBorderedHeight;
}

void privUpdateDesiredHostWindowDims(void) {
	/* Aspect-correct the window dimensions if desired;
	 * There is (usually) no need for these bars if it is not
	 * a full screen! Well, unless -forcecutfullscreen is specified.
	 * Similarly change the window dimensions if no scaling is done at all.
	 *
	 * But the exact ratio is probably different from 4:3, since the actual
	 * contents, *without* the overscan border, should have that ratio.
	 */

	// Including the overscan border, the ratio is borderedRatioHoriz:borderedRatioVert.
	// But first we (re)calculate the user's (or default) choice of window dimensions.
	if (!engine_gfx_effective_arguments.windowWidth || !engine_gfx_effective_arguments.windowHeight) {
		// Is there really a need to involve
		// the golden ratio here? Hmm...
		engine_screen.host.winWidth = engine_screen.host.desktopWidth * 309 / 500;
		engine_screen.host.winHeight = engine_screen.host.desktopHeight * 309 / 500;
	} else {
		engine_screen.host.winWidth = engine_gfx_effective_arguments.windowWidth;
		engine_screen.host.winHeight = engine_gfx_effective_arguments.windowHeight;
	}
	// We next resize appropriately
	if (engine_gfx_effective_arguments.scaleType == GFX_SCALE_ASPECT) {
		if (engine_screen.dims.borderedRatioVert*engine_screen.host.winWidth >= engine_screen.dims.borderedRatioHoriz*engine_screen.host.winHeight) { // Wider than 4:3
			engine_screen.host.winWidth = engine_screen.dims.borderedRatioHoriz*engine_screen.host.winHeight/engine_screen.dims.borderedRatioVert;
		} else { // Thinner than 4:3
			engine_screen.host.winHeight = engine_screen.dims.borderedRatioVert*engine_screen.host.winWidth/engine_screen.dims.borderedRatioHoriz;
		}
	} else if (engine_gfx_effective_arguments.scaleType == GFX_SCALE_BOXED) {
		engine_screen.host.winWidth = engine_screen.dims.clientZoomedBorderedWidth;
		engine_screen.host.winHeight = engine_screen.dims.clientZoomedBorderedHeight;
	}
	// Similarly query desired fullscreen window dimensions
	if (!engine_gfx_effective_arguments.fullWidth || !engine_gfx_effective_arguments.fullHeight) {
		engine_screen.host.fullWidth = engine_screen.host.desktopWidth;
		engine_screen.host.fullHeight = engine_screen.host.desktopHeight;
		engine_screen.host.useFullDesktopDims = true;
	} else {
		engine_screen.host.fullWidth = engine_gfx_effective_arguments.fullWidth;
		engine_screen.host.fullHeight = engine_gfx_effective_arguments.fullHeight;
		engine_screen.host.useFullDesktopDims = false;
	}
	// Repeat for a fullscreen window if this is forced
	if (engine_arguments.doForceCutFullScreen) {
		if (engine_gfx_effective_arguments.scaleType == GFX_SCALE_ASPECT) {
			if (engine_screen.dims.borderedRatioVert*engine_screen.host.fullWidth >= engine_screen.dims.borderedRatioHoriz*engine_screen.host.fullHeight) { // Wider than 4:3
				engine_screen.host.fullWidth = engine_screen.dims.borderedRatioHoriz*engine_screen.host.fullHeight/engine_screen.dims.borderedRatioVert;
			} else { // Thinner than 4:3
				engine_screen.host.fullHeight = engine_screen.dims.borderedRatioVert*engine_screen.host.fullWidth/engine_screen.dims.borderedRatioHoriz;
			}
		} else if (engine_gfx_effective_arguments.scaleType == GFX_SCALE_BOXED) {
			engine_screen.host.fullWidth = engine_screen.dims.clientZoomedBorderedWidth;
			engine_screen.host.fullHeight = engine_screen.dims.clientZoomedBorderedHeight;
		}
	}
}

bool privResizeHostWindowFailSafe(void) {
	// There are no resources to release now, so...
	return privCreateHostWindowFailSafe(); // That's all which is really done
}

// Same as privCreateHostWindow, but used for resizing an existing window.
bool privResizeHostWindow(void) {

#if SDL_VERSION_ATLEAST(2,0,0)

	int fullScrFlags = SDL_GetWindowFlags(engine_screen.sdl.window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
	int screenWidth = fullScrFlags ? engine_screen.host.fullWidth : engine_screen.host.winWidth;
	int screenHeight = fullScrFlags ? engine_screen.host.fullHeight : engine_screen.host.winHeight;
	//SDL_SetWindowPosition(engine_screen.sdl.window, SDL_WINDOWPOS_UNDEFINED_DISPLAY(engine_arguments.displayNumber), SDL_WINDOWPOS_UNDEFINED_DISPLAY(engine_arguments.displayNumber));
	switch (fullScrFlags) {
	case 0:
		SDL_SetWindowSize(engine_screen.sdl.window, screenWidth, screenHeight);
		break;
	case SDL_WINDOW_FULLSCREEN:
	{
		SDL_DisplayMode dispMode;
		SDL_GetWindowDisplayMode(engine_screen.sdl.window, &dispMode);
		if ((dispMode.w != screenWidth) || (dispMode.h != screenHeight)) {
			// HACK
			SDL_SetWindowFullscreen(engine_screen.sdl.window, 0);
			dispMode.w = screenWidth;
			dispMode.h = screenHeight;
			SDL_SetWindowDisplayMode(engine_screen.sdl.window, &dispMode);
			SDL_SetWindowFullscreen(engine_screen.sdl.window, SDL_WINDOW_FULLSCREEN);
		}
		break;
	}
	default: // Do nothing for SDL_WINDOW_FULLSCREEN_DESKTOP
		;
	}

	if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_SURFACE) {
		privResetBorderColor();
		//engine_screen.sdl.windowSurface = SDL_GetWindowSurface(engine_screen.sdl.window);
	}

#else // SDL 1.2

	int fullScrFlags = engine_screen.sdl.windowSurface->flags & SDL_FULLSCREEN;
	int screenWidth = fullScrFlags ? engine_screen.host.fullWidth : engine_screen.host.winWidth;
	int screenHeight = fullScrFlags ? engine_screen.host.fullHeight : engine_screen.host.winHeight;

	engine_screen.sdl.windowSurface = SDL_SetVideoMode(screenWidth, screenHeight, engine_screen.sdl.videoInfo->vfmt->BitsPerPixel, engine_screen.sdl.windowSurface->flags);
	if (!engine_screen.sdl.windowSurface) {
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
		if (engine_gfx_effective_arguments.outputSystem == OUTPUTSYS_OPENGL) {
			CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't set video mode: %s\n"
			                                                   "Reverting to software surface.\n", SDL_GetError());
			return privResizeHostWindowFailSafe();
		}
#endif
		CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't set video mode: %s\n", SDL_GetError());
		CVort_engine_shutdownSDL();
		return false;
	}
#endif
	return true;
}

bool CVort_engine_resetWindow(void) {
	privDestroyWindowResources();
	privDestroyHostWindow();

	if (engine_screen.client.currVidMode == -1) {
		// Use safer defaults
		engine_gfx_effective_arguments.fullWidth = engine_gfx_effective_arguments.fullHeight = 0;
		engine_gfx_effective_arguments.windowWidth = engine_gfx_effective_arguments.windowHeight = 0;
		engine_gfx_effective_arguments.outputSystem = CHOCOLATE_KEEN_LAUNCHER_UI_OUTPUTSYS;
		engine_gfx_effective_arguments.isFullscreen = engine_isLauncherFullscreen;
#if SDL_VERSION_ATLEAST(2,0,0)
		engine_gfx_effective_arguments.rendererDriverIndex = -1;
		engine_gfx_effective_arguments.scaleType = GFX_SCALE_ASPECT;
#else
		engine_gfx_effective_arguments.scaleType = GFX_SCALE_BOXED;
#endif
		engine_gfx_effective_arguments.zoomLevel = 2;
#if SDL_VERSION_ATLEAST(2,0,0)
		engine_gfx_effective_arguments.vSync = true; // Looks better with a given renderer
#else
		engine_gfx_effective_arguments.vSync = false; // Create a simple software surface
#endif
		engine_gfx_effective_arguments.bilinearInterpolation = true; // Just to be somewhat consistent...
		engine_gfx_effective_arguments.offScreenRendering = false;
	} else {
		engine_gfx_effective_arguments.fullWidth = engine_arguments.fullWidth;
		engine_gfx_effective_arguments.fullHeight = engine_arguments.fullHeight;
		engine_gfx_effective_arguments.windowWidth = engine_arguments.windowWidth;
		engine_gfx_effective_arguments.windowHeight = engine_arguments.windowHeight;
		engine_gfx_effective_arguments.outputSystem = engine_arguments.outputSystem;
		engine_gfx_effective_arguments.isFullscreen = engine_arguments.isFullscreen;
#if SDL_VERSION_ATLEAST(2,0,0)
		engine_gfx_effective_arguments.rendererDriverIndex = engine_arguments.rendererDriverIndex;
#endif
		engine_gfx_effective_arguments.scaleType = engine_arguments.scaleType;
		engine_gfx_effective_arguments.zoomLevel = engine_arguments.zoomLevel;
		engine_gfx_effective_arguments.vSync = engine_arguments.vSync;
		engine_gfx_effective_arguments.bilinearInterpolation = engine_arguments.bilinearInterpolation;
		engine_gfx_effective_arguments.offScreenRendering = engine_arguments.offScreenRendering;
	}

	privUpdateClientWindowDims(); // Maybe the zoom factor should be reset?
	privUpdateDesiredHostWindowDims(); // Resolution, scale type stuff?
	if (!(privCreateHostWindow() && privCreateWindowResources()))
		return false;
	CVort_engine_prepareWindowRects((engine_gfx_effective_arguments.scaleType != GFX_SCALE_FILL) && engine_gfx_effective_arguments.isFullscreen && !engine_arguments.doForceCutFullScreen);
	privResetBorderColor();
	privResetPalette();
	privResetEgaMemStartLocAndPanning();
	CVort_engine_toggleCursorLock((engine_screen.client.currVidMode != -1) && engine_gfx_effective_arguments.isFullscreen && engine_arguments.cursorAutoLock);
	return true;
}

bool CVort_engine_setVideoMode(int16_t vidMode) {
	static bool isFirstTime = true;
	// If it's either our very first time, or we change from/to
	// the launcher UI, then (re)create the window
	if (isFirstTime || ((vidMode != engine_screen.client.currVidMode) && ((vidMode == -1) || (engine_screen.client.currVidMode == -1)))) {
		isFirstTime = false;
		privSetVideoModeLow(vidMode);
		return CVort_engine_resetWindow();
	}
	// Not on first time
	privDestroyWindowResources();
	// Same as first time
	privSetVideoModeLow(vidMode);
	privUpdateClientWindowDims();
	privUpdateDesiredHostWindowDims();
	// Similar to CVort_engine_resetWindow after destroying resources+window
	// but with privResizeHostWindow rather than privCreateHostWindow
	if (!(privResizeHostWindow() && privCreateWindowResources()))
		return false;
	CVort_engine_prepareWindowRects((engine_gfx_effective_arguments.scaleType != GFX_SCALE_FILL) && engine_gfx_effective_arguments.isFullscreen && !engine_arguments.doForceCutFullScreen);
	privResetBorderColor();
	privResetPalette();
	privResetEgaMemStartLocAndPanning();
	// If everything continues as expeted it's better to NOT make call that
	// (Snap fullscreen window to the left on Ubuntu 12.04/14.04 with Unity
	// during gameplay, then unlock the cursor and finally quit to text
	// mode, only to get a (maximized) fullscreen windows again)
	//CVort_engine_toggleCursorLock((engine_screen.client.currVidMode != -1) && engine_gfx_effective_arguments.isFullscreen && engine_arguments.cursorAutoLock);
	return true;
}


#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_

int privGetPowerOfTwoCeiling(int num) {
    int exponent = 0;
    while (num > 0) {
        num >>= 1;
        exponent++;
    }
    return (1 << exponent);
}

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
GLuint privLoadGLShader(GLenum type, const char *src) {
    GLint compiled;
    GLuint shader = engine_glCreateShader(type);
    if (shader == 0) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create shader.\n");
    	return 0;
    }

    engine_glShaderSource(shader, 1, &src, NULL);
    engine_glCompileShader(shader);
    engine_glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLogLen = 0;
        engine_glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 1) {
            char* infoLog = (char *)malloc(sizeof(char) * infoLogLen);
            engine_glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Error compiling shader:\n%s\n", infoLog);
            free(infoLog);
        } else {
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Unknown error compiling shader:\n");
        }
        engine_glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool privPrepareGLShaderProgram(void) {
    char vShaderSrc[] =  
    "attribute vec4 a_position;\n"
    "attribute vec2 a_texCoord;\n"
    "varying vec2 v_texCoord;\n"

    "void main() {\n"
    "    gl_Position = a_position;\n"
    "    v_texCoord = a_texCoord;\n"
    "}\n";
   
    char fShaderSrc[] =  
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "varying vec2 v_texCoord;\n"
    "uniform sampler2D s_texture;\n"
    "uniform sampler2D s_palette;\n"
    "uniform int i_ispaletted;\n"
    "void main() {\n"
    "    gl_FragColor = (i_ispaletted == 1) ? texture2D(s_palette, vec2(texture2D(s_texture, v_texCoord).a*16.0, 0.0)) : texture2D(s_texture, v_texCoord);\n"
    "}\n";

    GLuint vertexShader;
    GLuint fragmentShader, palCyclingFragmentShader;
    GLuint shaderProgram;
    GLint linked;

    vertexShader = privLoadGLShader(GL_VERTEX_SHADER, vShaderSrc);
    if (!vertexShader)
        return false;

    fragmentShader = privLoadGLShader(GL_FRAGMENT_SHADER, fShaderSrc);
    if (!fragmentShader) {
        engine_glDeleteShader(vertexShader);
        return false;
    }

    shaderProgram = engine_glCreateProgram();
    if (!shaderProgram) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Couldn't create shader program.\n");
        engine_glDeleteShader(vertexShader);
        engine_glDeleteShader(fragmentShader);
        return false;
    }
    engine_glAttachShader(shaderProgram, vertexShader);
    engine_glAttachShader(shaderProgram, fragmentShader);

    engine_glLinkProgram(shaderProgram);
    engine_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLogLen = 0;
        engine_glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 1) {
            char* infoLog = (char *)malloc(sizeof(char) * infoLogLen);
            engine_glGetProgramInfoLog(shaderProgram, infoLogLen, NULL, infoLog);
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        engine_glDeleteProgram(shaderProgram);
        engine_glDeleteShader(vertexShader);
        engine_glDeleteShader(fragmentShader);
        return false;
    }
    engine_screen.gl.vertexShader = vertexShader;
    engine_screen.gl.fragmentShader = fragmentShader;
    engine_screen.gl.shaderProgram = shaderProgram;
    engine_screen.gl.positionLoc = engine_glGetAttribLocation(shaderProgram, "a_position");
    engine_screen.gl.texCoordLoc = engine_glGetAttribLocation(shaderProgram, "a_texCoord");
    engine_screen.gl.samplerLoc = engine_glGetUniformLocation(shaderProgram, "s_texture");
    engine_screen.gl.palSamplerLoc = engine_glGetUniformLocation(shaderProgram, "s_palette");
    engine_screen.gl.palCyclingIntLoc = engine_glGetUniformLocation(shaderProgram, "i_ispaletted");
    return true;
}
#endif // _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_

// Returns true on success, prints error and returns false otherwise
bool CVort_engine_preparegl() {
    int potWidth, potHeight, subImageWidth, subImageHeight;
    GLfloat vertexLeftPos = (2.0f*engine_screen.dims.clientOffsettedZoomedRect.x)/engine_screen.dims.clientZoomedBorderedWidth-1.0f,
            vertexRightPos = (2.0f*((float)engine_screen.dims.clientOffsettedZoomedRect.x+engine_screen.dims.clientOffsettedZoomedRect.w)/engine_screen.dims.clientZoomedBorderedWidth)-1.0f,
            // Here, screen top == -1.0f, bottom == 1.0f (while in GL it's "naturally" the opposite)
            vertexTopPos = (2.0f*engine_screen.dims.clientOffsettedZoomedRect.y)/engine_screen.dims.clientZoomedBorderedHeight-1.0f,
            vertexBottomPos = (2.0f*((float)engine_screen.dims.clientOffsettedZoomedRect.y+engine_screen.dims.clientOffsettedZoomedRect.h)/engine_screen.dims.clientZoomedBorderedHeight)-1.0f;

    engine_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
        engine_glMatrixMode(GL_PROJECTION);
        engine_glLoadIdentity();
        engine_glMatrixMode(GL_MODELVIEW);
        engine_glLoadIdentity();
    }
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
        if (!privPrepareGLShaderProgram()) {
            return false;
        }
        engine_glUseProgram(engine_screen.gl.shaderProgram);
    }
    engine_screen.gl.texturePixFormat = engine_screen.gl.gpuPaletteCycling ? GL_ALPHA : GL_RGBA;
#else
    engine_screen.gl.texturePixFormat = GL_RGBA;
#endif

    // Should be at least 64 everywhere... so a 16 (17?) colors palette has room
    GLint maxTexSize;
    engine_glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

    // Use FBO, *but* only if bilinear interpolation is requested.
    if (engine_screen.host.bilinearInterpolation && engine_screen.gl.offScreenRendering) {
        // Create framebuffer
        engine_glGenFramebuffers_CoreEXT(1, &engine_screen.gl.frameBufferName);
        //engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, engine_screen.gl.frameBufferName);
        // Generate a pair of textures
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
        if (engine_screen.gl.gpuPaletteCycling) {
            engine_glGenTextures(3, engine_screen.gl.textureNames);
            engine_screen.gl.palTextureName = &engine_screen.gl.textureNames[2];
        } else {
            engine_glGenTextures(2, engine_screen.gl.textureNames);
        }
#else
        engine_glGenTextures(2, engine_screen.gl.textureNames);
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
        // Similarly generate a pair of call lists
        if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
            engine_screen.gl.numOfCallLists = 2;
            engine_screen.gl.outputCallListName = engine_glGenLists(engine_screen.gl.numOfCallLists);
        }
#endif
        // NOTE: We are creating textures and call list in "reversed" order:
        // First the texture to render to, then the usual texture.

        // Second texture should have the basic zoomed image, including the
        // overscan borders.
        engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[1]);
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        subImageWidth = engine_screen.dims.clientZoomedBorderedWidth;
        subImageHeight = engine_screen.dims.clientZoomedBorderedHeight;
        potWidth = privGetPowerOfTwoCeiling(subImageWidth);
        potHeight = privGetPowerOfTwoCeiling(subImageHeight);
        if ((potWidth > maxTexSize) || (potHeight > maxTexSize)) {
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Maximum OpenGL texture size exceeded.\n");
            engine_glDeleteFramebuffers_CoreEXT(1, &engine_screen.gl.frameBufferName);
            engine_screen.gl.frameBufferName = 0;
            engine_glDeleteTextures(2, engine_screen.gl.textureNames);
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
            if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1)
                engine_glDeleteLists(engine_screen.gl.outputCallListName, engine_screen.gl.numOfCallLists);
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
            if (engine_screen.gl.gpuPaletteCycling)
                engine_glDeleteTextures(1, engine_screen.gl.palTextureName);
            if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0)
                privClearGLShaderResources();
#endif
            return false;
        }
        // This one is ALWAYS GL_RGBA (even as a target of palette cycling)
        engine_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, potWidth, potHeight,
                            0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        // Try to configure framebuffer
        //engine_glActiveTexture(GL_TEXTURE0);
        engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[1]);
        engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, engine_screen.gl.frameBufferName);
        engine_glFramebufferTexture2D_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, ENGINE_GL_COLOR_ATTACHMENT0_COREEXT, GL_TEXTURE_2D, engine_screen.gl.textureNames[1], 0);
        // Set list of draw buffers
        /*GLenum drawBuffers[1] = {ENGINE_GL_COLOR_ATTACHMENT0_COREEXT};
        engine_glDrawBuffers(1, drawBuffers);*/
	// Or just a single one
	//engine_glDrawBuffer(ENGINE_GL_COLOR_ATTACHMENT0_COREEXT);
        // Maybe there is an error
        if (engine_glCheckFramebufferStatus_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT) != ENGINE_GL_FRAMEBUFFER_COMPLETE_COREEXT) {
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Framebuffer configuration has failed.\n");
            engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, 0);
            engine_glDeleteFramebuffers_CoreEXT(1, &engine_screen.gl.frameBufferName);
            engine_glDeleteTextures(2, engine_screen.gl.textureNames);
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
            if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1)
                engine_glDeleteLists(engine_screen.gl.outputCallListName, engine_screen.gl.numOfCallLists);
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
            if (engine_screen.gl.gpuPaletteCycling)
                engine_glDeleteTextures(1, engine_screen.gl.palTextureName);
            if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0)
                privClearGLShaderResources();
#endif
            return false;
        }

	if (!engine_screen.gl.haveFramebufferBlit) {
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
            // Alright, we record the rendering to screen!
	    // ...But only if we CANNOT blit framebuffer to screen directly.
            if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
                engine_glNewList(1+engine_screen.gl.outputCallListName, GL_COMPILE);
                engine_glClear(GL_COLOR_BUFFER_BIT); // Clear window

                engine_glEnable(GL_TEXTURE_2D);
                engine_glBegin(GL_TRIANGLE_STRIP);
                // We should **NOT** invert the vertex y coordinates here!
                engine_glTexCoord2f(0.0f, 0.0f);
                engine_glVertex3f(-1.0f, -1.0f, 0.0f);
                engine_glTexCoord2f(((float) subImageWidth) / potWidth, 0.0f);
                engine_glVertex3f(1.0f, -1.0f, 0.0f);
                engine_glTexCoord2f(0.0f, ((float) subImageHeight) / potHeight);
                engine_glVertex3f(-1.0f, 1.0f, 0.0f);
                engine_glTexCoord2f(((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight);
                engine_glVertex3f(1.0f, 1.0f, 0.0f);
                engine_glEnd();
                engine_glDisable(GL_TEXTURE_2D);

                engine_glEndList();
            }
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
            if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
                // We should **NOT** invert the vertex y coordinates here!
                GLfloat ourPositionsBuffer[] = {
                    -1.0f, -1.0f,  0.0f,
                     0.0f,                               0.0f,
                     1.0f, -1.0f,  0.0f,
                     ((float) subImageWidth) / potWidth, 0.0f,
                    -1.0f,  1.0f,  0.0f,
                     0.0f,                               ((float) subImageHeight) / potHeight,
                     1.0f,  1.0f,  0.0f,
                     ((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight
                };
                // A secondary buffer used only if we don't do a framebuffer blit
                memcpy(engine_screen.gl.secondaryPositionsBuffer, ourPositionsBuffer, sizeof(ourPositionsBuffer));
            }
#endif
	}

        /*** Back to the first texture */

        // First texture should have the basic non-zoomed image.
        // Here, the CLIENT SCANLINE LENGTH is used for the width.
        engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[0]);
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        subImageWidth = engine_screen.dims.clientRect.w;
        subImageHeight = engine_screen.dims.clientRect.h;
        // NOTE: Again, the SCANLINE length is used here.
        // In practice it should not make a great difference...

	// HACK: There's no need to compare to the the maximum texture size
	// again. (The scanline length is 384 so the corresponding pot width is
	// 512, also required with the non-zoomed/bordered width of 320.)
        potWidth = privGetPowerOfTwoCeiling(engine_screen.dims.clientScanLineLength);
        potHeight = privGetPowerOfTwoCeiling(subImageHeight);
        engine_glTexImage2D(GL_TEXTURE_2D, 0, engine_screen.gl.texturePixFormat, potWidth, potHeight,
                            0, engine_screen.gl.texturePixFormat, GL_UNSIGNED_BYTE, 0);

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
        // Let us record the rendering to the FBO now!
        if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
            engine_glNewList(engine_screen.gl.outputCallListName, GL_COMPILE);
	    engine_glPushAttrib(GL_VIEWPORT_BIT); // Store window viewport
            engine_glViewport(0, 0, engine_screen.dims.clientZoomedBorderedWidth, engine_screen.dims.clientZoomedBorderedHeight);
            // We do NOT clear the FBO here.

            engine_glEnable(GL_TEXTURE_2D);
            engine_glBegin(GL_TRIANGLE_STRIP);
            // This time we DO invert the vertex y coordinates.
            // Furthermore, the overscan borders should be ignored.
            engine_glTexCoord2f(0.0f, 0.0f);
            engine_glVertex3f(vertexLeftPos, -vertexTopPos, 0.0f);
            engine_glTexCoord2f(((float) subImageWidth) / potWidth, 0.0f);
            engine_glVertex3f(vertexRightPos, -vertexTopPos, 0.0f);
            engine_glTexCoord2f(0.0f, ((float) subImageHeight) / potHeight);
            engine_glVertex3f(vertexLeftPos, -vertexBottomPos, 0.0f);
            engine_glTexCoord2f(((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight);
            engine_glVertex3f(vertexRightPos, -vertexBottomPos, 0.0f);

            engine_glEnd();
            engine_glDisable(GL_TEXTURE_2D);

	    engine_glPopAttrib(); // Restore window viewport

            engine_glEndList();
        }
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
        if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
            // Store these for rendering to FBO

            // This time we DO invert the vertex y coordinates.
            // Furthermore, the overscan borders should be ignored.
            GLfloat ourPositionsBuffer[] = {
                  vertexLeftPos,    -vertexTopPos, 0.0f,
                 0.0f,                               0.0f,
                 vertexRightPos,    -vertexTopPos, 0.0f,
                 ((float) subImageWidth) / potWidth, 0.0f,
                  vertexLeftPos, -vertexBottomPos, 0.0f,
                 0.0f,                               ((float) subImageHeight) / potHeight,
                 vertexRightPos, -vertexBottomPos, 0.0f,
                 ((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight
            };
            memcpy(engine_screen.gl.positionsBuffer, ourPositionsBuffer, sizeof(ourPositionsBuffer));
            // We also use these first (if not only these)
            engine_glVertexAttribPointer(engine_screen.gl.positionLoc, 3, GL_FLOAT, 
                                         GL_FALSE, 5*sizeof(GLfloat), engine_screen.gl.positionsBuffer);
            engine_glVertexAttribPointer(engine_screen.gl.texCoordLoc, 2, GL_FLOAT,
                                         GL_FALSE, 5*sizeof(GLfloat), &engine_screen.gl.positionsBuffer[3]);
            engine_glEnableVertexAttribArray(engine_screen.gl.positionLoc);
            engine_glEnableVertexAttribArray(engine_screen.gl.texCoordLoc);
            // Further set the texture sampler uniform's value to 0
            engine_glUniform1i(engine_screen.gl.samplerLoc, 0);
            // Similarly, just in case, set this to 1
            engine_glUniform1i(engine_screen.gl.palSamplerLoc, 1);
            // Set to 1 if and only if palette cycling is enabled
            // (may be temporarily set to 0 if two texture quad draws are done)
            engine_glUniform1i(engine_screen.gl.palCyclingIntLoc, engine_screen.gl.gpuPaletteCycling ? 1 : 0);

            // Let's finish with the palette texture if desired
            // (at least 2 texture units should be supported)
            if (engine_screen.gl.gpuPaletteCycling) {
                engine_glActiveTexture(GL_TEXTURE0 + 1);
                engine_glBindTexture(GL_TEXTURE_2D, *engine_screen.gl.palTextureName);
                // MUST use GL_NEAREST
                engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                engine_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 1,
                                    0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                // Restore
                engine_glActiveTexture(GL_TEXTURE0 + 0);
            }
        }
#endif
        return true;
    }

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    if (engine_screen.gl.gpuPaletteCycling) {
        engine_glGenTextures(2, engine_screen.gl.textureNames);
        engine_screen.gl.palTextureName = &engine_screen.gl.textureNames[1];
    } else {
        engine_glGenTextures(1, engine_screen.gl.textureNames);
    }
#else
    engine_glGenTextures(1, engine_screen.gl.textureNames);
#endif
    engine_glBindTexture(GL_TEXTURE_2D, engine_screen.gl.textureNames[0]);

    // Use a POT (Power-Of-Two) sized texture, just in case this is
    // necessary. 2D subimages generated later are NOT POT-sized, though.
    if (engine_screen.host.bilinearInterpolation) {
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // The overscan borders are contained
        subImageWidth = engine_screen.dims.clientZoomedBorderedWidth;
        subImageHeight = engine_screen.dims.clientZoomedBorderedHeight;
        // That's expected for now...
        potWidth = privGetPowerOfTwoCeiling(subImageWidth);
    } else {
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // The overscan border are NOT added here
        subImageWidth = engine_screen.dims.clientRect.w;
        subImageHeight = engine_screen.dims.clientRect.h;
        // NOTE: As before, the SCANLINE length is used here. And again,
        // practically it should not make a great difference...
        potWidth = privGetPowerOfTwoCeiling(engine_screen.dims.clientScanLineLength);
    }
    // Don't forget this!!
    potHeight = privGetPowerOfTwoCeiling(subImageHeight);

    if ((potWidth > maxTexSize) || (potHeight > maxTexSize)) {
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_ERROR, "Maximum OpenGL texture size exceeded.\n");
        engine_glDeleteTextures(1, engine_screen.gl.textureNames);
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
        if (engine_screen.gl.gpuPaletteCycling)
            engine_glDeleteTextures(1, engine_screen.gl.palTextureName);
        if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0)
            privClearGLShaderResources();
#endif
        return false;
    }
    engine_glTexImage2D(GL_TEXTURE_2D, 0, engine_screen.gl.texturePixFormat, potWidth, potHeight,
                        0, engine_screen.gl.texturePixFormat, GL_UNSIGNED_BYTE, 0);

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
    if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_1_1) {
        engine_screen.gl.numOfCallLists = 1;
        engine_screen.gl.outputCallListName = engine_glGenLists(engine_screen.gl.numOfCallLists);
        // Render to screen.
        engine_glNewList(engine_screen.gl.outputCallListName, GL_COMPILE);
        // Do NOT clear otherwise, since we need to apply TWO clears
        // (one for the whole viewport and the other for the overscan border.)
        if (engine_screen.host.bilinearInterpolation) {
            engine_glClear(GL_COLOR_BUFFER_BIT);
        }
        engine_glEnable(GL_TEXTURE_2D);
        engine_glBegin(GL_TRIANGLE_STRIP);
        // We should INVERT the vertex y coordinates here!
        if (engine_screen.host.bilinearInterpolation) { // Overscan is inside texture
            engine_glTexCoord2f(0.0f, 0.0f);
            engine_glVertex3f(-1.0f, 1.0f, 0.0f);
            engine_glTexCoord2f(((float) subImageWidth) / potWidth, 0.0f);
            engine_glVertex3f(1.0f, 1.0f, 0.0f);
            engine_glTexCoord2f(0.0f, ((float) subImageHeight) / potHeight);
            engine_glVertex3f(-1.0f, -1.0f, 0.0f);
            engine_glTexCoord2f(((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight);
            engine_glVertex3f(1.0f, -1.0f, 0.0f);
        } else { // Overscan is NOT in, but, again we INVERT the y coordinates
            engine_glTexCoord2f(0.0f, 0.0f);
            engine_glVertex3f(vertexLeftPos, -vertexTopPos, 0.0f);
            engine_glTexCoord2f(((float) subImageWidth) / potWidth, 0.0f);
            engine_glVertex3f(vertexRightPos, -vertexTopPos, 0.0f);
            engine_glTexCoord2f(0.0f, ((float) subImageHeight) / potHeight);
            engine_glVertex3f(vertexLeftPos, -vertexBottomPos, 0.0f);
            engine_glTexCoord2f(((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight);
            engine_glVertex3f(vertexRightPos, -vertexBottomPos, 0.0f);
        }
        engine_glEnd();
        engine_glDisable(GL_TEXTURE_2D);

        engine_glEndList();
    }
#endif
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
    if (engine_screen.gl.outputGLClass == OUTPUTGL_CLASS_2_0) {
        // We should INVERT the vertex y coordinates here!
        if (engine_screen.host.bilinearInterpolation) { // Overscan is inside texture
            GLfloat ourPositionsBuffer_Bilinear[] = {
                -1.0f,  1.0f, 0.0f,
                 0.0f,                               0.0f,
                 1.0f,  1.0f, 0.0f,
                 ((float) subImageWidth) / potWidth, 0.0f,
                -1.0f, -1.0f, 0.0f,
                 0.0f,                               ((float) subImageHeight) / potHeight,
                 1.0f, -1.0f, 0.0f,
                 ((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight
            };
            memcpy(engine_screen.gl.positionsBuffer, ourPositionsBuffer_Bilinear, sizeof(ourPositionsBuffer_Bilinear));
        } else { // Overscan is NOT in, but, again we INVERT the y coordinates
            GLfloat ourPositionsBuffer_Nearest[] = {
                  vertexLeftPos,    -vertexTopPos, 0.0f,
                 0.0f,                               0.0f,
                 vertexRightPos,    -vertexTopPos, 0.0f,
                 ((float) subImageWidth) / potWidth, 0.0f,
                  vertexLeftPos, -vertexBottomPos, 0.0f,
                 0.0f,                               ((float) subImageHeight) / potHeight,
                 vertexRightPos, -vertexBottomPos, 0.0f,
                 ((float) subImageWidth) / potWidth, ((float) subImageHeight) / potHeight
            };
            memcpy(engine_screen.gl.positionsBuffer, ourPositionsBuffer_Nearest, sizeof(ourPositionsBuffer_Nearest));
        }
        // Prepare the vertex data for now
        engine_glVertexAttribPointer(engine_screen.gl.positionLoc, 3, GL_FLOAT, 
                                     GL_FALSE, 5*sizeof(GLfloat), engine_screen.gl.positionsBuffer);

        engine_glVertexAttribPointer(engine_screen.gl.texCoordLoc, 2, GL_FLOAT,
                                     GL_FALSE, 5*sizeof(GLfloat), &engine_screen.gl.positionsBuffer[3]);
        engine_glEnableVertexAttribArray(engine_screen.gl.positionLoc);
        engine_glEnableVertexAttribArray(engine_screen.gl.texCoordLoc);
        // Further set the texture sampler uniform's value to 0
        engine_glUniform1i(engine_screen.gl.samplerLoc, 0);
        // Similarly, just in case, set this to 1
        engine_glUniform1i(engine_screen.gl.palSamplerLoc, 1);
        // Set to 1 if and only if palette cycling is enabled
        engine_glUniform1i(engine_screen.gl.palCyclingIntLoc, engine_screen.gl.gpuPaletteCycling ? 1 : 0);
        // Let's finish with the palette texture if desired
        // (at least 2 texture units should be supported)
        if (engine_screen.gl.gpuPaletteCycling) {
            engine_glActiveTexture(GL_TEXTURE0 + 1);
            engine_glBindTexture(GL_TEXTURE_2D, *engine_screen.gl.palTextureName);
            // MUST use GL_NEAREST
            engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            engine_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            engine_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 1,
                                0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            // Restore
            engine_glActiveTexture(GL_TEXTURE0 + 0);
        }
    }
#endif

    return true;
}
#endif

#if 0
void CVort_engine_updatePanning(uint16_t panning)
{
	engine_panning = panning;
}
#endif
bool CVort_engine_isVerticalBlank()
{
	// Here we think of a refresh rate of 70Hz, and each frame is measured
	// by 1000 "time units". A portion of it is used for VBlank.
	// Actually, the number "1000" appears TWICE: Once as a scale factor
	// for the rate (which is not an exact integer otherwise), and again to
	// consider the time units used by SDL_GetTicks.
	return ((Uint64)(engine_arguments.calc.scaledRefreshRate*SDL_GetTicks()/ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR)%1000 <= 1000*engine_screen.client.vertRetraceLen/engine_screen.client.totalScanHeight);
#if 0
	return (((Uint64)(engine_arguments.calc.scaledRefreshRate)*SDL_GetTicks()/ENGINE_EGAVGA_REFRESHRATE_SCALE_FACTOR)%1000 <= ENGINE_EGA_VBLANKCAP);
#endif
}

void CVort_engine_copyToTxtMemory(uint8_t *buffer) {
	memmove(engine_screen.client.egaTxtMemory, buffer, sizeof(engine_screen.client.egaTxtMemory));
	CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr(0, 0, ENGINE_EGAVGA_TXT_COLS_NUM, ENGINE_EGAVGA_TXT_ROWS_NUM);
	engine_isFrameReadyToDisplay = true;
}

// TODO: Vanilla code seems to do some more things...?
void CVort_engine_drawChar(uint16_t x, uint16_t y, uint16_t val)
{
	// Multiply by 8, since we measure in pixels of 1 byte for each,
	// rather than 1 bit as done in the original code for every EGA plane.
	x <<= 3;

	uint8_t *fontPixelPtr = engine_egaFonts[val];
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN;

	uint16_t currY;
	for (currY = 0; currY < 8; currY++)
	{
		memcpy(byteEgaPtr, fontPixelPtr, 8);
		fontPixelPtr += 8;
		byteEgaPtr += ENGINE_EGA_GFX_SCANLINE_LEN;
	}
	CVort_engine_updateEgaGfxNonPalRect_ptr(engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN, 8, 8);
	engine_isFrameReadyToDisplay = true;
}

void CVort_engine_drawSprite(uint16_t x, uint16_t y, uint16_t num)
{
	if (num >= 4*engine_egaHeadGeneral.spriteNum)
		num = 0;
	// Multiply by 8 as done in engine_drawChar, BUT also apply a shift.
	x = (x << 3) + ((num & 3) << 1);
	// We should also do the same for the sprite "offset" field
	// UPDATE: Actually no, that's already taken care of in the
	// graphics decompression routine.

	uint8_t *spritePixelPtr = engine_egaSprites[num/4];
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN;

	num -= (num&3);

	uint16_t currY, currX;
	for (currY = 0; currY < engine_maskedSpriteEntry[num].height; currY++)
	{
		for (currX = 0; currX < 8*engine_maskedSpriteEntry[num].width; currX++, spritePixelPtr++, byteEgaPtr++)
		{
			/**************************************************
			For a proposed bit of EGA color (say red), take the
			current bit in EGA screen, "and" with the mask bit
			and then "or" with the desired sprite bit.
			The result (same as Vanilla): If the mask bit is
			the *only* one toggled, no change is done.
			And, if the mask bit is not toggled,
			the desired pixel is copied as-is.

			Here it's done for all 4 bits at once.
			**************************************************/
			*byteEgaPtr = (((((*spritePixelPtr) & 16) >> 4)*15)&(*byteEgaPtr))|((*spritePixelPtr) & 15);
		}
		byteEgaPtr += ENGINE_EGA_GFX_SCANLINE_LEN-currX;
	}
	CVort_engine_updateEgaGfxNonPalRect_ptr(engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN, 8*engine_maskedSpriteEntry[num].width, engine_maskedSpriteEntry[num].height);
	engine_isFrameReadyToDisplay = true;
}

void CVort_engine_drawTile(uint16_t x, uint16_t y, uint16_t num)
{
	if (((num | 0x8000) ^ 0x8000) >= engine_egaHeadGeneral.tileNum)
		num = num & 0x8000;
	x <<= 3;
	// TODO: Declare "num" as a signed int and check that "num < 0"?
	bool isMasked = (num & 0x8000);
	if (isMasked)
		// Remove the minus sign due to masking
		num = (num & 0x7FFF);
	else
		num = anim_plane[anim_plane_i][num];

	uint8_t *tilePixelPtr = engine_egaTiles[num];
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN;

	uint16_t currY, currX;
	if (isMasked)
	{
		uint8_t tileMask, tileMaskInv;
		for (currY = 0; currY < 16; currY++)
		{
			for (currX = 0; currX < 16; currX++, tilePixelPtr++, byteEgaPtr++)
			{
				//if (!(currX % 8))
				//{
					tileMask = *(tilePixelPtr+256);
					tileMaskInv = tileMask^0xFF;
				//}
				/************************************************
				For a proposed bit of EGA color (say red), take
				the current bit in EGA screen, "and" with the
				mask bit (from a following mask-only tile), then
				"and" the tile's bit with the mask's *inverse*,
				and finally "or" the two results of the "and"-s.
	
				Here it's done for all 4 bits at once.
				**************************************************/
				*byteEgaPtr = (((*byteEgaPtr)&tileMask)|((*tilePixelPtr)&tileMaskInv));
			}
			//tilePixelPtr += 16; // Skip the mask for current row!
			byteEgaPtr += ENGINE_EGA_GFX_SCANLINE_LEN-currX;
		}
	}
	else
		for (currY = 0; currY < 16; currY++)
		{
			memcpy(byteEgaPtr, tilePixelPtr, 16);
			tilePixelPtr += 16;
			byteEgaPtr += ENGINE_EGA_GFX_SCANLINE_LEN;
		}
	CVort_engine_updateEgaGfxNonPalRect_ptr(engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN, 16, 16);
	engine_isFrameReadyToDisplay = true;
}

void CVort_engine_drawBitmap(uint16_t x, uint16_t y, uint16_t num)
{
	if (num >= engine_egaHeadGeneral.bmpNum)
		num = 0;

	x <<= 3;

	uint8_t *bitmapPixelPtr = engine_egaBmps[num];
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN;

	uint16_t currY;
	for (currY = 0; currY < engine_egaHeadUnmasked[num].v; currY++)
	{
		memcpy(byteEgaPtr, bitmapPixelPtr, 8*engine_egaHeadUnmasked[num].h);
		bitmapPixelPtr += 8*engine_egaHeadUnmasked[num].h;
		byteEgaPtr += ENGINE_EGA_GFX_SCANLINE_LEN;
	}
	CVort_engine_updateEgaGfxNonPalRect_ptr(engine_dstPage+x+y*ENGINE_EGA_GFX_SCANLINE_LEN, 8*engine_egaHeadUnmasked[num].h, engine_egaHeadUnmasked[num].v);
	engine_isFrameReadyToDisplay = true;
}

uint16_t CVort_engine_drawSpriteAt(int32_t posX, int32_t posY, uint16_t frame)
{
	int16_t tileX1, tileX2, tileY1, tileY2;
	int16_t sdraw_xbyte, sdraw_yrow;
	uint16_t sdraw_scopy;
	uint16_t tileID;
	int16_t tileType;

	sdraw_xbyte = posX / 0x100 - ((scrollX / 0x100)&0xFFF0);
	sdraw_yrow  = posY / 0x100 - ((scrollY / 0x100)&0xFFF0);

	if ((sdraw_xbyte < -0x20) || (sdraw_yrow < -0x20) || (sdraw_xbyte > 0x150) || (sdraw_yrow > 0xC7))
		return 0;
	// getMSE
	sdraw_scopy = (sdraw_xbyte&7)/2+(frame<<2);
	temp_MSE = engine_maskedSpriteEntry[sdraw_scopy];

	sdraw_xbyte = (sdraw_xbyte+0x20)/8-4;
	tileX1 = sdraw_xbyte/2;
	if (tileX1 > 0x15)
		return 0;
	if (tileX1 < 0)
		tileX1 = 0;
	tileX2 = (sdraw_xbyte+temp_MSE.width-1)/2;
	if (tileX2 < 0)
		return 0;
	if (tileX2 > 0x15)
		tileX2 = 0x15;
	tileY1 = sdraw_yrow/0x10;
	if (tileY1 > 0xE) // This is not a typo. 0xE, not 0xD.
		return 0;
	if (tileY1 < 0)
		tileY1 = 0;
	tileY2 = (sdraw_yrow+temp_MSE.height-1)/0x10;
	if (tileY2 < 0)
		return 0;
	if (tileY2 > 0xD) // And yet, here we have 0xD as a bound.
		tileY2 = 0xD;

	sdraw_xbyte += 4;
	sdraw_yrow += 0x20;

	for (int16_t currY = tileY1, currX; currY <= tileY2; currY++)
		for (currX = tileX1; currX <= tileX2; currX++)
		{
			tileID = map_data_tiles[(((currY+scrollY_T)*map_width_T)&65535)+(currX+scrollX_T)];
			tileType = TILEINFO_Type[tileID];
			if (tileType >= 0)
			{
				// For ATR: Update this tile
				screentiles[screentiles_i] = currY*0x15+currX;
				screentiles_i++;
			}
			else
			{
				tiledraws[tiledraws_i].x_byte = (currX<<1)+4;
				tiledraws[tiledraws_i].y_line = (currY<<4)+0x20;
				tiledraws[tiledraws_i].tile_id = tileID;
				// Do masked
				if (tileType == -2)
				{
					tiledraws[tiledraws_i].tile_id |= 0x8000;
					// For ATR: Update this tile
					screentiles[screentiles_i] = currY*0x15+currX;
					screentiles_i++;
				}
				tiledraws_c++;
				tiledraws_i++;
			}
		}
	spritedraws[spritedraws_i].x_byte = sdraw_xbyte;
	spritedraws[spritedraws_i].y_row = sdraw_yrow;
	spritedraws[spritedraws_i].sprite_copy = sdraw_scopy;
	spritedraws_c++;
	spritedraws_i++;
	return 1;
}

uint16_t CVort_engine_drawTileAt(int32_t posX, int32_t posY, uint16_t tilenum)
{
	int16_t tileX1, tileX2, tileY1, tileY2;
	int16_t sdraw_xbyte, sdraw_yrow;
	sdraw_xbyte = posX / 0x100 - ((scrollX / 0x100)&0xFFF0);
	sdraw_yrow = posY / 0x100 - ((scrollY / 0x100)&0xFFF0);
	if ((sdraw_xbyte < -0x20) || (sdraw_yrow < -0x20))
		return 0;

	sdraw_xbyte = (sdraw_xbyte+0x20)/8-4;
	tileX1 = sdraw_xbyte/2;
	if (tileX1 > 0x15)
		return 0;
	if (tileX1 < 0)
		tileX1 = 0;
	tileX2 = (sdraw_xbyte+temp_MSE.width-1)/2;
	if (tileX2 < 0)
		return 0;
	if (tileX2 > 0x14)
		tileX2 = 0x14;
	tileY1 = sdraw_yrow/0x10;
	if (tileY1 > 0xD) // 0xD is a common bound here...
		return 0;
	if (tileY1 < 0)
		tileY1 = 0;
	tileY2 = (sdraw_yrow+temp_MSE.height-1)/0x10;
	if (tileY2 < 0)
		return 0;
	if (tileY2 > 0xD) // ...and here.
		tileY2 = 0xD;

	sdraw_xbyte += 4;
	sdraw_yrow += 0x20;

	for (int16_t currY = tileY1, currX; currY <= tileY2; currY++)
		for (currX = tileX1; currX <= tileX2; currX++)
		{
			screentiles[screentiles_i] = currY*0x15+currX;
			screentiles_i++;
		}
	tiledraws[tiledraws_i].x_byte = sdraw_xbyte;
	tiledraws[tiledraws_i].y_line = sdraw_yrow;
	tiledraws[tiledraws_i].tile_id = tilenum;
	tiledraws_c++;
	tiledraws_i++;
	return 1;
}

void CVort_engine_clearOverlay()
{	// Luckily this is endianness independent!!
	memset(ATR[0], 0xFF, 2*0x126);
	memset(ATR[1], 0xFF, 2*0x126);
}

void CVort_private_engine_setTicks(uint32_t currTicks);
uint32_t CVort_private_engine_getTicks();
void CVort_private_engine_setTicksSync(uint32_t currTicks);
uint32_t CVort_private_engine_getTicksSync();

void CVort_engine_syncDrawing() {
	tiledraws_c = bmpdraws_c = spritedraws_c = 0; // Counts = 0

	spritedraws_i = 0;
	bmpdraws_i = 0;
	tiledraws_i = 0;
	screentiles_i = 0;

        // Instead of a busy loop or even a similar loop that ends with a
        // potentially too long sleep, we use an AMORTIZED delay function.
        CVort_engine_delayInGameTicks(ticks_sync, 6);
        // Should be non-negative, in fact ~6. For demos this is FORCED to 6.
	if (engine_arguments.extras.vorticonsDemoModeToggle) {
		sprite_sync = 6;
		ticks_sync += 6;
		CVort_private_engine_setTicks(ticks_sync);
	} else {
	sprite_sync = (CVort_private_engine_getTicks()&0xFFFF)-(ticks_sync&0xFFFF);
		if (sprite_sync > 15)
			sprite_sync = 15;
		ticks_sync = CVort_private_engine_getTicks();
	}
}

void CVort_engine_drawScreen()
{
	CVort_engine_doDrawing();
	uint8_t refreshPage = engine_currPage^1;
	// For ATR:  Update these tiles
	for (int tileCounter = 0; tileCounter < screentiles_i; tileCounter++)
		ATR[refreshPage][screentiles[tileCounter]] = 0xFFFF;
}

void CVort_engine_doDrawing()
{
	CVort_engine_egaPageFlip();
	//do_drawing_0 = 0;
	anim_plane_i = (((CVort_private_engine_getTicks() & 65535) >> anim_speed) & 6) >> 1;
	if ((anim_plane_i < 0) || (anim_plane_i >= 4))
		assert(false);
	//anim_plane_i = (((CVort_private_engine_getTicks() & 65535) >> (anim_speed & 255)) & 6) >> 1;
	CVort_engine_adaptiveTileRefresh((((map_width_B * ((scrollY & 16777215) >> 12)) & 65535) >> 1) + ((scrollX & 16777215) >> 12));
	//CVort_engine_adaptiveTileRefresh(((map_width_B * ((scrollY & 16777215) >> 12)) & 65535) + (((scrollX & 16777215) >> 12) << 1));
	uint32_t origDstPage = engine_dstPage;
	engine_dstPage = engine_currPageStart;
	uint16_t loopVar, drawCounter;
	drawCounter = spritedraws_c;
	// TODO: Can't we use a single loop variable?
	// Well, not if spritedraws_c and the like are modified before the call
	// to doDrawing is done, but it doesn't look like this happens for now..
	if (drawCounter)
		for (loopVar = 0; drawCounter > 0; loopVar++, drawCounter--)
			CVort_engine_drawSprite(spritedraws[loopVar].x_byte, spritedraws[loopVar].y_row, spritedraws[loopVar].sprite_copy);
	drawCounter = tiledraws_c;
	if (drawCounter)
		for (loopVar = 0; drawCounter > 0; loopVar++, drawCounter--)
			CVort_engine_drawTile(tiledraws[loopVar].x_byte, tiledraws[loopVar].y_line, tiledraws[loopVar].tile_id);
	drawCounter = bmpdraws_c;
	if (drawCounter)
		for (loopVar = 0; drawCounter > 0; loopVar++, drawCounter--)
			CVort_engine_drawBitmap(bmpdraws[loopVar].x, bmpdraws[loopVar].y, bmpdraws[loopVar].bmp_loc);
	engine_dstPage = origDstPage;
	if (draw_func)
		(draw_func)();
		//(*this.*draw_func)();

	CVort_private_engine_setEgaMemStartLocAndPanning(engine_egaStart, pel_panning);

#if 0
	//FIXME: Wastes more CPU cycles but possibly more accurate.
	while (engine_isVerticalBlank());
	//engine_currSetEgaStart = engine_egaStart;
	while (!engine_isVerticalBlank());
#endif
#if 0
   // Taken from CVort_engine_delay - simulates waits for (emulated) vertical
   // retrace and then for the end of the retrace
   engine_momentToDelayToInScanlines -= (engine_momentToDelayToInScanlines % engine_arguments.client.totalScanHeight);
   engine_momentToDelayToInScanlines += engine_arguments.client.totalScanHeight;
#endif
#if 0
	while (engine_isVerticalBlank())
		CVort_engine_shortSleep(true);
	//engine_currSetEgaStart = engine_egaStart;
	while (!engine_isVerticalBlank())
		CVort_engine_shortSleep(true);
#endif

	//engine_screen.client.currPanning = pel_panning;

   // Now AFTER panning is set, we might update the display contents
   engine_isFrameReadyToDisplay = true;
   /* NOTE: Originally we do NOT (!!!) call the delay function. Rather,
    * we do the job of CVort_engine_delay(1) right here, directly...
    * and a bit earlier.
    * However, we may wish to do a few more things done by CVort_engine_delay
    * itself, like resetting "time loses" after a call of the form
    * CVort_engine_shortSleep(true).
    */
   CVort_engine_delay(1);
   //engine_doWait();

	//engine_updateActualDisplay();
}

void CVort_engine_blitTile(uint16_t num, uint32_t firstPos)
{
	/* WORKAROUND/FIX: Avoid a crash from the following call list: */
	/* showImageFile -> drawScreen -> doDrawing ->                 */
	/* -> adaptiveTileRefresh -> blitTile                          */
	/* The reason: showImageFile USES map_data FOR THE IMAGE!!!!!! */
	if (num >= engine_egaHeadGeneral.tileNum)
		num = 0;
	uint8_t *tilePixelPtr = engine_egaTiles[num];
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+engine_currPageStart+firstPos;

	uint16_t currY;
	for (currY = 0; currY < 16; currY++)
	{
		memcpy(byteEgaPtr, tilePixelPtr, 16);
		tilePixelPtr += 16;
		byteEgaPtr += ENGINE_EGA_GFX_SCANLINE_LEN;
	}
	CVort_engine_updateEgaGfxNonPalRect_ptr(engine_currPageStart+firstPos, 16, 16);
	// NOT NEEDED - This func is called by CVort_engine_adaptiveTileRefresh only
	//engine_isFrameReadyToDisplay = true;
}

void CVort_engine_adaptiveTileRefresh(uint16_t initTileIndex)
{
	uint8_t refreshPage = engine_currPage^1;
	uint16_t tileNum;
	uint32_t wrapEgaOffset = 0;
	// Cases when screen wrap should be added:
	// 28h, 52h, 7Ch, A6h, D0h, FAh, 124h, 14Eh, 178h, 1A2h, 1CCh, 1F6h, 220h, 24Ah
	// (For the latter it is probably unnecessary...)
	for (uint16_t loopVar = 0; loopVar < 0x126; loopVar++)
	{
		/* bp should be anim_plane_p, which stores an offset
		 * to some anim_plane[i].
		 * We rather let such "i" index (in the range 0..3) be anim_plane_i.
		 * But wait...
		 *
		 * WORKAROUND/FIX: Avoid a crash from the following call list:
		 * showImageFile -> drawScreen ->
		 * -> doDrawing -> adaptiveTileRefresh
		 * The reason: showImageFile USES map_data FOR THE IMAGE!!!!!!
		 *
		 * ANOTHER WORKAROUND/FIX: Make sure this is an UNSIGNED
		 * comparison. (Maybe a value like -1 is involved?)
		 */
		if ((unsigned)(map_data_tiles[initTileIndex+loopVar]) >= engine_egaHeadGeneral.tileNum)
			continue;

		tileNum = anim_plane[anim_plane_i][map_data_tiles[initTileIndex+loopVar]];
		if (tileNum != ATR[refreshPage][loopVar])
		{
			ATR[refreshPage][loopVar] = tileNum;
			CVort_engine_blitTile(tileNum, 0x3020+16*loopVar+wrapEgaOffset);
		}
		switch (loopVar)
		{
		case 0x14:
		case 0x29:
		case 0x3e:
		case 0x53:
		case 0x68:
		case 0x7d:
		case 0x92:
		case 0xa7:
		case 0xbc:
		case 0xd1:
		case 0xe6:
		case 0xfb:
		case 0x110:
		case 0x125:
			initTileIndex += screen_wrap_single;
			wrapEgaOffset += 0x16b0; // For vanilla code it's 0x2d6
			break;
		default:
			break;
		}
	}
  // NOT NEEDED - This func is called by CVort_engine_adaptiveTileRefresh only
  //engine_isFrameReadyToDisplay = true;
}

void CVort_engine_scrollText(int16_t top_line_offs, int16_t bot_line_offs, int16_t direction)
{
	int16_t var_2 = ((bot_line_offs-top_line_offs)<<3)*0x30, srcOffset, dstOffset;
	if (direction == 0) // Relative down, i.e. up on screen
	{
		top_line_offs *= 0x180;
		srcOffset = dstOffset = top_line_offs;
		srcOffset += 0x180;

		memmove(engine_screen.client.byteEgaMemory+engine_dstPage+(dstOffset<<3), engine_screen.client.byteEgaMemory+engine_dstPage+(srcOffset<<3), var_2<<3);
		if (!engine_screen.host.isIndexedColorFormatted)
			memmove(engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_dstPage+(dstOffset<<3)), engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_dstPage+(srcOffset<<3)), engine_screen.host.bytesPerPixel*(var_2<<3));
	}
	else if (direction == 1) // Up
	{
		bot_line_offs = bot_line_offs * 0x180 + 0x17F;
		srcOffset = dstOffset = bot_line_offs;
		srcOffset -= 0x180;
		// The vanilla way moves stuff in BACKWARDS DIRECTION...
		memmove(engine_screen.client.byteEgaMemory+engine_dstPage+(dstOffset<<3)-((var_2-1)<<3), engine_screen.client.byteEgaMemory+engine_dstPage+(srcOffset<<3)-((var_2-1)<<3), var_2<<3);
		if (!engine_screen.host.isIndexedColorFormatted)
			memmove(engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_dstPage+(dstOffset<<3)-((var_2-1)<<3)), engine_screen.host.egaMemoryPtr+engine_screen.host.bytesPerPixel*(engine_dstPage+(srcOffset<<3)-((var_2-1)<<3)), engine_screen.host.bytesPerPixel*(var_2<<3));
	}
  engine_isFrameReadyToDisplay = true;
}

void CVort_engine_egaPageFlip()
{
	// Hope it is all right... note that we measure by pixel==BYTE,
	// not BIT (within a single EGA plane).
	engine_currPage ^= 1;
	engine_currPageStart = 0x18000*engine_currPage;

	engine_egaStart = 0x3020+(0x180*(unsigned)((scrollY >> 8) & 0xF)) + (((scrollX & 0x800) >> 11) << 3) + engine_currPageStart;
	engine_dstPage = (engine_egaStart >> 7) << 7;

	pel_panning = (scrollX >> 8) & 7;
}

/* Gets a value represeting 6 EGA signals determining a color number
and returns it in a "Blue Green Red Intensity" 4-bit format.
Usually, the 6 signals represented by the given input mean:
"Blue Green Red Secondary-Blue Secondary-Green Secondary-Red". However, for
the histroic reason of compatibility with CGA monitors, on the 200-lines mode
used by Keen the Secondary-Green signal is treated as an Intensity one and
the two other intensity signals are ignored.                               */
uint8_t CVort_private_engine_convertEGASignalToEGAEntry(const uint8_t color)
{
	return (color & 7) | ((color & 16) >> 1);
}

void privResetBorderColor(void) {
	switch (engine_gfx_effective_arguments.outputSystem) {
#if SDL_VERSION_ATLEAST(2,0,0)
		case OUTPUTSYS_TEXTURE:
			if (!engine_screen.host.bilinearInterpolation) {
				break; // We re-draw overscan border per frame using SDL_RenderDrawRect
			}
			if (engine_screen.sdl.renderTexture) {
				SDL_SetRenderDrawColor(engine_screen.sdl.renderer,
				                       engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 16,
				                       (engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 8) & 0xFF,
				                       engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] & 0xFF,
			                               0xFF
				);
				SDL_SetRenderTarget(engine_screen.sdl.renderer, engine_screen.sdl.renderTexture);
				SDL_RenderClear(engine_screen.sdl.renderer);
				SDL_SetRenderTarget(engine_screen.sdl.renderer, NULL);
				// Reset clear color
				SDL_SetRenderDrawColor(engine_screen.sdl.renderer, 0, 0, 0, 0xFF);
				// But don't yet clear (since we do so per frame anyway)
			} else {
				void *buffer;
				int pitch;
				SDL_LockTexture(engine_screen.sdl.texture, NULL, &buffer, &pitch);
				CVort_engine_updateBorderedZoomedRectBufferBorder_ptr((uint8_t *)buffer);
				SDL_UnlockTexture(engine_screen.sdl.texture);
			}
			// Otherwise we call SDL_RenderDrawRect for each
			// buffer swap (since we can't do it here just once.)
			break;
#else
		case OUTPUTSYS_OVERLAY:
		{
			SDL_LockYUVOverlay(engine_screen.sdl.overlay);
			CVort_engine_updateBorderedZoomedRectBufferBorder_ptr(*engine_screen.sdl.overlay->pixels);
			SDL_UnlockYUVOverlay(engine_screen.sdl.overlay);
			break;
		}
#endif
#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
		case OUTPUTSYS_OPENGL:
			if (!engine_screen.host.bilinearInterpolation) {
				break; // We re-draw overscan border per frame
			}
			engine_glClearColor(((GLclampf)(engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 16)) / 255.0f,
			                    ((GLclampf)((engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 8) & 0xFF)) / 255.0f,
			                    ((GLclampf)(engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] & 0xFF)) / 255.0f,
			                    1.0f
			);
			if (engine_screen.gl.offScreenRendering) {
				engine_glBindFramebuffer_CoreEXT(ENGINE_GL_FRAMEBUFFER_COREEXT, engine_screen.gl.frameBufferName);
			}
			engine_glClear(GL_COLOR_BUFFER_BIT);
			if (!engine_screen.gl.offScreenRendering) {
				int texWidth = privGetPowerOfTwoCeiling(engine_screen.dims.clientZoomedBorderedWidth),
				    texHeight = privGetPowerOfTwoCeiling(engine_screen.dims.clientZoomedBorderedHeight);
				int xOffset, yOffset;
				// FIXME: Use *whole* window, not just the view
				// port. Another way might be better, though.
				for (yOffset = 0; yOffset + engine_screen.dims.viewportRect.h < texHeight; yOffset += engine_screen.dims.viewportRect.h) {
					for (xOffset = 0; xOffset + engine_screen.dims.viewportRect.w < texWidth; xOffset += engine_screen.dims.viewportRect.w) {
						engine_glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, 0, 0,
						                           engine_screen.dims.viewportRect.w,
						                           engine_screen.dims.viewportRect.h
						);
					}
					engine_glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, 0, 0,
					                           texWidth-xOffset,
					                           engine_screen.dims.viewportRect.h
					);
				}
				for (xOffset = 0; xOffset + engine_screen.dims.viewportRect.w < texWidth; xOffset += engine_screen.dims.viewportRect.w) {
					engine_glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, 0, 0,
					                           engine_screen.dims.viewportRect.w,
					                           texHeight-yOffset
					);
				}
				engine_glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, 0, 0,
				                           texWidth-xOffset,
				                           texHeight-yOffset
				);
			}
			// If an FBO is used, we don't bind back right now - no need.

			// Reset clear color
			engine_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			// But don't yet clear (since we do so per frame anyway)
			break;
#endif
		case OUTPUTSYS_SURFACE:
			if (SDL_MUSTLOCK(engine_screen.sdl.windowSurface)) {
				SDL_LockSurface(engine_screen.sdl.windowSurface);
			}
			SDL_FillRect(engine_screen.sdl.windowSurface,
			             &engine_screen.dims.borderedViewportRect,
			             //engine_screen.host.colorTable[engine_screen.client.currParsedBorderColor]);
			             SDL_MapRGB(engine_screen.sdl.windowSurface->format,
			                        engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 16,
			                        (engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] >> 8) & 0xFF,
			                        engine_egaRGBColorTable[engine_screen.client.currParsedBorderColor] & 0xFF
			             )
			);
			if (SDL_MUSTLOCK(engine_screen.sdl.windowSurface)) {
				SDL_UnlockSurface(engine_screen.sdl.windowSurface);
			}
			break;
	}
}

// We set the overscan border color, based on the *default* EGA color palette.
// Technically, this is done by filling the SDL screen. Since its 320x200 rect
// is updated as a whole anyway, we can simply do a fill like this.
void CVort_engine_setBorderColor(const uint8_t color)
{
	engine_screen.client.currParsedBorderColor = CVort_private_engine_convertEGASignalToEGAEntry(color);
	privResetBorderColor();
}

void CVort_engine_gotoXY(uint8_t x, uint8_t y) {
	x--;
	y--;
	if ((x > ENGINE_EGAVGA_TXT_COLS_NUM) || (y > ENGINE_EGAVGA_TXT_ROWS_NUM)) {
		return;
	}
	CVort_engine_updateEgaNonBlinkingTxtNonPalRect_ptr(x, y, 1, 1);
	engine_isFrameReadyToDisplay = true;
	engine_screen.client.txtCursorPosX = x;	
	engine_screen.client.txtCursorPosY = y;
}

void privResetPalette(void) {
	// Bit 0 = blue, bit 1 = Green, bit 2 = red
	// and bit 4 (NOT 3) = intensity. We "shift" the latter to bit 3
	// and then pick the actual color (currently in ABGR format).
	uint8_t index;
	if (!engine_screen.host.isIndexedColorFormatted) {
		uint8_t *nonPalPtr, *palPtr;
		for (index = 0, nonPalPtr = engine_screen.host.mappedEgaColorTable; index < 16; index++, nonPalPtr+=engine_screen.host.bytesPerPixel) {
			memcpy(nonPalPtr, engine_screen.host.colorTable+engine_screen.host.bytesPerPixel*engine_screen.client.currParsedPalette[index], engine_screen.host.bytesPerPixel);
		}
		// Now we refresh the "RGBA converted" variation of the EGA memory
		int currPix;
		for (currPix = 0, palPtr = engine_screen.client.byteEgaMemory, nonPalPtr = engine_screen.host.egaMemoryPtr; currPix < sizeof(engine_screen.client.byteEgaMemory); currPix++, palPtr++, nonPalPtr+=engine_screen.host.bytesPerPixel) {
			memcpy(nonPalPtr, engine_screen.host.mappedEgaColorTable+engine_screen.host.bytesPerPixel*(*palPtr), engine_screen.host.bytesPerPixel);
			//engine_screen.host.egaMemoryPtr[currPix] = engine_screen.host.mappedEgaColorTable[engine_screen.client.byteEgaMemory[currPix]];
		}
	} else switch (engine_gfx_effective_arguments.outputSystem) { // Indexed color
#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
	case OUTPUTSYS_OPENGL:
		if (engine_screen.gl.gpuPaletteCycling) {
			static uint32_t actualPalette[16]; // TODO: Can it be non-static?
			for (index = 0; index < 16; index++) {
				actualPalette[index] = SDL_SwapBE32(engine_egaRGBColorTable[engine_screen.client.currParsedPalette[index]] << 8);
			}
	                engine_glActiveTexture(GL_TEXTURE0 + 1);
	                engine_glBindTexture(GL_TEXTURE_2D, *engine_screen.gl.palTextureName);
			engine_glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 1,
			                       GL_RGBA, GL_UNSIGNED_BYTE, actualPalette
			);
	                // Restore
	                engine_glActiveTexture(GL_TEXTURE0 + 0);
		}
		break;
#endif
	}
#if 0
	// Now we update *both* pages (surfaces).
	for (uint32_t page = 0, pixIndex; page < 2; page++)
		for (pixIndex = 0; pixIndex < engine_egapages[page]->w*engine_egapages[page]->h; pixIndex++)
			((uint32_t *)engine_egapages[page]->pixels)[pixIndex] = engine_screen.host.mappedEgaColorTable[engine_palettedpages[page][pixIndex]];
	// Let one see the results
#endif
	engine_isFrameReadyToDisplay = true;
	//engine_updateActualDisplay();
}

void CVort_engine_setPaletteAndBorderColor(const uint8_t *palette) {
	CVort_engine_setBorderColor(palette[16]);
	for (int i = 0; i < 16; i++) {
		engine_screen.client.currParsedPalette[i] =  CVort_private_engine_convertEGASignalToEGAEntry(palette[i]);
	}
	privResetPalette();
}

void CVort_engine_showImageFile(const char *filename)
{
	FILE *fp = CVort_engine_cross_ro_data_fopen(filename);
	if (!fp) // TODO: What to do, if not this?
	{
		scrollX = scrollY = 0;
		return;
	}
	// The original code does read to a part of the map_data structure!!!
	fread(map_data + 0x4000, CVort_filelength(fp), 1, fp);
	fclose(fp);
	// It also decompresses from one part to the other.
	// NOTE NOTE NOTE: The initial 4-byte value is LITTLE-ENDIAN ordered!!!
	do_image_file_decomp((uint8_t *)(map_data + 0x4000), (uint8_t *)map_data);

	scrollX = scrollY = 0;
	CVort_engine_syncDrawing();
	CVort_engine_drawScreen();


	// The image data is planar and we need to convert it; Twice.
	uint8_t currPixelVal;
	uint32_t currBit = 0;
	uint8_t *vanillaPtr = (uint8_t *)map_data;
	// While the source offset is 0, the EGA screen offset is 4 bytes.
	// Here we translate it accordingly.
	uint8_t *byteEgaPtr = engine_screen.client.byteEgaMemory+engine_dstPage+32, *byteEgaLinePtr = byteEgaPtr;
	for (int16_t currLine = 0, currCol; currLine < ENGINE_EGA_GFX_HEIGHT; currLine++)
	{
		for (currCol = 0; currCol < ENGINE_EGA_GFX_WIDTH; currCol++)
		{
			currPixelVal = 0;
			if (*(vanillaPtr+currBit/8)&(1<<(currBit%8^7)))
				currPixelVal |= 1; // Blue bit
			if (*(vanillaPtr+currBit/8+0x2000)&(1<<(currBit%8^7)))
				currPixelVal |= 2; // Green bit
			if (*(vanillaPtr+currBit/8+0x4000)&(1<<(currBit%8^7)))
				currPixelVal |= 4; // Red bit
			if (*(vanillaPtr+currBit/8+0x6000)&(1<<(currBit%8^7)))
				currPixelVal |= 8; // Intensity bit
			*byteEgaPtr = currPixelVal;
			currBit++;
			byteEgaPtr++;
		}
		byteEgaLinePtr += ENGINE_EGA_GFX_SCANLINE_LEN;
		byteEgaPtr = byteEgaLinePtr;
	}
	CVort_engine_updateEgaGfxNonPalRect_ptr(engine_dstPage+32, ENGINE_EGA_GFX_WIDTH, ENGINE_EGA_GFX_HEIGHT);
	CVort_engine_clearOverlay();
	engine_isFrameReadyToDisplay = true;
	//engine_updateActualDisplay();
}
