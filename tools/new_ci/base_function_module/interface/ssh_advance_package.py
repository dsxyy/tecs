#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�ssh_advance_package.py
# �������ݣ����Ի��������ݿ�ָ��ͱ��ݷ���, �����ӿ����ã�������Ҫ��ci_exit����Ϊ��ֻ�Ƿ�װ
#           ���Խ�����ظ��ϲ㣬���ϲ����ж�
# ��ǰ�汾��1.0 
# ��    �ߣ���Т��
# ������ڣ�2012/12/24
#*******************************************************************************/
import sys,os,getopt,time
from base_interface.ssh_cmd import *
from base_interface.rpc.contrib.tecs_common import *

# TECS�����ļ��࣬�����Ǹ�����׼��ʹ�õģ�������һ��ʧ���жϣ����˳���ci_exit
class tecs_cfg_file:
    def __init__(self,config_para):
        self.cmd = ssh_cmd(config_para.server_addr, '22', config_para.test_host_ssh_pwd)
        self.config_para = config_para
        
    def __del__(self):
        pass;
        
    def copy_cfg(self, src_file, dest_file):
        if self.cmd.do_cmd("cp -f " + src_file + " " + dest_file) == None:
            ci_exit(-1, "copy tecs cfg file fail")
            
    def back_tc_cfg(self):
        self.copy_cfg("/opt/tecs/tc/etc/tecs.conf", "/opt/tecs/tc/tc_tecs_bak.conf")

    def back_cc_cfg(self):
        self.copy_cfg("/opt/tecs/cc/etc/tecs.conf", "/opt/tecs/cc/cc_tecs_bak.conf")

    def back_hc_cfg(self):
        self.copy_cfg("/opt/tecs/hc/etc/tecs.conf", "/opt/tecs/hc/hc_tecs_bak.conf")

    def recover_tc_cfg(self):
        self.copy_cfg("/opt/tecs/tc/tc_tecs_bak.conf", "/opt/tecs/tc/etc/tecs.conf")
        
    def recover_cc_cfg(self):
        self.copy_cfg("/opt/tecs/cc/cc_tecs_bak.conf", "/opt/tecs/cc/etc/tecs.conf")

    def recover_hc_cfg(self):
        self.copy_cfg("/opt/tecs/hc/hc_tecs_bak.conf", "/opt/tecs/hc/etc/tecs.conf")

    def updata_config(self, modes, key, value):
        self.cmd.do_cmd("configtecs.sh %s %s %s" %(modes, str(key), str(value)))
        
    def updata_tc_config(self, key, value):
        self.updata_config("tc", key, value)
        
    def updata_cc_config(self, key, value):
        self.updata_config("cc", key, value)
        
    def updata_hc_config(self, key, value):
        self.updata_config("hc", key, value)

class tecs_run_ctl:
    def __init__(self,config_para):
        self.cmd = ssh_cmd(config_para.server_addr, '22', config_para.test_host_ssh_pwd)
        
    def stop_all_server(self):
        self.cmd.do_cmd("echo y | stoptecs.sh")
        
    def start_all_server(self):
        self.cmd.do_cmd("service tecs-tc start")
        self.cmd.do_cmd("service guard start")
        time_delay(5, "start all server,need wait tc start")
        
    def restart_hc_server(self):
        self.cmd.do_cmd("service tecs-hc restart")
        
    def SimHcBoardReboot(self):
        self.cmd.do_cmd("service guard stop")
        self.cmd.do_cmd("service tecs-hc stop")
        # ɾ�������
        vm_ids = self.cmd.do_cmd("xl list | awk -F' ' '{print $2}' | grep -v ID")
        for vm_id in vm_ids:
            if long(vm_id) == 0:
                continue
            self.cmd.do_cmd("xm destroy %d" % long(vm_id))
        # ɾ������λ�õ���ʱ�ļ�
        self.cmd.do_cmd("rm -rf /dev/shm/tecs/*")
        # ��HC����
        self.cmd.do_cmd("service guard start")
        
