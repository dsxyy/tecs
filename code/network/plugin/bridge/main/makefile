# makefile for PLUGIN_BR MAIN

_NETWORK_PLUGIN_BR_MAIN_SRC_PATH = $(_NETWORK_PLUGIN_BR_PATH)/main/src
BR_MAIN_CPP  = $(wildcard $(_NETWORK_PLUGIN_BR_MAIN_SRC_PATH)/*.cpp)
BR_MAIN_DEP  = $(addprefix $(_NETWORK_PLUGIN_BR_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(BR_MAIN_CPP))))
BR_MAIN_OBJ  = $(addprefix $(_NETWORK_PLUGIN_BR_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(BR_MAIN_CPP))))

_NETWORK_ALL_OBJECTS += $(BR_MAIN_OBJ)

$(_NETWORK_PLUGIN_BR_TMP_DEP_PATH)/%.d:$(_NETWORK_PLUGIN_BR_MAIN_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_BR_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_BR_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_PLUGIN_BR_TMP_DEP_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_PLUGIN_BR_TMP_OBJ_PATH)/%.o:$(_NETWORK_PLUGIN_BR_MAIN_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_BR_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_BR_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(BR_MAIN_DEP)
endif

