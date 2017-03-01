%define tecs_conf_path  /etc/tecs
%define cc_conf_file %{tecs_conf_path}/cc.config

BuildRoot: %{buildroot}
Summary: Cluster Controller in TECS
Name: tecs-cc
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/cc
#BuildRequires: 
Requires: openais ntpdate ntp qpidc-client >= 0.12 postgresql-client tecs-common >= %{_verno}-%{release} 
Prefix: %{_prefix}
%description
Cluster Controller is used to manage all physical hosts 
in TECS cluster. 
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build


%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update cc, tecs-cc will stop"
    service tecs-cc stop &>/dev/null
fi



%install
cp %{_cc_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{cc_conf_file}
cd %{buildroot}/%{_prefix}/cc/bin
mv CC%{_ver_suffix} tecs-cc-main
#mv CC_NAT%{_ver_suffix} tecs-cc-nat
mv IMAGE_AGENT%{_ver_suffix} tecs-cc-imageagent
mv VERSION%{_ver_suffix} tecs-cc-version
mv CC_SSH_TRUST%{_ver_suffix} tecs-cc-ssh-trust
cd -

%clean
echo cleaning cc rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/etc/init.d/tecs-cc
%attr(755, root, root) /etc/init.d/tecs-cc
%config %{cc_conf_file}
%dir %{_prefix}/cc
%dir %{_prefix}/cc/bin
%dir %{_prefix}/cc/lib
%dir %{_prefix}/cc/etc
%dir %{_prefix}/cc/scripts
%dir /etc/tecs/guard_list
%{_prefix}/cc/bin/tecs-cc-main
%{_prefix}/cc/bin/tecs-cc-imageagent
#%{_prefix}/cc/bin/tecs-cc-nat
#%{_prefix}/cc/bin/tecs-cc-version
%{_prefix}/cc/bin/tecs-cc-ssh-trust
%{_prefix}/cc/lib/libsky.so
%{_prefix}/cc/lib/libnet.so
%{_prefix}/cc/lib/libtecsdb.so
%{_prefix}/cc/lib/libworkflow.so
%{_prefix}/cc/lib/libpq.*
%{_prefix}/cc/lib/libstorage.so
%config %{_prefix}/cc/etc/*.conf
%{_prefix}/cc/etc/*.sql
%{_prefix}/cc/etc/updates/*
%{_prefix}/cc/scripts/*
/etc/tecs/guard_list/tecs-cc
%config /etc/logrotate.d/tecs_local
%config /etc/logrotate.d/tecs_hc

%changelog

%post
chmod +x $RPM_INSTALL_PREFIX/cc/scripts/*
chmod +x $RPM_INSTALL_PREFIX/cc/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-main  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-imageagent -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
#/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-nat  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
#/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-version -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-ssh-trust -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/lib/libsky.so  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/lib/libtecsdb.so  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/lib/libworkflow.so  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/cc/lib/libnet.so  -r $RPM_INSTALL_PREFIX/cc/lib >/dev/null 2>&1

echo $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-main -f $RPM_INSTALL_PREFIX/cc/etc/tecs.conf  -d > %{cc_conf_file} 
echo $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-imageagent -f $RPM_INSTALL_PREFIX/cc/etc/tecs.conf -d >> %{cc_conf_file} 
#echo $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-nat -f $RPM_INSTALL_PREFIX/cc/etc/tecs.conf  -d >> %{cc_conf_file} 
echo $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-ssh-trust -f $RPM_INSTALL_PREFIX/cc/etc/tecs.conf  -d >> %{cc_conf_file} 
#echo $RPM_INSTALL_PREFIX/cc/bin/tecs-cc-version -f $RPM_INSTALL_PREFIX/cc/etc/tecs.conf  -d >> %{cc_conf_file}

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
/sbin/chkconfig --add tecs-cc
chmod +x /opt/tecs/cc/scripts/update_cfg.sh
/opt/tecs/cc/scripts/update_cfg.sh /opt/tecs/cc/etc
echo "finish..."

%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-cc stop &>/dev/null
    /sbin/chkconfig --del tecs-cc
fi


%postun
/sbin/ldconfig >/dev/null 2>&1

