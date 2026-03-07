#include "SDL.h"

#include "core/globals.h"
#include "input/input_default_mappings.h"
#include "input/input_keys.h"
#include "input/input_names.h"
#include "platform/platform.h"

// Practically used as a deadzone for analog axes
#define CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD 16384
// And for mixing host mouse motion with whole different things
// (as well as client mouse motion with different kinds of input)
#define CHOCOLATE_KEEN_EVENT_HANDLING_MOUSE_MOTION_SCALE_FACTOR 4096
#define CHOCOLATE_KEEN_ARRAY_LEN(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))

#if SDL_VERSION_ATLEAST(2,0,0)
#define CHOCOLATE_KEEN_MAPPER_FILENAME "chocolate-keen-mapper-sdl2_0.map"
#else
#define CHOCOLATE_KEEN_MAPPER_FILENAME "chocolate-keen-mapper-sdl1_2.map"
#endif

static void shutdown_and_exit(int status) {
    CVort_engine_shutdown();
    exit(status);
}

static void handle_input_handler_event(int handlerValue) {
    switch (handlerValue) {
        case INPUTHANDLER_FULLSCREEN:
            engine_arguments.isFullscreen = !engine_arguments.isFullscreen;
            if (!CVort_engine_resetWindow()) {
                exit(1);
            }
            break;
        case INPUTHANDLER_CAPTURECURSOR:
            CVort_engine_toggleCursorLock(!engine_isCursorLocked);
            break;
        case INPUTHANDLER_SHUTDOWN:
            shutdown_and_exit(0);
            break;
        default:
            break;
    }
}

static int16_t get_scaled_joystick_axis_poll(int32_t action) {
    int32_t poll = (action+32768) * 500 / 65535;
    if (poll < 0) {
        return 0;
    }
    if (poll > 500) {
        return 500;
    }
    return (int16_t)poll;
}

static void apply_mouse_delta_with_upper_bound(int16_t *value, int32_t delta, int16_t upperBound) {
    *value += delta;
    if (*value > upperBound) {
        *value = upperBound;
    }
}

static void apply_mouse_delta_with_lower_bound(int16_t *value, int32_t delta) {
    *value -= delta;
    if (*value < 0) {
        *value = 0;
    }
}

/* fileBuffer contains the contents of the mapper file as-is;
 * tempBuffer should begin with a " char, followed by a string representing
 * some kind of host input. For instance: "key 100".
 * pEventList refers to the host input's associated event list to be filled.
 */
bool CVort_engine_addInputMappingsFromFile(const char *fileBuffer, const char *tempBuffer, MappedInputEventList_T *pEventList) {
    const char *ptrLoopVar = strstr(fileBuffer, tempBuffer), *ptr;
    int loopVar;
    bool eventFound;

#define CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(emuStr) \
    ((strstr(ptr, emuStr) != ptr) || (strchr(ptr, ' ') == NULL) || \
     (strlen(emuStr) != (size_t)(strchr(ptr, ' ') - ptr)))

#define CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW() \
    CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Mapper file has more than %d entries for a single host device.\nOnly %d are loaded and the rest may be removed in the future.\n", MAX_EMU_MAPPINGS_PER_HOST_ENTRY, MAX_EMU_MAPPINGS_PER_HOST_ENTRY)

    // For any occurrence of the given HOST input found in the mapper file...
    while (ptrLoopVar) {
        // Look for the emulated event, by traversing in the file backwards
        ptr = ptrLoopVar;
        while ((ptr != fileBuffer) && (*ptr != '\n')) {
            ptr--;
        }
        if (ptr != fileBuffer) {
            ptr++; // We have reached a "line feed" character, so skip it.
        }
        // Check for emulated keyboard event
        eventFound = false;
        for (loopVar = 0; loopVar < engine_emulatedKeysTableCount; loopVar++) {
            if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(engine_emulatedKeysTable[loopVar].name)) {
                    continue;
            }
            // Emulated keyboard event found, process now
            ptr += strlen(engine_emulatedKeysTable[loopVar].name);
            // If we've reached the cap, issue a warning just in case
            if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
            } else {
                pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_KEYPRESS;
                pEventList->list[pEventList->numOfEvents].value = loopVar;
            }
            eventFound = true;
            break;
        }
        // Check for emulated mouse button event IF no other one has been found
        if (!eventFound) {
            for (loopVar = 0; loopVar < 3; loopVar++) {
                if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(mapperClientMouseButtonNames[loopVar])) {
                    continue;
                }
                // Emulated mouse button event found, process now
                ptr += strlen(mapperClientMouseButtonNames[loopVar]);
                if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                    CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
                } else {
                    pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_MOUSEBUTTONPRESS;
                    pEventList->list[pEventList->numOfEvents].value = (1 << loopVar);
                }
                eventFound = true;
                break;
            }
        }
        // Check for emulated mouse motion event if none has been found
        if (!eventFound) {
            for (loopVar = 0; loopVar < 4; loopVar++) {
                if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(mapperClientMouseAxisNames[loopVar])) {
                    continue;
                }
                // Emulated mouse button event found, process now
                ptr += strlen(mapperClientMouseAxisNames[loopVar]);
                if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                    CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
                } else {
                    pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_MOUSEMOTION;
                    pEventList->list[pEventList->numOfEvents].value = ((loopVar%2)*2-1) * (loopVar/2 + 1);
                }
                eventFound = true;
                break;
            }
        }
        // Check for emulated joystick button event if none has been found
        if (!eventFound) {
            for (loopVar = 0; loopVar < 6; loopVar++) {
                if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(mapperClientJoyButtonNames[loopVar])) {
                    continue;
                }
                // Emulated joystick button event found, process now
                ptr += strlen(mapperClientJoyButtonNames[loopVar]);
                if (loopVar >= 4) {
                    loopVar -= 2; // e.g. jbutton_1_0 is the same as jbutton_0_2
                }
                if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                    CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
                } else {
                    pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_JOYBUTTONPRESS;
                    pEventList->list[pEventList->numOfEvents].value = (1 << loopVar);
                }
                eventFound = true;
                break;
            }
        }
        // Check for emulated joystick motion event if none has been found
        if (!eventFound) {
            for (loopVar = 0; loopVar < 8; loopVar++) {
                if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(mapperClientJoyAxisNames[loopVar])) {
                    continue;
                }
                // Emulated joystick motion event found, process now
                ptr += strlen(mapperClientJoyAxisNames[loopVar]);
                if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                    CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
                } else {
                    pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_JOYMOTION;
                    pEventList->list[pEventList->numOfEvents].value = ((loopVar%2)*2-1) * (loopVar/2 + 1);
                }
                eventFound = true;
                break;
            }
        }
        // Check for an internal handler event if not has been found
        if (!eventFound) {
            for (loopVar = 0; loopVar < 3; loopVar++) {
                if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(mapperClientHandlerNames[loopVar])) {
                    continue;
                }
                // Handler event found, process now
                ptr += strlen(mapperClientHandlerNames[loopVar]);
                if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                    CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
                } else {
                    pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_HANDLER;
                    pEventList->list[pEventList->numOfEvents].value = loopVar+1;
                }
                eventFound = true;
                break;
            }
        }
        // Check for a modifier toggle event if not has been found
        if (!eventFound) {
            for (loopVar = 0; loopVar < 3; loopVar++) {
                if (CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE(mapperClientModNames[loopVar])) {
                    continue;
                }
                // Modifier toggle event found, process now
                ptr += strlen(mapperClientModNames[loopVar]);
                if (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY) {
                    CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW();
                } else {
                    pEventList->list[pEventList->numOfEvents].emulatedInput = EMULATEDINPUT_MODTOGGLE;
                    pEventList->list[pEventList->numOfEvents].value = (1 << loopVar);
                }
                eventFound = true;
                break;
            }
        }
	// Maybe it's a simple line comment, or there's an overflow, so skip.
        if (!eventFound || (pEventList->numOfEvents == MAX_EMU_MAPPINGS_PER_HOST_ENTRY)) {
            ptrLoopVar = strstr(ptrLoopVar+1, tempBuffer);
            continue;
        }
        // For the event to apply, a few modifiers may need to be used, too. 
        pEventList->list[pEventList->numOfEvents].modMask = 0;
        /* Currently ptr is pointing at what should be a space character,
         * following the emulated event and preceding the relevant host input,
         * beginning with double-quotes (e.g. "key 10").
         */
        ptr = strstr(ptr, tempBuffer) + strlen(tempBuffer) - 1;
        while (*ptr != '"') {
            if (*ptr != ' ') { // Something's wrong
                return false;
            }
            ptr++;
            if (strstr(ptr, "mod1") == ptr) {
                pEventList->list[pEventList->numOfEvents].modMask |= 1;
            } else if (strstr(ptr, "mod2") == ptr) {
                pEventList->list[pEventList->numOfEvents].modMask |= 2;
            } else if (strstr(ptr, "mod3") == ptr) {
                pEventList->list[pEventList->numOfEvents].modMask |= 4;
            } else {
                return false;
            }
            ptr += 4; // Skip the "mod#" substring
        }
        // Finally, we are done.
        pEventList->numOfEvents++;
        // Go to the next occurrence of the host input
        ptrLoopVar = strstr(ptrLoopVar+1, tempBuffer);
    }

#undef CHOCOLATE_KEEN_IS_STRING_FOUND_IN_THE_RIGHT_PLACE
#undef CHOCOLATE_KEEN_WARN_OF_HOST_MAPPING_OVERFLOW

    return true;
}

