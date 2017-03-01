# Spec file for Qpid C++ packages, created by zwj

%define name qpidc
%define version 0.14
%define release 3

BuildRoot: %{buildroot}

Summary: An AMQP message broker daemon
Name: %{name}
Version: %{version}
Release: %{release}
Vendor: Red Hat, Inc.
License: ASL 2.0
Group: System Environment/Daemons
URL: http://qpid.apache.org
Source0: qpid-cpp-0.12.tar.gz
%description
A message broker daemon that receives stores and routes messages using
the open AMQP messaging protocol.

%package broker
Summary: An AMQP message broker daemon
Group: System Environment/Daemons
%description broker
A message broker daemon that receives stores and routes messages using
the open AMQP messaging protocol.

%package devel
Summary: Libraries, Header files for developing Qpid C++ clients
Group: Development/System
%description devel
Libraries, header files for developing AMQP clients in C++ using Qpid.  
Qpid implements the AMQP messaging specification.

%package client
Summary: Libraries for Qpid C++ client applications
Group: System Environment/Libraries
%description client
Run-time libraries for AMQP client applications developed using Qpid
C++. Clients exchange messages with an AMQP message broker using
the AMQP protocol.

%prep
#%setup -q

%build
#./configure --prefix="$RPM_BUILD_ROOT"
#make

%install
cp %{_qpid_temp}/* %{buildroot}/ -R


%clean
rm -rf %{buildroot}

%files broker
%defattr(-,root,root)
/etc/init.d/qpid
%attr(755, root, root) /etc/init.d/qpid
/usr/local/bin
/usr/local/etc
/usr/local/lib
/usr/local/lib64
/usr/local/libexec
/usr/local/sbin
/usr/local/var
%config /etc/logrotate.d/qpid
%changelog

%files devel
%defattr(-,root,root)
/usr/local/include
#/usr/local/lib/_cqpid.*
#/usr/local/lib/libcqpid_perl.*
/usr/local/lib/libqpidclient.*
/usr/local/lib/libqpidcommon.*
/usr/local/lib/libqpidmessaging.*
/usr/local/lib/libqpidtypes.*
%changelog

%files client
%defattr(-,root,root)
/usr/local/etc/qpid
%config /usr/local/etc/qpid/qpidc.conf
#/usr/local/lib/_cqpid.*
#/usr/local/lib/libcqpid_perl.*
/usr/local/lib/libqpidclient.*
/usr/local/lib/libqpidcommon.*
/usr/local/lib/libqpidmessaging.*
/usr/local/lib/libqpidtypes.*
%changelog

#安装之前需要先在/etc/ld.so.conf中加入/usr/local/lib/
%define ld_conf_file /etc/ld.so.conf.d/qpid.conf
%post broker
/sbin/chkconfig --add qpid
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1
chkconfig --level 345 qpid on

%post devel
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

%pre broker
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update qpid, qpid will stop"
    service qpid stop &>/dev/null
fi

%post client
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/lib > %{ld_conf_file}
fi
/sbin/ldconfig >/dev/null 2>&1

%postun
/sbin/ldconfig >/dev/null 2>&1
    
%preun broker 
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service qpid stop &>/dev/null
    /sbin/chkconfig --del qpid 
fi
