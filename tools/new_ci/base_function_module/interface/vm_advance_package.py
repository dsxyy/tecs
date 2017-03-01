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

from base_interface.rpc.user import *
from base_interface.rpc.usergroup import *
from base_interface.rpc.clustermanager import *
from base_interface.rpc.host import *
from base_interface.rpc.tcu import *
from base_interface.rpc.image import *
from base_interface.rpc.vmcfg import *
from base_interface.rpc.project import *
from base_interface.rpc.file import *
from base_interface.rpc.vmtemplate import *
from base_interface.rpc.tecssystem import *
from base_interface.rpc.netplane_default import *
from base_interface.rpc.netplane import*
from base_interface.rpc.port import *
from base_interface.rpc.portable_disk import *

class vm_module:
    def __init__(self, host, session):       
        self.method = vmcfg(host, session)
        self.vm_id = -1L
        self.request_id = False
        self.portable = portabledisk(host, session)
        self.cfg = {}

# 添加当前类的虚拟机基本配置        
    def add_base(self, vm_name, tcu, vcpu, memery, imgid, project_name, postion):
        cfg_base = {}
        cfg_base["description"] = ""
        cfg_base["vcpu"] = vcpu
        cfg_base["tcu"] = tcu
        cfg_base["memory"] = long(memery * 1024 * 1024)
        cfg_base["machine"] = {"id":imgid,"size":0L,"bus":"ide","type":"machine","target":"hda","position":postion}
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["bootloader"] = ""
        cfg_base["virt_type"] = "hvm"
        cfg_base["hypervisor"] = "any"
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = []
        cfg_base["nics"] = []
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["vnc_passwd"] = ""
        cfg_base["enable_coredump"] = False
        cfg_base["enable_serial"] = False
        cfg_base["devices"] = []
        cfg_base["enable_livemigrate"] = True
        
        
        cfg_info = {}
        cfg_info["base_info"] = cfg_base
        cfg_info["vm_name"] = vm_name
        cfg_info["project_name"] = project_name

        cfg = {}
        cfg["cfg_info"] = cfg_info
        cfg["vm_num"] = 1 
        self.cfg = cfg  
 
# 添加当前类的磁盘配置        
    def add_disk(self, size, bus_type, target, postion, fstype):
        disk = {"id":-1L,"size":long(size * 1024 * 1024),"bus":bus_type,"type":"disk","target":target,"position":postion,"fstype":fstype}
        self.cfg["cfg_info"]["base_info"]["disks"].append(disk)

# 清除当前类的磁盘配置        
    def clear_disk(self):
        self.cfg["cfg_info"]["base_info"]["disks"] = []

# 添加当前类的网口配置    
    def add_nic(self, net_plane, dhcp_enable):
        nic = {"pci_order":0,"plane":net_plane,"vlan":0,"dhcp":bool(dhcp_enable),"ip":"","netmask":"","gateway":"","mac":""}
        self.cfg["cfg_info"]["base_info"]["nics"].append(nic)

# 清除当前类的网口配置        
    def clear_nic(self):
        self.cfg["cfg_info"]["base_info"]["nics"] = []

# 添加当前类的虚拟机上下文        
    def add_context(self, key, value):
        self.cfg["cfg_info"]["base_info"]["context"][key] = value

# 清除当前类的虚拟机上下文        
    def clear_context(self):
        self.cfg["cfg_info"]["base_info"]["context"].clear()

# 添加当前类的虚拟机互斥关系        
    def add_qualifications(self, vm_id):
        self.cfg["cfg_info"]["base_info"]["qualifications"].append(vm_id)
        
# 添加虚拟机coredump标志
    def enable_vm_crash_coredump(self):
        self.cfg["cfg_info"]["base_info"]["enable_coredump"] = bool(1)
        
# 删除虚拟机coredump标志
    def disable_vm_crash_coredump(self):
        self.cfg["cfg_info"]["base_info"]["enable_coredump"] = bool(0)

# 清除当前类的虚拟机互斥关系        
    def clear_qualifications(self):
        self.cfg["cfg_info"]["base_info"]["qualifications"] = []

# 清除当前类的记录的所有虚拟机设备配置
    def clear_all_device(self):
        self.clear_disk()
        self.clear_nic()
        self.clear_context()
        self.clear_qualifications()

