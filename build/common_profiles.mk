ifeq ($(BUILD_PROFILE), emscripten)
EMCC_CFLAGS ?= -s USE_SDL=2
EMCC_LDFLAGS ?= -s USE_SDL=2 -s WASM=1 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1
# Emscripten warning budget:
# - Suppress switch exhaustiveness noise (especially large SDL enums/scancodes).
# - Keep correctness warnings (e.g., -Wreturn-type, -Wunused-value) visible.
EMCC_WARN_CFLAGS_BASE ?= -Wno-switch
EMCC_WARN_CFLAGS ?= $(EMCC_WARN_CFLAGS_BASE)
endif

ifeq ($(BUILD_PROFILE), emscripten-legacy)
EMCC_CFLAGS ?= -s USE_SDL=2 -s WASM=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_FILE=data.binary -s ALLOW_MEMORY_GROWTH=1
EMCC_LDFLAGS ?= -s USE_SDL=2 -s WASM=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_FILE=data.binary -s ALLOW_MEMORY_GROWTH=1
EMCC_WARN_CFLAGS_BASE ?= -Wno-switch
EMCC_WARN_CFLAGS ?= $(EMCC_WARN_CFLAGS_BASE)
endif
