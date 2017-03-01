#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：portabledisk.py
# 测试内容：portabledisk模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：
# 完成日期：2013/05/10
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：
#    修改内容：创建
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
PD_CREATE = 0
PD_ATTACH = 1
PD_DEATTACH = 2
PD_DELETE= 3
SUCCESS=0
ERROR=1
ERROR_OP_RUNNING=33
ATTACH_DISK=24
DEATTACH_DISK=25
ATTACHED =0
ATTACHING=1
AUTHED  =2
DETACHING=3
UNLOAD  =4
#****************************************************************************#
class portabledisk:
    ''' tecs portabledisk methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session

    def create(self,volsize,cluster_name,description):
        result = self.apisvr.tecs.portabledisk.create(self.session,volsize,cluster_name,description) 
        if result[0] != 25 and result[0] != 0:
            print "failed to create portabledisk! error code = %d" % result[0]
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create portabledisk, request_id %s!" % result[1]
            return result[1]

    def delete(self,request_id):     
        result = self.apisvr.tecs.portabledisk.delete(self.session,request_id) 
        if result[0] != 25 and result[0] != 0:
            print "failed to delete request_id %s! error code = %d" % (request_id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete portabledisk request_id %s!" % request_id
            return True

    def attach(self,request_id,vid,target,bus):     
        result = self.apisvr.tecs.vmcfg.portable_disk.attach(self.session,request_id,vid,target,bus) 
        if result[0] != 25 and result[0] != 0:
            print "failed to attach request_id %s! to vid %ld target %s error code = %d" % (request_id,vid,target,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to attach portabledisk request_id %s from vid %ld!" % (request_id,vid)
            return True

    def detach(self,vid,request_id):     
        result = self.apisvr.tecs.vmcfg.portable_disk.detach(self.session,vid,request_id) 
        if result[0] != 25 and result[0] != 0:
            print "failed to detach request_id %s! from vid %ld error code = %d" % (request_id,vid,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to detach portabledisk request_id %s from vid %ld!" % (request_id,vid)
            return True

    '''def query(self,start_index,query_count,type,user_name,id,filename):     
        result = self.apisvr.tecs.image.query(self.session,start_index,query_count,type,user_name,id,filename) 
        if result[0] != 0:
            print "failed to query image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query image %d!" % id

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        images  = result[3]
        for img in images:
            for n,v in img.items():
                print '%s = %s' % (n,v)
        return True

    def get_id_by_filename(self,filename):
        result = self.apisvr.tecs.image.query(self.session,START_INDEX,QUERY_COUNT,QUERY_BY_IMAGE_NAME,NO_USER_NAME,NO_IMAGE_ID,filename) 
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

    def query_value(self,type,user_name,id,name):     
        result = self.apisvr.tecs.image.query(self.session,START_INDEX,QUERY_COUNT,type,user_name,id,NO_IMAGE_NAME) 
        if result[0] != 0:
            print "failed to query image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to query image %d!" % id

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        images  = result[3]
        for img in images:
            for n,v in img.items():                
                print '%s = %s' % (n,v)
        for img in images:
            return img[name]
        return None  

    def query_name_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"name")
   
                    
    def get_file_size(self,fileid):
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
                    '''
