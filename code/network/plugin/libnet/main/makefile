# makefile for PLUGIN_NET MAIN

_NETWORK_PLUGIN_NET_MAIN_SRC_PATH = $(_NETWORK_PLGIN_NET_PATH)/main/src

#$(warning $(_NETWORK_PLUGIN_NET_MAIN_SRC_PATH))

NET_MAIN_CPP  = $(wildcard $(_NETWORK_PLUGIN_NET_MAIN_SRC_PATH)/*.cpp)
NET_MAIN_DEP  = $(addprefix $(_NETWORK_PLUGIN_NET_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(NET_MAIN_CPP))))
NET_MAIN_OBJ  = $(addprefix $(_NETWORK_PLUGIN_NET_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(NET_MAIN_CPP))))

_NETWORK_ALL_OBJECTS += $(NET_MAIN_OBJ)

$(_NETWORK_PLUGIN_NET_TMP_DEP_PATH)/%.d:$(_NETWORK_PLUGIN_NET_MAIN_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_NET_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_NET_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_PLUGIN_NET_TMP_DEP_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_PLUGIN_NET_TMP_OBJ_PATH)/%.o:$(_NETWORK_PLUGIN_NET_MAIN_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_NET_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_NET_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(NET_MAIN_DEP)
endif

