#######################################################################
###
###     this file is used to config config Version Information, such as
###        Version Path and VersionNo, and so on...
###
###     Author:  Jixuepeng, CSP
###     History:
###     1. jixuepeng,2011/07/21  ���� 
#######################################################################

##��Windows����PCLint���ʱ����
ifeq (_OS_NT,$(_COMPILE_TYPE))
export _TECS_ROOT_PATH           =    z:
else
export _TECS_ROOT_PATH           =    $(shell pwd)/..
endif

export _TECS_CODE_PATH           = $(_TECS_ROOT_PATH)/code
export _TECS_API_PATH            = $(_TECS_ROOT_PATH)/code/api
export _TECS_CONTRIB_PATH        = $(_TECS_ROOT_PATH)/contrib
export _TECS_CONFIG_PATH         = $(_TECS_ROOT_PATH)/config
export _TECS_DBSCHEMA_PATH       = $(_TECS_ROOT_PATH)/config/sql
export _TECS_TARGET_PATH         = $(_TECS_ROOT_PATH)/target
export _TECS_TMP_PATH            = $(_TECS_ROOT_PATH)/tmp
export _TECS_MAK_PATH            = $(_TECS_ROOT_PATH)/make
export _TECS_TOOLS_PATH          = $(_TECS_ROOT_PATH)/tools
export _TECS_RPM_PATH            = $(_TECS_ROOT_PATH)/rpm
export _TECS_CLIENT_PATH         = $(_TECS_ROOT_PATH)/client
export _TECS_CLI_PATH            = $(_TECS_ROOT_PATH)/client/cli
export _TECS_NET_PATH            = $(_TECS_ROOT_PATH)/code/network

## ���빤����·��
export CROSS_COMPILE_BASE        = /opt
export LINT_CROSS_COMPILE_BASE   =X:

export _TECS_VERNO        =    01.01.20
##���ɲ��԰汾��
export VER_I              = 0
##ϵͳ���԰汾��
export VER_B              = 0
##���ⷢ���汾��
export VER_P              = 0
##RELEASE�汾��,�˱���һ����Ҫ�ڱ����ʱ��ָ��
export _VER_REL           = $(VER_P).$(VER_B).$(VER_I)