bool CVort_engine_tryToLoadInputMappings(void) {
    // Yeah, open in BINARY mode for reading; If the file contains occurrences
    // of the Carriage Return character (13 '\r') or not, it does not bother us.
    FILE *fp = CVort_engine_configpath_fopen(CHOCOLATE_KEEN_MAPPER_FILENAME, "rb");
    if (!fp) {
        return false;
    }
    uint32_t numOfDblQuotes = 0, length = CVort_filelength(fp), loopVar, joystickNum;
    char *fileBuffer = (char *)malloc((length+1)*sizeof(char));
    if (!fileBuffer || (fread(fileBuffer, length, 1, fp) != 1)) {
        if (fileBuffer) {
            free(fileBuffer);
        }
        fclose(fp);
        return false;
    }
    fclose(fp);
    // DON'T FORGET THIS!!
    fileBuffer[length] = '\0';

    for (loopVar = 0; loopVar < length; loopVar++) {
        if (fileBuffer[loopVar] == '"') {
            numOfDblQuotes++;
        }
    }
    if (numOfDblQuotes % 2 == 1) { // Something's wrong with the given format
        free(fileBuffer);
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Mapper file is not properly formatted; Reverting to defaults.\n");
        return false;
    }
    //uint32_t numOfEventEntries = numOfDblQuotes/2;

    // Prepare buffer of all events mappings
    //engine_inputMappings.mainEventsBuffer = (MappedInputEvent_T *)malloc(numOfEventEntries * sizeof(MappedInputEvent_T));
    //int currBufferIndex = 0;

    /* We now rescan the file contents in "reversed order" - i.e. by the
     * host input (e.g. a specific keyboard's key). Reason is that we want all
     * of its events to reside with no gaps within the buffer just allocated.
     */

    char tempBuffer[64];

    // For convenience, defining some macro...

#define CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(eventList) \
    { \
        /*(eventList).list = &engine_inputMappings.mainEventsBuffer[currBufferIndex];*/ \
        (eventList).numOfEvents = 0; \
        if (!CVort_engine_addInputMappingsFromFile(fileBuffer, tempBuffer, &(eventList))) { \
            free(fileBuffer); \
            /*free(engine_inputMappings.mainEventsBuffer);*/ \
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Mapper file is not properly formatted; Reverting to defaults.\n"); \
            return false; \
        } \
        tempBuffer[strlen(tempBuffer)-1] = '"'; \
        if (!CVort_engine_addInputMappingsFromFile(fileBuffer, tempBuffer, &(eventList))) { \
            free(fileBuffer); \
            /*free(engine_inputMappings.mainEventsBuffer);*/ \
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Mapper file is not properly formatted; Reverting to defaults.\n"); \
            return false; \
        } \
        /*currBufferIndex += (eventList).numOfEvents;*/ \
    }

    // First, looking for host keyboard events...
    for (loopVar = 0; loopVar < (uint32_t)CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.keyMappings); loopVar++) {
        snprintf(tempBuffer, sizeof(tempBuffer), "\"key %u ", loopVar);
        CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.keyMappings[loopVar]);
    }
    // Host mouse button events come next
    for (loopVar = 0; loopVar < (uint32_t)CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.mouseButtonMappings); loopVar++) {
        snprintf(tempBuffer, sizeof(tempBuffer), "\"mouse button %u ", loopVar);
        CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.mouseButtonMappings[loopVar]);
    }
    // Mouse axes follow
    for (loopVar = 0; loopVar < (uint32_t)CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.mouseRelPAxisMappings); loopVar++) {
        snprintf(tempBuffer, sizeof(tempBuffer), "\"mouse axis %u 1 ", loopVar);
        CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.mouseRelPAxisMappings[loopVar]);
        snprintf(tempBuffer, sizeof(tempBuffer), "\"mouse axis %u 0 ", loopVar);
        CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.mouseRelNAxisMappings[loopVar]);
    }
    // Time to handle host joysticks
    for (joystickNum = 0; joystickNum < (uint32_t)engine_inputMappings.numOfJoysticks; joystickNum++) {
        // Beginning with the buttions
        for (loopVar = 0; loopVar < (uint32_t)engine_inputMappings.joystickMappings[joystickNum].numOfButtons; loopVar++) {
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u button %u ", joystickNum, loopVar);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickButtonMappings[loopVar]);
        }
        // Analog axes come next
        for (loopVar = 0; loopVar < (uint32_t)engine_inputMappings.joystickMappings[joystickNum].numOfAxes; loopVar++) {
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u axis %u 1 ", joystickNum, loopVar);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickPAxisMappings[loopVar]);
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u axis %u 0 ", joystickNum, loopVar);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickNAxisMappings[loopVar]);
        }
        // Finally, HAT switches
        for (loopVar = 0; loopVar < (uint32_t)engine_inputMappings.joystickMappings[joystickNum].numOfHats; loopVar++) {
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u hat %u %u ", joystickNum, loopVar, SDL_HAT_RIGHT);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickPHorizHatMappings[loopVar]);
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u hat %u %u ", joystickNum, loopVar, SDL_HAT_DOWN);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickPVertHatMappings[loopVar]);
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u hat %u %u ", joystickNum, loopVar, SDL_HAT_LEFT);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickNHorizHatMappings[loopVar]);
            snprintf(tempBuffer, sizeof(tempBuffer), "\"stick_%u hat %u %u ", joystickNum, loopVar, SDL_HAT_UP);
            CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO(engine_inputMappings.joystickMappings[joystickNum].joystickNVertHatMappings[loopVar]);
        }
    }

#undef CHOCOLATE_KEEN_HANDLE_HOST_INPUT_MACRO

    free(fileBuffer);
    return true;
}

