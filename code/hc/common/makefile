HC_COMMON_PATH = $(_HC_COMMON_PATH)/src
HC_COMMON_CPP  = $(wildcard $(HC_COMMON_PATH)/*.cpp)
HC_COMMON_DEP  = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(HC_COMMON_CPP))))
HC_COMMON_OBJ  = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(HC_COMMON_CPP))))

_HC_ALL_OBJECTS += $(HC_COMMON_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(HC_COMMON_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(HC_COMMON_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(HC_COMMON_DEP)
endif
