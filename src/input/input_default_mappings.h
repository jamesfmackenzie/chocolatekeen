// input_default_mappings.h: Declares input default mappings interfaces for the input subsystem.

#ifndef _CHOCOLATE_KEEN_INPUT_DEFAULT_MAPPINGS_
#define _CHOCOLATE_KEEN_INPUT_DEFAULT_MAPPINGS_

#include "input/input.h"

extern const MappedInputEvent_T defaultKeyMappings[];
extern const int defaultKeyMappingsCount;
extern const MappedInputEvent_T defaultJoyButtonMappings[];
extern const MappedInputEvent_T defaultJoyPAxisMappings[];
extern const MappedInputEvent_T defaultJoyNAxisMappings[];
extern const MappedInputEvent_T defaultFullScreenHandlerMapping;
extern const MappedInputEvent_T defaultCursorLockHandlerMapping;
extern const MappedInputEvent_T defaultQuitHandlerMapping;
extern const MappedInputEvent_T defaultFirstModifierMapping;
extern const MappedInputEvent_T defaultSecondModifierMapping;

#endif
