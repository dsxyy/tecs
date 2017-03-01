#!/bin/bash
# ���ȫ�ֹ��ú��������ܻᱻ���������ű�����

#��ֹ�ű��ظ�������
if [ ! "$_TECS_COMMON_FUNC_FILE" ];then

#######################�ʴ𽻻���ػ�������############################
# get 'yes' or 'no' answer from user
function read_bool
{
    local prompt=$1
    local default=$2

    [[ $prompt != "" ]] && echo -e "$prompt (y/n? default: $default): \c "
    read answer
    [[ ! -z `echo $answer |grep -iwE "y|yes"` ]] && answer=yes
    [[ ! -z `echo $answer |grep -iwE "n|no"` ]] && answer=no
    case $answer in
            "yes")
            answer="yes";;
            "no")
            answer="no";;
            "")
            answer="$default";;
            *)
            echo "Please input y or n"
            read_bool "$prompt" "$default";;
            
    esac
    return 0
}

#get common string answer from user
function read_string
{
    local prompt=$1
    local default=$2

    [[ $prompt != "" ]] && echo -e "$prompt (default: $default): \c "
    read answer
    [ -z "$answer" ] && answer="$default"
}

function read_string_input_null_check
{
    read_string "$1" "$2"
    if [[ $answer == "" ]];then
        read_string_input_null_check "$1" "$2"
    fi
}

# ��ȡ��ȡһ��IP�б�Ĺ���
function read_iplist
{
    local prompt=$1
    local ip_list=$2
    local recommend=$3
    if [[ $prompt != "" ]];then
        echo -e "$prompt"
        echo -e "local ip list:"
        echo -e "$ip_list"
        echo -e "(recommend: $recommend): \c "
    fi 
    read answer
    [ -z $answer ] && answer="$recommend"
}

#######################���ö�д��ػ�������############################

function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return
    #���Ծ��ſ�ͷ��ע�����Լ�����֮����grep����"key"���ڵ���
    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep -w "$key"| grep "$key[[:space:]]*=" -m1`
    if [ -z "$line" ]; then
        config_answer=""
    else
        #����һ��=���滻Ϊ�ո���ɾ����һ�����ʵõ�value
        config_answer=`echo $line | sed 's/=/ /' | sed -e 's/^\w*\ *//'`
    fi
}

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
    
    if [[ $value == "#" ]];then
        if [ $exist -gt 0 ];then
            sed  -i "/^[^#]/s/$key[[:space:]]*=/\#$key=/" $file       
        fi
        return
    fi

    if [ $exist -gt 0 ];then
        #����Ѿ�����δע�͵���Ч�����У�ֱ�Ӹ���value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key=$value#" $file
        
    elif [ $comment -gt 0 ];then
        #��������Ѿ�ע�͵��Ķ�Ӧ�����У���ȥ��ע�ͣ�����value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key=$value@" $file
    else
        #������ĩβ׷����Ч������
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key=$value" >> $file
    fi
}
#��ȡ�ļ���string������
function get_string_num
{
    local file=$1
    local string=$2

    [ ! -e $file ] && { echo "$file doesn't exist."; exit 1; }
    #���Ծ��ſ�ͷ��ע�����Լ�����֮����grep����"key"���ڵ���
    string_num=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep -cw "$string"`
}

