ifeq (_LIB_AUTH,$(_LIB_VER))
include $(_TECS_MAK_PATH)/compileoption.mak
include ../makeconfig
#export AR = ar -cr
export LIBNAME = libauth
endif
	
AUTH_MANAGER_PATH = $(_TC_AUTH_MANAGER_PATH)/src
AUTH_MANAGER_CPP = $(wildcard $(AUTH_MANAGER_PATH)/*.cpp)
AUTH_MANAGER_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(AUTH_MANAGER_CPP))))
AUTH_MANAGER_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(AUTH_MANAGER_CPP))))

ifeq (_LIB_AUTH,$(_LIB_VER))
AUTH_LIB_OBJ = $(subst $(_TC_TMP_OBJ_PATH)/userapi.o, ,$(AUTH_MANAGER_OBJ))
endif

_TC_ALL_OBJECTS += $(AUTH_MANAGER_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(AUTH_MANAGER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)	
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(AUTH_MANAGER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (_LIB_AUTH,$(_LIB_VER))
.PHONY: default
default: $(AUTH_LIB_OBJ)
	$(ECHO) $(AUTH_LIB_OBJ)
ifeq (DYNAMIC,$(_LIB_TYPE))
	$(ECHO) making $(LIBNAME).so ...		
	$(CC) $(CROSS_FLAGS) -shared -o $(_TARGET_LIB_PATH)/$(LIBNAME).so $(AUTH_LIB_OBJ) 
else
	$(ECHO) making $(LIBNAME).a ... 
	$(AR) -cr $(_TARGET_LIB_PATH)/$(LIBNAME).a $(AUTH_LIB_OBJ)
endif
	$(ECHO) Done!
	
clean:
	$(ECHO) -----------------------------------------------------------------	
	$(ECHO) Deleting Files $(AUTH_MANAGER_OBJ) !Go..
	-$(RM) $(AUTH_MANAGER_OBJ) 
	$(ECHO) -----------------------------------------------------------------
endif

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(AUTH_MANAGER_DEP)
endif
