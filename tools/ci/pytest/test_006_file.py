#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_002.py
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
#*******************************************************************************/
import sys,os,getopt,time
import hashlib
from contrib import xmlrpclibex
from ftplib import FTP

from tecs.user import *
from tecs.usergroup import *
from tecs.clustermanager import *
from tecs.host import *
from tecs.tcu import *
from tecs.image import *
from tecs.vmcfg import *
from tecs.project import *
from tecs.file import *

#���ڲ��Ե�tc xmlrpc��������ַ
server_addr = "localhost"
#���ڲ��Ե�tc xmlrpc�������˿ں�
server_port = "8080"
# tc/cc/hc��������Ҫ��start.sh�еĶ��屣��һ�£�
#���ڲ��Ե�tc application����������
test_cloud = "tecscloud"
#���ڲ��Ե�cc application������Ⱥ����
test_cluster = "tecscluster"
#���ڲ��Ե�hc application�������������
test_host = "citesthost"
#����Ա�˺ź�����
test_root_user = "admin"
test_root_passwd = "admin"
#��ͨ�û�������
test_tenant_usergroup = "cigroup"
#�⻧�˺ź�����
test_tenant_user = "citest"
test_tenant_passwd = "citest"
#�����ϴ��������������ӳ���ļ�·��
test_image = "ttylinux.img"
#�����Ե����������
test_vm_name = "ttylinux"
#�����ԵĹ�������
test_project_name = "test_prj_1"
#test����������
test_admin_usergroup = "testadmingroup"
#test�������ʺź�����
test_admin_user      = "testadmin"
test_admin_passwd    = "testadmin"
server_bath_path ="/mnt/tecs_tc/"