# 基于当前类创建虚拟机,失败返回-1    
    def create(self):
        print self.cfg
        self.vm_id = self.method.allocate(self.cfg)
        return self.vm_id

# 获取虚拟机在XEND上面的实例命令
    def get_vm_instance_name(self):
        return "instance_%d" % self.vm_id
    
# 基于虚拟机模板创建虚拟机
    def creat_by_vt(self,vt_name,project_name,vm_name,vnc_passwd):
        self.vm_id =   self.method.allocate_by_vt(vt_name, project_name, vm_name, vnc_passwd, 1)   
        return self.vm_id 
        
# 查询前台虚拟机的配置到当前类中来
    def query_vm(self):
        self.cfg = self.method.query(self.vm_id)
        
# 打印当前类的配置
    def print_cfg(self):
        print("%s" % self.cfg)

# 修改虚拟机     
    def modify(sef):
        self.method.set(self.cfg)

# 设置虚拟机id, 一般用在修改，或者操作一个存在的虚拟机id       
    def set_vmid(self, vm_id):
        self.vm_id = long(vm_id)
        return True

# 部署    
    def deploy(self):
        return self.method.action(self.vm_id, "deploy")

# 开机
    def start(self):
        return self.method.action(self.vm_id, "start")

# 重启虚拟机        
    def reboot(self):
        return self.method.action(self.vm_id, "reboot")

# 强制重启虚拟机        
    def reset(self):
        return self.method.action(self.vm_id, "reset")  

# 挂起虚拟机运行        
    def pause(self):
        return self.method.action(self.vm_id, "pause")  

# 恢复虚拟机运行        
    def resume(self):
        return self.method.action(self.vm_id, "resume")

# 关机        
    def stop(self):
        return self.method.action(self.vm_id, "stop")  

# 强制关机        
    def destory(self):
        return self.method.action(self.vm_id, "destroy")  

# 取消虚拟机部署        
    def cancel(self):
        return self.method.action(self.vm_id, "cancel")  

# 保存虚拟机为映像        
    def upload_img(self):
        return self.method.action(self.vm_id, "upload_img")    

# 取消虚拟机映像上传        
    def cancel_upload_img(self):
        return self.method.action(self.vm_id, "cancel_upload_img")

# 删除虚拟机配置        
    def delete(self):
        if self.method.action(self.vm_id, "delete"):
            self.vm_id = -1
            return True
        return False              
# 基于当前类创建移动盘,失败返回False,成功返回request_id  
    def create_portable_disk(self, volsize, cluster_name, description):
        self.request_id = self.portable.create(volsize, cluster_name, description)
        if False != self.request_id:
            return self.request_id
        return False

# 基于当前类删除移动盘,失败返回False,成功返回True   
    def delete_portable_disk(self, request_id):
        return self.portable.delete(request_id)

# 基于当前类删除移动盘,失败返回False,成功返回True   
    def attach_portable_disk(self, request_id, vid, target, bus):
        return self.portable.attach(request_id, vid, target, bus)

# 基于当前类删除移动盘,失败返回False,成功返回True   
    def detach_portable_disk(self, vid, request_id):
        return self.portable.detach(vid, request_id)
        
# 恢复虚拟机镜像备份,失败返回False,成功返回True   
    def restore_image_backup(self, vid, request_id, target):
        return self.method.restore_image_backup(vid, request_id, target)

# 镜像创建           
    def create_image_backup(self, vid, target):
        return self.method.create_image_backup(vid, target)

# 镜像创建           
    def delete_image_backup(self, vid, request_id):
        return self.method.delete_image_backup(vid, request_id)
        
    def vm_clone(self, vid):
        return self.method.clone(vid) 

    def vm_add_ar(self, cfg):
        return self.method.add_affinity_region(cfg) 

    def vm_del_ar(self, cfg):
        return self.method.del_affinity_region(cfg) 
        
    def allocate(self, cfg):
        print cfg
        return self.method.allocate(cfg)

    def allocatebyvt(self, cfg):
        print cfg
        return self.method.allocatebyvt(cfg)

    def query_staticinfo(self, type, vid):
        return self.method.query_static_info(type, vid)

    def query_runninginfo(self, type, vid):
        return self.method.query_running_info(type, vid)

    def set(self, cfg):
        print cfg
        return self.method.set(cfg)
   
    def vm_migrate(self, cfg):
        return self.method.migrate(cfg)   
     
