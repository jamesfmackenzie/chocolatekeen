/*
 * CRLE.cpp
 *
 *  Created on: 31.05.2010
 *      Author: gerstrong
 *
 *  Modified to C style on: 03.07.2013 (July 3rd)
 *
 *  RLE(W) decompression
 *
 	 The algorithm works as follows:
	 1.) If implemented, get the first dword in the file, [Final Length]
	 2.) If [Length of data so far] < [Final Length] then:
	 3.) Get a word
	 4.) Is this word $FEFE?
	 -> If yes;
	 Get the next two words (Word1 and Word2)
	 Copy Word1 [Word2] times
	 Move forward three words and got to 2.)
	 -> If no;
	 Copy the word
	 Move forward a word and go to 2.)

	 This also means that if the file is already decompressed, the algorithm
	 will just read the bytes and dump a word buffer which should be 16-bit
 *
 */

#include "CRLE.h"

#define NEARTAG     0xA7
#define FARTAG      0xA8
#define WORDSIZE    2

#define COUNT       i
#define TAG         i+1
#define OFFSET      i+2

#define OFFSET_MSB  i+3 // Fartag Offset
#define OFFSET_LSB  i+2

#define COPY_BYTE1  j
#define COPY_BYTE2  j+1

void CRLE_expandSwapped( word *dst, const byte *src, word key )
{
	size_t finsize, howmany;
	word value;
	size_t inc;

	finsize = (src[1]<<8) | src[0];
	finsize /= 2;


	for(size_t i=WORDSIZE, elementnum=0 ; elementnum < finsize ; i+=inc)
    {
		// Read datum (word)
		value = (src[i+1]<<8)+src[i];
		// If datum is 0xFEFE/0xABCD Then
		if (value == key)
		{
			// Read count (word)
			howmany = (src[i+3]<<8)+src[i+2];
			value = (src[i+5]<<8)+src[i+4];

			// Do count times
			for(Uint32 j=0;j<howmany;j++,elementnum++)
				dst[elementnum] = value;

			inc = 3*WORDSIZE;
		}
		else
		{
			dst[elementnum] = value;
			elementnum++;
			inc = WORDSIZE;
		}
    }
}

#if 0
void CRLE_expand( word *dst, const byte *src, word key )
{
    uint16_t word, count, inc;

	size_t finsize;
	byte high_byte, low_byte;

	low_byte = src[1];
	high_byte = src[0];
	finsize = (high_byte<<8) | low_byte;
	finsize /= 2;

    for(size_t i=WORDSIZE, elementnum=0 ; elementnum < finsize ; i+=inc)
    {
        // Read datum (word)
        word = (src[i]<<8)+src[i+1];
        // If datum is 0xFEFE/0xABCD Then
        if( word == key )
        {
            // Read count (word)
            count = (src[i+2]<<8)+src[i+3];
			word = (src[i+4]<<8)+src[i+5];

            // Do count times
			for(Uint32 j=0;j<count;j++,elementnum++)
				dst[elementnum] = word;

            inc = 3*WORDSIZE;
        }
        else
        {
            // Write datum (word)
			dst[elementnum] = word;
			elementnum++;
			inc = WORDSIZE;
        }
    }
}
#endif
