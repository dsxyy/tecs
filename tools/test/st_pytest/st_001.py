#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012��FreeWorld��All rights reserved.
# 
# �ļ����ƣ�st_001.py

# ����ժҪ���޸Ŀ����ű����в��ԡ�
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012/4/17

# #file:st_001--tecsϵͳTC/CC/HC��һ��ʼ�������������
# arg:$1 addr:IP��ַ  $2 image:ӳ���ļ�
# style:st_001.py -a -i
#*******************************************************************************/
import sys,os,getopt
import hashlib
import time
from st_comm_lib import *

#���ڲ��Ե�tc xmlrpc��������ַ
server_addr = "localhost"
#���ڲ��Ե�tc xmlrpc�������˿ں�
server_port = "8080"
# tc/cc/hc��������Ҫ��start.sh�еĶ��屣��һ�£�
#���ڲ��Ե�tc application����������
test_cloud = "st_cloud"
#���ڲ��Ե�cc application������Ⱥ����
test_cluster = "st_cluster"
#���ڲ��Ե�hc application�������������
test_host = "st_host"
#�ƹ���Ա�˺ź�����
cloud_user = "admin"
cloud_passwd = "admin"
#��ͨ�û�������
default_group = "st_group"
#�⻧�˺ź�����
guest_user = "st_guest"
guest_passwd = "st_guest"
#ӳ���ļ������Ŀ¼
image_home = "/home/ci_server/images/"
#�����ϴ��������������ӳ���ļ���
test_image = "ttylinux0.img"
#�����Ե����������
test_vm_name = "ttylinux"
#�����ԵĹ�������
test_project_name = "default"
#�����ԵĹ�������admin

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
        test_image = image_home + v
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)

# �Զ������ɲ��Կ�ʼ!!!
print "===================TECS CI TEST START==================="        
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',cloud_user,':',cloud_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�    
server.system.listMethods()

# ��¼tecs����ȡ����Ա�˻����ܺ��session
encrypted_root_session = login(server,cloud_user,cloud_passwd)
if encrypted_root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")

    
# ע��cc��tc
print "=============================================="
cm = clustermanager(server,encrypted_root_session) 
#if cm.register(test_cluster,"test") != True:
#    ci_exit(-1)

#TODO ��Ҫ����ѭ���ȴ�
count=0
while (cm.get_cluster_max_count() == 0 or cm.get_cluster_max_count()== None):
    time.sleep(3)
    count = count + 1    
    if(count >= 19):
        st_exit(-1,lines())
  
print "get_cluster_max_count count= %d "% count 
 
#��ѯ��Ⱥ�б�
if cm.query(0L,100L,0) != True:
    st_exit(-1, lines())

#��ѯ��һ�����ڵļ�Ⱥ  Ӧ���ܲ�ѯ��
fisrt_clustername=cm.get_fisrt_clustername()
if fisrt_clustername is None:
    print "get_fisrt_clustername fail"
    st_exit(-1,lines())
else:
    print "fisrt_clustername is %s" % fisrt_clustername
    
#ͬ����ʽ�鿴��Ⱥ������״̬  Ӧ�������ߵ�
if cm.get_is_online(fisrt_clustername,1) != 1:
    st_exit(-1,lines())
    
#���ò�ѯ���ļ�Ⱥ   Ӧ�óɹ�
if cm.set(fisrt_clustername,"test set") != True:
    st_exit(-1,lines())

#�ٴ��޸ļ�Ⱥ������Ϣ   Ӧ�óɹ�
if cm.set(fisrt_clustername,"test modify for tecs") != True:
    st_exit(-1,lines())

#�鿴��ǰ���core��Ŀ
corenum=cm.get_core_num()
if corenum is None:
    st_exit(-1,line())
else:
    print "corenum is %s" % corenum
    
#�鿴��ǰ���tcu��Ŀ
tcunum=cm.get_tcu_num()
if tcunum is None:
    st_exit(-1,line())     
else:
    print "tcunum is %s" % tcunum
    
# ע��hc��cc
print "=============================================="
h = host(server,encrypted_root_session)
#if h.register(test_cluster,test_host,False,"test") != True:
#    ci_exit(-1)   
    
#TODO ��Ҫ����ѭ���ȴ�    
#if h.query(0L,100L,test_cluster,test_host) != True:
#    ci_exit(-1)
count=0
while (h.query_register_state(test_cluster,test_host) != 2 ):
    time.sleep(3)
    count = count + 1    
    if(count >= 19):
        st_exit(-1,lines())

# ����svcpu 
print "=============================================="
# ��ȡ�ո�ע���������cpu id   
cpu_id = h.get_cpu_id(test_cluster,test_host)    
if cpu_id is None:
    st_exit(-1, lines())
print "cpu id of host %s = %d" % (test_host,cpu_id)    

#��cpu����Ĭ����ߵ���ȡ��tcunum
s = svcpu(server,encrypted_root_session)
count=0
while (s.set(test_cluster,"",cpu_id,tcunum,"test cpu description") != True):
    time.sleep(3)
    count = count + 1
    if(count >= 19):
        st_exit(-1, lines())
if s.query(0L,100L,test_cluster,"",cpu_id) != True:
    st_exit(-1, lines())   

