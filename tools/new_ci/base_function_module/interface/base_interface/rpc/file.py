#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：file.py
# 测试内容：file模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
# 修改记录2：
#    修改日期：2012/04/26
#    版 本 号：V1.0
#    修 改 人：刘雪峰
#    修改内容：file模块调试
#*******************************************************************************/
import sys,os,getopt
from ftplib import FTP

def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)
      
def upload_image_file(server,user_name,password,image):
    ''' 上传指定目录的镜像文件，目标目录由服务器指定 '''
    ftp = FTP()
    print "ftp login server %s with username=%s,password=%s ..." % (server,user_name,password)
    try:
        ftp.connect(server,21)
        ftp.login(user_name,password)
    except:
        print "ftp login failed!"
        print "Unexpected error:", sys.exc_info()[0]
        return False        
        
    print ftp.getwelcome()
    print image
    try:
        cmd='%s %s' % ("stor",os.path.basename(image))
        print cmd
        ftp.storbinary(cmd,open(image,'rb'),1024)
    except Exception, e:
       print "ftp upload failed!,%s" %e
       return False
    print "ftp upload successfully"
    return True

def delete_image_file(server,user_name,password,serverimage,mode):
    ''' 删除服务器的镜像文件，分为按目录和按文件删除模式 '''
    ftp = FTP()
    print "ftp login server %s with username=%s,password=%s ..." % (server,user_name,password)
    try:
        ftp.connect(server,21)
        ftp.login(user_name,password)
    except:
        print "ftp login failed!"
        print "Unexpected error:", sys.exc_info()[0]
        return False        
    print ftp.getwelcome()
    
    print serverimage
    try:
        cmd='%s %s in mode %d' % ("delete",serverimage,mode)
        print cmd
        try:
            print "current dir %s" %ftp.cwd(os.path.dirname(serverimage))
        except Exception, e:
            print"ftp error, %s" %e
            print "Unexpected error:", sys.exc_info()[0]
            return False
        if mode == 0:
        #模式0未测试过
            try:
                delete_files = ftp.nlst()
            except Exception, e:
                print"ftp nlst error, %s" %e
                return False
            try:
                for file_name in delete_files:
                    ftp.delete(file_name)
            except Exception, e:
                print"ftp delete error, %s" %e
                print "Unexpected error:", sys.exc_info()[0]
                return False
        else:
            try:
                ftp.delete(os.path.basename(serverimage))
            except Exception, e:
                print"ftp delete error, %s" %e
                print "Unexpected error:", sys.exc_info()[0]
                return False
            
    except:
       print "ftp delete failed!"
       return False
    print "ftp delete successfully"
    return True

          