# makefile for sys
SYS_PATH = $(SRCDIR)/sys
SYS_CPP = $(wildcard $(SYS_PATH)/*.cpp)
SYS_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(SYS_CPP))))
SYS_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(SYS_CPP))))
SKY_OBJECTS += $(SYS_OBJ)

$(SKY_DEP_DIR)/%.d:$(SYS_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(SYS_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi    
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(SYS_DEP)
