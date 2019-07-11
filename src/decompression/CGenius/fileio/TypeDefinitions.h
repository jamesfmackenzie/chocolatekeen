/*
 * TyoeDefinitions.h
 *
 *  Created on: 30.05.2010
 *      Author: gerstrong
 *
 *
 */

#ifndef TYOEDEFINITIONS_H_
#define TYOEDEFINITIONS_H_

#include <SDL.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef int32_t fixed;
typedef uint32_t longword;
typedef void * memptr;

static inline word READWORD(byte **pPtr)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	word val = (*pPtr)[0]<<8 | (*pPtr)[1];
#else
	word val = (*pPtr)[0] | (*pPtr)[1] << 8;
#endif
    *pPtr += sizeof(word);
    return val;
}

static inline word GETWORD(byte *ptr){
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return (ptr[0]<<8 | ptr[1]);
#else
	return (ptr[0] | ptr[1] << 8);
#endif
}

static inline longword READLONGWORD(byte **pPtr)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	longword val = (*pPtr)[0]<<24 | (*pPtr)[1] << 16 | (*pPtr)[2] << 8 | (*pPtr)[3];
#else
	longword val = (*pPtr)[0] | (*pPtr)[1] << 8 | (*pPtr)[2] << 16 | (*pPtr)[3] << 24;
#endif
    *pPtr += 4;
    return val;
}


#endif /* TYOEDEFINITIONS_H_ */
