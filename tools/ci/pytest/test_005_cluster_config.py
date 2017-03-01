#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_005_cluster_config.py
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
# 修改记录2：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息
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

# 解析处理命令行参数
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:p:i:v", ["help", "addr=","port=","image="])
except getopt.GetoptError:
    # print help information and exit:
    usage()
    ci_exit(-2,line(),"GetoptError")
    
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
            ci_exit(-2,line(),"image file %s not exist!" % test_image)
        
# 自动化集成测试开始!!!
print "===================TECS CI TEST START==================="        
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,line(),"failed to create xmlrpc ServerProxy!")

#列出当前支持的所有xmlrpc接口    
server.system.listMethods()

# 登录管理员帐号
encrypted_root_session = login(server,test_root_user,test_root_passwd)
if encrypted_root_session is None:
    ci_exit(-1,line(),"failed to get encrypted root session!")

#系统运行状态查询 EC:611003077308 tecs提供rpc接口查询系统运行时信息
#此处延时20s，否则hc的MU_VM_HANDLER和MU_HOST_HEARTBEAT不存在
time.sleep(100)
#必须是管理员账户
sys_runtime_query = tecssystem(server,encrypted_root_session)
#查询TC状态
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,DEFAULT_TC)) != RUNTIME_STATE_NORMAL:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,DEFAULT_TC)
    ci_exit_ex(-1,line(),"TC status is abnormal!")
#查询CC状态
if int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cluster)) != RUNTIME_STATE_NORMAL:
    sys_runtime_query.show_runtime_state(TYPE_CACULATE,test_cluster)
    ci_exit_ex(-1,line(),"CC status is abnormal!")
#查询HC状态
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
# 把当前系统中的集群注册到TC  集群名称为 test_cluster
#if cm.register(test_cluster,"reg cluster") != True:
#    ci_exit(-1,line())
# 延时20S
time.sleep(20)

#  查询集群配置中自动主机加入的状态 参数 $test_cluster, allow_host_auto_register  返回结果应该为no
#ret = cc.query_config_value(test_cluster,"allow_host_auto_register")
#if ret == None:
#    ci_exit(-1,line())
#elif ret != "no":
#    print "host auto register flag is err:%s" % ret
#    ci_exit(-1,line())
#  查询集群配置中自动主机加入的状态 参数 $test_cluster, registered_tecs_name  返回结果应该为tecscloud
#if cc.query_config_value(test_cluster,"registered_tecs_name") != test_cloud:
#    ci_exit(-1,line(),"tecs name shoud be tecscloud")
#  配置主机自动加入集群 参数为 $test_cluster, allow_host_auto_register, yes  应该为成功
#if cc.set(test_cluster,"allow_host_auto_register","yes") != True:
#    ci_exit(-1,line())
#  查询集群配置中自动主机加入的状态 参数 $test_cluster, allow_host_auto_register  返回结果应该为yes
#if cc.query_config_value(test_cluster,"allow_host_auto_register") != "yes":
#    ci_exit(-1,line(),"host auto register flag shouldn't be no")

#  配置一个非法的选项 test  参数为 $test_cluster, test, test  应该为失败
#if cc.set(test_cluster,"test","test") != False:
#    ci_exit(-1,line(),"cluster cfg shouldn't support 'test' name register")
# 延时 20S，等待HOST被自动注册
time.sleep(20)

# 调用主机查询接口，看citesthost 主机是否已经被注册   citesthost 这个主机应该已经被注册
if h.query(START_INDEX,START_INDEX,test_cluster,test_host) != True:
    ci_exit(-1,line())

# 删除集群配置  参数为 $test_cluster,test    应该失败
#f cc.delete(test_cluster,"test") != False:
#   ci_exit(-1,line())

# 删除集群配置 参数为 $test_cluster, registered_tecs_name  应该失败
if cc.delete(test_cluster,"registered_tecs_name") != False:
    ci_exit(-1,line())

