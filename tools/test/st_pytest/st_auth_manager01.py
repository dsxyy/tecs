#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�st_auth_manager01.py
# ����ժҪ���û������������������
# ��    �ߣ�XXX
# ������ڣ�2012/4/17
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecsĿ����Ի���ַ���û�            
server_addr = "localhost"
account_user = "admin"

# ��������
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:v", ["help", "addr=","user="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #��-i��--image����ָ����������õ�ӳ���ļ�
    if o in ("-u", "--user"):
        account_user = v
__builtin__.LOG = server_addr.replace('.','_')

##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_USER-01003 ���ƹ���Ա�ʻ���¼����
#������ʼ
result = True
users = open('/home/tecs/st_data/newadmin.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #�����ʻ���¼tecs
    if login_user(server_addr,name) is None:
        st_log(-1,lines(),"STC-F-TECS_USER-01003: failed to login with new administrators!")
        result = False
        break
#��¼��ȷ��������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01003: success to login with new administrators!")
#��������


#STC-F-TECS_USER-01004 ����ͨ�ʻ���¼����
#������ʼ
result = True
users = open('/home/tecs/st_data/newtenant.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #�����ʻ���¼tecs
    if login_user(server_addr,name) is None:
        st_log(-1,lines(),"STC-F-TECS_USER-01004: failed to login with new guest!")
        result = False
        break
#��¼��ȷ��������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01004: success to login with new guests!")
#��������


#STC-F-TECS_USER-01005 �ƹ���Ա�޸������ʻ�������Ϣ
#������ʼ
result= True
admin_user = "tecs"
encrypted_account_session, server = login_user(server_addr,admin_user)
u = user(server,encrypted_account_session)
#�޸�tecs�ʻ���phone��email��location����Ϣ
#set(username,password,confirm_pass,role,groupname,enable,phone,email,location)
if u.set(admin_user,"","",1,"",1,"025-888888","tecs@zte.com.cn","the earth!") != True:
    st_log(-1,lines(),"STC-F-TECS_USER-01005: failed to modify user info!")
    result = False
#�޸��ʻ���Ϣ�ɹ���������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01005: success to modify user info!")
#��������    


#STC-F-TECS_USER-01006 ��ͨ�⻧�޸������ʻ�������Ϣ
#������ʼ
result= True
tenant_user = "st_guest"
encrypted_account_session, server = login_user(server_addr,tenant_user)
u = user(server,encrypted_account_session)
#�޸�st_guest�ʻ���phone��email��location����Ϣ
#set(username,password,confirm_pass,role,groupname,enable,phone,email,location)
if u.set(tenant_user,"","",3,"",1,"025-666666","guest@zte.com.cn","the earth!") != True:
    st_log(-1,lines(),"STC-F-TECS_USER-01006: failed to modify user info!")
    result = False
#�޸��ʻ���Ϣ�ɹ���������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01006: success to modify user info!")
#��������    


#STC-F-TECS_USER-01007 ɾ���˻�����
#������ʼ
result= True
tenant_user = ("st_v3_guest", "administrator")
encrypted_account_session, server = login_user(server_addr,account_user)
u = user(server,encrypted_account_session)
for name in tenant_user:
    if u.delete(name) != True:
        st_log(-1,lines(),"STC-F-TECS_USER-01007: failed to delete account!")
        result = False
        break
#�޸��ʻ���Ϣ�ɹ���������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01007: success to delete account!")
#��������


st_log(2,'',"END")
