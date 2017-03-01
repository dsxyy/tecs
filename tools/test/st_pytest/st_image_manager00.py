#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# �ļ����ƣ�st_image_manager00.py
# ����ժҪ��ӳ�����ģ��Ĺ����������̡�
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
img_name = "ttylinux0.img"
img_name1 = "windowsxp_0406.img"
img_name2 = "centos6.2_hvm_0405.img"

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
i = image(server,encrypted_account_session)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_IMAGE-01001 �ƹ���Աadmin�ϴ�ttylinuxӳ��ע��
#������ʼ
result = True
#image_id--ttylinuxӳ���ID
encrypted_account_session, server,image_id = upload_img(server_addr,account_user,img_name)
#��ѯӳ���Ƿ��ϴ��ɹ�
if i.get_id_by_filename(img_name) == None:
    st_log(-1, lines(), "STC-F-TECS_IMAGE-01001: failed to register ttylinux image!")
    result = False
#����ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_IMAGE-01001: success to register ttylinux image!")
#��������


#STC-F-TECS_IMAGE-01002 �ƹ���Աadmin�ϴ�windowsxpӳ��ע��
#������ʼ
result = True
#image_id1--windowsxpӳ���ID
encrypted_account_session, server,image_id1 = upload_img(server_addr,account_user,img_name1)
#��ѯӳ���Ƿ��ϴ��ɹ�
if i.get_id_by_filename(img_name1) == None:
    st_log(-1, lines(), "STC-F-TECS_IMAGE-01001: failed to register windowsxp image!")
    result = False
#����ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_IMAGE-01002: success to register windowsxp image!")
#��������


#STC-F-TECS_IMAGE-01003 �ƹ���Աadmin�ϴ�centosӳ��ע��
#������ʼ
result = True
#image_id1--windowsxpӳ���ID
encrypted_account_session, server,image_id1 = upload_img(server_addr,account_user,img_name2)
#��ѯӳ���Ƿ��ϴ��ɹ�
if i.get_id_by_filename(img_name2) == None:
    st_log(-1, lines(), "STC-F-TECS_IMAGE-01001: failed to register centos image!")
    result = False
#����ִ�гɹ�������st_log(1)��¼ִ�н��
if result == True:
    st_log(1,lines(),"STC-F-TECS_IMAGE-01002: success to register centos image!")
#��������

st_log(2,'',"END")
