#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�ST-F_VM-Operate01.py
# ����ժҪ������������������������̡�
# ��    �ߣ�pengwei
# ������ڣ�2012/5/23
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecsĿ����Ի���ַ���û�            
server_addr = "localhost"
account_user = "st_guest"

vmname="tty1"
img="ttylinux0.img"

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

#ǰ������������3���������״̬�ֱ�Ϊrunning��shutoff��pause
for i in range(3):
    print i
    encrypted_account_session, server, vm_id0=create_vm(server_addr,account_user,vmname,img)
    vm_state = v.get_vm_state(vm_id0)
    #print "[DEBUG], vm %d state is %d" %(vm_id0, vm_state)
#��󴴽��������״̬��Ϊpause    
if v.action(vm_id0, "pause") != True:
    st_exit(-1, lines(), "vm %s status is not pause!"%vm_id0)
count=0
while(v.get_vm_state(vm_id0) !=3 and v.get_vm_state(vm_id0) !=None):
    time.sleep(10)
    count = count + 1
    if(count >= 5):
        #�ȴ�1���ӣ���������״̬��Ϊpause�����˳���
        st_exit(-1, lines(), "vm %s status is not pause!"%vm_id0)
print "vm %s status is pause!"%vm_id0
#�����ڶ��������״̬��Ϊshutoff
vm_id1 = vm_id0 - 1
if v.action(vm_id1, "stop") != True:
    st_exit(-1, lines(), "vm %s status is not shutoff!"%vm_id1)
count=0
while(v.get_vm_state(vm_id1) !=4 and v.get_vm_state(vm_id1) !=None):
    time.sleep(10)
    count = count + 1
    if(count >= 8):
        #�ȴ�1�ְ��ӣ���������״̬��Ϊshutoff�����˳���
        st_exit(-1, lines(), "vm %s status is not shutoff!"%vm_id1)
print "vm %s status is shutoff!"%vm_id1
#��һ�������������״̬Ϊrunning
vm_id2 = vm_id0 - 2
if v.get_vm_state(vm_id2) != 2:
    st_exit(-1, lines(), "vm %s status is not running!"%vm_id2)
print "vm %s status is running!"%vm_id2
##############################################################
##############################################################
st_log(2,'',"START")


#STC-F-TECS_04-03-VM-VmOperate-0006  ��������ر�״̬�������
#������ʼ
result = True
#��������ر�״̬�������
if v.action(vm_id1, "cancel") != True:
    #��������ʧ�ܣ�������ִ��ʧ�ܼ�¼��־
    st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0006: failed to cancel vm %d of shutoff!" % vm_id1)
    result = False
#�����ɹ��󣬲�ѯ�������״̬
else:
    vm_state = v.get_vm_state(vm_id1)  
    count = 0
    # ��ȡ�����״̬��Ӧ��Ϊ0 
    while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id1)
            print "[DEBUG],after cancel, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1�����������״̬��Ϊ0��������ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0006: failed to query vm %d of configuration!" % vm_id1)
                result = False
#�����״̬Ϊ0��configuration����������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0006: success to cancel vm %d of shutoff!" % vm_id1)
#��������


#STC-F-TECS_04-03-VM-VmOperate-0007  ����������ͣ״̬�������
#������ʼ
result = True
#����������ͣ״̬�������
if v.action(vm_id0, "cancel") != True:
    #��������ʧ�ܣ�������ִ��ʧ�ܼ�¼��־
    st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0007: failed to cancel vm %d of pause!" % vm_id0)
    result = False
#�����ɹ��󣬲�ѯ�������״̬
else:
    vm_state = v.get_vm_state(vm_id0)  
    count = 0
    # ��ȡ�����״̬��Ӧ��Ϊ0 
    while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id0)
            print "[DEBUG],after cancel, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1�����������״̬��Ϊ0��������ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0007: failed to query vm %d of configuration!" % vm_id0)
                result = False
#�����״̬Ϊ0��configuration����������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0007: success to cancel vm %d of pause!" % vm_id0)
#��������


#STC-F-TECS_04-03-VM-VmOperate-0008  ������������״̬�������
#������ʼ
result = True
#������������״̬�������
if v.action(vm_id2, "cancel") != True:
    #��������ʧ�ܣ�������ִ��ʧ�ܼ�¼��־
    st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0007: failed to cancel vm %d of running!" % vm_id2)
    result = False
#�����ɹ��󣬲�ѯ�������״̬
else:
    vm_state = v.get_vm_state(vm_id2)  
    count = 0
    # ��ȡ�����״̬��Ӧ��Ϊ0 
    while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id2)
            print "[DEBUG],after cancel, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1�����������״̬��Ϊ0��������ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-F-TECS_04-03-VM-VmOperate-0008: failed to query vm %d of configuration!" % vm_id2)
                result = False
#�����״̬Ϊ0��configuration����������ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-F-TECS_04-03-VM-VmOperate-0008: success to cancel vm %d of running!" % vm_id2)
#��������


st_log(2,'',"END")
