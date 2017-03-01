#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：affinity_region_advance_package.py
# 测试内容：
# 当前版本：1.0 
# 作    者：
# 完成日期：2013/08/30

#*******************************************************************************/
import sys,os,getopt,time,inspect
import hashlib
from ftplib import FTP


from base_interface.rpc.affinity_region import *


class ar_module:
    def __init__(self, host, session):       
        self.method = affinity_region(host, session)
        self.cfg = {}

# 基于当前类创建亲和域  
    def create(self,name,description,level,is_forced):
        return self.method.allocate(name,description,level,is_forced)
        
# 修改亲和域  
    def modify(self,arid,name,description,level,is_forced):
        return self.method.set(arid,name,description,level,is_forced)

# 查询
    def query(self,flag,arid,user_name):
        return self.method.query(flag,arid,user_name)
        
# 删除        
    def delete(self,arid):  
        return self.method.delete(arid)