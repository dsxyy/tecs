# makefile for message
MESSAGE_PATH = $(SRCDIR)/message
MESSAGE_CPP = $(wildcard $(MESSAGE_PATH)/*.cpp)
MESSAGE_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(MESSAGE_CPP))))
MESSAGE_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(MESSAGE_CPP))))
SKY_OBJECTS += $(MESSAGE_OBJ)

$(SKY_DEP_DIR)/%.d:$(MESSAGE_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(MESSAGE_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(MESSAGE_DEP)
