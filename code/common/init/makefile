# makefile for COMMON_INIT
COMMON_INIT_PATH = $(_TECS_CODE_PATH)/common/init
COMMON_INIT_CPP  = $(wildcard $(COMMON_INIT_PATH)/*.cpp)
COMMON_INIT_DEP = $(addprefix $(_TECS_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(COMMON_INIT_CPP))))
COMMON_INIT_OBJ = $(addprefix $(_TECS_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(COMMON_INIT_CPP))))

_TECS_COMMON_OBJS += $(COMMON_INIT_OBJ)

$(_TECS_COMMON_TMP_DEP_PATH)/%.d:$(COMMON_INIT_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TECS_COMMON_TMP_OBJ_PATH)/%.o:$(COMMON_INIT_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(COMMON_INIT_DEP)
endif

