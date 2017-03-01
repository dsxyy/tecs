# makefile for workflow
WORKFLOW_PATH = $(SRCDIR)/workflow
WORKFLOW_CPP = $(wildcard $(WORKFLOW_PATH)/*.cpp)
WORKFLOW_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(WORKFLOW_CPP))))
WORKFLOW_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(WORKFLOW_CPP))))
WF_OBJECTS += $(WORKFLOW_OBJ)

$(SKY_DEP_DIR)/%.d:$(WORKFLOW_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(WORKFLOW_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi    
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(WORKFLOW_DEP)
