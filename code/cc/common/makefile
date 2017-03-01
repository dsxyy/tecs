# makefile for CC_COMMON

CC_COMMON_PATH = $(_CC_COMMON_PATH)/src
CC_COMMON_CPP  = $(wildcard $(CC_COMMON_PATH)/*.cpp)
CC_COMMON_DEP  = $(addprefix $(_CC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(CC_COMMON_CPP))))
CC_COMMON_OBJ  = $(addprefix $(_CC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(CC_COMMON_CPP))))

_CC_ALL_OBJECTS += $(CC_COMMON_OBJ)

$(_CC_TMP_DEP_PATH)/%.d:$(CC_COMMON_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CC_TMP_OBJ_PATH)/%.o:$(CC_COMMON_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(CC_COMMON_DEP)
endif

