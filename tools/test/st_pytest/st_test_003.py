#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：st_test_003.py
# 内容摘要：测试临时调试库脚本

import sys,os,getopt
sys.path.append('/home/ci_server/Tecs_stinit00/ci/pytest')
from contrib import xmlrpclibex
from st_comm_lib1 import *
from tecs.project import *

server_addr = "10.44.129.115"
guest_user = "admin"            
 
#encrypted_account_session, server = login_user(server_addr,guest_user)
#p = project(server,encrypted_account_session)
#if p.query(0L,100L,-5L,'Default',guest_user) != True:
#   print "can not find project"

st_log(0,lines(),'error')
st_log(1,lines(),'success')
st_log(-1,lines(),'fail')