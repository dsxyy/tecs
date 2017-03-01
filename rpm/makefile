#export INSTALL_PATH =/opt/tecs
#PURE_OS_TYPE = $(shell echo $(subst _,,$(_OS_TYPE))|tr A-Z a-z)
PURE_OS_TYPE = el5
#RPM_OS_TYPE = $(PURE_OS_TYPE)
ifeq (_el6,$(_OS_TYPE))
PURE_OS_TYPE = el6
endif
RPM_OS_TYPE=linux

VER1 =  $(shell echo $(_TECS_VERNO)|awk -F'.' '{printf $$1}'|sed -e 's/^0//g')
VER2 =  $(shell echo $(_TECS_VERNO)|awk -F'.' '{printf $$2}'|sed -e 's/^0//g')
VER3 =  $(shell echo $(_TECS_VERNO)|awk -F'.' '{printf $$3}')
_TECS_RPM_VERNO=$(VER1).$(VER2).$(VER3)

BIN_TYPE = $(PURE_OS_TYPE)_$(ARCH)

# 如果文件存在，则表示hypervisor是kvm，需要使用hc.kvm.spec制作hc.rpm
HC_SPEC_NAME = hc.xen.spec
ifeq (/dev/kvm, $(wildcard /dev/kvm))
HC_SPEC_NAME = hc.kvm.spec
else 
 ifeq (/proc/xen/xenbus, $(wildcard /proc/xen/xenbus))
HC_SPEC_NAME = hc.xen.spec
 endif
endif

export RPMBUILD = rpmbuild -bb  --define '_topdir $(_TECS_RPM_PATH)' \
                                --define '_unpackaged_files_terminate_build 0'  \
                                --define '_arch $(ARCH)' \
                                --define '_os $(RPM_OS_TYPE)' \
                                --define '_rpmdir %{_topdir}/RPMS/$(PURE_OS_TYPE)' \
                                --define '_build_name_fmt %%{ARCH}/%%{NAME}-%%{VERSION}-%%{RELEASE}.$(PURE_OS_TYPE).%%{ARCH}.rpm' \
                                --define '_arch $(ARCH)' \
                                --define 'debug_package %{nil}' \
                                --define '__strip /bin/true' \
                                --define '__os_install_post     /usr/lib/rpm/redhat/brp-compress \
                                                                %{!?__debug_package:/usr/lib/rpm/redhat/brp-strip %{__strip}} \
                                                                /usr/lib/rpm/redhat/brp-strip-static-archive %{__strip} \
                                                                /usr/lib/rpm/redhat/brp-strip-comment-note %{__strip} %{__objdump} \
                                                                /usr/lib/rpm/redhat/brp-python-hardlink \
                                                                %{!?__jar_repack:/usr/lib/rpm/redhat/brp-java-repack-jars} \
                                                                 %{nil}' \
                                --target=$(ARCH)-zte-$(RPM_OS_TYPE)

# --buildarch=$(ARCH) --buildos=$(shell echo $PURE_OS_TYPE|tr A-Z a-z)

export TECS_RPMBUILD = $(RPMBUILD) --define '_prefix $(INSTALL_PATH)' --define '_verno $(_TECS_RPM_VERNO)' --define '_ver_suffix $(VERSION_SUFFIX)'\
                       --define 'release $(_VER_REL)' --define '_description $(_CI_VER_NAME)_$(_CI_VER_NUM)_$(_CI_VER_NO)'  --define '_svn_revision $(_CI_SVN_REVISION)'\
#export TECS_RPMBUILD = $(RPMBUILD) --define '_verno $(_TECS_VERNO)' --define '_ver_suffix $(VERSION_SUFFIX)'

