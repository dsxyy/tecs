
CLUSTER_MANAGER_PATH = $(_TC_CLUSTER_MANAGER_PATH)/src
CLUSTER_MANAGER_CPP = $(wildcard $(CLUSTER_MANAGER_PATH)/*.cpp)
CLUSTER_MANAGER_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(CLUSTER_MANAGER_CPP))))
CLUSTER_MANAGER_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(CLUSTER_MANAGER_CPP))))

_TC_ALL_OBJECTS +=$(CLUSTER_MANAGER_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(CLUSTER_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(CLUSTER_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(CLUSTER_MANAGER_DEP)
endif
