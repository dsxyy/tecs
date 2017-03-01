
# makefile for tc
SPECIAL_DEVICE_SVR_PATH = $(_SPECIAL_DEVICE_SVR_PATH)
ifeq (_SVR_SPECIAL_DEVICE,$(_SVR_TYPE))
SPECIAL_DEVICE_SVR_CPP = special_device.cpp
endif

SPECIAL_DEVICE_SVR_DEP = $(addprefix $(_SPECIAL_DEVICE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(SPECIAL_DEVICE_SVR_CPP))))
SPECIAL_DEVICE_SVR_OBJ = $(addprefix $(_SPECIAL_DEVICE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(SPECIAL_DEVICE_SVR_CPP))))

_SPECIAL_DEVICE_ALL_OBJECTS += $(SPECIAL_DEVICE_SVR_OBJ)

$(_SPECIAL_DEVICE_TMP_DEP_PATH)/%.d:$(SPECIAL_DEVICE_SVR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_SPECIAL_DEVICE_TMP_DEP_PATH) ]; then $(MKDIR) $(_SPECIAL_DEVICE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_SPECIAL_DEVICE_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_SPECIAL_DEVICE_TMP_OBJ_PATH)/%.o:$(SPECIAL_DEVICE_SVR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_SPECIAL_DEVICE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_SPECIAL_DEVICE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(SPECIAL_DEVICE_SVR_DEP)
endif
