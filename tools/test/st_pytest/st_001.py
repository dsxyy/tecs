#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：st_001.py

# 内容摘要：修改开发脚本进行测试。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012/4/17

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
test_image = "ttylinux0.img"
#待测试的虚拟机名称
test_vm_name = "ttylinux"
#待测试的工程名称
test_project_name = "default"
#待测试的工程名称admin

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
        test_image = image_home + v
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)

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

    
# 注册cc到tc
print "=============================================="
cm = clustermanager(server,encrypted_root_session) 
#if cm.register(test_cluster,"test") != True:
#    ci_exit(-1)

#TODO 需要增加循环等待
count=0
while (cm.get_cluster_max_count() == 0 or cm.get_cluster_max_count()== None):
    time.sleep(3)
    count = count + 1    
    if(count >= 19):
        st_exit(-1,lines())
  
print "get_cluster_max_count count= %d "% count 
 
#查询集群列表
if cm.query(0L,100L,0) != True:
    st_exit(-1, lines())

#查询第一个存在的集群  应该能查询到
fisrt_clustername=cm.get_fisrt_clustername()
if fisrt_clustername is None:
    print "get_fisrt_clustername fail"
    st_exit(-1,lines())
else:
    print "fisrt_clustername is %s" % fisrt_clustername
    
#同步方式查看集群的在线状态  应该是在线的
if cm.get_is_online(fisrt_clustername,1) != 1:
    st_exit(-1,lines())
    
#设置查询到的集群   应该成功
if cm.set(fisrt_clustername,"test set") != True:
    st_exit(-1,lines())

#再次修改集群描述信息   应该成功
if cm.set(fisrt_clustername,"test modify for tecs") != True:
    st_exit(-1,lines())

#查看当前最大core数目
corenum=cm.get_core_num()
if corenum is None:
    st_exit(-1,line())
else:
    print "corenum is %s" % corenum
    
#查看当前最大tcu数目
tcunum=cm.get_tcu_num()
if tcunum is None:
    st_exit(-1,line())     
else:
    print "tcunum is %s" % tcunum
    
# 注册hc到cc
print "=============================================="
h = host(server,encrypted_root_session)
#if h.register(test_cluster,test_host,False,"test") != True:
#    ci_exit(-1)   
    
#TODO 需要增加循环等待    
#if h.query(0L,100L,test_cluster,test_host) != True:
#    ci_exit(-1)
count=0
while (h.query_register_state(test_cluster,test_host) != 2 ):
    time.sleep(3)
    count = count + 1    
    if(count >= 19):
        st_exit(-1,lines())

# 设置svcpu 
print "=============================================="
# 获取刚刚注册的主机的cpu id   
cpu_id = h.get_cpu_id(test_cluster,test_host)    
if cpu_id is None:
    st_exit(-1, lines())
print "cpu id of host %s = %d" % (test_host,cpu_id)    

#将cpu能力默认提高到获取的tcunum
s = svcpu(server,encrypted_root_session)
count=0
while (s.set(test_cluster,"",cpu_id,tcunum,"test cpu description") != True):
    time.sleep(3)
    count = count + 1
    if(count >= 19):
        st_exit(-1, lines())
if s.query(0L,100L,test_cluster,"",cpu_id) != True:
    st_exit(-1, lines())   

# 创建一个类型为1的用户组，用于容纳普通租户的账号 
print "=============================================="
ug = usergroup(server,encrypted_root_session)
if ug.query(0L,100L,default_group) != True:
    if ug.allocate(default_group,1) != True:
        st_exit(-1, lines())
    
# 创建一个普通账号用于后续模拟来自租户的操作 
u = user(server,encrypted_root_session)
if u.query(0L,100L,guest_user) != True:
    if u.allocate(guest_user,guest_passwd,guest_passwd,3,default_group) != True:
        st_exit(-1, lines())
    
# 使用普通租户账户登录tecs，获取该账户加密后的session
encrypted_tenant_session = login(server,guest_user,guest_passwd)
if encrypted_tenant_session is None:
    ci_exit(-1,"failed to get encrypted tenant session!")
    
