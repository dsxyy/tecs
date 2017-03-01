#/bin/sh
# 作者： 李孝成
# 编写日期：2012.10.24
# 在测试目标机上本地运行，用于在本机安装、启动tecs
# 此脚本由ci server通过ssh的方式在目标机的远程执行

#update key = value config option in an ini file
function update_config
{
    local file=$1
    local key=$2
    local value=$3

    [ ! -e $file ] && return

    #echo update key $key to value $value in file $file ...
    local exist=`grep "^[[:space:]]*[^#]" $file | grep -c "$key[[:space:]]*=[[:space:]]*.*"`
    #注意：如果某行是注释，开头第一个字符必须是#号!!!
    local comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`

    if [ $exist -gt 0 ];then
        #如果已经存在未注释的有效配置行，直接更新value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key = $value#" $file
    elif [ $comment -gt 0 ];then
        #如果存在已经注释掉的对应配置行，则去掉注释，更新value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key = $value@" $file
    else
        #否则在末尾追加有效配置行
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key = $value" >> $file
    fi
}

config_mode=$1
config_file=""

# 判断修改的模块配置文件
if [[ $config_mode == "tc" ]]; then
config_file="/opt/tecs/tc/etc/tecs.conf"
elif [[ $config_mode == "cc" ]]; then
config_file="/opt/tecs/cc/etc/tecs.conf"
else
config_file="/opt/tecs/hc/etc/tecs.conf"
fi

# 如果没有带其他参数就设置默认参数
if [[ $2 == "" ]];then
exit 0
fi
key=$2

if [[ $3 == "" ]];then
exit 0
fi
value=$3

# 否则把配置信息写到对应的配置文件中
update_config $config_file "$key" "$value" 

exit 0


