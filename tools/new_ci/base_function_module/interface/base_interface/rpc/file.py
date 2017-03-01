#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�file.py
# �������ݣ�fileģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
# �޸ļ�¼2��
#    �޸����ڣ�2012/04/26
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ѩ��
#    �޸����ݣ�fileģ�����
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
    ''' �ϴ�ָ��Ŀ¼�ľ����ļ���Ŀ��Ŀ¼�ɷ�����ָ�� '''
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
    ''' ɾ���������ľ����ļ�����Ϊ��Ŀ¼�Ͱ��ļ�ɾ��ģʽ '''
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
        #ģʽ0δ���Թ�
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

          