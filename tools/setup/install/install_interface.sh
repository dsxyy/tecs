#!/bin/bash
# 提供和yum安装相关的公共函数和变量
if [ ! "$_INSTALL_INTERFACE_FILE" ];then
_INSTALL_INTERFACE_DIR=`pwd`
cd $_INSTALL_INTERFACE_DIR/../common/
.  tecs_common_func.sh
# 调用防火墙接口函数
cd $_INSTALL_INTERFACE_DIR/../base/
.  qpid_interface.sh
.  pgdb_interface.sh
.  firewall_interface.sh

cd $_INSTALL_INTERFACE_DIR
.  install_global_var.sh
.  install_func.sh

# 手动安装的入口
function custom_install
{
    stop_service_all
    
    install_qpid_service_component
    install_db_component
    install_tgtd_service_component
    
    service_start  "qpid"
    service_start  "postgresql"
    service_start  "tgtd"  
    
    get_install_config "" "debug" "bool" "run tecs in debug mode?" ""
    debugmode="$config_value"
    #debug模式下多记点异常日志到文件中，便于定位问题
    if [[ $debugmode == "yes" ]]; then
        exc_log_level=5 #SYS_NOTICE
    else
        exc_log_level=3 #SYS_ERROR
    fi
    
    install_tc_component
    install_cc_component
    install_hc_component
    install_vna_component
    install_vnm_component
    install_wdg_component
    install_sd_component
    install_storage_component
    install_wui_component
    install_nicchk_component
    install_guard_component
    install_firewall_component
    
    ask_service_start_all
}

