#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：ST-F_VM-Operate02.py
# 内容摘要：虚拟机操作的稳定性测试。
# 作    者：pengwei
# 完成日期：2012/6/1
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecs目标测试机地址和用户            
server_addr = "localhost"
account_user = "st_guest"

vmname1="test_s_01"
vmname2="test_s_02"
img="ttylinux0.img"

# 参数解析
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-u", "--user"):
        account_user = v
__builtin__.LOG = server_addr.replace('.','_')

#登录映像管理模块
encrypted_account_session, server = login_user(server_addr,account_user)
v = vmcfg(server,encrypted_account_session)

#前提条件，配置一个新的虚拟机
vm_id1 = v.get_id_by_name(-5L, vmname1, account_user)
if (vm_id1 == None) or (bool(vm_id1) is False):
    encrypted_account_session, server, vm_id1 = cfg_vm(server_addr,account_user, vmname1, img)
else:
    vm_id1=max(vm_id1)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-S-TECS_04-03-VM-VmOperate-0001  反复部署和撤消一个虚拟机
#用例开始
result = True
loop_times = 2
#重复部署、撤消虚拟机2次
for loop in range(loop_times):
    print "Loop  No. %d start ...... "%loop
    #部署新虚拟机
    if v.action(vm_id1,"deploy") != True:
        #部署操作失败，则执行失败记录日志
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:faile to deploy vm %s!"% vm_id1)
    else:
        #部署成功后，查询虚拟机状态
        vm_state = v.get_vm_state(vm_id1)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id1)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 14):
                #等待2分半钟虚拟机查询状态不为2，则执行失败记录日志
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:failed to deploy vm %s!"% vm_id1)
                break
    print "the vm %s(%s) is runnig!"%(vmname1, vm_id1)
    #查询状态正确，虚拟机部署成功后正确运行，再撤消虚拟机
    if v.action(vm_id1,"cancel") != True:
        #撤消操作失败，则执行失败记录日志
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:faile to cancel vm %s!"% vm_id1)
    else:
        #撤消操作成功后，查询虚拟机状态
        vm_state = v.get_vm_state(vm_id1)  
        count = 0
        while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id1)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟虚拟机查询状态不为0，则执行失败记录日志
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:failed to cancel vm %s!"% vm_id1)
                break
    print "the vm %s(%s) is configuration!"%(vmname1, vm_id1)
    #撤消成功后，等待3分钟的资源回收
    print "wait 200s" 
    time.sleep(200)
    print "Loop  No. %d end ...... "%loop
#再次部署虚拟机、撤消，进行循环操作成功，则执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-S-TECS_04-03-VM-VmOperate-0001: success to cancel vm %d of running!" % vm_id1)
#用例结束


#STC-S-TECS_04-03-VM-VmOperate-0002 反复部署和撤消虚拟机
#用例开始
result = True
loop_times = 2
#重复部署、撤消虚拟机2次
for loop in range(loop_times):
    print "Loop  No. %d start ...... "%loop
    #每次都配置一个新的虚拟机test_s_01（因为同一个HOST不能撤消/部署同一个虚拟机）
    encrypted_account_session, server, vm_id2 = cfg_vm(server_addr,account_user, vmname2, img)
    #部署新虚拟机
    if v.action(vm_id2,"deploy") != True:
        #部署操作失败，则执行失败记录日志
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:faile to deploy vm %s!"% vm_id2)
    else:
        #部署成功后，查询虚拟机状态
        vm_state = v.get_vm_state(vm_id2)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id2)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 14):
                #等待2分半钟虚拟机查询状态不为2，则执行失败记录日志
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:failed to deploy vm %s!"% vm_id2)
                break
    print "the vm %s(%s) is runnig!"%(vmname2, vm_id2)
    #查询状态正确，虚拟机部署成功后正确运行，再撤消虚拟机
    if v.action(vm_id2,"cancel") != True:
        #撤消操作失败，则执行失败记录日志
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:faile to cancel vm %s!"% vm_id2)
    else:
        #撤消操作成功后，查询虚拟机状态
        vm_state = v.get_vm_state(vm_id2)  
        count = 0
        while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id2)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟虚拟机查询状态不为0，则执行失败记录日志
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:failed to cancel vm %s!"% vm_id2)
                break
    print "the vm %s(%s) is configuration!"%(vmname2, vm_id2)
    #撤消成功后，等待3分钟的资源回收
    print "wait 200" 
    time.sleep(200)
    print "Loop  No. %d end ...... "%loop
#再次部署虚拟机、撤消，进行循环操作成功，则执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-S-TECS_04-03-VM-VmOperate-0002: success to cancel vm %d of running!" % vm_id2)
#用例结束


st_log(2,'',"END")
