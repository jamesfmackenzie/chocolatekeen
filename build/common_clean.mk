CLEAN_DIRS ?= $(OBJ)

clean:
	-rm -rf $(CLEAN_DIRS)
ifneq ($(strip $(CLEAN_FILES)),)
	-rm -f $(CLEAN_FILES)
endif