# 安装3合1的模式
function all_install
{
    # 如果指定目录下面没有img,则不进行demo的安装
    local img_exist=`ls /home/tecs_install/ | grep -c .img`
    if [[ $img_exist -ne 0 ]]; then
        # 把demo脚本放到rc.local下面执行,下次系统启动的时候执行一次
        echo "echo wait tecs start...; sleep 60; /opt/tecs/tc/cli/demo/tecs-start_demo.py" >> /etc/rc.d/rc.local
        echo "sed -i '/tecs-start_demo.py/d' /etc/rc.d/rc.local" >> /etc/rc.d/rc.local
    fi

    # 执行安装TECS全部安装脚本
    echo "*******************************************************************************"
    echo "tecs will installed on local host ..." 
    echo "*******************************************************************************"

    # 安装消息服务器和数据库组件    
    install_qpid_service_component
    install_db_component
    
    create_db_user    

    service qpid start
    service postgresql start

    # 安装TECS组件，存储组件不属于TC组件中的，在默认3合1环境下面不装
    install_tc_component
    install_cc_component

    install_tgtd_service_component
    service_start  "tgtd" 
    install_storage_component
    #check if libvirt need to update
    update_libvirt 
    install_hc_component
    install_sd_component
    install_vna_component 
    install_vnm_component 
    install_wdg_component
    install_wui_component
    install_nicchk_component
    install_guard_component
 
    # 打开防火墙设置
    install_firewall_component
    
    [ -x /etc/init.d/xend ] && service xend start &>/dev/null
    [ -x /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    
    start_service_all

    echo "tecs all is installed and started now!"
}

# 除HC外的所有控制结点
function ctrl_install
{
   # 如果指定目录下面没有img,则不进行demo的安装
    local img_exist=`ls /home/tecs_install/ | grep -c .img`
    if [[ $img_exist -ne 0 ]]; then
        # 把demo脚本放到rc.local下面执行,下次系统启动的时候执行一次
        echo "echo wait tecs start...; sleep 60; /opt/tecs/tc/cli/demo/tecs-start_demo.py" >> /etc/rc.d/rc.local
        echo "sed -i '/tecs-start_demo.py/d' /etc/rc.d/rc.local" >> /etc/rc.d/rc.local
    fi

    # 执行安装TECS控制结点的脚本
    
    # 清除老的映射关系
    sed -i "/$cloudname/d" /etc/hosts
    # 把当前的本地地址建立成主机名，TECS使用主机名访问
    [ `cat /etc/hosts | grep -c $cloudname` -eq 0 ] && echo "127.0.0.1 $cloudname" >> /etc/hosts

    # 安装消息服务器和数据库组件
    install_qpid_service_component
   
    db_component_rpm_install
    create_db_user

    service qpid start
    service postgresql start

    # 安装TECS组件，存储组件不属于TC组件中的，在默认3合1环境下面不装
    install_tc_component 
    install_cc_component 
    install_storage_component
    #check if libvirt need to update
    update_libvirt 

    # do not install hc in this mode
    install_sd_component
    
    install_vna_component 
    install_vnm_component 
    install_wui_component
    install_nicchk_component
    install_guard_component
    
    # 打开防火墙设置
    install_firewall_component
        
    [ -e /etc/init.d/xend ] && service xend start &>/dev/null
    [ -e /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null
    
    start_service_all

    echo "tecs ctrl is installed and started successfully!"

}
# 仅仅安装cluster的模式,cc+vna+db+qpid
function cluster_install
{
    # 安装消息服务器和数据库组件    
    install_qpid_service_component
    install_db_component
    create_db_user    

    service qpid start
    service postgresql start

    # 安装TECS组件
    update_libvirt
    
    install_sd_component
    
    install_vna_component 
    
    install_cc_component
    
    install_nicchk_component

    install_guard_component
 
    # 打开防火墙设置
    install_firewall_component
    
    [ -x /etc/init.d/xend ] && service xend start &>/dev/null
    [ -x /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    
    # 启动服务  
    service_start  "tecs-sd"    
    service_start  "tecs-vna"
    service_start  "tecs-cc"
    service_start  "nicchk"
    service_start  "tecs-guard"
   
    echo "tecs cluster is installed and started now!"
}

# 仅仅安装host的模式，hc+vna+wdg
function host_install
{
    #hc的消息服务器，默认为空，因为消息服务器不会在本地

    latest_message_server=$answer
    if [ "$1" != "" ];then
        latest_message_server="$1"
        echo -e "you has input the message broker addr to CC: $latest_message_server"
    else
        read_msg_broker "tecs-hc" "Please input message broker addr to CC:" ""
        latest_message_server=$answer
    fi 
    #check if libvirt need to update
    update_libvirt  
    
    install_sd_component
    
    # 需要先安装vna
    install_vna_component
    
    install_hc_component
    
    install_wdg_component
    
    install_nicchk_component

    install_guard_component

    # 打开防火墙设置
    install_firewall_component   

    [ -e /etc/init.d/xend ] && service xend start &>/dev/null
    [ -e /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    
    # 启动服务
    service_start  "tecs-sd"
    service_start  "tecs-vna"
    service_start  "tecs-hc"
    service_start  "tecs-wdg"
    service_start  "nicchk"
    service_start  "tecs-guard"

    echo "tecs host is installed and started successfully!"
}

# 仅仅安装SA的模式
function sa_install
{   
    #默认为空，因为消息服务器不会在本地
    if [ "$1" != "" ];then
        latest_message_server="$1"
        echo -e "you has input the message broker addr to SA: $latest_message_server"
    else
        read_msg_broker "tecs-sa" "Please input message broker addr to SA:" ""
        latest_message_server=$answer
    fi 
    # 清除老的映射关系
    sed -i "/$cloudname/d" /etc/hosts
    # 给HC下载image的时候，映像地址增加个地址映射关系
    [ `cat /etc/hosts | grep -c $cloudname` -eq 0 ] && echo "$latest_message_server $cloudname" >> /etc/hosts

    # 安装RPM
    install_tgtd_service_component
    service_start  "tgtd" 
    
    install_storage_component
    install_sd_component
    install_nicchk_component
    install_guard_component
    
    install_firewall_component
    
    # 启动服务    
    service_start  "tecs-sa"
    service_start  "tecs-sd"
    service_start  "nicchk"
    service_start  "tecs-guard"

    #等待一段时间，tecs进程都上电完成了再返回?
    echo "tecs sa is installed and started successfully!"
}

_INSTALL_INTERFACE_FILE="install_interface.sh"

fi

