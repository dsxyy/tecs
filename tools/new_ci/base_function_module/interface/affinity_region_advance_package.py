#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�affinity_region_advance_package.py
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ�
# ������ڣ�2013/08/30

#*******************************************************************************/
import sys,os,getopt,time,inspect
import hashlib
from ftplib import FTP


from base_interface.rpc.affinity_region import *


class ar_module:
    def __init__(self, host, session):       
        self.method = affinity_region(host, session)
        self.cfg = {}

# ���ڵ�ǰ�ഴ���׺���  
    def create(self,name,description,level,is_forced):
        return self.method.allocate(name,description,level,is_forced)
        
# �޸��׺���  
    def modify(self,arid,name,description,level,is_forced):
        return self.method.set(arid,name,description,level,is_forced)

# ��ѯ
    def query(self,flag,arid,user_name):
        return self.method.query(flag,arid,user_name)
        
# ɾ��        
    def delete(self,arid):  
        return self.method.delete(arid)