def usage():
    ''' ��ӡ������Ϣ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -p,--port, the xmlrpc server port"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)
    
def line():
    try:
        raise Exception
    except:
        return sys.exc_info()[2].tb_frame.f_back.f_lineno 

def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)

def ci_exit_ex(value=0,line = line(),error="null",):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, line = %d, error string = %s" % (value,line,error)
        sys.exit(value)

# �������������в���
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:p:i:v", ["help", "addr=","port=","image="])
except getopt.GetoptError:
    # print help information and exit:
    usage()
    ci_exit(-2,"GetoptError")
    
for o, v in opts:
    #��-h��--help��ӡ����
    if o in ("-h", "--help"):
        usage()
    #��-a��--addr����ָ��tc��xmlrpc��������ַ��Ĭ��Ϊlocalhost
    if o in ("-a", "--addr"):
        server_addr = v
    #��-p��--port����ָ��tc��xmlrpc�������˿ںţ�Ĭ��Ϊ8080
    if o in ("-p", "--port"):
        server_port = v
    #��-i��--image����ָ����������õ�ӳ���ļ�
    if o in ("-i", "--image"):
        test_image = v
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)
        
# �Զ������ɲ��Կ�ʼ!!!
print "===================TECS CI TEST START==================="        
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�    
server.system.listMethods()

# ------------------����׼��------------------
# ��¼tecs����ȡ����Ա�˻����ܺ��session
encrypted_root_session = login(server,test_root_user,test_root_passwd)
if encrypted_root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")

# ����һ�������鼰����Ա�ʺ�
ug  = usergroup(server,encrypted_root_session)
if ug.allocate(test_admin_usergroup,1) is not True:
    ci_exit(-1)
u   = user(server,encrypted_root_session)
if  u.allocate(test_admin_user,test_admin_passwd,test_admin_passwd,1,test_admin_usergroup) != True:
    ci_exit(-1)

# ����һ���û��鼰�⻧�ʺ�
ug3  = usergroup(server,encrypted_root_session)
if ug3.allocate(test_tenant_usergroup,3) is not True:
    ci_exit_ex(-1,line())
u3   = user(server,encrypted_root_session)
if u3.allocate(test_tenant_user,test_tenant_passwd,test_tenant_passwd,3,test_tenant_usergroup) != True:
    ci_exit_ex(-1,line())

# ��¼�⻧  ��ȡ�⻧session �Ա�����ʹ��
user_session = login(server,test_tenant_user,test_tenant_passwd)
if user_session is None:
    ci_exit_ex(-1,"failed to get user session!")


# ------------------����ԱȨ�޲���------------------
# �Թ���Ա����ϴ�һ��ӳ���ļ�  Ӧ�óɹ� 
if upload_image_file(server_addr,test_root_user,test_root_passwd,test_image) !=True:
    ci_exit_ex(-1,line())
# ���⻧����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�

# ��ȡ���ļ���file1id  fileģ��.get_file_id    ���� os.path.basename(test_image)
time.sleep(1)
f = tecs_file(server,encrypted_root_session)
fileid = f.get_file_id(os.path.basename(test_image))
if fileid is None:
    ci_exit_ex(-1,line())
# ���ϴ�һ�� ͬ����ӳ���ļ� ��Ӧ�óɹ�
if upload_image_file(server_addr,test_root_user,test_root_passwd,test_image) !=True:
    ci_exit_ex(-1,line())
# ���»�ȡ�ļ�id ��Ӧ�óɹ�
time.sleep(1)  
fileid = f.get_file_id(os.path.basename(test_image))
if fileid is None:
    ci_exit_ex(-1,line())

# ��ѯ��ǰ�ļ�������  Ӧ��Ϊtest_root_user
fileowner=f.get_file_owner(fileid)
if fileowner is None:
    ci_exit_ex(-1,line())
elif fileowner != test_root_user:
    ci_exit_ex(-1,line())
    
# ��ѯ��ǰ�ļ�������״̬ Ӧ��Ϊδ����״̬
filelock=f.get_file_lock(fileid)
if filelock is None:
    ci_exit_ex(-1,line())
elif filelock != 0:
    ci_exit_ex(-1,line())

# ��ѯ��ǰ�ļ�������״̬  Ӧ��Ϊ����״̬
filelock=f.get_file_lock(fileid)
if filelock is None:
    ci_exit_ex(-1,line())
elif filelock != 1:
    ci_exit_ex(-1,line())
# ɾ����ǰ�ļ�    Ӧ��ʧ��

# ��ȡ��ǰ�ļ��Ĵ�С Ӧ�ò�Ϊ0
filesize=f.get_file_size(fileid)
if filesize is None:
    ci_exit_ex(-1,line())
elif filesize == 0:
    ci_exit_ex(-1,line())
# ��ȡimage���ļ���С Ӧ�ú� �ļ���Сһ��


# ------------------��ͨ�û�Ȩ�޲���------------------
print "**************test user**************"
# ��ѯ����Ա�ϴ��� fileid    Ӧ��ʧ��
f3 = tecs_file(server,user_session)
fileid3 = f3.get_file_id(os.path.basename(test_image))
if fileid3 is not None:
    ci_exit_ex(-1,line())

# ��ͨ�⻧�ϴ�һ��ӳ���ļ���ʹ�õ��ļ����͹���Աһ����  Ӧ�óɹ�
upload_image_file(server_addr,test_tenant_user,test_tenant_passwd,test_image)

# ��ѯӵ���ļ��ļ�¼��       Ӧ��Ϊ1��
max_count3=f3.get_file_max_count(-4L)
if max_count3 is None:
    ci_exit_ex(-1,line())
elif max_count3 !=1:
    ci_exit_ex(-1,line())

# ע��Ϊimage   Ӧ�óɹ�
time.sleep(1)
fileid3 = f3.get_file_id(os.path.basename(test_image))
if fileid3 is None:
    ci_exit_ex(-1,line())
i3 = image(server,user_session)
if i3.register(fileid3, "ttylinux", "machine", "x86_64", bool(0), bool(1), "reg image for  test file mod") == False:
    ci_exit_ex(-1,line())
    
# ------------------����ԱȨ�޲���------------------
# ��ѯӵ�е�ӳ���ļ���   Ӧ��ֻ��һ��
max_count=f.get_file_max_count(-4L)
if max_count is None:
    ci_exit_ex(-1,line())
elif max_count !=1:
    ci_exit_ex(-1,line())

# ע�� ӳ���ļ�
if i.unregister(fileid) != True:
    ci_exit_ex(-1)

# ��ѯ��ǰ�ļ�������״̬  Ӧ��Ϊ������״̬
filelock=f.get_file_lock(fileid)
if filelock is None:
    ci_exit_ex(-1,line())
elif filelock != 0:
    ci_exit_ex(-1,line())

# ɾ������Ա�ϴ����ļ�    Ӧ�óɹ�
if delete_image_file(server_addr,test_root_user,test_root_passwd,server_bath_path+"1/"+os.path.basename(test_image),1) == False:
    ci_exit_ex(-1)
# ɾ���⻧�ϴ����ļ�      Ӧ��ʧ��
if delete_image_file(server_addr,test_root_user,test_root_passwd,server_bath_path+"3/"+os.path.basename(test_image),1) != False:
    ci_exit_ex(-1)
    
# ------------------��ͨ�û�Ȩ�޲���------------------

# ע���⻧����ӳ���ļ�  Ӧ�óɹ�
if i3.unregister(fileid3) == False:
    ci_exit_ex(-1,line())
    
# ɾ���⻧�ϴ����ļ�      Ӧ��ʧ��?
if delete_image_file(server_addr,test_tenant_user,test_tenant_passwd,server_bath_path+"3/"+os.path.basename(test_image),1) == False:
    ci_exit_ex(-1)

















