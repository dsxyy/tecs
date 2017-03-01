#!/bin/sh
# ��ĳ���������������ң��Ժ�ssh��¼��ȥ����Ҫ����

#�������Ƿ�Ϸ�
ip=$1
if [ -z $ip ]; then
  echo "Usage: `basename $0` ipaddr" >&2
  exit
fi

#���ԶԶ��ܲ���ping��ͨ
unreachable=`ping $ip -c 1 -W 3 | grep -c "100% packet loss"`
if [ $unreachable -eq 1 ]; then
  echo "host $ip is unreachable!!!"
  exit
fi

#���������û��ssh��Կ��������һ��
if [ ! -e ~/.ssh/id_dsa.pub ]; then
  ssh-keygen -t dsa
fi

#�����ڶԶ�ɾ��ԭ����������ι�Կ
user=`whoami`
host=`hostname`
keyend="$user@$host"
echo "my keyend = $keyend"
cmd="sed '/$keyend$/d'  -i ~/.ssh/authorized_keys"
echo cmd:$cmd
echo "clear my old pub key on $ip ..."
ssh $ip "touch ~/.ssh/authorized_keys"
ssh $ip "$cmd"

#�������ɵĿ�����ȥ
echo "copy my public key to $ip ..."
tmpfile=/tmp/`hostname`.key.pub
scp ~/.ssh/id_dsa.pub  $ip:$tmpfile

#�ڶԶ˽���׷�ӵ�authorized_keys
echo "on $ip, append my public key to ~/.ssh/authorized_keys ..."
ssh $ip "cat $tmpfile >> ~/.ssh/authorized_keys"
echo "rm tmp file $ip:$tmpfile ..."
ssh $ip "rm $tmpfile" 






