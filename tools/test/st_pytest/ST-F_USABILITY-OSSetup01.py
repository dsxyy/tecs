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


#######ST-F_USABILITY-OSSetup02 创建，修改，删除和查询帐户测试##########
st_log(2,'',"START")

#登录用户管理模块
u = user(server,encrypted_root_session)
ug  = usergroup(server,encrypted_root_session)

#用例开始
result = True
#创建用户组usergroup0和usergroup1, 应该成功
for i in range(2):
    mygroup = "usergroup"+str(i)
    if ug.allocate(mygroup,1) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: mygroup can not create!")
        result = False

#并设置用户组usergroup0为禁用，应该成功
if ug.set("usergroup0",2,"disable usergroup0")  != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: set usergroup0 is failed !")
    result = False

#创建用户，应该成功
if  u.allocate("user0","user0","user0",1,"usergroup0") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: create uaer0 is failed!")
    result = False

#使用 user0 用户登录    应该失败
encrypted_user0_session, server_user0= login_user(server_addr,"user0")
nu = user(server_user0,encrypted_user0_session)
if nu.query(0L,0L,"user0") != False:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 should login !")
    result = False
    
#设置usergroup0为启用，应该成功。
if  ug.set("usergroup0",1,"enable usergroup0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: usergroup0 shouldn't enable !")
    result = False

#再使用user0用户登录，应该成功
if nu.query(0L,0L,"user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 shouldn't login !")
    result = False
    
#删除usergroup0 用户组    应该失败
if ug.delete("usergroup0") != False:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: success to delete usergroup0!")
    result = False
    
#删除user0用户后，再删除usergroup0 用户组    应该成功
if  u.delete("user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete user0!")
    result = False  
if ug.delete("usergroup0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete usergroup0!")
    result = False

#创建用户组usergroup0, 再创建用户user0，并且设置密码不一致测试
ug.allocate("usergroup0",1)
if u.allocate("user0","user0","user",1,"usergroup0")  == True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: success to create uaer0 when pwd is error!")
    result = False

#创建用户user0，并且设置密码一致测试，应该成功
if u.allocate("user0","user0","user0",1,"usergroup0")  != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to create uaer0 when pwd is right!")
    result = False

#云管理员创建用户user0-user2，共3个用户角色分别为云管理员，账户管理员和租户，并且已创建的用户不能再被创建。
for i in range(3):
    myuser = "user"+str(i)
    print myuser
    if u.allocate(myuser,myuser,myuser,i+1,"usergroup0")  != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: myuser can not create!")
        result = False
    if u.allocate(myuser,myuser,myuser,i+1,"usergroup0")  != False:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: myuser can create again!")
        result = False

# 云管理员来修改租户信息
for i in range(3):
    myuser = "user"+str(i)
    mypwd = "user"+str(i+1)
    myemail = "email"+str(i)
    myaddr = "addr"+str(i)
    print myuser
    if u.set(myuser,mypwd,mypwd,2-i,"usergroup1",1,"13601455012",myemail,myaddr) != True:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: myuser cann't modify by admin!")
        result = False

#云管理员查询用户的电话信息
for i in range(3):
    myuser = "user"+str(i)
    print myuser
    if  u.query_phone(0L,50L,myuser) != "13601455012":
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user of phone!")
        result = False

#云管理员查询用户的邮箱地址
for i in range(3):
    myuser = "user"+str(i)
    myemail = "email"+str(i)
    print myuser
    if  u.query_email(0L,50L,myuser) != myemail:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user of email!")
        result = False

#云管理员查询用户的地址信息
for i in range(3):
    myuser = "user"+str(i)
    myaddr = "addr"+str(i)
    print myuser
    if  u.query_location(0L,50L,myuser) != myaddr:
        st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user of location!")
        result = False

#租户 user0 用户登录    应该成功
encrypted_user_pwd_session, server_user_pwd= login_user_with_pwd(server_addr,"user0","user1")
nu_p = user(server_user_pwd,encrypted_user_pwd_session)
if nu_p.query(0L,0L,"user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 should login !")
    result = False

#租户user0用户登录后，查询自身用户信息
if  nu_p.query_phone(0L,0L,"user0") != "13601455012" or \
nu_p.query_email(0L,50L,"user0") != "email0" or \
nu_p.query_location(0L,50L,"user0") != "addr0":
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user information!")
    result = False

#租户user0用户修改自身信息
if nu_p.set("user0","user","user",3,"usergroup0",1,"13601455012","email","addr") != True:
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: my information cann't modify by user0!")
    result = False

#使用修改后的用户密码登录系统，并查询用户信息是否被正确修改。
encrypted_user0_session, server_user0= login_user_with_pwd(server_addr,"user0","user")
nu0 = user(server_user0,encrypted_user0_session)
if nu0.query(0L,0L,"user0") != True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: user0 shouldn't login !")
    result = False
if  nu0.query_phone(0L,0L,"user0") != "13601455012" or \
nu0.query_email(0L,50L,"user0") != "email" or \
nu0.query_location(0L,50L,"user0") != "addr":
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: failed to user information!")
    result = False

#修改自己为云管理员或修改自己为禁用状态，应该失败
if nu0.set("user0","user","user",1,"usergroup0",1,"13601455012","mail","addr") == True \
or nu0.set("user0","user","user",3,"usergroup0",2,"13601455012","mail","addr") == True :
    st_log(-1,lines(),"ST-F_USABILITY-OSSetup01: success to modify user information ?")
    result = False

#删除用户
for i in range(3):
    myuser = "user"+str(i)
    if u.delete(myuser) !=True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete user!")
        result = False
    
#删除用户组
for i in range(2):
    mygroup = "usergroup"+str(i)
    if ug.delete(mygroup) !=True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to delete usergroup!")
        result = False

#查询用户组是否存在
if  ug.query(0L,200L,"") != True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to query usergroup!")
        result = False

#查询用户是否存在
if  u.query(0L,200L,"") != True:
        st_log(1,lines(),"ST-F_USABILITY-OSSetup01: failed to query usergroup!")
        result = False


if result == True:
    st_log(1,lines(),"ST-F_USABILITY-OSSetup01: create/delete/query/modify is success!")
#用例结束。

st_log(2,'',"END")
