
# Makefile for TC_VMCFG_CONFIG
TC_VMCFG_PATH = $(_TC_VMCFG_MANAGER_PATH)/src
TC_VMCFG_CPP = $(wildcard $(TC_VMCFG_PATH)/*.cpp)
TC_VMCFG_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TC_VMCFG_CPP))))
TC_VMCFG_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TC_VMCFG_CPP))))

_TC_ALL_OBJECTS +=$(TC_VMCFG_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(TC_VMCFG_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(TC_VMCFG_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(TC_VMCFG_DEP)
endif
