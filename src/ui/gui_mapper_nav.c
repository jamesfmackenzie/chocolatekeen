#include "core/globals.h"
#include "ui/gui_mapper_nav.h"
#include "ui/gui_types.h"

static GUI_Mapper_Tile_T **CVort_gui_mapper_getFirstSelectableTilePtr(GUI_Mapper_Page_T *page) {
	for (GUI_Mapper_Tile_T **tilePtr = page->tiles; *tilePtr; tilePtr++) {
		if ((*tilePtr)->emuEvent.emulatedInput != EMULATEDINPUT_NONE) {
			return tilePtr;
		}
	}
	return 0;
}

void CVort_gui_mapper_moveSelection(
	GUI_Mapper_Page_T *page,
	GUI_Mapper_Tile_T ***currentTilePtr,
	GUI_Mapper_MoveDirection_T direction
) {
	if (!(*currentTilePtr)) {
		*currentTilePtr = CVort_gui_mapper_getFirstSelectableTilePtr(page);
		return;
	}

	switch (direction) {
	case GUI_MAPPER_MOVE_LEFT:
		if ((*(*currentTilePtr))->nearTiles.left) {
			*currentTilePtr = &((*(*currentTilePtr))->nearTiles.left);
		}
		break;
	case GUI_MAPPER_MOVE_RIGHT:
		if ((*(*currentTilePtr))->nearTiles.right) {
			*currentTilePtr = &((*(*currentTilePtr))->nearTiles.right);
		}
		break;
	case GUI_MAPPER_MOVE_UP:
		if ((*(*currentTilePtr))->nearTiles.up) {
			*currentTilePtr = &((*(*currentTilePtr))->nearTiles.up);
		}
		break;
	case GUI_MAPPER_MOVE_DOWN:
		if ((*(*currentTilePtr))->nearTiles.down) {
			*currentTilePtr = &((*(*currentTilePtr))->nearTiles.down);
		}
		break;
	}
}
