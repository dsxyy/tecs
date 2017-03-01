# makefile for misc
MISC_PATH = $(SRCDIR)/misc
MISC_CPP = $(wildcard $(MISC_PATH)/*.cpp)
MISC_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(MISC_CPP))))
MISC_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(MISC_CPP))))
SKY_OBJECTS += $(MISC_OBJ)

$(SKY_DEP_DIR)/%.d:$(MISC_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(MISC_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi        
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(MISC_DEP)
    