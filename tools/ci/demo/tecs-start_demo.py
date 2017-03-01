#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_001.py
# 测试内容：测试虚拟机创建部署流程
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
# 修改记录2：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息
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

# 本DEMO的基本配置，如果使用自动化安装的系统，下面这些参数不需要修改，如果自己手动安装的系统，需要根据TECS系统真实情况进行修改
#DEMO的tc xmlrpc服务器地址
server_addr = "localhost"
#DEMO的tc xmlrpc服务器端口号
server_port = "8080"
#DEMO的tc application，即云名称
test_cloud = "tecscloud"
#DEMO的cc application，即集群名称
test_cluster = "tecscluster"
#DEMO的hc application，即物理机名称
test_host = "localhost"
#管理员账号和密码
test_root_user = "admin"
test_root_passwd = "admin"



# 下面的这些是公用代码，不需要修改
def line():
    try:
            raise Exception
    except:
            return sys.exc_info()[2].tb_frame.f_back.f_lineno
def file():
    return inspect.currentframe().f_code.co_filename

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

def ci_exit_ex(value=0,line = line(),error="null",):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, line = %d, error string = %s" % (value,line,error)
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
        test_image = v
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)
        
# **********************************开始构建demo**************************************
print "===================TECS Demo START==================="        
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

    
# 登录管理员,获取管理员session  以备后面使用
root_session = login(server,test_root_user,test_root_passwd)
if root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")
#系统运行状态查询 EC:611003077308 tecs提供rpc接口查询系统运行时信息
#此处延时40s，否则hc的MU_VM_HANDLER和MU_HOST_HEARTBEAT不存在
print "wait sys process work, please wait moment!!"
time.sleep(40)

# 查询当前系统是否运行正常
sys_runtime_query = tecssystem(server,root_session)
#查询TC状态
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,"")) != 0:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,"")
    ci_exit_ex(-1,line(),"TC status is abnormal!")
#查询CC状态
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cluster)) != 0:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cluster)
    ci_exit_ex(-1,line(),"CC status is abnormal!")
#查询HC状态
test_cc_hc = test_cluster + "," + test_host
hc_state = int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cc_hc))
if  hc_state != 0:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cc_hc)
    if hc_state > 2:
        ci_exit_ex(-1,line(),"HC status is abnormal!")

        
root_cluster_manager_method = clustermanager(server,root_session) 
# 查询下集群
if root_cluster_manager_method.query(0L,100L,1) != True:
    ci_exit(-1)

# 等待 20S 等待资源上报
time.sleep(20) 

root_host_method = host(server,root_session)
if root_host_method.query(0L,100L,test_cluster,test_host) != True:
    ci_exit(-1)

# 设置tcu 8　
cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)    
if cpu_id is None:
    ci_exit(-1)
print "cpu id of host %s = %d" % (test_host,cpu_id)    

root_tcu_method = tcu(server,root_session)
if root_tcu_method.set(test_cluster,"",cpu_id,8,"test cpu description") != True:
    ci_exit(-1)

    
# ------------------------------开始构建用户的DEMO基本配置------------------------------
# 当需要修改DEMO配置，可以参考下面的描述进行修改即可
# ------------------------------开始构建用户的DEMO基本配置------------------------------

# ------------------上传映像------------------
# 映像上传，配置部分,如果想更换img ,可以根换下面的demo_image的内容
demo_image = "/home/tecs_install/ttylinux0.img"
upload_image_file(server_addr,test_root_user,test_root_passwd,demo_image)

# 根据文件名获取刚刚上传的映像文件在tecs中的id
i = image(server,root_session)
image_id = i.get_id_by_filename(os.path.basename(demo_image))
if image_id is None:
    ci_exit(-1,"failed to get image id of file %s!",os.path.basename(demo_image))
     
#发布映像
i.publish(image_id,IS_PUBLIC)

    
#待测试的虚拟机名称
test_vm_name = "ttylinux"

     
# ------------------创建工程------------------
# 需要创建的工程名称
demo_project_name = "demo_project"

root_project = project(server,root_session)
if root_project.allocate(demo_project_name,"demo porject description") != True:
    ci_exit(-1)

# 如果需要再增加一个工程，参考下面用例再写一个即可
#demo_project_name1 = "demo_project1"
#if root_project.allocate(demo_project_name1,"demo porject description1") != True:
#    ci_exit(-1)
    
