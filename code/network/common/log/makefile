# makefile for NETWORKCOMMON_LOG
NETWORK_COMMON_LOG_PATH = $(_NETWORK_COMMON_SRC_PATH)/log
NETWORK_COMMON_LOG_CPP  = $(wildcard $(NETWORK_COMMON_LOG_PATH)/*.cpp)
NETWORK_COMMON_LOG_DEP = $(addprefix $(_NETWORK_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(NETWORK_COMMON_LOG_CPP))))
NETWORK_COMMON_LOG_OBJ = $(addprefix $(_NETWORK_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(NETWORK_COMMON_LOG_CPP))))

_NETWORK_ALL_OBJECTS += $(NETWORK_COMMON_LOG_OBJ)

$(_NETWORK_COMMON_TMP_DEP_PATH)/%.d:$(NETWORK_COMMON_LOG_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_NETWORK_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_COMMON_TMP_OBJ_PATH)/%.o:$(NETWORK_COMMON_LOG_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(NETWORK_COMMON_LOG_DEP)
endif

