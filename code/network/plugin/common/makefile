# makefile for PLUGIN COMMON


_NETWORK_PLUGIN_COMMON_SRC_PATH = $(_NETWORK_PLUGIN_PATH)/common/src
COMMON_CPP  = $(wildcard $(_NETWORK_PLUGIN_COMMON_SRC_PATH)/*.cpp)
COMMON_DEP  = $(addprefix $(_NETWORK_PLUGIN_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(COMMON_CPP))))
COMMON_OBJ  = $(addprefix $(_NETWORK_PLUGIN_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(COMMON_CPP))))

_NETWORK_ALL_OBJECTS += $(COMMON_OBJ)

$(_NETWORK_PLUGIN_COMMON_TMP_DEP_PATH)/%.d:$(_NETWORK_PLUGIN_COMMON_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_PLUGIN_COMMON_TMP_DEP_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_PLUGIN_COMMON_TMP_OBJ_PATH)/%.o:$(_NETWORK_PLUGIN_COMMON_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_PLUGIN_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_PLUGIN_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(COMMON_DEP)
endif

