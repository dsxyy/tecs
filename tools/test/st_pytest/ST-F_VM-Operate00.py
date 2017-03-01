#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：ST-F_VM-Operate00.py
# 内容摘要：虚拟机操作的公共基本流程。
# 作    者：pengwei
# 完成日期：2012/5/21
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecs目标测试机地址和用户            
server_addr = "localhost"
account_user = "st_guest"
#1号虚拟机
vm_id0 = long(1)

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

#前提条件（1号虚拟机正在运行）
if v.get_vm_state(vm_id0) != 2:
    #如果1号虚拟机状态不为runing，或者不存在，直接创建一个新虚拟机
    print "vm %d status is not running!" % vm_id0
    encrypted_account_session, server, vm_id0 = create_vm(server_addr,account_user, "test01","ttylinux0.img")
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_04-03-VM-VmOperate-0001  关闭虚拟机
#用例开始
result = True
#查询1号虚拟机状态应该为2(running), (6为未知)
count = 0
vm_state = v.get_vm_state(vm_id0)   
while (vm_state !=2 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    print "[DEBUG],now vm_state is %d" %vm_state
    count = count + 1
    if(count >= 2):
        #等待半分钟获取1号虚拟机状态不为running，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0001: failed to query vm %d of running!" % vm_id0)
        result = False
        break
#关闭running状态的1号虚拟机
if (vm_state == 2):
    if v.action(vm_id0, "stop") != True:
        #关闭虚拟机失败，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0001: failed to stop vm %d!" % vm_id0)
        result = False
    else:
        #关闭操作后，再查询1号虚拟机状态应该为4（shutoff）
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 4 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after stop, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 8):
                #等待1分半钟获取1号虚拟机状态不为shutoff，则执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0001: failed to query vm %d of shutoff!" % vm_id0)
                result = False
                break
#1号虚拟机状态为shutdown，则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0001: success to stop vm %d!" % vm_id0)
#用例结束


#STC-F-TECS_04-03-VM-VmOperate-0002  启动虚拟机
#用例开始
result = True
#查询1号虚拟机状态应该为4(shutoff)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 4 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 2):
        #等待半分钟获取1号虚拟机状态不为shutoff，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0002: failed to query vm %d of shutoff!" % vm_id0)
        result = False
        break
#启动shutoff状态的1号虚拟机
if (vm_state == 4):
    if v.action(vm_id0, "start") != True:
        #启动虚拟机失败，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0002: failed to start vm %d!" % vm_id0)
        result = False
    else:
        #启动操作后，查询1号虚拟机状态应改为2（running）
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after start, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟获取1号虚拟机状态仍未变为running，则执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0002: failed to query vm %d of running!" % vm_id0)
                result = False
                break
#1号虚拟机状态为running，则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0002: success to start vm %d!" % vm_id0)
#用例结束


#STC-F-TECS_04-03-VM-VmOperate-0003  暂停虚拟机
#用例开始
result = True
#查询1号虚拟机状态应该为2(running)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 2 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 2):
        #等待半分钟获取1号虚拟机状态不为running，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0003: failed to query vm %d of running!" % vm_id0)
        result = False
        break
#暂停running状态的1号虚拟机
if (vm_state == 2):
    if v.action(vm_id0, "pause") != True:
        #暂停虚拟机失败，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0003: failed to pause vm %d!" % vm_id0)
        result = False
    else:
        #暂停操作后，查询1号虚拟机状态应改为3（paused）
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 3 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after pause, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟获取1号虚拟机状态仍未变为paused，则执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0003: failed to query vm %d of paused!" % vm_id0)
                result = False
                break
#1号虚拟机状态为paused，则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0003: success to pause vm %d!" % vm_id0)
#用例结束


#STC-F-TECS_04-03-VM-VmOperate-0004  恢复虚拟机
#用例开始
result = True
#查询1号虚拟机状态应该为3(paused)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 3 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 2):
        #等待半分钟获取1号虚拟机状态不为paused，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0004: failed to query vm %d of paused!" % vm_id0)
        result = False
        break
#恢复paused状态的1号虚拟机
if (vm_state == 3):
    if v.action(vm_id0, "resume") != True:
        #恢复虚拟机失败，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0004: failed to resume vm %d!" % vm_id0)
        result = False
    else:
        #恢复操作后，查询1号虚拟机状态应改为2（running）
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after resume, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟获取1号虚拟机状态仍未变为running，则执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0004: failed to query vm %d of running!" % vm_id0)
                result = False
                break
#1号虚拟机状态为paused，则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0004: success to resume vm %d!" % vm_id0)
#用例结束


#STC-F-TECS_04-03-VM-VmOperate-0005  重启虚拟机
#用例开始
result = True
#查询1号虚拟机状态应该为2(running)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 2 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 5):
        #等待1分钟获取1号虚拟机状态不为paused，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0005: failed to query vm %d of running!" % vm_id0)
        result = False
        break
#恢复paused状态的1号虚拟机
if (vm_state == 2):
    if v.action(vm_id0, "reboot") != True:
        #重启虚拟机失败，则执行失败记录日志
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0005: failed to reboot vm %d!" % vm_id0)
        result = False
    else:
        #重启操作后，查询1号虚拟机状态应改为2（running）
        #先延长30s再查询
        time.sleep(30)
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after reboot, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 8):
                #等待1分半获取1号虚拟机状态仍未变为running，则执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0005: failed to query vm %d of running!" % vm_id0)
                result = False
                break
#1号虚拟机状态为paused，则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0005: success to reboot vm %d!" % vm_id0)
#用例结束


st_log(2,'',"END")
