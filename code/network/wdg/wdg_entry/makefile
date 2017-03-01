# makefile for WDG_ENTRY

_NETWORK_WDG_ENTRY_SRC_PATH = $(_NETWORK_WDG_ENTRY_PATH)/src
WDG_ENTRY_CPP  = $(wildcard $(_NETWORK_WDG_ENTRY_SRC_PATH)/*.cpp)
WDG_ENTRY_DEP  = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(WDG_ENTRY_CPP))))
WDG_ENTRY_OBJ  = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(WDG_ENTRY_CPP))))

_NETWORK_ALL_OBJECTS += $(WDG_ENTRY_OBJ)

$(_NETWORK_TMP_DEP_PATH)/%.d:$(_NETWORK_WDG_ENTRY_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(_NETWORK_WDG_ENTRY_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(WDG_ENTRY_DEP)
endif

