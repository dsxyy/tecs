#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�ST-F_VM-Operate02.py
# ����ժҪ��������������ȶ��Բ��ԡ�
# ��    �ߣ�pengwei
# ������ڣ�2012/6/1
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecsĿ����Ի���ַ���û�            
server_addr = "localhost"
account_user = "st_guest"

vmname1="test_s_01"
vmname2="test_s_02"
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

#ǰ������������һ���µ������
vm_id1 = v.get_id_by_name(-5L, vmname1, account_user)
if (vm_id1 == None) or (bool(vm_id1) is False):
    encrypted_account_session, server, vm_id1 = cfg_vm(server_addr,account_user, vmname1, img)
else:
    vm_id1=max(vm_id1)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-S-TECS_04-03-VM-VmOperate-0001  ��������ͳ���һ�������
#������ʼ
result = True
loop_times = 2
#�ظ����𡢳��������2��
for loop in range(loop_times):
    print "Loop  No. %d start ...... "%loop
    #�����������
    if v.action(vm_id1,"deploy") != True:
        #�������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:faile to deploy vm %s!"% vm_id1)
    else:
        #����ɹ��󣬲�ѯ�����״̬
        vm_state = v.get_vm_state(vm_id1)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id1)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 14):
                #�ȴ�2�ְ����������ѯ״̬��Ϊ2����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:failed to deploy vm %s!"% vm_id1)
                break
    print "the vm %s(%s) is runnig!"%(vmname1, vm_id1)
    #��ѯ״̬��ȷ�����������ɹ�����ȷ���У��ٳ��������
    if v.action(vm_id1,"cancel") != True:
        #��������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:faile to cancel vm %s!"% vm_id1)
    else:
        #���������ɹ��󣬲�ѯ�����״̬
        vm_state = v.get_vm_state(vm_id1)  
        count = 0
        while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id1)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1�����������ѯ״̬��Ϊ0����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0001:failed to cancel vm %s!"% vm_id1)
                break
    print "the vm %s(%s) is configuration!"%(vmname1, vm_id1)
    #�����ɹ��󣬵ȴ�3���ӵ���Դ����
    print "wait 200s" 
    time.sleep(200)
    print "Loop  No. %d end ...... "%loop
#�ٴβ��������������������ѭ�������ɹ�����ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-S-TECS_04-03-VM-VmOperate-0001: success to cancel vm %d of running!" % vm_id1)
#��������


#STC-S-TECS_04-03-VM-VmOperate-0002 ��������ͳ��������
#������ʼ
result = True
loop_times = 2
#�ظ����𡢳��������2��
for loop in range(loop_times):
    print "Loop  No. %d start ...... "%loop
    #ÿ�ζ�����һ���µ������test_s_01����Ϊͬһ��HOST���ܳ���/����ͬһ���������
    encrypted_account_session, server, vm_id2 = cfg_vm(server_addr,account_user, vmname2, img)
    #�����������
    if v.action(vm_id2,"deploy") != True:
        #�������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:faile to deploy vm %s!"% vm_id2)
    else:
        #����ɹ��󣬲�ѯ�����״̬
        vm_state = v.get_vm_state(vm_id2)  
        count = 0
        while (vm_state != 2 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id2)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 14):
                #�ȴ�2�ְ����������ѯ״̬��Ϊ2����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:failed to deploy vm %s!"% vm_id2)
                break
    print "the vm %s(%s) is runnig!"%(vmname2, vm_id2)
    #��ѯ״̬��ȷ�����������ɹ�����ȷ���У��ٳ��������
    if v.action(vm_id2,"cancel") != True:
        #��������ʧ�ܣ���ִ��ʧ�ܼ�¼��־
        st_log(-1,lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:faile to cancel vm %s!"% vm_id2)
    else:
        #���������ɹ��󣬲�ѯ�����״̬
        vm_state = v.get_vm_state(vm_id2)  
        count = 0
        while (vm_state != 0 and vm_state != None):
            time.sleep(10)
            vm_state = v.get_vm_state(vm_id2)
            #print "[DEBUG],after deploy, now vm_state is %d" %vm_state
            count = count + 1
            if (count >= 5):
                #�ȴ�1�����������ѯ״̬��Ϊ0����ִ��ʧ�ܼ�¼��־
                st_log(-1, lines(), "STC-S-TECS_04-03-VM-VmOperate-0002:failed to cancel vm %s!"% vm_id2)
                break
    print "the vm %s(%s) is configuration!"%(vmname2, vm_id2)
    #�����ɹ��󣬵ȴ�3���ӵ���Դ����
    print "wait 200" 
    time.sleep(200)
    print "Loop  No. %d end ...... "%loop
#�ٴβ��������������������ѭ�������ɹ�����ִ�гɹ���¼��־
if result == True:
    st_log(1,lines(),"STC-S-TECS_04-03-VM-VmOperate-0002: success to cancel vm %d of running!" % vm_id2)
#��������


st_log(2,'',"END")
