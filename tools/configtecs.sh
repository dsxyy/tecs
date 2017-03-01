#!/bin/sh
# 作者： 高明
# 编写日期：2012.08.27

# 本地执行tecs的配置
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
#key的开头不能有空格
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
#注意：如果某行是注释，开头第一个字符必须是#号!!!
comment=`grep -c "^#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`

if [ $exist -gt 0 ];then
    #如果已经存在未注释的有效配置行，直接更新value
    sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key = $value#" $file
elif [ $comment -gt 0 ];then
    #如果存在已经注释掉的对应配置行，则去掉注释，更新value
    sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key = $value@" $file
else
    #否则在末尾追加有效配置行
    timestamp=`env LANG=en_US.UTF-8 date`
    writer=`whoami`
    echo "" >> $file
    echo "# added by $writer at $timestamp" >> $file
    echo "$key = $value" >> $file
fi
echo "done!"
exit

