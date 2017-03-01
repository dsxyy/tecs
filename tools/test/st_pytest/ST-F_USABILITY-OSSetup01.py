#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�ST-F_VM-Operate00.py
# ����ժҪ���û�������ԡ�
# ��    �ߣ�xujiakui
# ������ڣ�2012/5/21
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *
from contrib import xmlrpclibex
from tecs.user import *

#tecsĿ����Ի���ַ���û�            
server_addr = "10.44.127.9"
root_user = "admin"
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
        root_user = v
__builtin__.LOG = server_addr.replace('.','_')

#��¼ӳ�����ģ��
encrypted_root_session, server = login_user(server_addr,root_user)           #//��ɫ���岿��Ϊ������ÿ��.py�ļ��л������������ģ�ֱ�Ӵӷ����п�����

##############################################################
##############################################################


#######ST-F_USABILITY-OSSetup02 �������޸ģ�ɾ���Ͳ�ѯ�ʻ�����##########
st_log(2,'',"START")

#��¼�û�����ģ��
u = user(server,encrypted_root_session)
ug  = usergroup(server,encrypted_root_session)

#������ʼ
result = True
#�����û���usergroup0��usergroup1, Ӧ�óɹ�
for i in range(2):
    mygroup = "usergroup"+str(i)
    if ug.allocate(mygroup,1) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: mygroup can not create!")
        result = False

#�������û���usergroup0Ϊ���ã�Ӧ�óɹ�
if ug.set("usergroup0",2,"disable usergroup0")  != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: set usergroup0 is failed !")
    result = False

#�����û���Ӧ�óɹ�
if  u.allocate("user0","user0","user0",1,"usergroup0") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: create uaer0 is failed!")
    result = False

#ʹ�� user0 �û���¼    Ӧ��ʧ��
encrypted_user0_session, server_user0= login_user(server_addr,"user0")
nu = user(server_user0,encrypted_user0_session)
if nu.query(0L,0L,"user0") != False:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 should login !")
    result = False
    
#����usergroup0Ϊ���ã�Ӧ�óɹ���
if  ug.set("usergroup0",1,"enable usergroup0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: usergroup0 shouldn't enable !")
    result = False

#��ʹ��user0�û���¼��Ӧ�óɹ�
if nu.query(0L,0L,"user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 shouldn't login !")
    result = False
    
#ɾ��usergroup0 �û���    Ӧ��ʧ��
if ug.delete("usergroup0") != False:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: success to delete usergroup0!")
    result = False
    
#ɾ��user0�û�����ɾ��usergroup0 �û���    Ӧ�óɹ�
if  u.delete("user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete user0!")
    result = False  
if ug.delete("usergroup0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete usergroup0!")
    result = False

#�����û���usergroup0, �ٴ����û�user0�������������벻һ�²���
ug.allocate("usergroup0",1)
if u.allocate("user0","user0","user",1,"usergroup0")  == True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: success to create uaer0 when pwd is error!")
    result = False

#�����û�user0��������������һ�²��ԣ�Ӧ�óɹ�
if u.allocate("user0","user0","user0",1,"usergroup0")  != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to create uaer0 when pwd is right!")
    result = False

#�ƹ���Ա�����û�user0-user2����3���û���ɫ�ֱ�Ϊ�ƹ���Ա���˻�����Ա���⻧�������Ѵ������û������ٱ�������
for i in range(3):
    myuser = "user"+str(i)
    print myuser
    if u.allocate(myuser,myuser,myuser,i+1,"usergroup0")  != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: myuser can not create!")
        result = False
    if u.allocate(myuser,myuser,myuser,i+1,"usergroup0")  != False:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: myuser can create again!")
        result = False

# �ƹ���Ա���޸��⻧��Ϣ
for i in range(3):
    myuser = "user"+str(i)
    mypwd = "user"+str(i+1)
    myemail = "email"+str(i)
    myaddr = "addr"+str(i)
    print myuser
    if u.set(myuser,mypwd,mypwd,2-i,"usergroup1",1,"13601455012",myemail,myaddr) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: myuser cann't modify by admin!")
        result = False

#�ƹ���Ա��ѯ�û��ĵ绰��Ϣ
for i in range(3):
    myuser = "user"+str(i)
    print myuser
    if  u.query_phone(0L,50L,myuser) != "13601455012":
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user of phone!")
        result = False

#�ƹ���Ա��ѯ�û��������ַ
for i in range(3):
    myuser = "user"+str(i)
    myemail = "email"+str(i)
    print myuser
    if  u.query_email(0L,50L,myuser) != myemail:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user of email!")
        result = False

#�ƹ���Ա��ѯ�û��ĵ�ַ��Ϣ
for i in range(3):
    myuser = "user"+str(i)
    myaddr = "addr"+str(i)
    print myuser
    if  u.query_location(0L,50L,myuser) != myaddr:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user of location!")
        result = False

#�⻧ user0 �û���¼    Ӧ�óɹ�
encrypted_user_pwd_session, server_user_pwd= login_user_with_pwd(server_addr,"user0","user1")
nu_p = user(server_user_pwd,encrypted_user_pwd_session)
if nu_p.query(0L,0L,"user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 should login !")
    result = False

#�⻧user0�û���¼�󣬲�ѯ�����û���Ϣ
if  nu_p.query_phone(0L,0L,"user0") != "13601455012" or \
nu_p.query_email(0L,50L,"user0") != "email0" or \
nu_p.query_location(0L,50L,"user0") != "addr0":
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user information!")
    result = False

#�⻧user0�û��޸�������Ϣ
if nu_p.set("user0","user","user",3,"usergroup0",1,"13601455012","email","addr") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: my information cann't modify by user0!")
    result = False

#ʹ���޸ĺ���û������¼ϵͳ������ѯ�û���Ϣ�Ƿ���ȷ�޸ġ�
encrypted_user0_session, server_user0= login_user_with_pwd(server_addr,"user0","user")
nu0 = user(server_user0,encrypted_user0_session)
if nu0.query(0L,0L,"user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 shouldn't login !")
    result = False
if  nu0.query_phone(0L,0L,"user0") != "13601455012" or \
nu0.query_email(0L,50L,"user0") != "email" or \
nu0.query_location(0L,50L,"user0") != "addr":
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user information!")
    result = False

#�޸��Լ�Ϊ�ƹ���Ա���޸��Լ�Ϊ����״̬��Ӧ��ʧ��
if nu0.set("user0","user","user",1,"usergroup0",1,"13601455012","mail","addr") == True \
or nu0.set("user0","user","user",3,"usergroup0",2,"13601455012","mail","addr") == True :
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: success to modify user information ?")
    result = False

#ɾ���û�
for i in range(3):
    myuser = "user"+str(i)
    if u.delete(myuser) !=True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete user!")
        result = False
    
#ɾ���û���
for i in range(2):
    mygroup = "usergroup"+str(i)
    if ug.delete(mygroup) !=True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete usergroup!")
        result = False

#��ѯ�û����Ƿ����
if  ug.query(0L,200L,"") != True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to query usergroup!")
        result = False

#��ѯ�û��Ƿ����
if  u.query(0L,200L,"") != True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to query usergroup!")
        result = False


if result == True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: create/delete/query/modify is success!")
#����������

st_log(2,'',"END")