void CVort_engine_setDefaultInputMappings(void) {
    int loopVar, secondaryLoopVar;
    // We should first reset the mapping lists.
    memset(engine_inputMappings.keyMappings, 0, sizeof(engine_inputMappings.keyMappings));
    memset(engine_inputMappings.mouseButtonMappings, 0, sizeof(engine_inputMappings.mouseButtonMappings));
    memset(engine_inputMappings.mouseRelPAxisMappings, 0, sizeof(engine_inputMappings.mouseRelPAxisMappings));
    memset(engine_inputMappings.mouseRelNAxisMappings, 0, sizeof(engine_inputMappings.mouseRelNAxisMappings));
    for (loopVar = 0; loopVar < engine_inputMappings.numOfJoysticks; loopVar++) {
        memset(engine_inputMappings.joystickMappings[loopVar].joystickButtonMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfButtons * sizeof(MappedInputEventList_T));
        memset(engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfAxes * sizeof(MappedInputEventList_T));
        memset(engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfAxes * sizeof(MappedInputEventList_T));
        memset(engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfHats * sizeof(MappedInputEventList_T));
        memset(engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfHats * sizeof(MappedInputEventList_T));
        memset(engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfHats * sizeof(MappedInputEventList_T));
        memset(engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings, 0, engine_inputMappings.joystickMappings[loopVar].numOfHats * sizeof(MappedInputEventList_T));
    }

    // First prepare keyboard event maps
    for (loopVar = 0; loopVar < defaultKeyMappingsCount; loopVar++) {
        if (defaultKeyMappings[loopVar].emulatedInput == EMULATEDINPUT_KEYPRESS) {
#if SDL_VERSION_ATLEAST(2,0,0)
            if ((loopVar == SDL_SCANCODE_LALT) || (loopVar == SDL_SCANCODE_RALT))
#else
            if ((loopVar == SDLK_LALT) || (loopVar == SDLK_RALT))
#endif
            {
                // Add a MOD toggle event
                engine_inputMappings.keyMappings[loopVar].numOfEvents = 2;
                //engine_inputMappings.keyMappings[loopVar].list = (MappedInputEvent_T *)malloc(engine_inputMappings.keyMappings[loopVar].numOfEvents*sizeof(MappedInputEvent_T));
                // The usual...
                engine_inputMappings.keyMappings[loopVar].list[0] = defaultKeyMappings[loopVar];
                // Act as a modifier (for internal event handling)
                engine_inputMappings.keyMappings[loopVar].list[1] = defaultSecondModifierMapping;
            } else
#if SDL_VERSION_ATLEAST(2,0,0)
                   if ((loopVar == SDL_SCANCODE_LCTRL) || (loopVar == SDL_SCANCODE_RCTRL))
#else
                   if ((loopVar == SDLK_LCTRL) || (loopVar == SDLK_RCTRL))
#endif
            {
                // Add a MOD toggle event
                engine_inputMappings.keyMappings[loopVar].numOfEvents = 2;
                //engine_inputMappings.keyMappings[loopVar].list = (MappedInputEvent_T *)malloc(engine_inputMappings.keyMappings[loopVar].numOfEvents*sizeof(MappedInputEvent_T));
                // The usual...
                engine_inputMappings.keyMappings[loopVar].list[0] = defaultKeyMappings[loopVar];
                // Act as a modifier
                engine_inputMappings.keyMappings[loopVar].list[1] = defaultFirstModifierMapping;
            } else
#if SDL_VERSION_ATLEAST(2,0,0)
                   if (loopVar == SDL_SCANCODE_RETURN)
#else
                   if (loopVar == SDLK_RETURN)
#endif
            {
                // Add fullscreen handling
                engine_inputMappings.keyMappings[loopVar].numOfEvents = 2;
                //engine_inputMappings.keyMappings[loopVar].list = (MappedInputEvent_T *)malloc(engine_inputMappings.keyMappings[loopVar].numOfEvents*sizeof(MappedInputEvent_T));
                // The usual...
                engine_inputMappings.keyMappings[loopVar].list[0] = defaultKeyMappings[loopVar];
                // Fullscreen toggle
                engine_inputMappings.keyMappings[loopVar].list[1] = defaultFullScreenHandlerMapping;
            } else
#if SDL_VERSION_ATLEAST(2,0,0)
                   if (loopVar == SDL_SCANCODE_F10)
#else
                   if (loopVar == SDLK_F10)
#endif
            {
                // Add cursor (un)lock handling
                engine_inputMappings.keyMappings[loopVar].numOfEvents = 2;
                //engine_inputMappings.keyMappings[loopVar].list = (MappedInputEvent_T *)malloc(engine_inputMappings.keyMappings[loopVar].numOfEvents*sizeof(MappedInputEvent_T));
                // The usual...
                engine_inputMappings.keyMappings[loopVar].list[0] = defaultKeyMappings[loopVar];
                // Cursor un(lock)
                engine_inputMappings.keyMappings[loopVar].list[1] = defaultCursorLockHandlerMapping;
            
            } else
#if SDL_VERSION_ATLEAST(2,0,0)
                   if (loopVar == SDL_SCANCODE_F9)
#else
                   if (loopVar == SDLK_F9)
#endif
            {
                // Add quit
                engine_inputMappings.keyMappings[loopVar].numOfEvents = 2;
                //engine_inputMappings.keyMappings[loopVar].list = (MappedInputEvent_T *)malloc(engine_inputMappings.keyMappings[loopVar].numOfEvents*sizeof(MappedInputEvent_T));
                // The usual...
                engine_inputMappings.keyMappings[loopVar].list[0] = defaultKeyMappings[loopVar];
                // Quit
                engine_inputMappings.keyMappings[loopVar].list[1] = defaultQuitHandlerMapping;
            } else {
                engine_inputMappings.keyMappings[loopVar].numOfEvents = 1;
                //engine_inputMappings.mainEventsBuffer[eventCounter] = defaultKeyMappings[loopVar];
                //engine_inputMappings.keyMappings[loopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
		engine_inputMappings.keyMappings[loopVar].list[0] = defaultKeyMappings[loopVar];
            }
            //eventCounter++;
        }
    }
    // Continue with the mouse: Buttons first
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_LEFT-1].list[0].emulatedInput = EMULATEDINPUT_MOUSEBUTTONPRESS;
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_LEFT-1].list[0].value = 1;
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_LEFT-1].numOfEvents = 1;

    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_RIGHT-1].list[0].emulatedInput = EMULATEDINPUT_MOUSEBUTTONPRESS;
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_RIGHT-1].list[0].value = 2;
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_RIGHT-1].numOfEvents = 1;

    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_MIDDLE-1].list[0].emulatedInput = EMULATEDINPUT_MOUSEBUTTONPRESS;
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_MIDDLE-1].list[0].value = 4;
    engine_inputMappings.mouseButtonMappings[SDL_BUTTON_MIDDLE-1].numOfEvents = 1;

    // Mouse motion comes next
    engine_inputMappings.mouseRelPAxisMappings[0].list[0].emulatedInput = EMULATEDINPUT_MOUSEMOTION;
    engine_inputMappings.mouseRelPAxisMappings[0].list[0].value = 1;
    engine_inputMappings.mouseRelPAxisMappings[0].numOfEvents = 1;

    engine_inputMappings.mouseRelPAxisMappings[1].list[0].emulatedInput = EMULATEDINPUT_MOUSEMOTION;
    engine_inputMappings.mouseRelPAxisMappings[1].list[0].value = 2;
    engine_inputMappings.mouseRelPAxisMappings[1].numOfEvents = 1;

    engine_inputMappings.mouseRelNAxisMappings[0].list[0].emulatedInput = EMULATEDINPUT_MOUSEMOTION;
    engine_inputMappings.mouseRelNAxisMappings[0].list[0].value = -1;
    engine_inputMappings.mouseRelNAxisMappings[0].numOfEvents = 1;

    engine_inputMappings.mouseRelNAxisMappings[1].list[0].emulatedInput = EMULATEDINPUT_MOUSEMOTION;
    engine_inputMappings.mouseRelNAxisMappings[1].list[0].value = -2;
    engine_inputMappings.mouseRelNAxisMappings[1].numOfEvents = 1;

    // Next, handle (host) joysticks.

    for (loopVar = 0; loopVar < engine_inputMappings.numOfJoysticks; loopVar++) {
        // Map buttons as-is, mod 4
        //eventCounter = eventJoyStart;
        for (secondaryLoopVar = 0; secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfButtons; secondaryLoopVar++) {
            //engine_inputMappings.joystickMappings[loopVar].joystickButtonMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickButtonMappings[secondaryLoopVar].list[0] = defaultJoyButtonMappings[secondaryLoopVar%4];
            engine_inputMappings.joystickMappings[loopVar].joystickButtonMappings[secondaryLoopVar].numOfEvents = 1;
        }

        // Map axes in the positive directions as-is, for the first 4 axes only
        //eventCounter = eventJoyStart + 4;
        for (secondaryLoopVar = 0; (secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfAxes) && (secondaryLoopVar < 4); secondaryLoopVar++) {
            //engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings[secondaryLoopVar].list[0] = defaultJoyPAxisMappings[secondaryLoopVar/*%4*/];
            engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings[secondaryLoopVar].numOfEvents = 1;
        }
        // For any other axis we have possibly left out,
        // assign an empty list of events
        for (; secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfAxes; secondaryLoopVar++)
            engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings[secondaryLoopVar].numOfEvents = 0;

        // Map axes in the negative directions as-is for the first 4 axes only
        //eventCounter = eventJoyStart + 8;
        for (secondaryLoopVar = 0; (secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfAxes) && (secondaryLoopVar < 4); secondaryLoopVar++) {
            //engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings[secondaryLoopVar].list[0] = defaultJoyNAxisMappings[secondaryLoopVar/*%4*/];
            engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings[secondaryLoopVar].numOfEvents = 1;
        }
        // As before, assign empty event lists
        for (; secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfAxes; secondaryLoopVar++)
            engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings[secondaryLoopVar].numOfEvents = 0;

        /* Similarly map HAT switches, but NOT omitting any of them, and mod 4.
         * To be more specific, the first HAT switch is mapped to axes 0,1;
         * The next one is mapped to axes 2,3; The third is mapped to 0,1;
         * and so it repeats.
         */

        // The actual mapped events are... the emulated joystick axes for now.

        //eventCounter = eventJoyStart + 4; // Primary joystick
        for (secondaryLoopVar = 0; secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfHats; secondaryLoopVar += 2) {
            //eventCounter = eventJoyStart + 4;

            //engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings[secondaryLoopVar].list[0] = defaultJoyPAxisMappings[0];
            engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter++;
            //engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings[secondaryLoopVar].list[0] = defaultJoyPAxisMappings[1];
            engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter += 3;
            //engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings[secondaryLoopVar].list[0] = defaultJoyNAxisMappings[0];
            engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter++;
            //engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings[secondaryLoopVar].list[0] = defaultJoyNAxisMappings[1];
            engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter = eventJoyStart + 4;
        }

        //eventCounter = eventJoyStart + 8; // Secondary joystick
        for (secondaryLoopVar = 1; secondaryLoopVar < engine_inputMappings.joystickMappings[loopVar].numOfHats; secondaryLoopVar += 2) {
            //eventCounter = eventJoyStart + 8;

            //engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings[secondaryLoopVar].list[0] = defaultJoyPAxisMappings[2];
            engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter++;
            //engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings[secondaryLoopVar].list[0] = defaultJoyPAxisMappings[3];
            engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter += 3;
            //engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings[secondaryLoopVar].list[0] = defaultJoyNAxisMappings[2];
            engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter++;
            //engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings[secondaryLoopVar].list = &engine_inputMappings.mainEventsBuffer[eventCounter];
            engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings[secondaryLoopVar].list[0] = defaultJoyNAxisMappings[3];
            engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings[secondaryLoopVar].numOfEvents = 1;
            //eventCounter = eventJoyStart + 8
        }
    }
}

void CVort_engine_setupInputMappings(void) {
    int loopVar;

    CK_PlatformPrepareInput();

    // Clear all first
    memset(&engine_inputMappings, 0, sizeof(engine_inputMappings));

    // Initialize joystick related resources
    engine_inputMappings.numOfJoysticks = SDL_NumJoysticks();
    engine_inputMappings.sdlJoysticks = (SDL_Joystick **)malloc(engine_inputMappings.numOfJoysticks * sizeof(SDL_Joystick *));
    for (loopVar = 0; loopVar < engine_inputMappings.numOfJoysticks; loopVar++) {
        engine_inputMappings.sdlJoysticks[loopVar] = SDL_JoystickOpen(loopVar);
    }
    engine_inputMappings.joystickMappings = (HostJoystickMapping_T *)malloc(engine_inputMappings.numOfJoysticks*sizeof(HostJoystickMapping_T));
    for (loopVar = 0; loopVar < engine_inputMappings.numOfJoysticks; loopVar++) {
        engine_inputMappings.joystickMappings[loopVar].numOfButtons = SDL_JoystickNumButtons(engine_inputMappings.sdlJoysticks[loopVar]);
        engine_inputMappings.joystickMappings[loopVar].numOfAxes = SDL_JoystickNumAxes(engine_inputMappings.sdlJoysticks[loopVar]);
        engine_inputMappings.joystickMappings[loopVar].numOfHats = SDL_JoystickNumHats(engine_inputMappings.sdlJoysticks[loopVar]);
        engine_inputMappings.joystickMappings[loopVar].joystickButtonMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfButtons*sizeof(MappedInputEventList_T));
        engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfAxes*sizeof(MappedInputEventList_T));
        engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfAxes*sizeof(MappedInputEventList_T));
        engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfHats*sizeof(MappedInputEventList_T));
        engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfHats*sizeof(MappedInputEventList_T));
        engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfHats*sizeof(MappedInputEventList_T));
        engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings = (MappedInputEventList_T *)malloc(engine_inputMappings.joystickMappings[loopVar].numOfHats*sizeof(MappedInputEventList_T));
    }

    // Try loading from file
    if (CVort_engine_tryToLoadInputMappings()) {
        return;
    }
    // In case of failure we revert to internal defaults.
    CVort_engine_setDefaultInputMappings();
}