_TECS_NETWORK_SCRIPTS_PATH = $(_TECS_RPM_PATH)/../code/network/scripts
_TECS_NETWORK_RESTRPC_PATH = $(_TECS_RPM_PATH)/../contrib/target/$(ARCH)/$(PURE_OS_TYPE)/restrpc-c/restrpc-c
_TECS_NETWORK_DB_MAIN_SETUP_FILE = $(_TECS_RPM_PATH)/../code/network/vnm/db/sql/update_vnm_pgdb.sh
_TECS_NETWORK_DB_PROCEDURE_SCRIPTS_PATH = $(_TECS_RPM_PATH)/../code/network/vnm/db/sql/procedure
_TECS_NETWORK_DB_TABLE_SCRIPTS_PATH = $(_TECS_RPM_PATH)/../code/network/vnm/db/sql/table
_TECS_NETWORK_DB_VIEW_SCRIPTS_PATH = $(_TECS_RPM_PATH)/../code/network/vnm/db/sql/view
_TECS_COMMON_SCRIPTS_PATH = $(_TECS_RPM_PATH)/../code/common/scripts

rpmforce:
	@if [ ! -d $(_TECS_RPM_PATH)/RPMS/$(PURE_OS_TYPE) ]; then $(MKDIR) $(_TECS_RPM_PATH)/RPMS/$(PURE_OS_TYPE) ; fi

all:clean rpms

rpms:qpid common tc cc hc guard postgresql openais wui sd sa vna vnm wdg nicchk

