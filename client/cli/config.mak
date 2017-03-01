export BASE_DIR = $(shell pwd)
export TMP_DIR = $(BASE_DIR)/tmp
export LIBTYPE = DYNAMIC
#export LIBTYPE = STATIC
export CC = @g++
export LD = @ld
export AR = ar
CPPFLAGS  += -g  
ifeq ($(LIBTYPE),DYNAMIC)
	CPPFLAGS  += -fPIC
endif
export CPPFLAGS  += -Wall 
LIBS_CLIENT = -lxmlrpc++ -lxmlrpc_client++

ifeq ($(FROM_TECS),TRUE)
include $(_TECS_MAK_PATH)/compileoption.mak
export BASE_DIR = $(_TECS_CLI_PATH)
export CLI_DEP_DIR = $(_TEMP_DEP_PATH)/cli
export CLI_OBJ_DIR = $(_TEMP_OBJ_PATH)/cli
export TMP_DIR = $(_TEMP_OBJ_PATH)/cli
export _VERSION_NAME = CLI
export _OTHER_LINK_LIB = $(LIBS_CLIENT)
export PROCESS=cli
endif

export EXAM_DIR = $(BASE_DIR)/examples
export DIST_DIR = $(BASE_DIR)/dist

export LIBS = -lpthread -lrt -lboost_regex
export ECHO = @echo
export MAKE = @make
export MKDIR = mkdir -p
export RM = rm -rf

export CLI_DIR = $(BASE_DIR)
export _CLI_ALL_OBJECTS = 
export _OAM_CLIMANAGER_PATH = $(CLI_DIR)/climanager
export _OAM_CLI_PUB_PATH = $(CLI_DIR)/pub
export _OAM_CLI_TELNETSVR_PATH = $(CLI_DIR)/telnetserver
export _OAM_CLI_SSHSVR_PATH = $(CLI_DIR)/sshserver
export _OAM_CLI_APP_PATH = $(CLI_DIR)/cliapp
export _OAM_CLI_VTY_PATH = $(CLI_DIR)/clivty
export _TECSAPI_PATH = $(CLI_DIR)/api

export _XMLRPC_LIB_PATH = $(CLI_DIR)/xmlrpc

export _OAM_INCLUDE_PATH = -I$(_TECSAPI_PATH)/include \
                           -I$(_OAM_CLIMANAGER_PATH)/include \
                           -I$(_OAM_CLI_APP_PATH)/include \
                           -I$(_OAM_CLI_PUB_PATH)/include \
                           -I$(_OAM_CLI_TELNETSVR_PATH)/include \
                           -I$(_OAM_CLI_SSHSVR_PATH)/include \
                           -I$(_OAM_CLI_VTY_PATH)/include \
                           -I$(_XMLRPC_LIB_PATH)/include/
                           
export _OAM_CLI_CFLAGS = $(_OAM_INCLUDE_PATH) $(CPPFLAGS) -D_GNU_SOURCE