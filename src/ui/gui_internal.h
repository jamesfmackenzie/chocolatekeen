// gui_internal.h: Declares gui internal interfaces for the ui subsystem.

#ifndef _CHOCOLATE_KEEN_GUI_INTERNAL_H_
#define _CHOCOLATE_KEEN_GUI_INTERNAL_H_

#include "ui/gui_types.h"

void CVort_gui_layoutMenu(GUI_Menu_T *menu);
void CVort_gui_prepareMenuItemsChoiceBuffers(void);
void CVort_gui_setChoicesBuffer(GUI_Menu_Item_T *item, const char **choices);

#endif
