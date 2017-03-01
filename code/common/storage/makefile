include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig
 
.PHONY: storagelib clean cleanall
default: storagelib

ifeq (_LIB_STORAGE,$(_LIB_VER))
#include $(_LVMLIB_MAKEFILE)
endif

#include ./makeconfig1
#start

# makefile for SMART
#STORAGE_PATH = $(_STORAGE_PATH)/src
STORAGE_CPP  = $(wildcard $(_STORAGE_PATH)/*.cpp)
STORAGE_DEP  = $(addprefix $(_STORAGE_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(STORAGE_CPP))))
STORAGE_OBJ  = $(addprefix $(_STORAGE_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(STORAGE_CPP))))

_STORAGE_ALL_OBJECTS +=$(STORAGE_OBJ)

$(_STORAGE_TMP_DEP_PATH)/%.d:$(_STORAGE_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)		
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_STORAGE_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_STORAGE_TMP_OBJ_PATH)/%.o:$(_STORAGE_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)	
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(STORAGE_DEP)
endif

#end

export _VERSION_OBJS_ALL = $(_STORAGE_ALL_OBJECTS)
storagelib: $(_STORAGE_ALL_OBJECTS)
	$(ECHO) generate $(_VERSION_NAME) begin...	
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak lib _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!
	


clean:
	$(ECHO) Deleting Files $(_STORAGE_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_STORAGE_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) storageobj has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_STORAGE_TMP_OBJ_PATH)\*.o $(_STORAGE_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_STORAGE_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory storagedb !Go..
	-$(RMDIR) $(_STORAGE_TMP_OBJ_PATH) $(_STORAGE_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) storage has been cleaned all!
	$(ECHO) -----------------------------------------------------------------
