
# Makefile for TC_OPERATOR_LOG
TC_OPLOG_PATH = $(_TC_OPERATOR_LOG_PATH)/src
TC_OPLOG_CPP = $(wildcard $(TC_OPLOG_PATH)/*.cpp)
TC_OPLOG_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TC_OPLOG_CPP))))
TC_OPLOG_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TC_OPLOG_CPP))))

_TC_ALL_OBJECTS +=$(TC_OPLOG_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(TC_OPLOG_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(TC_OPLOG_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(TC_OPLOG_DEP)
endif
