include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: utilsobj clean cleanall
default: utilsobj

ifeq (_SVR_VERSION,$(_SVR_TYPE))
include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_TECS_COMMON_DB_MAKEFILE)
#include $(_TECS_COMMON_MS_MAKEFILE)
include $(_PACKAGE_MANAGER_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
endif

ifeq (_SVR_TC_SSH_TRUST,$(_SVR_TYPE))
include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_SSH_TRUST_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
endif

ifeq (_SVR_CC_SSH_TRUST,$(_SVR_TYPE))
include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_SSH_TRUST_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
endif

include $(_UTILSSVR_MAKEFILE)

export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS) $(_UTILS_ALL_OBJECTS)
utilsobj: $(_VERSION_OBJS_ALL)
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_UTILS_TMP_OBJ_PATH) ]; then $(MKDIR) $(_UTILS_TMP_OBJ_PATH); fi
	@if [ ! -d $(_UTILS_TMP_DEP_PATH) ]; then $(MKDIR) $(_UTILS_TMP_DEP_PATH); fi
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done!

clean:
	$(ECHO) Deleting Files $(_UTILS_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_UTILS_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) utils has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_UTILS_TMP_OBJ_PATH)\*.o $(_UTILS_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_UTILS_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory utils !Go..
	-$(RMDIR) $(_UTILS_TMP_OBJ_PATH) $(_UTILS_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) utils has been cleaned all!
	$(ECHO) -----------------------------------------------------------------
