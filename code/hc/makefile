include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: hcobj clean cleanall test
default: hcobj

include $(_TECS_COMMON_INIT_MAKEFILE)
#include $(_TECS_COMMON_DB_MAKEFILE)
#include $(_TECS_COMMON_ALARM_MAKEFILE)
#include $(_TECS_COMMON_MS_MAKEFILE)
#include $(_TECS_COMMON_STORAGE_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
include $(_TECS_COMMON_MISC_MAKEFILE)
include $(_HC_COMMON_MAKEFILE)

ifeq (_SVR_HC,$(_SVR_TYPE))
include $(_STORAGE_MAKEFILE)
include $(_VM_HANDLER_MAKEFILE)
include $(_HOST_HANDLER_MAKEFILE)
include $(_HOST_AGENT_MAKEFILE)
include $(_VNET_CONTROLER_MAKEFILE)
include $(_HCSVR_MAKEFILE)
include $(_HOST_PORT_MANAGER_MAKEFILE)
include $(_HOST_HEARTBEAT_MAKEFILE)
export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS) $(_HC_ALL_OBJECTS)
endif

ifeq (_SVR_HYS,$(_SVR_TYPE))
#include $(_VM_HANDLER_MAKEFILE)
include $(_HCSVR_MAKEFILE)
export _VERSION_OBJS_ALL = $(_HC_ALL_OBJECTS)
endif


hcobj: $(_VERSION_OBJS_ALL)
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_HC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_HC_TMP_OBJ_PATH); fi
	@if [ ! -d $(_HC_TMP_DEP_PATH) ]; then $(MKDIR) $(_HC_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_HC_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_HC_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) hc has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_HC_TMP_OBJ_PATH)\*.o $(_HC_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_HC_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory hc !Go..
	-$(RMDIR) $(_HC_TMP_OBJ_PATH) $(_HC_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) hc has been cleaned all!
	$(ECHO) -----------------------------------------------------------------