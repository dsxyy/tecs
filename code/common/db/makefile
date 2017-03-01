# makefile for COMMON_DB
COMMON_DB_PATH = $(_TECS_CODE_PATH)/common/db
COMMON_DB_CPP  = $(wildcard $(COMMON_DB_PATH)/*.cpp)
COMMON_DB_DEP = $(addprefix $(_TECS_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(COMMON_DB_CPP))))
COMMON_DB_OBJ = $(addprefix $(_TECS_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(COMMON_DB_CPP))))

_TECS_COMMON_OBJS += $(COMMON_DB_OBJ)

$(_TECS_COMMON_TMP_DEP_PATH)/%.d:$(COMMON_DB_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TECS_COMMON_TMP_OBJ_PATH)/%.o:$(COMMON_DB_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(COMMON_DB_DEP)
endif

