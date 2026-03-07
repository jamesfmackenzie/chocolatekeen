// gui_mapper_menu.c: Implements gui mapper menu for the ui subsystem.

#include <stdio.h>

#include "core/globals.h"
#include "input/input_names.h"
#include "ui/gui_internal.h"
#include "ui/gui_runtime.h"

static void CVort_gui_refreshMapperMenuWithSomeBinding(void) {
	guiMapperMenu.items = guiMenuItemsMapperSomeBindings;
	guiMenuItemMapperBindingModifiers.value = guiCurrentMappedInputEvent->modMask;
	switch (guiCurrentHostEventDetails.inputT) {
	case HOSTINPUT_KEYPRESS:
#if SDL_VERSION_ATLEAST(2,0,0)
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Key %s (SC %d)", SDL_GetScancodeName(guiCurrentHostEventDetails.inputVal), guiCurrentHostEventDetails.inputVal);
#else
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Key %s (KS %d)", SDL_GetKeyName(guiCurrentHostEventDetails.inputVal), guiCurrentHostEventDetails.inputVal);
#endif
		break;
	case HOSTINPUT_MOUSEBUTTONPRESS:
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse button no. %d", guiCurrentHostEventDetails.inputVal);
		break;
	case HOSTINPUT_MOUSEMOTION:
		switch (guiCurrentHostEventDetails.inputVal) {
		case 1:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement right");
			break;
		case -1:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement left");
			break;
		case 2:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement down");
			break;
		case -2:
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Mouse movement up");
			break;
		}
		break;
	case HOSTINPUT_JOYBUTTONPRESS:
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d button no. %d", guiCurrentHostEventDetails.inputId, guiCurrentHostEventDetails.inputVal);
		break;
	case HOSTINPUT_JOYMOTION:
		if (guiCurrentHostEventDetails.inputVal < 0) {
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d axis %d-", guiCurrentHostEventDetails.inputId, -guiCurrentHostEventDetails.inputVal-1);
		} else {
			snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d axis %d+", guiCurrentHostEventDetails.inputId, guiCurrentHostEventDetails.inputVal-1);
		}
		break;
	case HOSTINPUT_JOYHAT:
		if (guiCurrentHostEventDetails.inputVal < 0) {
			if (guiCurrentHostEventDetails.inputVal % 2 != 0) {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d left", guiCurrentHostEventDetails.inputId, (-guiCurrentHostEventDetails.inputVal-1)/2);
			} else {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d up", guiCurrentHostEventDetails.inputId, (-guiCurrentHostEventDetails.inputVal-2)/2);
			}
		} else {
			if (guiCurrentHostEventDetails.inputVal % 2 != 0) {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d right", guiCurrentHostEventDetails.inputId, (guiCurrentHostEventDetails.inputVal-1)/2);
			} else {
				snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Joystick %d hat %d down", guiCurrentHostEventDetails.inputId, (guiCurrentHostEventDetails.inputVal-2)/2);
			}
		}
		break;
	default:
		snprintf(guiMapperMenuBindingString, sizeof(guiMapperMenuBindingString), "Binding: Unknown, this shouldn't happen");
	}
	guiMenuItemMapperBindingLabel.label = guiMapperMenuBindingString;
	CVort_gui_layoutMenu(&guiMapperMenu);
	CVort_gui_setCurrentMenu(&guiMapperMenu);
}

static void CVort_gui_refreshMapperMenuWithNoBinding(void) {
	guiMapperMenu.items = guiMenuItemsMapperNoBinding;
	CVort_gui_layoutMenu(&guiMapperMenu);
	CVort_gui_setCurrentMenu(&guiMapperMenu);
}

void CVort_gui_refreshMapperMenu(void) {
	if (guiCurrentMappedInputEvent) {
		CVort_gui_refreshMapperMenuWithSomeBinding();
	} else {
		CVort_gui_refreshMapperMenuWithNoBinding();
	}
}

static char guiMapperMenuTitle[39];

void CVort_gui_showMapperMenu(void) {
	guiCurrentMappedInputEvent = CVort_engine_getNextInputMapping(guiCurrentEmuEventDetails.emulatedInput, guiCurrentEmuEventDetails.value,
	                                                              HOSTINPUT_NONE, 0, 0,
	                                                              &guiCurrentHostEventDetails.inputT,
	                                                              &guiCurrentHostEventDetails.inputId,
	                                                              &guiCurrentHostEventDetails.inputVal
	);
	switch (guiCurrentEmuEventDetails.emulatedInput) {
	case EMULATEDINPUT_KEYPRESS:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", engine_emulatedKeysTable[guiCurrentEmuEventDetails.value].name);
		break;
	case EMULATEDINPUT_MOUSEBUTTONPRESS:
		// 1 is first (left, counting from 0), 2 second (right) and 4 third (middle)
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientMouseButtonNames[guiCurrentEmuEventDetails.value-1-((guiCurrentEmuEventDetails.value&4)>>2)]);
		break;
	case EMULATEDINPUT_MOUSEMOTION:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientMouseAxisNames[(guiCurrentEmuEventDetails.value < 0) ? (-guiCurrentEmuEventDetails.value-1)*2 : (guiCurrentEmuEventDetails.value-1)*2+1]);
		break;
	case EMULATEDINPUT_JOYBUTTONPRESS:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientJoyButtonNames[guiCurrentEmuEventDetails.value]);
		break;
	case EMULATEDINPUT_JOYMOTION:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientJoyAxisNames[(guiCurrentEmuEventDetails.value < 0) ? (-guiCurrentEmuEventDetails.value-1)*2 : (guiCurrentEmuEventDetails.value-1)*2+1]);
		break;
	case EMULATEDINPUT_HANDLER:
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientHandlerNames[guiCurrentEmuEventDetails.value-1]);
		break;
	case EMULATEDINPUT_MODTOGGLE:
		// 1 is first (counting from 0), 2 second and 4 third
		snprintf(guiMapperMenuTitle, sizeof(guiMapperMenuTitle), "Emulated event %s", mapperClientModNames[guiCurrentEmuEventDetails.value-1-((guiCurrentEmuEventDetails.value&4)>>2)]);
		break;
	}
	guiMapperMenu.title = guiMapperMenuTitle;
	CVort_gui_refreshMapperMenu();
}
