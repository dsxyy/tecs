#!/usr/bin/python
# -*- coding: gb2312 -*-
# �ļ����ƣ�st_ltest_002.py
# ����ժҪ����������ִ�еĲ��Խű�
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

server_addr = "10.44.129.34"
guest_user = "admin"
__builtin__.LOG = server_addr.replace('.','_')

list = [2L, 27L, 29L, 28L, 31L, 30L]
print max(list)
