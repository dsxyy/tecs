export SKYDIR=$(shell pwd)
export TARGET=$(SKYDIR)/target
export TEMP=$(SKYDIR)/target/tmp
export TMPDIR=$(TEMP)
export DISTDIR=$(TARGET)
export SRCDIR=$(SKYDIR)/src
export INCDIR=$(SKYDIR)/include
export EXAMDIR=$(SKYDIR)/examples
export LIBSKYNAME = libsky
export LIBDBNAME = libtecsdb
export LIBWFNAME = libworkflow
export SKYLIB = -lsky
export DBLIB = -ltecsdb
export WFLIB = -lworkflow
export QPIDLIBS=-lqpidclient -lqpidmessaging
export PQLIBS=-lpq
export OSLIBS=-lpthread -lreadline -lrt -lutil -lncurses -lssl

#单独编译sky时总是生成静态调试库
export _LIB_TYPE=STATIC
export CPPFLAGS += -g -Wall 
ifeq ($(_LIB_TYPE),DYNAMIC)
	CPPFLAGS += -fPIC
endif
export CC = @g++
export LD = @ld
export AR = ar
export ECHO = @echo
export MAKE = @make
export MKDIR = mkdir -p
export RM = rm -rf

