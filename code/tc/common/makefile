
TC_COMMON_PATH = $(_TC_COMMON_PATH)/src
TC_COMMON_CPP = $(wildcard $(TC_COMMON_PATH)/*.cpp)
TC_COMMON_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TC_COMMON_CPP))))
TC_COMMON_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TC_COMMON_CPP))))

_TC_ALL_OBJECTS +=$(TC_COMMON_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(TC_COMMON_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(TC_COMMON_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(TC_COMMON_DEP)
endif
