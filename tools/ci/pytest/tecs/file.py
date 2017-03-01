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

class tecs_file:
    ''' tecs image methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session
   
    def get_file_id(self,filename):
        '''����ָ�����ļ�����ѯ�ļ�id'''
        result = self.apisvr.tecs.image.query(self.session,0L,50L,3,"",0L,filename) 
        if result[0] != 0:
            print "failed to query image information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            if len(result[3]) == 0:
                print "No image files"
                return None
                
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            image_list = result[3]
            print "---------file list---------" 
            for k in image_list:
                print "image %s" % k
            for i in image_list:
                return long(i["id"])
            return None
   
    def get_file_state(self,fileid):
        '''����ָ����id��ȡ��ǰ�ļ����������'''
        result = self.apisvr.tecs.image.query(self.session,0L,50L,1,"",fileid,"") 
        if result[0] != 0:
            print "failed to query file information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            if len(result[3]) == 0:
                print "No image files"
                return None
                
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            file_list = result[3]
            print "---------file list---------" 
            for f in file_list:
                print "file %s" % f["id"]
                for k in f.keys():
                    print "  %s = %s" % (k,f[k])
                if f["id"] == fileid:
                    return int(f["state"])
     
    def get_file_owner(self,fileid):
        '''����ָ����id��ȡ��ǰ�ļ���������'''
        result = self.apisvr.tecs.image.query(self.session,0L,50L,1,"",fileid,"") 
        if result[0] != 0:
            print "failed to query file information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            if len(result[3]) == 0:
                print "No image files"
                return None
                
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            file_list = result[3]
            print "---------file list---------" 
            for f in file_list:
                print "file %s" % f["id"]
                for k in f.keys():
                    print "  %s = %s" % (k,f[k])
                if f["id"] == fileid:
                    return f["user_name"]
                    
    def get_file_size(self,fileid):
        '''����ָ����id��ȡ��ǰ�ļ��Ĵ�С'''
        result = self.apisvr.tecs.image.query(self.session,0L,50L,1,"",fileid,"") 
        if result[0] != 0:
            print "failed to query file information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            if len(result[3]) == 0:
                print "No image files"
                return None
                
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            file_list = result[3]
            print "---------file list---------" 
            for f in file_list:
                print "file %s" % f["id"]
                for k in f.keys():
                    print "  %s = %s" % (k,f[k])
                if f["id"] == fileid:
                    return long(f["size"])
                    
    def get_file_max_count(self,fileid):
        '''����ָ����id��ȡ��ǰ�ļ��Ĵ�С'''
        result = self.apisvr.tecs.image.query(self.session,0L,50L,1,"",fileid,"") 
        if result[0] != 0:
            print "failed to query file information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            if len(result[3]) == 0:
                print "No image files"
                return None
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]    
            return long(result[2])                