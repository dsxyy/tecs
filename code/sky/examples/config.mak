WORKPATH = $(EXAMDIR)/$(EXAMPLE)
DEPENDPATH = $(EXAM_TMP_DIR)/$(EXAMPLE)/depend
OBJPATH= $(EXAM_TMP_DIR)/$(EXAMPLE)/object
EXEPATH= $(EXAM_TMP_DIR)/$(EXAMPLE)

CPPFILES = $(wildcard $(WORKPATH)/*.cpp)
DEPENDS = $(addprefix $(DEPENDPATH)/,$(subst .cpp,.d,$(notdir $(CPPFILES))))
OBJECTS = $(addprefix $(OBJPATH)/,$(subst .cpp,.o,$(notdir $(CPPFILES))))

#�����CPP����DEP��ģʽ����
$(DEPENDPATH)/%.d:$(WORKPATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(DEPENDPATH) ]; then $(MKDIR) $(DEPENDPATH); fi    
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(OBJPATH))\/$*.o \: /g" > $@

#�����CPP����OBJ��ģʽ����
$(OBJPATH)/%.o:$(WORKPATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(OBJPATH) ]; then $(MKDIR) $(OBJPATH); fi   
	$(CC) $(CPPFLAGS) -c $< -o $@

#�����OBJ����EXE��ģʽ����
#ÿ����ʾ����ֻ����һ��Դ�ļ����������ɵ�exe���ƾ��ǽ�Դ�ļ������Ƶĺ�׺���ĳ�exe
#Ҫ����ʾ����д�þ����ܼ��׶�����ʾ�����ȫ�����ܱ����ڵ���Դ�ļ������
$(EXEPATH)/%.exe:$(OBJPATH)/%.o
	$(ECHO) make $(notdir $@)
	$(CC) $(CPPFLAGS) -o $@  $<  $(LIBS)

-include $(DEPENDS)
    