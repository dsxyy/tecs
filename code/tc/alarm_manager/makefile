
# Makefile for Request Manager
ALARM_MANAGER_PATH = $(_TC_ALARM_MANAGER_PATH)/src
ALARM_MANAGER_CPP = $(wildcard $(ALARM_MANAGER_PATH)/*.cpp)
ALARM_MANAGER_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(ALARM_MANAGER_CPP))))
ALARM_MANAGER_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(ALARM_MANAGER_CPP))))

_TC_ALL_OBJECTS +=$(ALARM_MANAGER_OBJ)
 
$(_TC_TMP_DEP_PATH)/%.d:$(ALARM_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(ALARM_MANAGER_PATH)/%.cpp 
	$(ECHO) compile $(notdir $@) 
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ ..... 
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(ALARM_MANAGER_DEP) 
endif 
