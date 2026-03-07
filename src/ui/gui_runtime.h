#ifndef _CHOCOLATE_KEEN_GUI_RUNTIME_H_
#define _CHOCOLATE_KEEN_GUI_RUNTIME_H_

#include "ui/gui_data.h"
#include "ui/gui_handlers.h"

typedef struct {
	int w, h;
} GUI_VideoMode_T;

extern GUI_Menu_T *guiCurrentMenuPtr;
extern GUI_Menu_Item_T **guiCurrentMenuItemSelectionPtr;
extern GUI_VideoMode_T *guiVideoModes;
extern const char **guiFullScreenModeStrs;
extern const char **guiWindowedModeStrs;

void CVort_gui_setCurrentMenu(GUI_Menu_T *menu);
void CVort_gui_drawCurrentMenu(void);
void CVort_gui_mapper_drawCurrentPage(void);
void CVort_gui_mapper_setCurrentPage(GUI_Mapper_Page_T *page);
void CVort_gui_mapper_runLoop(void);
void CVort_gui_deleteScreenResolutionBuffers(void);
int CVort_gui_createScreenResolutionBuffers(void);
void CVort_gui_setChoicesBuffer(GUI_Menu_Item_T *item, const char **choices);
void CVort_gui_refreshMapperMenu(void);

#endif
