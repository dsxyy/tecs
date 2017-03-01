
# makefile for SMART
SMART_PATH = $(_SMART_PATH)/src
SMART_CPP  = $(wildcard $(SMART_PATH)/*.cpp)
SMART_DEP  = $(addprefix $(_SMART_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(SMART_CPP))))
SMART_OBJ  = $(addprefix $(_SMART_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(SMART_CPP))))

_SMART_ALL_OBJECTS +=$(SMART_OBJ)

$(_SMART_TMP_DEP_PATH)/%.d:$(SMART_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_SMART_TMP_DEP_PATH) ]; then $(MKDIR) $(_SMART_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_SMART_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_SMART_TMP_OBJ_PATH)/%.o:$(SMART_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_SMART_TMP_OBJ_PATH) ]; then $(MKDIR) $(_SMART_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(SMART_DEP)
endif

