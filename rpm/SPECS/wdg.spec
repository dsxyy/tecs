%define tecs_conf_path  /etc/tecs
%define wdg_conf_file %{tecs_conf_path}/wdg.config

BuildRoot: %{buildroot}
Summary: virtual network agent in TECS
Name: tecs-wdg
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/network/wdg
#BuildRequires: 
Requires: qpidc-client >= 0.12 tecs-common >= %{_verno}-%{release}
Prefix: %{_prefix}
%description
watch dog service
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build

%install
cp %{_wdg_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{wdg_conf_file}
cd %{buildroot}/%{_prefix}/network/wdg/bin
mv NETWORK_WDG*  tecs-network-wdg
cd -

%clean
echo cleaning wdg rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/etc/init.d/tecs-wdg
%config %{wdg_conf_file}
%dir %{_prefix}/network/wdg
%dir %{_prefix}/network/wdg/bin
%dir %{_prefix}/network/wdg/lib
%dir %{_prefix}/network/wdg/etc
%dir %{_prefix}/network/wdg/scripts
%dir /etc/tecs/guard_list
%{_prefix}/network/wdg/bin/tecs-network-wdg
%{_prefix}/network/wdg/lib/*.so
%config %{_prefix}/network/wdg/etc/*.conf
%{_prefix}/network/wdg/scripts/*
/etc/tecs/guard_list/tecs-wdg
%config /etc/logrotate.d/tecs_local


%changelog

%post
chmod +x $RPM_INSTALL_PREFIX/network/wdg/scripts/*
chmod +x $RPM_INSTALL_PREFIX/network/wdg/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/wdg/bin/tecs-network-wdg  -r $RPM_INSTALL_PREFIX/network/wdg/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/wdg/lib/libsky.so  -r $RPM_INSTALL_PREFIX/network/wdg/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/wdg/lib/libtecsdb.so  -r $RPM_INSTALL_PREFIX/network/wdg/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/network/wdg/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/network/wdg/lib >/dev/null 2>&1

echo $RPM_INSTALL_PREFIX/network/wdg/bin/tecs-network-wdg -f $RPM_INSTALL_PREFIX/network/wdg/etc/tecs.conf  -d > %{wdg_conf_file} 

chkconfig --level 35 tecs-wdg on

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
chmod +x /etc/init.d/tecs-wdg
if [ ! -d %{tecs_conf_path} ]; then
    mkdir -p %{tecs_conf_path}
fi
if [ ! -e %{tecs_guard_file} ]; then
    touch %{tecs_guard_file}
else
    sed -i '/tecs-wdg/d' %{tecs_guard_file}
fi
echo "tecs-wdg" >> %{tecs_guard_file}
chmod +x /opt/tecs/network/wdg/scripts/update_cfg.sh
/opt/tecs/network/wdg/scripts/update_cfg.sh /opt/tecs/network/wdg/etc
echo "finish..."
echo "tecs-wdg install finished!"

%preun
#卸载之前，首先将服务名称从监控列表中删除
    service tecs-wdg stop &>/dev/null
[ -e %{tecs_guard_file} ] && sed -i '/tecs-wdg/d' %{tecs_guard_file}
echo "tecs-wdg uninstall finished!"

%postun
#guard文件如果已经是空的，就删除掉
if [ -e %{tecs_guard_file} ]; then
    lineno=`cat %{tecs_guard_file} | wc -l`
    [ "$lineno" == "0" ] &&  rm -f %{tecs_guard_file} 
fi
/sbin/ldconfig >/dev/null 2>&1


