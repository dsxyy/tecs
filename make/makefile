include versionconfig.mak
include command.mak
include cpuconfig.mak
include compilepara.mak

ifeq (TRUE,$(CONCURRENT_COMPILE))
    CPU_NUM = $(shell cat /proc/cpuinfo | grep 'processor' | wc -l)
    LOAD_AVERAGE = $(shell expr $(CPU_NUM) + $(CPU_NUM))
    MAKE += -j -l $(LOAD_AVERAGE)
endif

ifeq (,$(_CPU_TYPE))
export _CPU_TYPE     = _CPU_X86_64
endif

ifeq (,$(_CI_VER_NO))
export _CI_VER_NO     = without_input_no
endif

ifeq (,$(_CI_VER_NUM))
export _CI_VER_NUM     = without_input_num
endif

ifeq (,$(_CI_VER_NAME))
export _CI_VER_NAME     = without_input_name
endif

ifeq (,$(_VERSION_TYPE))
export _VERSION_TYPE = _DEBUG
endif

ifeq (,$(_CI_SVN_REVISION))
export _CI_SVN_REVISION     = without_svn_revision
endif

export RPM_ERR=$(_TECS_TARGET_PATH)/rpmErr_$(OS)_$(subst _CPU_,,$(_CPU_TYPE))$(_VERSION_TYPE).txt
##rpmbuild过程信息会被当错错误信息输出，为方便以后检查编译告警和错误，默认单独出错到一个文件中。
##如果想让rpmbuild的错误同当前错误输出到一起，在编译时定义RPM_ERR_OUTPUT为空。
##例如 make allrpm _OS_TYPE=_CGSL _CPU_TYPE=_CPU_X86_64 _VERSION_TYPE=_RELEASE RPM_ERR_OUTPUT=
##由于rpmbuild出错会停止编译，因此这里只保存了最后一次错误信息。
export RPM_ERR_OUTPUT= 2>$(RPM_ERR)

include compileoption.mak

.PHONY:help sd hc cc tc apisvr sky cleanall all

help:
	$(ECHO) ===============================================================================
	$(ECHO) make usage:
	$(ECHO) ===============================================================================
	$(ECHO) make hc  ... generate hc application
	$(ECHO) example: make hc _CPU_TYPE=_CPU_X86 _OS_TYPE=_LINUX_PC _VERSION_TYPE=_DEBUG
	$(ECHO) example: make hc _CPU_TYPE=_CPU_X86_64 _OS_TYPE=_CGSL _VERSION_TYPE=_RELEASE
	$(ECHO) make hc using local compiler 
	$(ECHO) example: make hc _CPU_TYPE=_CPU_X86_64 LOCAL_BUILD=TRUE _VERSION_TYPE=_DEBUG 
	$(ECHO) make hc using static link... ##default dynamic link
	$(ECHO) example: make hc _CPU_TYPE=_CPU_X86_64 _LINK_TYPE=STATIC _VERSION_TYPE=_DEBUG 
	$(ECHO) ===============================================================================
	$(ECHO) make cc  ... generate cc application
	$(ECHO) example: make cc _CPU_TYPE=_CPU_X86_64 _OS_TYPE=_el5 _VERSION_TYPE=_DEBUG
	$(ECHO) ===============================================================================
	$(ECHO) make tc  ... generate tc application
	$(ECHO) example: make tc _CPU_TYPE=_CPU_X86_64 _OS_TYPE=_CGSL _VERSION_TYPE=_DEBUG
	$(ECHO) ===============================================================================
	$(ECHO) make sky  ... generate sky lib
	$(ECHO) example: make sky _CPU_TYPE=_CPU_X86_64 _OS_TYPE=_CGSL _VERSION_TYPE=_DEBUG
	$(ECHO) ===============================================================================
	$(ECHO) make rpm  ... generate rpm package
	$(ECHO) make a single rpm package, err is output to target/rpmErr_*.txt alone.
	$(ECHO) example: make rpm _RPM=cc _CPU_TYPE=_CPU_X86_64 _OS_TYPE=_CGSL _VERSION_TYPE=_RELEASE 
	$(ECHO) make all rpm package, err is output to the same file with make.
	$(ECHO) example: make allrpm _CPU_TYPE=_CPU_X86_64 _OS_TYPE=_CGSL _VERSION_TYPE=_RELEASE RPM_ERR_OUTPUT=
	$(ECHO) ===============================================================================		
	$(ECHO) Tips:
	$(ECHO) specified _OS_TYPE _CGSL,_LINUX,_LINUX_PC,_ZXVE and _el5 correspond to _el5 
	$(ECHO) specified _OS_TYPE _el6 correspond to _el6 
	$(ECHO) default _OS_TYPE depend on the compile environment OS type 	
	$(ECHO) specified _OS_TYPE _ZXVE will use ZXVE cross compile tools,others use local compile tools 
	$(ECHO) ===============================================================================		
	$(ECHO) MAKE=$(MAKE)
	$(ECHO) TMM=$(TMM)
    
