# makefile for HOST_HANDLER

HOST_HANDLER_PATH = $(_HC_HOST_HANDLER_PATH)/src
HOST_HANDLER_CPP  = $(wildcard $(HOST_HANDLER_PATH)/*.cpp)
HOST_HANDLER_DEP  = $(addprefix $(_HC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(HOST_HANDLER_CPP))))
HOST_HANDLER_OBJ  = $(addprefix $(_HC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(HOST_HANDLER_CPP))))

_HC_ALL_OBJECTS += $(HOST_HANDLER_OBJ)

$(_HC_TMP_DEP_PATH)/%.d:$(HOST_HANDLER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_HC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_HC_TMP_OBJ_PATH)/%.o:$(HOST_HANDLER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(HOST_HANDLER_DEP)
endif

