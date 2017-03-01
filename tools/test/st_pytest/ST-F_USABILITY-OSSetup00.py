#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：ST-F_VM-Operate00.py
# 内容摘要：用户管理测试。
# 作    者：xujiakui
# 完成日期：2012/5/21
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *
from contrib import xmlrpclibex
from tecs.user import *

#tecs目标测试机地址和用户            
server_addr = "10.44.127.9"
root_user = "admin"
#1号虚拟机
vm_id0 = long(1)

# 参数解析
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-u", "--user"):
        root_user = v
__builtin__.LOG = server_addr.replace('.','_')

#登录映像管理模块
encrypted_root_session, server = login_user(server_addr,root_user)           #//蓝色字体部分为范本，每个.py文件中基本都是这样的！直接从范例中拷贝！

##############################################################
##############################################################

st_log(2,'',"START")

#ST-F_USABILITY-OSSetup01 创建，修改，删除和查询帐户组测试
#用例开始
result = True

#登录用户管理模块
u = user(server,encrypted_root_session)
ug  = usergroup(server,encrypted_root_session)

#创建一个用户组使用参数为 st_group1,1 ,应该成功
if ug.allocate("st_group1",1) != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: failed to create st_group1!")
    result = False
    
#创建一个用户组使用参数为 st_group1,2  应该失败
if ug.allocate("st_group1",2) == True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: success to create st_group1!")
    result = False

#查看st_group1 是否启用   应该为启用
if ug.get_use_flag("st_group1") != True:
   st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group1 can not enable!")
   result = False

#设置st_group1 为禁用
if ug.set("st_group1",2,"disable st_group1") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00:  st_group1 can not disable!")
    result = False
    
#查看st_group1 是否启用   应该为禁用
if ug.get_use_flag("st_group1") != 2:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00:  st_group1 can not disable!")
    result = False
    
#删除st_group1 用户组    应该成功
if ug.delete("st_group1") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group1 can not delete!")
    result = False
    
#创建一个用户组使用参数为 st_group2,1  应该成功
if ug.query(0L,200L,"st_group2") == True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2  is created!")
else:
    ug.allocate("st_group2",1) != True
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2  can not create!")
    result = False
    
#设置st_group2 为禁用
if ug.set("st_group2",2,"disable st_group2") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2 can not disable!")
    result = False
    
#删除st_group2 用户组    应该成功    
if ug.delete("st_group2") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: st_group2 can not delete!")
    result = False
    
   
#创建usergroup0-usergroup200的用户组，应该成功
i=1
numble =200
for i in range(numble):
    mygroup = "usergroup"+str(i)
    print mygroup
    if ug.allocate(mygroup,1) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: mygroup can not create!")
        result = False

# 查询200个用户组，应该成功
if ug.query(0L,200L,"")  != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: XXXXXXX !")
    result = False
    
#删除usergroup0-usergroup200的用户组
 
for i in range(numble):
    mygroup = "usergroup"+str(i)
    print mygroup
    if ug.delete(mygroup) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup00: mygroup can not delete!")
        result = False
    
if result == True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup00: success to create usergroup!")
#用例结束


