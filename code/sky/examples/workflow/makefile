.PHONY: default
all:default
EXAMPLE = workflow
LIBS = $(DISTDIR)/$(LIBWFNAME).a $(OSLIBS) $(QPIDLIBS) /usr/local/pgsql/lib/libpq.so
include $(EXAM_CONFIG_FILE)
default:$(EXEPATH)/test_workflow.exe
$(EXEPATH)/test_workflow.exe:$(OBJPATH)/test_workflow.o
	$(ECHO) make $(notdir $@)
	$(CC) -o $@   $^ $(LIBS)