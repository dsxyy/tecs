#!/bin/bash
#1.���ȫ�ֱ��������ܻᱻ���������ű����ã�export��Ҳ�ɱ��ӽ����е�ִ�еĽű�����
#2.���нű��в����к���Щ�������ֳ�ͻ�ı���
#3.�벻Ҫ�������ȫ�ֱ������⸳ֵ����Щ��ʼ������Ϊ�� 

#��ֹ�ű��ظ�������
if [ ! "$_GLOBAL_VAR_FILE" ];then

#tecs��װ�򵼲���ʱ���¼
export current_time=""
#TECS����������install��uninstall��clean��upgrade��
export operation=""
#��װ�еľ���ģʽ
export mode=""
#yum�����װ
export tecs_yum=""

#tecs��װ�ļ�Ŀ¼
export tecs_install_path="/home/tecs_install"

#�û�ָ�������ļ�
export user_specify_config=""
#tecs��װ�����Դ��������ļ�
export tecs_publish_config=""
#tecs��װ�����б���ı�������·�����ļ�
export user_config_save_path=${tecs_install_path}
export user_config_save_file=""

#���ݿ��û�������
export dbusername=tecs
export dbpassword=tecs

#tecs��������ļ�
export tecs_tc_conf=/opt/tecs/tc/etc/tecs.conf
export tecs_cc_conf=/opt/tecs/cc/etc/tecs.conf
export tecs_hc_conf=/opt/tecs/hc/etc/tecs.conf
export tecs_sa_conf=/opt/tecs/sa/etc/tecs.conf
export tecs_vnm_conf=/opt/tecs/network/vnm/etc/tecs.conf

#nicchk�����ļ�
export nicchk_conf=/etc/nicchk/nicchk.conf

export _GLOBAL_VAR_FILE="global_var.sh"
fi
