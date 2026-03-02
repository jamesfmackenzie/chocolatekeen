$(OBJ)/chocolate-keen_icon.o: $(RSRC)/chocolate-keen.rc
	$(WINDRES) $< -O coff -o $@

$(OBJ)/lz.o: $(SRC)/third_party/cgenius/fileio/lz.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/CRLE.o: $(SRC)/third_party/cgenius/fileio/compression/CRLE.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/Cunlzexe.o: $(SRC)/third_party/cgenius/fileio/compression/Cunlzexe.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/imageRLE.o: $(SRC)/decompression/imageRLE.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/chocolate-keen.o: $(SRC)/app/chocolate-keen.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_gameplay.o: $(SRC)/game/gameplay.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode1.o: $(SRC)/episodes/episode1.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode1_memdata.o: $(SRC)/episodes/episode1_memdata.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode1_engine.o: $(SRC)/episodes/episode1_engine.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode2.o: $(SRC)/episodes/episode2.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode2_memdata.o: $(SRC)/episodes/episode2_memdata.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode2_engine.o: $(SRC)/episodes/episode2_engine.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode3.o: $(SRC)/episodes/episode3.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode3_memdata.o: $(SRC)/episodes/episode3_memdata.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/episodes_episode3_engine.o: $(SRC)/episodes/episode3_engine.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_worldmap.o: $(SRC)/game/worldmap.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_menus.o: $(SRC)/game/menus.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_ui.o: $(SRC)/game/ui.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_physics.o: $(SRC)/game/physics.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_enemies.o: $(SRC)/game/enemies.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/game_episode_constants.o: $(SRC)/game/episode_constants.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/render_memdata.o: $(SRC)/render/memdata.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/engine_engine.o: $(SRC)/engine/engine.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/engine_engine_io.o: $(SRC)/engine/engine_io.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/render_gfx.o: $(SRC)/render/gfx.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/input_input.o: $(SRC)/input/input.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/ui_gui.o: $(SRC)/ui/gui.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(PLATFORM_SLEEP_OBJECT): $(PLATFORM_SLEEP_SOURCE)
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(PLATFORM_INPUT_OBJECT): $(PLATFORM_INPUT_SOURCE)
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
$(OBJ)/%.o: $(SRC)/%.c
	$(CXX) -c $(INTCXXFLAGS) $(CXXFLAGS) $< -o $@
