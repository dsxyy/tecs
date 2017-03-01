
# makefile for host_heartbeat

HOST_HEARTBEAT_PATH = $(_HC_HOST_HEARTBEAT_PATH)/src
HOST_HEARTBEAT_CPP = $(wildcard $(HOST_HEARTBEAT_PATH)/*.cpp)
HOST_HEARTBEAT_DEP = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(HOST_HEARTBEAT_CPP))))
HOST_HEARTBEAT_OBJ = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(HOST_HEARTBEAT_CPP))))

_HC_ALL_OBJECTS +=$(HOST_HEARTBEAT_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(HOST_HEARTBEAT_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(HOST_HEARTBEAT_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(HOST_HEARTBEAT_DEP)
endif
