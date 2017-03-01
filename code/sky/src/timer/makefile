# makefile for timer
TIMER_PATH = $(SRCDIR)/timer
TIMER_CPP = $(wildcard $(TIMER_PATH)/*.cpp)
TIMER_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(TIMER_CPP))))
TIMER_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(TIMER_CPP))))
SKY_OBJECTS += $(TIMER_OBJ)

$(SKY_DEP_DIR)/%.d:$(TIMER_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(TIMER_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi    
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(TIMER_DEP)
