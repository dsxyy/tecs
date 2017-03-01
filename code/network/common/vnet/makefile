# makefile for NETWORKCOMMON_VNET
NETWORK_COMMON_VNET_PATH = $(_NETWORK_COMMON_SRC_PATH)/vnet
NETWORK_COMMON_VNET_CPP  = $(wildcard $(NETWORK_COMMON_VNET_PATH)/*.cpp)
NETWORK_COMMON_VNET_DEP = $(addprefix $(_NETWORK_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(NETWORK_COMMON_VNET_CPP))))
NETWORK_COMMON_VNET_OBJ = $(addprefix $(_NETWORK_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(NETWORK_COMMON_VNET_CPP))))

_NETWORK_ALL_OBJECTS += $(NETWORK_COMMON_VNET_OBJ)

#$(warning $(_NETWORK_COMMON_OBJS))


$(_NETWORK_COMMON_TMP_DEP_PATH)/%.d:$(NETWORK_COMMON_VNET_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_COMMON_TMP_OBJ_PATH)/%.o:$(NETWORK_COMMON_VNET_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(NETWORK_COMMON_VNET_DEP)
endif

