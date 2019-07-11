/*
 * Cunlzexe.h
 *
 *  Created on: 29.12.2009
 *      Author: gerstrong
 *
 *  Modified to C style on: 03.07.2013 (July 3rd)
 *
 *  Embedded Extraction Code for all executable files which are lzexed
 *  Based on the code of Vesselin Bontchev v0.8.
 *  Can be used for extracting every exe which is compressed with lzexe
 */

#ifndef CUNLZEXE_H_
#define CUNLZEXE_H_

#include <stdbool.h>
#include <stdint.h>

typedef uint16_t WORD_16BIT;
typedef uint8_t BYTE;

bool Cunlzexe_decompress(BYTE *data, BYTE **outdata);
unsigned long Cunlzexe_getUncompressedExeSize();
unsigned long Cunlzexe_getHeaderSize();
void Cunlzexe_free(BYTE **outdata);

#endif /* CUNLZEXE_H_ */
