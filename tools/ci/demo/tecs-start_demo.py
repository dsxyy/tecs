#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_001.py
# �������ݣ����������������������
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
import sys,os,getopt,time

sys.path.append("/opt/tecs/tc/cli") 

import hashlib
import unittest 
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
from tecs.vmtemplate import *
from tecs.tecssystem import *
from tecs.file import *

# ��DEMO�Ļ������ã����ʹ���Զ�����װ��ϵͳ��������Щ��������Ҫ�޸ģ�����Լ��ֶ���װ��ϵͳ����Ҫ����TECSϵͳ��ʵ��������޸�
#DEMO��tc xmlrpc��������ַ
server_addr = "localhost"
#DEMO��tc xmlrpc�������˿ں�
server_port = "8080"
#DEMO��tc application����������
test_cloud = "tecscloud"
#DEMO��cc application������Ⱥ����
test_cluster = "tecscluster"
#DEMO��hc application�������������
test_host = "localhost"
#����Ա�˺ź�����
test_root_user = "admin"
test_root_passwd = "admin"



# �������Щ�ǹ��ô��룬����Ҫ�޸�
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
        
# **********************************��ʼ����demo**************************************
print "===================TECS Demo START==================="        
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

    
# ��¼����Ա,��ȡ����Աsession  �Ա�����ʹ��
root_session = login(server,test_root_user,test_root_passwd)
if root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")
#ϵͳ����״̬��ѯ EC:611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#�˴���ʱ40s������hc��MU_VM_HANDLER��MU_HOST_HEARTBEAT������
print "wait sys process work, please wait moment!!"
time.sleep(40)

# ��ѯ��ǰϵͳ�Ƿ���������
sys_runtime_query = tecssystem(server,root_session)
#��ѯTC״̬
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,"")) != 0:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,"")
    ci_exit_ex(-1,line(),"TC status is abnormal!")
#��ѯCC״̬
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cluster)) != 0:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cluster)
    ci_exit_ex(-1,line(),"CC status is abnormal!")
#��ѯHC״̬
test_cc_hc = test_cluster + "," + test_host
hc_state = int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cc_hc))
if  hc_state != 0:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cc_hc)
    if hc_state > 2:
        ci_exit_ex(-1,line(),"HC status is abnormal!")

        
root_cluster_manager_method = clustermanager(server,root_session) 
# ��ѯ�¼�Ⱥ
if root_cluster_manager_method.query(0L,100L,1) != True:
    ci_exit(-1)

# �ȴ� 20S �ȴ���Դ�ϱ�
time.sleep(20) 

root_host_method = host(server,root_session)
if root_host_method.query(0L,100L,test_cluster,test_host) != True:
    ci_exit(-1)

# ����tcu 8��
cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)    
if cpu_id is None:
    ci_exit(-1)
print "cpu id of host %s = %d" % (test_host,cpu_id)    

root_tcu_method = tcu(server,root_session)
if root_tcu_method.set(test_cluster,"",cpu_id,8,"test cpu description") != True:
    ci_exit(-1)

    
# ------------------------------��ʼ�����û���DEMO��������------------------------------
# ����Ҫ�޸�DEMO���ã����Բο���������������޸ļ���
# ------------------------------��ʼ�����û���DEMO��������------------------------------

# ------------------�ϴ�ӳ��------------------
# ӳ���ϴ������ò���,��������img ,���Ը��������demo_image������
demo_image = "/home/tecs_install/ttylinux0.img"
upload_image_file(server_addr,test_root_user,test_root_passwd,demo_image)

# �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
i = image(server,root_session)
image_id = i.get_id_by_filename(os.path.basename(demo_image))
if image_id is None:
    ci_exit(-1,"failed to get image id of file %s!",os.path.basename(demo_image))
     
#����ӳ��
i.publish(image_id,IS_PUBLIC)

    
#�����Ե����������
test_vm_name = "ttylinux"

     
# ------------------��������------------------
# ��Ҫ�����Ĺ�������
demo_project_name = "demo_project"

root_project = project(server,root_session)
if root_project.allocate(demo_project_name,"demo porject description") != True:
    ci_exit(-1)

# �����Ҫ������һ�����̣��ο�����������дһ������
#demo_project_name1 = "demo_project1"
#if root_project.allocate(demo_project_name1,"demo porject description1") != True:
#    ci_exit(-1)
    
