include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: smartlib clean cleanall
default: smartlib

ifeq (_LIB_SMART,$(_LIB_VER))
include $(_SMARTLIB_MAKEFILE)
endif

export _VERSION_OBJS_ALL = $(_SMART_ALL_OBJECTS)
smartlib: $(_SMART_ALL_OBJECTS)
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_SMART_TMP_OBJ_PATH) ]; then $(MKDIR) $(_SMART_TMP_OBJ_PATH); fi
	@if [ ! -d $(_SMART_TMP_DEP_PATH) ]; then $(MKDIR) $(_SMART_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak lib _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_SMART_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_SMART_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) smartobj has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: 
	$(ECHO) Deleting Files $(_SMART_TMP_OBJ_PATH)\*.o $(_SMART_TMP_DEP_PATH)\*.d !Go..
	$(ECHO) $(_SMART_TMP_OBJ_PATH)
	-$(RM) $(_SMART_TMP_OBJ_PATH)/*.o 
	-$(RM) $(_SMART_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory storagedb !Go..
	-$(RMDIR) $(_SMART_TMP_OBJ_PATH) $(_SMART_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) smart has been cleaned all!
	$(ECHO) -----------------------------------------------------------------

