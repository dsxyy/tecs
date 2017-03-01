BuildRoot: %{buildroot}

Summary: Guard in TECS
Name: tecs-guard
Version: %{_verno}
Release: %{release}
Vendor: zte
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/guard
#BuildRequires: 
#Prefix: %{_prefix}
%description
Guard is used to start and monitor all processes of TECS. 
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build
%install
cp %{_guard_temp}/* %{buildroot}/ -R
cd %{buildroot}/%{_prefix}/guard
mv GUARD%{_ver_suffix} tecs-guard
cd -
%clean
echo cleaning guard rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%dir %{_prefix}/guard
%{_prefix}/guard/tecs-guard
%{_prefix}/guard/guard.sh
/etc/init.d/tecs-guard
/etc/init.d/guard
%attr(755, root, root) /etc/init.d/tecs-guard
%config /etc/logrotate.d/tecs_guard

%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update tecs-guard, tecs-guard will stop"
    if [ -e /etc/init.d/tecs-guard ];then
        service tecs-guard stop &>/dev/null
    else
        service guard stop &>/dev/null
fi
fi

%post
#chkconfig --add tecs-guard
[ $1 = 2 ] && /sbin/chkconfig --list guard &>/dev/null && /sbin/chkconfig --del guard
/sbin/chkconfig --add tecs-guard
chmod +x %{_prefix}/guard/guard.sh
chkconfig --level 35 tecs-guard on
echo "finish..."

%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-guard stop &>/dev/null
    /sbin/chkconfig --del guard
    rm -rf /etc/init.d/guard
fi



%postun


