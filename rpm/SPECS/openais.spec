# Spec file for Openais C++ packages, created by Newton

%define name openais
%define version 0.80.6
%define release 0

BuildRoot: %{buildroot}

Summary: An Openais daemon
Name: %{name}
Version: %{version}
Release: %{release}
Vendor: Red Hat, Inc.
License: ASL 2.0
Group: System Environment/Daemons
URL: http://www.openais.org
Source0: openais-0.80.6.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
%description
A message broker daemon that receives stores and routes messages using
the open ais protocol.

%package devel
Summary: Libraries, Header files for developing Openais Application
Group: Development/System

%description devel
Libraries, header files for developing Openais Application.  
Openais Application implements.

%prep
#%setup -q
#echo ${buildroot}

%build
#./configure --prefix="$RPM_BUILD_ROOT"
#make

%install
echo "openais install begin"
cp %{_openais_temp}/* %{buildroot}/ -R
#./keygen
#cp  amf.conf      /etc/ais/amf.conf
#cp  openais.conf  /etc/ais/openais.conf
#./aisexec

%clean

%files
%defattr(-,root,root)
/etc/init.d/openais
%attr(755, root, root) /etc/init.d/openais
/usr/local/bin/aisexec
/usr/local/bin/keygen
/usr/local/bin/aisparser.lcrso
/usr/local/bin/objdb.lcrso
/usr/local/bin/service_amf.lcrso
/usr/local/bin/service_cfg.lcrso
/usr/local/bin/service_ckpt.lcrso
/usr/local/bin/service_clm.lcrso
/usr/local/bin/service_confdb.lcrso
/usr/local/bin/service_cpg.lcrso
/usr/local/bin/service_evs.lcrso
/usr/local/bin/service_evt.lcrso
/usr/local/bin/service_lck.lcrso
/usr/local/bin/service_msg.lcrso
/usr/local/bin/vsf_ykd.lcrso
/etc/ais/openais.conf
/etc/ais/amf.conf
/usr/local/lib/libms.so
/usr/local/lib/libms.a
/usr/local/lib/libcpg.*
/usr/local/lib/libcfg.*
%changelog

%files devel
%defattr(-,root,root)
/usr/local/include
%changelog

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update openais, openais will stop"
    service openais stop &>/dev/null
fi

%post
/sbin/chkconfig --add openais
chkconfig --level 2345 openais on

%post devel

%postun
/sbin/ldconfig
  
%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service openais stop &>/dev/null
    /sbin/chkconfig --del openais 
fi
    