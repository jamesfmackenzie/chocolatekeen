COMMON_OBJECTS=$(OBJ)/game_gameplay.o \
        $(OBJ)/game_worldmap.o \
        $(OBJ)/game_menus.o \
        $(OBJ)/game_ui.o \
        $(OBJ)/game_physics.o \
        $(OBJ)/game_enemies.o \
        $(OBJ)/game_episode_constants.o \
        $(OBJ)/render_memdata.o \
        $(OBJ)/engine_engine.o \
        $(OBJ)/engine_engine_audio.o \
        $(OBJ)/engine_engine_config.o \
        $(OBJ)/engine_engine_io.o \
        $(OBJ)/engine_engine_loader.o \
        $(OBJ)/engine_engine_timing.o \
        $(OBJ)/render_gfx.o \
        $(OBJ)/input_input.o \
        $(OBJ)/input_input_default_mappings.o \
        $(OBJ)/ui_gui.o \
        $(OBJ)/ui_gui_data.o \
        $(OBJ)/ui_gui_handlers.o \
        $(OBJ)/ui_gui_mapper_menu.o \
        $(OBJ)/ui_gui_menu_loop.o \
        $(OBJ)/ui_gui_loop_common.o \
        $(OBJ)/ui_gui_mapper_nav.o \
        $(OBJ)/ui_gui_mapper.o \
	$(OBJ)/lz.o \
	$(OBJ)/CRLE.o \
	$(OBJ)/Cunlzexe.o \
	$(OBJ)/imageRLE.o \
	$(OBJ)/chocolate-keen.o

EPISODE_OBJECTS=
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 1))
	EPISODE_OBJECTS+= $(OBJ)/episodes_episode1.o \
	                  $(OBJ)/episodes_episode1_memdata.o \
	                  $(OBJ)/episodes_episode1_engine.o
endif
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 2))
	EPISODE_OBJECTS+= $(OBJ)/episodes_episode2.o \
	                  $(OBJ)/episodes_episode2_memdata.o \
	                  $(OBJ)/episodes_episode2_engine.o
endif
ifeq ($(SINGLE_EPISODE),$(filter $(SINGLE_EPISODE),0 3))
	EPISODE_OBJECTS+= $(OBJ)/episodes_episode3.o \
	                  $(OBJ)/episodes_episode3_memdata.o \
	                  $(OBJ)/episodes_episode3_engine.o
endif
