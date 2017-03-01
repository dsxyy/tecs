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

wui_cfg_file_save=$1/db_connect.ini.rpmsave
wui_cfg_file=$1/db_connect.ini
wui_cfg_file_savebak=$1/db_connect.ini.rpmsave.bak
[ ! -e $wui_cfg_file_save ] && exit
#echo "$wui_cfg_file_save exist"
[ ! -e $wui_cfg_file ] && exit
#echo "$wui_cfg_file doesn't exist"
tmp=$IFS
IFS=$'\n'
for item in `cat $wui_cfg_file_save | grep -v "^[[:space:]]*#" | grep "="`;
do
    cfg_name=`echo "$item"|awk -F '=' '{print$1}'|awk -F ' ' '{print$1}'`
    cfg_value=${item#*=}
    #echo "update $cfg_name:$cfg_value"
    update_config  $wui_cfg_file $cfg_name $cfg_value
done
mv -f $wui_cfg_file_save $wui_cfg_file_savebak
echo "warning:$wui_cfg_file_save saved as $wui_cfg_file_savebak"
IFS=tmp