#����������Ϣ,ָ�������ļ���װ��ʽ��ж�ز���Ҫ����
#���ñ�����ʱӦ��֤ͬһģ������ò���һ���������꣬�粻�������ò���tc��Ȼ������cc���ֻ�ͷ����tc
function user_config_save
{  
    local component="$1"
    local key="$2"
    local value="$3"

    if [ "$operation" = "install" ];then
        #����û������ļ��Ѿ������򱸷ݾɵ�����
        if [ ! -f $user_config_save_file ];then
            mkdir -p ${user_config_save_path}     
            touch $user_config_save_file 
            echo -e "## tecs installation configure file at ${current_time}" >$user_config_save_file
            echo -e "## you can edit it and install tecs by conf_file option, as \"./installtecs_(arch).bin conf_file /home/tecs_install/user_install.conf\"\n" >>$user_config_save_file
            echo -e "## global configration section\nmode=$mode\n">>$user_config_save_file
            echo -e "## component configration section">>$user_config_save_file
        fi
        # �����û����������������mode��
        if [ "$component" = "" ];then
            [ "$key" = "" ] && { echo -e "\nkey is null, please check!"; exit 1; }
            sed -i "/mode=$mode/a $key=$value" $user_config_save_file
        else        
            [ -z "`cat $user_config_save_file |grep -w "\[$component\]"`" ] && echo -e "\n[$component]" >>$user_config_save_file
            [[ $key != "" ]] && echo "$key=$value" >>$user_config_save_file
        fi
    fi
}
# �˺������ڻ�ȡ����Ƿ�װ���ж�,Ĭ���Ƕ���װ��
function get_install_component
{ 
    local component=$1
    local prompt=$2
    local component_exist=""

    if [[ "$operation" = "install" ]];then        
        if [ "$mode" = "custom" ];then
            # custom��װ��ʽ�����ָ�������ļ�����ģ�飬���Զ���װ��û���򽻻�
            [ "$user_specify_config" != "" ] && component_exist=`cat $user_specify_config | grep "\[$component\]"`
            if [ "$component_exist" != "" ];then
                config_answer="yes" 
                echo -e "$prompt (y/n? default: yes):$config_answer\n"
            else 
                read_bool "$prompt" "yes"
                config_answer=$answer
            fi
        else
            #��costomģʽֻҪ���õ��ú�������Ϊ�ǰ�װ��ģ��
            config_answer="yes"
            echo -e "$prompt (y/n? default: yes):$config_answer\n"
        fi 
           
        #����ǰ���ñ�������
        [[ $config_answer = "yes" ]] && user_config_save "$component"
    fi  
}

# �������ļ��л�ȡָ��ģ���е����ò���
function get_component_config
{
    local component="$1"
    local file="$2"
    local key="$3"
    
    config_answer=""

    #�����ļ������ڣ�����
    [ ! -f "$file" ] && { echo -e "\n$file not exist."; exit 1; }
    [ -z "$component" ] && { echo -e "\ncomponent doesn't specified in get_install_config function"; exit 1; }
    get_string_num "$file" "\[$component\]"
    #δ���ø�ģ��
    [ "$string_num" -eq 0 ] && return 0
    #ģ�����ù��࣬����
    [ "$string_num" -gt 1 ] && { echo -e "\nthere are too many \"[$component]\" in $file"; exit 1; }

    #��[$component]��ʼ����һ�����֮�俪ʼƥ��"key="
    local key_num=`cat $file |sed '/^[[:space:]]*#/d' |sed -n "/\[$component\]/,/\[*\]/p" |grep -w "$key[[:space:]]*" |grep -c "$key[[:space:]]*="`
    #ģ����û�����ø�key
    [ "$key_num" -eq 0 ] && return 0
    #һ��key�ж��ֵ������
    [ "$key_num" -gt 1 ] && { echo -e "\nthere are too many \"$key\" for $component in $file"; exit 1; }
    
    local key_line=`cat $file|sed '/^[[:space:]]*#/d' |sed -n "/\[$component\]/,/\[*\]/p"|grep -w "$key[[:space:]]*"|grep "$key[[:space:]]*="`

    if [ "$key_line" != "" ];then
        config_answer=`echo $key_line |awk -F'=' '{print $2}'`
    else
        echo -e "\nthere is no line inclued \"$key=\" for $component in $file"
        exit 1
    fi
    return 0
}

