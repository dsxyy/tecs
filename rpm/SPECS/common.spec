BuildRoot: %{buildroot}

Summary: Common utils in TECS
Name: tecs-common
Version: %{_verno}
Release: %{release}
Vendor: zte
License: GPL
Group: Applications/Daemons
URL: http://tecs.zte.com.cn/
#BuildRequires: 
#Prefix: %{_prefix}
%description
tecs-common is basic component used by TC,CC and HC in TECS. 
CI Build Id = %{_description}
SVN Revision = %{_svn_revision}
%prep
%build
%install
cp %{_common_temp}/* %{buildroot}/ -R
%clean
echo cleaning common rpm build temp files...
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/usr/local/bin/
%changelog

%post
chmod +x /usr/local/bin/*.sh
echo "finish..."
%postun

