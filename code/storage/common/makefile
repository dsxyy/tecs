
STORAGE_COMMON_PATH = $(_STORAGE_COMMON_PATH)/src
STORAGE_COMMON_CPP = $(wildcard $(STORAGE_COMMON_PATH)/*.cpp)
STORAGE_COMMON_DEP = $(addprefix $(_STORAGE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(STORAGE_COMMON_CPP))))
STORAGE_COMMON_OBJ = $(addprefix $(_STORAGE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(STORAGE_COMMON_CPP))))

_STORAGE_ALL_OBJECTS +=$(STORAGE_COMMON_OBJ)

$(_STORAGE_TMP_DEP_PATH)/%.d:$(STORAGE_COMMON_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_STORAGE_TMP_OBJ_PATH)/%.o:$(STORAGE_COMMON_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(STORAGE_COMMON_DEP)
endif
