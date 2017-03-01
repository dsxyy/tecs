#!/usr/bin/python
# -*- coding: gb2312 -*-
# 文件名称：st_ltest_002.py
# 内容摘要：服务器上执行的测试脚本
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

server_addr = "10.44.129.34"
guest_user = "admin"
__builtin__.LOG = server_addr.replace('.','_')

#class vmcfg1:
#    ''' tecs vmcfg methods '''
#    def __init__(self, server,session):
#        self.apisvr = server
#        self.session = session
#    def print_list(self,list):
#        for item in list:
#            if type(item) == type([]):
#                self.print_list(item)
#            elif type(item) == type({}):
#                self.print_map(item)        
#            else:
#                print "%s" % item
#    def print_map(self,map):
#        for n,v in map.items():
#            if type(v) == type([]):
#                print "------%s start------" % n
#                self.print_list(v)
#                print "------%s end------" % n
#            elif type(v) == type({}):
#                print "------%s start------" % n
#                self.print_map(v)
#                print "------%s end------" % n
#            else:
#                print "%s = %-20s" % (n,v)
#    def get_vm_num(self,vm_name):
#        '''根据指定的虚拟机名称查询虚拟机id '''
#        result = self.apisvr.tecs.vmcfg.query(self.session,1L,20L,-5L) 
#        if result[0] != 0:
#            print "failed to query vmcfg information of vm %s! error code =%d" % (vm_name,result[0])
#            print "error string = %s" % result[1]
#            return None
#        else:
#            vmcfg_list = result[3]
#            self.print_list(vmcfg_list)
#            for v in vmcfg_list:
#                if v["vm_name"] == vm_name:
#                    print "succeed to get vm id = %d" % long(v["base_info"]["id"])
#                    return long(v["base_info"]["id"])
#            print "failed to get vm id!"
#            return None

#img = "windowsxp.img"
img = "ttylinux0.img"
vmname = "tty"
encrypted_account_session, server = login_user(server_addr,guest_user)
#i = image(server, encrypted_account_session)
#img_id = i.get_id_by_filename(img)
#print "................"
#i.query(0L,5L,-4,guest_user,img_id)
#upload_img(server_addr,guest_user,img)
#reg_img(server_addr,guest_user,img)
#encrypted_account_session, server, vm_id = cfg_vm(server_addr,guest_user,vmname,img)
#v = vmcfg(server, encrypted_account_session)
##v.get_id_by_name(-5L, vmname, guest_user)
##print "~~~~~~~~~~~~~~~~~~~~~~~~~"
##vm_id = v.get_vm_id(vmname) 
##print vm_id
##print ".........................."
##v.query(vm_id)
##print ".........................."
##v1 =vmcfg1(server, encrypted_account_session)
##v1.get_vm_num(vm_id)
#state = v.get_vm_state(vm_id)
#print state
#encrypted_account_session, server, vm_id = create_vm(server_addr,guest_user,vmname,img)
#print "=============================================="
#cm = clustermanager(server,encrypted_account_session) 
#
#cm.query(0L,100L,0)

h = host(server,encrypted_account_session)
state = h.query_register_state("st_cluster", "st_host")
print state
# 设置svcpu 
print "=============================================="
# 获取刚刚注册的主机的cpu id   
cpu_id = h.get_cpu_id("st_cluster", "st_host")    
if cpu_id is None:
    st_exit(-1, lines())
print "cpu id of host %s = %d" % ("st_host",cpu_id)    

s = svcpu(server,encrypted_account_session)
if s.set("st_cluster","",cpu_id,4,"test cpu description") != True:
    st_exit(-1, lines())