# 使用ftp客户端上传一个虚拟机machine映像
i = image(server,encrypted_tenant_session)
image_id = i.get_id_by_filename(os.path.basename(test_image))
if image_id is None:
    upload_image_file(server_addr,guest_user,guest_passwd,test_image)

# 根据文件名获取刚刚上传的映像文件在tecs中的id
i = image(server,encrypted_tenant_session)
image_id = i.get_id_by_filename(os.path.basename(test_image))
if image_id is None:
    ci_exit(-1,"failed to get image id of file %s!" % os.path.basename(test_image))
    
# 注册映像
print "image id of file %s = %d" % (test_image,image_id)
if i.query(0L,200L,1,"",image_id,os.path.basename(test_image)) != True:
    if i.register(image_id,"test","machine","x86_64",False,True,"test image description") != True:
        st_exit(-1, lines())

# 设置一个网络平面

# 查询网络平面名称  不需要和设置的一样，有可能是下面上报的

# 配置DHCP

# 创建工程
p = project(server,encrypted_tenant_session)
if p.query_count(0L,100L,-5L,"","") == 0:
    if p.allocate(test_project_name,"test project description") != True:
        st_exit(-1, lines())

# 创建虚拟机
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


# 根据名称获取刚刚创建的虚拟机的id    
vmid = v.get_vm_id(test_vm_name)
if vmid is None:
    ci_exit(-1,"failed to get vm id!")

#获取虚拟机信息
if v.query(vmid) != True:
    ci_exit(-1,"failed to get vm info!")

# 获取虚拟机状态
vmstate = v.get_vm_state(vmid)
if vmstate is None:
    ci_exit(-1,"failed to get vm state!")   
    
#增加一个磁盘
disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
disks = [disk1]
if v.set_disk(vmid,disks) != True:
    st_exit(-1,lines())

#增加一个网卡
#nic1={"pci_order":0,"plane":"","vlan":0,"dhcp":bool(1),"ip":"10.43.180.250","netmask":"255.255.254.0","gateway":"10.43.180.1","mac":""}
#nics = [nic1]
#if v.set_nic(vmid,nics) != True:
#    st_exit(-1,lines())

#等待tc上集群资源信息到位 
count = 0   
while (cm.query(0L,100L,1) != True):
    time.sleep(3)
    count = count + 1
    print "cluster query syn ", count
    if (count >= 19):
        st_exit(-1,lines(), "cluster query syn failed!")

print "waiting for hc power on ..."    
time.sleep(180)   
    
# 部署虚拟机
if v.action(1L,"deploy") != True:
    st_exit(-1,lines())
    
# 获取虚拟机状态   应该是部署成功态，状态6为位置状态
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=2 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())           

# 重启虚拟机
if v.action(1L,"reboot") != True:
    st_exit(-1,lines())

# 获取虚拟机状态，应该为2    
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=2 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
# 取消虚拟机
if v.action(1L,"cancel") != True:
    st_exit(-1,lines())     
#取消需要3分钟，这里延时200秒
print "wait 200s" 
time.sleep(200)
# 获取虚拟机状态，应该为0 
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=0 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())  
        
#再次部署虚拟机
if v.action(1L,"deploy") != True:
    st_exit(-1,lines())
# 获取虚拟机状态，应该为2
count = 0
vm_state=6   
while (v.get_vm_state(vmid)!=2 and v.get_vm_state(vmid)!= None):
    time.sleep(10)
    count = count + 1
    if(count >= 17):
        vm_state=v.get_vm_state(vmid)
        print "state error,now vm_state is %d" %vm_state
        st_exit(-1,lines())          
# 检测磁盘是否存在

# 检测网络是否通

# cancle虚拟机  应该成功

# 等3分钟

# 检测磁盘是否撤销  应该撤销

# 检测映像盘是否撤销 应该撤销

# 删除虚拟机  

# ------------------管理员测试, 放在了 st_001_admin.py------------------
 
    
