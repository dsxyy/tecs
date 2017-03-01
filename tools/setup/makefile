ifeq (_el6,$(_OS_TYPE))
PURE_OS_TYPE = el6
endif
RPM_OS_TYPE=linux
BIN_TYPE = $(PURE_OS_TYPE)_$(ARCH)

bin: binclean
	$(MKDIR) $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/*
	$(MKDIR) $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(MKDIR) $(_TECS_TOOLS_PATH)/setup/bin_temp/common   
	$(MKDIR) $(_TECS_TOOLS_PATH)/setup/bin_temp/install
	$(MKDIR) $(_TECS_TOOLS_PATH)/setup/bin_temp/upgrade  
	$(MKDIR) $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/base/*
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/common/*
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/install/*
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/upgrade/*
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall/*
	$(CP) $(_TECS_RPM_PATH)/RPMS/$(PURE_OS_TYPE)/$(ARCH)/* $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp/*devel*.rpm
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-devel-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-daemon-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-client-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-daemon-config-network-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-daemon-config-nwfilter-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/libvirt-docs-0.9.12-1.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/libvirt/pm-utils-1.2.5-9.el6.x86_64.rpm $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/tgtd/scsi-target-utils-1.0.24-3.el6_4.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/tgtd/lsof-4.82-4.el6.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/tgtd/sg3_utils-1.28-4.el6.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/tgtd/sg3_utils-libs-1.28-4.el6.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_TOOLS_PATH)/setup/setup.sh $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_TOOLS_PATH)/setup/tecs_install.conf $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_TOOLS_PATH)/setup/network_patch.sh $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_TOOLS_PATH)/config_sensors_detect.sh $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_TOOLS_PATH)/setup/common/tecs_common_func.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/common
	$(CP) $(_TECS_TOOLS_PATH)/setup/common/tecs_global_var.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/common
	$(CP) $(_TECS_TOOLS_PATH)/setup/common/tecs_yum.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/common
	$(CP) $(_TECS_TOOLS_PATH)/setup/base/firewall_config.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(CP) $(_TECS_TOOLS_PATH)/setup/base/firewall_interface.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(CP) $(_TECS_TOOLS_PATH)/setup/base/install_pgdb.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(CP) $(_TECS_TOOLS_PATH)/setup/base/update_pgdb.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(CP) $(_TECS_TOOLS_PATH)/setup/base/pgdb_interface.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(CP) $(_TECS_TOOLS_PATH)/setup/base/qpid_interface.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/base
	$(CP) $(_TECS_TOOLS_PATH)/setup/install/create_kernel_port.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/install
	$(CP) $(_TECS_TOOLS_PATH)/setup/install/install_func.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/install
	$(CP) $(_TECS_TOOLS_PATH)/setup/install/install_interface.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/install
	$(CP) $(_TECS_TOOLS_PATH)/setup/install/install_global_var.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/install
	$(CP) $(_TECS_TOOLS_PATH)/ci/agent_tools/clear.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall
	$(CP) $(_TECS_TOOLS_PATH)/ci/agent_tools/netclear.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall
	$(CP) $(_TECS_TOOLS_PATH)/setup/uninstall/uninstall_interface.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall
	$(CP) $(_TECS_TOOLS_PATH)/setup/uninstall/uninstall_func.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall
	$(CP) $(_TECS_TOOLS_PATH)/setup/upgrade/upgrade_interface.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/upgrade
	$(CP) $(_TECS_TOOLS_PATH)/setup/upgrade/upgrade_func.sh $(_TECS_TOOLS_PATH)/setup/bin_temp/upgrade
	#$(CP) $(_TECS_CONTRIB_PATH)/openvswitch/kmod-openvswitch-1.4.5-1.el6.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	#$(CP) $(_TECS_CONTRIB_PATH)/openvswitch/openvswitch-1.4.5-1.x86_64.rpm           $(_TECS_TOOLS_PATH)/setup/bin_temp
	#chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/kmod-openvswitch-1.4.5-1.el6.x86_64.rpm
	#chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/openvswitch-1.4.5-1.x86_64.rpm 
	#$(CP) $(_TECS_CONTRIB_PATH)/openvswitch/kmod-openvswitch-1.10.0-1.el6.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	#$(CP) $(_TECS_CONTRIB_PATH)/openvswitch/openvswitch-1.10.0-1.x86_64.rpm           $(_TECS_TOOLS_PATH)/setup/bin_temp
	#chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/kmod-openvswitch-1.10.0-1.el6.x86_64.rpm
	#chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/openvswitch-1.10.0-1.x86_64.rpm
	$(CP) $(_TECS_CONTRIB_PATH)/openvswitch/kmod-openvswitch-1.11.0-1.el6.x86_64.rpm  $(_TECS_TOOLS_PATH)/setup/bin_temp
	$(CP) $(_TECS_CONTRIB_PATH)/openvswitch/openvswitch-1.11.0-1.x86_64.rpm           $(_TECS_TOOLS_PATH)/setup/bin_temp
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/kmod-openvswitch-1.11.0-1.el6.x86_64.rpm
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/openvswitch-1.11.0-1.x86_64.rpm	

	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/setup.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/network_patch.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/config_sensors_detect.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/common/tecs_common_func.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/common/tecs_global_var.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/common/tecs_yum.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/base/firewall_config.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/base/firewall_interface.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/base/install_pgdb.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/base/qpid_interface.sh 
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/base/update_pgdb.sh 
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/base/pgdb_interface.sh 
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/install/create_kernel_port.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/install/install_func.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/install/install_interface.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/install/install_global_var.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall/uninstall_interface.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall/clear.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall/netclear.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/uninstall/uninstall_func.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/upgrade/upgrade_interface.sh
	chmod +x $(_TECS_TOOLS_PATH)/setup/bin_temp/upgrade/upgrade_func.sh
	cd $(_TECS_TOOLS_PATH)/setup; chmod +x ./makeself.sh; ./makeself.sh ./bin_temp installtecs_$(BIN_TYPE).bin tecs ./setup.sh
	$(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp

binclean:
	if [ -e $(_TECS_TOOLS_PATH)/setup/installtecs_$(BIN_TYPE).bin ]; then $(RM) $(_TECS_TOOLS_PATH)/setup/installtecs_$(BIN_TYPE).bin; fi
	if [ -e $(_TECS_TOOLS_PATH)/setup/bin_temp ]; then $(RM) $(_TECS_TOOLS_PATH)/setup/bin_temp; fi
