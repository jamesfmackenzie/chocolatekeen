/*
 * test_input_mapping_nav.c - Focused tests for host input mapping traversal
 */

#include "core/globals.h"
#include "test_framework.h"

/* Minimal stubs required by src/input/input.c for linking in tests */
FILE *CVort_engine_configpath_fopen(const char *filename, const char *mode) {
    (void)filename;
    (void)mode;
    return NULL;
}

void CVort_engine_cross_logMessage(CVort_Log_Message_Class_T msgClass, const char *format, ...) {
    (void)msgClass;
    (void)format;
}

uint32_t CVort_filelength(FILE *fp) {
    (void)fp;
    return 0;
}

void CK_PlatformPrepareInput(void) {
}

void CK_PlatformSleepMs(unsigned int ms) {
    (void)ms;
}

void CVort_engine_shutdown(void) {
}

void CVort_engine_reactToWindowResize(int x, int y) {
    (void)x;
    (void)y;
}

void CVort_engine_handleWindowSideChange(void) {
}

void CVort_engine_updateActualDisplay(void) {
}

void CVort_engine_toggleCursorLock(bool toggle) {
    (void)toggle;
}

bool CVort_engine_resetWindow(void) {
    return true;
}

static MappedInputEventList_T *get_test_input_event_list(HostInput_T inputT, int inputId, int inputVal) {
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
        if (inputVal > 0) {
            return &engine_inputMappings.joystickMappings[inputId].joystickPAxisMappings[inputVal-1];
        }
        if (inputVal < 0) {
            return &engine_inputMappings.joystickMappings[inputId].joystickNAxisMappings[-inputVal-1];
        }
        return NULL;
    case HOSTINPUT_JOYHAT:
        if (inputVal > 0) {
            if (inputVal % 2) {
                return &engine_inputMappings.joystickMappings[inputId].joystickPHorizHatMappings[(inputVal-1)/2];
            }
            return &engine_inputMappings.joystickMappings[inputId].joystickPVertHatMappings[(inputVal-2)/2];
        }
        if (inputVal < 0) {
            if (inputVal % 2) {
                return &engine_inputMappings.joystickMappings[inputId].joystickNHorizHatMappings[(-inputVal-1)/2];
            }
            return &engine_inputMappings.joystickMappings[inputId].joystickNVertHatMappings[(-inputVal-2)/2];
        }
        return NULL;
    default:
        return NULL;
    }
}

static void setup_test_joysticks(int numJoysticks) {
    int joy;

    memset(&engine_inputMappings, 0, sizeof(engine_inputMappings));
    engine_inputMappings.numOfJoysticks = numJoysticks;
    if (numJoysticks == 0) {
        return;
    }

    engine_inputMappings.joystickMappings = calloc(numJoysticks, sizeof(HostJoystickMapping_T));
    TEST_ASSERT(engine_inputMappings.joystickMappings != NULL);

    engine_inputMappings.sdlJoysticks = calloc(numJoysticks, sizeof(SDL_Joystick *));
    TEST_ASSERT(engine_inputMappings.sdlJoysticks != NULL);

    for (joy = 0; joy < numJoysticks; joy++) {
        engine_inputMappings.joystickMappings[joy].numOfButtons = 0;
        engine_inputMappings.joystickMappings[joy].numOfAxes = 0;
        engine_inputMappings.joystickMappings[joy].numOfHats = 0;
    }
}

static void set_test_joystick_shape(int joy, int buttons, int axes, int hats) {
    HostJoystickMapping_T *map = &engine_inputMappings.joystickMappings[joy];

    map->numOfButtons = buttons;
    map->numOfAxes = axes;
    map->numOfHats = hats;
    if (buttons > 0) {
        map->joystickButtonMappings = calloc((size_t)buttons, sizeof(MappedInputEventList_T));
        TEST_ASSERT(map->joystickButtonMappings != NULL);
    }
    if (axes > 0) {
        map->joystickPAxisMappings = calloc((size_t)axes, sizeof(MappedInputEventList_T));
        map->joystickNAxisMappings = calloc((size_t)axes, sizeof(MappedInputEventList_T));
        TEST_ASSERT(map->joystickPAxisMappings != NULL);
        TEST_ASSERT(map->joystickNAxisMappings != NULL);
    }
    if (hats > 0) {
        map->joystickPHorizHatMappings = calloc((size_t)hats, sizeof(MappedInputEventList_T));
        map->joystickNHorizHatMappings = calloc((size_t)hats, sizeof(MappedInputEventList_T));
        map->joystickPVertHatMappings = calloc((size_t)hats, sizeof(MappedInputEventList_T));
        map->joystickNVertHatMappings = calloc((size_t)hats, sizeof(MappedInputEventList_T));
        TEST_ASSERT(map->joystickPHorizHatMappings != NULL);
        TEST_ASSERT(map->joystickNHorizHatMappings != NULL);
        TEST_ASSERT(map->joystickPVertHatMappings != NULL);
        TEST_ASSERT(map->joystickNVertHatMappings != NULL);
    }
}

