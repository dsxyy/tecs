#config for TECS lib_rm
export TECS_DIR = /pub/software/yinhao
export SKY_DIR = $(TECS_DIR)/code/sky
export SKY_INC= $(SKY_DIR)/include
export COM_INC = $(TECS_DIR)/code/tc/common/include
export BASE_DIR = $(shell pwd)
export TMP_DIR = $(BASE_DIR)/tmp
export SRC_DIR = $(BASE_DIR)/src
export INC_DIR = $(BASE_DIR)/include
export EXAM_DIR = $(BASE_DIR)/examples
export DIST_DIR = $(BASE_DIR)/dist
export LIBNAME = lib_rm
#export LIBTYPE = DYNAMIC
export LIBTYPE = STATIC

export CC = @g++
export LD = @ld
export AR = ar -cr
CPPFLAGS  += -g  -I $(INC_DIR) -I $(SKY_INC) -I $(COM_INC)
ifeq ($(LIBTYPE),DYNAMIC)
	CPPFLAGS  += -fPIC
endif
export CPPFLAGS  += -Wall 
export QPIDLIBS = -lqpidclient -lqpidmessaging -lpthread
export ECHO = @echo
export MAKE = @make
export MKDIR = mkdir -p
export RM = rm -rf

export CLI_DIR = $(TECS_DIR)/code/tc/cli
exprot _CLI_ALL_OBJECTS = 
