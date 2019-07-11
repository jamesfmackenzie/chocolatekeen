#include "SDL.h"

/* Implementation based on info posted here:                   */
/* http://www.shikadi.net/moddingwiki/Keen_1-3_RLE_compression */

/* Gets a compressed image file in LITTLE-ENDIAN format and decompresses it.  */
/* Note that the Little-Endian part is mainly relevant for the first 4 bytes. */
void do_image_file_decomp(Uint8 *compressedSrc, Uint8 *decompressionBuffer)
{
	Sint32 dataLength = SDL_SwapLE32(*(Uint32 *)(compressedSrc));
	compressedSrc += 4;
	Uint8 currAction, timesToApply;
	Uint16 loopVar;
	while (dataLength >= 0)
	{
		currAction = *compressedSrc;
		compressedSrc++;
		if (currAction >= 0x80)
		{
			currAction -= 0x7F;
			for (loopVar = 0; loopVar < currAction; loopVar++, compressedSrc++, decompressionBuffer++)
				*decompressionBuffer = *compressedSrc;
		}
		else
		{
			currAction += 3;
			for (loopVar = 0; loopVar < currAction; loopVar++, decompressionBuffer++)
				*decompressionBuffer = *compressedSrc;
			compressedSrc++;
		}
		dataLength -= currAction;
	}
}
