#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：st_auth_manager00.py
# 内容摘要：用户管理的公共基本流程
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

#用户登录tecs            
encrypted_account_session, server = login_user(server_addr,account_user)

#登录用户管理模块
u = user(server,encrypted_account_session)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_USER-01001 创建新云管理员帐户
#用例开始
result = True
users = open('/home/tecs/st_data/newadmin.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #创建新云管理员帐户
    if u.allocate(name,name,name,1,"Default") != True:
        #创建帐户失败，则用例执行失败，调用st_log(-1)记录执行结果
        st_log(-1,lines(),"STC-F-TECS_USER-01001: failed to create new administrator!")
        result = False
        break
    else:
        if u.query(0L,100L,name) != True:
            #查询新帐户失败，则用例执行失败，调用st_log(-1)记录执行结果
            st_log(-1,lines(),"STC-F-TECS_USER-01001: failed to query new administrator!")
            result = False
            break
#正确创建帐号和可以查询到，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01001: success to create new administrators!")
#用例结束。


#STC-F-TECS_USER-01002 创建新租户帐户
#用例开始
result = True
users = open('/home/tecs/st_data/newtenant.txt','r')
new_account = [line.strip() for line in users.readlines()]
users.close()
for name in new_account:
    print name
    #创建新普通用户帐户
    if u.allocate(name,name,name,3,"st_group") != True:
        #创建帐户失败，则用例执行失败，调用st_log(-1)记录执行结果
        st_log(-1,lines(),"STC-F-TECS_USER-01002: failed to create new guest!")
        result = False
        break
    else:
        if u.query(0L,100L,name) != True:
            #查询新帐户失败，则用例执行失败，调用st_log(-1)记录执行结果
            st_log(-1,lines(),"STC-F-TECS_USER-01002: failed to query new guest!")
            result = False
            break
#正确创建帐号和可以查询到，则用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_USER-01001: success to create new guests!")
#用例结束。


st_log(2,'',"END")
