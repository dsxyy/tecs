#_OAM_CLI_APP_ALL_C = $(wildcard $(_OAM_CLI_APP_PATH)/source/*.c)
_OAM_CLI_APP_ALL_CPP = $(wildcard $(_OAM_CLI_APP_PATH)/source/*.cpp)

#_OAM_CLI_APP_ALL_OBJS = $(addprefix $(TMP_DIR)/,$(subst .c,.o,$(notdir $(_OAM_CLI_APP_ALL_C))))
_OAM_CLI_APP_ALL_OBJS_CPP = $(addprefix $(TMP_DIR)/,$(subst .cpp,.o,$(notdir $(_OAM_CLI_APP_ALL_CPP))))

#_CLI_ALL_OBJECTS  +=  $(_OAM_CLI_APP_ALL_OBJS)
_CLI_ALL_OBJECTS  +=  $(_OAM_CLI_APP_ALL_OBJS_CPP)

#编译生成目标对象
$(TMP_DIR)/%.o :  $(_OAM_CLI_APP_PATH)/source/%.cpp
	$(ECHO) start compile $<
	@if [ ! -d $(TMP_DIR) ]; then $(MKDIR) $(TMP_DIR); fi
	$(ECHO) Making $@ ......
	$(CC) $(_OAM_CLI_CFLAGS) -c $< -o $@
	$(ECHO) Done!

