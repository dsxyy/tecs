#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_014_vm.py
# 测试内容：
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
# 修改记录3：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息

#*******************************************************************************/
import sys,os,getopt,time,inspect
import hashlib
from ftplib import FTP


from base_interface.rpc.vmtemplate import *


class vmt_module:
    def __init__(self, host, session):       
        self.method = vmtemplate(host, session)
        self.cfg = {}

# 添加当前类的虚拟机基本配置        
    def add_base(self, vt_name, tcu, vcpu, memery, imgid, postion):
        cfg_base = {}
        cfg_base["vcpu"] = vcpu
        cfg_base["tcu"] = tcu
        cfg_base["memory"] = long(memery * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":imgid,"size":0L,"bus":"ide","type":"machine","target":"hda","position":postion}
        cfg_base["virt_type"] = "hvm"
        cfg_base["hypervisor"] = "xen"		
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = []
        cfg_base["nics"] = []
        
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vt_name"] = vt_name
        cfg["is_public"] = bool(1)
        self.cfg = cfg    
        self.vt_name = vt_name
 
# 添加当前类的磁盘配置        
    def add_disk(self, size, bus_type, target, postion, fstype):
        disk = {"id":-1L,"size":long(size * 1024 * 1024),"bus":bus_type,"type":"disk","target":target,"position":postion,"fstype":fstype}
        self.cfg["base_info"]["disks"].append(disk)

# 清除当前类的磁盘配置        
    def clear_disk(self):
        self.cfg["base_info"]["disks"] = []

# 添加当前类的网口配置    
    def add_nic(self, net_plane, dhcp_enable):
        nic = {"pci_order":0,"plane":net_plane,"vlan":0,"dhcp":bool(dhcp_enable),"ip":"","netmask":"","gateway":"","mac":""}
        self.cfg["base_info"]["nics"].append(nic)

# 清除当前类的网口配置        
    def clear_nic(self):
        self.cfg["base_info"]["nics"] = []

# 添加当前类的虚拟机上下文        
    def add_context(self, key, value):
        self.cfg["base_info"]["context"][key] = value

# 清除当前类的虚拟机上下文        
    def clear_context(self):
        self.cfg["base_info"]["context"].clear()

# 清除当前类的记录的所有虚拟机模板设备配置
    def clear_all_device(self):
        self.clear_disk()
        self.clear_nic()
        self.clear_context()
        self.clear_qualifications()

# 基于当前类创建虚拟机模板,失败返回-1    
    def create(self):
        self.print_cfg()
        self.method.allocate(self.cfg)
        
# 打印当前类的配置
    def print_cfg(self):
        print("%s" % self.cfg)

# 修改虚拟机     
    def modify(self,cfg):
        print("%s" % cfg)
        return self.method.set(cfg)

# 创建模版     
    def allocate(self,cfg):
        print("%s" % cfg)
        return self.method.allocate(cfg)

# 查询模版     
    def query(self, type, vt_name):
        return self.method.show_vmt_info(type, vt_name)

# 修改虚拟机模板名称
    def set_vt_name(name):
        self.vt_name = name
        
# 发布   
    def publish(self):
        return self.method.publish_vmt(self.vt_name)

# 取消发布   
    def publish(self):
        return self.method.unpublish_vmt(self.vt_name)
        
# 删除        
    def delete(self):
        if self.method.del_vmt(self.vt_name, "delete"):
            self.vt_name = ""
            return True
        return False              