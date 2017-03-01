
# makefile for utils
UTILS_SVR_PATH = $(_UTILS_CCSVR_PATH)
ifeq (_SVR_VERSION,$(_SVR_TYPE))
TPM_SVR_CPP = tecs_version.cpp
endif
#ifeq (_SVR_VERSION_AGENT,$(_SVR_TYPE))
#TPM_SVR_CPP = tecs_version_agent.cpp
#endif

ifeq (_SVR_GUARD,$(_SVR_TYPE))
TPM_SVR_CPP = tecs_guard.cpp
endif

ifeq (_SVR_TC_SSH_TRUST,$(_SVR_TYPE))
TPM_SVR_CPP = ssh_trust_main.cpp
endif

ifeq (_SVR_CC_SSH_TRUST,$(_SVR_TYPE))
TPM_SVR_CPP = ssh_trust_main.cpp
endif

UTILS_SVR_DEP = $(addprefix $(_UTILS_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TPM_SVR_CPP))))
UTILS_SVR_OBJ = $(addprefix $(_UTILS_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TPM_SVR_CPP))))

_UTILS_ALL_OBJECTS += $(UTILS_SVR_OBJ)

$(_UTILS_TMP_DEP_PATH)/%.d:$(UTILS_SVR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_UTILS_TMP_DEP_PATH) ]; then $(MKDIR) $(_UTILS_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_UTILS_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_UTILS_TMP_OBJ_PATH)/%.o:$(UTILS_SVR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_UTILS_TMP_OBJ_PATH) ]; then $(MKDIR) $(_UTILS_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(UTILS_SVR_DEP)
endif
