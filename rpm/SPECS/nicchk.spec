# Spec file for nicchk packages, created by zb

Summary: An nic check daemon
BuildRoot: %{buildroot}
Summary: Host Controller in TECS
Name: nicchk
Version: %{_verno}
Release: %{release}
Vendor: zte
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
%description
A nic state check daemon customed.
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build

%install
cp %{_nicchk_temp}/*  %{buildroot}/ -R


%clean
rm -rf %{buildroot}

%files 
%defattr(-,root,root)
/etc/init.d/nicchk
/etc/nicchk/nicchk.conf
/usr/bin/nicchk.py
/etc/tecs/guard_list/nicchk
/etc/logrotate.d/nicchk
%attr(755, root, root) /etc/init.d/nicchk
%attr(755,root,root) /usr/bin/nicchk.py
%attr(644,root,root) /etc/nicchk/nicchk.conf
%attr(644,root,root) /etc/tecs/guard_list/nicchk
%attr(644,root,root) /etc/logrotate.d/nicchk
%changelog



%post 
/sbin/chkconfig --add nicchk
chkconfig --level 345 nicchk on
/sbin/ldconfig >/dev/null 2>&1

%pre 

if [ $1 = 2 ]; then
    echo "update nicchk, nicchk will stop"
    service nicchk stop &>/dev/null
fi

%postun
/sbin/ldconfig >/dev/null 2>&1
    
%preun  
if [ $1 = 0 ]; then
    service nicchk stop &>/dev/null
    /sbin/chkconfig --del nicchk 
fi
