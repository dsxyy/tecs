# makefile for HOST_MANAGER

HOST_MANAGER_PATH = $(_CC_HOST_MANAGER_PATH)/src
HOST_MANAGER_CPP  = $(wildcard $(HOST_MANAGER_PATH)/*.cpp)
HOST_MANAGER_DEP  = $(addprefix $(_CC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(HOST_MANAGER_CPP))))
HOST_MANAGER_OBJ  = $(addprefix $(_CC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(HOST_MANAGER_CPP))))

_CC_ALL_OBJECTS += $(HOST_MANAGER_OBJ)

$(_CC_TMP_DEP_PATH)/%.d:$(HOST_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CC_TMP_OBJ_PATH)/%.o:$(HOST_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(HOST_MANAGER_DEP)
endif

