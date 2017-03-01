#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：vmcfg.py
# 测试内容：vmcfg模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
# 修改记录2：
#    修改日期：2012/06/8
#    版 本 号：V1.0
#    修 改 人：刘雪峰
#    修改内容：模块调试合入1
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
VM_START_INDEX = 0L
VM_QUERY_COUNT = 50L
#从租户的角度看到的虚拟机状态
U_STATE_RUNNING  = 2
U_STATE_PAUSED   = 3
U_STATE_SHUTOFF  = 4
U_STATE_CRASHED  = 5
U_STATE_UNKNOWN  = 6
U_STATE_CONFIGURATION = 7
#VM_ENABLE = bool(1)
#VM_DISABLE = bool(0)
USER_CREAT_VM = -5L
CREAT_VM_NUM1 = 1
#****************************************************************************#

class vmcfg:
    ''' tecs vmcfg methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session

    def action(self,id,operate):
        result = self.apisvr.tecs.vmcfg.action(self.session,id,operate) 
        if result[0] != 0:
            print "failed to %s vm %d! error code = %d" % (operate,id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to %s vm %s!" % (operate,id)
            return True
    
    def allocate_by_vt(self,vt_name,project_name,vm_name,vnc_passwd,vm_num):
        result = self.apisvr.tecs.vmcfg.allocate_by_vt(self.session,vt_name,project_name,vm_name,vnc_passwd,vm_num) 
        if result[0] != 0:
            print "failed to create by vt! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return -1
        else:
            print "success to create vm %s by vt!" % vm_name
            return long(result[1]) 
                
    def allocate(self,cfg):
        result = self.apisvr.tecs.vmcfg.allocate(self.session,cfg) 
        if result[0] != 0:
            print "failed to create vmcfg! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return -1
        else:
            print "success to create vm %s!" % cfg["cfg_info"]["vm_name"]
            print "new vm id : "
            vid_list = result[1]
            for v in vid_list:
                print " %d," % long(v)
            return long(v)

    def set(self,cfg):
        result = self.apisvr.tecs.vmcfg.set(self.session,cfg) 
        if result[0] != 0:
            print "failed to set vmcfg! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set vm %s!" % cfg["cfg_info"]["vm_name"]
            return True
                
    def clone(self,vid):
        result = self.apisvr.tecs.vmcfg.clone(self.session, vid) 
        if result[0] != 0:
            print "failed to clone vm! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return -1
        else:
            print "success to create vm new id : %d"  % (result[1])
            return long(result[1])
    
    def allocatebyvt(self,cfg):
        result = self.apisvr.tecs.vmcfg.allocate_by_vt(self.session,cfg) 
        if result[0] != 0:
            print "failed to create vmcfg! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create vm " 

            print "new vm id : "
            vid_list = result[1]
            for v in vid_list:
                print " %d," % long(v)
            return True

                
    def restore_image_backup(self, vid, request_id, target):
        result = self.apisvr.tecs.vmcfg.image_backup.restore(self.session, vid, request_id, target) 
        if result[0] != 0 and result[0] != 25:
            print "failed to restore image_backup! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to restore image_backup !"
            return True
   
    def create_image_backup(self, vid,  target):
        result = self.apisvr.tecs.vmcfg.image_backup.create(self.session, vid, target) 
        if result[0] != 0 and result[0] != 25:
            print "failed to create image_backup! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create image_backup !"
            return True
                   
    def delete_image_backup(self, vid,  request_id):
        result = self.apisvr.tecs.vmcfg.image_backup.delete(self.session, vid, request_id) 
        if result[0] != 0 and result[0] != 25:
            print "failed to delete image_backup! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete image_backup !"
            return True
    
#查询的接口及其拆分--begin   
    def get_vm_id(self,vm_name):
        '''根据指定的虚拟机名称查询虚拟机id '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,USER_CREAT_VM) 
        if result[0] != 0:
            print "failed to query vmcfg information of vm %s! error code =%d" % (vm_name,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            vmcfg_list = result[3]
            for v in vmcfg_list:
                if v["vm_name"] == vm_name:
                    print "succeed to get vm id = %d" % long(v["base_info"]["id"])
                    return long(v["base_info"]["id"])
            print "failed to get vm id!"
            return None

    def get_vm_state(self,id):
        '''根据指定的虚拟机id查询其状态  '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,long(id)) 
        if result[0] != 0:
            print "failed to query vmcfg information of vm %d! error code =%d" % (id,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            vmcfg_list = result[3]
            for v in vmcfg_list:
                if v["base_info"]["id"] == id:
                    return long(v["state"])

    def get_vm_cfg(self,id):
        '''根据指定的虚拟机id查询虚拟机配置'''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,long(id)) 
        if result[0] != 0:
            print "failed to query vmcfg information of vm %d! error code =%d" % (id,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            vmcfg_list = result[3]
            for v in vmcfg_list:
                if long(v["base_info"]["id"]) == long(id):
                    return v
            return None

    def query_vm_syn_state(self,configname):
        temp = self.apisvr.tecs.vmcfg.query_vm_syn_state(self.session,configname) 
        if temp[0] != 0:
            print "vmcfg:query vmcfg information failed! error code =%d" % temp[0] 
            print "error string = %s" % temp[1]
            return False
        else:
            print "-------vmcfg information-------:" 
            vmcfg_list = temp[1]
            for k in vmcfg_list:
                print "---------------vm info---------------"
                print "result = %d" % k["result"]
                print "id = %d" % k["vm_id"]
                print "vm_state = %d" % k["vm_state"]
                print "vnc_ip = %s" % k["vnc_ip"]
                print "vnc_port = %d" % k["vnc_port"]
                print "cpu_use_rate = %d" % k["cpu_use_rate"]
                print "mem_use_rate = %d" % k["mem_use_rate"]
                print "host_name = %s" % k["host_name"]
                
                nic_list = k["nic_info"]
                for n in nic_list:
                    for p in n.keys():
                        print " %s = %s" % (p,n[p])
                    

            return True
            
    def query(self,id):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,long(id)) 
        if result[0] != 0:
            print "failed to query vmcfg information of vm %d! error code =%d" % (id,result[0])
            print "error string = %s" % result[1]
            return NONE
        else:
            if len(result[3]) == 0:
                print "no vm %d!" % id
                return True;
            
            vmcfg_list = result[3]
            for v in vmcfg_list:
                print "==========================="
                print "vm id = %d" % v["base_info"]["id"]
                print "vm name = %s" % v["vm_name"]
                print "created at = %s" % v["create_time"]
                print "vm state = %d" % v["state"]
                print "project  = %s" % v["project_name"]
                
                print "virt ype = %s" % v["base_info"]["virt_type"]
                print "hypervisor type = %s" % v["base_info"]["hypervisor"]					
                print "assigned cluster = %s" % v["base_info"]["cluster"]
                print "assigned host = %s" % v["base_info"]["host"]
                print "machine image id = %s" % v["base_info"]["machine"]["id"]
                print "vcpu = %d" % v["base_info"]["vcpu"]
                print "memory = %d" % v["base_info"]["memory"]
              
                disk_list = v["base_info"]["disks"]
                for d in disk_list:
                    print "virtual disk %d: " % (disk_list.index(d) + 1)
                    for k in d.keys():
                        print " %s = %s" % (k,d[k])

                nic_list = v["base_info"]["nics"]
                for n in nic_list:
                    print "virtual network card %d: " % (nic_list.index(n) + 1)
                    for k in n.keys():
                        print "  %s = %s" % (k,n[k])
                        
                return v        
                    
    def get_id_by_name(self,query_type,query_name,query_user):
        '''query_type=-5表示当前用户，-4表示指定用户，-3表示所有用户'''
        result = self.apisvr.tecs.vmcfg.get_id_by_name(self.session,query_type,query_name,query_user) 
        if result[0] != 0:
            print "failed to query vmcfg full information of vm %s! error code =%d" % (query_name,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print " when name = %s: " % query_name  
            return result[1]
                    
    def fullquery(self,query_type,query_user,query_project,VM_START_INDEX,VM_QUERY_COUNT):
        '''显示指定的虚拟机配置信息  '''
        vid = query_type
        query_state = -1
        result = self.apisvr.tecs.vmcfg.full.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,query_type,query_user,query_project,query_state) 
        if result[0] != 0:
            print "failed to query vmcfg full information of vid %d! error code =%d" % (vid,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            if len(result[3]) == 0:
                print "no vm %d!" % vid
                return True;
                
            print "vmcfg information:" 
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            print "---------vmcfg list---------"  
            vmcfg_list = result[3]
            for v in vmcfg_list:
                print "vm %d:" % v["vid"]
                for k in v.keys():
                    print "  %s = %s" % (k,v[k])
            return True       
            
#查询的接口及其拆分--end
    def print_list(self,list):
        for item in list:
            if type(item) == type([]):
                self.print_list(item)
            elif type(item) == type({}):
                self.print_map(item)        
            else:
                print "%s" % item

    def print_map(self,map):
        for n,v in map.items():
            if type(v) == type([]):
                print "------%s start------" % n
                self.print_list(v)
                print "------%s end------" % n
            elif type(v) == type({}):
                print "------%s start------" % n
                self.print_map(v)
                print "------%s end------" % n
            else:
                print "%s = %-20s" % (n,v)

    def print_vmcfg_list(self,vmcfg_list):
        print "==========================="
        if type(vmcfg_list) == type([]):
            self.print_list(vmcfg_list)
        elif type(vmcfg_list) == type({}):
            self.print_map(vmcfg_list)    
        else:
            print "%s" % vmcfg_list    

    def query_count(self,id):
        '''查询虚拟机的数目 '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,id) 
        if result[0] != 0:
            print "failed to query vm information! error code =%d" % (result[0])
            print "error string = %s" % result[1]
            return None

        print "vmcfg information:" 
        print "next_index = %d" % result[1]
        print "max_count = %d" % result[2]
        vmcfg_list = result[3]
        self.print_vmcfg_list(vmcfg_list)

        return result[2]

    def query_disks_count(self,id):
        ''' 查询虚拟机的硬盘数目  '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,id) 
        if result[0] != 0:
            print "failed to query vm information! error code =%d" % (result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vm type %d!" % type
                return count;

            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)
            count = 0;
            for v in vmcfg_list:             
                disk_list = v["base_info"]["disks"]
                for d in disk_list:
                    count = count + 1
            return count  
                    
    def query_nic_count(self,id):
        ''' 查询虚拟机的网卡数目  '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,id) 
        if result[0] != 0:
            print "failed to query vm information! error code =%d" % (result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vm %d!" % id
                return count;
            
            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)

            for v in vmcfg_list:
                nic_list = v["base_info"]["nics"]
                for n in nic_list:
                    count = count + 1
            return count

    def query_value(self,id,query_name):
        ''' 指定查询明查询，建议作为本文件内部使用  '''
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,long(id)) 
        if result[0] != 0:
            print "failed to query vm information! error code =%d" % (result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vm %d!" % id
                return None;
            
            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)

            for v in vmcfg_list:
                return v[query_name]
            return None

    def query_base_info_value(self,id,query_name,query_name2=""):
        result = self.apisvr.tecs.vmcfg.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,long(id)) 
        if result[0] != 0:
            print "failed to query vm information, type %d! error code =%d" % (type,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vm type %d!" % type
                return None;
            
            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)
            for v in vmcfg_list:
                v_type = type(v["base_info"][query_name])
                if v_type == type([]):
                    v_map_list = v["base_info"][query_name]
                    for v_map in v_map_list:
                        return v_map[query_name2]
                elif v_type == type({}): 
                    v_map = v["base_info"][query_name]
                    return v_map[query_name2]
                else:
                    return v["base_info"][query_name]

            return None

    def query_pub(self,id):
        return self.query_value(id,"is_public")    
     
    def query_vmname(self,id):
        return self.query_value(id,"vm_name")  
    
    def query_projectname(self,id):
        return self.query_value(id,"project_name")  
        
    def query_vcpu(self,id):
        return self.query_base_info_value(id,"vcpu")
    
    def query_tcu(self,id):
        return self.query_base_info_value(id,"tcu")
        
    def query_description(self,id):
        return self.query_base_info_value(id,"description")   
       
    def query_vnc_passwd(self,id):
        return self.query_base_info_value(id,"vnc_passwd")  
        
    def query_virt_type(self,id):
        return self.query_base_info_value(id,"virt_type")

    def query_memory(self,id):
        return self.query_base_info_value(id,"memory")              

    def query_disks_value(self,id,query_name):
        return self.query_base_info_value(id,"disks",query_name) 

    def query_disks_size(self,id):
        return self.query_disks_value(id,"size") 

    def query_disks_bus(self,id):
        return self.query_disks_value(id,"bus") 

    def query_disks_target(self,id):
        return self.query_disks_value(id,"target") 

    def query_disks_position(self,id):
        return self.query_disks_value(id,"position") 

    def query_disks_fstype(self,id):
        return self.query_disks_value(id,"fstype") 
    
    def query_disks_position(self,id):
        return self.query_disks_value(id,"position") 
            
    def query_nics_value(self,id,query_name):
        return self.query_base_info_value(id,"nics",query_name) 

    def query_nics_index(self,id):
        return self.query_nics_value(id,"nic_index") 

    def query_nics_plane(self,id):
        return self.query_nics_value(id,"plane")
        
    def query_nics_vlan(self,id):
        return self.query_nics_value(id,"vlan") 
    
    def query_nics_dhcp(self,id):
        return self.query_nics_value(id,"dhcp") 
        
    def query_machine(self,id,query_name):
        return self.query_base_info_value(id,"machine",query_name) 
                   
    def query_machine_id(self,id):
        return self.query_machine(id,"id")

    def query_machine_bus(self,id):
        return self.query_machine(id,"bus")

    def query_machine_target(self,id):
        return self.query_machine(id,"target")

    def query_machine_type(self,id):
        return self.query_machine(id,"type")
                   
    def add_affinity_region(self,cfg):
        result = self.apisvr.tecs.vmcfg.affinity_region.member.add(self.session,cfg) 
        if result[0] != 0:
            print "failed to add vmcfg in affinity region error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to add vmcfg in affinity region "         
            return True
                  
    def del_affinity_region(self,cfg):
        result = self.apisvr.tecs.vmcfg.affinity_region.member.delete(self.session,cfg) 
        if result[0] != 0:
            print "failed to del vmcfg from affinity region error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete vmcfg from affinity region "         
            return True                
                    
    def query_static_info(self,type,id):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmcfg.staticinfo.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,type,id) 
        if result[0] != 0:
            print "failed to query vmcfg information of vm %d! error code =%d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            if len(result[3]) == 0:
                print "no vm %d!" % id
                return True;
            
            vmcfg_list = result[3]
            for v in vmcfg_list:
                print "=========vm %d static info=================="% v["vid"]
                print "project id = %d" % v["project_id"]
                print "user id = %d" % v["uid"]
                print "user name = %s" % v["user_name"]
                print "created at = %s" % v["create_time"]

                print "vm name = %s" % v["cfg_info"]["vm_name"]
                print "project_name = %s" % v["cfg_info"]["project_name"]

                    
                print "description = %s" % v["cfg_info"]["base_info"]["description"]
                print "vcpu = %d" % v["cfg_info"]["base_info"]["vcpu"]
                print "tcu = %d" % v["cfg_info"]["base_info"]["tcu"]
                print "memory = %d" % v["cfg_info"]["base_info"]["memory"]
                print "root_device = %s" % v["cfg_info"]["base_info"]["root_device"]
                print "kernel_command = %s" % v["cfg_info"]["base_info"]["kernel_command"]
                print "bootloader = %s" % v["cfg_info"]["base_info"]["bootloader"]
                print "virt_type = %s" % v["cfg_info"]["base_info"]["virt_type"]
                print "hypervisor = %s" % v["cfg_info"]["base_info"]["hypervisor"]
                print "rawdata = %s" % v["cfg_info"]["base_info"]["rawdata"]
                print "assigned cluster = %s" % v["cfg_info"]["base_info"]["cluster"]
                print "assigned host = %s" % v["cfg_info"]["base_info"]["host"]
                print "vm_wdtime = %d" % v["cfg_info"]["base_info"]["vm_wdtime"]
                print "vnc_passwd = %s" % v["cfg_info"]["base_info"]["vnc_passwd"]
                print "enable_coredump = %d" % v["cfg_info"]["base_info"]["enable_coredump"]
                print "enable_serial = %d" % v["cfg_info"]["base_info"]["enable_serial"]
                
                print "---machine info:---------------" 
                print "machine virtual disk %d " % v["cfg_info"]["base_info"]["machine"]["id"]
                print "position: %d " % v["cfg_info"]["base_info"]["machine"]["position"]
                print "disk size: %d " % v["cfg_info"]["base_info"]["machine"]["size"]
                print "bus: %s " % v["cfg_info"]["base_info"]["machine"]["bus"]
                print "disk type: %s" % v["cfg_info"]["base_info"]["machine"]["type"]
                print "target: %s" % v["cfg_info"]["base_info"]["machine"]["target"]
                print "fstype: %s" % v["cfg_info"]["base_info"]["machine"]["fstype"]
                print "----machine info---------------" 
              
                disk_list = v["cfg_info"]["base_info"]["disks"]
                for d in disk_list:
                    print "---virtual disk %d ------------" % d["id"]
                    print "position: %d " % d["position"]
                    print "disk size: %d " % d["size"]
                    print "bus: %s " % d["bus"]
                    print "disk type: %s" % d["type"]
                    print "target: %s" % d["target"]
                    print "fstype: %s" % d["fstype"]
                    print "-------------------------------" 

                nic_list = v["cfg_info"]["base_info"]["nics"]
                for n in nic_list:
                    print "----virtual network card %d ---" % n["nic_index"]
                    print "sriov %d " % n["sriov"]
                    print "loop %d " % n["loop"]
                    print "logic_network_id: %s " % n["logic_network_id"]
                    print "model: %s " % n["model"]
                    print "-------------------------------"

                q_list = v["cfg_info"]["base_info"]["qualifications"]
                for q in q_list:
                    print "-------------------------------"
                    print "key %s " % n["key"]
                    print "op %s " % n["op"]
                    print "value: %s " % n["value"]
                    print "-------------------------------"
                        
            return True        
                    
    def query_running_info(self,type,id):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmcfg.runninginfo.query(self.session,VM_START_INDEX,VM_QUERY_COUNT,type,id) 
        if result[0] != 0:
            print "failed to query vmcfg information of vm %d! error code =%d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            if len(result[3]) == 0:
                print "no vm %d!" % id
                return True;
            
            vmcfg_list = result[3]
            for v in vmcfg_list:
                print "=========vm %d running info=================="% v["vid"]
                print "state = %d" % v["computationinfo"]["state"]
                #print "disk_size = %d" % v["computationinfo"]["disk_size"]
                print "deployed_cluster = %s" % v["computationinfo"]["deployed_cluster"]
                print "deployed_hc = %s" % v["computationinfo"]["deployed_hc"]
                print "vnc_ip = %s" % v["computationinfo"]["vnc_ip"]

                print "vnc_port = %d" % v["computationinfo"]["vnc_port"]
                print "serial_port = %d" % v["computationinfo"]["serial_port"]
                print "cpu_rate = %d" % v["computationinfo"]["cpu_rate"]
                print "memory_rate = %d" % v["computationinfo"]["memory_rate"]

                print "disk_size = %d" % v["storageinfo"]["disk_size"]

                print "use_rate = %d" % v["netinfo"]["use_rate"]
                        
            return True        
  
    def migrate(self, cfg):
        result = self.apisvr.tecs.vmcfg.migrate(self.session,cfg) 
        if result[0] != 0:
            print "failed to migrate vm %d! error code =%d" % (cfg["vid"],result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to migrate vm %d! " % cfg["vid"]
            return True;          
    
