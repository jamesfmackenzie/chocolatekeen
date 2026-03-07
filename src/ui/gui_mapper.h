// gui_mapper.h: Declares gui mapper interfaces for the ui subsystem.

#ifndef _CHOCOLATE_KEEN_GUI_MAPPER_H_
#define _CHOCOLATE_KEEN_GUI_MAPPER_H_

#include "ui/gui_types.h"

void CVort_gui_mapper_drawCurrentPage(void);
void CVort_gui_mapper_setCurrentPage(GUI_Mapper_Page_T *page);
void CVort_gui_mapper_runLoop(void);

#endif
