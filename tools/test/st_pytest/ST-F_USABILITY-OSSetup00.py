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

st_log(2,'',"START")

#ST-F_USABILITY-OSSetup01 �������޸ģ�ɾ���Ͳ�ѯ�ʻ������
#������ʼ
result = True

#��¼�û�����ģ��
u = user(server,encrypted_root_session)
ug  = usergroup(server,encrypted_root_session)

#����һ���û���ʹ�ò���Ϊ st_group1,1 ,Ӧ�óɹ�
if ug.allocate("st_group1",1) != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: failed to create st_group1!")
    result = False
    
#����һ���û���ʹ�ò���Ϊ st_group1,2  Ӧ��ʧ��
if ug.allocate("st_group1",2) == True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: success to create st_group1!")
    result = False

#�鿴st_group1 �Ƿ�����   Ӧ��Ϊ����
if ug.get_use_flag("st_group1") != True:
   st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group1 can not enable!")
   result = False

#����st_group1 Ϊ����
if ug.set("st_group1",2,"disable st_group1") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00:  st_group1 can not disable!")
    result = False
    
#�鿴st_group1 �Ƿ�����   Ӧ��Ϊ����
if ug.get_use_flag("st_group1") != 2:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00:  st_group1 can not disable!")
    result = False
    
#ɾ��st_group1 �û���    Ӧ�óɹ�
if ug.delete("st_group1") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group1 can not delete!")
    result = False
    
#����һ���û���ʹ�ò���Ϊ st_group2,1  Ӧ�óɹ�
if ug.query(0L,200L,"st_group2") == True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2  is created!")
else:
    ug.allocate("st_group2",1) != True
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2  can not create!")
    result = False
    
#����st_group2 Ϊ����
if ug.set("st_group2",2,"disable st_group2") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2 can not disable!")
    result = False
    
#ɾ��st_group2 �û���    Ӧ�óɹ�    
if ug.delete("st_group2") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2 can not delete!")
    result = False
    
   
#����usergroup0-usergroup200���û��飬Ӧ�óɹ�
i=1
numble =200
for i in range(numble):
    mygroup = "usergroup"+str(i)
    print mygroup
    if ug.allocate(mygroup,1) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: mygroup can not create!")
        result = False

# ��ѯ200���û��飬Ӧ�óɹ�
if ug.query(0L,200L,"")  != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: XXXXXXX !")
    result = False
    
#ɾ��usergroup0-usergroup200���û���
 
for i in range(numble):
    mygroup = "usergroup"+str(i)
    print mygroup
    if ug.delete(mygroup) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: mygroup can not delete!")
        result = False
    
if result == True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup00: success to create usergroup!")
#��������


