#!/bin/sh
# ���ߣ� ����
# ��д���ڣ�2012.09.04

# get 'yes' or 'no' answer from user
function read_bool
{
    local prompt=$1
    local default=$2

    echo -e "$prompt (y/n? default: $default): \c "    
    read answer
    case $answer in
            "Y")
            answer="yes" ;;
            "y")
            answer="yes";;
            "n")
            answer="no";;
            "N")
            answer="no";;
            "")
            answer="$default";;
            *)
            echo "Please input y or n"
            read_bool;;
    esac
    echo ""
    return 0
}

service_dir="/etc/init.d"
if [ -x "${service_dir}/tecs-guard" ];then
#�ж�guard�����Ƿ��Ѿ��ر�
    service tecs-guard status >/dev/null
if [ 0 -eq $? ];then
    #ʹ��here document��ӡ��ʾ��
    cat <<-EOF
====================================================
Attention: 
    tecs-guard is running, it will restart the 
        tecs services in 5 seconds after you stop them.    
====================================================
EOF
    read_bool "do you want to stop tecs-guard?"  "yes"
        [ $answer == "yes" ] && service tecs-guard stop  
fi
fi
for i in `ls ${service_dir} |grep ^tecs-`
do
    [ "$i" != "tecs-guard" ] && service $i stop
done

echo "done!"
exit 0 