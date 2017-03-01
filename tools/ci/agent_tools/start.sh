#/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
# �ڲ���Ŀ����ϱ������У������ڱ�����װ������tecs
# �˽ű���ci serverͨ��ssh�ķ�ʽ��Ŀ�����Զ��ִ��

[ -z $1 ] && echo "no tecs ci directory!" && exit -1
[ ! -z $1 ] && tecsdir=$1

#��������xend
#service xend restart
#[ 0 -ne $? ] && exit -1

# tecs web������Ŀǰ��APISVR������������ʹ��xmlrpc�Դ���abyss�����web rootĿ¼������
# �Ժ��������apache��Ϊweb���������������Ϳ��Ժ�����
[ ! -e /usr/local/abyss/wui ] && mkdir -p /usr/local/abyss/wui

#����tecs�����ļ��������еĿ���̨��ӡ�����ָ�����ļ��У����ڵ��Զ�λ����
tecs_output_dir=/dev/shm/tecsprint
[ ! -e $tecs_output_dir ] && mkdir -p $tecs_output_dir
tcexes=`awk -F' ' '{ print $1 }' /etc/tecs/tc.config`
for i in $tcexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/tc.config
done

ccexes=`awk -F' ' '{ print $1 }' /etc/tecs/cc.config`
for i in $ccexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/cc.config
done

hcexes=`awk -F' ' '{ print $1 }' /etc/tecs/hc.config`
for i in $hcexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/hc.config
done

service tecs-tc start
[ 0 -ne $? ] && exit -1
service tecs-cc start
[ 0 -ne $? ] && exit -1
service tecs-hc start
[ 0 -ne $? ] && exit -1

#����guard
service guard start
[ 0 -ne $? ] && exit -1

#�ȴ�һ��ʱ�䣬tecs���̶��ϵ�������ٷ���?
echo "tecs is started successfully!"
exit 0