# ------------------创建虚拟机模板------------------
#虚拟机模板基本配置
cfg_base = {}
# 虚拟机CPU数量
cfg_base["vcpu"] = 1
# 虚拟机CPU能力
cfg_base["tcu"] = 1
# 虚拟机的内存大小，单位字节
cfg_base["memory"] = long(128 * 1024 * 1024)
# 下面两个字节，不需要配置
cfg_base["cluster"] = ""
cfg_base["host"] = ""
# 看门狗时长，如果不为0，则表示启动虚拟机的看门狗功能
cfg_base["vm_wdtime"] = 0
# 映像的配置，一般不用修改，使用的该DEMO上传的image.
cfg_base["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
# 下面两项不用修改
cfg_base["kernel"] = {"id":-1L,"size":0L}
cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
# 该项表示是全虚拟机化还是半虚拟化，默认不需要修改
cfg_base["virt_type"] = "hvm"
# 下面3项是高级配置，基本不用修改
cfg_base["root_device"] = ""
cfg_base["kernel_command"] = ""
cfg_base["rawdata"] = ""
# 下面是上下文配置，即用户在虚拟机外面给虚拟机里面传递数据的配置，一般以CD的方式传到虚拟机中
# 第一个参数是文件名称，第二个参数是文件的内容
cfg_base["context"] = {"file_name":"file_value", "file_name1":"file_value1"}

# 下面是虚拟机互斥关机的配置，一般不配置，如果要配置，格式如下[{vm1_id,vm2_id},{vm3_id,vm4_id}]
cfg_base["qualifications"] = []

#下面是磁盘配置
#增加一个磁盘,target表示盘符，postion字段不要动，采用默认就可，fstype表示磁盘是否需要格式化""表示不格式话，下面格式是表示格式为ext3,
#具体参数，请参考TECS发布的RPC接口文档
disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":0,"fstype":"ext3"}
disk2 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdd","position":0,"fstype":"ext3"}
disks = [disk1,disk2]
cfg_base["disks"] = disks

#下面是网卡配置,由于网卡配置，需要在TECS系统中增加网络平面，所以如果想打开下面配置，需要在这之前，先配置网络平面
# 下面参数"plane":"test_plane" 表示网口所链接的网络平面，"dhcp":bool(1) 表示使用DHCP获取IP地址
#nic1 = {"pci_order":0,"plane":"test_plane","vlan":0,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#nic2 = {"pci_order":0,"plane":"test_plane","vlan":1,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#cfg_base["nics"] = [nic1,nic2]
    
cfg = {}
cfg["base_info"] = cfg_base
# 虚拟机模板名称
vt_demo_name = "vt_demo_name"
cfg["vt_name"] = vt_demo_name
# 虚拟机模板是否发布
cfg["is_public"] = bool(1)

# 创建模板的名字执行
vt = vmtemplate(server,root_session)
if vt.allocate(cfg) != True:
    ci_exit_ex(-1,line())
   
# ------------------创建虚拟机------------------
#虚拟机基本配置
cfg_base = {}
# 虚拟机CPU数量
cfg_base["vcpu"] = 1
# 虚拟机CPU能力
cfg_base["tcu"] = 1
# 虚拟机的内存大小，单位字节
cfg_base["memory"] = long(128 * 1024 * 1024)
# 下面两个字节，不需要配置
cfg_base["cluster"] = ""
cfg_base["host"] = ""
# 看门狗时长，如果不为0，则表示启动虚拟机的看门狗功能
cfg_base["vm_wdtime"] = 0
# 映像的配置，一般不用修改，使用的该DEMO上传的image.
cfg_base["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
# 下面两项不用修改
cfg_base["kernel"] = {"id":-1L,"size":0L}
cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
# 该项表示是全虚拟机化还是半虚拟化，默认不需要修改
cfg_base["virt_type"] = "hvm"
# 下面3项是高级配置，基本不用修改
cfg_base["root_device"] = ""
cfg_base["kernel_command"] = ""
cfg_base["rawdata"] = ""
# 下面是上下文配置，即用户在虚拟机外面给虚拟机里面传递数据的配置，一般以CD的方式传到虚拟机中
# 第一个参数是文件名称，第二个参数是文件的内容
cfg_base["context"] = {"file_name":"file_value", "file_name1":"file_value1"}

# 下面是虚拟机互斥关机的配置，一般不配置，如果要配置，格式如下[{vm1_id,vm2_id},{vm3_id,vm4_id}]
cfg_base["qualifications"] = []

#下面是磁盘配置
#增加一个磁盘,target表示盘符，postion字段不要动，采用默认就可，fstype表示磁盘是否需要格式化""表示不格式话，下面格式是表示格式为ext3,
#具体参数，请参考TECS发布的RPC接口文档
disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":0,"fstype":"ext3"}
disk2 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdd","position":0,"fstype":"ext3"}
disks = [disk1,disk2]
cfg_base["disks"] = disks

#下面是网卡配置,由于网卡配置，需要在TECS系统中增加网络平面，所以如果想打开下面配置，需要在这之前，先配置网络平面
# 下面参数"plane":"test_plane" 表示网口所链接的网络平面，"dhcp":bool(1) 表示使用DHCP获取IP地址
#nic1 = {"pci_order":0,"plane":"test_plane","vlan":0,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#nic2 = {"pci_order":0,"plane":"test_plane","vlan":1,"dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}
#cfg_base["nics"] = [nic1,nic2]
    
cfg = {}
cfg["base_info"] = cfg_base
# 下面是配置虚拟机的名称
vm_demo1 = "vm_demo1"
cfg["vm_name"] = vm_demo1
# 下面是配置虚拟机所在的工程
cfg["project_name"] = demo_project_name   
# 下面是配置虚拟机的数量
cfg["vm_num"] = 1

#获取管理员操作权限
v = vmcfg(server,root_session)
if v.allocate(cfg) != True:
    ci_exit_ex(-1,line())
#创建完查询vm_id
vm_id = v.get_vm_id(vm_demo1)
if vm_id is None:
    ci_exit_ex(-1,line(),"failed to get vm id!")
    
# 部署虚拟机
if v.action(vm_id,"deploy") != True:
    ci_exit_ex(-1,line())





