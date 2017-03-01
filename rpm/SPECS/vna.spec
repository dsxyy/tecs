%define tecs_conf_path  /etc/tecs
%define vna_conf_file %{tecs_conf_path}/vna.config

BuildRoot: %{buildroot}
Summary: virtual network agent in TECS
Name: tecs-vna
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/network/vna
#BuildRequires: 
Requires: qpidc-client >= 0.12 tecs-common >= %{_verno}-%{release}
Prefix: %{_prefix}
%description
virtual network agent in TECS
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build

%install
cp %{_vna_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{vna_conf_file}
cd %{buildroot}/%{_prefix}/network/vna/bin
mv NETWORK_VNA*  tecs-network-vna
cd -

%clean
echo cleaning vna rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/etc/init.d/tecs-vna
%config %{vna_conf_file}
%dir %{_prefix}/network/vna
%dir %{_prefix}/network/vna/bin
%dir %{_prefix}/network/vna/lib
%dir %{_prefix}/network/vna/etc
%dir %{_prefix}/network/vna/scripts
%dir /etc/tecs/guard_list
%{_prefix}/network/vna/bin/tecs-network-vna
%{_prefix}/network/vna/lib/*.so
%config %{_prefix}/network/vna/etc/*.conf
%{_prefix}/network/vna/scripts/*
/etc/tecs/guard_list/tecs-vna
%config /etc/logrotate.d/tecs_local

%changelog

%post
chmod +x $RPM_INSTALL_PREFIX/network/vna/scripts/*
chmod +x $RPM_INSTALL_PREFIX/network/vna/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vna/bin/tecs-network-vna  -r $RPM_INSTALL_PREFIX/network/vna/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vna/lib/libsky.so  -r $RPM_INSTALL_PREFIX/network/vna/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vna/lib/libtecsdb.so  -r $RPM_INSTALL_PREFIX/network/vna/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/vna/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/network/vna/lib >/dev/null 2>&1

echo $RPM_INSTALL_PREFIX/network/vna/bin/tecs-network-vna -f $RPM_INSTALL_PREFIX/network/vna/etc/tecs.conf  -d > %{vna_conf_file} 

chkconfig --level 35 tecs-vna on

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
chmod +x /etc/init.d/tecs-vna
if [ ! -d %{tecs_conf_path} ]; then
    mkdir -p %{tecs_conf_path}
fi
if [ ! -e %{tecs_guard_file} ]; then
    touch %{tecs_guard_file}
else
    sed -i '/tecs-vna/d' %{tecs_guard_file}
fi
echo "tecs-vna" >> %{tecs_guard_file}

chmod +x /opt/tecs/network/vna/scripts/update_cfg.sh
/opt/tecs/network/vna/scripts/update_cfg.sh /opt/tecs/network/vna/etc
echo "finish..."
echo "tecs-vna install finished!"

%preun
#卸载之前，首先将服务名称从监控列表中删除
    service tecs-vna stop &>/dev/null
[ -e %{tecs_guard_file} ] && sed -i '/tecs-vna/d' %{tecs_guard_file}
echo "tecs-vna uninstall finished!"

%postun
#guard文件如果已经是空的，就删除掉
if [ -e %{tecs_guard_file} ]; then
    lineno=`cat %{tecs_guard_file} | wc -l`
    [ "$lineno" == "0" ] &&  rm -f %{tecs_guard_file} 
fi
/sbin/ldconfig >/dev/null 2>&1


