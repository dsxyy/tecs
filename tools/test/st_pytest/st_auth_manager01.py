#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：st_auth_manager01.py
# 内容摘要：用户管理的其他正常流程
# 作    者：XXX
# 完成日期：2012/4/17
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecs目标测试机地址和用户            
server_addr = "localhost"
account_user = "admin"

# 参数解析
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:v", ["help", "addr=","user="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-u", "--user"):
        account_user = v
__builtin__.LOG = server_addr.replace('.','_')

##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_USER-01003 新云管理员帐户登录测试
#用例开始
result = True
users = open('/home/tecs/st_data/newadmin.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #用新帐户登录tecs
    if login_user(server_addr,name) is None:
        st_log(-1,lines(),"STC-F-TECS_USER-01003: failed to login with new administrators!")
        result = False
        break
#登录正确，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01003: success to login with new administrators!")
#用例结束


#STC-F-TECS_USER-01004 新普通帐户登录测试
#用例开始
result = True
users = open('/home/tecs/st_data/newtenant.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #用新帐户登录tecs
    if login_user(server_addr,name) is None:
        st_log(-1,lines(),"STC-F-TECS_USER-01004: failed to login with new guest!")
        result = False
        break
#登录正确，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01004: success to login with new guests!")
#用例结束


#STC-F-TECS_USER-01005 云管理员修改自身帐户其他信息
#用例开始
result= True
admin_user = "tecs"
encrypted_account_session, server = login_user(server_addr,admin_user)
u = user(server,encrypted_account_session)
#修改tecs帐户的phone、email、location的信息
#set(username,password,confirm_pass,role,groupname,enable,phone,email,location)
if u.set(admin_user,"","",1,"",1,"025-888888","tecs@zte.com.cn","the earth!") != True:
    st_log(-1,lines(),"STC-F-TECS_USER-01005: failed to modify user info!")
    result = False
#修改帐户信息成功，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01005: success to modify user info!")
#用例结束    


#STC-F-TECS_USER-01006 普通租户修改自身帐户其他信息
#用例开始
result= True
tenant_user = "st_guest"
encrypted_account_session, server = login_user(server_addr,tenant_user)
u = user(server,encrypted_account_session)
#修改st_guest帐户的phone、email、location的信息
#set(username,password,confirm_pass,role,groupname,enable,phone,email,location)
if u.set(tenant_user,"","",3,"",1,"025-666666","guest@zte.com.cn","the earth!") != True:
    st_log(-1,lines(),"STC-F-TECS_USER-01006: failed to modify user info!")
    result = False
#修改帐户信息成功，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01006: success to modify user info!")
#用例结束    


#STC-F-TECS_USER-01007 删除账户测试
#用例开始
result= True
tenant_user = ("st_v3_guest", "administrator")
encrypted_account_session, server = login_user(server_addr,account_user)
u = user(server,encrypted_account_session)
for name in tenant_user:
    if u.delete(name) != True:
        st_log(-1,lines(),"STC-F-TECS_USER-01007: failed to delete account!")
        result = False
        break
#修改帐户信息成功，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01007: success to delete account!")
#用例结束


st_log(2,'',"END")
