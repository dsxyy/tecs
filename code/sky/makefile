include config.mak
.PHONY: all sky sky_clean examples examples_clean clean cleanall
all:sky examples     
sky:
	$(MAKE) default -f src/makefile 
	$(ECHO) $@ done!
    
sky_clean:
	@$(RM) $(TARGET)/lib*
	@$(RM) $(TEMP)
	$(ECHO) $@ done!
    
examples:sky
	@$(MAKE) default -f $(EXAMDIR)/makefile 
	$(ECHO) $@ done!
    
examples_clean:
	@$(RM) $(TARGET)/examples
	$(ECHO) $@ done!

clean:sky_clean

cleanall:sky_clean examples_clean
	@$(RM) $(TARGET)