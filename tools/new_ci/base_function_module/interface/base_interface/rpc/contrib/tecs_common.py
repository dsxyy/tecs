#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：tecs_common.py
# 测试内容：
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/11/27
#*******************************************************************************/
import sys,os,time

def line():
    try:
            raise Exception
    except:
            return sys.exc_info()[2].tb_frame.f_back.f_lineno 

def ci_exit_ex(value=0,line = line(),error="null",):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, line = %d, error string = %s" % (value,line,error)
        sys.exit(value)         

            
def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)            

# 带打点的超时函数
def time_delay(value, des):
    print des
    print "will wait %d s" %value
    i = 0
    while i <= long(value):
        if i % 5 == 0:
            print "%d" % i,
        else:
            print ".",
        sys.stdout.flush()
        time.sleep(1)
        i += 1
        

        