【1】在/root/rpmbuild/SOURCES下新建文件夹BladeCrystal-1.0.0，将BladeCrystal.c、Makefile、start.sh拷贝到此文件夹下;
【2】到/root/rpmbuild/SOURCES下打包源码：tar -zcvf BladeCrystal-1.0.0.tar.gz BladeCrystal-1.0.0;
【3】将BladeCrystal.spec拷贝到/root/rpmbuild/SPECS下;
【4】到/root/rpmbuild/SPECS下执行：rpmbuild -ba BladeCrystal.spec，生成的二进制rpm包位于/root/rpmbuild/RPMS/x86_64下;
【5】到/root/rpmbuild/RPMS/x86_64下执行安装命令：rpm -ivh BladeCrystal-1.0.0-1.x86_64.rpm;
【6】安装完会在proc下生成zte文件夹，在zte下生成crystal文件;
【7】查看晶振状态的命令： cat /proc/zte/crystal;
【8】SBCJ单板上晶振状态查询结果：
/ # cat /proc/zte/crystal 
 1 : 25M              : OK        
 2 : RTC_32K          : OK 