#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_014_vm.py
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
# �޸ļ�¼2��
#    �޸����ڣ�2012/06/8
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ѩ��
#    �޸����ݣ�ģ����Ժ���1
# �޸ļ�¼3��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ

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

# ��ӵ�ǰ����������������        
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
 
# ��ӵ�ǰ��Ĵ�������        
    def add_disk(self, size, bus_type, target, postion, fstype):
        disk = {"id":-1L,"size":long(size * 1024 * 1024),"bus":bus_type,"type":"disk","target":target,"position":postion,"fstype":fstype}
        self.cfg["cfg_info"]["base_info"]["disks"].append(disk)

# �����ǰ��Ĵ�������        
    def clear_disk(self):
        self.cfg["cfg_info"]["base_info"]["disks"] = []

# ��ӵ�ǰ�����������    
    def add_nic(self, net_plane, dhcp_enable):
        nic = {"pci_order":0,"plane":net_plane,"vlan":0,"dhcp":bool(dhcp_enable),"ip":"","netmask":"","gateway":"","mac":""}
        self.cfg["cfg_info"]["base_info"]["nics"].append(nic)

# �����ǰ�����������        
    def clear_nic(self):
        self.cfg["cfg_info"]["base_info"]["nics"] = []

# ��ӵ�ǰ��������������        
    def add_context(self, key, value):
        self.cfg["cfg_info"]["base_info"]["context"][key] = value

# �����ǰ��������������        
    def clear_context(self):
        self.cfg["cfg_info"]["base_info"]["context"].clear()

# ��ӵ�ǰ�������������ϵ        
    def add_qualifications(self, vm_id):
        self.cfg["cfg_info"]["base_info"]["qualifications"].append(vm_id)
        
# ��������coredump��־
    def enable_vm_crash_coredump(self):
        self.cfg["cfg_info"]["base_info"]["enable_coredump"] = bool(1)
        
# ɾ�������coredump��־
    def disable_vm_crash_coredump(self):
        self.cfg["cfg_info"]["base_info"]["enable_coredump"] = bool(0)

# �����ǰ�������������ϵ        
    def clear_qualifications(self):
        self.cfg["cfg_info"]["base_info"]["qualifications"] = []

# �����ǰ��ļ�¼������������豸����
    def clear_all_device(self):
        self.clear_disk()
        self.clear_nic()
        self.clear_context()
        self.clear_qualifications()

# ���ڵ�ǰ�ഴ�������,ʧ�ܷ���-1    
    def create(self):
        print self.cfg
        self.vm_id = self.method.allocate(self.cfg)
        return self.vm_id

# ��ȡ�������XEND�����ʵ������
    def get_vm_instance_name(self):
        return "instance_%d" % self.vm_id
    
# ���������ģ�崴�������
    def creat_by_vt(self,vt_name,project_name,vm_name,vnc_passwd):
        self.vm_id =   self.method.allocate_by_vt(vt_name, project_name, vm_name, vnc_passwd, 1)   
        return self.vm_id 
        
# ��ѯǰ̨����������õ���ǰ������
    def query_vm(self):
        self.cfg = self.method.query(self.vm_id)
        
# ��ӡ��ǰ�������
    def print_cfg(self):
        print("%s" % self.cfg)

# �޸������     
    def modify(sef):
        self.method.set(self.cfg)

# ���������id, һ�������޸ģ����߲���һ�����ڵ������id       
    def set_vmid(self, vm_id):
        self.vm_id = long(vm_id)
        return True

# ����    
    def deploy(self):
        return self.method.action(self.vm_id, "deploy")

# ����
    def start(self):
        return self.method.action(self.vm_id, "start")

# ���������        
    def reboot(self):
        return self.method.action(self.vm_id, "reboot")

# ǿ�����������        
    def reset(self):
        return self.method.action(self.vm_id, "reset")  

# �������������        
    def pause(self):
        return self.method.action(self.vm_id, "pause")  

# �ָ����������        
    def resume(self):
        return self.method.action(self.vm_id, "resume")

# �ػ�        
    def stop(self):
        return self.method.action(self.vm_id, "stop")  

# ǿ�ƹػ�        
    def destory(self):
        return self.method.action(self.vm_id, "destroy")  

# ȡ�����������        
    def cancel(self):
        return self.method.action(self.vm_id, "cancel")  

# ���������Ϊӳ��        
    def upload_img(self):
        return self.method.action(self.vm_id, "upload_img")    

# ȡ�������ӳ���ϴ�        
    def cancel_upload_img(self):
        return self.method.action(self.vm_id, "cancel_upload_img")

# ɾ�����������        
    def delete(self):
        if self.method.action(self.vm_id, "delete"):
            self.vm_id = -1
            return True
        return False              
# ���ڵ�ǰ�ഴ���ƶ���,ʧ�ܷ���False,�ɹ�����request_id  
    def create_portable_disk(self, volsize, cluster_name, description):
        self.request_id = self.portable.create(volsize, cluster_name, description)
        if False != self.request_id:
            return self.request_id
        return False

# ���ڵ�ǰ��ɾ���ƶ���,ʧ�ܷ���False,�ɹ�����True   
    def delete_portable_disk(self, request_id):
        return self.portable.delete(request_id)

# ���ڵ�ǰ��ɾ���ƶ���,ʧ�ܷ���False,�ɹ�����True   
    def attach_portable_disk(self, request_id, vid, target, bus):
        return self.portable.attach(request_id, vid, target, bus)

# ���ڵ�ǰ��ɾ���ƶ���,ʧ�ܷ���False,�ɹ�����True   
    def detach_portable_disk(self, vid, request_id):
        return self.portable.detach(vid, request_id)
        
# �ָ���������񱸷�,ʧ�ܷ���False,�ɹ�����True   
    def restore_image_backup(self, vid, request_id, target):
        return self.method.restore_image_backup(vid, request_id, target)

# ���񴴽�           
    def create_image_backup(self, vid, target):
        return self.method.create_image_backup(vid, target)

# ���񴴽�           
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
     