void CVort_engine_teardownInputMappings(void) {
    int loopVar;

    if (engine_inputMappings.sdlJoysticks) {
        for (loopVar = 0; loopVar < engine_inputMappings.numOfJoysticks; loopVar++) {
            if (engine_inputMappings.sdlJoysticks[loopVar]) {
                SDL_JoystickClose(engine_inputMappings.sdlJoysticks[loopVar]);
            }
        }
        free(engine_inputMappings.sdlJoysticks);
        engine_inputMappings.sdlJoysticks = NULL;
    }

    if (engine_inputMappings.joystickMappings) {
        for (loopVar = 0; loopVar < engine_inputMappings.numOfJoysticks; loopVar++) {
            free(engine_inputMappings.joystickMappings[loopVar].joystickButtonMappings);
            free(engine_inputMappings.joystickMappings[loopVar].joystickPAxisMappings);
            free(engine_inputMappings.joystickMappings[loopVar].joystickNAxisMappings);
            free(engine_inputMappings.joystickMappings[loopVar].joystickPHorizHatMappings);
            free(engine_inputMappings.joystickMappings[loopVar].joystickNHorizHatMappings);
            free(engine_inputMappings.joystickMappings[loopVar].joystickPVertHatMappings);
            free(engine_inputMappings.joystickMappings[loopVar].joystickNVertHatMappings);
            memset(&engine_inputMappings.joystickMappings[loopVar], 0, sizeof(HostJoystickMapping_T));
        }
        free(engine_inputMappings.joystickMappings);
        engine_inputMappings.joystickMappings = NULL;
    }

    engine_inputMappings.numOfJoysticks = 0;
}

MappedInputEvent_T *CVort_engine_recordNewInputMapping(EmulatedInput_T emuInput, int value, unsigned int mstimeout, HostInput_T *retInputT, int *retInputId, int *retInputVal) {
	SDL_Event event;
	HostInput_T inputT;
	int inputId, inputVal;
	MappedInputEventList_T *inputEventList = NULL;
	// First ignore "old" key/button and co presses
	while (SDL_PollEvent(&event)) {
	        switch (event.type) {
#if SDL_VERSION_ATLEAST(2,0,0)
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				CVort_engine_reactToWindowResize(event.window.data1, event.window.data2);
			} else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				CVort_engine_handleWindowSideChange();
			}
			break;
#else
		case SDL_VIDEORESIZE:
			CVort_engine_reactToWindowResize(event.resize.w, event.resize.h);
			break;
#endif
		case SDL_QUIT:
			shutdown_and_exit(0);
		default:; // Includes key/button presses and MUCH more
		}
	}
	// Now we do the work for real
	uint32_t currTime = SDL_GetTicks();
	bool doWait = true;
	int totalmousexmove = 0, totalmouseymove = 0;
	while (doWait && (SDL_GetTicks() - currTime < mstimeout)) {
		while (SDL_PollEvent(&event)) {
		        switch (event.type) {
#if SDL_VERSION_ATLEAST(2,0,0)
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					CVort_engine_reactToWindowResize(event.window.data1, event.window.data2);
				} else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					CVort_engine_handleWindowSideChange();
				}
				break;
#else
			case SDL_VIDEORESIZE:
				CVort_engine_reactToWindowResize(event.resize.w, event.resize.h);
				break;
#endif
			case SDL_KEYDOWN:
#if SDL_VERSION_ATLEAST(2,0,0)
				inputVal = event.key.keysym.scancode;
#else
				inputVal = event.key.keysym.sym;
#endif
				inputEventList = &engine_inputMappings.keyMappings[inputVal];
				inputT = HOSTINPUT_KEYPRESS;
				inputId = 0;
				doWait = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if ((event.button.button < 1) || (event.button.button > 5)) {
					break;
				}
				inputVal = event.button.button-1;
				inputEventList = &engine_inputMappings.mouseButtonMappings[inputVal];
				inputT = HOSTINPUT_MOUSEBUTTONPRESS;
				inputId = 0;
				doWait = false;
				break;
			case SDL_MOUSEMOTION:
				totalmousexmove += event.motion.xrel;
				totalmouseymove += event.motion.yrel;
				if (totalmousexmove >= 40) {
					inputVal = 1;
					inputEventList = &engine_inputMappings.mouseRelPAxisMappings[0];
				} else if (totalmousexmove <= -40) {
					inputVal = -1;
					inputEventList = &engine_inputMappings.mouseRelNAxisMappings[0];
				} else if (totalmouseymove >= 40) {
					inputVal = 2;
					inputEventList = &engine_inputMappings.mouseRelPAxisMappings[1];
				} else if (totalmouseymove <= -40) {
					inputVal = -2;
					inputEventList = &engine_inputMappings.mouseRelNAxisMappings[1];
				} else {
					break;
				}
				// Detected something
				inputT = HOSTINPUT_MOUSEMOTION;
				inputId = 0;
				doWait = false;
				break;
			case SDL_JOYBUTTONDOWN:
	        	        if ((event.jbutton.which >= engine_inputMappings.numOfJoysticks) || (event.jbutton.button >= engine_inputMappings.joystickMappings[event.jbutton.which].numOfButtons))
					break; // No overflow again...
				inputVal = event.jbutton.button;
				inputT = HOSTINPUT_JOYBUTTONPRESS;
				inputId = event.jbutton.which;
				inputEventList = &engine_inputMappings.joystickMappings[inputId].joystickButtonMappings[inputVal];
				doWait = false;
				break;
			case SDL_JOYAXISMOTION:
				if ((event.jaxis.which >= engine_inputMappings.numOfJoysticks) || (event.jaxis.axis >= engine_inputMappings.joystickMappings[event.jaxis.which].numOfAxes))
						break; // ...
				if (event.jaxis.value >= CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD) {
					inputVal = event.jaxis.axis+1;
					inputEventList = &engine_inputMappings.joystickMappings[event.jaxis.which].joystickPAxisMappings[event.jaxis.axis];
				} else if (event.jaxis.value <= -CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD) {
					inputVal = -event.jaxis.axis-1;
					inputEventList = &engine_inputMappings.joystickMappings[event.jaxis.which].joystickNAxisMappings[event.jaxis.axis];
				} else {
					break;
				}
				inputT = HOSTINPUT_JOYMOTION;
				inputId = event.jaxis.which;
				doWait = false;
				break;
			case SDL_JOYHATMOTION:
				if ((event.jhat.which >= engine_inputMappings.numOfJoysticks) || (event.jhat.hat >= engine_inputMappings.joystickMappings[event.jhat.which].numOfHats))
					break; // ...
				if (event.jhat.value & SDL_HAT_RIGHT) {
					inputVal = 1+2*event.jhat.hat;
					inputEventList = &engine_inputMappings.joystickMappings[event.jhat.which].joystickPHorizHatMappings[event.jhat.hat];
				} else if (event.jhat.value & SDL_HAT_LEFT) {
					inputVal = -1-2*event.jhat.hat;
					inputEventList = &engine_inputMappings.joystickMappings[event.jhat.which].joystickNHorizHatMappings[event.jhat.hat];
				} else if (event.jhat.value & SDL_HAT_DOWN) {
					inputVal = 2+2*event.jhat.hat;
					inputEventList = &engine_inputMappings.joystickMappings[event.jhat.which].joystickPVertHatMappings[event.jhat.hat];
				} else if (event.jhat.value & SDL_HAT_UP) {
					inputVal = -2-2*event.jhat.hat;
					inputEventList = &engine_inputMappings.joystickMappings[event.jhat.which].joystickNVertHatMappings[event.jhat.hat];
				} else {
					break;
				}
				inputT = HOSTINPUT_JOYHAT;
				inputId = event.jhat.which;
				doWait = false;
				break;
			case SDL_QUIT:
				shutdown_and_exit(0);
			default:;
			}
		}
		if (engine_isFrameReadyToDisplay || ((uint32_t)(SDL_GetTicks() - engine_lastDisplayUpdateTime) >= 100)) {
			CVort_engine_updateActualDisplay();
			engine_lastDisplayUpdateTime = SDL_GetTicks();
		}
        CK_PlatformSleepMs(1);
	}
	if (doWait) // Timed out
		return NULL;
	// First check if the desired (emulated) event is already in the list
	int loopVar;
	for (loopVar = 0; loopVar < inputEventList->numOfEvents; loopVar++) {
		if ((inputEventList->list[loopVar].emulatedInput == emuInput) &&
		    (inputEventList->list[loopVar].value == value)) {
			*retInputT = inputT;
			*retInputId = inputId;
			*retInputVal = inputVal;
			return &(inputEventList->list[loopVar]);
		}
	}
	// No overflow? (Technically it *should* come from duplications)
	if (loopVar == MAX_EMU_MAPPINGS_PER_HOST_ENTRY)
		return NULL;
	inputEventList->list[loopVar].emulatedInput = emuInput;
	inputEventList->list[loopVar].value = value;
	inputEventList->list[loopVar].modMask = 0;
	inputEventList->numOfEvents++;
	*retInputT = inputT;
	*retInputId = inputId;
	*retInputVal = inputVal;
	return &(inputEventList->list[loopVar]);
}

static MappedInputEventList_T *get_input_event_list(HostInput_T inputT, int inputId, int inputVal) {
	switch (inputT) {
	case HOSTINPUT_KEYPRESS:
		return &engine_inputMappings.keyMappings[inputVal];
	case HOSTINPUT_MOUSEBUTTONPRESS:
		return &engine_inputMappings.mouseButtonMappings[inputVal];
	case HOSTINPUT_MOUSEMOTION:
		switch (inputVal) {
		case 1:
			return &engine_inputMappings.mouseRelPAxisMappings[0];
		case -1:
			return &engine_inputMappings.mouseRelNAxisMappings[0];
		case 2:
			return &engine_inputMappings.mouseRelPAxisMappings[1];
		case -2:
			return &engine_inputMappings.mouseRelNAxisMappings[1];
		default:
			return NULL;
		}
	case HOSTINPUT_JOYBUTTONPRESS:
		return &engine_inputMappings.joystickMappings[inputId].joystickButtonMappings[inputVal];
	case HOSTINPUT_JOYMOTION:
		if (inputVal > 0)
			return &engine_inputMappings.joystickMappings[inputId].joystickPAxisMappings[inputVal-1];
		if (inputVal < 0)
			return &engine_inputMappings.joystickMappings[inputId].joystickNAxisMappings[-inputVal-1];
		return NULL;
	case HOSTINPUT_JOYHAT:
		if (inputVal == 0)
			return NULL;
		if (inputVal > 0) {
			if (inputVal % 2 != 0)
				return &engine_inputMappings.joystickMappings[inputId].joystickPHorizHatMappings[(inputVal-1)/2];
			return &engine_inputMappings.joystickMappings[inputId].joystickPVertHatMappings[(inputVal-2)/2];
		}
		if (inputVal % 2 != 0)
			return &engine_inputMappings.joystickMappings[inputId].joystickNHorizHatMappings[(-inputVal-1)/2];
		return &engine_inputMappings.joystickMappings[inputId].joystickNVertHatMappings[(-inputVal-2)/2];
	default:
		return NULL;
	}
}

