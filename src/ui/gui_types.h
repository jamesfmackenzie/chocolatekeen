// gui_types.h: Declares gui types interfaces for the ui subsystem.

#ifndef _CHOCOLATE_KEEN_GUI_TYPES_H_
#define _CHOCOLATE_KEEN_GUI_TYPES_H_

#include <stdbool.h>

#include "input/input.h"

typedef struct GUI_Menu_Item_Struct GUI_Menu_Item_T;
typedef void GUI_Menu_Item_HandlerNonPtr_T(GUI_Menu_Item_T *);
typedef GUI_Menu_Item_HandlerNonPtr_T *GUI_Menu_Item_Handler_T;

struct GUI_Menu_Item_Struct {
    int x, y, text_y, width, height, choiceMaxPixWidth;
    const char *label;
    const char **choices;
    int value, numOfChoices;
    void (*handler)(struct GUI_Menu_Item_Struct *thisItem); // Called upon activation
};

typedef struct GUI_Menu_Struct GUI_Menu_T;

struct GUI_Menu_Struct {
    int title_x, title_y;
    const char *title;
    int width;
    GUI_Menu_Item_T **items;
    struct GUI_Menu_Struct *backPage, *prevPage, *nextPage;
    // An alternative for backPage (used in the internal mapper menu)
    void (*altBackHandler)(void);
    // Don't draw back button even if there's a back page/handler
    // (but accept input from a dedicated back button like the Escape key).
    // Used when there is already a button in the menu for that.
    bool hideBackButton;
};

typedef struct GUI_Mapper_Tile_Struct GUI_Mapper_Tile_T;

typedef struct GUI_Mapper_NearTiles_Struct {
    GUI_Mapper_Tile_T *left, *right, *up, *down;
} GUI_Mapper_NearTiles_T;

struct GUI_Mapper_Tile_Struct {
    int x, y, width, height;
    const char *topLeftLabel, *midLeftLabel, *botLeftLabel;
    struct {
        EmulatedInput_T emulatedInput;
        int value;
    } emuEvent;
    // To move between tiles in the UI with a keyboard/d-pad
    GUI_Mapper_NearTiles_T nearTiles;
};

typedef struct GUI_Mapper_Page_Struct GUI_Mapper_Page_T;

struct GUI_Mapper_Page_Struct {
    GUI_Mapper_Tile_T **tiles;
    /* There is never a "back" mapper page, but it's always possible
     * to leave a mapper page and return to the usual launcher UI.
     */
    struct GUI_Mapper_Page_Struct *prevPage, *nextPage;
};

#endif
