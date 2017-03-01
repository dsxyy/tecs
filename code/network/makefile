include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: networkobj clean cleanall test
default: networkobj

include $(_TECS_COMMON_INIT_MAKEFILE)
ifeq (_SVR_NETWORK_VNM,$(_SVR_TYPE))
include $(_TECS_COMMON_DB_MAKEFILE)
endif
include $(_TECS_COMMON_ALARM_MAKEFILE)
include $(_TECS_COMMON_MS_MAKEFILE)
include $(_TECS_COMMON_VNET_MAKEFILE)
#include $(_TECS_COMMON_STORAGE_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)

export _NETWORK_ALL_OBJECTS =

ifeq (_SVR_NETWORK_VNM,$(_SVR_TYPE))
include $(_NETWORK_COMMON_MAKEFILE)
include $(_NETWORK_VNM_MAKEFILE)
endif

ifeq (_SVR_NETWORK_VNA,$(_SVR_TYPE))
include $(_NETWORK_COMMON_MAKEFILE)
include $(_NETWORK_PROTOCOL_MAKEFILE)
include $(_NETWORK_VNA_MAKEFILE)
endif

ifeq (_SVR_NETWORK_WDG,$(_SVR_TYPE))
include $(_NETWORK_COMMON_MAKEFILE)
include $(_NETWORK_PROTOCOL_MAKEFILE)
include $(_NETWORK_WDG_MAKEFILE)
endif

export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS) $(_NETWORK_ALL_OBJECTS)
networkobj: $(_VERSION_OBJS_ALL)
	$(ECHO) network all obj $(_NETWORK_ALL_OBJECTS) 
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
#	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak lib _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done network !

clean:
	$(ECHO) Deleting Files $(_NETWORK_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_NETWORK_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) network has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_NETWORK_TMP_OBJ_PATH)\*.o $(_NETWORK_TMP_OBJ_PATH)\*.d !Go..
	-$(RM) $(_NETWORK_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory network !Go..
	-$(RMDIR) $(_NETWORK_TMP_OBJ_PATH) $(_NETWORK_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) network has been cleaned all!
	$(ECHO) -----------------------------------------------------------------
