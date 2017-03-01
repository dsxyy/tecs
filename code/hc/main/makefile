
# makefile for hc
HC_SVR_PATH = $(_HC_HCSVR_PATH)
ifeq (_SVR_HC,$(_SVR_TYPE))
HC_SVR_CPP = hc.cpp
endif

ifeq (_SVR_HYS,$(_SVR_TYPE))
HC_SVR_CPP = hyperver_simulate.cpp
endif

HC_SVR_DEP = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(HC_SVR_CPP))))
HC_SVR_OBJ = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(HC_SVR_CPP))))

_HC_ALL_OBJECTS += $(HC_SVR_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(HC_SVR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(HC_SVR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(HC_SVR_DEP)
endif