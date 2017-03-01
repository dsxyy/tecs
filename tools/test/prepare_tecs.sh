#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26

# 脚本启动参数处理
source  ./common.sh

[ -z $4 ] && echo "no version no.!" && exit -1
version_no=$4

#下载版本到CI服务器的/home/tecs/$version_no目录
version_home=/home/tecs/$version_no
[ ! -d $version_home ] && mkdir -p $version_home
cd $version_home
[ ! -e /home/tecs/$version_no/installtecs_el6_x86_64.bin ] && wget http://versioner:versioner@10.43.179.13/tecs_dev_build/$version_no/installtecs_el6_x86_64.bin
#./test/update/getVersion.sh $version_no $version_sub $version_branch
[ ! -e /home/tecs/$version_no/installtecs_el6_x86_64.bin ] && echo "auto download version failed!" && exit -1

#将版本发送到测试目标机器的临时工作目录agent_build_dir
echo "[ ! -d $agent_build_dir ] && mkdir -p $agent_build_dir"
ssh $target "[ ! -d $agent_build_dir ] && mkdir -p $agent_build_dir"
echo "scp /home/tecs/$version_no/* $target:$agent_build_dir"
scp /home/tecs/$version_no/* $target:$agent_build_dir
[ 0 -ne $? ] && echo "[error]send version to target failed!" && exit -1

#将系统测试所需数据目录st_data发送到测试目标机器上
echo "[ ! -d /home/tecs/st_data ] && mkdir -p /home/tecs/st_data"
ssh $target "[ ! -d /home/tecs/st_data ] && mkdir -p /home/tecs/st_data"
echo "scp -r /home/tecs/st_data/* $target:/home/tecs/st_data"
scp -r /home/tecs/st_data/* $target:/home/tecs/st_data 
[ 0 -ne $? ] && echo "[error]send st_data to target failed!" && exit -1

#NFS共享CI服务器端自动化持续集成工作目录
nfsshare=$workspace
#cat /etc/exports
#[ `exportfs | grep -c "$JOB_NAME"` -eq 0 ] && echo "$nfsshare *(rw,no_root_squash)" >> /etc/exports
#exportfs -rv
#killnfs.sh 

#远程目标测试机mount映像服务器的工作目录
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

#更新远程目标测试机的update
cd $job_bulid_dir/test
echo "[ ! -d /home/tecs/update ] && mkdir -p /home/tecs/update"
ssh $target "[ ! -d /home/tecs/update ] && mkdir -p /home/tecs/update"
echo "scp ./update/* to $target:/home/tecs/update"
scp ./update/* $target:/home/tecs/update
ssh $target "chmod +x /home/tecs/update/autoupdate"
echo "cd /home/tecs/update && ./autoupdate"
ssh $target "cd /home/tecs/update && ./autoupdate"

