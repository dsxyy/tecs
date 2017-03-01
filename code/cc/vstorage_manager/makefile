# makefile for VSTORAGE_MANAGER

VSTORAGE_MANAGER_PATH = $(_CC_VSTORAGE_MANAGER_PATH)/src
VSTORAGE_MANAGER_CPP  = $(wildcard $(VSTORAGE_MANAGER_PATH)/*.cpp)
VSTORAGE_MANAGER_DEP  = $(addprefix $(_CC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(VSTORAGE_MANAGER_CPP))))
VSTORAGE_MANAGER_OBJ  = $(addprefix $(_CC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(VSTORAGE_MANAGER_CPP))))

_CC_ALL_OBJECTS += $(VSTORAGE_MANAGER_OBJ)

$(_CC_TMP_DEP_PATH)/%.d:$(VSTORAGE_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CC_TMP_OBJ_PATH)/%.o:$(VSTORAGE_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(VSTORAGE_MANAGER_DEP)
endif

