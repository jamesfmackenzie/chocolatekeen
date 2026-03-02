#ifndef _CHOCOLATE_KEEN_ENGINE_TIMING_H_
#define _CHOCOLATE_KEEN_ENGINE_TIMING_H_

#include <stdint.h>

void CVort_private_engine_setTicks(uint32_t currTicks);
uint32_t CVort_private_engine_getTicks(void);
void CVort_private_engine_setTicksSync(uint32_t currTicks);
uint32_t CVort_private_engine_getTicksSync(void);

#endif
