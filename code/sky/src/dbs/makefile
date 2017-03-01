# makefile for dbs
DBS_PATH = $(SRCDIR)/dbs
DBS_CPP = $(wildcard $(DBS_PATH)/*.cpp)
DBS_DEP = $(addprefix $(SKY_DEP_DIR)/,$(subst .cpp,.d,$(notdir $(DBS_CPP))))
DBS_OBJ = $(addprefix $(SKY_OBJ_DIR)/,$(subst .cpp,.o,$(notdir $(DBS_CPP))))
DB_OBJECTS += $(DBS_OBJ)

$(SKY_DEP_DIR)/%.d:$(DBS_PATH)/%.cpp
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(SKY_DEP_DIR) ]; then $(MKDIR) $(SKY_DEP_DIR); fi
	$(CC) -MM $(CPPFLAGS)  $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(SKY_OBJ_DIR))\/$*.o \: /g" > $@
    
$(SKY_OBJ_DIR)/%.o:$(DBS_PATH)/%.cpp
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(SKY_OBJ_DIR) ]; then $(MKDIR) $(SKY_OBJ_DIR); fi
	$(CC) $(CPPFLAGS) -c $< -o $@

-include $(DBS_DEP)
