WORKPATH = $(EXAMDIR)/$(EXAMPLE)
DEPENDPATH = $(EXAM_TMP_DIR)/$(EXAMPLE)/depend
OBJPATH= $(EXAM_TMP_DIR)/$(EXAMPLE)/object
EXEPATH= $(EXAM_TMP_DIR)/$(EXAMPLE)

CPPFILES = $(wildcard $(WORKPATH)/*.cpp)
DEPENDS = $(addprefix $(DEPENDPATH)/,$(subst .cpp,.d,$(notdir $(CPPFILES))))
OBJECTS = $(addprefix $(OBJPATH)/,$(subst .cpp,.o,$(notdir $(CPPFILES))))

#定义从CPP生成DEP的模式规则
$(DEPENDPATH)/%.d:$(WORKPATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(DEPENDPATH) ]; then $(MKDIR) $(DEPENDPATH); fi    
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(OBJPATH))\/$*.o \: /g" > $@

#定义从CPP生成OBJ的模式规则
$(OBJPATH)/%.o:$(WORKPATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(OBJPATH) ]; then $(MKDIR) $(OBJPATH); fi   
	$(CC) $(CPPFLAGS) -c $< -o $@

#定义从OBJ生成EXE的模式规则
#每个演示用例只能有一个源文件，最终生成的exe名称就是将源文件的名称的后缀名改成exe
#要求演示用例写得尽可能简单易懂，演示所需的全部功能必须在单个源文件中完成
$(EXEPATH)/%.exe:$(OBJPATH)/%.o
	$(ECHO) make $(notdir $@)
	$(CC) $(CPPFLAGS) -o $@  $<  $(LIBS)

-include $(DEPENDS)
    