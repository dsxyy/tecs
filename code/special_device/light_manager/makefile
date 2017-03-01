# Makefile for LIGHT_MANAGER
LIGHT_MANAGER_PATH = $(_SPECIAL_DEVICE_LIGHT_MANAGER_PATH)/src
LIGHT_MANAGER_CPP = $(wildcard $(LIGHT_MANAGER_PATH)/*.cpp)
LIGHT_MANAGER_DEP = $(addprefix $(_SPECIAL_DEVICE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(LIGHT_MANAGER_CPP))))
LIGHT_MANAGER_OBJ = $(addprefix $(_SPECIAL_DEVICE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(LIGHT_MANAGER_CPP))))

_SPECIAL_DEVICE_ALL_OBJECTS +=$(LIGHT_MANAGER_OBJ)

$(_SPECIAL_DEVICE_TMP_DEP_PATH)/%.d:$(LIGHT_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_SPECIAL_DEVICE_TMP_DEP_PATH) ]; then $(MKDIR) $(_SPECIAL_DEVICE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_SPECIAL_DEVICE_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_SPECIAL_DEVICE_TMP_OBJ_PATH)/%.o:$(LIGHT_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_SPECIAL_DEVICE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_SPECIAL_DEVICE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(LIGHT_MANAGER_DEP)
endif