MakeForce:
	@if [ ! -d $(_TARGET_LIB_PATH) ]; then $(MKDIR) $(_TARGET_LIB_PATH) ; fi 
	@chmod +x $(_TECS_MAK_PATH)/precompile.sh 
	@$(_TECS_MAK_PATH)/precompile.sh  $(CONTRIB_QPID_PATH) $(CONTRIB_PGSQL_PATH) $(CONTRIB_XMLRPC_PATH) $(_TECS_DBSCHEMA_PATH)/updates/tc $(_TECS_DBSCHEMA_PATH)/updates/cc $(_TECS_DBSCHEMA_PATH)/views $(CONTRIB_RESTRPC_PATH)
	@chmod +x $(_TECS_MAK_PATH)/dbschema.sh
	@$(_TECS_MAK_PATH)/dbschema.sh -f $(_TECS_ROOT_PATH)/code/tc/common/include/dbschema_ver.h -d $(_TECS_DBSCHEMA_PATH)/updates/tc
	@$(_TECS_MAK_PATH)/dbschema.sh -f $(_TECS_ROOT_PATH)/code/cc/common/include/dbschema_ver.h -d $(_TECS_DBSCHEMA_PATH)/updates/cc
	@$(_TECS_MAK_PATH)/dbschema.sh -f $(_TECS_ROOT_PATH)/code/network/vnm/db/include/dbschema_ver.h -d $(_TECS_DBSCHEMA_PATH)/updates/vnm

common:
	$(MAKE) -C $(_TECS_CODE_PATH)/common
commonclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/common clean 
commoncleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/common cleanall

openais:
	$(MAKE) -C $(_TECS_CONTRIB_PATH)/openais all MAKE=make ECHO=echo
#	$(MAKE) -C $(_TECS_CONTRIB_PATH)/openais MAKE=make ECHO=echo CC=$(CC) AR=$(AR) 

openaisclean:
	$(MAKE) -C $(_TECS_CONTRIB_PATH)/openais clean MAKE=make ECHO=echo CC=$(CC) AR=$(AR) 
    
tc:sky common openais authlib  storagelib net
	$(MAKE) -C $(_TECS_CODE_PATH)/tc _SVR_TYPE=_SVR_TC
tcclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc clean 
tccleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc cleanall
    
# BEGION OF  TECS API SERVICE PROCESS
apisvr:sky common net
	$(MAKE) -C $(_TECS_CODE_PATH)/tc _SVR_TYPE=_SVR_APISVR
apisvrclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc clean _SVR_TYPE=_SVR_APISVR
apisvrcleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc cleanall _SVR_TYPE=_SVR_APISVR
# END OF  TECS API SERVICE PROCESS 
    
cc:sky common openais storagelib net
	$(MAKE) -C $(_TECS_CODE_PATH)/cc _SVR_TYPE=_SVR_CC
ccclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/cc clean
cccleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/cc cleanall
	
hc:sky common  smartlib storagelib hyperver_simulate net

	$(MAKE) -C $(_TECS_CODE_PATH)/hc  -f makefile _SVR_TYPE=_SVR_HC
hcclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/hc clean
hccleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/hc -f makefile cleanall _SVR_TYPE=_SVR_HC

networkcommon:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/common
netwrokcommonclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/common clean 
networkcommoncleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/common cleanall

vnm: sky common openais
	$(MAKE) -C $(_TECS_CODE_PATH)/network  -f makefile _SVR_TYPE=_SVR_NETWORK_VNM
vnmclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network  clean
vnmcleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network -f makefile cleanall _SVR_TYPE=_SVR_NETWORK_VNM

vna: sky common openais br ovs
	$(MAKE) -C $(_TECS_CODE_PATH)/network  -f makefile _SVR_TYPE=_SVR_NETWORK_VNA
vnaclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network  clean
vnacleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network -f makefile cleanall _SVR_TYPE=_SVR_NETWORK_VNA
	
wdg: sky common openais br ovs net
	$(MAKE) -C $(_TECS_CODE_PATH)/network  -f makefile _SVR_TYPE=_SVR_NETWORK_WDG
wdgclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network  clean
wdgcleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network -f makefile cleanall _SVR_TYPE=_SVR_NETWORK_WDG

