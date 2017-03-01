export EXAM_TMP_DIR = $(TARGET)/examples
export EXAM_CONFIG_FILE = $(EXAMDIR)/config.mak
CPPFLAGS += -I $(INCDIR)
default: message timer sys shell serialization dbs misc workflow

timer:
	@$(MAKE) default -f $(EXAMDIR)/timer/makefile 

message:
	@$(MAKE) default -f $(EXAMDIR)/message/makefile 
    
sys:
	@$(MAKE) default -f $(EXAMDIR)/sys/makefile 

shell:
	@$(MAKE) default -f $(EXAMDIR)/shell/makefile 
    
serialization:
	@$(MAKE) default -f $(EXAMDIR)/serialization/makefile 
    
dbs:
	@$(MAKE) default -f $(EXAMDIR)/dbs/makefile 
    
misc:
	@$(MAKE) default -f $(EXAMDIR)/misc/makefile 

workflow:
	@$(MAKE) default -f $(EXAMDIR)/workflow/makefile
