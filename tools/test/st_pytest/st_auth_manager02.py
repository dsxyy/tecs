#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：st_auth_manager02.py
# 内容摘要：用户管理的异常流程
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



st_log(2,'',"END")
