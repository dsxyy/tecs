
# Makefile for Request Manager
SNMP_AGENT_PATH = $(_TC_SNMP_AGENT_PATH)/src
SNMP_AGENT_CPP = $(wildcard $(SNMP_AGENT_PATH)/*.cpp)
SNMP_AGENT_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(SNMP_AGENT_CPP))))
SNMP_AGENT_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(SNMP_AGENT_CPP))))

_TC_ALL_OBJECTS +=$(SNMP_AGENT_OBJ)
 
$(_TC_TMP_DEP_PATH)/%.d:$(SNMP_AGENT_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(SNMP_AGENT_PATH)/%.cpp 
	$(ECHO) compile $(notdir $@) 
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ ..... 
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(SNMP_AGENT_DEP) 
endif 
