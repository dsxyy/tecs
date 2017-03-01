#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_001.py
# �������ݣ����������������������
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
# �޸ļ�¼2
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#*******************************************************************************/
import sys,os,getopt,time

import test_user_group_rpc
import test_cluster_manager_rpc
import test_session_login_rpc
import test_host_rpc
import test_image_rpc
import test_netplane_rpc
import test_port_rpc
import test_project_rpc
import test_sa_rpc
import test_snmp_rpc
import test_system_license_rpc
import test_tcu_rpc
import test_trapaddress_rpc
import test_trunk_rpc
import test_version_rpc
import test_vmcfg_rpc
import test_vmtemplate_rpc
import test_vnet_dhcp_rpc
import test_vnet_vlan_rpc


# TECS����RPC�ӿ�ģ����Ի��ܽӿ�
class AllRpcTest():
    def __init__(self):
        pass;
        
    def TestEntery(self, environment):
        test_user_group_rpc.all_test_entry(environment)
        test_cluster_manager_rpc.all_test_entry(environment)
        test_session_login_rpc.all_test_entry(environment)
        test_host_rpc.all_test_entry(environment)
        test_image_rpc.all_test_entry(environment)
        test_netplane_rpc.all_test_entry(environment)
        test_port_rpc.all_test_entry(environment)
        test_project_rpc.all_test_entry(environment)
        test_sa_rpc.all_test_entry(environment)
        test_snmp_rpc.all_test_entry(environment)
        test_system_license_rpc.all_test_entry(environment)
        test_tcu_rpc.all_test_entry(environment)
        test_trapaddress_rpc.all_test_entry(environment)
        test_trunk_rpc.all_test_entry(environment)
        test_version_rpc.all_test_entry(environment)
        test_vmcfg_rpc.all_test_entry(environment)
        test_vmtemplate_rpc.all_test_entry(environment)
        test_vnet_dhcp_rpc.all_test_entry(environment)
        test_vnet_vlan_rpc.all_test_entry(environment)
        

def DocEntery(cidoc):
    test_user_group_rpc.all_doc_entry(cidoc)
    test_cluster_manager_rpc.all_doc_entry(cidoc)
    test_session_login_rpc.all_doc_entry(cidoc)
    test_host_rpc.all_doc_entry(cidoc)
    test_image_rpc.all_doc_entry(cidoc)
    test_netplane_rpc.all_doc_entry(cidoc)
    test_port_rpc.all_doc_entry(cidoc)
    test_project_rpc.all_doc_entry(cidoc)
    test_sa_rpc.all_doc_entry(cidoc)
    test_snmp_rpc.all_doc_entry(cidoc)
    test_system_license_rpc.all_doc_entry(cidoc)
    test_tcu_rpc.all_doc_entry(cidoc)
    test_trapaddress_rpc.all_doc_entry(cidoc)
    test_trunk_rpc.all_doc_entry(cidoc)
    test_version_rpc.all_doc_entry(cidoc)
    test_vmcfg_rpc.all_doc_entry(cidoc)
    test_vmtemplate_rpc.all_doc_entry(cidoc)
    test_vnet_dhcp_rpc.all_doc_entry(cidoc)
    test_vnet_vlan_rpc.all_doc_entry(cidoc)
        



 
