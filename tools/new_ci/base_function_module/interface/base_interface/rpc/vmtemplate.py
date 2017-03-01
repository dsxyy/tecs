#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：vmcfg.py
# 测试内容：vmcfg模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：李孝成
#    修改内容：创建
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
VT_START_INDEX = 0L
VT_QUERY_COUNT = 50L
VT_DISABLE = bool(0)
VT_ENABLE = bool(1)
QUERY_USER_CREAT_VT = 1
QUERY_USER_VISIBLE_VT = 2
QUERY_BY_VT_NAME = 3
QUERY_FIRST_NIC = 0
QUERY_SECOND_NIC = 1
NO_VT_NAME = ""
#****************************************************************************#
class vmtemplate:
    ''' tecs vmtemplate methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session
        
            
    def allocate(self,cfg):
        result = self.apisvr.tecs.vmtemplate.allocate(self.session,cfg) 
        if result[0] != 0:
            print "failed to create vmt! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create vmt %s!" % cfg["vt_name"]
            return True

    def set(self,cfg):
        result = self.apisvr.tecs.vmtemplate.set(self.session,cfg) 
        if result[0] != 0:
            print "failed to set vmt! error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set vmt %s!" % cfg["vt_name"]
            return True           
            
    def del_vmt(self,vt_name):
        result = self.apisvr.tecs.vmtemplate.delete(self.session,vt_name) 
        if result[0] != 0:
            print "failed to del vmt error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to del vmt !"
            return True
           
    def publish_vmt(self,vt_name):
        result = self.apisvr.tecs.vmtemplate.publish(self.session,vt_name,bool(1)) 
        if result[0] != 0:
            print "failed to publish vmt error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to publish vmt !"
            return True
            
    def unpublish_vmt(self,vt_name):
        result = self.apisvr.tecs.vmtemplate.publish(self.session,vt_name,bool(0)) 
        if result[0] != 0:
            print "failed to unpublish vmt error code = %d" % (result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to unpublish vmt !"
            return True
            
    def show_vmt_info(self,type,vt_name):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmtemplate.query(self.session,VT_START_INDEX,VT_QUERY_COUNT,type,vt_name) 
        if result[0] != 0:
            print "failed to query vmt information, type %d! error code =%d" % (type,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            if len(result[3]) == 0:
                print "no vmt type %d!" % type
                return True;
            
            vtcfg_list = result[3]
            for v in vtcfg_list:
                print "==========================="
                print "vmt id = %d" % v["vt_id"]
                print "vmt name = %s" % v["vt_name"]
                print "public is = %d" % v["is_public"]
                print "uid = %s" % v["uid"]
                print "owner = %s" % v["user_name"]
                print "created at = %s" % v["create_time"]
                
                #print "vm name = %s" % v["cfg_info"]["vm_name"]
                #print "project_name = %s" % v["cfg_info"]["project_name"]
              
                
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
                print "------%s ended------" % n
            elif type(v) == type({}):
                print "------%s start------" % n
                self.print_map(v)
                print "------%s ended------" % n
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

    def query_count(self,type,vt_name):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmtemplate.query(self.session,VT_START_INDEX,VT_QUERY_COUNT,type,vt_name) 
        if result[0] != 0:
            print "failed to query vmt information, type %d! error code =%d" % (type,result[0])
            print "error string = %s" % result[1]
            return None

        print "vmtemplate information:" 
        print "next_index = %d" % result[1]
        print "max_count = %d" % result[2]
        vmcfg_list = result[3]
        self.print_vmcfg_list(vmcfg_list)

        return result[2]

    def query_disks_count(self,type,vt_name):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmtemplate.query(self.session,VT_START_INDEX,VT_QUERY_COUNT,type,vt_name) 
        if result[0] != 0:
            print "failed to query vmt information, type %d! error code =%d" % (type,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vmt type %d!" % type
                return count;

            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)
            count = 0;
            for v in vmcfg_list:             
                disk_list = v["base_info"]["disks"]
                for d in disk_list:
                    count = count + 1
            print "count = %d" % count
            return count  
                    
    def query_nic_count(self,type,vt_name):
        '''显示指定的虚拟机配置信息  '''
        result = self.apisvr.tecs.vmtemplate.query(self.session,VT_START_INDEX,VT_QUERY_COUNT,type,vt_name) 
        if result[0] != 0:
            print "failed to query vmt information, type %d! error code =%d" % (type,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vmt type %d!" % type
                return count;
            
            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)

            for v in vmcfg_list:
                nic_list = v["base_info"]["nics"]
                for n in nic_list:
                    count = count + 1
            return count

    def query_value(self,type,vt_name,query_name):
        result = self.apisvr.tecs.vmtemplate.query(self.session,VT_START_INDEX,VT_QUERY_COUNT,type,vt_name) 
        if result[0] != 0:
            print "failed to query vmt information, type %d! error code =%d" % (type,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vmt type %d!" % type
                return None;
            
            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)

            for v in vmcfg_list:
                return v[query_name]
            return None


    def query_base_info_value(self,query_type,vt_name,query_name,query_name2="",which=0):
        result = self.apisvr.tecs.vmtemplate.query(self.session,VT_START_INDEX,VT_QUERY_COUNT,query_type,vt_name) 
        if result[0] != 0:
            print "failed to query vmt information, type %d! error code =%d" % (query_type,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            count = 0;
            if len(result[3]) == 0:
                print "no vmt type %d!" % query_type
                return None;
            
            vmcfg_list = result[3]
            self.print_vmcfg_list(vmcfg_list)
            for v in vmcfg_list:
                v_type = type(v["base_info"][query_name])
                if v_type == type([]):
                    v_map_list = v["base_info"][query_name]
                    for v_map in v_map_list:
                        print which
                        print"v_map=%s" %v_map
                        if which == 0:
                            return v_map[query_name2]
                        else:
                            which = which -1
                elif v_type == type({}): 
                    v_map = v["base_info"][query_name]                    
                    return v_map[query_name2]
                else:                    
                    return v["base_info"][query_name]

            return None

    def query_pub(self,type,vt_name):
        return self.query_value(type,vt_name,"is_public")          
                   
    def query_vcpu(self,type,vt_name):
        return self.query_base_info_value(type,vt_name,"vcpu")
        
    def query_description(self,type,vt_name):
        return self.query_base_info_value(type,vt_name,"description")
        
    def query_vnc_passwd(self,type,vt_name):
        return self.query_base_info_value(type,vt_name,"vnc_passwd")
        
    def query_virt_type(self,type,vt_name):
        return self.query_base_info_value(type,vt_name,"virt_type")

    def query_memory(self,type,vt_name):
        return self.query_base_info_value(type,vt_name,"memory")     
    
    def query_vm_wdtime(self,type,vt_name):
        return self.query_base_info_value(type,vt_name,"vm_wdtime")   

    def query_machine_value(self,type,vt_name,query_name):
        return self.query_base_info_value(type,vt_name,"machine",query_name) 

    def query_machine_bus(self,type,vt_name):
        return self.query_machine_value(type,vt_name,"bus") 

    def query_disks_value(self,type,vt_name,query_name):
        return self.query_base_info_value(type,vt_name,"disks",query_name) 

    def query_disks_size(self,type,vt_name):
        return self.query_disks_value(type,vt_name,"size") 

    def query_disks_bus(self,type,vt_name):
        return self.query_disks_value(type,vt_name,"bus") 

    def query_disks_target(self,type,vt_name):
        return self.query_disks_value(type,vt_name,"target") 

    def query_disks_fstype(self,type,vt_name):
        return self.query_disks_value(type,vt_name,"fstype") 

    def query_nics_value(self,type,vt_name,query_name,which):
        return self.query_base_info_value(type,vt_name,"nics",query_name,which) 

    def query_nics_index(self,type,vt_name,which):
        return self.query_nics_value(type,vt_name,"nic_index",which) 
                    
    