function get_global_config
{
    local file="$1"
    local key="$2"
    
    config_answer=""

    #�����ļ������ڣ�����
    [ ! -f "$file" ] && { echo -e "\n$file not exist."; exit 1; }

    local key_num=`cat $file |sed '/^[[:space:]]*#/d' |sed -n "1,/\[*\]/p" |grep -w "$key[[:space:]]*" |grep -c "$key[[:space:]]*="`
    #ģ����û�����ø�key
    [ "$key_num" -eq 0 ] && return 0
    #һ��key�ж��ֵ������
    [ "$key_num" -gt 1 ] && { echo -e "\nthere are too many \"$key\" for $component in $file"; exit 1; }
    
    local key_line=`cat $file|sed '/^[[:space:]]*#/d' |sed -n "1,/\[*\]/p"|grep -w "$key[[:space:]]*"|grep "$key[[:space:]]*="`

    if [ "$key_line" != "" ];then
        config_answer=`echo $key_line |awk -F'=' '{print $2}'`
    else
        echo -e "\nthere is no line inclued \"$key=\" for $component in $file"
        exit 1
    fi
    return 0
}
# ���ڻ�ȡָ��ģ���µ���������
# value��ȡ���ȼ����û�ָ���������ļ�>tecs��װ�����Դ��������ļ�>����
# ���������1. custom��ʽ�����κ������ļ���ȡvalue��
#           2. ָ�������ļ�������������default��������ָ�������ļ���ѯ����valueʱ���Զ�ʹ�ýű��е�Ĭ��ֵ.
function get_install_config
{   
    local component=$1
    local key=$2
    local type=$3
    local prompt="$4"
    local default_value="$5"
    # ���ip_list�Ǹ�typeΪiplistʱ�õģ�����Ϊ��
    local ip_list="$6"
    
    config_value=""
    
    [ -z "$key" ] && { echo -e "\nkey doesn't specified in get_install_config function"; exit 1; }
    
    #������øú���ʱ��Ĭ��ֵΪ�գ����tecs�Դ��������ļ��л�ȡĬ��ֵ
    if [ "$default_value" = "" ];then
        if [ -z "$component" ];then
            get_global_config "$tecs_publish_config" "$key"
        else            
            get_component_config "$component" "$tecs_publish_config" "$key"
        fi
        default_value="$config_answer"
    fi
    
    # 1. ���û�ָ�������ļ��л�ȡ����
    if [ "$user_specify_config" != "" ];then
         if [ -z "$component" ];then
              get_global_config "$user_specify_config" "$key"
         else            
              get_component_config "$component" "$user_specify_config" "$key"
         fi
         config_value="$config_answer"
    fi
   
    # 2. mode!=customʱ��ʹ��tecs�ṩ��ϵͳĬ��ֵ
    if [ "$config_value" = "" ];then
        if [ "$operation" = "install" -a "$mode" != "custom"  ];then
             config_value="$default_value"
        fi
    fi

    #3. �˹�������ִ�е��˲��ĳ�����
    #  1) ͨ���˵�ѡ��Ϊcustom��ʽ��װ;
    #  2) �û�ָ���������ļ��а�װģʽΪcustom����δ���ļ������õĲ���.
    if [ "$config_value" = "" ];then
        case $type in
        "string")
            read_string "$prompt" "$default_value"
            ;; 
        "bool")
            read_bool "$prompt" "$default_value"
            ;;
        "iplist")
            read_iplist "$prompt" "$ip_list" "$default_value"
            ;;
        *)
            echo "unknown type in get_install_config: $type!"
            exit 1
            ;;         
        esac
        config_value="$answer"
    else
        #��������ǽ���ģʽ�µ���ʾ��ӡ
        if [ "$type" = "iplist" ];then
            echo -e "$prompt\nlocal ip list:\n$local_ip_list\n(recommend: $default_value): $config_value"
        elif [ "$type" = "bool" ];then
            echo -e "$prompt (y/n? default: $default_value): $config_value"
        else
            echo -e "$prompt (default: $default_value): $config_value"
        fi
    fi

    #����ǰ���ñ�������
    user_config_save "$component" "$key" "$config_value"
}


#######################rpm��������ػ�������############################

#�ж�ĳrpm���Ƿ��Ѱ�װ
function check_installed
{
    has_installed="no"
    
    rpm -qi $1 &>/dev/null
    
    if [ 0 == $? ];then
        has_installed="yes"
    fi    
}

# ���rpm���Ƿ�����
function check_depend
{
    local rpm_name=$1
    # ����������Ƿ񱻱���ʹ��
    rpm -q --whatrequires $rpm_name &>/dev/null
    # ����ѯ�����������Ĺ�ϵ��rpmδ��װ�����ص���1������Ϊ0
    return "$?"
}

# ��װ���ĺ���
function install_rpm_by_yum
{
    local rpm_name=$1
    
    [[ $tecs_yum = "" ]] && { echo "yum repository doesn't create!"; exit 1; }
    $tecs_yum install $rpm_name
    local result=$?
    if [ $result -ne 0 ];then
        echo -e "\ninstall $rpm_name failed!"
        exit $result
    fi    
}

