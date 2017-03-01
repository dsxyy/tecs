
# makefile for storage
STORAGE_PATH = $(_HC_STORAGE_PATH)/src
STORAGE_CPP = $(wildcard $(STORAGE_PATH)/*.cpp)
STORAGE_DEP = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(STORAGE_CPP))))
STORAGE_OBJ = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(STORAGE_CPP))))

_HC_ALL_OBJECTS +=$(STORAGE_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(STORAGE_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(STORAGE_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(STORAGE_DEP)
endif
