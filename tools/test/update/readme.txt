使用说明：
把整个update文件夹拷贝到linux机器上，执行./autoupdate就可以了；以后也只需要把新增的脚本拷贝到update目录下，再执行./autoupdate即可。

autoupdate           ----自动升级update目录下有更新的sh脚本，不需要再手动cp了。只需要每次把svn上的文件download下来，执行./autoupdate即可

Tecs_DailyCheck.tcl  ----沈毅写的那个CLI自动配置部署脚本
Tecs_DailyUpdate.tcl ----版本自动下载和安装脚本（安装时还是需要手动输入Y/N）

getVersion.sh        ----自动FTP获取版本，如：getVersion.sh 20120312 就可以自动下载20120312号版本到/home/tecs目录下
tecs_start.sh        ----增加了提示删除/dev/shm/tecs和/var/lib/tecs/instances下面的旧东西，防止安装出现异常
mylib.sh             ----公用库
starttecs.sh         ----新版本多了一个进程，因此更新了下，也兼容联调版本的启动
telnettecs.sh        ----自动telnet到tecs指定进程，如：telnettecs.sh tc


1.telnettecs.sh用法示例：
[root@Pwvir01 tecs]# telnettecs.sh tc

connecting active background tecs processes ...
telnet to and port is 45932 ...
Trying 127.0.0.1...
Connected to Pwvir01 (127.0.0.1).
Escape character is '^]'.
<7>2012/03/23 01:51:29:459 tecscloud tc: recv action reg req from Cluster_Manager_Agent-30156-20120323095129-378270
<7>2012/03/23 01:51:29:494 tecscloud tc: recv cluster reg ack from tecscluster
<7>2012/03/23 01:51:29:668 tecscloud tc: recv asyn cluster info req from Cluster_Manager_Agent-30158-20120323095129-663985
tc_shell-> 

如果不记得进程名，随便输入一个错误的参数，就会显示当前正在运行的进程名，然后copy即可：
[root@Pwvir01 tecs]# telnettecs.sh a
 
no running tecs process, or process name is error!

now running process!
hc
proc_version
apisvr
ftp_server
proc_version
proc_version_agent
tc_file_storage
cc
cc_file_storage
proc_version
[root@Pwvir01 tecs]# 


2.autoupdate用法示例：
第一次需要执行下，生成update.log日志
[root@Pwvir01 update]# ./autoupdate 

Congratulations! generate update.log and update all sh success!

以后执行就会自动更新发生变化的和新增的脚本
[root@Pwvir01 update]# ./autoupdate 

update mylib.sh success!
add new test1.sh success!


3.getVersion.sh用法示例：
默认下载el5分支版本
[root@Pwvir01 update]# ./getVersion.sh 20120326    

drwxr-xr-x    4 0        0            4096 Mar 26 10:36 20120326
start downloading version el5 ......
Failed to open file.
Congratulations! Update version successfully!
[root@Pwvir01 update]# 
如果想下载el6分支版本请输入第二个参数（分支版本名）
[root@Pwvir01 update]# ./getVersion.sh 20120326 el6

drwxr-xr-x    4 0        0            4096 Mar 26 10:36 20120326
start downloading version el6 ......
Failed to open file.
Congratulations! Update version successfully!
[root@Pwvir01 update]# cd ../20120326
[root@Pwvir01 20120326]# ll
total 117612
-rwxr-xr-- 1 root root 66654780 Mar 27 09:27 installtecs_el6_x86_64.bin
-rwxr-xr-- 1 root root   273764 Mar 27 09:27 openais-0.80.6-0.el6.x86_64.rpm
-rwxr-xr-- 1 root root  6080908 Mar 27 09:27 postgresql-8.4.8-1.el6.x86_64.rpm
-rwxr-xr-- 1 root root 24049836 Mar 27 09:27 qpidc-broker-0.14-1.el6.x86_64.rpm
-rwxr-xr-- 1 root root  8151228 Mar 27 09:27 qpidc-client-0.14-1.el6.x86_64.rpm
-rwxr-xr-- 1 root root  4910108 Mar 27 09:27 tecs-cc-1.1.10-0.1.1.el6.x86_64.rpm
-rwxr-xr-- 1 root root    16992 Mar 27 09:27 tecs-common-1.1.10-0.1.1.el6.x86_64.rpm
-rwxr-xr-- 1 root root    24952 Mar 27 09:27 tecs-guard-1.1.10-0.1.1.el6.x86_64.rpm
-rwxr-xr-- 1 root root  2781484 Mar 27 09:27 tecs-hc-1.1.10-0.1.1.el6.x86_64.rpm
-rwxr-xr-- 1 root root  7309248 Mar 27 09:27 tecs-tc-1.1.10-0.1.1.el6.x86_64.rpm

