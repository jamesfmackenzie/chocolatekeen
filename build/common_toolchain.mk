ifneq ($(filter $(BUILD_PROFILE),emscripten emscripten-legacy),)
	# Emscripten flags are set in common_profiles.mk by BUILD_PROFILE.
	INTCXXFLAGS+= -I$(SRC) $(EMCC_CFLAGS) $(EMCC_WARN_CFLAGS)
	INTLDFLAGS=$(EMCC_LDFLAGS)
else
	INTCXXFLAGS+= -I$(SRC) `$(SDLCONFIG) --cflags`
	INTLDFLAGS=`$(SDLCONFIG) --libs`
endif