void CVort_engine_deleteInputMapping(EmulatedInput_T emuInput, int emuValue, HostInput_T inputT, int inputId, int inputVal) {
	MappedInputEventList_T *inputEventList = get_input_event_list(inputT, inputId, inputVal);
	if (!inputEventList)
		return;
	int loopVar;
		for (loopVar = 0; loopVar < inputEventList->numOfEvents; loopVar++) {
			if ((inputEventList->list[loopVar].emulatedInput == emuInput) &&
			    (inputEventList->list[loopVar].value == emuValue)) {
				// Keep mappings densely packed with O(1) removal (order is not preserved).
				inputEventList->list[loopVar] = inputEventList->list[inputEventList->numOfEvents-1];
				inputEventList->numOfEvents--;
				return;
			}
		}
}

static void inc_input_mapping_host_side(HostInput_T *pInputT, int *pInputId, int *pInputVal) {
	switch (*pInputT) {
	case HOSTINPUT_NONE:
		*pInputT = HOSTINPUT_KEYPRESS;
		*pInputId = 0; // Single host keyboard supported for now...
		*pInputVal = 0;
		return; // A valid host input so we're done
	case HOSTINPUT_KEYPRESS:
		// The number is either SDL_NUM_SCANCODES (SDL 2.0) or SDLK_LAST (SDL 1.2)
			if ((*pInputVal) + 1 < CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.keyMappings)) {
			(*pInputVal)++;
			return; // Finish
		}
		// Otherwise change to a mouse button
		*pInputT = HOSTINPUT_MOUSEBUTTONPRESS;
		*pInputId = 0; // Single host mouse supported for now...
		*pInputVal = 0;
		return; // Again we can finish
	case HOSTINPUT_MOUSEBUTTONPRESS:
		// Similarly checking if there's another mouse button
			if ((*pInputVal) + 1 < CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.mouseButtonMappings)) {
			(*pInputVal)++;
			return; // Finish
		}
		// Otherwise change to mouse motion
		*pInputT = HOSTINPUT_MOUSEMOTION;
		*pInputId = 0; // Single host mouse supported for now...
		*pInputVal = 1; // Horizontal axis, pointing right
		return; // We're done
	case HOSTINPUT_MOUSEMOTION:
		// Change to next axis/direction if there's any
		if (*pInputVal == 1) {
			*pInputVal = -1;
			return;
		}
		if (*pInputVal == -1) {
			*pInputVal = 2;
			return;
		}
		if (*pInputVal == 2) {
			*pInputVal = -2;
			return;
		}
		// Otherwise we should begin going over the joysticks
			*pInputT = HOSTINPUT_JOYBUTTONPRESS;
			*pInputId = -1; // Signal we're coming out of mouse motion handling
			// Do NOT return now
			/* fall through */
		case HOSTINPUT_JOYBUTTONPRESS:
		// Coming out of mouse motion case?
		if (*pInputId == -1) {
			*pInputId = 0; // Any (first) joystick?
			*pInputVal = 0; // Any joystick (first) button?
		} else {
			// Increase joystick button by one?
			if ((*pInputVal) + 1 < engine_inputMappings.joystickMappings[*pInputId].numOfButtons) {
				(*pInputVal)++;
				return; // Finish
			}
			// Try going to next joystick (and its first button, if any)
			(*pInputId)++;
			*pInputVal = 0;
		}
		// Look for a new joystick, with at least one button
		while ((*pInputId < engine_inputMappings.numOfJoysticks) && (engine_inputMappings.joystickMappings[*pInputId].numOfButtons == 0))
			(*pInputId)++;
		if (*pInputId < engine_inputMappings.numOfJoysticks)
			return; // Joystick found with some button, so finish
		// Otherwise we should begin going over the axes
			*pInputT = HOSTINPUT_JOYMOTION;
			*pInputId = -1; // Signal we're coming out of button handling
			// Do NOT return now
			/* fall through */
		case HOSTINPUT_JOYMOTION:
		// Coming out of joystick button case
		if (*pInputId == -1) {
			*pInputId = 0; // Any (first) joystick?
			*pInputVal = 1; // Positive direction for first axis
		} else {
			// Change axis direction (to negative)?
			if (*pInputVal > 0) {
				*pInputVal = -(*pInputVal);
				return; // FINISH
			}
			// Otherwise it is already negative, any following axis?
			if (-(*pInputVal) < engine_inputMappings.joystickMappings[*pInputId].numOfAxes) {
				*pInputVal = -(*pInputVal)+1; // Next axis, positive direction
				return; // Finish
			}
			// Try going to next joystick (and its first axis, if any)
			(*pInputId)++;
			*pInputVal = 1; // Positive direction
		}
		// Look for a new joystick, with at least one axis
		while ((*pInputId < engine_inputMappings.numOfJoysticks) && (engine_inputMappings.joystickMappings[*pInputId].numOfAxes == 0))
			(*pInputId)++;
		if (*pInputId < engine_inputMappings.numOfJoysticks)
			return; // Joystick found with some axis, so finish
		// Otherwise we should begin going over the hats
			*pInputT = HOSTINPUT_JOYHAT;
			*pInputId = -1; // Signal we're coming out of axis handling
			// Do NOT return now
			/* fall through */
		case HOSTINPUT_JOYHAT:
		// Coming out of joystick axis case
		if (*pInputId == -1) {
			*pInputId = 0; // Any (first) joystick?
			*pInputVal = 1; // First hat pointing right
		} else {
			int hatNum;
			// Flip hat direction to negative? (Right->Left or Down->Up)
			if (*pInputVal > 0) {
				*pInputVal = -(*pInputVal);
				return; // FINISH
			}
			// Negative here. Hat pointing left?
			if (*pInputVal % 2 != 0) {
				hatNum = (-(*pInputVal)-1)/2;
				// Point down
				*pInputVal = 2+2*hatNum;
				return; // FINISH
			}
			// Hat points up so move to next hat, if there's any
			hatNum = (-(*pInputVal)-2)/2;
			if (hatNum+1 < engine_inputMappings.joystickMappings[*pInputId].numOfHats) {
				hatNum++;
				// Point right
				*pInputVal = 1+2*hatNum;
				return; // FINISH
			}
			// Try going to next joystick (and its first hat, if any)
			(*pInputId)++;
			*pInputVal = 1; // Point right
		}
		// Look for a new joystick, with at least one hat
		while ((*pInputId < engine_inputMappings.numOfJoysticks) && (engine_inputMappings.joystickMappings[*pInputId].numOfHats == 0))
			(*pInputId)++;
		if (*pInputId < engine_inputMappings.numOfJoysticks)
			return; // Joystick found with some hat, so finish
			// Otherwise revert to no-input
		*pInputT = HOSTINPUT_NONE;
		*pInputId = 0;
		*pInputVal = 0;
		return;
	}
}

