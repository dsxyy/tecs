%define tecs_conf_path  /etc/tecs
%define vnm_conf_file %{tecs_conf_path}/vnm.config

BuildRoot: %{buildroot}
Summary: virtual network managment in TECS
Name: tecs-vnm
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/network/vnm
#BuildRequires: 
Requires: qpidc-client >= 0.12 postgresql-client tecs-common >= %{_verno}-%{release}
Prefix: %{_prefix}
%description
virtual network managment in TECS
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build

%install
cp %{_vnm_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{vnm_conf_file}
cd %{buildroot}/%{_prefix}/network/vnm/bin
mv NETWORK_VNM*  tecs-network-vnm
cd -

%clean
echo cleaning vnm rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/etc/init.d/tecs-vnm
%config %{vnm_conf_file}
%dir %{_prefix}/network/vnm
%dir %{_prefix}/network/vnm/bin
%dir %{_prefix}/network/vnm/lib
%dir %{_prefix}/network/vnm/etc
%dir %{_prefix}/network/vnm/etc/updates
%dir %{_prefix}/network/vnm/scripts
%dir /etc/tecs/guard_list
%{_prefix}/network/vnm/bin/tecs-network-vnm
%{_prefix}/network/vnm/lib/*
%config %{_prefix}/network/vnm/etc/*.conf
%{_prefix}/network/vnm/scripts/*
%{_prefix}/network/vnm/etc/updates/*
%{_prefix}/network/vnm/etc/*.sql
/etc/tecs/guard_list/tecs-vnm
%config /etc/logrotate.d/tecs_local

%changelog

%post
chmod +x $RPM_INSTALL_PREFIX/network/vnm/scripts/*
chmod +x $RPM_INSTALL_PREFIX/network/vnm/etc/*
chmod +x $RPM_INSTALL_PREFIX/network/vnm/etc/updates/*
chmod +x $RPM_INSTALL_PREFIX/network/vnm/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/bin/tecs-network-vnm  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/libsky.so  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/libtecsdb.so  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_xmltok.so.3.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_xmlparse.so.3.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_util.so.3.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc.so.3.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_client.so.3.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_packetsocket.so.7.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_client++.so.7.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc++.so.7.25  -r $RPM_INSTALL_PREFIX/network/vnm/lib >/dev/null 2>&1

ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_xmltok.so.3.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_xmltok.so.3
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_xmlparse.so.3.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_xmlparse.so.3
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_util.so.3.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_util.so.3
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc.so.3.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc.so.3
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_client.so.3.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_client.so.3
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_packetsocket.so.7.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_packetsocket.so.7
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_client++.so.7.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc_client++.so.7
ln -s $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc++.so.7.25 $RPM_INSTALL_PREFIX/network/vnm/lib/librestrpc++.so.7

echo $RPM_INSTALL_PREFIX/network/vnm/bin/tecs-network-vnm -f $RPM_INSTALL_PREFIX/network/vnm/etc/tecs.conf  -d > %{vnm_conf_file} 

chkconfig --level 35 tecs-vnm on

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
chmod +x /etc/init.d/tecs-vnm
if [ ! -d %{tecs_conf_path} ]; then
    mkdir -p %{tecs_conf_path}
fi
if [ ! -e %{tecs_guard_file} ]; then
    touch %{tecs_guard_file}
else
    sed -i '/tecs-vnm/d' %{tecs_guard_file}
fi
echo "tecs-vnm" >> %{tecs_guard_file}

chmod +x /opt/tecs/network/vnm/scripts/update_cfg.sh
/opt/tecs/network/vnm/scripts/update_cfg.sh /opt/tecs/network/vnm/etc
echo "tecs-vnm install finished!"

%preun
#卸载之前，首先将服务名称从监控列表中删除
    service tecs-vnm stop &>/dev/null
[ -e %{tecs_guard_file} ] && sed -i '/tecs-vnm/d' %{tecs_guard_file}
echo "tecs-vnm uninstall finished!"

%postun
#guard文件如果已经是空的，就删除掉
if [ -e %{tecs_guard_file} ]; then
    lineno=`cat %{tecs_guard_file} | wc -l`
    [ "$lineno" == "0" ] &&  rm -f %{tecs_guard_file} 
fi
/sbin/ldconfig >/dev/null 2>&1


