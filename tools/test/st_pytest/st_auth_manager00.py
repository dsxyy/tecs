#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�st_auth_manager00.py
# ����ժҪ���û�����Ĺ�����������
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

#�û���¼tecs            
encrypted_account_session, server = login_user(server_addr,account_user)

#��¼�û�����ģ��
u = user(server,encrypted_account_session)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_USER-01001 �������ƹ���Ա�ʻ�
#������ʼ
result = True
users = open('/home/tecs/st_data/newadmin.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #�������ƹ���Ա�ʻ�
    if u.allocate(name,name,name,1,"Default") != True:
        #�����ʻ�ʧ�ܣ�������ִ��ʧ�ܣ�����st_log(-1)��¼ִ�н��
        st_log(-1,lines(),"STC-F-TECS_USER-01001: failed to create new administrator!")
        result = False
        break
    else:
        if u.query(0L,100L,name) != True:
            #��ѯ���ʻ�ʧ�ܣ�������ִ��ʧ�ܣ�����st_log(-1)��¼ִ�н��
            st_log(-1,lines(),"STC-F-TECS_USER-01001: failed to query new administrator!")
            result = False
            break
#��ȷ�����ʺźͿ��Բ�ѯ����������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01001: success to create new administrators!")
#����������


#STC-F-TECS_USER-01002 �������⻧�ʻ�
#������ʼ
result = True
users = open('/home/tecs/st_data/newtenant.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #��������ͨ�û��ʻ�
    if u.allocate(name,name,name,3,"st_group") != True:
        #�����ʻ�ʧ�ܣ�������ִ��ʧ�ܣ�����st_log(-1)��¼ִ�н��
        st_log(-1,lines(),"STC-F-TECS_USER-01002: failed to create new guest!")
        result = False
        break
    else:
        if u.query(0L,100L,name) != True:
            #��ѯ���ʻ�ʧ�ܣ�������ִ��ʧ�ܣ�����st_log(-1)��¼ִ�н��
            st_log(-1,lines(),"STC-F-TECS_USER-01002: failed to query new guest!")
            result = False
            break
#��ȷ�����ʺźͿ��Բ�ѯ����������ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01001: success to create new guests!")
#����������


st_log(2,'',"END")