static void dec_input_mapping_host_side(HostInput_T *pInputT, int *pInputId, int *pInputVal) {
	int joyNum;
	switch (*pInputT) {
	case HOSTINPUT_NONE:
		for (joyNum = engine_inputMappings.numOfJoysticks-1; joyNum >= 0; joyNum--) {
			if (engine_inputMappings.joystickMappings[joyNum].numOfHats > 0) {
				*pInputT = HOSTINPUT_JOYHAT;
				*pInputId = joyNum;
				*pInputVal = -(2 + 2*(engine_inputMappings.joystickMappings[joyNum].numOfHats-1));
				return;
			}
		}
		for (joyNum = engine_inputMappings.numOfJoysticks-1; joyNum >= 0; joyNum--) {
			if (engine_inputMappings.joystickMappings[joyNum].numOfAxes > 0) {
				*pInputT = HOSTINPUT_JOYMOTION;
				*pInputId = joyNum;
				*pInputVal = -(engine_inputMappings.joystickMappings[joyNum].numOfAxes);
				return;
			}
		}
		for (joyNum = engine_inputMappings.numOfJoysticks-1; joyNum >= 0; joyNum--) {
			if (engine_inputMappings.joystickMappings[joyNum].numOfButtons > 0) {
				*pInputT = HOSTINPUT_JOYBUTTONPRESS;
				*pInputId = joyNum;
				*pInputVal = engine_inputMappings.joystickMappings[joyNum].numOfButtons-1;
				return;
			}
		}
		*pInputT = HOSTINPUT_MOUSEMOTION;
		*pInputId = 0;
		*pInputVal = -2;
		return;
	case HOSTINPUT_KEYPRESS:
		if (*pInputVal > 0) {
			(*pInputVal)--;
			return;
		}
		*pInputT = HOSTINPUT_NONE;
		*pInputId = 0;
		*pInputVal = 0;
		return;
	case HOSTINPUT_MOUSEBUTTONPRESS:
		if (*pInputVal > 0) {
			(*pInputVal)--;
			return;
		}
		*pInputT = HOSTINPUT_KEYPRESS;
		*pInputId = 0;
		*pInputVal = sizeof(engine_inputMappings.keyMappings)/sizeof(MappedInputEventList_T)-1;
		return;
	case HOSTINPUT_MOUSEMOTION:
		if (*pInputVal == -2) {
			*pInputVal = 2;
			return;
		}
		if (*pInputVal == 2) {
			*pInputVal = -1;
			return;
		}
		if (*pInputVal == -1) {
			*pInputVal = 1;
			return;
		}
		*pInputT = HOSTINPUT_MOUSEBUTTONPRESS;
		*pInputId = 0;
		*pInputVal = sizeof(engine_inputMappings.mouseButtonMappings)/sizeof(MappedInputEventList_T)-1;
		return;
	case HOSTINPUT_JOYBUTTONPRESS:
		if (*pInputVal > 0) {
			(*pInputVal)--;
			return;
		}
		for (joyNum = (*pInputId)-1; joyNum >= 0; joyNum--) {
			if (engine_inputMappings.joystickMappings[joyNum].numOfButtons > 0) {
				*pInputId = joyNum;
				*pInputVal = engine_inputMappings.joystickMappings[joyNum].numOfButtons-1;
				return;
			}
		}
		*pInputT = HOSTINPUT_MOUSEMOTION;
		*pInputId = 0;
		*pInputVal = -2;
		return;
	case HOSTINPUT_JOYMOTION:
		if (*pInputVal < 0) {
			*pInputVal = -(*pInputVal);
			return;
		}
		if (*pInputVal > 1) {
			*pInputVal = -(*pInputVal-1);
			return;
		}
		for (joyNum = (*pInputId)-1; joyNum >= 0; joyNum--) {
			if (engine_inputMappings.joystickMappings[joyNum].numOfAxes > 0) {
				*pInputId = joyNum;
				*pInputVal = -(engine_inputMappings.joystickMappings[joyNum].numOfAxes);
				return;
			}
		}
		for (joyNum = engine_inputMappings.numOfJoysticks-1; joyNum >= 0; joyNum--) {
			if (engine_inputMappings.joystickMappings[joyNum].numOfButtons > 0) {
				*pInputT = HOSTINPUT_JOYBUTTONPRESS;
				*pInputId = joyNum;
				*pInputVal = engine_inputMappings.joystickMappings[joyNum].numOfButtons-1;
				return;
			}
		}
		*pInputT = HOSTINPUT_MOUSEMOTION;
		*pInputId = 0;
		*pInputVal = -2;
		return;
	case HOSTINPUT_JOYHAT:
	{
		int hatNum, hatStep;
		if (*pInputVal > 0) {
			hatNum = (*pInputVal-1)/2;
			hatStep = (*pInputVal % 2) ? 0 : 2;
		} else {
			hatNum = (-(*pInputVal)-1)/2;
			hatStep = (*pInputVal % 2) ? 1 : 3;
		}
		hatStep--;
		if (hatStep < 0) {
			if (hatNum > 0) {
				hatNum--;
				hatStep = 3;
			} else {
				for (joyNum = (*pInputId)-1; joyNum >= 0; joyNum--) {
					if (engine_inputMappings.joystickMappings[joyNum].numOfHats > 0) {
						*pInputId = joyNum;
						*pInputVal = -(2 + 2*(engine_inputMappings.joystickMappings[joyNum].numOfHats-1));
						return;
					}
				}
				for (joyNum = engine_inputMappings.numOfJoysticks-1; joyNum >= 0; joyNum--) {
					if (engine_inputMappings.joystickMappings[joyNum].numOfAxes > 0) {
						*pInputT = HOSTINPUT_JOYMOTION;
						*pInputId = joyNum;
						*pInputVal = -(engine_inputMappings.joystickMappings[joyNum].numOfAxes);
						return;
					}
				}
				for (joyNum = engine_inputMappings.numOfJoysticks-1; joyNum >= 0; joyNum--) {
					if (engine_inputMappings.joystickMappings[joyNum].numOfButtons > 0) {
						*pInputT = HOSTINPUT_JOYBUTTONPRESS;
						*pInputId = joyNum;
						*pInputVal = engine_inputMappings.joystickMappings[joyNum].numOfButtons-1;
						return;
					}
				}
				*pInputT = HOSTINPUT_MOUSEMOTION;
				*pInputId = 0;
				*pInputVal = -2;
				return;
			}
		}
		if (hatStep == 0) {
			*pInputVal = 1+2*hatNum;
			return;
		}
		if (hatStep == 1) {
			*pInputVal = -(1+2*hatNum);
			return;
		}
		if (hatStep == 2) {
			*pInputVal = 2+2*hatNum;
			return;
		}
		*pInputVal = -(2+2*hatNum);
		return;
	}
	default:
		*pInputT = HOSTINPUT_NONE;
		*pInputId = 0;
		*pInputVal = 0;
		return;
	}
}

static MappedInputEvent_T *get_input_mapping_template(
    EmulatedInput_T emuInput, int value,
    HostInput_T inputT, int inputId, int inputVal,
    HostInput_T *retInputT, int *retInputId, int *retInputVal,
    void (*hostInputStepFunc)(HostInput_T *pInputT, int *pInputId, int *pInputVal)
) {
	HostInput_T currInputT = inputT;
	int currInputId = inputId, currInputVal = inputVal, loopVar;
	MappedInputEventList_T *inputEventList;
	do {
		// Increment/Decrement our "variable"
		hostInputStepFunc(&currInputT, &currInputId, &currInputVal);
		inputEventList = get_input_event_list(currInputT, currInputId, currInputVal);
		if (!inputEventList)
			continue;
		for (loopVar = 0; loopVar < inputEventList->numOfEvents; loopVar++) {
			if ((inputEventList->list[loopVar].emulatedInput == emuInput) &&
			    (inputEventList->list[loopVar].value == value)) {
				*retInputT = currInputT;
				*retInputId = currInputId;
				*retInputVal = currInputVal;
				return &(inputEventList->list[loopVar]);
			}
		}
	} while ((currInputT != inputT) || (currInputId != inputId) || (currInputVal != inputVal));
	// We're back to the original host input, which doesn't have a match, either.
	// So, no match found at all.
	return NULL;
}

MappedInputEvent_T *CVort_engine_getNextInputMapping(
    EmulatedInput_T emuInput, int value,
    HostInput_T inputT, int inputId, int inputVal,
    HostInput_T *retInputT, int *retInputId, int *retInputVal
) {
	return get_input_mapping_template(
	           emuInput, value,
	           inputT, inputId, inputVal,
	           retInputT, retInputId, retInputVal,
	           &inc_input_mapping_host_side);
}

MappedInputEvent_T *CVort_engine_getPrevInputMapping(
    EmulatedInput_T emuInput, int value,
    HostInput_T inputT, int inputId, int inputVal,
    HostInput_T *retInputT, int *retInputId, int *retInputVal
) {
	return get_input_mapping_template(
	           emuInput, value,
	           inputT, inputId, inputVal,
	           retInputT, retInputId, retInputVal,
	           &dec_input_mapping_host_side);
}

void CVort_engine_writeEventModifiers(FILE *fp, int modMask) {
    if (modMask & 1) {
        fprintf(fp, " mod1");
    }
    if (modMask & 2) {
        fprintf(fp, " mod2");
    }
    if (modMask & 4) {
        fprintf(fp, " mod3");
    }
    fprintf(fp, "\"");
}

/* Given an emulated event, writes its mappings to the file
 * and ends a new line. It does NOT add the emulated event's name to the file.
 *
 * ASSUMPTION: fp is opened for writing (in textual mode)
 */
void CVort_engine_writeMappedHostInputs(FILE *fp, const MappedInputEvent_T *pTestEvent) {
    int loopVar, eventLoopVar, joyLoopVar;

#define CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(currEvent) \
    ((pTestEvent->emulatedInput != (currEvent).emulatedInput) || \
     (pTestEvent->value != (currEvent).value))

    // First, looking for host keyboard events...
    for (loopVar = 0; loopVar < CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.keyMappings); loopVar++) {
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.keyMappings[loopVar].numOfEvents; eventLoopVar++) {
            if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.keyMappings[loopVar].list[eventLoopVar])) {
                continue;
            }
            fprintf(fp, " \"key %u", loopVar);
            CVort_engine_writeEventModifiers(fp, engine_inputMappings.keyMappings[loopVar].list[eventLoopVar].modMask);
        }
    }
    // Next follow host mouse button events
    for (loopVar = 0; loopVar < CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.mouseButtonMappings); loopVar++) {
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseButtonMappings[loopVar].numOfEvents; eventLoopVar++) {
            if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.mouseButtonMappings[loopVar].list[eventLoopVar])) {
                continue;
            }
            fprintf(fp, " \"mouse button %u", loopVar);
            CVort_engine_writeEventModifiers(fp, engine_inputMappings.mouseButtonMappings[loopVar].list[eventLoopVar].modMask);
        }
    }
    // Host mouse motion
    for (loopVar = 0; loopVar < CHOCOLATE_KEEN_ARRAY_LEN(engine_inputMappings.mouseRelPAxisMappings); loopVar++) {
        // Negative
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelNAxisMappings[loopVar].numOfEvents; eventLoopVar++) {
            if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.mouseRelNAxisMappings[loopVar].list[eventLoopVar])) {
                continue;
            }
            fprintf(fp, " \"mouse axis %u 0", loopVar);
            CVort_engine_writeEventModifiers(fp, engine_inputMappings.mouseRelNAxisMappings[loopVar].list[eventLoopVar].modMask);
        }
        // Positive
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelPAxisMappings[loopVar].numOfEvents; eventLoopVar++) {
            if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.mouseRelPAxisMappings[loopVar].list[eventLoopVar])) {
                continue;
            }
            fprintf(fp, " \"mouse axis %u 1", loopVar);
            CVort_engine_writeEventModifiers(fp, engine_inputMappings.mouseRelPAxisMappings[loopVar].list[eventLoopVar].modMask);
        }
    }
    // Host joysticks, general
    for (joyLoopVar = 0; joyLoopVar <  engine_inputMappings.numOfJoysticks; joyLoopVar++) {
        // Joystick buttons
        for (loopVar = 0; loopVar < engine_inputMappings.joystickMappings[joyLoopVar].numOfButtons; loopVar++) {
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickButtonMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickButtonMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u button %u", joyLoopVar, loopVar);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickButtonMappings[loopVar].list[eventLoopVar].modMask);
            }
        }
        // Joystick axes
        for (loopVar = 0; loopVar < engine_inputMappings.joystickMappings[joyLoopVar].numOfAxes; loopVar++) {
            // Negative
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickNAxisMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickNAxisMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u axis %u 0", joyLoopVar, loopVar);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickNAxisMappings[loopVar].list[eventLoopVar].modMask);
            }
            // Positive
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickPAxisMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickPAxisMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u axis %u 1", joyLoopVar, loopVar);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickPAxisMappings[loopVar].list[eventLoopVar].modMask);
            }
        }
        // Joystick HAT switches
        for (loopVar = 0; loopVar < engine_inputMappings.joystickMappings[joyLoopVar].numOfHats; loopVar++) {
            // Negative, horizontal
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickNHorizHatMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickNHorizHatMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u hat %u %u", joyLoopVar, loopVar, SDL_HAT_LEFT);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickNHorizHatMappings[loopVar].list[eventLoopVar].modMask);
            }
            // Negative, vertical
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickNVertHatMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickNVertHatMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u hat %u %u", joyLoopVar, loopVar, SDL_HAT_UP);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickNVertHatMappings[loopVar].list[eventLoopVar].modMask);
            }
            // Positive, horizontal
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickPHorizHatMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickPHorizHatMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u hat %u %u", joyLoopVar, loopVar, SDL_HAT_RIGHT);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickPHorizHatMappings[loopVar].list[eventLoopVar].modMask);
            }
            // Positive, vertical
            for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[joyLoopVar].joystickPVertHatMappings[loopVar].numOfEvents; eventLoopVar++) {
                if (CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING(engine_inputMappings.joystickMappings[joyLoopVar].joystickPVertHatMappings[loopVar].list[eventLoopVar])) {
                    continue;
                }
                fprintf(fp, " \"stick_%u hat %u %u", joyLoopVar, loopVar, SDL_HAT_DOWN);
                CVort_engine_writeEventModifiers(fp, engine_inputMappings.joystickMappings[joyLoopVar].joystickPVertHatMappings[loopVar].list[eventLoopVar].modMask);
            }
         }
    }

