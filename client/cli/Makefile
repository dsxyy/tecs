include config.mak
include ./pub/pub.mak
include ./cliapp/cliapp.mak
include ./climanager/climanager.mak
include ./clivty/clivty.mak
include ./telnetserver/telnetserver.mak
include ./sshserver/sshserver.mak

ifeq ($(SRCDIR),)
SRCDIR = $(CURDIR)/xmlrpc
BLDDIR = $(SRCDIR)
endif
XMLRPC_C_CONFIG = $(BLDDIR)/xmlrpc-c-config.test
#LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) c++2 client --libs)

.PHONY: all cli clean

all:prepare_tecsapi cli
   
export _VERSION_OBJS_ALL = $(_CLI_ALL_OBJECTS)
   
cli:$(_CLI_ALL_OBJECTS)
ifeq ($(FROM_TECS),TRUE)
	$(MAKE) -C $(_TECS_MAK_PATH) -f link.mak svr _LINK_TYPE=$(_LINK_TYPE) 
else	
	$(CC) -o $@ $+ $(LIBS) $(LIBS_CLIENT)
endif
    
prepare_tecsapi:
	@if [ ! -e $(_TECSAPI_PATH) ]; then ln -s $(CLI_DIR)/../../code/api $(_TECSAPI_PATH); fi

clean:
	@$(RM) $(TMP_DIR)/
	@$(RM) $(DIST_DIR)/*
	$(ECHO) $@ done!
    
