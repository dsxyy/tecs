# Makefile for PACKAGE_MANAGER

PACKAGE_MANAGER_PATH = $(_PACKAGE_MANAGER_PATH)/src
ifeq (_SVR_VERSION,$(_SVR_TYPE))
PACKAGE_MANAGER_CPP = $(wildcard $(PACKAGE_MANAGER_PATH)/*.cpp)
endif
PACKAGE_MANAGER_DEP  = $(addprefix $(_UTILS_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(PACKAGE_MANAGER_CPP))))
PACKAGE_MANAGER_OBJ  = $(addprefix $(_UTILS_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(PACKAGE_MANAGER_CPP))))

_UTILS_ALL_OBJECTS += $(PACKAGE_MANAGER_OBJ)

$(_UTILS_TMP_DEP_PATH)/%.d:$(PACKAGE_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_UTILS_TMP_DEP_PATH) ]; then $(MKDIR) $(_UTILS_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_UTILS_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_UTILS_TMP_OBJ_PATH)/%.o:$(PACKAGE_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_UTILS_TMP_OBJ_PATH) ]; then $(MKDIR) $(_UTILS_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(PACKAGE_MANAGER_DEP)
endif

