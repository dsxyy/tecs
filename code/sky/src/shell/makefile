# makefile for shell
SHELL_PATH = $(SRCDIR)/shell
#SHELL_LEX = $(wildcard $(SHELL_PATH)/*.l)
#SHELL_LEX_CPP = $(subst .l,.cpp,$(SHELL_LEX))
SHELL_CPP = $(wildcard $(SHELL_PATH)/*.cpp) 
SHELL_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(SHELL_CPP))))
SHELL_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(SHELL_CPP))))
SKY_OBJECTS += $(SHELL_OBJ)

#lexclean:
#	@$(RM) $(SHELL_PATH)/parser.cpp $(SHELL_PATH)/parser.h
    
#$(SHELL_PATH)/parser.cpp: lexclean $(SHELL_PATH)/parser.l
#	$(ECHO) lex  parser.l
# 根据lex文件parser.l生成词法分析器的命令：
# flex -R  -L --outfile=$(SHELL_PATH)/parser.cpp --header-file=$(SHELL_PATH)/parser.h --prefix=sky $(SHELL_PATH)/parser.l
# 注意flex的版本必须是2.5.31以上，RHEL5默认带的版本不支持-R可重入等选项
# 生成的分析器源代码直接编译可能会有告警，需要解决
    
$(SKY_DEP_DIR)/%.d:$(SHELL_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(SHELL_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi    
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(SHELL_DEP)
