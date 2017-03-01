# makefile for COMMON_MS
COMMON_MS_PATH = $(_TECS_CODE_PATH)/common/ms
COMMON_MS_CPP  = $(wildcard $(COMMON_MS_PATH)/*.cpp)
COMMON_MS_DEP = $(addprefix $(_TECS_COMMON_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(COMMON_MS_CPP))))
COMMON_MS_OBJ = $(addprefix $(_TECS_COMMON_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(COMMON_MS_CPP))))

CPPFLAGS += $(addprefix -I,$(_TECS_CONTRIB_PATH)/openais/include)
DEPCPPFLAGS += $(addprefix -I,$(_TECS_CONTRIB_PATH)/openais/include)

_OTHER_LINK_LIB += $(_TECS_CONTRIB_PATH)/openais/lib/libSaCkpt.a \
                   $(_TECS_CONTRIB_PATH)/openais/lib/libSaClm.a \
                   $(_TECS_CONTRIB_PATH)/openais/lib/libSaEvt.a \
                   $(_TECS_CONTRIB_PATH)/openais/lib/libSaLck.a \
                   $(_TECS_CONTRIB_PATH)/openais/lib/libSaMsg.a

_TECS_COMMON_OBJS += $(COMMON_MS_OBJ)

$(_TECS_COMMON_TMP_DEP_PATH)/%.d:$(COMMON_MS_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_DEP_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TECS_COMMON_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TECS_COMMON_TMP_OBJ_PATH)/%.o:$(COMMON_MS_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TECS_COMMON_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TECS_COMMON_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(COMMON_MS_DEP)
endif

