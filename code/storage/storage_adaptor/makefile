
STORAGE_SA_PATH = $(_STORAGE_SA_PATH)/src
STORAGE_SA_CPP = $(wildcard $(STORAGE_SA_PATH)/*.cpp)
STORAGE_SA_DEP = $(addprefix $(_STORAGE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(STORAGE_SA_CPP))))
STORAGE_SA_OBJ = $(addprefix $(_STORAGE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(STORAGE_SA_CPP))))

_STORAGE_ALL_OBJECTS +=$(STORAGE_SA_OBJ)

$(_STORAGE_TMP_DEP_PATH)/%.d:$(STORAGE_SA_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_STORAGE_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_STORAGE_TMP_OBJ_PATH)/%.o:$(STORAGE_SA_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(STORAGE_SA_DEP)
endif
