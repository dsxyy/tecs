include $(_TECS_MAK_PATH)/compileoption.mak
include ./makeconfig

.PHONY: commonobj clean cleanall
default: commonobj

include $(_TECS_COMMON_INIT_MAKEFILE)
include $(_TECS_COMMON_DB_MAKEFILE)
include $(_TECS_COMMON_ALARM_MAKEFILE)
include $(_TECS_COMMON_MS_MAKEFILE)
#include $(_TECS_COMMON_STORAGE_MAKEFILE)
include $(_TECS_COMMON_LOG_MAKEFILE)
include $(_TECS_COMMON_SPECIAL_DEVICE_MAKEFILE)
include $(_TECS_COMMON_MISC_MAKEFILE)

commonobj: $(_TECS_COMMON_OBJS) 

clean:
	$(ECHO) Deleting Files $(_TECS_COMMON_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_TECS_COMMON_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) tecs common objects has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_TECS_COMMON_TMP_DEP_PATH)\*.d !Go..
	-$(RM) $(_TECS_COMMON_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory common! Go..
	-$(RMDIR) $(_TECS_COMMON_TMP_OBJ_PATH) $(_TECS_COMMON_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) tecs common has been cleaned all!
	$(ECHO) -----------------------------------------------------------------