# ���Ҫ��װ�İ��Ƿ���ڣ���������ڣ�����ʾ�Ƿ���Ҫ��װ
function check_and_install_rpm
{
    local rpm_name=$1
    check_installed $rpm_name
    if [[ "$has_installed" != "yes" ]];then
        echo "$rpm_name not installed, need to install"
        install_rpm_by_yum $rpm_name
        return 0
    else
        echo "$rpm_name has installed"
        return 1
    fi
}

# ���Ҫ��װ�İ��Ƿ���ڣ���������ڣ�����ʾ�Ƿ���Ҫ��װ��������ڣ�����Ҫ����
function check_and_install_or_upgrade_rpm
{
    local rpm_name=$1
    check_installed $rpm_name
    if [[ "$has_installed" != "yes" ]];then
        echo "$rpm_name not installed, need to install"
        install_rpm_by_yum $rpm_name
        return 0
    else
        echo "$rpm_name has installed"
        check_app_is_upgrade $rpm_name
        if [[ "$is_update" = "yes" ]];then
            echo "$rpm_name need to update ..."
            upgrade_rpms_by_yum $rpm_name
        fi
        return 1
    fi
}

# ����Ƿ�Э��RPM��������Ѿ���װ���ˣ���ʾ�Ƿ�Ҫж��
function check_uninstall_tecs_rpm
{
    local rpm_name=$1
    check_installed "$rpm_name"
    if [[ "$has_installed" == "no" ]];then
        return 1
    fi
    
    read_bool "$rpm_name already installed, remove it?"  "no"
    if [ $answer == "yes" ]; then
        service_stop $rpm_name
        return 0
    fi
    return 1
}

# ж�ذ��ĺ���,��ж��tecs��rpm������������ж�ز�Ҫ��,��ʱȥ��
function uninstall_rpm_by_yum_delete
{
    local rpm_name=$1
    # ��ȡ������
    local pkgname=`rpm -qR $rpm_name | grep -v / | grep -v "(" | awk -F ' ' '{print$1}'`
    rpm -qR $rpm_name &>/dev/null
    local check_result=$?
    
    # ��ж���Լ���RPM����Ȼ���ټ��������������Ƿ����ж��
    yum -y --disablerepo=* --enablerepo=$repo_name remove  $rpm_name
    
    # ������������������������ж��
    if [ $check_result -eq 0 ];then
        for i in $pkgname
        do
            # �����xen����libvrit��rpm������ж��    
            if [ `echo "$i"|grep -c "^libvirt"` -ne 0 ] ||
               [ `echo "$i"|grep -c "openvswitch"` -ne 0 ] ||                                          
               [ `echo "$i"|grep -c "^xen"` -ne 0 ];then  
                continue
            fi  
            # ����������Ƿ񱻱���ʹ��
            rpm -q --whatrequires $i &>/dev/null
            # ����ѯ�����������Ĺ�ϵ�����ص���ʧ��
            if [ $? -ne 0 ];then
                # ����ж�ظ�������
                yum -y --disablerepo=* --enablerepo=$repo_name erase  $i
            fi
        done
    fi
}

# ж�ص�������
function uninstall_rpm_by_yum
{
    local rpm_name=$1
    
    # ж�ذ�
    yum -y --disablerepo=* --enablerepo=$repo_name remove  $rpm_name

}

# ���ָ�����Ƿ���Ҫ����
function check_app_is_upgrade
{ 
    local app=$1
    is_update="yes"

    num=`$tecs_yum list updates | awk -F ' ' '{print$1}' | grep $app -c`
    if [[ $num == 0 ]];then
        is_update="no"
    fi
}

# �������ĺ���
function upgrade_rpms_by_yum
{
    local app_list="$1"
    
    if [ "$app_list" = "" ];then
        echo -e "\nsorry, there is no rpm need to upgrade"!
        exit 0
    fi    
    #�˴������app_list��Ϊupgrade�Ĳ����������һ������Ҫ�������ѱ�����
    #����tecs���񣬻ᵼ����������ʧ�ܣ��������дΪ��ÿ�����񵥶��ж��Ƿ�����
    [ "$tecs_yum" = "" ] && { echo "tecs yum doesn't set, update rpms failed!"; exit 1; }
    for app in $app_list
    do
        check_app_is_upgrade "$app"
        if [[ "$is_update" == "yes" ]];then             
            echo -e "\n$app will upgrade..."            
            $tecs_yum upgrade $app                
            local result=$?
            if [ $result -ne 0 ];then
                echo -e "\nupgrade $app failed,return $result"!
                exit $result
            fi 
        else
            echo -e "\n$app don't need to upgrade"!
        fi
    done    
}

