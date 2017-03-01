include $(_TECS_MAK_PATH)/compileoption.mak
include  $(_TECS_CODE_PATH)/network/plugin/makeconfig
include  $(_TECS_CODE_PATH)/network/plugin/common/makeconfig
include  $(_TECS_CODE_PATH)/network/plugin/bridge/makeconfig
include  $(_TECS_CODE_PATH)/network/plugin/ovs/makeconfig
include  $(_TECS_CODE_PATH)/network/plugin/libnet/makeconfig
include  $(_TECS_CODE_PATH)/common/makeconfig


.PHONY: networkobj clean cleanall test
default: networkobj


ifeq (_LIB_NETWORK_BR,$(_LIB_VER))
include $(_TECS_COMMON_MAKEFILE)
include $(_NETWORK_PLUGIN_COMMON_MAKEFILE)
include $(_NETWORK_PLUGIN_BR_MAKEFILE)
endif 

ifeq (_LIB_NETWORK_OVS,$(_LIB_VER))
include $(_NETWORK_PLUGIN_COMMON_MAKEFILE)
include $(_NETWORK_PLUGIN_OVS_MAKEFILE)
endif 


ifeq (_LIB_NETWORK_NET,$(_LIB_VER))
include $(_TECS_COMMON_MAKEFILE)
include $(_NETWORK_PLUGIN_NET_MAKEFILE)
endif 

#	$(ECHO) lib network br $(_LIB_VER) 
#	$(ECHO) netwrok include $(_NETWORK_INCLUDE_PATH) 
#	$(ECHO) br makefile $(_NETWORK_PLUGIN_BR_MAKEFILE) 	
#	$(ECHO) br_main makefile $(_NETWORK_PLUGIN_BR_MAIN_MAKEFILE) 		
#	$(ECHO) br_main path $(_NETWORK_PLUGIN_BR_MAIN_PATH) 	


export _VERSION_OBJS_ALL = $(_TECS_COMMON_OBJS) $(_NETWORK_ALL_OBJECTS)
networkobj: $(_VERSION_OBJS_ALL)
	$(ECHO) network all obj $(_NETWORK_ALL_OBJECTS) 
	$(ECHO) generate $(_VERSION_NAME) begin...
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
#	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE)
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak lib _LINK_TYPE=$(_LINK_TYPE)
	$(ECHO) Done network plugin !


clean:
	$(ECHO) Deleting Files $(_NETWORK_TMP_OBJ_PATH)\*.o !Go..
	-$(RM) $(_NETWORK_TMP_OBJ_PATH)/*.o 
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) network plugin has been cleaned!
	$(ECHO) -----------------------------------------------------------------

cleanall: clean
	$(ECHO) Deleting Files $(_NETWORK_TMP_OBJ_PATH)\*.o $(_NETWORK_TMP_OBJ_PATH)\*.d !Go..
	-$(RM) $(_NETWORK_TMP_DEP_PATH)/*.d
	$(ECHO) Deleting Directory network !Go..
	-$(RMDIR) $(_NETWORK_TMP_OBJ_PATH) $(_NETWORK_TMP_DEP_PATH)
	$(ECHO) -----------------------------------------------------------------
	$(ECHO) network plugin has been cleaned all!
	$(ECHO) -----------------------------------------------------------------
	