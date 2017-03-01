#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：ST-F_VM-Operate01.py
# 内容摘要：虚拟机操作的其他操作流程。
# 作    者：pengwei
# 完成日期：2012/5/23
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecs目标测试机地址和用户            
server_addr = "localhost"
account_user = "st_guest"

vmname="tty1"
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

#前提条件，创建3个虚拟机，状态分别为running，shutoff，pause
for i in range(3):
    print i
    encrypted_account_session, server, vm_id0=create_vm(server_addr,account_user,vmname,img)
    vm_state = v.get_vm_state(vm_id0)
    #print "[DEBUG], vm %d state is %d" %(vm_id0, vm_state)
#最后创建的虚拟机状态置为pause    
if v.action(vm_id0, "pause") != True:
    st_exit(-1, lines(), "vm %s status is not pause!"%vm_id0)
count=0
while(v.get_vm_state(vm_id0) !=3 and v.get_vm_state(vm_id0) !=None):
    time.sleep(10)
    count = count + 1
    if(count >= 5):
        #等待1分钟，如果虚拟机状态不为pause，则退出。
        st_exit(-1, lines(), "vm %s status is not pause!"%vm_id0)
print "vm %s status is pause!"%vm_id0
#倒数第二个虚拟机状态置为shutoff
vm_id1 = vm_id0 - 1
if v.action(vm_id1, "stop") != True:
    st_exit(-1, lines(), "vm %s status is not shutoff!"%vm_id1)
count=0
while(v.get_vm_state(vm_id1) !=4 and v.get_vm_state(vm_id1) !=None):
    time.sleep(10)
    count = count + 1
    if(count >= 8):
        #等待1分半钟，如果虚拟机状态不为shutoff，则退出。
        st_exit(-1, lines(), "vm %s status is not shutoff!"%vm_id1)
print "vm %s status is shutoff!"%vm_id1
#第一个创建的虚拟机状态为running
vm_id2 = vm_id0 - 2
if v.get_vm_state(vm_id2) != 2:
    st_exit(-1, lines(), "vm %s status is not running!"%vm_id2)
print "vm %s status is running!"%vm_id2
##############################################################
##############################################################
st_log(2,'',"START")


#STC-F-TECS_04-03-VM-VmOperate-0006  撤消部署关闭状态的虚拟机
#用例开始
result = True
#撤消部署关闭状态的虚拟机
if v.action(vm_id1, "cancel") != True:
    #撤消动作失败，则用例执行失败记录日志
    st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0006: failed to cancel vm %d of shutoff!" % vm_id1)
    result = False
#撤消成功后，查询虚拟机的状态
else:
    vm_state = v.get_vm_state(vm_id1)  
    count = 0
    # 获取虚拟机状态，应该为0 
    while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id1)
            print "[DEBUG],after cancel, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟虚拟机的状态不为0，则用例执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0006: failed to query vm %d of configuration!" % vm_id1)
                result = False
#虚拟机状态为0（configuration），则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0006: success to cancel vm %d of shutoff!" % vm_id1)
#用例结束


#STC-F-TECS_04-03-VM-VmOperate-0007  撤消部署暂停状态的虚拟机
#用例开始
result = True
#撤消部署暂停状态的虚拟机
if v.action(vm_id0, "cancel") != True:
    #撤消动作失败，则用例执行失败记录日志
    st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0007: failed to cancel vm %d of pause!" % vm_id0)
    result = False
#撤消成功后，查询虚拟机的状态
else:
    vm_state = v.get_vm_state(vm_id0)  
    count = 0
    # 获取虚拟机状态，应该为0 
    while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after cancel, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟虚拟机的状态不为0，则用例执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0007: failed to query vm %d of configuration!" % vm_id0)
                result = False
#虚拟机状态为0（configuration），则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0007: success to cancel vm %d of pause!" % vm_id0)
#用例结束


#STC-F-TECS_04-03-VM-VmOperate-0008  撤消部署运行状态的虚拟机
#用例开始
result = True
#撤消部署运行状态的虚拟机
if v.action(vm_id2, "cancel") != True:
    #撤消动作失败，则用例执行失败记录日志
    st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0007: failed to cancel vm %d of running!" % vm_id2)
    result = False
#撤消成功后，查询虚拟机的状态
else:
    vm_state = v.get_vm_state(vm_id2)  
    count = 0
    # 获取虚拟机状态，应该为0 
    while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id2)
            print "[DEBUG],after cancel, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #等待1分钟虚拟机的状态不为0，则用例执行失败记录日志
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0008: failed to query vm %d of configuration!" % vm_id2)
                result = False
#虚拟机状态为0（configuration），则用例执行成功记录日志
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0008: success to cancel vm %d of running!" % vm_id2)
#用例结束


st_log(2,'',"END")
