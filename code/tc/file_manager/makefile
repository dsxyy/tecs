# Makefile for TC_FILE_MANAGER
TC_FILE_MANAGER_PATH = $(_TC_FILE_MANAGER_PATH)/src
TC_FILE_MANAGER_CPP = $(wildcard $(TC_FILE_MANAGER_PATH)/*.cpp)
TC_FILE_MANAGER_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TC_FILE_MANAGER_CPP))))
TC_FILE_MANAGER_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TC_FILE_MANAGER_CPP))))

_TC_ALL_OBJECTS +=$(TC_FILE_MANAGER_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(TC_FILE_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(TC_FILE_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(TC_FILE_MANAGER_DEP)
endif
