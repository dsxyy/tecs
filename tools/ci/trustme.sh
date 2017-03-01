#!/bin/sh
# 让某个主机彻底信任我，以后ssh登录过去不需要密码

#检查参数是否合法
ip=$1
if [ -z $ip ]; then
  echo "Usage: `basename $0` ipaddr" >&2
  exit
fi

#试试对端能不能ping得通
unreachable=`ping $ip -c 1 -W 3 | grep -c "100% packet loss"`
if [ $unreachable -eq 1 ]; then
  echo "host $ip is unreachable!!!"
  exit
fi

#如果本机还没有ssh公钥，就生成一个
if [ ! -e ~/.ssh/id_dsa.pub ]; then
  ssh-keygen -t dsa
fi

#首先在对端删除原来保存的信任公钥
user=`whoami`
host=`hostname`
keyend="$user@$host"
echo "my keyend = $keyend"
cmd="sed '/$keyend$/d'  -i ~/.ssh/authorized_keys"
echo cmd:$cmd
echo "clear my old pub key on $ip ..."
ssh $ip "touch ~/.ssh/authorized_keys"
ssh $ip "$cmd"

#把新生成的拷贝过去
echo "copy my public key to $ip ..."
tmpfile=/tmp/`hostname`.key.pub
scp ~/.ssh/id_dsa.pub  $ip:$tmpfile

#在对端将其追加到authorized_keys
echo "on $ip, append my public key to ~/.ssh/authorized_keys ..."
ssh $ip "cat $tmpfile >> ~/.ssh/authorized_keys"
echo "rm tmp file $ip:$tmpfile ..."
ssh $ip "rm $tmpfile" 






