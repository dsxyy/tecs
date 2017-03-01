#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：st_001.py

# 内容摘要：修改开发脚本进行测试。
# 当前版本：1.0
# 作    者：xujie
# 完成日期：2012/8/6

# #file:st_001--tecs系统TC/CC/HC合一初始化，虚拟机部署；
# arg:$1 addr:IP地址  $2 image:映像文件
# style:st_001.py -a -i
#*******************************************************************************/
import sys,os,getopt
import hashlib
import time
from st_comm_lib import *

#用于测试的tc xmlrpc服务器地址
server_addr = "localhost"
#用于测试的tc xmlrpc服务器端口号
server_port = "8080"
# tc/cc/hc的名字需要和start.sh中的定义保持一致！
#用于测试的tc application，即云名称
test_cloud = "st_cloud"
#用于测试的cc application，即集群名称
test_cluster = "st_cluster"
#用于测试的hc application，即物理机名称
test_host = "st_host"
#云管理员账号和密码
cloud_user = "admin"
cloud_passwd = "admin"
#普通用户组名称
default_group = "st_group"
#租户账号和密码
guest_user = "st_guest"
guest_passwd = "st_guest"
#映像文件存放主目录
image_home = "/home/ci_server/images/"
#用于上传，创建虚拟机的映像文件名
test_image_cgel = "CGEL0726.img"
#待测试的虚拟机名称
test_vmadmin_name = "cgel"
#待测试的工程名称
test_project_name = "default"
#待测试的工程名称admin
test_project_name1 = "default_admin"
#待测试的vt 名
test_vt_name1 = "adminvt1"

def usage():
    ''' 打印帮助信息'''
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

# 解析处理命令行参数
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:p:i:v", ["help", "addr=","port=","image="])
except getopt.GetoptError:
    # print help information and exit:
    usage()
    ci_exit(-2,"GetoptError")
    
for o, v in opts:
    #用-h或--help打印帮助
    if o in ("-h", "--help"):
        usage()
    #用-a或--addr可以指定tc的xmlrpc服务器地址，默认为localhost
    if o in ("-a", "--addr"):
        server_addr = v
    #用-p或--port可以指定tc的xmlrpc服务器端口号，默认为8080
    if o in ("-p", "--port"):
        server_port = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-i", "--image"):
        test_image_cgel = image_home + v
        if os.path.isfile(test_image_cgel) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image_cgel)

# 自动化集成测试开始!!!
print "===================TECS CI TEST START==================="        
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',cloud_user,':',cloud_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#列出当前支持的所有xmlrpc接口    
server.system.listMethods()

# 登录tecs，获取管理员账户加密后的session
encrypted_root_session = login(server,cloud_user,cloud_passwd)
if encrypted_root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")

print "!!!!!!!!!!!!!!!!!!START ADMIN TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" 
# ------------------管理员测试------------------

#  以管理员身份使用ftp客户端上传一个虚拟机machine映像
i = image(server,encrypted_root_session)
image_id = i.get_id_by_filename(os.path.basename(test_image_cgel))
if image_id is None:
    upload_image_file(server_addr,cloud_user,cloud_user,test_image_cgel)

# 根据文件名获取刚刚上传的映像文件在tecs中的id
i = image(server,encrypted_root_session)
image_id = i.get_id_by_filename(os.path.basename(test_image_cgel))
if image_id is None:
    ci_exit(-1,"failed to get image id of file %s!" % os.path.basename(test_image_cgel))
    
print "image id of file %s = %d" % (test_image_cgel,image_id)

# 注册映像
if i.query(0L,200L,1,"",image_id,os.path.basename(test_image_cgel)) != True:
    if i.register(image_id,"cgel","machine","x86_64",False,True,"test image description") != True:
        st_exit(-1, lines())
    
# 创建工程
root_project = project(server,encrypted_root_session)
if root_project.allocate(test_project_name1,"test project description") != True:
    st_exit(-1)
    
# 创建模板测试
# 测试使用快速创建接口来进行创建参数如下：   应该成功
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
# 发布image
#Vmtemplate cannot public, for Image( -1 ) not public
if i.publish(image_id,bool(1)) != True:
    st_exit(-1,line())
   
# 创件模板：   应该成功 
if vt.allocate(cfg) != True:
    st_exit(-1,line())
    
disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":0,"fstype":"ext3"}
# test_vt_name 和 disks = [disk1]
cfg["base_info"]["disks"] = [disk1]
if vt.set(cfg) != True:
    st_exit(-1,line())
# 创建磁盘  参数如下    应当成功
disk2 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext4"}
# test_vt_name 和 disks = [disk2]
cfg["base_info"]["disks"] = [disk2]
if vt.set(cfg) != True:
    st_exit(-1,line())
   
#根据模板创建虚拟机 应该成功
v = vmcfg(server,encrypted_root_session)
if v.allocate_by_vt(test_vt_name1,test_project_name1,test_vmadmin_name,"cgel",1) != True:
    st_exit(-1,line())
# 根据名称获取刚刚创建的虚拟机的id 应该成功
vm_id = v.get_vm_id(test_vmadmin_name)
if vm_id is None:
    st_exit(-1,line(),"failed to get vm id!")

cm = clustermanager(server,encrypted_root_session) 
 #等待tc上集群资源信息到位 
count = 0   
while (cm.query(0L,100L,1) != True):
    time.sleep(3)
    count = count + 1
    print "cluster query syn ", count
    if (count >= 19):
        st_exit(-1,lines(), "cluster query syn failed!")

#print "waiting for hc power on ..."    
#time.sleep(180)   

 
# 部署虚拟机 应该成功
if v.action(vm_id,"deploy") != True:
    st_exit(-1,line())

        
# 获取虚拟机状态   应该是部署成功态，状态6为位置状态
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=2 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,line())           

# 重启虚拟机
if v.action(vm_id,"reboot") != True:
    st_exit(-1,lines())

# 获取虚拟机状态，应该为2    
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=2 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
# 取消虚拟机
if v.action(vm_id,"cancel") != True:
    st_exit(-1,lines())     
#取消需要3分钟，这里延时200秒
print "wait 200s" 
time.sleep(200)
# 获取虚拟机状态，应该为0 
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=0 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
#再次部署虚拟机
if v.action(vm_id,"deploy") != True:
    st_exit(-1,lines())
# 获取虚拟机状态，应该为2
count = 0
vm_state=6   
while (v.get_vm_state(vm_id)!=2 and v.get_vm_state(vm_id)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vm_id)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())          
    
