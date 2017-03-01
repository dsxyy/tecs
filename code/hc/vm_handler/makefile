
# makefile for VM_HAND
VM_HAND_PATH = $(_HC_VM_HANDLER_PATH)/src
VM_HAND_CPP = $(wildcard $(VM_HAND_PATH)/*.cpp)
VM_HAND_DEP = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(VM_HAND_CPP))))
VM_HAND_OBJ = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(VM_HAND_CPP))))

_HC_ALL_OBJECTS +=$(VM_HAND_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(VM_HAND_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(VM_HAND_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(VM_HAND_DEP)
endif
