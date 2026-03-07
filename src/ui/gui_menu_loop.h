#ifndef _CHOCOLATE_KEEN_GUI_MENU_LOOP_H_
#define _CHOCOLATE_KEEN_GUI_MENU_LOOP_H_

#include <stdbool.h>

void CVort_gui_runLoop(void);

bool CVort_gui_isBackButtonSelectedByMouse(int x, int y);
bool CVort_gui_isPrevButtonSelectedByMouse(int x, int y);
bool CVort_gui_isNextButtonSelectedByMouse(int x, int y);

void CVort_gui_drawBackButton(bool isMouseSelected);
void CVort_gui_drawPrevButton(bool isMouseSelected);
void CVort_gui_drawNextButton(bool isMouseSelected);

#endif
