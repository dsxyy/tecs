#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：image.py
# 测试内容：image模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
#*******************************************************************************/
#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
QUERY_USER_CREAT_IMAGE = -4
QUERY_USER_CREAT_VISBLE_IMAGE = -3
QUERY_USER_VISBLE_PUBLIC_IMAGE = -2
QUERY_BY_USER = 0
QUERY_BY_IMAGE_ID = 1
QUERY_ALL_IMAGE = 2
QUERY_BY_IMAGE_NAME = 3
NO_USER_NAME = ""
NO_IMAGE_NAME = "" 
NO_IMAGE_TYPE = ""
NO_IMAGE_ARCH = ""
NO_IMAGE_DESCRIPTION  = ""
NO_IMAGE_ID = 0L
I_ENABLE = bool(1)
I_DISABLE = bool(0)
IS_PUBLIC = bool(1)
NOT_PUBLIC = bool(0)
#****************************************************************************#

class image:
    ''' tecs image methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session
        
    def get_id_by_filename(self,filename):
        '''根据指定的文件名查询映像注册的id'''
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

    def deploy(self,id,name):     
        result = self.apisvr.tecs.image.deploy(self.session,id,name) 
        if result[0] != 0:
            print "failed to deploy image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to deploy image %d!" % (id)
            
        workflow_id = result[1]
        target = result[2]

        print 'deploy target %s  id %s ' % (target,workflow_id)
        return target,workflow_id

    def cancel(self,id,name):     
        result = self.apisvr.tecs.image.cancel(self.session,id,name) 
        if result[0] != 0:
            print "failed to publish image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to cancel image %s!" % (id)
            
        workflow_id = result[1]
        target = result[2]

        print 'cancel target %s  id %s ' % (target,workflow_id)
        return target,workflow_id
        
    def enable(self,id,flag):
        result = self.apisvr.tecs.image.enable(self.session,id,flag) 
        if result[0] != 0:
            print "failed to enable image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to enable image %d!" % id
            return True

    def publish(self,id,flag):     
        result = self.apisvr.tecs.image.publish(self.session,id,flag) 
        if result[0] != 0:
            print "failed to publish image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to publish image %d!" % id
            return True

    def query(self,start_index,query_count,type,user_name,id,filename):     
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

    def delete(self,id):     
        result = self.apisvr.tecs.image.delete(self.session,id) 
        if result[0] != 0:
            print "failed to delete image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete image %d!" % id
            return True    

    def set(self,id,name,type,arch,bus,description,os_type):     
        reginfo = {"id":id,"name":name,"type":type,"arch":arch,"bus":bus,"description":description,"os_type":os_type}
        result = self.apisvr.tecs.image.set(self.session,id,name,type,arch,bus,description,os_type) 
        if result[0] != 0:
            print "failed to set image %d! error code = %d" % (id,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set image %d %s %s %s %s!" % (id,name,type,arch,bus)
            return True

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

    def query_type_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"type")

    def query_arch_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"arch")

    def query_bus_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"bus")
        
    def query_description_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"description")

    def query_state_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"is_enabled")   
        
    def query_register_time(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"register_time")  

    def query_pub_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"is_public") 

    def get_query_count(self,start_index,query_count,type,user_name,id):
        result = self.apisvr.tecs.image.query(self.session,start_index,query_count,type,user_name,id,NO_IMAGE_NAME) 
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
        return max_count

    def query_running_vms_by_id(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"running_vms")   
        
    def query_ref_count_by_private_vt(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"ref_count_by_private_vt")  
        
    def query_ref_count_by_public_vt(self,id):
        return self.query_value(QUERY_BY_IMAGE_ID,NO_USER_NAME,id,"ref_count_by_public_vt")  
    def query_image_ftp_svr_info(self):     
        result = self.apisvr.tecs.image.ftp_svr_info_get(self.session) 
        if result[0] != 0:
            print "failed to query image ftp svr info! error code = %d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            print "success to query image " 
            ftp_svr_info = result[1]
            for n,v in ftp_svr_info.items():                
                print '%s = %s' % (n,v)
            return 1


