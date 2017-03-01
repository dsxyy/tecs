#!/bin/bash
# 最高全局公用函数，可能会被所有其他脚本调用

#防止脚本重复被包含
if [ ! "$_TECS_COMMON_FUNC_FILE" ];then

#######################问答交互相关基本函数############################
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

# 获取读取一个IP列表的功能
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

#######################配置读写相关基本函数############################

function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return
    #忽略井号开头的注释行以及空行之后再grep过滤"key"所在的行
    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep -w "$key"| grep "$key[[:space:]]*=" -m1`
    if [ -z "$line" ]; then
        config_answer=""
    else
        #将第一个=号替换为空格，再删除第一个单词得到value
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
    #注意：如果某行是注释，开头第一个字符必须是#号!!!
    local comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`
    
    if [[ $value == "#" ]];then
        if [ $exist -gt 0 ];then
            sed  -i "/^[^#]/s/$key[[:space:]]*=/\#$key=/" $file       
        fi
        return
    fi

    if [ $exist -gt 0 ];then
        #如果已经存在未注释的有效配置行，直接更新value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key=$value#" $file
        
    elif [ $comment -gt 0 ];then
        #如果存在已经注释掉的对应配置行，则去掉注释，更新value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key=$value@" $file
    else
        #否则在末尾追加有效配置行
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key=$value" >> $file
    fi
}
#获取文件中string的数量
function get_string_num
{
    local file=$1
    local string=$2

    [ ! -e $file ] && { echo "$file doesn't exist."; exit 1; }
    #忽略井号开头的注释行以及空行之后再grep过滤"key"所在的行
    string_num=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep -cw "$string"`
}

#保存配置信息,指定配置文件安装方式和卸载不需要保存
#调用本函数时应保证同一模块的配置参数一次性配置完，如不能先配置部分tc，然后配置cc，又回头配置tc
function user_config_save
{  
    local component="$1"
    local key="$2"
    local value="$3"

    if [ "$operation" = "install" ];then
        #如果用户配置文件已经存在则备份旧的配置
        if [ ! -f $user_config_save_file ];then
            mkdir -p ${user_config_save_path}     
            touch $user_config_save_file 
            echo -e "## tecs installation configure file at ${current_time}" >$user_config_save_file
            echo -e "## you can edit it and install tecs by conf_file option, as \"./installtecs_(arch).bin conf_file /home/tecs_install/user_install.conf\"\n" >>$user_config_save_file
            echo -e "## global configration section\nmode=$mode\n">>$user_config_save_file
            echo -e "## component configration section">>$user_config_save_file
        fi
        # 如果，没有组件归属，则放在mode后
        if [ "$component" = "" ];then
            [ "$key" = "" ] && { echo -e "\nkey is null, please check!"; exit 1; }
            sed -i "/mode=$mode/a $key=$value" $user_config_save_file
        else        
            [ -z "`cat $user_config_save_file |grep -w "\[$component\]"`" ] && echo -e "\n[$component]" >>$user_config_save_file
            [[ $key != "" ]] && echo "$key=$value" >>$user_config_save_file
        fi
    fi
}
# 此函数用于获取组件是否安装的判断,默认是都安装的
function get_install_component
{ 
    local component=$1
    local prompt=$2
    local component_exist=""

    if [[ "$operation" = "install" ]];then        
        if [ "$mode" = "custom" ];then
            # custom安装方式，如果指定配置文件中有模块，则自动安装，没有则交互
            [ "$user_specify_config" != "" ] && component_exist=`cat $user_specify_config | grep "\[$component\]"`
            if [ "$component_exist" != "" ];then
                config_answer="yes" 
                echo -e "$prompt (y/n? default: yes):$config_answer\n"
            else 
                read_bool "$prompt" "yes"
                config_answer=$answer
            fi
        else
            #非costom模式只要调用到该函数就认为是安装该模块
            config_answer="yes"
            echo -e "$prompt (y/n? default: yes):$config_answer\n"
        fi 
           
        #将当前配置保存起来
        [[ $config_answer = "yes" ]] && user_config_save "$component"
    fi  
}

# 从配置文件中获取指定模块中的配置参数
function get_component_config
{
    local component="$1"
    local file="$2"
    local key="$3"
    
    config_answer=""

    #配置文件不存在，错误
    [ ! -f "$file" ] && { echo -e "\n$file not exist."; exit 1; }
    [ -z "$component" ] && { echo -e "\ncomponent doesn't specified in get_install_config function"; exit 1; }
    get_string_num "$file" "\[$component\]"
    #未配置该模块
    [ "$string_num" -eq 0 ] && return 0
    #模块配置过多，错误
    [ "$string_num" -gt 1 ] && { echo -e "\nthere are too many \"[$component]\" in $file"; exit 1; }

    #从[$component]开始到下一个组件之间开始匹配"key="
    local key_num=`cat $file |sed '/^[[:space:]]*#/d' |sed -n "/\[$component\]/,/\[*\]/p" |grep -w "$key[[:space:]]*" |grep -c "$key[[:space:]]*="`
    #模块中没有配置该key
    [ "$key_num" -eq 0 ] && return 0
    #一个key有多个值，错误
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

    #配置文件不存在，错误
    [ ! -f "$file" ] && { echo -e "\n$file not exist."; exit 1; }

    local key_num=`cat $file |sed '/^[[:space:]]*#/d' |sed -n "1,/\[*\]/p" |grep -w "$key[[:space:]]*" |grep -c "$key[[:space:]]*="`
    #模块中没有配置该key
    [ "$key_num" -eq 0 ] && return 0
    #一个key有多个值，错误
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
# 用于获取指定模块下的配置数据
# value获取优先级：用户指定的配置文件>tecs安装包中自带的配置文件>交互
# 特殊情况：1. custom方式不从任何配置文件获取value；
#           2. 指定配置文件的组件中如果有default参数，则当指定配置文件查询不到value时，自动使用脚本中的默认值.
function get_install_config
{   
    local component=$1
    local key=$2
    local type=$3
    local prompt="$4"
    local default_value="$5"
    # 这个ip_list是给type为iplist时用的，其他为空
    local ip_list="$6"
    
    config_value=""
    
    [ -z "$key" ] && { echo -e "\nkey doesn't specified in get_install_config function"; exit 1; }
    
    #如果调用该函数时的默认值为空，则从tecs自带的配置文件中获取默认值
    if [ "$default_value" = "" ];then
        if [ -z "$component" ];then
            get_global_config "$tecs_publish_config" "$key"
        else            
            get_component_config "$component" "$tecs_publish_config" "$key"
        fi
        default_value="$config_answer"
    fi
    
    # 1. 从用户指定配置文件中获取配置
    if [ "$user_specify_config" != "" ];then
         if [ -z "$component" ];then
              get_global_config "$user_specify_config" "$key"
         else            
              get_component_config "$component" "$user_specify_config" "$key"
         fi
         config_value="$config_answer"
    fi
   
    # 2. mode!=custom时，使用tecs提供的系统默认值
    if [ "$config_value" = "" ];then
        if [ "$operation" = "install" -a "$mode" != "custom"  ];then
             config_value="$default_value"
        fi
    fi

    #3. 人工交互，执行到此步的场景：
    #  1) 通过菜单选择为custom方式安装;
    #  2) 用户指定的配置文件中安装模式为custom，且未在文件中配置的参数.
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
        #分类给出非交互模式下的提示打印
        if [ "$type" = "iplist" ];then
            echo -e "$prompt\nlocal ip list:\n$local_ip_list\n(recommend: $default_value): $config_value"
        elif [ "$type" = "bool" ];then
            echo -e "$prompt (y/n? default: $default_value): $config_value"
        else
            echo -e "$prompt (default: $default_value): $config_value"
        fi
    fi

    #将当前配置保存起来
    user_config_save "$component" "$key" "$config_value"
}


#######################rpm包处理相关基本函数############################

#判断某rpm包是否已安装
function check_installed
{
    has_installed="no"
    
    rpm -qi $1 &>/dev/null
    
    if [ 0 == $? ];then
        has_installed="yes"
    fi    
}

# 检查rpm包是否被依赖
function check_depend
{
    local rpm_name=$1
    # 检测依赖包是否被别人使用
    rpm -q --whatrequires $rpm_name &>/dev/null
    # 当查询不到被依赖的关系或rpm未安装，返回的是1，否则为0
    return "$?"
}

# 安装包的函数
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

# 检测要安装的包是否存在，如果不存在，则提示是否需要安装
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

# 检测要安装的包是否存在，如果不存在，则提示是否需要安装，如果存在，则需要升级
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

# 检测是否协助RPM包，如果已经安装过了，提示是否要卸载
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

# 卸载包的函数,用卸载tecs的rpm包，第三方包卸载不要用,暂时去掉
function uninstall_rpm_by_yum_delete
{
    local rpm_name=$1
    # 获取依赖包
    local pkgname=`rpm -qR $rpm_name | grep -v / | grep -v "(" | awk -F ' ' '{print$1}'`
    rpm -qR $rpm_name &>/dev/null
    local check_result=$?
    
    # 先卸载自己的RPM包，然后再检测下面的依赖包是否可以卸载
    yum -y --disablerepo=* --enablerepo=$repo_name remove  $rpm_name
    
    # 如果有依赖包，则进行依赖包卸载
    if [ $check_result -eq 0 ];then
        for i in $pkgname
        do
            # 如果是xen或者libvrit的rpm包，不卸载    
            if [ `echo "$i"|grep -c "^libvirt"` -ne 0 ] ||
               [ `echo "$i"|grep -c "openvswitch"` -ne 0 ] ||                                          
               [ `echo "$i"|grep -c "^xen"` -ne 0 ];then  
                continue
            fi  
            # 检测依赖包是否被别人使用
            rpm -q --whatrequires $i &>/dev/null
            # 当查询不到被依赖的关系，返回的是失败
            if [ $? -ne 0 ];then
                # 否则卸载该依赖包
                yum -y --disablerepo=* --enablerepo=$repo_name erase  $i
            fi
        done
    fi
}

# 卸载第三方包
function uninstall_rpm_by_yum
{
    local rpm_name=$1
    
    # 卸载包
    yum -y --disablerepo=* --enablerepo=$repo_name remove  $rpm_name

}

# 检测指定包是否需要升级
function check_app_is_upgrade
{ 
    local app=$1
    is_update="yes"

    num=`$tecs_yum list updates | awk -F ' ' '{print$1}' | grep $app -c`
    if [[ $num == 0 ]];then
        is_update="no"
    fi
}

# 升级包的函数
function upgrade_rpms_by_yum
{
    local app_list="$1"
    
    if [ "$app_list" = "" ];then
        echo -e "\nsorry, there is no rpm need to upgrade"!
        exit 0
    fi    
    #此处如果把app_list作为upgrade的参数，如果有一个不需要升级或已被升级
    #过的tecs服务，会导致整个升级失败，因此这里写为把每个服务单独判断是否升级
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
#######################服务处理相关基本函数############################
# 停止服务
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

# 启动服务服务
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

# 重启动服务服务
function service_restart
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
        cd /
        /etc/init.d/$app restart >/dev/null
        cd - >/dev/null
    fi
}

# 根据询问结果来停止服务
function ask_service_stop
{
    local app=$1
    if [ -x /etc/init.d/$app ]; then
        read_bool  "Stop service $app?" "yes"
        [ "$answer" == "yes" ] && /etc/init.d/$app stop >/dev/null 
    fi
}

# 根据询问结果来启动服务
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

# 自动停止所有服务
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

# 自动打开所有服务
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

# 检测并询问是否打开那个服务
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

#######################gurd监控############################
#guard相关函数放在这里其实不合理，但是因为被qpid和升级调用，不得不暂时放在这里
# 加入guard监控列表
function please_guard_me
{
    local service=$1
    local tecs_guard_dir=/etc/tecs/guard_list
    [ ! -x /etc/init.d/"$service" ] && return 0
    [ ! -d $tecs_guard_dir ] && mkdir -p $tecs_guard_dir

    #将服务加入guard监控列表
    touch $tecs_guard_dir/$service
}

# 从guard监控文件中剔除
function dont_guard_me
{
    local service=$1
    local tecs_guard_dir=/etc/tecs/guard_list
    [ ! -d $tecs_guard_dir ] && return 0
    
    #将服务移出guard监控列表
    rm -f $tecs_guard_dir/$service
}

_TECS_COMMON_FUNC_FILE="common_func.sh"
fi 