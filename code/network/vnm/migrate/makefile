# makefile for VNM

_NETWORK_VNM_MIGRATE_SRC_PATH = $(_NETWORK_VNM_MIGRATE_PATH)/src
VNM_MIGRATE_CPP  = $(wildcard $(_NETWORK_VNM_MIGRATE_SRC_PATH)/*.cpp)
VNM_MIGRATE_DEP  = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(VNM_MIGRATE_CPP))))
VNM_MIGRATE_OBJ  = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(VNM_MIGRATE_CPP))))

_NETWORK_ALL_OBJECTS += $(VNM_MIGRATE_OBJ)

$(_NETWORK_TMP_DEP_PATH)/%.d:$(_NETWORK_VNM_MIGRATE_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(_NETWORK_VNM_MIGRATE_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(VNM_MIGRATE_DEP)
endif

