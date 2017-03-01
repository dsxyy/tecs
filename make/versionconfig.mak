#######################################################################
###
###     this file is used to config config Version Information, such as
###        Version Path and VersionNo, and so on...
###
###     Author:  Jixuepeng, CSP
###     History:
###     1. jixuepeng,2011/07/21  创建 
#######################################################################

##在Windows进行PCLint检测时需求
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

## 编译工具链路径
export CROSS_COMPILE_BASE        = /opt
export LINT_CROSS_COMPILE_BASE   =X:

export _TECS_VERNO        =    01.01.20
##集成测试版本号
export VER_I              = 0
##系统测试版本号
export VER_B              = 0
##对外发布版本号
export VER_P              = 0
##RELEASE版本号,此变量一般需要在编译的时候指定
export _VER_REL           = $(VER_P).$(VER_B).$(VER_I)




