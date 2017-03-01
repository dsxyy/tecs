# makefile for WDG_MAIN

_NETWORK_WDG_MAIN_SRC_PATH = $(_NETWORK_WDG_MAIN_PATH)
WDG_MAIN_CPP  = $(wildcard $(_NETWORK_WDG_MAIN_SRC_PATH)/*.cpp)
WDG_MAIN_DEP  = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(WDG_MAIN_CPP))))
WDG_MAIN_OBJ  = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(WDG_MAIN_CPP))))

_NETWORK_ALL_OBJECTS += $(WDG_MAIN_OBJ)

$(_NETWORK_TMP_DEP_PATH)/%.d:$(_NETWORK_WDG_MAIN_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(_NETWORK_WDG_MAIN_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(WDG_MAIN_DEP)
endif

