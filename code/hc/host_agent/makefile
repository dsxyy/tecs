# Makefile for HOST_AGENT

HOST_AGENT_PATH = $(_HC_HOST_AGENT_PATH)/src
HOST_AGENT_CPP  = $(wildcard $(HOST_AGENT_PATH)/*.cpp)
HOST_AGENT_DEP  = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(HOST_AGENT_CPP))))
HOST_AGENT_OBJ  = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(HOST_AGENT_CPP))))

_HC_ALL_OBJECTS += $(HOST_AGENT_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(HOST_AGENT_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(HOST_AGENT_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(HOST_AGENT_DEP)
endif

