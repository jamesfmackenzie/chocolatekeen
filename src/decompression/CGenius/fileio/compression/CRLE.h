/*
 * CRLE.h
 *
 *  Created on: 31.05.2010
 *      Author: gerstrong
 *
 *  Modified to C style on: 03.07.2013 (July 3rd)
 *
 */

#ifndef CRLE_H_
#define CRLE_H_

#include "../TypeDefinitions.h"
//#include <vector>

#if 0
void CRLE_expand( word *dst, const byte *src, word key );
#endif
void CRLE_expandSwapped( word *dst, const byte *src, word key );

#endif /* CRLE_H_ */
