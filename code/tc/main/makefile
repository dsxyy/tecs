
# makefile for tc
TC_SVR_PATH = $(_TC_TCSVR_PATH)
ifeq (_SVR_TC,$(_SVR_TYPE))
TC_SVR_CPP = tc.cpp
endif

ifeq (_SVR_APISVR,$(_SVR_TYPE))
TC_SVR_CPP = apisvr.cpp
endif

ifeq (_SVR_TC_STORAGE,$(_SVR_TYPE))
TC_SVR_CPP = tc_storage.cpp
endif

ifeq (_SVR_IMAGE_MANAGER,$(_SVR_TYPE))
TC_SVR_CPP = tc_image.cpp
endif

ifeq (_SVR_FILE_MANAGER,$(_SVR_TYPE))
TC_SVR_CPP = tc_file_manager.cpp
endif

TC_SVR_DEP = $(addprefix $(_TC_TMP_DEP_PATH)/,$(subst .cpp,.d,$(notdir $(TC_SVR_CPP))))
TC_SVR_OBJ = $(addprefix $(_TC_TMP_OBJ_PATH)/,$(subst .cpp,.o,$(notdir $(TC_SVR_CPP))))

_TC_ALL_OBJECTS += $(TC_SVR_OBJ)

$(_TC_TMP_DEP_PATH)/%.d:$(TC_SVR_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_TC_TMP_DEP_PATH) ]; then $(MKDIR) $(_TC_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_TC_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_TC_TMP_OBJ_PATH)/%.o:$(TC_SVR_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_TC_TMP_OBJ_PATH) ]; then $(MKDIR) $(_TC_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@

ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(TC_SVR_DEP)
endif
