#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：ssh_advance_package.py
# 测试内容：测试环境的数据库恢复和备份方法, 新增接口配置，尽量不要加ci_exit，因为这只是封装
#           测试结果返回给上层，由上层来判断
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/12/24
#*******************************************************************************/
import sys,os,getopt,time
from base_interface.ssh_cmd import *
from base_interface.rpc.contrib.tecs_common import *

# TECS配置文件类，该类是给环境准备使用的，所以有一个失败判断，并退出的ci_exit
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
        # 删除虚拟机
        vm_ids = self.cmd.do_cmd("xl list | awk -F' ' '{print $2}' | grep -v ID")
        for vm_id in vm_ids:
            if long(vm_id) == 0:
                continue
            self.cmd.do_cmd("xm destroy %d" % long(vm_id))
        # 删除共享位置的临时文件
        self.cmd.do_cmd("rm -rf /dev/shm/tecs/*")
        # 打开HC服务
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
    
    # 多长时间内，指定名称的虚拟机要消失，不然超时返回失败
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
    
    # 查询虚拟机的DOMID,带超时功能
    def get_domid_timeout(self, name, time_limit):
        if self.time_out_check_NoEqual(self.get_domid, name, -1, time_limit):
            return self.get_domid(name)
        return -1;
                
    def check_domid_change_timeout(self, dom_name, old_dom_id, time_limit):
        # 先判断是否现在的domid 和老的domid相等
        if self.time_out_check_NoEqual(self.get_domid, dom_name, old_dom_id, time_limit):
            # 如果不相等了，判断下是否为-1，并且等-1过去，因为有可能出现-1不等于old_dom_id的情况
            if self.time_out_check_NoEqual(self.get_domid, dom_name, -1, time_limit):
                # 不相等，又不等于-1的情况下，再次检查下old_dom_id 和 当前domid是否相等
                return self.time_out_check_NoEqual(self.get_domid, dom_name, old_dom_id, time_limit)
            
        
        