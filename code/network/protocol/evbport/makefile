#Makefile_for_VNET_PROTOCOL_EVBPORT

NETWORK_PROTOCOL_EVBPORT_PATH = $(_NETWORK_PROTOCOL_EVBPORT_PATH)/source
NETWORK_PROTOCOL_EVBPORT_C = $(wildcard $(NETWORK_PROTOCOL_EVBPORT_PATH)/*.c)
NETWORK_PROTOCOL_EVBPORT_DEP = $(addprefix $(_NETWORK_TMP_DEP_PATH)/,$(subst .c,.d,$(notdir $(NETWORK_PROTOCOL_EVBPORT_C))))
NETWORK_PROTOCOL_EVBPORT_OBJ = $(addprefix $(_NETWORK_TMP_OBJ_PATH)/,$(subst .c,.o,$(notdir $(NETWORK_PROTOCOL_EVBPORT_C))))

_NETWORK_ALL_OBJECTS +=$(NETWORK_PROTOCOL_EVBPORT_OBJ)


$(_NETWORK_TMP_DEP_PATH)/%.d:$(NETWORK_PROTOCOL_EVBPORT_PATH)/%.c
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_NETWORK_TMP_DEP_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_NETWORK_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_NETWORK_TMP_OBJ_PATH)/%.o:$(NETWORK_PROTOCOL_EVBPORT_PATH)/%.c
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_NETWORK_TMP_OBJ_PATH) ]; then $(MKDIR) $(_NETWORK_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(NETWORK_PROTOCOL_EVBPORT_DEP)
endif
