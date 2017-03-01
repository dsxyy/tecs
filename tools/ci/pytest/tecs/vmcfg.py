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
    def __init__(self, server,session):
        self.apisvr = server
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
            
    def action2(self,id,operate):
        result = self.apisvr.tecs.vmcfg.action(self.session,id,operate) 
        if result[0] != 0:
            print "failed to %s vm %d! error code = %d" % (operate,id,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to %s vm %s!" % (operate,id)
            
        workflow_id = result[1]
        target = result[2]

        print 'target %s  id %s ' % (target,workflow_id)
        return target,workflow_id
        
    def portable_disk_attach(self,request_id,vid,target,bus):
        result = self.apisvr.tecs.vmcfg.portable_disk.attach(self.session,request_id,vid,target,bus) 

        if result[0] != 0:
            print "failed to attach%s on vm %d! error code = %d" % (request_id,vid,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to attach %s on vm %s!" % (request_id,vid)

        workflow_id = result[1]
        target = result[2]

        print 'attach portable_disk target %s  id %s ' % (target,workflow_id)
        return target,workflow_id

    def portable_disk_deattach(self,vid,request_id):
        result = self.apisvr.tecs.vmcfg.portable_disk.detach(self.session,vid,request_id)

        if result[0] != 0:
            print "failed to deattch %s on vm %d! error code = %d" % (request_id,vid,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to deattch %s on vm %s!" % (request_id,vid)

        workflow_id = result[1]
        target = result[2]
        print 'deattch portable_disk target %s  id %s ' % (target,workflow_id)
        return target,workflow_id
        
    def image_back_create(self,vid,target):
	result = self.apisvr.tecs.vmcfg.image_backup.create(self.session,vid,target) 

        if result[0] != 0:
            print "failed to create image_back on vm %d! error code = %d" % (vid,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to create image_back on vm %d!" % (vid)

        workflow_id = result[1]
        target = result[2]
        
        print 'create portable_disk target %s  id %s ' % (target,workflow_id)
        return target,workflow_id
        
    def image_back_delete(self,vid,request_id):
	result = self.apisvr.tecs.vmcfg.image_backup.delete(self.session,vid,request_id) 

        if result[0] != 0:
            print "failed to delete image_back on vm %d! error code = %d" % (vid,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to delete image_back on vm %d!" % (vid)

        workflow_id = result[1]
        target = result[2]
        
        print 'delete portable_disk target %s  id %s ' % (target,workflow_id)
        return target,workflow_id
        
    def image_back_restore(self,vid,request_id,target):
	result = self.apisvr.tecs.vmcfg.image_backup.restore(self.session,vid,request_id,target) 


        if result[0] != 0:
            print "failed to restore image_back on vm %d! error code = %d" % (vid,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to restore image_back on vm %d!" % (vid)

        workflow_id = result[1]
        target = result[2]
        
        print 'restore portable_disk target %s  id %s ' % (target,workflow_id)
        return target,workflow_id  
        
    def allocate(self,cfg):
        result = self.apisvr.tecs.vmcfg.allocate(self.session,cfg) 
        if result[0] != 0:
            print "failed to create vmcfg! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create vm %s!" % cfg["vm_name"]
            print "new vm id = %d" % result[1]
            return True

    def set(self,cfg):
        result = self.apisvr.tecs.vmcfg.set(self.session,cfg) 
        if result[0] != 0:
            print "failed to set vmcfg! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set vm %s!" % cfg["vm_name"]
            return True
            
    def set_disk(self,id,disks):
        result = self.apisvr.tecs.vmcfg.set_disk(self.session,id,disks) 
        if result[0] != 0:
            print "failed to set vm %d disks! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set vm %d disk!" % id
            return True
    
    def set_image_disk(self,id,imagedisks):
        result = self.apisvr.tecs.vmcfg.set_image_disk(self.session,id,imagedisks) 
        if result[0] != 0:
            print "failed to set vm %d image disks! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set vm %d image disk!" % id
            return True    
    
    def set_nic(self,id,nics):
        result = self.apisvr.tecs.vmcfg.set_nic(self.session,id,nics) 
        if result[0] != 0:
            print "failed to set vm %d nics! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set vm %d nic!" % id
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
            return False
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
                        
                return True        
                    
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
                   
                  
                    
                    
                    
                    

                    
    
