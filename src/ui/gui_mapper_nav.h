// gui_mapper_nav.h: Declares gui mapper nav interfaces for the ui subsystem.

#ifndef _CHOCOLATE_KEEN_GUI_MAPPER_NAV_H_
#define _CHOCOLATE_KEEN_GUI_MAPPER_NAV_H_

typedef struct GUI_Mapper_Page_Struct GUI_Mapper_Page_T;
typedef struct GUI_Mapper_Tile_Struct GUI_Mapper_Tile_T;

typedef enum {
	GUI_MAPPER_MOVE_LEFT,
	GUI_MAPPER_MOVE_RIGHT,
	GUI_MAPPER_MOVE_UP,
	GUI_MAPPER_MOVE_DOWN
} GUI_Mapper_MoveDirection_T;

void CVort_gui_mapper_moveSelection(
	GUI_Mapper_Page_T *page,
	GUI_Mapper_Tile_T ***currentTilePtr,
	GUI_Mapper_MoveDirection_T direction
);

#endif