# ------------------���������ģ��------------------
#�����ģ���������
cfg_base = {}
# �����CPU����
cfg_base["vcpu"] = 1
# �����CPU����
cfg_base["tcu"] = 1
# ��������ڴ��С����λ�ֽ�
cfg_base["memory"] = long(128 * 1024 * 1024)
# ���������ֽڣ�����Ҫ����
cfg_base["cluster"] = ""
cfg_base["host"] = ""
# ���Ź�ʱ���������Ϊ0�����ʾ����������Ŀ��Ź�����
cfg_base["vm_wdtime"] = 0
# ӳ������ã�һ�㲻���޸ģ�ʹ�õĸ�DEMO�ϴ���image.
cfg_base["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
# ����������޸�
cfg_base["kernel"] = {"id":-1L,"size":0L}
cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
# �����ʾ��ȫ����������ǰ����⻯��Ĭ�ϲ���Ҫ�޸�
cfg_base["virt_type"] = "hvm"
# ����3���Ǹ߼����ã����������޸�
cfg_base["root_device"] = ""
cfg_base["kernel_command"] = ""
cfg_base["rawdata"] = ""
# ���������������ã����û���������������������洫�����ݵ����ã�һ����CD�ķ�ʽ�����������
# ��һ���������ļ����ƣ��ڶ����������ļ�������
cfg_base["context"] = {"file_name":"file_value", "file_name1":"file_value1"}

# ���������������ػ������ã�һ�㲻���ã����Ҫ���ã���ʽ����[{vm1_id,vm2_id},{vm3_id,vm4_id}]
cfg_base["qualifications"] = []

#�����Ǵ�������
#����һ������,target��ʾ�̷���postion�ֶβ�Ҫ��������Ĭ�ϾͿɣ�fstype��ʾ�����Ƿ���Ҫ��ʽ��""��ʾ����ʽ���������ʽ�Ǳ�ʾ��ʽΪext3,
#�����������ο�TECS������RPC�ӿ��ĵ�
disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":0,"fstype":"ext3"}
disk2 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdd","position":0,"fstype":"ext3"}
disks = [disk1,disk2]
cfg_base["disks"] = disks

#��������������,�����������ã���Ҫ��TECSϵͳ����������ƽ�棬�����������������ã���Ҫ����֮ǰ������������ƽ��
# �������"plane":"test_plane" ��ʾ���������ӵ�����ƽ�棬"dhcp":bool(1) ��ʾʹ��DHCP��ȡIP��ַ
#nic1 = {"pci_order":0,"plane":"test_plane","vlan":0,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#nic2 = {"pci_order":0,"plane":"test_plane","vlan":1,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#cfg_base["nics"] = [nic1,nic2]
    
cfg = {}
cfg["base_info"] = cfg_base
# �����ģ������
vt_demo_name = "vt_demo_name"
cfg["vt_name"] = vt_demo_name
# �����ģ���Ƿ񷢲�
cfg["is_public"] = bool(1)

# ����ģ�������ִ��
vt = vmtemplate(server,root_session)
if vt.allocate(cfg) != True:
    ci_exit_ex(-1,line())
   
# ------------------���������------------------
#�������������
cfg_base = {}
# �����CPU����
cfg_base["vcpu"] = 1
# �����CPU����
cfg_base["tcu"] = 1
# ��������ڴ��С����λ�ֽ�
cfg_base["memory"] = long(128 * 1024 * 1024)
# ���������ֽڣ�����Ҫ����
cfg_base["cluster"] = ""
cfg_base["host"] = ""
# ���Ź�ʱ���������Ϊ0�����ʾ����������Ŀ��Ź�����
cfg_base["vm_wdtime"] = 0
# ӳ������ã�һ�㲻���޸ģ�ʹ�õĸ�DEMO�ϴ���image.
cfg_base["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
# ����������޸�
cfg_base["kernel"] = {"id":-1L,"size":0L}
cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
# �����ʾ��ȫ����������ǰ����⻯��Ĭ�ϲ���Ҫ�޸�
cfg_base["virt_type"] = "hvm"
# ����3���Ǹ߼����ã����������޸�
cfg_base["root_device"] = ""
cfg_base["kernel_command"] = ""
cfg_base["rawdata"] = ""
# ���������������ã����û���������������������洫�����ݵ����ã�һ����CD�ķ�ʽ�����������
# ��һ���������ļ����ƣ��ڶ����������ļ�������
cfg_base["context"] = {"file_name":"file_value", "file_name1":"file_value1"}

# ���������������ػ������ã�һ�㲻���ã����Ҫ���ã���ʽ����[{vm1_id,vm2_id},{vm3_id,vm4_id}]
cfg_base["qualifications"] = []

#�����Ǵ�������
#����һ������,target��ʾ�̷���postion�ֶβ�Ҫ��������Ĭ�ϾͿɣ�fstype��ʾ�����Ƿ���Ҫ��ʽ��""��ʾ����ʽ���������ʽ�Ǳ�ʾ��ʽΪext3,
#�����������ο�TECS������RPC�ӿ��ĵ�
disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":0,"fstype":"ext3"}
disk2 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdd","position":0,"fstype":"ext3"}
disks = [disk1,disk2]
cfg_base["disks"] = disks

#��������������,�����������ã���Ҫ��TECSϵͳ����������ƽ�棬�����������������ã���Ҫ����֮ǰ������������ƽ��
# �������"plane":"test_plane" ��ʾ���������ӵ�����ƽ�棬"dhcp":bool(1) ��ʾʹ��DHCP��ȡIP��ַ
#nic1 = {"pci_order":0,"plane":"test_plane","vlan":0,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#nic2 = {"pci_order":0,"plane":"test_plane","vlan":1,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#cfg_base["nics"] = [nic1,nic2]
    
cfg = {}
cfg["base_info"] = cfg_base
# ���������������������
vm_demo1 = "vm_demo1"
cfg["vm_name"] = vm_demo1
# ������������������ڵĹ���
cfg["project_name"] = demo_project_name   
# ���������������������
cfg["vm_num"] = 1

#��ȡ����Ա����Ȩ��
v = vmcfg(server,root_session)
if v.allocate(cfg) != True:
    ci_exit_ex(-1,line())
#�������ѯvm_id
vm_id = v.get_vm_id(vm_demo1)
if vm_id is None:
    ci_exit_ex(-1,line(),"failed to get vm id!")
    
# ���������
if v.action(vm_id,"deploy") != True:
    ci_exit_ex(-1,line())





