#!/bin/bash
# �ṩ��yum��װ��صĹ��������ͱ���
if [ ! "$_UPGRADE_INTERFACE_FILE" ];then
_UPGRADE_INTERFACE_DIR=`pwd`
cd $_UPGRADE_INTERFACE_DIR/../common/
.  tecs_common_func.sh
cd $_UPGRADE_INTERFACE_DIR/../base/
.  qpid_interface.sh
.  pgdb_interface.sh

cd $_UPGRADE_INTERFACE_DIR/../install/
.  install_func.sh

cd $_UPGRADE_INTERFACE_DIR
.  upgrade_func.sh

function upgrade_tecs
{
    local result=0
    
    # ��ֹͣ���з���
    echo "stop all service..."
    stop_service_all
   
    #���tc��cc�Ĺ��������Ƿ�Ϊ��
    check_workflow "tecs-tc" "$tecs_tc_conf"
    check_workflow "tecs-cc" "$tecs_cc_conf"
    check_workflow "tecs-vnm" "$tecs_vnm_conf"

    # ���TC,CC�Ƿ���Ҫ����
    check_app_is_upgrade "tecs-tc"
    tc_is_update=$is_update
    check_app_is_upgrade "tecs-cc"
    cc_is_update=$is_update
    check_app_is_upgrade "tecs-vnm"
    vnm_is_update=$is_update
    
    #�����Ϣ�����rpm�����ݿ�����rpm�Ƿ���Ҫ����
    check_installed "qpidc-broker"
    if [ $has_installed = "yes" ];then
        check_app_is_upgrade "qpidc-broker"
        [ $is_update = "yes" ] && { $tecs_yum install qpidc-broker qpidc-client && service qpid start || result=$?; }
        [ "$result" -ne 0 ] && echo -e "\nupgrade qpid failed,return $result"! && exit $result
    fi
    #���ݿ�rpm���������ű������⣬��ʱ����������汾��������p13b2��ɸ�Ϊ����qpid�Ĵ�����ʽ
    check_installed "postgresql"
    if [ $has_installed = "yes" ];then
        check_app_is_upgrade "postgresql"
        [ $is_update = "yes" ] && { $tecs_yum remove postgresql; $tecs_yum install postgresql postgresql-client && service postgresql start || result=$?; }
        [ "$result" -ne 0 ] && echo -e "\nupgrade postgresql failed,return $result"! && exit $result
        [ -e /etc/tecs/guard_list/postgresql ] || please_guard_me "postgresql"
    fi
    #���tgtd�Ƿ���Ҫ����
    check_installed "scsi-target-utils"
    if [ $has_installed = "yes" ];then
        check_app_is_upgrade "scsi-target-utils"
        [ $is_update = "yes" ] && { $tecs_yum install scsi-target-utils && service tgtd start || result=$?; }
        [ "$result" -ne 0 ] && echo -e "\nupgrade tgtd failed,return $result"! && exit $result
    fi
    
    #���nicchk�Ƿ�װ������
    check_installed "nicchk"
    if [ $has_installed = "yes" ];then
        $tecs_yum upgrade nicchk
    else
        install_nicchk_component
    fi    
    service nicchk start
    #��ȡ��ǰ����tecs�����
    get_tecs_services
    
    # ����tecs�����
    upgrade_rpms_by_yum "$all_tecs_services"
    
    # ����TC�����ݿ�
    if [[ "$tc_is_update" == "yes" ]];then
        upgrade_tc_database
    fi
    
    # ����CC�����ݿ�
    if [[ "$cc_is_update" == "yes" ]];then
        upgrade_cc_database
    fi
    
    # ����VNM�����ݿ�
    if [[ "$vnm_is_update" == "yes" ]];then
        upgrade_vnm_database
    fi

	chkconfig --list |grep iptables|grep on >/dev/null
	[ "$?" -eq 0 ] && { cd $_UPGRADE_INTERFACE_DIR/../base/; ./firewall_config.sh; cd - >/dev/null; }
		
    # ����TECS����
    echo "start all service..."
    start_service_all
}

_UPGRADE_INTERFACE_FILE="upgrade_interface.sh"
fi
