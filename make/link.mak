#######################################################################
###
###     this file is used to link objs to a lib or an application
###
###     Author:  Jixuepeng, CSP
###     History:
###     1. jixuepeng,2011/07/21  ���� 
#######################################################################
include ./versionconfig.mak
include ./command.mak
include ./compileoption.mak
include ./compilepara.mak

###_LINK_FLAG���ڽ�����໥����������
##_LINK_FLAG += -Xlinker -lxxx -lyyyy -rdynamic

#���� _VERSION_NAME, _VERSION_OBJS_ALL, _OTHER_LINK_LIB, _OTHER_LINK_PATH �ǵ����ߴ����

_VERSION_BIN_FULL_NAME = $(_TARGET_BIN_PATH)/$(_VERSION_NAME)$(VERSION_SUFFIX)

ifneq (DYNAMIC,$(_LIB_TYPE))
_VERSION_LIB_FULL_NAME = $(_TARGET_LIB_PATH)/$(_VERSION_NAME).a
else
_VERSION_LIB_FULL_NAME = $(_TARGET_LIB_PATH)/$(_VERSION_NAME).so
endif

MAK_BIN_VERSION_TEMP = $(_VERSION_BIN_FULL_NAME).obj
MAK_LIB_VERSION_TEMP = $(_VERSION_LIB_FULL_NAME).obj
MAK_BIN_VERSION_PATH = $(dir $(_VERSION_BIN_FULL_NAME))
MAK_LIB_VERSION_PATH = $(dir $(_VERSION_LIB_FULL_NAME))
LD_LINK_PATH = -L$(TARGET_LIBS_NAME)/lib -L$(_TARGET_LIB_PATH) $(_OTHER_LINK_PATH) $(_TECS_CONTRIB_LIB_PATH)
LD_LINK_LIB  = $(_OTHER_LINK_LIB) $(GLIBC_FOR_TECS)

PURE_OS_TYPE = $(subst _,,$(_OS_TYPE))


svr:
	@if [ ! -d $(MAK_BIN_VERSION_PATH) ]; then $(MKDIR) $(MAK_BIN_VERSION_PATH); fi
###����TCL�ű��Լ���Ƿ��������ķ��ţ�7�������ֱ�Ϊ���⼰Ŀ���·��  ϵͳ�� Ӧ�ó���Ŀ�� ���ɵĿ�ִ���ļ��� �������ű����ù���	��������
#	@tclsh symb_conflict.tcl "$(LD_LINK_LIB)" "$(LD_LINK_PATH)" "$(_VERSION_OBJS_ALL)" "$(_VERSION_BIN_FULL_NAME)" "$(NM)" "$(_TECS_TMP_PATH)" "$(_LINK_TYPE)"
#	$(RM) $(_VERSION_BIN_FULL_NAME).sym.txt
ifeq (_RELEASE,$(_VERSION_TYPE))
	$(LD) -r -S -o $(MAK_BIN_VERSION_TEMP) $(_VERSION_OBJS_ALL) 
else
	$(LD) -r -o $(MAK_BIN_VERSION_TEMP) $(_VERSION_OBJS_ALL) 
endif
	@echo _LINK_TYPE=$(_LINK_TYPE)
ifeq (STATIC,$(_LINK_TYPE))
	$(CC) -o $(_VERSION_BIN_FULL_NAME) $(MAK_BIN_VERSION_TEMP) $(LD_LINK_PATH) $(LD_LINK_LIB)  $(_LINK_FLAG) -static
else
#	@echo $(CC) $(CROSS_FLAGS) -o $(_VERSION_BIN_FULL_NAME) $(MAK_BIN_VERSION_TEMP) $(LD_LINK_PATH) $(LD_LINK_LIB)  $(_LINK_FLAG)
	$(CC) $(CROSS_FLAGS) -o $(_VERSION_BIN_FULL_NAME) $(MAK_BIN_VERSION_TEMP) $(LD_LINK_PATH) $(LD_LINK_LIB)  $(_LINK_FLAG)
endif
	$(RM) $(MAK_BIN_VERSION_TEMP)
	
lib:mkdir_version_lib
	@if [ ! -d $(MAK_LIB_VERSION_PATH) ]; then $(MKDIR) $(MAK_LIB_VERSION_PATH); fi
	@if [ -f $(_VERSION_LIB_FULL_NAME) ]; then $(RM) $(_VERSION_LIB_FULL_NAME); fi
	@echo _LIB_TYPE=$(_LIB_TYPE)
ifeq (DYNAMIC,$(_LIB_TYPE))
	$(CC) $(CROSS_FLAGS) -shared -o $(_VERSION_LIB_FULL_NAME) $(_VERSION_OBJS_ALL) $(LD_LINK_PATH) $(LD_LINK_LIB)
else
ifeq (_RELEASE,$(_VERSION_TYPE))
	$(AR) -r $(_VERSION_LIB_FULL_NAME) $(_VERSION_OBJS_ALL)
else
	$(AR) -r $(_VERSION_LIB_FULL_NAME) $(_VERSION_OBJS_ALL)
endif
endif
ifeq (_LINUX_PC,$(_OS_TYPE))
	$(CP) $(_VERSION_LIB_FULL_NAME) /version/lib/
endif

#LINUX_PC���Խ��̲��ö�̬���Ӻ�/version/lib ���ڴ��TECS�����ɵĶ�̬��
mkdir_version_lib:
	@if [ ! -d /version/lib ]; then $(MKDIR) /version/lib ; fi