br: sky common 
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin  -f makefile _LIB_VER=_LIB_NETWORK_BR _LIB_TYPE=DYNAMIC
brclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin  clean
brcleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin -f makefile cleanall _LIB_VER=_LIB_NETWORK_BR
	
ovs: sky common 
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin  -f makefile _LIB_VER=_LIB_NETWORK_OVS _LIB_TYPE=DYNAMIC
ovsclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin  clean
ovscleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin -f makefile cleanall _LIB_VER=_LIB_NETWORK_OVS
	
net: sky common 
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin  -f makefile _LIB_VER=_LIB_NETWORK_NET _LIB_TYPE=DYNAMIC
netclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin  clean
netcleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/network/plugin -f makefile cleanall _LIB_VER=_LIB_NETWORK_NET

sc:sky common openais 
	$(MAKE) -C $(_TECS_CODE_PATH)/storage _SVR_TYPE=_SVR_SC
scclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/storage clean
scleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/storage cleanall


sa:sky common openais  storagelib
	$(MAKE) -C $(_TECS_CODE_PATH)/storage  _SVR_TYPE=_SVR_SA  
saclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/storage clean
sacleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/storage cleanall
		    
hyperver_simulate:
	$(MAKE) -C $(_TECS_CODE_PATH)/hc -f makefile _SVR_TYPE=_SVR_HYS
hyperver_simulateclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/hc clean
    
sky:MakeForce
	$(MAKE) -C $(_TECS_CODE_PATH)/sky sky _LIB_VER=sky _LIB_TYPE=DYNAMIC TEMP=$(_TEMP_PATH)/sky TARGET=$(_TARGET_LIB_PATH) FROM_TECS=TRUE  
#	@if [ -f $(_TARGET_LIB_PATH)/libsky.so ]; then cp -f $(_TARGET_LIB_PATH)/libsky.so $(TECS_BASE_PATH)/lib; fi
#	@if [ -f $(_TARGET_LIB_PATH)/libtecsdb.so ]; then cp -f $(_TARGET_LIB_PATH)/libtecsdb.so $(TECS_BASE_PATH)/lib; fi
skyclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/sky clean TEMP=$(_TEMP_PATH)/sky TARGET=$(_TARGET_LIB_PATH)/sky
	$(ECHO) clean lib tmp files

doc:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc/request_manager/doc/ -f makefile _VERSION=V$(_TECS_VERNO).$(_VER_REL)
docclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc/request_manager/doc/ -f makefile clean
	$(ECHO) clean doc tmp files
	
image_manager:sky common openais authlib
	$(MAKE) -C $(_TECS_CODE_PATH)/tc _SVR_TYPE=_SVR_IMAGE_MANAGER 
image_managerclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc clean _SVR_TYPE=_SVR_IMAGE_MANAGER
  
file_manager:sky common openais authlib
	$(MAKE) -C $(_TECS_CODE_PATH)/tc _SVR_TYPE=_SVR_FILE_MANAGER 
file_managerclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc clean _SVR_TYPE=_SVR_FILE_MANAGER
  
image_agent:sky common openais
	$(MAKE) -C $(_TECS_CODE_PATH)/cc _SVR_TYPE=_SVR_IMAGE_AGENT
image_agentclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/cc clean _SVR_TYPE=_SVR_IMAGE_AGENT

storagelib:
#	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage -f makefile  _LIB_VER=_LIB_STORAGE
	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage -f makefile   _LIB_VER=_LIB_STORAGE _LIB_TYPE=DYNAMIC 
storagelibclean:
#	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage -f makefile clean _LIB_VER=_LIB_STORAGE
	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage -f makefile clean _LIB_VER=_LIB_STORAGE _LIB_TYPE=DYNAMIC
	$(ECHO) clean lib tmp files
storagelibcleanall:
#	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage -f makefile cleanall _LIB_VER=_LIB_STORAGE
	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage -f makefile cleanall _LIB_VER=_LIB_STORAGE _LIB_TYPE=DYNAMIC
	$(ECHO) clean lib tmp files
	
smartlib:
#	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage/smart -f makefile _LIB_VER=_LIB_SMART
	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage/smart -f makefile _LIB_VER=_LIB_SMART _LIB_TYPE=DYNAMIC
smartlibclean:
#	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage/smart -f makefile clean _LIB_VER=_LIB_SMART
	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage/smart -f makefile clean _LIB_VER=_LIB_SMART _LIB_TYPE=DYNAMIC
	$(ECHO) clean lib tmp files
smartlibcleanall:
#	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage/smart -f makefile cleanall _LIB_VER=_LIB_SMART
	$(MAKE) -C $(_TECS_CODE_PATH)/common/storage/smart -f makefile cleanall _LIB_VER=_LIB_SMART _LIB_TYPE=DYNAMIC
	$(ECHO) clean lib tmp files
	
