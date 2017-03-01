%define tecs_conf_path  /etc/tecs

BuildRoot: %{buildroot}

Summary: ZTE WUI
Name: tecs-wui
Version: %{_verno}
Release: %{release}
Vendor: ZTE
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRoot: %{_topdir}/BUILD/wui
Prefix: %{_prefix}
%description
tecs web user interface.
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build
%install
cp %{_wui_temp}/* %{buildroot}/ -R

%clean
rm -rf %{buildroot}
echo cleaning wui rpm build temp files...

%files
%defattr(-,root,root)
/etc/init.d/tecs-wui
%attr(755, root, root) /etc/init.d/tecs-wui
/opt/tecs/wui/
%config /opt/tecs/wui/htdocs/newweb/php/db_connect.ini
/etc/tecs/guard_list/tecs-wui
%dir /etc/tecs/guard_list

%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update tecs-wui, tecs-wui will stop"
    service tecs-wui stop &>/dev/null
fi

%post
/sbin/chkconfig --add tecs-wui
chmod +x /opt/tecs/wui/update_wui.sh
/opt/tecs/wui/update_wui.sh /opt/tecs/wui/htdocs/newweb/php
echo "finish..."

%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service tecs-wui stop &>/dev/null
    /sbin/chkconfig --del tecs-wui
fi