#undef CHOCOLATE_KEEN_ARE_EVENTS_NOT_MATCHING
    fprintf(fp, "\n");
}

void CVort_engine_saveInputMappings() {
    // In CONTRARY to mapper file loading, we open this in TEXTUAL mode.
    FILE *fp = CVort_engine_configpath_fopen(CHOCOLATE_KEEN_MAPPER_FILENAME, "w");
    if (!fp) {
        return;
    }
    MappedInputEvent_T testEvent;
#if SDL_VERSION_ATLEAST(2,0,0)
    fprintf(fp, "# Generated by Chocolate Keen, using SDL 2.0 scancodes for the key numbers.\n");
#else
    fprintf(fp, "# Generated by Chocolate Keen, using SDL 1.2 keysyms for the key numbers.\n");
#endif
    // We first want to print the emulated events, beginning with the keyboard
    testEvent.emulatedInput = EMULATEDINPUT_KEYPRESS;
    int loopVar;
    for (loopVar = 0; loopVar < engine_emulatedKeysTableCount; loopVar++) {
        testEvent.value = loopVar;
        fprintf(fp, "%s", engine_emulatedKeysTable[loopVar].name);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    // Mouse buttons come next
    testEvent.emulatedInput = EMULATEDINPUT_MOUSEBUTTONPRESS;
    for (loopVar = 0; loopVar < 3; loopVar++) {
        testEvent.value = (1 << loopVar);
        fprintf(fp, "%s", mapperClientMouseButtonNames[loopVar]);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    // Mouse motion
    testEvent.emulatedInput = EMULATEDINPUT_MOUSEMOTION;
    for (loopVar = 0; loopVar < 4; loopVar++) {
        testEvent.value = ((loopVar%2)*2-1) * (loopVar/2 + 1);
        fprintf(fp, "%s", mapperClientMouseAxisNames[loopVar]);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    // Joystick buttons
    fprintf(fp, "# NOTE: jbutton_1_0 is equivalent to jbutton_0_2.\n"
                "# Similarly jbutton_1_1 is equivalent to jbutton_0_3.\n"
                "# However, only ONE equivalent choice should be specified in each case.\n");
    testEvent.emulatedInput = EMULATEDINPUT_JOYBUTTONPRESS;
    for (loopVar = 0; loopVar < 4; loopVar++) {
        testEvent.value = (1 << loopVar);
        fprintf(fp, "%s", mapperClientJoyButtonNames[loopVar]);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    // Joystick motion
    testEvent.emulatedInput = EMULATEDINPUT_JOYMOTION;
    for (loopVar = 0; loopVar < 8; loopVar++) {
        testEvent.value = ((loopVar%2)*2-1) * (loopVar/2 + 1);
        fprintf(fp, "%s", mapperClientJoyAxisNames[loopVar]);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    // Internal handlers
    testEvent.emulatedInput = EMULATEDINPUT_HANDLER;
    for (loopVar = 0; loopVar < 3; loopVar++) {
        testEvent.value = loopVar + 1;
        fprintf(fp, "%s", mapperClientHandlerNames[loopVar]);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    // Modifiers
    testEvent.emulatedInput = EMULATEDINPUT_MODTOGGLE;
    for (loopVar = 0; loopVar < 3; loopVar++) {
        testEvent.value = (1 << loopVar);
        fprintf(fp, "%s", mapperClientModNames[loopVar]);
        CVort_engine_writeMappedHostInputs(fp, &testEvent);
    }
    fclose(fp);
}

// NOTE: 0 <= action < 32768, unless it comes from mouse motion where it can be
// up to 32767*CHOCOLATE_KEEN_EVENT_HANDLING_MOUSE_MOTION_SCALE_FACTOR.
void CVort_engine_handleEvent(const MappedInputEvent_T *pMappedEvent, int32_t action) {
    uint8_t dosScanCode;
    switch (pMappedEvent->emulatedInput) {
        case EMULATEDINPUT_KEYPRESS:
            dosScanCode = engine_emulatedKeysTable[pMappedEvent->value].dosScanCode;
            if ((action >= CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD &&
                pMappedEvent->modMask == (pMappedEvent->modMask & engine_inputMappings.currEmuInputStatus.modifiersMask))) {
                g_input.key_scane = dosScanCode | 0x80;
                g_input.key_map[dosScanCode] = 1;

                engine_lastScanCode = dosScanCode;
                engine_lastKeyTime = SDL_GetTicks();
                engine_isBeforeKeyDelay = true;
            } else {
                g_input.key_map[dosScanCode] = 0;
                if (dosScanCode == engine_lastScanCode)
                    engine_lastScanCode = 0;
            }
            break;
        case EMULATEDINPUT_MOUSEBUTTONPRESS:
            if ((action >= CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD &&
                pMappedEvent->modMask == (pMappedEvent->modMask & engine_inputMappings.currEmuInputStatus.modifiersMask))) {
                engine_inputMappings.currEmuInputStatus.mouseButtonsMask |= pMappedEvent->value;
            } else {
                engine_inputMappings.currEmuInputStatus.mouseButtonsMask &= ~pMappedEvent->value;
            }
            break;
        case EMULATEDINPUT_MOUSEMOTION:
            // Revert scaling IF it is the result of real mouse motion;
            // Otherwise, well, we scale anyway...
            action /= CHOCOLATE_KEEN_EVENT_HANDLING_MOUSE_MOTION_SCALE_FACTOR;
            switch (pMappedEvent->value) {
                case 1:
                    apply_mouse_delta_with_upper_bound(
                        &engine_inputMappings.currEmuInputStatus.mouseColumn,
                        action,
                        EMULATED_MOUSE_HORIZONTAL_RES - 1
                    );
                    break;
                case 2:
                    apply_mouse_delta_with_upper_bound(
                        &engine_inputMappings.currEmuInputStatus.mouseRow,
                        action,
                        EMULATED_MOUSE_VERTICAL_RES - 1
                    );
                    break;
                case -1:
                    apply_mouse_delta_with_lower_bound(
                        &engine_inputMappings.currEmuInputStatus.mouseColumn,
                        action
                    );
                    break;
                case -2:
                    apply_mouse_delta_with_lower_bound(
                        &engine_inputMappings.currEmuInputStatus.mouseRow,
                        action
                    );
                    break;
                default: ;
            }
            break;
        case EMULATEDINPUT_JOYBUTTONPRESS:
            if (action >= CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD) {
                engine_inputMappings.currEmuInputStatus.joystickButtonsMask |= pMappedEvent->value;
            } else {
                engine_inputMappings.currEmuInputStatus.joystickButtonsMask &= ~pMappedEvent->value;
            }
            break;
        case EMULATEDINPUT_JOYMOTION:
            // Convert SDL joystick axis values to the game's [0..500] gameport scale.
            switch (pMappedEvent->value) {
                case 1:
                case 2:
                case 3:
                case 4:
                    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[pMappedEvent->value-1] =
                        get_scaled_joystick_axis_poll(action);
                    break;
                case -1:
                case -2:
                case -3:
                case -4:
                    engine_inputMappings.currEmuInputStatus.joystickAxesPolls[-pMappedEvent->value-1] =
                        get_scaled_joystick_axis_poll(-action);
                    break;
                default: ;
            }
            break;
        case EMULATEDINPUT_HANDLER:
            if ((action >= CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD &&
                pMappedEvent->modMask == (pMappedEvent->modMask & engine_inputMappings.currEmuInputStatus.modifiersMask))) {
                handle_input_handler_event(pMappedEvent->value);
            }
            break;
        case EMULATEDINPUT_MODTOGGLE:
            if (action >= CHOCOLATE_KEEN_EVENT_HANDLING_THRESHOLD) {
                engine_inputMappings.currEmuInputStatus.modifiersMask |= pMappedEvent->value;
            } else {
                engine_inputMappings.currEmuInputStatus.modifiersMask &= ~pMappedEvent->value;
            }
            break;
        default: ;
    }
}

// NOTE: Joystick input handling is done separately.
// Known limitation: Emulated Pause shares a DOS scancode with Ctrl in legacy
// mappings, so "wait for key press" loops may consume a release event.
// NOTE: Window resize is intentionally handled in this poll loop because
// viewport/state updates are currently coupled with event processing.

void CVort_engine_updateInputStatus() {
    int eventLoopVar;
    int32_t scaledXDiff, scaledYDiff;
    SDL_Event event;
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_Scancode currSDLKeyId;
#else
    SDLKey currSDLKeyId;
#endif
    static uint32_t lastMouseMotionTime = 0;
    static bool isAnyMouseMotionDone = false;
    // First we handle events as usual
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
#if SDL_VERSION_ATLEAST(2,0,0)
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    CVort_engine_reactToWindowResize(event.window.data1, event.window.data2);
                } else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    CVort_engine_handleWindowSideChange();
		}
                break;
#else
            case SDL_VIDEORESIZE:
                CVort_engine_reactToWindowResize(event.resize.w, event.resize.h);
                break;
#endif
            case SDL_KEYDOWN:
            case SDL_KEYUP:
#if SDL_VERSION_ATLEAST(2,0,0)
                currSDLKeyId = event.key.keysym.scancode;
#else
                currSDLKeyId = event.key.keysym.sym;
#endif
                if (currSDLKeyId >= sizeof(engine_inputMappings.keyMappings)/sizeof(MappedInputEventList_T))
                    break; // If we don't break, a BUFFER OVERFLOW is possible!
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.keyMappings[currSDLKeyId].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.keyMappings[currSDLKeyId].list[eventLoopVar], (event.type == SDL_KEYDOWN) ? 32767 : 0);
                break;
	            case SDL_MOUSEBUTTONDOWN:
	                if (!engine_isCursorLocked) {
	                    CVort_engine_toggleCursorLock(true);
	                    break;
	                }
	                /* fall through */
	            case SDL_MOUSEBUTTONUP:
                if ((event.button.button < 1) || (event.button.button > 5))
                    break; // Again, without this a buffer overflow may occur...
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseButtonMappings[event.button.button-1].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.mouseButtonMappings[event.button.button-1].list[eventLoopVar], (event.type == SDL_MOUSEBUTTONDOWN) ? 32767 : 0);
                break;
            case SDL_MOUSEMOTION:
                if (!engine_isCursorLocked) {
                    break;
                }
                // Just in case these are NOT converted
                // to mouse motion events, scale.
                scaledXDiff = event.motion.xrel * CHOCOLATE_KEEN_EVENT_HANDLING_MOUSE_MOTION_SCALE_FACTOR;
                scaledYDiff = event.motion.yrel * CHOCOLATE_KEEN_EVENT_HANDLING_MOUSE_MOTION_SCALE_FACTOR;
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelPAxisMappings[0].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.mouseRelPAxisMappings[0].list[eventLoopVar], (scaledXDiff > 0) ? scaledXDiff : 0);
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelPAxisMappings[1].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.mouseRelPAxisMappings[1].list[eventLoopVar], (scaledYDiff > 0) ? scaledYDiff : 0);
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelNAxisMappings[0].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.mouseRelNAxisMappings[0].list[eventLoopVar], (scaledXDiff < 0) ? -scaledXDiff : 0);
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelNAxisMappings[1].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.mouseRelNAxisMappings[1].list[eventLoopVar], (scaledYDiff < 0) ? -scaledYDiff : 0);
                lastMouseMotionTime = SDL_GetTicks();
                isAnyMouseMotionDone = true;
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                if ((event.jbutton.which >= engine_inputMappings.numOfJoysticks) || (event.jbutton.button >= engine_inputMappings.joystickMappings[event.jbutton.which].numOfButtons))
                    break; // No overflow again...
                for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jbutton.which].joystickButtonMappings[event.jbutton.button].numOfEvents; eventLoopVar++)
                    CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jbutton.which].joystickButtonMappings[event.jbutton.button].list[eventLoopVar], (event.type == SDL_JOYBUTTONDOWN) ? 32767 : 0);
                break;
            case SDL_JOYAXISMOTION:
                if ((event.jaxis.which >= engine_inputMappings.numOfJoysticks) || (event.jaxis.axis >= engine_inputMappings.joystickMappings[event.jbutton.which].numOfAxes))
                    break; // ...
                // The internal orders of calls (e.g. negative first, positive second) IS IMPORTANT!!!
                if (event.jaxis.value >= 0) {
                    // First disable any possible event for the opposite direction
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jaxis.which].joystickNAxisMappings[event.jaxis.axis].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jaxis.which].joystickNAxisMappings[event.jaxis.axis].list[eventLoopVar], 0);
                    // If we called this before, things could go wrong.
                    // (Think of emulating motion to the right first and then zero horizontal motion.)
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jaxis.which].joystickPAxisMappings[event.jaxis.axis].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jaxis.which].joystickPAxisMappings[event.jaxis.axis].list[eventLoopVar], event.jaxis.value);
                } else {
                    // First disable any possible event for the opposite direction
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jaxis.which].joystickPAxisMappings[event.jaxis.axis].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jaxis.which].joystickPAxisMappings[event.jaxis.axis].list[eventLoopVar], 0);
                    // If we called this before, things could go wrong.
                    // (Think of emulating motion to the left first and then zero horizontal motion.)
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jaxis.which].joystickNAxisMappings[event.jaxis.axis].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jaxis.which].joystickNAxisMappings[event.jaxis.axis].list[eventLoopVar], -event.jaxis.value-1);
                }
                break;
            case SDL_JOYHATMOTION:
                if ((event.jhat.which >= engine_inputMappings.numOfJoysticks) || (event.jhat.hat >= engine_inputMappings.joystickMappings[event.jbutton.which].numOfHats))
                    break; // ...
                // The internal orders of calls (e.g. negative first, positive second) IS IMPORTANT!!!

                // First handle horizontal directions
                if (!(event.jhat.value & SDL_HAT_LEFT)) { // NOT pointing left
                    // First disable any possible event for the opposite direction
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickNHorizHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickNHorizHatMappings[event.jhat.hat].list[eventLoopVar], 0);
                    // If we called this before, things could go wrong.
                    // (Think of emulating motion to the right first and then zero horizontal motion.)
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickPHorizHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickPHorizHatMappings[event.jhat.hat].list[eventLoopVar], (event.jhat.value & SDL_HAT_RIGHT) ? 32767 : 0);
                } else { // Pointing left
                    // First disable any possible event for the opposite direction
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickPHorizHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickPHorizHatMappings[event.jhat.hat].list[eventLoopVar], 0);
                    // If we called this before, things could go wrong.
                    // (Think of emulating motion to the left first and then zero horizontal motion.)
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickNHorizHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickNHorizHatMappings[event.jhat.hat].list[eventLoopVar], (event.jhat.value & SDL_HAT_LEFT) ? 32767 : 0);
                }
                // Repeat with the vertical ones
                if (!(event.jhat.value & SDL_HAT_UP)) { // NOT pointing up
                    // First disable any possible event for the opposite direction
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickNVertHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickNVertHatMappings[event.jhat.hat].list[eventLoopVar], 0);
                    // If we called this before, things could go wrong.
                    // (Think of emulating motion to the right first and then zero horizontal motion.)
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickPVertHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickPVertHatMappings[event.jhat.hat].list[eventLoopVar], (event.jhat.value & SDL_HAT_DOWN) ? 32767 : 0);
                } else { // Pointing up
                    // First disable any possible event for the opposite direction
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickPVertHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickPVertHatMappings[event.jhat.hat].list[eventLoopVar], 0);
                    // If we called this before, things could go wrong.
                    // (Think of emulating motion to the left first and then zero horizontal motion.)
                    for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.joystickMappings[event.jhat.which].joystickNVertHatMappings[event.jhat.hat].numOfEvents; eventLoopVar++)
                        CVort_engine_handleEvent(&engine_inputMappings.joystickMappings[event.jhat.which].joystickNVertHatMappings[event.jhat.hat].list[eventLoopVar], (event.jhat.value & SDL_HAT_UP) ? 32767 : 0);
                }
                break;
            case SDL_QUIT:
                shutdown_and_exit(0);
            default:;
        }
    }
    // We may further "disable" relative motion events specially
    // Simulate key repeat/delay
    if (isAnyMouseMotionDone && (SDL_GetTicks() - lastMouseMotionTime >= 10)) {
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelPAxisMappings[0].numOfEvents; eventLoopVar++)
            CVort_engine_handleEvent(&engine_inputMappings.mouseRelPAxisMappings[0].list[eventLoopVar], 0);
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelPAxisMappings[1].numOfEvents; eventLoopVar++)
            CVort_engine_handleEvent(&engine_inputMappings.mouseRelPAxisMappings[1].list[eventLoopVar], 0);
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelNAxisMappings[0].numOfEvents; eventLoopVar++)
            CVort_engine_handleEvent(&engine_inputMappings.mouseRelNAxisMappings[0].list[eventLoopVar], 0);
        for (eventLoopVar = 0; eventLoopVar < engine_inputMappings.mouseRelNAxisMappings[1].numOfEvents; eventLoopVar++)
            CVort_engine_handleEvent(&engine_inputMappings.mouseRelNAxisMappings[1].list[eventLoopVar], 0);
    }
    if (engine_lastScanCode) {
        if (engine_isBeforeKeyDelay) {
            if ((SDL_GetTicks() - engine_lastKeyTime) >= 500) {
                engine_lastKeyTime = SDL_GetTicks();
                g_input.key_scane = engine_lastScanCode | 0x80;
                g_input.key_map[engine_lastScanCode] = 1;
                engine_isBeforeKeyDelay = false;
            }
        } else if ((SDL_GetTicks() - engine_lastKeyTime)*3 >= 100) {
            // Since it's immediate...
            //engine_lastKeyTime = SDL_GetTicks();
            g_input.key_scane = engine_lastScanCode | 0x80;
            g_input.key_map[engine_lastScanCode] = 1;
        }
    }
}
