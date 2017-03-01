
# makefile for cc
CC_SVR_PATH = $(_CC_CCSVR_PATH)
ifeq (_SVR_CC,$(_SVR_TYPE))
CC_SVR_CPP = cc.cpp
endif

ifeq (_SVR_CC_STORAGE,$(_SVR_TYPE))
CC_SVR_CPP = cc_storage.cpp
endif

ifeq (_SVR_IMAGE_AGENT,$(_SVR_TYPE))
CC_SVR_CPP = cc_image.cpp
endif

ifeq (_SVR_CC_NAT,$(_SVR_TYPE))
CC_SVR_CPP = nat_agent.cpp
endif

CC_SVR_DEP = $(addprefix $(_CC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(CC_SVR_CPP))))
CC_SVR_OBJ = $(addprefix $(_CC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(CC_SVR_CPP))))

_CC_ALL_OBJECTS += $(CC_SVR_OBJ)

$(_CC_TMP_DEP_PATH)/%.d:$(CC_SVR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CC_TMP_OBJ_PATH)/%.o:$(CC_SVR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(CC_SVR_DEP)
endif
