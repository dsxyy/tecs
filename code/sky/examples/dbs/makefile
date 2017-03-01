.PHONY: default
all:default
EXAMPLE = dbs
LIBS = $(DISTDIR)/$(LIBSKYNAME).a $(DISTDIR)/$(LIBDBNAME).a  $(OSLIBS) $(QPIDLIBS) /usr/local/pgsql/lib/libpq.so
include $(EXAM_CONFIG_FILE)
default:$(EXEPATH)/test_dbs.exe
$(EXEPATH)/test_dbs.exe:$(OBJPATH)/test_main.o $(OBJPATH)/student.o $(OBJPATH)/student_pool.o $(OBJPATH)/sample_type.o $(OBJPATH)/person_pool.o
	$(ECHO) make $(notdir $@)
	$(CC) -o $@   $^ $(LIBS)  