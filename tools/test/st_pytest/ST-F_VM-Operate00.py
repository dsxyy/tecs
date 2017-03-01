#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�ST-F_VM-Operate00.py
# ����ժҪ������������Ĺ����������̡�
# ��    �ߣ�pengwei
# ������ڣ�2012/5/21
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecsĿ����Ի���ַ���û�            
server_addr = "localhost"
account_user = "st_guest"
#1�������
vm_id0 = long(1)

# ��������
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #��-i��--image����ָ����������õ�ӳ���ļ�
    if o in ("-u", "--user"):
        account_user = v
__builtin__.LOG = server_addr.replace('.','_')

#��¼ӳ�����ģ��
encrypted_account_session, server = login_user(server_addr,account_user)
v = vmcfg(server,encrypted_account_session)

#ǰ��������1��������������У�
if v.get_vm_state(vm_id0) != 2:
    #���1�������״̬��Ϊruning�����߲����ڣ�ֱ�Ӵ���һ���������
    print "vm %d status is not running!" % vm_id0
    encrypted_account_session, server, vm_id0 = create_vm(server_addr,account_user, "test01","ttylinux0.img")
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_04-03-VM-VmOperate-0001  �ر������
#������ʼ
result = True
#��ѯ1�������״̬Ӧ��Ϊ2(running), (6Ϊδ֪)
count = 0
vm_state = v.get_vm_state(vm_id0)   
while (vm_state !=2 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    print "[DEBUG],now vm_state is %d" %vm_state
    count = count + 1
    if(count >= 2):
        #�ȴ�����ӻ�ȡ1�������״̬��Ϊrunning����ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0001: failed to query vm %d of running!" % vm_id0)
        result = False
        break
#�ر�running״̬��1�������
if (vm_state == 2):
    if v.action(vm_id0, "stop") != True:
        #�ر������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0001: failed to stop vm %d!" % vm_id0)
        result = False
    else:
        #�رղ������ٲ�ѯ1�������״̬Ӧ��Ϊ4��shutoff��
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 4 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after stop, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 8):
                #�ȴ�1�ְ��ӻ�ȡ1�������״̬��Ϊshutoff����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0001: failed to query vm %d of shutoff!" % vm_id0)
                result = False
                break
#1�������״̬Ϊshutdown��������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0001: success to stop vm %d!" % vm_id0)
#��������


#STC-F-TECS_04-03-VM-VmOperate-0002  ���������
#������ʼ
result = True
#��ѯ1�������״̬Ӧ��Ϊ4(shutoff)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 4 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 2):
        #�ȴ�����ӻ�ȡ1�������״̬��Ϊshutoff����ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0002: failed to query vm %d of shutoff!" % vm_id0)
        result = False
        break
#����shutoff״̬��1�������
if (vm_state == 4):
    if v.action(vm_id0, "start") != True:
        #���������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0002: failed to start vm %d!" % vm_id0)
        result = False
    else:
        #���������󣬲�ѯ1�������״̬Ӧ��Ϊ2��running��
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after start, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1���ӻ�ȡ1�������״̬��δ��Ϊrunning����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0002: failed to query vm %d of running!" % vm_id0)
                result = False
                break
#1�������״̬Ϊrunning��������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0002: success to start vm %d!" % vm_id0)
#��������


#STC-F-TECS_04-03-VM-VmOperate-0003  ��ͣ�����
#������ʼ
result = True
#��ѯ1�������״̬Ӧ��Ϊ2(running)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 2 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 2):
        #�ȴ�����ӻ�ȡ1�������״̬��Ϊrunning����ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0003: failed to query vm %d of running!" % vm_id0)
        result = False
        break
#��ͣrunning״̬��1�������
if (vm_state == 2):
    if v.action(vm_id0, "pause") != True:
        #��ͣ�����ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0003: failed to pause vm %d!" % vm_id0)
        result = False
    else:
        #��ͣ�����󣬲�ѯ1�������״̬Ӧ��Ϊ3��paused��
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 3 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after pause, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1���ӻ�ȡ1�������״̬��δ��Ϊpaused����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0003: failed to query vm %d of paused!" % vm_id0)
                result = False
                break
#1�������״̬Ϊpaused��������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0003: success to pause vm %d!" % vm_id0)
#��������


#STC-F-TECS_04-03-VM-VmOperate-0004  �ָ������
#������ʼ
result = True
#��ѯ1�������״̬Ӧ��Ϊ3(paused)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 3 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 2):
        #�ȴ�����ӻ�ȡ1�������״̬��Ϊpaused����ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0004: failed to query vm %d of paused!" % vm_id0)
        result = False
        break
#�ָ�paused״̬��1�������
if (vm_state == 3):
    if v.action(vm_id0, "resume") != True:
        #�ָ������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0004: failed to resume vm %d!" % vm_id0)
        result = False
    else:
        #�ָ������󣬲�ѯ1�������״̬Ӧ��Ϊ2��running��
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after resume, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1���ӻ�ȡ1�������״̬��δ��Ϊrunning����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0004: failed to query vm %d of running!" % vm_id0)
                result = False
                break
#1�������״̬Ϊpaused��������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0004: success to resume vm %d!" % vm_id0)
#��������


#STC-F-TECS_04-03-VM-VmOperate-0005  ���������
#������ʼ
result = True
#��ѯ1�������״̬Ӧ��Ϊ2(running)
vm_state = v.get_vm_state(vm_id0)  
count = 0
while (vm_state != 2 and vm_state != None):
    time.sleep(10)
    vm_state = v.get_vm_state(vm_id0)
    count = count + 1
    if (count >= 5):
        #�ȴ�1���ӻ�ȡ1�������״̬��Ϊpaused����ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0005: failed to query vm %d of running!" % vm_id0)
        result = False
        break
#�ָ�paused״̬��1�������
if (vm_state == 2):
    if v.action(vm_id0, "reboot") != True:
        #���������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0005: failed to reboot vm %d!" % vm_id0)
        result = False
    else:
        #���������󣬲�ѯ1�������״̬Ӧ��Ϊ2��running��
        #���ӳ�30s�ٲ�ѯ
        time.sleep(30)
        vm_state = v.get_vm_state(vm_id0)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after reboot, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 8):
                #�ȴ�1�ְ��ȡ1�������״̬��δ��Ϊrunning����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0005: failed to query vm %d of running!" % vm_id0)
                result = False
                break
#1�������״̬Ϊpaused��������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0005: success to reboot vm %d!" % vm_id0)
#��������


st_log(2,'',"END")