# 获取tcu id 推荐使用 host模块.get_cpu_id(test_cluster,test_host) 
id = h.get_cpu_id(test_cluster,test_host)
if id == None:
    ci_exit(-1,line())

# 配置tcu 参数为 $test_cluster,空字符串,上面获得的id,4,test_description  应该成功
sc = tcu(server,encrypted_root_session)
if sc.set(test_cluster,NO_CPU_INFO,id,TCU_NUM4,"test_description") != True:
     ci_exit(-1,line())

# 查询配置是否成功， 参数 $test_cluster,空字符串,上面获得的id  返回的tcu数量应该是上面设置的数量
if sc.get_tcu_num_by_id(test_cluster,NO_CPU_INFO,id) != TCU_NUM4:
    ci_exit(-1,line())

# 查询配置的描述信息是否正确， 参数 $test_cluster,空字符串,上面获得的id  返回的描述信息和配置一致
if sc.get_description_by_id(test_cluster,NO_CPU_INFO,id) != "test_description":
    ci_exit(-1,line())

# 配置tcu 参数为 $test_cluster,空字符串,上面获得的id,8,test_description1  应该成功
if sc.set(test_cluster,NO_CPU_INFO,id,TCU_NUM8,"test_description1") != True:
     ci_exit(-1,line())

# 查询配置是否成功， 参数 $test_cluster,空字符串,上面获得的id  返回的tcu数量应该是上面设置的数量
if sc.get_tcu_num_by_id(test_cluster,NO_CPU_INFO,id) != TCU_NUM8:
    ci_exit(-1,line())

# 查询配置的描述信息是否正确， 参数 $test_cluster,空字符串,上面获得的id  返回的描述信息和配置一致
if sc.get_description_by_id(test_cluster,NO_CPU_INFO,id) != "test_description1":
    ci_exit(-1,line())

# 配置tcu 参数为 $test_cluster,空字符串,100,8,test_description1  应该失败
if sc.set(test_cluster,NO_CPU_INFO,CPU_INFO_ID,TCU_NUM8,"test_description1") != False:
     ci_exit(-1,line())

# 删除 SVPU配置 参数为 $test_cluster,空字符串,100  应该失败
if sc.delete(test_cluster,NO_CPU_INFO,CPU_INFO_ID) != False:
     ci_exit(-1,line())

# 删除 SVPU配置 参数为 $test_cluster,空字符串,上面获得的id  应该成功
if sc.delete(test_cluster,NO_CPU_INFO,id) != True:
     ci_exit(-1,line())

# 查询tcu值， 参数 test_cluster,空字符串,上面获得的id  返回的tcu数量应该为0
if sc.get_tcu_num_by_id(test_cluster,NO_CPU_INFO,id) != 0:
     ci_exit(-1,line())

# 创建一个普通用户帐号
ug  = usergroup(server,encrypted_root_session)
if ug.allocate("test_group1",GROUP_TYPE1) is not True:
    ci_exit(-1,line())
u  = user(server,encrypted_root_session)
if  u.allocate("guest","test","test",ACCOUNTUSER,"test_group1") != True:
    ci_exit(-1,line())

ns = login(server,"guest","test")
if ns is None:
    ci_exit(-1,line(),"failed to get test session!")

# 使用普通用户帐号进行查询   参数 $test_cluster,空字符串,上面获得的id  应该失败
nsc = tcu(server,ns)
if nsc.get_tcu_num_by_id(test_cluster,"",id) != None:
    ci_exit(-1,line(),"test shouldn't query successfully !")
# 使用普通用户帐号进行设置   参数 $test_cluster,空字符串,上面获得的id, 4,test_description   应该失败
if nsc.set(test_cluster,NO_CPU_INFO,id,TCU_NUM4,"test_description") != False:
    ci_exit(-1,line(),"test shouldn't set successfully!")





