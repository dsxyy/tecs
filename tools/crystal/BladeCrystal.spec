Summary: BladeCrystal for CentOS
Name: BladeCrystal
Version: 1.0.0
Release: 1
License: GPL
Group: Applications/Engineering
Source: BladeCrystal-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Vendor:ZTE

%description
BladeCrystal provides crystal status of blade board by /proc/zte/crystal.

%prep

%setup -q
 
%build
make

%install
rm -rf $RPM_BUILD_ROOT
make install INSTALLROOT="$RPM_BUILD_ROOT" 

%clean
rm -rf $RPM_BUILD_ROOT
 
%files
/start.sh
/BladeCrystal.ko

%post
chmod 777 -R /start.sh
./start.sh
echo "finish insmod BladeCrystal..."
 
%postun
rmmod BladeCrystal
echo "finish rmmod BladeCrystal..."
 
%changelog
*Wed Mar 28 2012 jimei <ji.mei@zte.com.cn>
-creat
