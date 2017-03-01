%define tecs_conf_path  /etc/tecs
%define hc_conf_file %{tecs_conf_path}/hc.config

BuildRoot: %{buildroot}
Summary: Host Controller in TECS
Name: tecs-hc
Version: %{_verno}
Release: %{release}
Vendor: zte
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/tc
#BuildRequires: 
Requires: qpidc-client >= 0.12
Requires: tecs-common >= %{_verno}-%{release}
#如果xen>=4.2，可再添加依赖xen-devel，这个包xen4.1没有，4.2有，它只在tecs编译时有用
Requires: xen >= 4.1.2   xen-licenses >= 4.1.2 xen-runtime >= 4.1.2 xen-libs >= 4.1.2 xen-hypervisor >= 4.1.2 
Requires: libvirt >= 0.9.12 libvirt-daemon >= 0.9.12 libvirt-devel >= 0.9.12 libvirt-client >= 0.9.12 
Requires: lm_sensors
Prefix: %{_prefix}
%description
Host Controller is used to manage single physical host in TECS. --SPEC For XEN
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build

%install
cp %{_hc_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{hc_conf_file}
cd %{buildroot}/%{_prefix}/hc/bin
mv HC%{_ver_suffix} tecs-hc-main
mv VERSION%{_ver_suffix} tecs-hc-version
cd -

%clean
echo cleaning hc rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/etc/init.d/tecs-hc
%attr(755, root, root) /etc/init.d/tecs-hc
%config %{hc_conf_file}
%dir %{_prefix}/hc
%dir %{_prefix}/hc/bin
%dir %{_prefix}/hc/lib
%dir %{_prefix}/hc/etc
%dir %{_prefix}/hc/scripts
%dir /etc/tecs/guard_list
%{_prefix}/hc/bin/tecs-hc-main
#%{_prefix}/hc/bin/tecs-hc-version
%{_prefix}/hc/bin/ubench
%{_prefix}/hc/lib/libsky.so
%{_prefix}/hc/lib/libsmart.so
%{_prefix}/hc/lib/libstorage.so
%{_prefix}/hc/lib/libnet.so
%{_prefix}/hc/lib/libtecsdb.so
%config %{_prefix}/hc/etc/*.conf
%{_prefix}/hc/scripts/*
/etc/tecs/guard_list/tecs-hc
%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update hc, tecs-hc will stop"
    service tecs-hc stop &>/dev/null
fi

%post
chmod +x $RPM_INSTALL_PREFIX/hc/bin/*
chmod +x $RPM_INSTALL_PREFIX/hc/scripts/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/bin/tecs-hc-main  -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1
#/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/bin/tecs-hc-version -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/lib/libsky.so  -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/lib/libsmart.so  -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1 
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/lib/libnet.so  -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/hc/lib/libtecsdb.so  -r $RPM_INSTALL_PREFIX/hc/lib >/dev/null 2>&1
#修改xend-config.sxp
sed -i '/^(network-script/s/^/#####/' /etc/xen/xend-config.sxp 

echo $RPM_INSTALL_PREFIX/hc/bin/tecs-hc-main -f $RPM_INSTALL_PREFIX/hc/etc/tecs.conf  -d > %{hc_conf_file} 
#echo $RPM_INSTALL_PREFIX/hc/bin/tecs-hc-version -f $RPM_INSTALL_PREFIX/hc/etc/tecs.conf  -d >> %{hc_conf_file}

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
/sbin/chkconfig --add tecs-hc
chmod +x /opt/tecs/hc/scripts/hc_poweron_upgrade.sh
chmod +x /opt/tecs/hc/scripts/update_cfg.sh
[ `cat /etc/rc.d/rc.local | grep hc_poweron_upgrade.sh -c` -eq 0 ] && echo "/opt/tecs/hc/scripts/hc_poweron_upgrade.sh" >> /etc/rc.d/rc.local
/opt/tecs/hc/scripts/update_cfg.sh /opt/tecs/hc/etc
echo "finish..."

%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-hc stop &>/dev/null
    /sbin/chkconfig --del tecs-hc
fi


%postun
/sbin/ldconfig >/dev/null 2>&1




