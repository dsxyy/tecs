#!/bin/sh
# ���ߣ� ����
# ��д���ڣ�2012.08.27

# ����ִ��tecs������
# ./configtecs.sh  "cc"  "key"  "value"

[ -z "$1" ] && echo "no tecs config file!" && exit -1
[ -z "$2" ] && echo "no config key!" && exit -1
[ -z "$3" ] && echo "no config value!" && exit -1

case $1 in
    "tc")      file="/opt/tecs/tc/etc/tecs.conf" ;;
    "cc")      file="/opt/tecs/cc/etc/tecs.conf" ;;
    "hc")      file="/opt/tecs/hc/etc/tecs.conf";;
    "wui")     file="/opt/tecs/wui/htdocs/newweb/php/db_connect.ini" ;;
    *)    
    if [ -e $1 ];then
        file=$1
    else
        echo "unknown config file: $1!"
        exit -1
    fi    
esac

key=$2
#key�Ŀ�ͷ�����пո�
if [ "${key/ /}" != "$key" ];then
    echo "there is space in $key!"
    exit
fi 

value=$3

echo "=========================================="
echo "Now you are configing $file , please wait ..."
echo "=========================================="

[ ! -e $file ] && echo "config file not exist!" &&exit -1

#echo update key $key to value $value in file $file ...
exist=`grep "^[[:space:]]*[^#]" $file | grep -c "^[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"`
#ע�⣺���ĳ����ע�ͣ���ͷ��һ���ַ�������#��!!!
comment=`grep -c "^#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`

if [ $exist -gt 0 ];then
    #����Ѿ�����δע�͵���Ч�����У�ֱ�Ӹ���value
    sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key = $value#" $file
elif [ $comment -gt 0 ];then
    #��������Ѿ�ע�͵��Ķ�Ӧ�����У���ȥ��ע�ͣ�����value
    sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key = $value@" $file
else
    #������ĩβ׷����Ч������
    timestamp=`env LANG=en_US.UTF-8 date`
    writer=`whoami`
    echo "" >> $file
    echo "# added by $writer at $timestamp" >> $file
    echo "$key = $value" >> $file
fi
echo "done!"
exit

