# makefile for VNM DB
_NETWORK_TMP_OBJ_PATH = $(_TEMP_OBJ_PATH)/network
_NETWORK_TMP_DEP_PATH = $(_TEMP_DEP_PATH)/network

_NETWORK_VNM_DB_SRC_PATH = $(_NETWORK_VNM_DB_PATH)/src
VNM_DB_CPP  = $(wildcard $(_NETWORK_VNM_DB_SRC_PATH)/*.cpp)
VNM_DB_DEP  = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(VNM_DB_CPP))))
VNM_DB_OBJ  = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(VNM_DB_CPP))))

_NETWORK_ALL_OBJECTS += $(VNM_DB_OBJ)
#_CC_ALL_OBJECTS +=$(VNM_DB_OBJ)

$(_NETWORK_TMP_DEP_PATH)/%.d:$(_NETWORK_VNM_DB_SRC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(_NETWORK_VNM_DB_SRC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(VNM_DB_DEP)
endif

