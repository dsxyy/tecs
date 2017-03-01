# makefile for VNA_SWITCH

_NETWORK_VNA_SWITCH_SRC_PATH = $(_NETWORK_VNA_SWITCH_PATH)/src
VNA_SWITCH_CPP  = $(wildcard $(_NETWORK_VNA_SWITCH_SRC_PATH)/*.cpp)
VNA_SWITCH_DEP  = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(VNA_SWITCH_CPP))))
VNA_SWITCH_OBJ  = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(VNA_SWITCH_CPP))))

_NETWORK_ALL_OBJECTS += $(VNA_SWITCH_OBJ)

$(_NETWORK_TMP_DEP_PATH)/%.d:$(_NETWORK_VNA_SWITCH_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(_NETWORK_VNA_SWITCH_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(VNA_SWITCH_DEP)
endif

