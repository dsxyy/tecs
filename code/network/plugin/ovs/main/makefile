# makefile for PLUGIN_OVS MAIN


_NETWORK_PLUGIN_OVS_MAIN_SRC_PATH = $(_NETWORK_PLUGIN_OVS_PATH)/main/src
OVS_MAIN_CPP  = $(wildcard $(_NETWORK_PLUGIN_OVS_MAIN_SRC_PATH)/*.cpp)
OVS_MAIN_DEP  = $(addprefix $(_NETWORK_PLUGIN_OVS_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(OVS_MAIN_CPP))))
OVS_MAIN_OBJ  = $(addprefix $(_NETWORK_PLUGIN_OVS_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(OVS_MAIN_CPP))))

_NETWORK_ALL_OBJECTS += $(OVS_MAIN_OBJ)

$(_NETWORK_PLUGIN_OVS_TMP_DEP_PATH)/%.d:$(_NETWORK_PLUGIN_OVS_MAIN_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_OVS_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_OVS_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_PLUGIN_OVS_TMP_DEP_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_PLUGIN_OVS_TMP_OBJ_PATH)/%.o:$(_NETWORK_PLUGIN_OVS_MAIN_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_OVS_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_OVS_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(OVS_MAIN_DEP)
endif

