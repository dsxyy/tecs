#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�st_auth_manager02.py
# ����ժҪ���û�������쳣����
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



st_log(2,'',"END")
