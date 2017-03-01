
# Makefile for TC_PROJECT_MANAGER
TC_PRJMGR_PATH = $(_TC_PROJECT_MANAGER_PATH)/src
TC_PRJMGR_CPP = $(wildcard $(TC_PRJMGR_PATH)/*.cpp)
TC_PRJMGR_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TC_PRJMGR_CPP))))
TC_PRJMGR_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TC_PRJMGR_CPP))))

_TC_ALL_OBJECTS +=$(TC_PRJMGR_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(TC_PRJMGR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(TC_PRJMGR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(TC_PRJMGR_DEP)
endif
