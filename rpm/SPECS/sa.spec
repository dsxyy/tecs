%define tecs_conf_path  /etc/tecs
%define sa_conf_file %{tecs_conf_path}/sa.config

BuildRoot: %{buildroot}
Summary: Cluster Controller in TECS
Name: tecs-sa
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/sa
#BuildRequires: 
Requires: ntpdate openais  qpidc-client >= 0.12 tecs-common >= %{_verno}-%{release}
Prefix: %{_prefix}
%description
Cluster Controller is used to manage all physical hosts 
in TECS cluster. 
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build

%install
cp %{_sa_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{sa_conf_file}
cd %{buildroot}/%{_prefix}/sa/bin
mv SA%{_ver_suffix} tecs-sa-main
cd -

%clean
echo cleaning sa rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/etc/init.d/tecs-sa
%attr(755, root, root) /etc/init.d/tecs-sa
%config %{sa_conf_file}
%dir %{_prefix}/sa
%dir %{_prefix}/sa/bin
%dir %{_prefix}/sa/lib
%dir %{_prefix}/sa/etc
%dir %{_prefix}/sa/scripts
%dir /etc/tecs/guard_list
%{_prefix}/sa/bin/tecs-sa-main
%{_prefix}/sa/lib/libsky.so
%{_prefix}/sa/lib/libstorage.so
%{_prefix}/sa/lib/libUspCapi.so
%config %{_prefix}/sa/etc/*.conf
%config %{_prefix}/sa/etc/sa.xml.example
%{_prefix}/sa/scripts/*
/etc/tecs/guard_list/tecs-sa
%config /etc/logrotate.d/tecs_local
%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update sa, tecs-sa will stop"
    service tecs-sa stop &>/dev/null
fi

%post
chmod +x $RPM_INSTALL_PREFIX/sa/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/sa/bin/tecs-sa-main  -r $RPM_INSTALL_PREFIX/sa/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/sa/lib/libsky.so  -r $RPM_INSTALL_PREFIX/sa/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/sa/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/sa/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/sa/lib/libUspCapi.so  -r $RPM_INSTALL_PREFIX/sa/lib >/dev/null 2>&1

echo $RPM_INSTALL_PREFIX/sa/bin/tecs-sa-main -f $RPM_INSTALL_PREFIX/sa/etc/tecs.conf  -d > %{sa_conf_file} 



#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
/sbin/chkconfig --add tecs-sa
chmod +x /opt/tecs/sa/scripts/update_cfg.sh
/opt/tecs/sa/scripts/update_cfg.sh /opt/tecs/sa/etc
echo "finish..."

%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-sa stop &>/dev/null
    /sbin/chkconfig --del tecs-sa
fi

%postun
/sbin/ldconfig >/dev/null 2>&1