class ssh_pg_db_ctl:
    def __init__(self,config_para):
        self.cmd = ssh_cmd(config_para.server_addr, '22', config_para.test_host_ssh_pwd)
        self.host = config_para.server_addr
        
    def back_tc_db(self, mode, db_name):
        self.cmd.do_cmd("export PGPASSWORD=tecs;/usr/local/pgsql/bin/pg_dump -h %s -p 5432 -U tecs -w  --inserts -F c -f /opt/tecs/tc/tc_db_%s_back.sql %s" %(self.host, mode, db_name))
        
    def back_cc_db(self, mode, db_name):
        self.cmd.do_cmd("export PGPASSWORD=tecs;/usr/local/pgsql/bin/pg_dump -h %s -p 5432 -U tecs -w  --inserts -F c -f /opt/tecs/cc/cc_db_%s_back.sql %s" %(self.host, mode, db_name))

    def drop_db(self, db_name):
        self.cmd.do_cmd("export PGPASSWORD=tecs;/usr/local/pgsql/bin/dropdb -h %s -p 5432 -U tecs -w %s" %(self.host, db_name))
 
    def recover_tc_db(self, mode, db_name):
        self.cmd.do_cmd("export PGPASSWORD=tecs;/usr/local/pgsql/bin/pg_restore -h %s -p 5432 -U tecs -w -c -d %s /opt/tecs/tc/tc_db_%s_back.sql" %(self.host, db_name, mode))
        
    def recover_cc_db(self, mode, db_name):
        self.cmd.do_cmd("export PGPASSWORD=tecs;/usr/local/pgsql/bin/pg_restore -h %s -p 5432 -U tecs -w -c -d %s /opt/tecs/cc/cc_db_%s_back.sql" %(self.host, db_name, mode))

class ssh_vm_check:
    def __init__(self,config_para):
        self.cmd = ssh_cmd(config_para.server_addr, '22', config_para.test_host_ssh_pwd)
        self.host = config_para.server_addr
        
    def xl_list(self):
        return self.cmd.do_cmd("xl list")
        
    def check_vm_exist(self, name):
        vms = self.xl_list()
        for vm in vms:
            if vm.find(name) != -1:
                return True
        return False
    
    def time_out_check_NoEqual(self, function, para, check_value, time_limit):
        result = function(para)
        time_used = 0
        while result == check_value:
            time_delay(2, "ssh cmd is checking")
            result = function(para)
            time_used += 2
            if time_used >= time_limit:
                return False
        return True
    
    # �೤ʱ���ڣ�ָ�����Ƶ������Ҫ��ʧ����Ȼ��ʱ����ʧ��
    def check_vm_not_exist_with_time_out(self, name, time_limit):
        return self.time_out_check_NoEqual(self.check_vm_exist, name, True, time_limit)
            
    
    def get_domid(self, name):
        vms = self.xl_list()
        for vm in vms:
            if vm.find(name) != -1:
                values = vm.split(" ")
                result = []
                for value in values:
                    if value != '':
                        result.append(value)
                return long(result[1])
        return -1
    
    # ��ѯ�������DOMID,����ʱ����
    def get_domid_timeout(self, name, time_limit):
        if self.time_out_check_NoEqual(self.get_domid, name, -1, time_limit):
            return self.get_domid(name)
        return -1;
                
    def check_domid_change_timeout(self, dom_name, old_dom_id, time_limit):
        # ���ж��Ƿ����ڵ�domid ���ϵ�domid���
        if self.time_out_check_NoEqual(self.get_domid, dom_name, old_dom_id, time_limit):
            # ���������ˣ��ж����Ƿ�Ϊ-1�����ҵ�-1��ȥ����Ϊ�п��ܳ���-1������old_dom_id�����
            if self.time_out_check_NoEqual(self.get_domid, dom_name, -1, time_limit):
                # ����ȣ��ֲ�����-1������£��ٴμ����old_dom_id �� ��ǰdomid�Ƿ����
                return self.time_out_check_NoEqual(self.get_domid, dom_name, old_dom_id, time_limit)
            
        
        