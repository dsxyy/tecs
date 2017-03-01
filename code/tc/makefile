include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: tcobj clean cleanall test
default: tcobj

include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_TECS_COMMON_DB_MAKEFILE)
include $(_TECS_COMMON_ALARM_MAKEFILE)
include $(_TECS_COMMON_MS_MAKEFILE)
include $(_TECS_COMMON_VNET_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
include $(_TECS_COMMON_MISC_MAKEFILE)
include $(TC_COMMON_MAKEFILE)

ifeq (_SVR_TC,$(_SVR_TYPE))
include $(CLUSTER_MANAGER_MAKEFILE)
include $(CLOUD_NODE_MANAGER_MAKEFILE)
include $(MAIN_MAKEFILE)
include $(VMCFG_MANAGER_MAKEFILE)
include $(PROJECT_MANAGER_MAKEFILE)
endif

ifeq (_SVR_APISVR,$(_SVR_TYPE))
include $(OPERATOR_LOG_MAKEFILE)
include $(MAIN_MAKEFILE)
include $(REQUEST_MANAGER_MAKEFILE)
include $(ALARM_MANAGER_MAKEFILE)
include $(AUTH_MANAGER_MAKEFILE)
include $(SNMP_AGENT_MAKEFILE)
endif

ifeq (_SVR_IMAGE_MANAGER,$(_SVR_TYPE))
include $(MAIN_MAKEFILE)
include $(IMAGE_MANAGER_MAKEFILE)
#include $(_TECS_COMMON_STORAGE_MAKEFILE)
endif

ifeq (_SVR_FILE_MANAGER,$(_SVR_TYPE))
include $(MAIN_MAKEFILE)
include $(FILE_MANAGER_MAKEFILE)
endif

export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS)  $(_TC_ALL_OBJECTS)
tcobj: $(_VERSION_OBJS_ALL)
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_TC_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_TC_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) tc has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_TC_TMP_OBJ_PATH)\*.o $(_TC_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_TC_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory hc !Go..
	-$(RMDIR) $(_TC_TMP_OBJ_PATH) $(_TC_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) tc has been cleaned all!
	$(ECHO) -----------------------------------------------------------------