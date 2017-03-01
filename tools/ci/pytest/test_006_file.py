#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_002.py
# 测试内容：
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
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

#用于测试的tc xmlrpc服务器地址
server_addr = "localhost"
#用于测试的tc xmlrpc服务器端口号
server_port = "8080"
# tc/cc/hc的名字需要和start.sh中的定义保持一致！
#用于测试的tc application，即云名称
test_cloud = "tecscloud"
#用于测试的cc application，即集群名称
test_cluster = "tecscluster"
#用于测试的hc application，即物理机名称
test_host = "citesthost"
#管理员账号和密码
test_root_user = "admin"
test_root_passwd = "admin"
#普通用户组名称
test_tenant_usergroup = "cigroup"
#租户账号和密码
test_tenant_user = "citest"
test_tenant_passwd = "citest"
#用于上传，创建虚拟机的映像文件路径
test_image = "ttylinux.img"
#待测试的虚拟机名称
test_vm_name = "ttylinux"
#待测试的工程名称
test_project_name = "test_prj_1"
#test管理组名称
test_admin_usergroup = "testadmingroup"
#test管理组帐号和密码
test_admin_user      = "testadmin"
test_admin_passwd    = "testadmin"
server_bath_path ="/mnt/tecs_tc/"

def usage():
    ''' 打印帮助信息'''
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
        
# 自动化集成测试开始!!!
print "===================TECS CI TEST START==================="        
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#列出当前支持的所有xmlrpc接口    
server.system.listMethods()

# ------------------环境准备------------------
# 登录tecs，获取管理员账户加密后的session
encrypted_root_session = login(server,test_root_user,test_root_passwd)
if encrypted_root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")

# 创建一个管理组及管理员帐号
ug  = usergroup(server,encrypted_root_session)
if ug.allocate(test_admin_usergroup,1) is not True:
    ci_exit(-1)
u   = user(server,encrypted_root_session)
if  u.allocate(test_admin_user,test_admin_passwd,test_admin_passwd,1,test_admin_usergroup) != True:
    ci_exit(-1)

# 创建一个用户组及租户帐号
ug3  = usergroup(server,encrypted_root_session)
if ug3.allocate(test_tenant_usergroup,3) is not True:
    ci_exit_ex(-1,line())
u3   = user(server,encrypted_root_session)
if u3.allocate(test_tenant_user,test_tenant_passwd,test_tenant_passwd,3,test_tenant_usergroup) != True:
    ci_exit_ex(-1,line())

# 登录租户  获取租户session 以备后面使用
user_session = login(server,test_tenant_user,test_tenant_passwd)
if user_session is None:
    ci_exit_ex(-1,"failed to get user session!")


# ------------------管理员权限测试------------------
# 以管理员身份上传一个映像文件  应该成功 
if upload_image_file(server_addr,test_root_user,test_root_passwd,test_image) !=True:
    ci_exit_ex(-1,line())
# 以租户身份上传一个映像文件  应该成功

# 获取该文件的file1id  file模块.get_file_id    参数 os.path.basename(test_image)
time.sleep(1)
f = tecs_file(server,encrypted_root_session)
fileid = f.get_file_id(os.path.basename(test_image))
if fileid is None:
    ci_exit_ex(-1,line())
# 再上传一次 同名的映像文件 ，应该成功
if upload_image_file(server_addr,test_root_user,test_root_passwd,test_image) !=True:
    ci_exit_ex(-1,line())
# 重新获取文件id ，应该成功
time.sleep(1)  
fileid = f.get_file_id(os.path.basename(test_image))
if fileid is None:
    ci_exit_ex(-1,line())

# 查询当前文件所有者  应该为test_root_user
fileowner=f.get_file_owner(fileid)
if fileowner is None:
    ci_exit_ex(-1,line())
elif fileowner != test_root_user:
    ci_exit_ex(-1,line())
    
# 查询当前文件的锁定状态 应该为未锁定状态
filelock=f.get_file_lock(fileid)
if filelock is None:
    ci_exit_ex(-1,line())
elif filelock != 0:
    ci_exit_ex(-1,line())

# 查询当前文件的锁定状态  应该为锁定状态
filelock=f.get_file_lock(fileid)
if filelock is None:
    ci_exit_ex(-1,line())
elif filelock != 1:
    ci_exit_ex(-1,line())
# 删除当前文件    应该失败

# 获取当前文件的大小 应该不为0
filesize=f.get_file_size(fileid)
if filesize is None:
    ci_exit_ex(-1,line())
elif filesize == 0:
    ci_exit_ex(-1,line())
# 获取image的文件大小 应该和 文件大小一致


# ------------------普通用户权限测试------------------
print "**************test user**************"
# 查询管理员上传的 fileid    应该失败
f3 = tecs_file(server,user_session)
fileid3 = f3.get_file_id(os.path.basename(test_image))
if fileid3 is not None:
    ci_exit_ex(-1,line())

# 普通租户上传一个映像文件，使用的文件名和管理员一样，  应该成功
upload_image_file(server_addr,test_tenant_user,test_tenant_passwd,test_image)

# 查询拥有文件的记录数       应该为1条
max_count3=f3.get_file_max_count(-4L)
if max_count3 is None:
    ci_exit_ex(-1,line())
elif max_count3 !=1:
    ci_exit_ex(-1,line())

# 注册为image   应该成功
time.sleep(1)
fileid3 = f3.get_file_id(os.path.basename(test_image))
if fileid3 is None:
    ci_exit_ex(-1,line())
i3 = image(server,user_session)
if i3.register(fileid3, "ttylinux", "machine", "x86_64", bool(0), bool(1), "reg image for  test file mod") == False:
    ci_exit_ex(-1,line())
    
# ------------------管理员权限测试------------------
# 查询拥有的映像文件数   应该只有一条
max_count=f.get_file_max_count(-4L)
if max_count is None:
    ci_exit_ex(-1,line())
elif max_count !=1:
    ci_exit_ex(-1,line())

# 注销 映像文件
if i.unregister(fileid) != True:
    ci_exit_ex(-1)

# 查询当前文件的锁定状态  应该为非锁定状态
filelock=f.get_file_lock(fileid)
if filelock is None:
    ci_exit_ex(-1,line())
elif filelock != 0:
    ci_exit_ex(-1,line())

# 删除管理员上传的文件    应该成功
if delete_image_file(server_addr,test_root_user,test_root_passwd,server_bath_path+"1/"+os.path.basename(test_image),1) == False:
    ci_exit_ex(-1)
# 删除租户上传的文件      应该失败
if delete_image_file(server_addr,test_root_user,test_root_passwd,server_bath_path+"3/"+os.path.basename(test_image),1) != False:
    ci_exit_ex(-1)
    
# ------------------普通用户权限测试------------------

# 注销租户创建映像文件  应该成功
if i3.unregister(fileid3) == False:
    ci_exit_ex(-1,line())
    
# 删除租户上传的文件      应该失败?
if delete_image_file(server_addr,test_tenant_user,test_tenant_passwd,server_bath_path+"3/"+os.path.basename(test_image),1) == False:
    ci_exit_ex(-1)

















