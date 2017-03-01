# Makefile for SSH_TRUST

SSH_TRUST_PATH = $(_SSH_TRUST_PATH)
ifeq (_SVR_TC_SSH_TRUST,$(_SVR_TYPE))
SSH_TRUST_CPP = ssh_trust.cpp
endif
ifeq (_SVR_CC_SSH_TRUST,$(_SVR_TYPE))
SSH_TRUST_CPP = ssh_trust.cpp
endif
SSH_TRUST_DEP  = $(addprefix $(_UTILS_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(SSH_TRUST_CPP))))
SSH_TRUST_OBJ  = $(addprefix $(_UTILS_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(SSH_TRUST_CPP))))

_UTILS_ALL_OBJECTS += $(SSH_TRUST_OBJ)

$(_UTILS_TMP_DEP_PATH)/%.d:$(SSH_TRUST_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_UTILS_TMP_DEP_PATH) ]; then $(MKDIR) $(_UTILS_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_UTILS_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_UTILS_TMP_OBJ_PATH)/%.o:$(SSH_TRUST_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_UTILS_TMP_OBJ_PATH) ]; then $(MKDIR) $(_UTILS_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(SSH_TRUST_DEP)
endif