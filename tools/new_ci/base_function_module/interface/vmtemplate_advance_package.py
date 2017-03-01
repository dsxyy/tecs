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


from base_interface.rpc.vmtemplate import *


class vmt_module:
    def __init__(self, host, session):       
        self.method = vmtemplate(host, session)
        self.cfg = {}

# ��ӵ�ǰ����������������        
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
 
# ��ӵ�ǰ��Ĵ�������        
    def add_disk(self, size, bus_type, target, postion, fstype):
        disk = {"id":-1L,"size":long(size * 1024 * 1024),"bus":bus_type,"type":"disk","target":target,"position":postion,"fstype":fstype}
        self.cfg["base_info"]["disks"].append(disk)

# �����ǰ��Ĵ�������        
    def clear_disk(self):
        self.cfg["base_info"]["disks"] = []

# ��ӵ�ǰ�����������    
    def add_nic(self, net_plane, dhcp_enable):
        nic = {"pci_order":0,"plane":net_plane,"vlan":0,"dhcp":bool(dhcp_enable),"ip":"","netmask":"","gateway":"","mac":""}
        self.cfg["base_info"]["nics"].append(nic)

# �����ǰ�����������        
    def clear_nic(self):
        self.cfg["base_info"]["nics"] = []

# ��ӵ�ǰ��������������        
    def add_context(self, key, value):
        self.cfg["base_info"]["context"][key] = value

# �����ǰ��������������        
    def clear_context(self):
        self.cfg["base_info"]["context"].clear()

# �����ǰ��ļ�¼�����������ģ���豸����
    def clear_all_device(self):
        self.clear_disk()
        self.clear_nic()
        self.clear_context()
        self.clear_qualifications()

# ���ڵ�ǰ�ഴ�������ģ��,ʧ�ܷ���-1    
    def create(self):
        self.print_cfg()
        self.method.allocate(self.cfg)
        
# ��ӡ��ǰ�������
    def print_cfg(self):
        print("%s" % self.cfg)

# �޸������     
    def modify(self,cfg):
        print("%s" % cfg)
        return self.method.set(cfg)

# ����ģ��     
    def allocate(self,cfg):
        print("%s" % cfg)
        return self.method.allocate(cfg)

# ��ѯģ��     
    def query(self, type, vt_name):
        return self.method.show_vmt_info(type, vt_name)

# �޸������ģ������
    def set_vt_name(name):
        self.vt_name = name
        
# ����   
    def publish(self):
        return self.method.publish_vmt(self.vt_name)

# ȡ������   
    def publish(self):
        return self.method.unpublish_vmt(self.vt_name)
        
# ɾ��        
    def delete(self):
        if self.method.del_vmt(self.vt_name, "delete"):
            self.vt_name = ""
            return True
        return False              