static void teardown_test_joysticks(void) {
    int joy;
    for (joy = 0; joy < engine_inputMappings.numOfJoysticks; joy++) {
        free(engine_inputMappings.joystickMappings[joy].joystickButtonMappings);
        free(engine_inputMappings.joystickMappings[joy].joystickPAxisMappings);
        free(engine_inputMappings.joystickMappings[joy].joystickNAxisMappings);
        free(engine_inputMappings.joystickMappings[joy].joystickPHorizHatMappings);
        free(engine_inputMappings.joystickMappings[joy].joystickNHorizHatMappings);
        free(engine_inputMappings.joystickMappings[joy].joystickPVertHatMappings);
        free(engine_inputMappings.joystickMappings[joy].joystickNVertHatMappings);
    }
    free(engine_inputMappings.joystickMappings);
    free(engine_inputMappings.sdlJoysticks);
    memset(&engine_inputMappings, 0, sizeof(engine_inputMappings));
}

static void add_mapping(
    HostInput_T hostInputType, int hostInputId, int hostInputVal,
    EmulatedInput_T emuInputType, int emuValue
) {
    MappedInputEventList_T *entryList = get_test_input_event_list(hostInputType, hostInputId, hostInputVal);
    TEST_ASSERT(entryList != NULL);
    entryList->list[0].emulatedInput = emuInputType;
    entryList->list[0].value = emuValue;
    entryList->list[0].modMask = 0;
    entryList->numOfEvents = 1;
}

TEST_SUITE(test_input_mapping_next_skips_unmapped_entries) {
    HostInput_T outType;
    int outId, outVal;
    MappedInputEvent_T *result;

    setup_test_joysticks(0);
    add_mapping(HOSTINPUT_MOUSEBUTTONPRESS, 0, 2, EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A);

    result = CVort_engine_getNextInputMapping(
        EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A,
        HOSTINPUT_KEYPRESS, 0, 0,
        &outType, &outId, &outVal
    );
    TEST_ASSERT(result != NULL);
    TEST_ASSERT_EQUAL(HOSTINPUT_MOUSEBUTTONPRESS, outType);
    TEST_ASSERT_EQUAL(0, outId);
    TEST_ASSERT_EQUAL(2, outVal);

    teardown_test_joysticks();
}

TEST_SUITE(test_input_mapping_next_skips_empty_joystick_slots) {
    HostInput_T outType;
    int outId, outVal;
    MappedInputEvent_T *result;

    setup_test_joysticks(2);
    set_test_joystick_shape(0, 0, 0, 0);
    set_test_joystick_shape(1, 1, 0, 0);
    add_mapping(HOSTINPUT_JOYBUTTONPRESS, 1, 0, EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A);

    result = CVort_engine_getNextInputMapping(
        EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A,
        HOSTINPUT_MOUSEMOTION, 0, -2,
        &outType, &outId, &outVal
    );
    TEST_ASSERT(result != NULL);
    TEST_ASSERT_EQUAL(HOSTINPUT_JOYBUTTONPRESS, outType);
    TEST_ASSERT_EQUAL(1, outId);
    TEST_ASSERT_EQUAL(0, outVal);

    teardown_test_joysticks();
}

TEST_SUITE(test_input_mapping_prev_wraps_to_last_entry) {
    HostInput_T outType;
    int outId, outVal;
    MappedInputEvent_T *result;

    setup_test_joysticks(1);
    set_test_joystick_shape(0, 0, 0, 1);
    add_mapping(HOSTINPUT_JOYHAT, 0, -2, EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A);

    result = CVort_engine_getPrevInputMapping(
        EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A,
        HOSTINPUT_KEYPRESS, 0, 10,
        &outType, &outId, &outVal
    );
    TEST_ASSERT(result != NULL);
    TEST_ASSERT_EQUAL(HOSTINPUT_JOYHAT, outType);
    TEST_ASSERT_EQUAL(0, outId);
    TEST_ASSERT_EQUAL(-2, outVal);

    teardown_test_joysticks();
}

TEST_SUITE(test_input_mapping_prev_next_roundtrip_single_mapping) {
    HostInput_T outType, outType2;
    int outId, outVal, outId2, outVal2;
    MappedInputEvent_T *nextResult;
    MappedInputEvent_T *prevResult;

    setup_test_joysticks(0);
    add_mapping(HOSTINPUT_KEYPRESS, 0, 7, EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A);

    nextResult = CVort_engine_getNextInputMapping(
        EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A,
        HOSTINPUT_KEYPRESS, 0, 7,
        &outType, &outId, &outVal
    );
    TEST_ASSERT(nextResult != NULL);
    TEST_ASSERT_EQUAL(HOSTINPUT_KEYPRESS, outType);
    TEST_ASSERT_EQUAL(0, outId);
    TEST_ASSERT_EQUAL(7, outVal);

    prevResult = CVort_engine_getPrevInputMapping(
        EMULATEDINPUT_KEYPRESS, EMULATEDKEYINDEX_A,
        outType, outId, outVal,
        &outType2, &outId2, &outVal2
    );
    TEST_ASSERT(prevResult != NULL);
    TEST_ASSERT_EQUAL(HOSTINPUT_KEYPRESS, outType2);
    TEST_ASSERT_EQUAL(0, outId2);
    TEST_ASSERT_EQUAL(7, outVal2);

    teardown_test_joysticks();
}

TEST_SUITE(test_input_mapping_nav) {
    RUN_TEST(test_input_mapping_next_skips_unmapped_entries);
    RUN_TEST(test_input_mapping_next_skips_empty_joystick_slots);
    RUN_TEST(test_input_mapping_prev_wraps_to_last_entry);
    RUN_TEST(test_input_mapping_prev_next_roundtrip_single_mapping);
}
