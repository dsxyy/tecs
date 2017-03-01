#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：image_advance_package.py
# 测试内容：
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/11/27
# 说明    ：映像模块的封装
#*******************************************************************************/
import sys,os,getopt,time,inspect

from base_interface.rpc.file import *
from base_interface.rpc.image import *


class image_module:
    def __init__(self, host, session): 
        self.host_addr = host   
        self.session = session
        self.image_id = -1L
        self.img = image(host, session)

# 无返回值        
    def check_and_upload_img(self, img_server, user_name, pwd, img_url):
        i = image(self.host_addr,self.session)
        image_id = i.get_id_by_filename(os.path.basename(img_url))
        if image_id == None:
            if upload_image_file(img_server,user_name,pwd,img_url) == False:
                ci_exit(-1,"check_and_upload_img fail")
            image_id = i.get_id_by_filename(os.path.basename(img_url))
        self.image_id = image_id
        self.image_rpc_server = i

# 函数返回的是img_id        
    def get_img_id(self):
        return self.image_id

# 下面几个函数返回的是bool        
    def publish(self):
        return self.image_rpc_server.publish(self.image_id,IS_PUBLIC)
        
    def no_publish(self):
        return self.image_rpc_server.publish(self.image_id,NOT_PUBLIC)
        
    def enable(self):
        return self.image_rpc_server.enable(self.image_id,I_ENABLE)      
        
    def disable(self):
        return self.image_rpc_server.enable(self.image_id,I_DISABLE)  
        
    def delete_img(self):
        return self.image_rpc_server.delete(self.image_id)        
        
    def get_imgid_by_name(self,filename):
        return self.img.get_file_id(filename)   
        
    def query_disk_format_by_id(self,id):
        return self.img.query_disk_format_by_id(id) 

    def query_disk_size_by_id(self,id):
        return self.img.query_disk_size_by_id(id) 
         
          