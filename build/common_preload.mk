PRELOAD_FILES=
PRELOAD_DATA_DIR ?= data
PRELOAD_REQUIRE_EXISTING_DIR ?= 0

ifeq ($(CC), emcc)
ifeq ($(PRELOAD_REQUIRE_EXISTING_DIR),1)
ifneq ($(wildcard $(PRELOAD_DATA_DIR)),)
	PRELOAD_FILES= --preload-file $(PRELOAD_DATA_DIR)@/
endif
else
	PRELOAD_FILES= --preload-file $(PRELOAD_DATA_DIR)@/
endif
endif
