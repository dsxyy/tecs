#/bin/sh
# ���ߣ� ��Т��
# ��д���ڣ�2012.10.24
# �ڲ���Ŀ����ϱ������У������ڱ�����װ������tecs
# �˽ű���ci serverͨ��ssh�ķ�ʽ��Ŀ�����Զ��ִ��

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
    else
        #������ĩβ׷����Ч������
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key = $value" >> $file
    fi
}

config_mode=$1
config_file=""

# �ж��޸ĵ�ģ�������ļ�
if [[ $config_mode == "tc" ]]; then
config_file="/opt/tecs/tc/etc/tecs.conf"
elif [[ $config_mode == "cc" ]]; then
config_file="/opt/tecs/cc/etc/tecs.conf"
else
config_file="/opt/tecs/hc/etc/tecs.conf"
fi

# ���û�д���������������Ĭ�ϲ���
if [[ $2 == "" ]];then
exit 0
fi
key=$2

if [[ $3 == "" ]];then
exit 0
fi
value=$3

# �����������Ϣд����Ӧ�������ļ���
update_config $config_file "$key" "$value" 

exit 0


