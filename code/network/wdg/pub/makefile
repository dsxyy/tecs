# makefile for WDG

_NETWORK_WDG_PUB_SRC_PATH = $(_NETWORK_WDG_PUB_PATH)/src
WDG_PUB_CPP  = $(wildcard $(_NETWORK_WDG_PUB_SRC_PATH)/*.cpp)
WDG_PUB_DEP  = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(WDG_PUB_CPP))))
WDG_PUB_OBJ  = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(WDG_PUB_CPP))))

_NETWORK_ALL_OBJECTS += $(WDG_PUB_OBJ)

$(_NETWORK_TMP_DEP_PATH)/%.d:$(_NETWORK_WDG_PUB_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(_NETWORK_WDG_PUB_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(WDG_PUB_DEP)
endif

