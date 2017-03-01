_OAM_CLI_TELNETSVR_ALL_C = $(wildcard $(_OAM_CLI_TELNETSVR_PATH)/source/*.c)
_OAM_CLI_TELNETSVR_ALL_OBJS = $(addprefix $(TMP_DIR)/,$(subst .c,.o,$(notdir $(_OAM_CLI_TELNETSVR_ALL_C))))

_CLI_ALL_OBJECTS  +=  $(_OAM_CLI_TELNETSVR_ALL_OBJS)

#编译生成目标对象
$(TMP_DIR)/%.o : $(_OAM_CLI_TELNETSVR_PATH)/source/%.c
	$(ECHO) start compile $<
	@if [ ! -d $(TMP_DIR) ]; then $(MKDIR) $(TMP_DIR); fi
	$(ECHO) Making $@ ......
	$(CC) $(_OAM_CLI_CFLAGS) -c $< -o $@
	$(ECHO) Done!