authlib:
#	$(MAKE) -C $(_TECS_CODE_PATH)/tc/auth_manager -f makefile _LIB_VER=_LIB_AUTH
	$(MAKE) -C $(_TECS_CODE_PATH)/tc/auth_manager -f makefile _LIB_VER=_LIB_AUTH _LIB_TYPE=DYNAMIC	
authlibclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/tc/auth_manager -f makefile clean _LIB_VER=_LIB_AUTH
	$(MAKE) -C $(_TECS_CODE_PATH)/tc/auth_manager -f makefile clean _LIB_VER=_LIB_AUTH _LIB_TYPE=DYNAMIC		
	
version:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_VERSION
versionclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile clean	

versionagent:
#	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_VERSION_AGENT
versionagentclean:
#	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile clean	
	
guard:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_GUARD
guardclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile clean	

tc_st:sky
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_TC_SSH_TRUST
tc_stclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile clean	

cc_st:sky
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_CC_SSH_TRUST
cc_stclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile clean	

st:sky
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_TC_SSH_TRUST
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile _SVR_TYPE=_SVR_CC_SSH_TRUST
stclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/utils -f makefile clean	

cli:MakeForce
	$(MAKE) -C $(_TECS_CLI_PATH) all FROM_TECS=TRUE
cliclean:
	$(MAKE) -C $(_TECS_CLI_PATH) clean FROM_TECS=TRUE
    
chrpath:
	$(MAKE) -C $(_TECS_TOOLS_PATH)/chrpath -f makefile
chrpathclean:
	$(MAKE) -C $(_TECS_TOOLS_PATH)/chrpath -f makefile clean

igmpv3:
	$(MAKE) -C $(_TECS_TOOLS_PATH)/igmpv3 -f makefile
igmpv3clean:
	$(MAKE) -C $(_TECS_TOOLS_PATH)/igmpv3 -f makefile clean

sd:sky common
	$(MAKE) -C $(_TECS_CODE_PATH)/special_device  -f makefile _SVR_TYPE=_SVR_SPECIAL_DEVICE
sdclean:
	$(MAKE) -C $(_TECS_CODE_PATH)/special_device clean
sdcleanall:
	$(MAKE) -C $(_TECS_CODE_PATH)/special_device -f makefile cleanall _SVR_TYPE=_SVR_SPECIAL_DEVICE

tcall:tc version apisvr image_manager file_manager tc_st sc

ccall:cc image_agent cc_st

hcall:hc 

saall:sa

vnaall:vna

wdgall:wdg

vnmall:vnm 


all:tcall ccall hcall sd guard igmpv3  sc sa  vnmall vnaall wdgall

export _RPM=all
rpm: chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ $(_RPM) 

allrpm:all rpm
	$(MAKE) -C $(_TECS_TOOLS_PATH)/setup bin
	@echo TECS compile and package success!

commonrpm:
	$(MAKE) -C $(_TECS_RPM_PATH)/ common
	
vnarpm: vna chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ vna  
	
wdgrpm: wdg chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ wdg
	
vnmrpm: vnm chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ vnm  
	
tcrpm:tcall chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ tc

ccrpm:ccall chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ cc

hcrpm:hcall chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ hc
	
sarpm:saall chrpath
	$(MAKE) -C $(_TECS_RPM_PATH)/ sa
	
rpmclean:chrpathclean
	$(MAKE) -C $(_TECS_RPM_PATH)/ clean
	
wuirpm:
	$(MAKE) -C $(_TECS_RPM_PATH)/ wui
	
sdrpm:
	$(MAKE) -C $(_TECS_RPM_PATH)/ sd
	
bin:
	$(MAKE) -C $(_TECS_TOOLS_PATH)/setup bin

cleanall:rpmclean cliclean openaisclean
	$(RMDIR)  $(_TECS_TMP_PATH)
	$(RMDIR)  $(_TECS_TARGET_PATH)
	@cd $(CONTRIB_QPID_PATH); rm -rf `ls ./ | grep -v qpid.tar.gz`; cd -
	@cd $(CONTRIB_PGSQL_PATH); rm -rf `ls ./ | grep -v postgresql.tar.gz`; cd -
	@cd $(CONTRIB_XMLRPC_PATH); rm -rf `ls ./ | grep -v xmlrpc.tar.gz`; cd -
	@cd $(CONTRIB_RESTRPC_PATH); rm -rf `ls ./ | grep -v restrpc-c.tar.gz`; cd -
	@echo "clean all successfully!"
    

