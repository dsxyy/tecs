%define tecs_conf_path  /etc/tecs

BuildRoot: %{buildroot}

Summary: special_device in TECS
Name: tecs-sd
Version: %{_verno}
Release: %{release}
Vendor: zte
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/sd
#BuildRequires: 
Requires: tecs-common >= %{_verno}-%{release}
Prefix: %{_prefix}
%description
Special_device is used to start tecs-special-device of TECS. 
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build
%install
cp %{_sd_temp}/* %{buildroot}/ -R
cd %{buildroot}/%{_prefix}/sd/bin
mv SPECIAL_DEVICE%{_ver_suffix} tecs-special-device
cd -
%clean
echo cleaning special-device rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%dir %{_prefix}/sd
%dir %{_prefix}/sd/bin
%dir %{_prefix}/sd/lib
%dir /etc/tecs/guard_list
%{_prefix}/sd/bin/tecs-special-device
%{_prefix}/sd/lib/libsky.so
/etc/init.d/tecs-sd
%attr(755, root, root) /etc/init.d/tecs-sd
%config %{_prefix}/sd/etc/*.conf
/etc/tecs/guard_list/tecs-sd
%config /etc/logrotate.d/tecs_local
%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update tecs-sd, tecs-sd will stop"
    service tecs-sd stop &>/dev/null
fi

%post
#chkconfig --add guard
/sbin/chkconfig --add tecs-sd
chmod +x $RPM_INSTALL_PREFIX/sd/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/sd/bin/tecs-special-device -r $RPM_INSTALL_PREFIX/sd/lib >/dev/null 2>&1

echo "finish..."

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

echo "tecs-sd install finished!"

%preun
/sbin/ldconfig >/dev/null 2>&1
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-sd stop &>/dev/null
    /sbin/chkconfig --del tecs-sd
fi


