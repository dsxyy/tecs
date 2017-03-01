# makefile for NETWORKCOMMON_ALARM
NETWORK_COMMON_ALARM_PATH = $(_NETWORK_COMMON_SRC_PATH)/alarm
NETWORK_COMMON_ALARM_CPP  = $(wildcard $(NETWORK_COMMON_ALARM_PATH)/*.cpp)
NETWORK_COMMON_ALARM_DEP = $(addprefix $(_NETWORK_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(NETWORK_COMMON_ALARM_CPP))))
NETWORK_COMMON_ALARM_OBJ = $(addprefix $(_NETWORK_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(NETWORK_COMMON_ALARM_CPP))))

_NETWORK_ALL_OBJECTS += $(NETWORK_COMMON_ALARM_OBJ)

$(_NETWORK_COMMON_TMP_DEP_PATH)/%.d:$(NETWORK_COMMON_ALARM_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_COMMON_TMP_OBJ_PATH)/%.o:$(NETWORK_COMMON_ALARM_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(NETWORK_COMMON_ALARM_DEP)
endif

