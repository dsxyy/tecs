#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�st_vm_manager00.py
# ����ժҪ�����������ģ��Ĺ����������̡�
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
#img_name = "windowsxp.img"
img_name = "ttylinux0.img"
vmname = "ttylinux"
num = 2

# ��������
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user=","image="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #��-i��--image����ָ����������õ�ӳ���ļ�
    if o in ("-u", "--user"):
        account_user = v
    if o in ("-i","--image"):
        img_name = v
__builtin__.LOG = server_addr.replace('.','_')

encrypted_account_session, server,vm_id = cfg_vm(server_addr,account_user,vmname,img_name,num)

#��¼��������ù���ģ��
v = vmcfg(server,encrypted_account_session)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_VMO-1001 
#������ʼ
result = True


st_log(2,'',"END")


