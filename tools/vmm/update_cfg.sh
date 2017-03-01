#!/bin/bash
#update key = value config option in an ini file
function update_config
{
    local file=$1
    local key=$2
    local value=$3

    [ ! -e $file ] && return

    #echo update key $key to value $value in file $file ...
    local exist=`grep "^[[:space:]]*[^#]" $file | grep -c "$key[[:space:]]*=[[:space:]]*.*"`
    #ע�⣺���ĳ����ע�ͣ���ͷ��һ���ַ�������#��!!!
    local comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`
    if [ $exist -gt 0 ];then
        #����Ѿ�����δע�͵���Ч�����У�ֱ�Ӹ���value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key = $value#" $file
    elif [ $comment -gt 0 ];then
        #��������Ѿ�ע�͵��Ķ�Ӧ�����У���ȥ��ע�ͣ�����value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key = $value@" $file
#    else
#        echo "can't $exist $comment in $file $key"
    fi
}

tecs_cfg_file_save=$1/tecs.conf.rpmsave
tecs_cfg_file=$1/tecs.conf
tecs_cfg_file_savebak=$1/tecs.conf.rpmsave.bak
[ ! -e $tecs_cfg_file_save ] && exit
#echo "$tecs_cfg_file_save exist"
[ ! -e $tecs_cfg_file ] && exit
#echo "$tecs_cfg_file doesn't exist"
tmp=$IFS
IFS=$'\n'
for item in `cat $tecs_cfg_file_save | grep -v "^[[:space:]]*#" | grep "="`;
do
    cfg_name=`echo "$item"|awk -F '=' '{print$1}'|awk -F ' ' '{print$1}'`
    cfg_value=${item#*=}
    #echo "update $cfg_name:$cfg_value"
    update_config  $tecs_cfg_file $cfg_name $cfg_value
done
mv -f $tecs_cfg_file_save $tecs_cfg_file_savebak
echo "warning:$tecs_cfg_file_save saved as $tecs_cfg_file_savebak"
IFS=tmp