#update_libvirt
function update_libvirt
{
    if [ ! -e /proc/xen ]; then
        return;
    fi
    
    check_app_is_upgrade "libvirt"
    if [[ "$is_update" = "yes" ]];then
        echo "libvirt need to update ..."
        upgrade_rpms_by_yum "libvirt-client libvirt-daemon libivrt-devel libvirt"
    fi
}
#######################��������ػ�������############################
# ֹͣ����
function service_stop
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
		/etc/init.d/$app stop >/dev/null 
		count=`/etc/init.d/$app status|grep -c stop`
	    if [ $count -eq 0 ]; then
			echo "$app can not be stopped"
		fi
	fi
}

# �����������
function service_start
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
        /etc/init.d/$app start >/dev/null
		count=`/etc/init.d/$app status|grep -c running`
	    if [ $count -eq 0 ]; then
			echo "$app can not be started"
		fi
    fi
}

# �������������
function service_restart
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
        cd /
        /etc/init.d/$app restart >/dev/null
        cd - >/dev/null
    fi
}

# ����ѯ�ʽ����ֹͣ����
function ask_service_stop
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
        read_bool  "Stop service $app?" "yes"
        [ "$answer" == "yes" ] && /etc/init.d/$app stop >/dev/null 
    fi
}

# ����ѯ�ʽ������������
function ask_service_start
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
        read_bool  "Start service $app?" "yes"
        if [ "$answer" == "yes" ]; then
            cd /
            /etc/init.d/$app start >/dev/null
            cd - >/dev/null
        fi
    fi
}

# �Զ�ֹͣ���з���
function stop_service_all
{
    service_stop  "tecs-guard"
    service_stop  "nicchk"
    service_stop  "tecs-tc"
    service_stop  "tecs-cc"
    service_stop  "tecs-hc"
    service_stop  "tecs-sd"
    service_stop  "tecs-sa"
    service_stop  "tecs-vna"
    service_stop  "tecs-vnm"
    service_stop  "tecs-wdg"
    service_stop  "tgtd"
}

# �Զ������з���
function start_service_all
{
    service_start  "tgtd"
    service_start  "tecs-wdg"
    service_start  "tecs-vnm"
    service_start  "tecs-vna"
    service_start  "tecs-sa"
    service_start  "tecs-sd"
    service_start  "tecs-hc"
    service_start  "tecs-cc"
    service_start  "tecs-tc"
    service_start  "nicchk"    
    service_start  "tecs-guard"
}

# ��Ⲣѯ���Ƿ���Ǹ�����
function ask_service_start_all
{
    [ -e /etc/init.d/xend ] && service xend start &>/dev/null
    [ -e /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    ask_service_start  "tecs-tc"
    ask_service_start  "tecs-cc"
    ask_service_start  "tecs-hc"
    ask_service_start  "tecs-sd"
    ask_service_start  "tecs-sa"
    ask_service_start  "tecs-wui"
    [ -e /etc/init.d/openvswitch ] && service openvswitch start &>/dev/null     
    ask_service_start  "tecs-vna"
    ask_service_start  "tecs-vnm"
    ask_service_start  "tecs-wdg"
    ask_service_start  "nicchk"    
    ask_service_start  "tecs-guard"
}

#######################gurd���############################
#guard��غ�������������ʵ������������Ϊ��qpid���������ã����ò���ʱ��������
# ����guard����б�
function please_guard_me
{
    local service=$1
    local tecs_guard_dir=/etc/tecs/guard_list
    [ ! -x /etc/init.d/"$service" ] && return 0
    [ ! -d $tecs_guard_dir ] && mkdir -p $tecs_guard_dir

    #���������guard����б�
    touch $tecs_guard_dir/$service
}

# ��guard����ļ����޳�
function dont_guard_me
{
    local service=$1
    local tecs_guard_dir=/etc/tecs/guard_list
    [ ! -d $tecs_guard_dir ] && return 0
    
    #�������Ƴ�guard����б�
    rm -f $tecs_guard_dir/$service
}

_TECS_COMMON_FUNC_FILE="common_func.sh"
fi 