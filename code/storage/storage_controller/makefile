
STORAGE_SC_PATH = $(_STORAGE_SC_PATH)/src
STORAGE_SC_CPP = $(wildcard $(STORAGE_SC_PATH)/*.cpp)
STORAGE_SC_DEP = $(addprefix $(_STORAGE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(STORAGE_SC_CPP))))
STORAGE_SC_OBJ = $(addprefix $(_STORAGE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(STORAGE_SC_CPP))))

_STORAGE_ALL_OBJECTS +=$(STORAGE_SC_OBJ)

$(_STORAGE_TMP_DEP_PATH)/%.d:$(STORAGE_SC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_STORAGE_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_STORAGE_TMP_OBJ_PATH)/%.o:$(STORAGE_SC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(STORAGE_SC_DEP)
endif
