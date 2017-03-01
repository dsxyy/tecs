#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012��FreeWorld��All rights reserved.
# 
# �ļ����ƣ�st_001.py

# ����ժҪ���޸Ŀ����ű����в��ԡ�
# ��ǰ�汾��1.0
# ��    �ߣ�xujie
# ������ڣ�2012/8/6

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
test_image_cgel = "CGEL0726.img"
#�����Ե����������
test_vmadmin_name = "cgel"
#�����ԵĹ�������
test_project_name = "default"
#�����ԵĹ�������admin
test_project_name1 = "default_admin"
#�����Ե�vt ��
test_vt_name1 = "adminvt1"

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
        test_image_cgel = image_home + v
        if os.path.isfile(test_image_cgel) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image_cgel)

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

print "!!!!!!!!!!!!!!!!!!START ADMIN TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" 
# ------------------����Ա����------------------

#  �Թ���Ա���ʹ��ftp�ͻ����ϴ�һ�������machineӳ��
i = image(server,encrypted_root_session)
image_id = i.get_id_by_filename(os.path.basename(test_image_cgel))
if image_id is None:
    upload_image_file(server_addr,cloud_user,cloud_user,test_image_cgel)

# �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
i = image(server,encrypted_root_session)
image_id = i.get_id_by_filename(os.path.basename(test_image_cgel))
if image_id is None:
    ci_exit(-1,"failed to get image id of file %s!" % os.path.basename(test_image_cgel))
    
print "image id of file %s = %d" % (test_image_cgel,image_id)

# ע��ӳ��
if i.query(0L,200L,1,"",image_id,os.path.basename(test_image_cgel)) != True:
    if i.register(image_id,"cgel","machine","x86_64",False,True,"test image description") != True:
        st_exit(-1, lines())
    
# ��������
root_project = project(server,encrypted_root_session)
if root_project.allocate(test_project_name1,"test project description") != True:
    st_exit(-1)
    
# ����ģ�����
# ����ʹ�ÿ��ٴ����ӿ������д����������£�   Ӧ�óɹ�
cfg = {}
cfg["vt_name"] = test_vt_name1
cfg["is_public"] = bool(1)
cfg["base_info"] = {}
cfg["base_info"]["vcpu"] = 2
cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
cfg["base_info"]["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
cfg["base_info"]["kernel"] = {"id":-1L,"size":0L}
cfg["base_info"]["ramdisk"] = {"id":-1L,"size":0L}
cfg["base_info"]["virt_type"] = "hvm"
cfg["base_info"]["root_device"] = ""
cfg["base_info"]["kernel_command"] = ""
cfg["base_info"]["rawdata"] = ""
#cfg["base_info"]["disks"] = [{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":0,"fstype":"ext2"}]

vt = vmtemplate(server,encrypted_root_session)
# ����image
#Vmtemplate cannot public, for Image( -1 ) not public
if i.publish(image_id,bool(1)) != True:
    st_exit(-1,line())
   
# ����ģ�壺   Ӧ�óɹ� 
if vt.allocate(cfg) != True:
    st_exit(-1,line())
    
disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":0,"fstype":"ext3"}
# test_vt_name �� disks = [disk1]
cfg["base_info"]["disks"] = [disk1]
if vt.set(cfg) != True:
    st_exit(-1,line())
# ��������  ��������    Ӧ���ɹ�
disk2 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext4"}
# test_vt_name �� disks = [disk2]
cfg["base_info"]["disks"] = [disk2]
if vt.set(cfg) != True:
    st_exit(-1,line())
   
#����ģ�崴������� Ӧ�óɹ�
v = vmcfg(server,encrypted_root_session)
if v.allocate_by_vt(test_vt_name1,test_project_name1,test_vmadmin_name,"cgel",1) != True:
    st_exit(-1,line())
# �������ƻ�ȡ�ոմ������������id Ӧ�óɹ�
vm_id = v.get_vm_id(test_vmadmin_name)
if vm_id is None:
    st_exit(-1,line(),"failed to get vm id!")

cm = clustermanager(server,encrypted_root_session) 
 #�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
count = 0   
while (cm.query(0L,100L,1) != True):
    time.sleep(3)
    count = count + 1
    print "cluster query syn ", count
    if (count >= 19):
        st_exit(-1,lines(), "cluster query syn failed!")

#print "waiting for hc power on ..."    
#time.sleep(180)   

 
# ��������� Ӧ�óɹ�
if v.action(vm_id,"deploy") != True:
    st_exit(-1,line())

        
# ��ȡ�����״̬   Ӧ���ǲ���ɹ�̬��״̬6Ϊλ��״̬
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=2 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,line())           

# ���������
if v.action(vm_id,"reboot") != True:
    st_exit(-1,lines())

# ��ȡ�����״̬��Ӧ��Ϊ2    
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=2 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
# ȡ�������
if v.action(vm_id,"cancel") != True:
    st_exit(-1,lines())     
#ȡ����Ҫ3���ӣ�������ʱ200��
print "wait 200s" 
time.sleep(200)
# ��ȡ�����״̬��Ӧ��Ϊ0 
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=0 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
#�ٴβ��������
if v.action(vm_id,"deploy") != True:
    st_exit(-1,lines())
# ��ȡ�����״̬��Ӧ��Ϊ2
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=2 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())          
    
