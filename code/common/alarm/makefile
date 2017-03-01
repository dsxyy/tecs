# makefile for COMMON_ALARM
COMMON_ALARM_PATH = $(_TECS_CODE_PATH)/common/alarm
COMMON_ALARM_CPP  = $(wildcard $(COMMON_ALARM_PATH)/*.cpp)
COMMON_ALARM_DEP = $(addprefix $(_TECS_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(COMMON_ALARM_CPP))))
COMMON_ALARM_OBJ = $(addprefix $(_TECS_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(COMMON_ALARM_CPP))))

_TECS_COMMON_OBJS += $(COMMON_ALARM_OBJ)

$(_TECS_COMMON_TMP_DEP_PATH)/%.d:$(COMMON_ALARM_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TECS_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TECS_COMMON_TMP_OBJ_PATH)/%.o:$(COMMON_ALARM_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(COMMON_ALARM_DEP)
endif

