#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_005_cluster_config.py
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
# �޸ļ�¼2��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#*******************************************************************************/
import sys,os,getopt,time,inspect
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
from tecs.clustercfg import *
from tecs.tecssystem import *

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
def line():
    try:
            raise Exception
    except:
            return sys.exc_info()[2].tb_frame.f_back.f_lineno
def file():
    return inspect.currentframe().f_code.co_filename

def usage():
    ''' ��ӡ������Ϣ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -p,--port, the xmlrpc server port"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)
    
def ci_exit(value=0,line = line(),error="null",):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, line = %d, error string = %s" % (value,line,error)
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
    ci_exit(-2,line(),"GetoptError")
    
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
            ci_exit(-2,line(),"image file %s not exist!" % test_image)
        
# �Զ������ɲ��Կ�ʼ!!!
print "===================TECS CI TEST START==================="        
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,line(),"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�    
server.system.listMethods()

# ��¼����Ա�ʺ�
encrypted_root_session = login(server,test_root_user,test_root_passwd)
if encrypted_root_session is None:
    ci_exit(-1,line(),"failed to get encrypted root session!")

#ϵͳ����״̬��ѯ EC:611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#�˴���ʱ20s������hc��MU_VM_HANDLER��MU_HOST_HEARTBEAT������
time.sleep(100)
#�����ǹ���Ա�˻�
sys_runtime_query = tecssystem(server,encrypted_root_session)
#��ѯTC״̬
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,DEFAULT_TC)) != RUNTIME_STATE_NORMAL:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,DEFAULT_TC)
    ci_exit_ex(-1,line(),"TC status is abnormal!")
#��ѯCC״̬
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cluster)) != RUNTIME_STATE_NORMAL:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cluster)
    ci_exit_ex(-1,line(),"CC status is abnormal!")
#��ѯHC״̬
test_cc_hc = test_cluster + "," + test_host
hc_state = int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cc_hc))
if  hc_state != RUNTIME_STATE_NORMAL:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cc_hc)
    if hc_state > RUNTIME_STATE_WARNNING:
        ci_exit_ex(-1,line(),"HC status is abnormal!")

print "test cluster config start......"
cm = clustermanager(server,encrypted_root_session)
cc = clustercfg(server,encrypted_root_session)
h = host(server,encrypted_root_session)
# �ѵ�ǰϵͳ�еļ�Ⱥע�ᵽTC  ��Ⱥ����Ϊ test_cluster
#if cm.register(test_cluster,"reg cluster") != True:
#    ci_exit(-1,line())
# ��ʱ20S
time.sleep(20)

#  ��ѯ��Ⱥ�������Զ����������״̬ ���� $test_cluster, allow_host_auto_register  ���ؽ��Ӧ��Ϊno
#ret = cc.query_config_value(test_cluster,"allow_host_auto_register")
#if ret == None:
#    ci_exit(-1,line())
#elif ret != "no":
#    print "host auto register flag is err:%s" % ret
#    ci_exit(-1,line())
#  ��ѯ��Ⱥ�������Զ����������״̬ ���� $test_cluster, registered_tecs_name  ���ؽ��Ӧ��Ϊtecscloud
#if cc.query_config_value(test_cluster,"registered_tecs_name") != test_cloud:
#    ci_exit(-1,line(),"tecs name shoud be tecscloud")
#  ���������Զ����뼯Ⱥ ����Ϊ $test_cluster, allow_host_auto_register, yes  Ӧ��Ϊ�ɹ�
#if cc.set(test_cluster,"allow_host_auto_register","yes") != True:
#    ci_exit(-1,line())
#  ��ѯ��Ⱥ�������Զ����������״̬ ���� $test_cluster, allow_host_auto_register  ���ؽ��Ӧ��Ϊyes
#if cc.query_config_value(test_cluster,"allow_host_auto_register") != "yes":
#    ci_exit(-1,line(),"host auto register flag shouldn't be no")

#  ����һ���Ƿ���ѡ�� test  ����Ϊ $test_cluster, test, test  Ӧ��Ϊʧ��
#if cc.set(test_cluster,"test","test") != False:
#    ci_exit(-1,line(),"cluster cfg shouldn't support 'test' name register")
# ��ʱ 20S���ȴ�HOST���Զ�ע��
time.sleep(20)

# ����������ѯ�ӿڣ���citesthost �����Ƿ��Ѿ���ע��   citesthost �������Ӧ���Ѿ���ע��
if h.query(START_INDEX,START_INDEX,test_cluster,test_host) != True:
    ci_exit(-1,line())

# ɾ����Ⱥ����  ����Ϊ $test_cluster,test    Ӧ��ʧ��
#f cc.delete(test_cluster,"test") != False:
#   ci_exit(-1,line())

# ɾ����Ⱥ���� ����Ϊ $test_cluster, registered_tecs_name  Ӧ��ʧ��
if cc.delete(test_cluster,"registered_tecs_name") != False:
    ci_exit(-1,line())

# ��ȡtcu id �Ƽ�ʹ�� hostģ��.get_cpu_id(test_cluster,test_host) 
id = h.get_cpu_id(test_cluster,test_host)
if id == None:
    ci_exit(-1,line())

# ����tcu ����Ϊ $test_cluster,���ַ���,�����õ�id,4,test_description  Ӧ�óɹ�
sc = tcu(server,encrypted_root_session)
if sc.set(test_cluster,NO_CPU_INFO,id,TCU_NUM4,"test_description") != True:
     ci_exit(-1,line())

# ��ѯ�����Ƿ�ɹ��� ���� $test_cluster,���ַ���,�����õ�id  ���ص�tcu����Ӧ�����������õ�����
if sc.get_tcu_num_by_id(test_cluster,NO_CPU_INFO,id) != TCU_NUM4:
    ci_exit(-1,line())

# ��ѯ���õ�������Ϣ�Ƿ���ȷ�� ���� $test_cluster,���ַ���,�����õ�id  ���ص�������Ϣ������һ��
if sc.get_description_by_id(test_cluster,NO_CPU_INFO,id) != "test_description":
    ci_exit(-1,line())

# ����tcu ����Ϊ $test_cluster,���ַ���,�����õ�id,8,test_description1  Ӧ�óɹ�
if sc.set(test_cluster,NO_CPU_INFO,id,TCU_NUM8,"test_description1") != True:
     ci_exit(-1,line())

# ��ѯ�����Ƿ�ɹ��� ���� $test_cluster,���ַ���,�����õ�id  ���ص�tcu����Ӧ�����������õ�����
if sc.get_tcu_num_by_id(test_cluster,NO_CPU_INFO,id) != TCU_NUM8:
    ci_exit(-1,line())

# ��ѯ���õ�������Ϣ�Ƿ���ȷ�� ���� $test_cluster,���ַ���,�����õ�id  ���ص�������Ϣ������һ��
if sc.get_description_by_id(test_cluster,NO_CPU_INFO,id) != "test_description1":
    ci_exit(-1,line())

# ����tcu ����Ϊ $test_cluster,���ַ���,100,8,test_description1  Ӧ��ʧ��
if sc.set(test_cluster,NO_CPU_INFO,CPU_INFO_ID,TCU_NUM8,"test_description1") != False:
     ci_exit(-1,line())

# ɾ�� SVPU���� ����Ϊ $test_cluster,���ַ���,100  Ӧ��ʧ��
if sc.delete(test_cluster,NO_CPU_INFO,CPU_INFO_ID) != False:
     ci_exit(-1,line())

# ɾ�� SVPU���� ����Ϊ $test_cluster,���ַ���,�����õ�id  Ӧ�óɹ�
if sc.delete(test_cluster,NO_CPU_INFO,id) != True:
     ci_exit(-1,line())

# ��ѯtcuֵ�� ���� test_cluster,���ַ���,�����õ�id  ���ص�tcu����Ӧ��Ϊ0
if sc.get_tcu_num_by_id(test_cluster,NO_CPU_INFO,id) != 0:
     ci_exit(-1,line())

# ����һ����ͨ�û��ʺ�
ug  = usergroup(server,encrypted_root_session)
if ug.allocate("test_group1",GROUP_TYPE1) is not True:
    ci_exit(-1,line())
u  = user(server,encrypted_root_session)
if  u.allocate("guest","test","test",ACCOUNTUSER,"test_group1") != True:
    ci_exit(-1,line())

ns = login(server,"guest","test")
if ns is None:
    ci_exit(-1,line(),"failed to get test session!")

# ʹ����ͨ�û��ʺŽ��в�ѯ   ���� $test_cluster,���ַ���,�����õ�id  Ӧ��ʧ��
nsc = tcu(server,ns)
if nsc.get_tcu_num_by_id(test_cluster,"",id) != None:
    ci_exit(-1,line(),"test shouldn't query successfully !")
# ʹ����ͨ�û��ʺŽ�������   ���� $test_cluster,���ַ���,�����õ�id, 4,test_description   Ӧ��ʧ��
if nsc.set(test_cluster,NO_CPU_INFO,id,TCU_NUM4,"test_description") != False:
    ci_exit(-1,line(),"test shouldn't set successfully!")





