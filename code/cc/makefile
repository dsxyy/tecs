include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: ccobj clean cleanall
default: ccobj

include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_TECS_COMMON_DB_MAKEFILE)
include $(_TECS_COMMON_ALARM_MAKEFILE)
include $(_TECS_COMMON_MS_MAKEFILE)
#include $(_TECS_COMMON_STORAGE_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
include $(_TECS_COMMON_MISC_MAKEFILE)
include $(_CC_COMMON_MAKEFILE)

ifeq (_SVR_CC,$(_SVR_TYPE))
include $(_HOST_MANAGER_MAKEFILE)
include $(_VM_MANAGER_MAKEFILE)
include $(_CCSVR_MAKEFILE)
include $(_VNET_MANAGER_MAKEFILE)
include $(_VSTORAGE_MANAGER_MAKEFILE)
include $(_CC_NETADAPTER_MAKEFILE)
endif

ifeq (_SVR_IMAGE_AGENT,$(_SVR_TYPE))
include $(_TECS_COMMON_IMAGE_MAKEFILE)
include $(_CC_IMAGE_AGENT_MAKEFILE)
include $(_CCSVR_MAKEFILE)
endif


export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS) $(_CC_ALL_OBJECTS) 
ccobj: $(_VERSION_OBJS_ALL) 
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_CC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CC_TMP_OBJ_PATH); fi
	@if [ ! -d $(_CC_TMP_DEP_PATH) ]; then $(MKDIR) $(_CC_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_CC_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_CC_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) cc has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_CC_TMP_OBJ_PATH)\*.o $(_CC_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_CC_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory cc !Go..
	-$(RMDIR) $(_CC_TMP_OBJ_PATH) $(_CC_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) cc has been cleaned all!
	$(ECHO) -----------------------------------------------------------------
