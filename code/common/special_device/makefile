# makefile for COMMON_SPECIAL_DEVICE
COMMON_SPECIAL_DEVICE_PATH = $(_TECS_CODE_PATH)/common/special_device
COMMON_SPECIAL_DEVICE_CPP  = $(wildcard $(COMMON_SPECIAL_DEVICE_PATH)/*.cpp)
COMMON_SPECIAL_DEVICE_DEP = $(addprefix $(_TECS_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(COMMON_SPECIAL_DEVICE_CPP))))
COMMON_SPECIAL_DEVICE_OBJ = $(addprefix $(_TECS_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(COMMON_SPECIAL_DEVICE_CPP))))

_TECS_COMMON_OBJS += $(COMMON_SPECIAL_DEVICE_OBJ)

$(_TECS_COMMON_TMP_DEP_PATH)/%.d:$(COMMON_SPECIAL_DEVICE_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TECS_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TECS_COMMON_TMP_OBJ_PATH)/%.o:$(COMMON_SPECIAL_DEVICE_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(COMMON_SPECIAL_DEVICE_DEP)
endif

