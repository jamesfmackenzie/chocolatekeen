#ifndef _CHOCOLATE_KEEN_IMAGE_RLE_
#define _CHOCOLATE_KEEN_IMAGE_RLE_

#include "SDL.h"

/* Gets a compressed image file in LITTLE-ENDIAN format and decompresses it.  */
/* Note that the Little-Endian part is mainly relevant for the first 4 bytes. */
void do_image_file_decomp(Uint8 *compressedSrc, Uint8 *decompressionBuffer);

#endif
