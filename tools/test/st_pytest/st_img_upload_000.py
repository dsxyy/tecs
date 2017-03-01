#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012��FreeWorld��All rights reserved.
# 
# �ļ����ƣ�st_img_upload_000.py
# �ļ���ʶ��
# ����ժҪ���ϴ�ӳ��ttylinux��centos��winxp��3��ӳ��tc��
# ����˵����-a address, -u user, -i image
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012/4/17
#*******************************************************************************/
import sys,os,getopt
sys.path.append('/home/ci_server/Tecs_stinit00/ci/pytest')
from contrib import xmlrpclibex
from ftplib import FTP
#from tecs.file import *
from tecs.user import *
from tecs.usergroup import *

#���ڲ��Ե�tc xmlrpc��������ַ
server_addr = "localhost"
#���ڲ��Ե�tc xmlrpc�������˿ں�
server_port = "8080"

#Ĭ���ƹ���Ա
cloud_user = "admin"
cloud_passwd = "admin"
#�û��˺ź�����
st_user = "admin"
st_user_passwd = "admin"
#ӳ���ļ������Ŀ¼
image_home = "/home/ci_server/images/"

#�����ϴ��������������ӳ���ļ�·��
test_image = "ttylinux.img"

def usage():
    ''' ��ӡ������Ϣ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -u,--user, the tecs user name"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)
    
def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)

def upload_image_file(server,user_name,password,image):
    ftp = FTP()
    try:
        ftp.connect(server,21)
        ftp.login(user_name,password)
    except:
        ci_exit(-1,"ftp login failed!")

    print ftp.getwelcome()
    try:
        cmd='%s %s' % ("stor",os.path.basename(image))
        ftp.storbinary(cmd,open(image,'rb'),1024)
    except:
       ci_exit(-1,"ftp upload failed!")

# �������������в���
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user=","image="])
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
    if o in ("-u", "--user"):
        st_user = v
        st_user_passwd = v
    #��-i��--image����ָ����������õ�ӳ���ļ�
    if o in ("-i", "--image"):
        test_image = image_home + v
        print test_image
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)

        
# �Զ���ϵͳ���Կ�ʼ!!!
print "===================TECS CI TEST START==================="   
         
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',cloud_user,':',cloud_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s again..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")
        
# �г���ǰ֧�ֵ�����xmlrpc�ӿ�    
server.system.listMethods()
    
# ��¼tecs����ȡ����Ա�˻����ܺ��session
encrypted_root_session = login(server,cloud_user,cloud_passwd)
if encrypted_root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")   
    
# ��ѯ�û��ʺ��Ƿ��Ѿ�ע��
u = user(server,encrypted_root_session)
    
    # ����һ������Ϊ1���û��飬����������ͨ�⻧���˺� 
    print "=============================================="
    ug = usergroup(server,encrypted_root_session)
    if ug.allocate(guest,1) != True:
        ci_exit(-1)
        
    # ����һ����ͨ�˺����ں���ģ�������⻧�Ĳ��� 
    u = user(server,encrypted_root_session)
    if u.allocate(st_user,st_user_passwd,st_user_passwd,3,guest) != True:
        ci_exit(-1)
    
upload_image_file(server_addr,st_user,st_user_passwd,test_image)

        
    
    
  