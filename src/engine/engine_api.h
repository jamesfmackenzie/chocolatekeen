#ifndef _CHOCOLATE_KEEN_ENGINE_API_H_
#define _CHOCOLATE_KEEN_ENGINE_API_H_

#include <stdbool.h>

bool CVort_engine_processArguments(int argc, char **argv);
bool CVort_engine_start(void);
void CVort_engine_shutdown(void);

#endif
