include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: storageobj clean cleanall test
default: storageobj

include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_TECS_COMMON_ALARM_MAKEFILE)
include $(_TECS_COMMON_MS_MAKEFILE)
include $(_TECS_COMMON_VNET_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
include $(_TECS_COMMON_MISC_MAKEFILE)
include $(TC_COMMON_MAKEFILE)

ifeq (_SVR_SC,$(_SVR_TYPE))
include $(_TECS_COMMON_DB_MAKEFILE)
include $(MAIN_MAKEFILE)
include $(_STORAGE_SC_MAKEFILE)
include $(STORAGE_COMMON_MAKEFILE)
endif

ifeq (_SVR_SA,$(_SVR_TYPE))
include $(MAIN_MAKEFILE)
include $(_STORAGE_SA_MAKEFILE)
include $(STORAGE_COMMON_MAKEFILE)
endif

export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS)  $(_STORAGE_ALL_OBJECTS)
storageobj: $(_VERSION_OBJS_ALL)
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_STORAGE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_OBJ_PATH); fi
	@if [ ! -d $(_STORAGE_TMP_DEP_PATH) ]; then $(MKDIR) $(_STORAGE_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_STORAGE_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_STORAGE_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) STORAGE has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_STORAGE_TMP_OBJ_PATH)\*.o $(_STORAGE_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_STORAGE_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory hc !Go..
	-$(RMDIR) $(_STORAGE_TMP_OBJ_PATH) $(_STORAGE_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) STORAGE has been cleaned all!
	$(ECHO) -----------------------------------------------------------------