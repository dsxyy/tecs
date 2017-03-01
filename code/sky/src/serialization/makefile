# makefile for serialization
SERIALIZATION_PATH = $(SRCDIR)/serialization
SERIALIZATION_CPP = $(wildcard $(SERIALIZATION_PATH)/*.cpp)
SERIALIZATION_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(SERIALIZATION_CPP))))
SERIALIZATION_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(SERIALIZATION_CPP))))
SKY_OBJECTS += $(SERIALIZATION_OBJ)

$(SKY_DEP_DIR)/%.d:$(SERIALIZATION_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(SERIALIZATION_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi        
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(SERIALIZATION_DEP)
    