# ����һ������Ϊ1���û��飬����������ͨ�⻧���˺� 
print "=============================================="
ug = usergroup(server,encrypted_root_session)
if ug.query(0L,100L,default_group) != True:
    if ug.allocate(default_group,1) != True:
        st_exit(-1, lines())
    
# ����һ����ͨ�˺����ں���ģ�������⻧�Ĳ��� 
u = user(server,encrypted_root_session)
if u.query(0L,100L,guest_user) != True:
    if u.allocate(guest_user,guest_passwd,guest_passwd,3,default_group) != True:
        st_exit(-1, lines())
    
# ʹ����ͨ�⻧�˻���¼tecs����ȡ���˻����ܺ��session
encrypted_tenant_session = login(server,guest_user,guest_passwd)
if encrypted_tenant_session is None:
    ci_exit(-1,"failed to get encrypted tenant session!")
    
# ʹ��ftp�ͻ����ϴ�һ�������machineӳ��
i = image(server,encrypted_tenant_session)
image_id = i.get_id_by_filename(os.path.basename(test_image))
if image_id is None:
    upload_image_file(server_addr,guest_user,guest_passwd,test_image)

# �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
i = image(server,encrypted_tenant_session)
image_id = i.get_id_by_filename(os.path.basename(test_image))
if image_id is None:
    ci_exit(-1,"failed to get image id of file %s!" % os.path.basename(test_image))
    
# ע��ӳ��
print "image id of file %s = %d" % (test_image,image_id)
if i.query(0L,200L,1,"",image_id,os.path.basename(test_image)) != True:
    if i.register(image_id,"test","machine","x86_64",False,True,"test image description") != True:
        st_exit(-1, lines())

# ����һ������ƽ��

# ��ѯ����ƽ������  ����Ҫ�����õ�һ�����п����������ϱ���

# ����DHCP

# ��������
p = project(server,encrypted_tenant_session)
if p.query_count(0L,100L,-5L,"","") == 0:
    if p.allocate(test_project_name,"test project description") != True:
        st_exit(-1, lines())

# ���������
cfg = {}
cfg["vm_name"] = test_vm_name
cfg["project_name"] = test_project_name
cfg["vm_num"] = 1
cfg["vcpu"] = 2
cfg["memory"] = long(128 * 1024 * 1024)
cfg["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
cfg["kernel"] = {"id":-1L,"size":0L}
cfg["ramdisk"] = {"id":-1L,"size":0L}
cfg["virt_type"] = "hvm"
#cfg["root_device"] = 
#cfg["kernel_command"] = 
#cfg["rawdata"] = 
v = vmcfg(server,encrypted_tenant_session)
vmid = v.get_vm_id(test_vm_name)
if vmid is None:
    if v.allocate_quick(cfg) != True:
        ci_exit(-1)


# �������ƻ�ȡ�ոմ������������id    
vmid = v.get_vm_id(test_vm_name)
if vmid is None:
    ci_exit(-1,"failed to get vm id!")

#��ȡ�������Ϣ
if v.query(vmid) != True:
    ci_exit(-1,"failed to get vm info!")

# ��ȡ�����״̬
vmstate = v.get_vm_state(vmid)
if vmstate is None:
    ci_exit(-1,"failed to get vm state!")   
    
#����һ������
disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
disks = [disk1]
if v.set_disk(vmid,disks) != True:
    st_exit(-1,lines())

#����һ������
#nic1={"pci_order":0,"plane":"","vlan":0,"dhcp":bool(1),"ip":"10.43.180.250","netmask":"255.255.254.0","gateway":"10.43.180.1","mac":""}
#nics = [nic1]
#if v.set_nic(vmid,nics) != True:
#    st_exit(-1,lines())

#�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
count = 0   
while (cm.query(0L,100L,1) != True):
    time.sleep(3)
    count = count + 1
    print "cluster query syn ", count
    if (count >= 19):
        st_exit(-1,lines(), "cluster query syn failed!")

print "waiting for hc power on ..."    
time.sleep(180)   
    
# ���������
if v.action(1L,"deploy") != True:
    st_exit(-1,lines())
    
# ��ȡ�����״̬   Ӧ���ǲ���ɹ�̬��״̬6Ϊλ��״̬
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=2 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())           

# ���������
if v.action(1L,"reboot") != True:
    st_exit(-1,lines())

# ��ȡ�����״̬��Ӧ��Ϊ2    
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=2 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
# ȡ�������
if v.action(1L,"cancel") != True:
    st_exit(-1,lines())     
#ȡ����Ҫ3���ӣ�������ʱ200��
print "wait 200s" 
time.sleep(200)
# ��ȡ�����״̬��Ӧ��Ϊ0 
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=0 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
#�ٴβ��������
if v.action(1L,"deploy") != True:
    st_exit(-1,lines())
# ��ȡ�����״̬��Ӧ��Ϊ2
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=2 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())          
# �������Ƿ����

# ��������Ƿ�ͨ

# cancle�����  Ӧ�óɹ�

# ��3����

# �������Ƿ���  Ӧ�ó���

# ���ӳ�����Ƿ��� Ӧ�ó���

# ɾ�������  

# ------------------����Ա����, ������ st_001_admin.py------------------
 
    
