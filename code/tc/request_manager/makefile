
# Makefile for Request Manager
REQ_MANAGER_PATH = $(_TC_REQUEST_MANAGER_PATH)/src
REQ_MANAGER_CPP = $(wildcard $(REQ_MANAGER_PATH)/*.cpp)
REQ_MANAGER_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(REQ_MANAGER_CPP))))
REQ_MANAGER_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(REQ_MANAGER_CPP))))

_TC_ALL_OBJECTS +=$(REQ_MANAGER_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(REQ_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(REQ_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(REQ_MANAGER_DEP)
endif
