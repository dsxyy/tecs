#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26

# �ű�������������
source  ./common.sh

[ -z $4 ] && echo "no version no.!" && exit -1
version_no=$4

#���ذ汾��CI��������/home/tecs/$version_noĿ¼
version_home=/home/tecs/$version_no
[ ! -d $version_home ] && mkdir -p $version_home
cd $version_home
[ ! -e /home/tecs/$version_no/installtecs_el6_x86_64.bin ] && wget http://versioner:versioner@10.43.179.13/tecs_dev_build/$version_no/installtecs_el6_x86_64.bin
#./test/update/getVersion.sh $version_no $version_sub $version_branch
[ ! -e /home/tecs/$version_no/installtecs_el6_x86_64.bin ] && echo "auto download version failed!" && exit -1

#���汾���͵�����Ŀ���������ʱ����Ŀ¼agent_build_dir
echo "[ ! -d $agent_build_dir ] && mkdir -p $agent_build_dir"
ssh $target "[ ! -d $agent_build_dir ] && mkdir -p $agent_build_dir"
echo "scp /home/tecs/$version_no/* $target:$agent_build_dir"
scp /home/tecs/$version_no/* $target:$agent_build_dir
[ 0 -ne $? ] && echo "[error]send version to target failed!" && exit -1

#��ϵͳ������������Ŀ¼st_data���͵�����Ŀ�������
echo "[ ! -d /home/tecs/st_data ] && mkdir -p /home/tecs/st_data"
ssh $target "[ ! -d /home/tecs/st_data ] && mkdir -p /home/tecs/st_data"
echo "scp -r /home/tecs/st_data/* $target:/home/tecs/st_data"
scp -r /home/tecs/st_data/* $target:/home/tecs/st_data 
[ 0 -ne $? ] && echo "[error]send st_data to target failed!" && exit -1

#NFS����CI���������Զ����������ɹ���Ŀ¼
nfsshare=$workspace
#cat /etc/exports
#[ `exportfs | grep -c "$JOB_NAME"` -eq 0 ] && echo "$nfsshare *(rw,no_root_squash)" >> /etc/exports
#exportfs -rv
#killnfs.sh 

#Զ��Ŀ����Ի�mountӳ��������Ĺ���Ŀ¼
echo "[ ! -d $agent_script_dir ] && mkdir -p $agent_script_dir"
ssh $target "[ ! -d $agent_script_dir ] && mkdir -p $agent_script_dir"
#echo "current job name is $JOB_NAME"
#ssh $target "mount | grep $JOB_NAME"
#if [ $? -eq 0 ]; then
#  echo "umount -vl $agent_script_dir"
#  ssh $target "umount -vl $agent_script_dir"
#fi

#echo "mount -t nfs 10.44.129.110:$nfsshare  $agent_script_dir"
#ssh $target "mount -t nfs 10.44.129.110:$nfsshare  $agent_script_dir"
ssh $target "mount | grep $agent_script_dir"
if [ $? -eq 0 ]; then
   echo "umount $agent_script_dir"
   ssh $target "umount $agent_script_dir"
fi
echo "scp -r $nfsshare/* $target:$agent_script_dir"
scp -r $nfsshare/* $target:$agent_script_dir
[ 0 -ne $? ] && echo "[error]mount server job dir failed!" && exit -1

#����Զ��Ŀ����Ի���update
cd $job_bulid_dir/test
echo "[ ! -d /home/tecs/update ] && mkdir -p /home/tecs/update"
ssh $target "[ ! -d /home/tecs/update ] && mkdir -p /home/tecs/update"
echo "scp ./update/* to $target:/home/tecs/update"
scp ./update/* $target:/home/tecs/update
ssh $target "chmod +x /home/tecs/update/autoupdate"
echo "cd /home/tecs/update && ./autoupdate"
ssh $target "cd /home/tecs/update && ./autoupdate"

