# Spec file for postgresql C++ packages, created by kimi

%define name postgresql
%define version 9.0.2
%define release 5

BuildRoot: %{buildroot}

Summary: POSTGRESQL9.0.2
Name: %{name}
Version: %{version}
Release: %{release}
Vendor: Red Hat, Inc.
License: ASL 2.0
Group: Database
URL: http://www.postgresql.org/
Source0: postgresql-9.0.2.tar
%description
PostgreSQL is an advanced Object-Relational database management system
(DBMS) that supports almost all SQL constructs (including
transactions, subselects and user-defined types and functions). The
postgresql package includes the client programs and libraries that
you'll need to access a PostgreSQL DBMS server.  These PostgreSQL
client programs are programs that directly manipulate the internal
structure of PostgreSQL databases on a PostgreSQL server. These client
programs can be located on the same machine with the PostgreSQL
server, or may be on a remote machine which accesses a PostgreSQL
server over a network connection. This package contains the client
libraries for C and C++, as well as command-line utilities for
managing PostgreSQL databases on a PostgreSQL server.

%package client
Summary: PostgreSQL client programs and libraries
Group: Databases
%description client
PostgreSQL is an advanced Object-Relational database management system
(DBMS) that supports almost all SQL constructs (including
transactions, subselects and user-defined types and functions). The
postgresql package includes the client programs and libraries that
you'll need to access a PostgreSQL DBMS server.  These PostgreSQL
client programs are programs that directly manipulate the internal
structure of PostgreSQL databases on a PostgreSQL server. These client
programs can be located on the same machine with the PostgreSQL
server, or may be on a remote machine which accesses a PostgreSQL
server over a network connection. This package contains the client
libraries for C and C++, as well as command-line utilities for
managing PostgreSQL databases on a PostgreSQL server.

%prep
%build
%install
cp %{_postgresql_temp}/* %{buildroot}/ -R

%pre
# 如果升级模式，安装前先停止自己的服务
if [ $1 = 2 ]; then
    echo "update postgresql, postgresql will stop"
    service postgresql stop &>/dev/null
fi


%preun
# 如果是完全删除，要停止服务，再删除服务链接
if [ $1 = 0 ]; then
    service postgresql stop &>/dev/null
    if [ 0 -ne $? ]; then
        echo "failed to stop database server!"
        exit
    fi
    /sbin/chkconfig --del postgresql
fi


if [ -f /usr/local/pgsql/share/pg_manager.conf ];
then
  cp -f /usr/local/pgsql/share/recovery.done /usr/local/pgsql/share/recovery.done.bak
fi

%preun client

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
/usr/local/pgsql/bin
/usr/local/pgsql/include
/usr/local/pgsql/lib
/usr/local/pgsql/share
/usr/local/pgsql/postgresql.env
%config /etc/logrotate.d/postgresql
%files client
%defattr(-,root,root)
/usr/local/pgsql/bin
/usr/local/pgsql/include
/usr/local/pgsql/lib
/usr/local/pgsql/postgresql.env

%changelog

#安装之前需要先在/etc/ld.so.conf中加入/usr/local/pgsql/lib
%define ld_conf_file /etc/ld.so.conf.d/postgresql.conf
%post
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/pgsql/lib > %{ld_conf_file}
fi
/sbin/ldconfig  >/dev/null 2>&1
rm -f /etc/init.d/postgresql
ln -s /usr/local/pgsql/bin/pg_manager.sh /etc/init.d/postgresql
/sbin/chkconfig --add postgresql
if [ -f /usr/local/pgsql/share/pg_manager.conf ];
then
  if [ -f /usr/local/pgsql/share/recovery.done.bak ];
  then
    cp -f /usr/local/pgsql/share/recovery.done.bak /usr/local/pgsql/share/recovery.done
  fi
fi

if [ -f /usr/local/pgsql/share/backup_pgdb_cron ]; then
  cp -f /usr/local/pgsql/share/backup_pgdb_cron /etc/cron.daily/backup_pgdb_cron
fi

if ! getent group postgres > /dev/null 
then
   /usr/sbin/groupadd postgres
fi

if getent passwd postgresql > /dev/null 
then
  POSTGRES_HOME=`su postgresql -c "echo \$HOME"`
  if [ ! -e $POSTGRES_HOME ]; 
  then
   /usr/sbin/usermod -d /usr/local/pgsql postgresql
  fi
else
  /usr/sbin/useradd -m -c "PostgreSQL" -d /usr/local/pgsql -g postgres postgresql
  #Required for OpenSuse platforms
  /usr/sbin/usermod -p "*" postgresql
fi
passwd -d postgresql
chown postgresql:postgres /usr/local/pgsql

#update sshd_config file
file=/etc/ssh/sshd_config
key=PermitEmptyPasswords
value=yes
if [ -f $file ]; then
    exist=`grep "^[[:space:]]*[^#]" $file | grep -c "$key[[:space:]]* [[:space:]]*.*"`
    #注意：如果某行是注释，开头第一个字符必须是#号!!!
    comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]* [[:space:]]*.*"  $file`
    if [ $exist -gt 0 ];then
        #如果已经存在未注释的有效配置行，直接更新value
        sed  -i "/^[^#]/s#$key[[:space:]]* .*#$key $value#" $file
    elif [ $comment -gt 0 ];then
        #如果存在已经注释掉的对应配置行，则去掉注释，更新value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]* [[:space:]]*.*@$key $value@" $file
    else
        #否则在末尾追加有效配置行
        timestamp=`env LANG=en_US.UTF-8 date`
        writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key $value" >> $file
    fi
fi
service sshd restart

# 如果升级模式，启动服务
if [ $1 -eq 2 ]; then
    echo "update postgresql, postgresql will start"
    service postgresql start &>/dev/null
fi

%post client
if [ ! -e %{ld_conf_file}  ]; then
    touch %{ld_conf_file}
    echo /usr/local/pgsql/lib > %{ld_conf_file}
fi
/sbin/ldconfig  >/dev/null 2>&1

%postun
/sbin/ldconfig  >/dev/null 2>&1
# 如果是完全删除，删除链接
if [ $1 -eq 0 ]; then 
    rm -f /etc/init.d/postgresql
    rm -f /etc/cron.daily/backup_pgdb_cron
fi

%postun client
/sbin/ldconfig  >/dev/null 2>&1

