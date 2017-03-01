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
import sys,os,getopt
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

def usage():
    ''' ��ӡ������Ϣ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -p,--port, the xmlrpc server port"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)
    
def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)

def login(server,username,passwd):
    '''��¼tecs����ȡ���ܺ��session'''
    session = '%s:%s' % (username,hashlib.sha1(passwd).hexdigest())
    result = server.tecs.user.query(session,0L,100L,username)
    #����ʧ��������������Ϊsessionû�м��ܣ�������20��ר�����ڷ��ؼ���session�ַ�����
    if result[0] != 0:
        if result[0] == 20:
            encryped_session = result[2]
            print "login session = %s" % encryped_session
            return encryped_session
        else:
            print "failed to query user information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
    else:
        return None

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
