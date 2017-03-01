
# makefile for tc
STORAGE_SVR_PATH = $(_STORAGE_MAIN_PATH)
ifeq (_SVR_SC,$(_SVR_TYPE))
STORAGE_SVR_CPP = sc.cpp
endif

ifeq (_SVR_SA,$(_SVR_TYPE))
STORAGE_SVR_CPP = sa.cpp
endif

STORAGE_SVR_DEP = $(addprefix $(_STORAGE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(STORAGE_SVR_CPP))))
STORAGE_SVR_OBJ = $(addprefix $(_STORAGE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(STORAGE_SVR_CPP))))

_STORAGE_ALL_OBJECTS += $(STORAGE_SVR_OBJ)

$(_STORAGE_TMP_DEP_PATH)/%.d:$(STORAGE_SVR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_STORAGE_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_STORAGE_TMP_OBJ_PATH)/%.o:$(STORAGE_SVR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(STORAGE_SVR_DEP)
endif
