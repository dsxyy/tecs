ʹ��˵����
������update�ļ��п�����linux�����ϣ�ִ��./autoupdate�Ϳ����ˣ��Ժ�Ҳֻ��Ҫ�������Ľű�������updateĿ¼�£���ִ��./autoupdate���ɡ�

autoupdate           ----�Զ�����updateĿ¼���и��µ�sh�ű�������Ҫ���ֶ�cp�ˡ�ֻ��Ҫÿ�ΰ�svn�ϵ��ļ�download������ִ��./autoupdate����

Tecs_DailyCheck.tcl  ----����д���Ǹ�CLI�Զ����ò���ű�
Tecs_DailyUpdate.tcl ----�汾�Զ����غͰ�װ�ű�����װʱ������Ҫ�ֶ�����Y/N��

getVersion.sh        ----�Զ�FTP��ȡ�汾���磺getVersion.sh 20120312 �Ϳ����Զ�����20120312�Ű汾��/home/tecsĿ¼��
tecs_start.sh        ----��������ʾɾ��/dev/shm/tecs��/var/lib/tecs/instances����ľɶ�������ֹ��װ�����쳣
mylib.sh             ----���ÿ�
starttecs.sh         ----�°汾����һ�����̣���˸������£�Ҳ���������汾������
telnettecs.sh        ----�Զ�telnet��tecsָ�����̣��磺telnettecs.sh tc


1.telnettecs.sh�÷�ʾ����
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

������ǵý��������������һ������Ĳ������ͻ���ʾ��ǰ�������еĽ�������Ȼ��copy���ɣ�
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


2.autoupdate�÷�ʾ����
��һ����Ҫִ���£�����update.log��־
[root@Pwvir01 update]# ./autoupdate 

Congratulations! generate update.log and update all sh success!

�Ժ�ִ�оͻ��Զ����·����仯�ĺ������Ľű�
[root@Pwvir01 update]# ./autoupdate 

update mylib.sh success!
add new test1.sh success!


3.getVersion.sh�÷�ʾ����
Ĭ������el5��֧�汾
[root@Pwvir01 update]# ./getVersion.sh 20120326    

drwxr-xr-x    4 0        0            4096 Mar 26 10:36 20120326
start downloading version el5 ......
Failed to open file.
Congratulations! Update version successfully!
[root@Pwvir01 update]# 
���������el6��֧�汾������ڶ�����������֧�汾����
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

