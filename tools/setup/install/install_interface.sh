#!/bin/bash
# �ṩ��yum��װ��صĹ��������ͱ���
if [ ! "$_INSTALL_INTERFACE_FILE" ];then
_INSTALL_INTERFACE_DIR=`pwd`
cd $_INSTALL_INTERFACE_DIR/../common/
.  tecs_common_func.sh
# ���÷���ǽ�ӿں���
cd $_INSTALL_INTERFACE_DIR/../base/
.  qpid_interface.sh
.  pgdb_interface.sh
.  firewall_interface.sh

cd $_INSTALL_INTERFACE_DIR
.  install_global_var.sh
.  install_func.sh

# �ֶ���װ�����
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
    #debugģʽ�¶�ǵ��쳣��־���ļ��У����ڶ�λ����
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

# ��װ3��1��ģʽ
function all_install
{
    # ���ָ��Ŀ¼����û��img,�򲻽���demo�İ�װ
    local img_exist=`ls /home/tecs_install/ | grep -c .img`
    if [[ $img_exist -ne 0 ]]; then
        # ��demo�ű��ŵ�rc.local����ִ��,�´�ϵͳ������ʱ��ִ��һ��
        echo "echo wait tecs start...; sleep 60; /opt/tecs/tc/cli/demo/tecs-start_demo.py" >> /etc/rc.d/rc.local
        echo "sed -i '/tecs-start_demo.py/d' /etc/rc.d/rc.local" >> /etc/rc.d/rc.local
    fi

    # ִ�а�װTECSȫ����װ�ű�
    echo "*******************************************************************************"
    echo "tecs will installed on local host ..." 
    echo "*******************************************************************************"

    # ��װ��Ϣ�����������ݿ����    
    install_qpid_service_component
    install_db_component
    
    create_db_user    

    service qpid start
    service postgresql start

    # ��װTECS������洢���������TC����еģ���Ĭ��3��1�������治װ
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
 
    # �򿪷���ǽ����
    install_firewall_component
    
    [ -x /etc/init.d/xend ] && service xend start &>/dev/null
    [ -x /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    
    start_service_all

    echo "tecs all is installed and started now!"
}

# ��HC������п��ƽ��
function ctrl_install
{
   # ���ָ��Ŀ¼����û��img,�򲻽���demo�İ�װ
    local img_exist=`ls /home/tecs_install/ | grep -c .img`
    if [[ $img_exist -ne 0 ]]; then
        # ��demo�ű��ŵ�rc.local����ִ��,�´�ϵͳ������ʱ��ִ��һ��
        echo "echo wait tecs start...; sleep 60; /opt/tecs/tc/cli/demo/tecs-start_demo.py" >> /etc/rc.d/rc.local
        echo "sed -i '/tecs-start_demo.py/d' /etc/rc.d/rc.local" >> /etc/rc.d/rc.local
    fi

    # ִ�а�װTECS���ƽ��Ľű�
    
    # ����ϵ�ӳ���ϵ
    sed -i "/$cloudname/d" /etc/hosts
    # �ѵ�ǰ�ı��ص�ַ��������������TECSʹ������������
    [ `cat /etc/hosts | grep -c $cloudname` -eq 0 ] && echo "127.0.0.1 $cloudname" >> /etc/hosts

    # ��װ��Ϣ�����������ݿ����
    install_qpid_service_component
   
    db_component_rpm_install
    create_db_user

    service qpid start
    service postgresql start

    # ��װTECS������洢���������TC����еģ���Ĭ��3��1�������治װ
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
    
    # �򿪷���ǽ����
    install_firewall_component
        
    [ -e /etc/init.d/xend ] && service xend start &>/dev/null
    [ -e /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null
    
    start_service_all

    echo "tecs ctrl is installed and started successfully!"

}
# ������װcluster��ģʽ,cc+vna+db+qpid
function cluster_install
{
    # ��װ��Ϣ�����������ݿ����    
    install_qpid_service_component
    install_db_component
    create_db_user    

    service qpid start
    service postgresql start

    # ��װTECS���
    update_libvirt
    
    install_sd_component
    
    install_vna_component 
    
    install_cc_component
    
    install_nicchk_component

    install_guard_component
 
    # �򿪷���ǽ����
    install_firewall_component
    
    [ -x /etc/init.d/xend ] && service xend start &>/dev/null
    [ -x /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    
    # ��������  
    service_start  "tecs-sd"    
    service_start  "tecs-vna"
    service_start  "tecs-cc"
    service_start  "nicchk"
    service_start  "tecs-guard"
   
    echo "tecs cluster is installed and started now!"
}

# ������װhost��ģʽ��hc+vna+wdg
function host_install
{
    #hc����Ϣ��������Ĭ��Ϊ�գ���Ϊ��Ϣ�����������ڱ���

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
    
    # ��Ҫ�Ȱ�װvna
    install_vna_component
    
    install_hc_component
    
    install_wdg_component
    
    install_nicchk_component

    install_guard_component

    # �򿪷���ǽ����
    install_firewall_component   

    [ -e /etc/init.d/xend ] && service xend start &>/dev/null
    [ -e /etc/init.d/libvirtd ] && service libvirtd start &>/dev/null 
    
    # ��������
    service_start  "tecs-sd"
    service_start  "tecs-vna"
    service_start  "tecs-hc"
    service_start  "tecs-wdg"
    service_start  "nicchk"
    service_start  "tecs-guard"

    echo "tecs host is installed and started successfully!"
}

# ������װSA��ģʽ
function sa_install
{   
    #Ĭ��Ϊ�գ���Ϊ��Ϣ�����������ڱ���
    if [ "$1" != "" ];then
        latest_message_server="$1"
        echo -e "you has input the message broker addr to SA: $latest_message_server"
    else
        read_msg_broker "tecs-sa" "Please input message broker addr to SA:" ""
        latest_message_server=$answer
    fi 
    # ����ϵ�ӳ���ϵ
    sed -i "/$cloudname/d" /etc/hosts
    # ��HC����image��ʱ��ӳ���ַ���Ӹ���ַӳ���ϵ
    [ `cat /etc/hosts | grep -c $cloudname` -eq 0 ] && echo "$latest_message_server $cloudname" >> /etc/hosts

    # ��װRPM
    install_tgtd_service_component
    service_start  "tgtd" 
    
    install_storage_component
    install_sd_component
    install_nicchk_component
    install_guard_component
    
    install_firewall_component
    
    # ��������    
    service_start  "tecs-sa"
    service_start  "tecs-sd"
    service_start  "nicchk"
    service_start  "tecs-guard"

    #�ȴ�һ��ʱ�䣬tecs���̶��ϵ�������ٷ���?
    echo "tecs sa is installed and started successfully!"
}

_INSTALL_INTERFACE_FILE="install_interface.sh"

fi