clean:rpmforce
	$(RM) $(_TECS_RPM_PATH)/SOURCES/*
	$(RM) $(_TECS_RPM_PATH)/BUILD/*
	$(RM) $(_TECS_RPM_PATH)/RPMS/*
	$(RM) $(_TECS_RPM_PATH)/SRPMS/*

qpid:rpmforce
	$(CP) $(_TECS_CONTRIB_PATH)/qpid/qpid-cpp-0.14.tar.gz $(_TECS_RPM_PATH)/SOURCES
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/qpid_temp/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/qpid_temp/etc/logrotate.d/
	@tar -xzvf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/qpid/qpid.tar.gz -C $(_TECS_RPM_PATH)/BUILD/qpid_temp
	$(CP) -R $(_TECS_TOOLS_PATH)/services/qpid $(_TECS_RPM_PATH)/BUILD/qpid_temp/etc/init.d
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/qpid      $(_TECS_RPM_PATH)/BUILD/qpid_temp/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/qpid_build
	$(RPMBUILD)  --buildroot $(_TECS_RPM_PATH)/BUILD/qpid_build --define '_qpid_temp $(_TECS_RPM_PATH)/BUILD/qpid_temp'  $(_TECS_RPM_PATH)/SPECS/qpid.spec
	$(RM) $(_TECS_RPM_PATH)/BUILD/qpid_*

vna:rpmforce
	@echo making vna rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/network/vna
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/scripts
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/etc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna/etc/logrotate.d/
	touch    $(_TECS_RPM_PATH)/BUILD/network/vna/etc/tecs/guard_list/tecs-vna
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/network/NETWORK_VNA*   $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/bin
	#$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/*               $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/bin
	$(CP)    $(_TARGET_LIB_PATH)/*.so                        $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/lib
	$(CP) -R $(_TECS_NETWORK_SCRIPTS_PATH)/*                 $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/scripts
	$(CP)    $(_TECS_CONFIG_PATH)/tecs.conf                  $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/etc
	$(CP)    $(_TECS_TOOLS_PATH)/services/tecs-vna           $(_TECS_RPM_PATH)/BUILD/network/vna/etc/init.d/
	$(CP)    $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh               $(_TECS_RPM_PATH)/BUILD/network/vna/$(INSTALL_PATH)/network/vna/scripts	
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/network/vna/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vna_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/network/vna_build --define '_vna_temp $(_TECS_RPM_PATH)/BUILD/network/vna' $(_TECS_RPM_PATH)/SPECS/vna.spec $(RPM_ERR_OUTPUT)

vnm:rpmforce
	@echo making vnm rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/network/vnm
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/etc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/scripts
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/etc/updates	
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm/etc/logrotate.d/
	touch    $(_TECS_RPM_PATH)/BUILD/network/vnm/etc/tecs/guard_list/tecs-vnm
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/network/NETWORK_VNM*   $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/bin
	#$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/*               $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/bin
	$(CP)    $(_TECS_NETWORK_DB_MAIN_SETUP_FILE)             $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/scripts
	$(CP)    $(_TARGET_LIB_PATH)/*.so                        $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(CP)    $(_TECS_NETWORK_RESTRPC_PATH)/src/*.so.3.25          $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(CP)    $(_TECS_NETWORK_RESTRPC_PATH)/src/cpp/*.so.7.25      $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(CP)    $(_TECS_NETWORK_RESTRPC_PATH)/lib/expat/xmlparse/*.so.3.25    $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(CP)    $(_TECS_NETWORK_RESTRPC_PATH)/lib/expat/xmltok/*.so.3.25      $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(CP)    $(_TECS_NETWORK_RESTRPC_PATH)/lib/libutil/*.so.3.25  $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/lib
	$(CP)    $(_TECS_CONFIG_PATH)/tecs.conf                  $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/baselines/vnm_baseline.sql $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/views/vnm_view.sql         $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/updates/vnm/*.sql          $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/etc/updates
	$(CP)    $(_TECS_TOOLS_PATH)/services/tecs-vnm           $(_TECS_RPM_PATH)/BUILD/network/vnm/etc/init.d/
	$(CP)    $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh               $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/scripts	
	$(CP)  $(_TECS_TOOLS_PATH)/setup/base/install_pgdb.sh $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/setup/base/update_pgdb.sh $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/scripts
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/network/vnm/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/vnm_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/network/vnm_build --define '_vnm_temp $(_TECS_RPM_PATH)/BUILD/network/vnm' $(_TECS_RPM_PATH)/SPECS/vnm.spec $(RPM_ERR_OUTPUT)

wdg:rpmforce
	@echo making wdg rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/network/wdg
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/scripts/procedure
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/scripts/table
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/scripts/view
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/etc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg/etc/logrotate.d/
	touch    $(_TECS_RPM_PATH)/BUILD/network/wdg/etc/tecs/guard_list/tecs-wdg
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/network/NETWORK_WDG*   $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/bin
	#$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/*               $(_TECS_RPM_PATH)/BUILD/network/vnm/$(INSTALL_PATH)/network/vnm/bin
	$(CP)    $(_TARGET_LIB_PATH)/*.so                        $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/lib
	$(CP)    $(_TECS_CONFIG_PATH)/tecs.conf                  $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/etc
	$(CP)    $(_TECS_TOOLS_PATH)/services/tecs-wdg           $(_TECS_RPM_PATH)/BUILD/network/wdg/etc/init.d/	
	$(CP)    $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh           $(_TECS_RPM_PATH)/BUILD/network/wdg/$(INSTALL_PATH)/network/wdg/scripts	
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/network/wdg/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/network/wdg_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/network/wdg_build --define '_wdg_temp $(_TECS_RPM_PATH)/BUILD/network/wdg' $(_TECS_RPM_PATH)/SPECS/wdg.spec $(RPM_ERR_OUTPUT)

	
openais:rpmforce
	$(CP) $(_TECS_CONTRIB_PATH)/openais/openais-0.80.6.tar.gz $(_TECS_RPM_PATH)/SOURCES
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/etc/init.d
	@tar -xzf $(_TECS_RPM_PATH)/SOURCES/openais-0.80.6.tar.gz -C $(_TECS_RPM_PATH)/BUILD/openais_temp
	$(CP) -R $(_TECS_TOOLS_PATH)/services/openais $(_TECS_RPM_PATH)/BUILD/openais_temp/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/usr;$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local/bin;$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/etc;$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/etc/ais
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local/include
	$(CP) $(_TECS_RPM_PATH)/BUILD/openais_temp/exec/* $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local/bin
	$(CP) $(_TECS_RPM_PATH)/BUILD/openais_temp/lib/*  $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local/lib
	$(CP) $(_TECS_RPM_PATH)/BUILD/openais_temp/conf/* $(_TECS_RPM_PATH)/BUILD/openais_temp/etc/ais
	$(CP) $(_TECS_RPM_PATH)/BUILD/openais_temp/include/* $(_TECS_RPM_PATH)/BUILD/openais_temp/usr/local/include
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/openais_build
	$(RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/openais_build  --define '_openais_temp $(_TECS_RPM_PATH)/BUILD/openais_temp' $(_TECS_RPM_PATH)/SPECS/openais.spec  $(RPM_ERR_OUTPUT)
	$(RM) $(_TECS_RPM_PATH)/BUILD/openais_*

tc:rpmforce
	@echo making tc rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/tc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/etc/updates
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/cli
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/etc/tecs
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/usr/local/abyss/conf
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/usr/local/abyss/wui
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/etc/logrotate.d/
	touch   $(_TECS_RPM_PATH)/BUILD/tc/etc/tecs/guard_list/tecs-tc
	$(CP) $(_TECS_CONFIG_PATH)/abyss.conf $(_TECS_RPM_PATH)/BUILD/tc/usr/local/abyss/conf
	$(CP) $(_TECS_CONFIG_PATH)/mime.types $(_TECS_RPM_PATH)/BUILD/tc/usr/local/abyss/conf
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/tc/* $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/bin
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/VERSION* $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/bin
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/storage/SC* $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/bin
	$(CP) $(_TARGET_LIB_PATH)/*.so $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/lib
#	@tar -C $(_TECS_RPM_PATH)/BUILD/tc -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/qpid/qpid.tar.gz ./usr/local/lib/libqpid*so*
	@tar -C $(_TECS_RPM_PATH)/BUILD/tc -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/xmlrpc/xmlrpc.tar.gz ./xmlrpc/lib/libxmlrpc*so*; mv $(_TECS_RPM_PATH)/BUILD/tc/xmlrpc/lib/* $(_TECS_RPM_PATH)/BUILD/tc/usr/local/lib/;rm -rf $(_TECS_RPM_PATH)/BUILD/tc/xmlrpc
	@tar -C $(_TECS_RPM_PATH)/BUILD/tc -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/postgresql/postgresql.tar.gz usr/local/pgsql/lib/libpq*so*; mv $(_TECS_RPM_PATH)/BUILD/tc/usr/local/pgsql/lib/* $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/lib/;rm -rf $(_TECS_RPM_PATH)/BUILD/tc/usr/local/pgsql
	$(CP)  $(_TECS_TOOLS_PATH)/services/tecs-tc $(_TECS_RPM_PATH)/BUILD/tc/etc/init.d/
	$(CP)  $(_TECS_TOOLS_PATH)/setup/base/install_pgdb.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/cloudcmd.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/setup/base/update_pgdb.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/upgradetecs.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/tecs_service.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/license/license.ini $(_TECS_RPM_PATH)/BUILD/tc/etc/tecs
	$(CP)  $(_TECS_TOOLS_PATH)/license/license.sig $(_TECS_RPM_PATH)/BUILD/tc/etc/tecs
	$(CP)  $(_TECS_TOOLS_PATH)/license/fingerprint.exe $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/bin
	$(CP)  $(_TECS_TOOLS_PATH)/cordump_file_zip.sh $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/scripts
	$(CP) -a $(_TECS_TOOLS_PATH)/new_ci/* $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/cli
	find   $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/cli -name "*.pyc" | xargs -n1 rm -f
	find   $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/cli -name "*.pyo" | xargs -n1 rm -f
	$(CP)  $(_TECS_CONFIG_PATH)/tecs.conf $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/snmp.xml $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/baselines/tc_baseline.sql $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/views/tc_view.sql $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/updates/tc/*.sql $(_TECS_RPM_PATH)/BUILD/tc/$(INSTALL_PATH)/tc/etc/updates
	$(CP)  $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/tc/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/tc_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/tc_build --define '_tc_temp $(_TECS_RPM_PATH)/BUILD/tc' $(_TECS_RPM_PATH)/SPECS/tc.spec $(RPM_ERR_OUTPUT)

cc:rpmforce
	@echo making cc rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/cc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/etc/updates
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/scripts
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/etc/logrotate.d/
	touch   $(_TECS_RPM_PATH)/BUILD/cc/etc/tecs/guard_list/tecs-cc
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/cc/* $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/bin
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/VERSION* $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/bin
	$(CP)  $(_TARGET_LIB_PATH)/*.so $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/lib
#	@tar -C $(_TECS_RPM_PATH)/BUILD/cc -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/qpid/qpid.tar.gz ./usr/local/lib/libqpid*so*
	@tar -C $(_TECS_RPM_PATH)/BUILD/cc -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/postgresql/postgresql.tar.gz usr/local/pgsql/lib/libpq*so*; mv $(_TECS_RPM_PATH)/BUILD/cc/usr/local/pgsql/lib/* $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/lib/;rm -rf $(_TECS_RPM_PATH)/BUILD/cc/usr/local/pgsql
	$(CP)  $(_TECS_TOOLS_PATH)/services/tecs-cc $(_TECS_RPM_PATH)/BUILD/cc/etc/init.d/
	$(CP)  $(_TECS_TOOLS_PATH)/setup/base/install_pgdb.sh $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/setup/base/update_pgdb.sh $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/scripts
	$(CP)  $(_TECS_CONFIG_PATH)/tecs.conf $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/baselines/cc_baseline.sql $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/views/cc_view.sql $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sql/updates/cc/*.sql $(_TECS_RPM_PATH)/BUILD/cc/$(INSTALL_PATH)/cc/etc/updates
	$(CP)  $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/cc/etc/logrotate.d/
	$(CP)  $(_TECS_TOOLS_PATH)/logrotate.d/tecs_hc         $(_TECS_RPM_PATH)/BUILD/cc/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/cc_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/cc_build --define '_cc_temp $(_TECS_RPM_PATH)/BUILD/cc' $(_TECS_RPM_PATH)/SPECS/cc.spec $(RPM_ERR_OUTPUT)

hc:rpmforce
	@echo making hc rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/hc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/etc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/scripts
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/usr/local/bin;$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/etc/tecs/guard_list/
	touch   $(_TECS_RPM_PATH)/BUILD/hc/etc/tecs/guard_list/tecs-hc
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/hc/* $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/bin
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/VERSION* $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/bin
	$(CP) -R $(_TECS_CODE_PATH)/hc/host_agent/src/*.sh $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/scripts
	$(CP) -R $(_TECS_CODE_PATH)/hc/host_handler/src/*.sh $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/scripts
	$(CP)  $(_TARGET_LIB_PATH)/*.so $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/lib
#	@tar -C $(_TECS_RPM_PATH)/BUILD/hc -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/qpid/qpid.tar.gz ./usr/local/lib/libqpid*so*
	$(CP)  $(_TECS_CONFIG_PATH)/tecs.conf $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/etc
	$(CP)  $(_TECS_TOOLS_PATH)/services/tecs-hc $(_TECS_RPM_PATH)/BUILD/hc/etc/init.d/
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/hc_poweron_upgrade.sh $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/scripts
	$(CP) $(_TECS_TOOLS_PATH)/ubench $(_TECS_RPM_PATH)/BUILD/hc/$(INSTALL_PATH)/hc/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/hc_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/hc_build --define '_hc_temp $(_TECS_RPM_PATH)/BUILD/hc'  $(_TECS_RPM_PATH)/SPECS/$(HC_SPEC_NAME) $(RPM_ERR_OUTPUT)

sa:rpmforce
	@echo making sa rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/sa
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/etc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/scripts
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/usr/local/bin;
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/usr/local/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/etc/logrotate.d/
	touch   $(_TECS_RPM_PATH)/BUILD/sa/etc/tecs/guard_list/tecs-sa
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/storage/SA* $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/bin
#	$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/* $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/bin	
	$(CP)  $(_TARGET_LIB_PATH)/libsky.so $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/lib
	$(CP)  $(_TARGET_LIB_PATH)/libstorage.so $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/lib
	$(CP)  $(_TECS_TOOLS_PATH)/usplib/lib/libUspCapi.so $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/lib
#	@tar -C $(_TECS_RPM_PATH)/BUILD/sa -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/qpid/qpid.tar.gz ./usr/local/lib/libqpid*so*
#	@tar -C $(_TECS_RPM_PATH)/BUILD/sa -zxf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/xmlrpc/xmlrpc.tar.gz ./xmlrpc/lib/libxmlrpc*so*; mv $(_TECS_RPM_PATH)/BUILD/sa/xmlrpc/lib/* $(_TECS_RPM_PATH)/BUILD/sa/usr/local/lib/;rm -rf $(_TECS_RPM_PATH)/BUILD/sa/xmlrpc
	$(CP)  $(_TECS_CONFIG_PATH)/tecs.conf $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/etc
	$(CP)  $(_TECS_CONFIG_PATH)/sa.xml.example $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/etc
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/update_cfg.sh $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/scripts
	$(CP)  $(_TECS_TOOLS_PATH)/services/tecs-sa $(_TECS_RPM_PATH)/BUILD/sa/etc/init.d/		
	$(CP)  $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/sa/etc/logrotate.d/
#	$(CP) -R $(_TECS_RPM_PATH)/../tools/network-bridge-ovs  $(_TECS_RPM_PATH)/BUILD/sa/$(INSTALL_PATH)/sa/bin	
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa/usr/local/bin	
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sa_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/sa_build --define '_sa_temp $(_TECS_RPM_PATH)/BUILD/sa'  $(_TECS_RPM_PATH)/SPECS/sa.spec $(RPM_ERR_OUTPUT)
	
wui:rpmforce
	@echo making wui rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/wui
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/wui/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/wui/opt/tecs/wui/htdocs
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/wui/etc/tecs/guard_list/
	touch   $(_TECS_RPM_PATH)/BUILD/wui/etc/tecs/guard_list/tecs-wui
	$(CP)  $(_TECS_TOOLS_PATH)/services/tecs-wui $(_TECS_RPM_PATH)/BUILD/wui/etc/init.d/
	$(CP)  $(_TECS_TOOLS_PATH)/vmm/update_wui.sh $(_TECS_RPM_PATH)/BUILD/wui/opt/tecs/wui/
	$(CP) -R $(_TECS_CLIENT_PATH)/wui/* $(_TECS_RPM_PATH)/BUILD/wui/opt/tecs/wui/htdocs
	@tar -C $(_TECS_RPM_PATH)/BUILD/wui/opt/tecs/wui/ -zxf $(_TECS_CONTRIB_PATH)/apache/apache.tar.gz;

	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/wui_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/wui_build --define '_wui_temp $(_TECS_RPM_PATH)/BUILD/wui/'   $(_TECS_RPM_PATH)/SPECS/wui.spec $(RPM_ERR_OUTPUT)

common:rpmforce
	@echo making common rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/common
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/logintecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/killtecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/toptecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/configtecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/starttecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/stoptecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_TOOLS_PATH)/backupconfig-tecs.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin    
	$(CP)  $(_TECS_COMMON_SCRIPTS_PATH)/config_ntp_client.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_COMMON_SCRIPTS_PATH)/config_ntp_server.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_COMMON_SCRIPTS_PATH)/config_dns_server.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TECS_COMMON_SCRIPTS_PATH)/config_dns_client.sh $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(CP)  $(_TARGET_BIN_BASE_PATH)/tools/chrpath $(_TECS_RPM_PATH)/BUILD/common/usr/local/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/common_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/common_build --define '_common_temp $(_TECS_RPM_PATH)/BUILD/common'  $(_TECS_RPM_PATH)/SPECS/common.spec $(RPM_ERR_OUTPUT)

guard:rpmforce
	@echo making guard rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/guard
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/guard/$(INSTALL_PATH)/guard/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/guard/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/guard/etc/logrotate.d/
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/common/GUARD* $(_TECS_RPM_PATH)/BUILD/guard/$(INSTALL_PATH)/guard/
	$(CP) -R $(_TECS_TOOLS_PATH)/services/tecs-guard $(_TECS_RPM_PATH)/BUILD/guard/etc/init.d
	cd $(_TECS_RPM_PATH)/BUILD/guard/etc/init.d/;ln -sf tecs-guard guard; cd -
	$(CP) -R $(_TECS_TOOLS_PATH)/guard.sh $(_TECS_RPM_PATH)/BUILD/guard/$(INSTALL_PATH)/guard/
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/tecs_guard      $(_TECS_RPM_PATH)/BUILD/guard/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/guard_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/guard_build --define '_guard_temp $(_TECS_RPM_PATH)/BUILD/guard'   $(_TECS_RPM_PATH)/SPECS/guard.spec $(RPM_ERR_OUTPUT)

sd:rpmforce
	@echo making special_device rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/sd
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/lib
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/etc
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd/etc/logrotate.d/
	touch   $(_TECS_RPM_PATH)/BUILD/sd/etc/tecs/guard_list/tecs-sd
	$(CP) -R $(_TARGET_BIN_BASE_PATH)/special_device/* $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/bin
	$(CP)  $(_TARGET_LIB_PATH)/libsky.so $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/lib
	$(CP)  $(_TECS_CONFIG_PATH)/tecs.conf $(_TECS_RPM_PATH)/BUILD/sd/$(INSTALL_PATH)/sd/etc
	$(CP) -R $(_TECS_TOOLS_PATH)/services/tecs-sd $(_TECS_RPM_PATH)/BUILD/sd/etc/init.d
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/tecs_local      $(_TECS_RPM_PATH)/BUILD/sd/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/sd_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/sd_build --define '_sd_temp $(_TECS_RPM_PATH)/BUILD/sd'   $(_TECS_RPM_PATH)/SPECS/sd.spec $(RPM_ERR_OUTPUT)

postgresql:rpmforce
	$(CP) $(_TECS_CONTRIB_PATH)/postgresql/postgresql-9.0.2.tar $(_TECS_RPM_PATH)/SOURCES
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/postgresql_temp/usr/local/pgsql
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/postgresql_temp/etc/logrotate.d/
	$(CP)    $(_TECS_TOOLS_PATH)/logrotate.d/postgresql      $(_TECS_RPM_PATH)/BUILD/postgresql_temp/etc/logrotate.d/
	@tar -xzvf $(_TECS_CONTRIB_PATH)/target/$(ARCH)/$(PURE_OS_TYPE)/postgresql/postgresql.tar.gz -C $(_TECS_RPM_PATH)/BUILD/postgresql_temp
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/postgresql_build
	$(RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/postgresql_build --define '_postgresql_temp $(_TECS_RPM_PATH)/BUILD/postgresql_temp'  $(_TECS_RPM_PATH)/SPECS/postgresql.spec $(RPM_ERR_OUTPUT)
	$(RM) $(_TECS_RPM_PATH)/BUILD/postgresql_*

nicchk:rpmforce
	@echo making nicchk rpm
	$(RM) $(_TECS_RPM_PATH)/BUILD/nicchk
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/nicchk/usr/bin
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/nicchk/etc/nicchk
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/nicchk/etc/init.d
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/nicchk/etc/tecs/guard_list/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/nicchk/etc/logrotate.d/
	touch   $(_TECS_RPM_PATH)/BUILD/nicchk/etc/tecs/guard_list/nicchk
	$(CP)  $(_TECS_TOOLS_PATH)/services/nicchk $(_TECS_RPM_PATH)/BUILD/nicchk/etc/init.d/
	$(CP)  $(_TECS_TOOLS_PATH)/nicchk/nicchk.py $(_TECS_RPM_PATH)/BUILD/nicchk/usr/bin/
	$(CP)  $(_TECS_TOOLS_PATH)/nicchk/nicchk.conf  $(_TECS_RPM_PATH)/BUILD/nicchk/etc/nicchk
	$(CP)  $(_TECS_TOOLS_PATH)/logrotate.d/nicchk      $(_TECS_RPM_PATH)/BUILD/nicchk/etc/logrotate.d/
	$(MKDIR) $(_TECS_RPM_PATH)/BUILD/nicchk_build
	$(TECS_RPMBUILD)  --buildroot=$(_TECS_RPM_PATH)/BUILD/nicchk_build --define '_nicchk_temp $(_TECS_RPM_PATH)/BUILD/nicchk'  $(_TECS_RPM_PATH)/SPECS/nicchk.spec $(RPM_ERR_OUTPUT)


