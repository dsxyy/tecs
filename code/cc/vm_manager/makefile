# makefile for VM_MANAGER

VM_MANAGER_PATH = $(_CC_VM_MANAGER_PATH)/src
VM_MANAGER_CPP  = $(wildcard $(VM_MANAGER_PATH)/*.cpp)
VM_MANAGER_DEP  = $(addprefix $(_CC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(VM_MANAGER_CPP))))
VM_MANAGER_OBJ  = $(addprefix $(_CC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(VM_MANAGER_CPP))))

_CC_ALL_OBJECTS += $(VM_MANAGER_OBJ)

$(_CC_TMP_DEP_PATH)/%.d:$(VM_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CC_TMP_OBJ_PATH)/%.o:$(VM_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(VM_MANAGER_DEP)
endif

