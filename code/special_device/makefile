include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: special_deviceobj clean cleanall test
default: special_deviceobj

include $(_TECS_COMMON_INIT_MAKEFILE)

ifeq (_SVR_SPECIAL_DEVICE,$(_SVR_TYPE))
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
include $(_LIGHT_MANAGER_MAKEFILE)
include $(_SPECIAL_DEVICE_SVR_MAKEFILE)
endif

export _VERSION_OBJS_ALL = $(_TECS_COMMON_TMP_OBJ_PATH)/tecs_init.o $(_TECS_COMMON_TMP_OBJ_PATH)/registered_system.o $(COMMON_SPECIAL_DEVICE_OBJ) $(_SPECIAL_DEVICE_ALL_OBJECTS)
special_deviceobj: $(_VERSION_OBJS_ALL)
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_SPECIAL_DEVICE_TMP_OBJ_PATH) ]; then $(MKDIR) $(_SPECIAL_DEVICE_TMP_OBJ_PATH); fi
	@if [ ! -d $(_SPECIAL_DEVICE_TMP_DEP_PATH) ]; then $(MKDIR) $(_SPECIAL_DEVICE_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_SPECIAL_DEVICE_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_SPECIAL_DEVICE_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) special_device has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_SPECIAL_DEVICE_TMP_OBJ_PATH)\*.o $(_SPECIAL_DEVICE_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_SPECIAL_DEVICE_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory special_device !Go..
	-$(RMDIR) $(_SPECIAL_DEVICE_TMP_OBJ_PATH) $(_SPECIAL_DEVICE_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) special_device has been cleaned all!
	$(ECHO) -----------------------------------------------------------------