%define tecs_conf_path  /etc/tecs
%define tc_conf_file %{tecs_conf_path}/tc.config

BuildRoot: %{buildroot}
Summary: ZTE TECS Center
Name: tecs-tc
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/tc
#BuildRequires:
Requires:  openais qpidc-client >= 0.12 postgresql-client tecs-common >= %{_verno}-%{release} ntp ntpdate
Prefix: %{_prefix}
%description
tecs center is manager of cluster controller in IaaS,
and provide API for user.
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build


%install
cp %{_tc_temp}/* %{buildroot}/ -R
mkdir -p %{buildroot}%{tecs_conf_path}
touch %{buildroot}%{tc_conf_file}
cd %{buildroot}/%{_prefix}/tc/bin
mv TC%{_ver_suffix} tecs-tc-main
mv APISVR%{_ver_suffix} tecs-tc-apisvr
mv IMAGE_MANAGER%{_ver_suffix} tecs-tc-imagemanager
mv VERSION%{_ver_suffix} tecs-tc-version
mv TC_SSH_TRUST%{_ver_suffix} tecs-tc-ssh-trust
mv SC%{_ver_suffix} tecs-sc-main
mv FILE_MANAGER%{_ver_suffix} tecs-file-manager
cd -

%clean
echo cleaning tc rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%config(noreplace) /etc/tecs/license.*
/etc/init.d/tecs-tc
%attr(755, root, root) /etc/init.d/tecs-tc
%config %{tc_conf_file}
/usr/local/lib/libxmlrpc++.*
/usr/local/lib/libxmlrpc_client++.*
/usr/local/lib/libxmlrpc_packetsocket.*
/usr/local/lib/libxmlrpc_client.*
/usr/local/lib/libxmlrpc_server++.*
/usr/local/lib/libxmlrpc.*
/usr/local/lib/libxmlrpc_xmlparse.*
/usr/local/lib/libxmlrpc_xmltok.*
/usr/local/lib/libxmlrpc_util.*
/usr/local/lib/libxmlrpc_server_abyss.*
/usr/local/lib/libxmlrpc_server.*
/usr/local/lib/libxmlrpc_abyss.*
/usr/local/lib/libxmlrpc_server_abyss++.*
%config /usr/local/abyss/conf/abyss.conf
/usr/local/abyss/conf/mime.types
/etc/tecs/guard_list/tecs-tc
%dir %{_prefix}/tc
%dir %{_prefix}/tc/bin
%dir %{_prefix}/tc/lib
%dir %{_prefix}/tc/etc
%dir %{_prefix}/tc/scripts
%dir %{_prefix}/tc/cli
%dir /usr/local/abyss/wui
%dir /etc/tecs/guard_list
%config %{_prefix}/tc/etc/*.conf
%config %{_prefix}/tc/etc/snmp.xml
%{_prefix}/tc/bin/*
%{_prefix}/tc/lib/*
%{_prefix}/tc/etc/*.sql
%{_prefix}/tc/etc/updates/*
%{_prefix}/tc/scripts/*
%{_prefix}/tc/cli/*
%config /etc/logrotate.d/tecs_local
%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update tecs-tc, tecs-tc will stop"
    service tecs-tc stop &>/dev/null
fi

%post
chmod +x $RPM_INSTALL_PREFIX/tc/scripts/*
chmod +x $RPM_INSTALL_PREFIX/tc/bin/*
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-main  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-imagemanager -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-apisvr -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-version -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-ssh-trust -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-sc-main  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/bin/tecs-file-manager  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/lib/libsky.so  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/lib/libtecsdb.so  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/lib/libworkflow.so  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/lib/libauth.so  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/lib/libstorage.so  -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1
/usr/local/bin/chrpath $RPM_INSTALL_PREFIX/tc/lib/libnet.so      -r $RPM_INSTALL_PREFIX/tc/lib >/dev/null 2>&1

echo $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-main -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d > %{tc_conf_file}
echo $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-imagemanager -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d >> %{tc_conf_file}
echo $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-apisvr -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d >> %{tc_conf_file}
echo $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-version -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d >> %{tc_conf_file}
echo $RPM_INSTALL_PREFIX/tc/bin/tecs-tc-ssh-trust -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d >> %{tc_conf_file}
echo $RPM_INSTALL_PREFIX/tc/bin/tecs-sc-main -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d >> %{tc_conf_file}
echo $RPM_INSTALL_PREFIX/tc/bin/tecs-file-manager -f $RPM_INSTALL_PREFIX/tc/etc/tecs.conf  -d >> %{tc_conf_file}

#安装好之后需要在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/tecs.conf
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

#最后一步：将服务名称加入监控列表
/sbin/chkconfig --add tecs-tc
chmod +x /opt/tecs/tc/scripts/update_cfg.sh
/opt/tecs/tc/scripts/update_cfg.sh /opt/tecs/tc/etc
echo "finish..."



%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-tc stop &>/dev/null
    /sbin/chkconfig --del tecs-tc
fi

%postun
/sbin/ldconfig >/dev/null 2>&1
