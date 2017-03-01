# makefile for IMAGE_AGENT
IMAGE_AGENT_PATH = $(_CC_IMAGE_AGENT_PATH)/src
IMAGE_AGENT_CPP  = $(wildcard $(IMAGE_AGENT_PATH)/*.cpp)
IMAGE_AGENT_DEP  = $(addprefix $(_CC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(IMAGE_AGENT_CPP))))
IMAGE_AGENT_OBJ  = $(addprefix $(_CC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(IMAGE_AGENT_CPP))))

_CC_ALL_OBJECTS += $(IMAGE_AGENT_OBJ)

$(_CC_TMP_DEP_PATH)/%.d:$(IMAGE_AGENT_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CC_TMP_OBJ_PATH)/%.o:$(IMAGE_AGENT_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(IMAGE_AGENT_DEP)
endif

