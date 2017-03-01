ifeq ($(FROM_TECS),TRUE)
include $(_TECS_MAK_PATH)/compileoption.mak
export SKY_DEP_DIR = $(_TEMP_DEP_PATH)/sky
export SKY_OBJ_DIR = $(_TEMP_OBJ_PATH)/sky
CPPFLAGS += -D_FROMTECS
else
export SKY_DEP_DIR = $(TMPDIR)/depend
export SKY_OBJ_DIR = $(TMPDIR)/object
CPPFLAGS += -D_SKYONLY
endif
export SKY_OBJECTS = 
export DB_OBJECTS = 
export WF_OBJECTS = 
CPPFLAGS += -I $(INCDIR)
CPPFLAGS += -I $(SRCDIR)/common

CPPFLAGS += -I $(SRCDIR)/misc
include $(SRCDIR)/misc/makefile

CPPFLAGS += -I $(SRCDIR)/sys
include $(SRCDIR)/sys/makefile

CPPFLAGS += -I $(SRCDIR)/message
include $(SRCDIR)/message/makefile

CPPFLAGS += -I $(SRCDIR)/timer
include $(SRCDIR)/timer/makefile

CPPFLAGS += -I $(SRCDIR)/shell
include $(SRCDIR)/shell/makefile

CPPFLAGS += -I $(SRCDIR)/serialization
include $(SRCDIR)/serialization/makefile

CPPFLAGS += -I $(SRCDIR)/dbs 
include $(SRCDIR)/dbs/makefile

CPPFLAGS += -I $(SRCDIR)/workflow
include $(SRCDIR)/workflow/makefile

.PHONY: default
default: dir $(SKY_OBJECTS) $(DB_OBJECTS) $(WF_OBJECTS)
ifeq (DYNAMIC,$(_LIB_TYPE))
	$(ECHO) making $(LIBSKYNAME).so ...		
	$(CC) $(CROSS_FLAGS) -shared -o $(DISTDIR)/$(LIBSKYNAME).so $(SKY_OBJECTS) $(_TECS_CONTRIB_LIB_PATH) $(QPIDLIBS) $(OSLIBS)
	$(ECHO) making $(LIBDBNAME).so ...		
	$(CC) $(CROSS_FLAGS) -shared -o $(DISTDIR)/$(LIBDBNAME).so $(DB_OBJECTS) $(_TECS_CONTRIB_LIB_PATH) $(PQLIBS)
	$(ECHO) making $(LIBWFNAME).so ...
	$(CC) $(CROSS_FLAGS) -shared -o $(DISTDIR)/$(LIBWFNAME).so $(WF_OBJECTS) -L $(DISTDIR)/ $(SKYLIB) $(DBLIB)
else
	$(ECHO) making $(LIBSKYNAME).a ... 
	@$(AR) -cr $(DISTDIR)/$(LIBSKYNAME).a $(SKY_OBJECTS)
	$(ECHO) making $(LIBDBNAME).a ... 
	@$(AR) -cr $(DISTDIR)/$(LIBDBNAME).a $(DB_OBJECTS)
	$(ECHO) making $(LIBWFNAME).a ... 
	@$(AR) -cr $(DISTDIR)/$(LIBWFNAME).a $(SKY_OBJECTS) $(DB_OBJECTS) $(WF_OBJECTS)
endif
	$(ECHO) Done!
 
dir:
	@if [ ! -d $(OBJDIR) ]; then $(MKDIR) $(OBJDIR); fi    
	@if [ ! -d $(DEPDIR) ]; then $(MKDIR) $(DEPDIR); fi    
	@if [ ! -d $(DISTDIR) ]; then $(MKDIR) $(DISTDIR); fi    
	
    