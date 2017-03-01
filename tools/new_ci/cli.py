#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_001.py
# 测试内容：测试虚拟机创建部署流程
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
import sys,os,cmd

# 基本引用头模块
from base_function_module.interface.config_para import config_para
from base_function_module.interface.base_interface.rpc.contrib.tecs_common import *
from base_function_module.interface.base_interface.rpc.contrib import xmlrpclibex

from base_function_module.interface.base_interface.rpc.clustercfg import *
from base_function_module.interface.base_interface.rpc.clustermanager import *
from base_function_module.interface.base_interface.rpc.dhcp import *
from base_function_module.interface.base_interface.rpc.file import *
from base_function_module.interface.base_interface.rpc.host import *
from base_function_module.interface.base_interface.rpc.image import *
from base_function_module.interface.base_interface.rpc.ip import *
from base_function_module.interface.base_interface.rpc.netplane import *
from base_function_module.interface.base_interface.rpc.netplane_default import *
from base_function_module.interface.base_interface.rpc.port import *
from base_function_module.interface.base_interface.rpc.project import *
from base_function_module.interface.base_interface.rpc.rpc import *
from base_function_module.interface.base_interface.rpc.tcu import *
from base_function_module.interface.base_interface.rpc.tecssystem import *
from base_function_module.interface.base_interface.rpc.trunk import *
from base_function_module.interface.base_interface.rpc.user import *
from base_function_module.interface.base_interface.rpc.usergroup import *
from base_function_module.interface.base_interface.rpc.vmcfg import *
from base_function_module.interface.base_interface.rpc.vmtemplate import *
from base_function_module.interface.base_interface.rpc.vnet_vlan import *

               
class TecsShell(cmd.Cmd):    
    def __init__(self, config_para_handler):
        cmd.Cmd.__init__(self)
        # 记录当前测试场景的配置参数
        self.config_para = config_para_handler
        
    def do_login(self,s):
        ''' login into tecs, arg1=username, arg2=password '''
        args=s.split()
        if len(args)!=2:
            print "invalid number of arguments!"
            return
        (username,password) = args
        '''登录tecs，获取管理员账户加密后的session'''
        server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',username,':',password,'@',self.config_para.server_addr,':',self.config_para.server_port,'/RPC2')
        print "Connect to xmlrpc url %s ..." % server_url
        try:
            self.server = xmlrpclibex.ServerProxy(server_url)
        except:
            ci_exit(-1,"failed to create xmlrpc ServerProxy!")
        self.session = login(self.server,username,password)
        if self.session is None:
            ci_exit(-1,"failed to get encrypted session!")
        print "login success!"   
        self.username = username
        self.password = password
        self.prompt = "[%s@cloud]# " % username        
    
    def do_runtime_query(self,s):
        '''query tecs runtime information'''
        targetname=""
        args=s.split()
        if len(args)==0:
            targetname=""
        else:
            if len(args)!=1:
                print "*** invalid number of arguments"
                return
            else:
                targetname=args[0]
                
        s = tecssystem(self.config_para.server_addr,self.session)
        s.show_runtime_state(TYPE_CACULATE,targetname)

    def do_get_runtime_state(self,s):
        '''query tecs node runtime information'''
        targetname=""
        args=s.split()
        if len(args)==0:
            targetname=""
        else:
            if len(args)!=1:
                print "*** invalid number of arguments"
                return
            else:
                targetname=args[0]
                
        s = tecssystem(self.config_para.server_addr,self.session)
        ret = s.get_runtime_state(TYPE_CACULATE,targetname)
        if ret == None:
            print "failed to get %s runtime state!"
        else:
            print "%s runtime state = %d" % (targetname, ret)
        
    def do_license_query(self,s):
        '''query tecs license information'''
        s = tecssystem(self.config_para.server_addr,self.session)
        s.show_license()
            
    def do_clustercfg_delete(self,s):
        ''' delete a clustercfg, arg1=clustername, arg2=configname '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (clustername,configname)=args
       
        clustercfg = clustercfg(self.config_para.server_addr,self.session)
        if clustercfg.delete(clustername,configname) != True:
            ci_exit(-1)
        if clustercfg.query(clustername,configname) != True:
            ci_exit(-1)    
    
    def do_clustercfg_query(self,s):
        ''' delete a clustercfg, arg1=clustername, arg2=configname '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (clustername,configname)=args
       
        ccfg = clustercfg(self.config_para.server_addr,self.session)
        if ccfg.query(clustername,"") != True:
            ci_exit(-1)  
    
    def do_clustercfg_set(self,s):
        ''' delete a clustercfg, arg1=clustername, arg2=configname, arg3=configvalue '''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            return
        (clustername,configname,configvalue)=args
       
        ccfg = clustercfg(self.config_para.server_addr,self.session)
        if ccfg.set(clustername,configname,configvalue) != True:
            ci_exit(-1)
        if ccfg.query(clustername,configname) != True:
            ci_exit(-1)   
            
    def do_regcc(self,s):
        ''' register a cluster, arg=clustername '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        clustername=args[0]
       
        cm = clustermanager(self.config_para.server_addr,self.session)
        if cm.register(clustername,"test") != True:
            ci_exit(-1)
        if cm.query(0L,100L,1) != True:
            ci_exit(-1)

    def do_unregcc(self,s):
        ''' unregister a cluster, arg=clustername '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        clustername=args[0]
       
        cm = clustermanager(self.config_para.server_addr,self.session)
        if cm.unregister(clustername,"test") != True:
            ci_exit(-1)
            
    def do_disablecc(self,s):
        ''' disable a cluster, arg=clustername '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        clustername=args[0]
       
        cm = clustermanager(self.config_para.server_addr,self.session)
        if cm.disable(clustername,"test") != True:
            ci_exit(-1)
    
    def do_enablecc(self,s):
        ''' enable a cluster, arg=clustername '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        clustername=args[0]
       
        cm = clustermanager(self.config_para.server_addr,self.session)
        if cm.enable(clustername,"test") != True:
            ci_exit(-1)
            
    def do_reghc(self,s):
        ''' register a host to cluster, arg1=clustername, arg2=hostname '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (clustername,hostname) = args    
        
        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,False,"test") != True:
            ci_exit(-1)
            
        if h.query(0L,100L,clustername,hostname) != True:
            ci_exit(-1)

        # 获取刚刚注册的主机的cpu id   
        cpu_id = h.get_cpu_id(clustername,hostname)    
        if cpu_id is None:
            ci_exit(-1)
        print "cpu id of host %s = %d" % (hostname,cpu_id) 
 
    def do_disable_hc(self,s): 
        ''' disable a host in cluster, arg1=clustername, arg2=hostname,arg3=is_forced ''' 
        args=s.split()
        if len(args)!=3: 
            print "*** invalid number of arguments"
            return
        (clustername,hostname,is_forced) = args  
        is_forced = bool(is_forced);

        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,is_forced,"register") != True: 
            ci_exit(-1)
        if h.disable(clustername,hostname,is_forced) != True: 
            ci_exit(-1)

    def do_enable_hc(self,s): 
        ''' enable a host in cluster, arg1=clustername, arg2=hostname ''' 
        args=s.split()
        if len(args)!=2: 
            print "*** invalid number of arguments"
            return
        (clustername,hostname) = args  

        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,True,"register") != True: 
            ci_exit(-1)
        if h.enable(clustername,hostname) != True: 
            ci_exit(-1)

    def do_forget_hc(self,s): 
        ''' forget a host in cluster, arg1=clustername, arg2=hostname ''' 
        args=s.split()
        if len(args)!=2: 
            print "*** invalid number of arguments"
            return
        (clustername,hostname) = args  

        h = host(self.config_para.server_addr,self.session)
        if h.unregister(clustername,hostname,"register") != True: 
            ci_exit(-1)
        if h.forget(clustername,hostname) != True: 
            ci_exit(-1)

    def do_reboot_hc(self,s): 
        ''' reboot a host in cluster, arg1=clustername, arg2=hostname ''' 
        args=s.split()
        if len(args)!=2: 
            print "*** invalid number of arguments"
            return
        (clustername,hostname) = args  

        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,True,"register") != True: 
            ci_exit(-1)
        if h.reboot(clustername,hostname) != True: 
            ci_exit(-1)

    def do_shutdown_hc(self,s): 
        ''' shutdown a host in cluster, arg1=clustername, arg2=hostname''' 
        args=s.split()
        if len(args)!=2: 
            print "*** invalid number of arguments"
            return
        (clustername,hostname) = args  

        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,True,"register") != True: 
            ci_exit(-1)
        if h.shutdown(clustername,hostname) != True: 
            ci_exit(-1)

    def do_unreg_hc(self,s): 
        ''' unregister a host from cluster, arg1=clustername, arg2=hostname ''' 
        args=s.split()
        if len(args)!=2: 
            print "*** invalid number of arguments"
            return
        (clustername,hostname) = args  

        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,True,"register") != True: 
            ci_exit(-1)
        if h.unregister(clustername,hostname,"unregister") != True: 
            ci_exit(-1)

    def do_test_hc(self,s):
        ''' test hc api,arg1=clustername, arg2=hostname'''

        args=s.split()
        if len(args)!=2: 
            print "*** invalid number of arguments"
            return

        (clustername,hostname) = args
        (is_forced,des) = [True,"test hc des"]

        h = host(self.config_para.server_addr,self.session)
        if h.register(clustername,hostname,is_forced,des) != True: 
            ci_exit(-1)

        if h.disable(clustername,hostname,True) != True: 
            ci_exit(-1)

        if h.enable(clustername,hostname) != True: 
            ci_exit(-1)

        if h.unregister(clustername,hostname,des) != True: 
            ci_exit(-1)

        if h.forget(clustername,hostname) != True: 
            ci_exit(-1)

        if h.register(clustername,hostname,is_forced,des) != True: 
            ci_exit(-1)

        if h.shutdown(clustername,hostname) != True: 
            ci_exit(-1)

        if h.reboot(clustername,hostname) != True: 
            ci_exit(-1)

        if h.unregister(clustername,hostname,des) != True: 
            ci_exit(-1)  
         
    def do_set_svcpu(self,s):
        ''' set svcpu in cluster, arg1=clustername, arg2=cpu_id, arg3=svcpu_num '''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            return
        clustername=args[0]
        cpu_id=long(args[1])
        svcpu_num=int(args[2])
        
        sv = svcpu(self.config_para.server_addr,self.session)
        if sv.set(clustername,"",cpu_id,svcpu_num,"test cpu description") != True:
            ci_exit(-1)

        if sv.query(0L,100L,clustername,"",cpu_id) != True:
            ci_exit(-1)  
            
    def do_delete_svcpu(self,s):
        ''' delet svcpu in cluster, arg1=clustername, arg2=cpu_id'''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (clustername,cpu_id) = args
        cpu_id=long(cpu_id)
        
        sv = svcpu(self.config_para.server_addr,self.session)
        if sv.delete(clustername,"",cpu_id) != True:
            ci_exit(-1)

        if sv.query(0L,100L,clustername,"",cpu_id) != True:
            ci_exit(-1)   
    
    def do_create_usergroup(self,s):
        ''' create a new user group, arg=groupname '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        groupname=args[0]
        
        ug = usergroup(self.config_para.server_addr,self.session)
        if ug.allocate(groupname,1) != True:
            ci_exit(-1)
        if ug.query(0L, 100L, groupname) != True:
            ci_exit(-1) 
        
    def do_delete_usergroup(self,s):
        ''' delete a user group,arg=groupname '''
        args=s.split()
        if len(args)!=1:
            print " *** invalid number of arguments "
            return
        groupname=args[0]
    
        ug = usergroup(self.config_para.server_addr,self.session)
        if ug.delete(groupname) != True:
            ci_exit(-1)     
        if ug.query(0L, 100L, groupname) != True:
            ci_exit(-1) 
            
    def do_query_usergroup(self,s):
        ''' query a user group,arg1= start_index, arg2=query_count, arg3=groupname '''
        args=s.split()
        if len(args)!=3:
            print " *** invalid number of arguments "
            return
        (start_index, query_count, groupname)=args
    
        ug = usergroup(self.config_para.server_addr,self.session)
        if ug.query(0L, 100L, groupname) != True:
           ci_exit(-1) 
    
    def do_set_usergroup(self,s):
        ''' set a user group,arg1= groupname, arg2=enabled arg3=description '''
        args=s.split()
        if len(args)!=3:
            print " *** invalid number of arguments "
            return
        groupname=args[0]
        enabled=int(args[1])
        description=args[2]
    
        ug = usergroup(self.config_para.server_addr,self.session)
        if ug.set(groupname,enabled,"test usergroup description") != True:
           ci_exit(-1)     
        if ug.query(0L, 100L, groupname) != True:
           ci_exit(-1) 
           
    def do_create_user(self,s):
        ''' create a new user, arg1=username, arg2=password, arg3=groupname '''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            return
        (username,password,groupname) = args    
        
        u = user(self.config_para.server_addr,self.session)
        if u.allocate(username,password,password,3,groupname) != True:
            ci_exit(-1)
        if u.query(0L, 100L, username) != True:
            ci_exit(-1)
            
    def do_delete_user(self,s):
        ''' delete a new user, arg1=username'''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        username = args[0]    
        
        u = user(self.config_para.server_addr,self.session)
        if u.delete(username) != True:
            ci_exit(-1)
    
    def do_query_user(self,s):
        ''' query a user,arg1= start_index, arg2=query_count, arg3=username '''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            return
        (start_index, query_count, username)=args    
        
        u = user(self.config_para.server_addr,self.session)
        if u.query(0L, 100L, username) != True:
            ci_exit(-1)
    
    def do_get_image_id(self,s):
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        image_name=args[0]
        i = image(self.config_para.server_addr,self.session)
        image_id = i.file_query(image_name)
        print "image_id = ", image_id
        
    def do_fullquery_user(self,s):
        ''' full query a user,arg1= start_index, arg2=query_count, arg3=query_type, arg4 = query_user'''
        args=s.split()
        if len(args)!=4:
            print "*** invalid number of arguments"
            print self.do_fullquery_user.__doc__
            return
        (start_index, query_count, query_type,query_user)=args    
        
        u = user(self.config_para.server_addr,self.session)
        if u.fullquery(long(start_index), long(query_count), int(query_type),query_user) != True:
            ci_exit(-1)
            
    def do_get_image_id(self,s):
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        image_name=args[0]
        i = image(self.config_para.server_addr,self.session)
        image_id = i.file_query(image_name)
        print "image_id = ", image_id
        
    def do_fullquery_user(self,s):
        ''' full query a user,arg1= start_index, arg2=query_count, arg3=query_type, arg4 = query_user'''
        args=s.split()
        if len(args)!=4:
            print "*** invalid number of arguments"
            print self.do_fullquery_user.__doc__
            return
        (start_index, query_count, query_type,query_user)=args    
        
        u = user(self.config_para.server_addr,self.session)
        if u.fullquery(long(start_index), long(query_count), int(query_type),query_user) != True:
            ci_exit(-1)
            
    def do_set_user(self,s):
        ''' set a user,arg1= username , arg2=password arg3=confirm_pass arg4=role arg5=groupname '''
        args=s.split()
        if len(args)!=5:
            print "*** invalid number of arguments"
            return
        (username,password,confirm_pass,role,groupname)=args    
        role=int(role)
        
        u = user(self.config_para.server_addr,self.session)
        if u.set(username,password,confirm_pass,role,groupname,1,"138*","testuser@163.com","zte") != True:
            ci_exit(-1)
            
    def do_upload_image(self,s):
        ''' upload a new image, arg=image_path '''
        args=s.split()
        if len(args)!=1:
            print "invalid number of arguments!"
            return
        image_path = args[0]
        
        # 使用ftp客户端上传一个虚拟机machine映像
        ftp = FTP()
        try:
            ftp.connect(server_addr,21)
            ftp.login(self.username,self.password)
        except:
            ci_exit(-1,"ftp login failed with username=%s,password=%s!" % (self.username,self.password))

        print ftp.getwelcome()
        try:
            cmd='%s %s' % ("stor",os.path.basename(image_path))
            ftp.storbinary(cmd,open(image_path,'rb'),1024)
        except:
            ci_exit(-1,"ftp upload failed!")

        # 根据文件名获取刚刚上传的映像文件在tecs中的id
        print "waiting for image id, sleeping ..."
        time.sleep(5)
        i = image(self.config_para.server_addr,self.session)
        image_id = i.get_id_by_filename(os.path.basename(image_path))
        if image_id is None:
            ci_exit(-1,"failed to get image id of file %s!" % os.path.basename(image_path))
            
        # 注册映像
        print "image id of file %s = %d" % (image_path,image_id)    
        if i.register(image_id,"test","machine","x86_64",False,True,"test image description") != True:
            ci_exit(-1)

        if i.enable(image_id,False) != True:
            ci_exit(-1)

        if i.enable(image_id,True) != True:
            ci_exit(-1)

        if i.publish(image_id,False) != True:
            ci_exit(-1) 

        if i.publish(image_id,True) != True:
            ci_exit(-1) 

        if i.query(0L,200L,1,"",image_id) != True:
            ci_exit(-1)

        (image_id,name,type,arch,bus,des) = [image_id,"test","cdrom","i386","ide","image description"]
        if i.set(image_id,name,type,arch,bus,des) != True:
            ci_exit(-1)

        (image_id,name,type,arch,bus,des) = [image_id,"test","machine","x86_64","scsi","test image description"]
        if i.set(image_id,name,type,arch,bus,des) != True:
            ci_exit(-1)

        if i.unregister(image_id) != True:
            ci_exit(-1)

    def do_enable_image(self,s):
        ''' enable a image , arg1=id, arg2=flage '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (id,flag)=args
        
        im = image(self.config_para.server_addr,self.session)
        if im.enable(id,bool(flag)) != True:
            ci_exit(-1)

    def do_publish_image(self,s):
        ''' publish a image , arg1=id, arg2=flage '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (id,flag)=args
        
        im = image(self.config_para.server_addr,self.session)
        if im.publish(id,bool(flag)) != True:
            ci_exit(-1)

    def do_query_image(self,s):
        ''' query a image , arg1=start_index,arg2=query_count,arg3=type,arg4=user_name,arg5=image_id'''

        args=s.split()
        if len(args)!=5:
            print "*** invalid number of arguments"
            return
        (start_index,query_count,type,user_name,image_id)=args
        
        im = image(self.config_para.server_addr,self.session)
        if im.query(start_index,query_count,type,user_name,image_id) != True:
            ci_exit(-1)

    def do_query_image_bus_by_id(self,s):
        ''' query a image bus, arg=id '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        id=long(args[0])
        im = image(self.config_para.server_addr,self.session)
        im.query_bus_by_id(id)

    def do_register_image(self,s):
        ''' register a image , arg1=id,arg2=name,arg3=arch,arg4=is_public,arg5=is_available '''
        args=s.split()
        if len(args)!=5:
            print "*** invalid number of arguments"
            return
        (id,name,type,arch,is_public,is_available)=args
        
        im = image(self.config_para.server_addr,self.session)
        if im.register(id,name,type,arch,bool(is_public),bool(is_available),"regimage") != True:

            ci_exit(-1)

    def do_set_image(self,s):
        ''' set a image , arg1=id,arg2=name,arg3=type,arg4=arch,arg5=bus'''
        args=s.split()
        if len(args)!=5:
            print "*** invalid number of arguments"
            return
        (id,name,type,arch,bus)=args
        
        im = image(self.config_para.server_addr,self.session)
        if im.set(long(id),name,type,arch,bus,"setimg") != True:
            ci_exit(-1)

    def do_unregister_image(self,s):
        ''' unregister a image , arg=id '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        id=long(args[0])
        
        im = image(self.config_para.server_addr,self.session)
        if im.unregister(id) != True:
            ci_exit(-1)

    def do_delete_image(self,s):
        ''' unregister a image , arg=id '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        id=long(args[0])
        
        im = image(self.config_para.server_addr,self.session)
        if im.delete(id) != True:
            ci_exit(-1)
            
    def do_test_image(self,s):     
        ''' test image api, arg=image_id  '''
        i = image(self.config_para.server_addr,self.session)
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        image_id=long(args[0])

        if i.register(image_id,"test","machine","x86_64",False,True,"test image description") != True:
            ci_exit(-1)

        if i.enable(image_id,False) != True:
            ci_exit(-1)

        if i.enable(image_id,True) != True:
            ci_exit(-1)

        if i.publish(image_id,False) != True:
            ci_exit(-1) 

        if i.publish(image_id,True) != True:
            ci_exit(-1) 

        if i.query(0L,200L,1,"",image_id) != True:
            ci_exit(-1)

        (name,type,arch,des) = ["test","cdrom","i386","image description"]
        if i.set(image_id,name,type,arch,des) != True:
            ci_exit(-1)

        (name,type,arch,des) = ["test","machine","x86_64","test image description"]
        if i.set(image_id,name,type,arch,des) != True:
            ci_exit(-1)

        if i.unregister(long(image_id)) != True:
            ci_exit(-1)

    def do_create_project(self,s):     
        ''' create a new project, arg=projectname '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            return
        projectname=args[0]
        p = project(self.config_para.server_addr,self.session)
        if p.allocate(projectname,"test project description") != True:
            ci_exit(-1)

    def do_delete_project(self,s):     
        ''' delete a project, arg1=projectname,arg2=user_name '''
        args=s.split()
        if len(args) < 1:
            print "*** invalid number of arguments"
            return

        if len(args) == 1:
            name=args[0]
            user_name = ""
        else:
            (name,user_name)=args

        p = project(self.config_para.server_addr,self.session)
        if p.delete(name,user_name) != True:
            ci_exit(-1)

    def do_query_project(self,s):     
        ''' create a new project,arg1=projectname,arg2=user_name '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (name,user_name)=args

        p = project(self.config_para.server_addr,self.session)
        if p.allocate(name,"test project description") != True:
            ci_exit(-1)
        if p.query(0L,200L,-5,name,user_name) != True:
            ci_exit(-1)

        if p.query(0L,200L,0,name,user_name) != True:
            ci_exit(-1)

    def do_query_vm_by_project(self,s):     
        ''' create a new project, arg1=projectname,arg2=user_name '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (start_index,query_count,name,user_name)=args
        p = project(self.config_para.server_addr,self.session)
        if p.query_vm_by_project(0L,50L,name,user_name) != True:
            ci_exit(-1)

    def do_set_project(self,s):     
        ''' set a project, arg1=projectname,arg2=description '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            return
        (name,description)=args
        p = project(self.config_para.server_addr,self.session)
        if p.set(name,description) != True:
            ci_exit(-1)

    def do_test_project(self,s):     
        ''' test project api '''

        (start_index,query_count,flag,name,user_name,description)=[0L,50L,0L,"test_project","","test project description"]
        p = project(self.config_para.server_addr,self.session)
        
        if p.allocate(name,description) != True:
            ci_exit(-1)
 
        if p.query(start_index,query_count,flag,name,user_name) != True:
            ci_exit(-1)

        if p.query_vm_by_project(start_index,query_count,name,user_name) != True:
            ci_exit(-1)

        if p.set(name,description) != True:
            ci_exit(-1)

        if p.delete(name,user_name) != True:
            ci_exit(-1)

    def do_delete_port(self,s):
        ''' delete port by cluster name,host name,port name '''

        cluster_name = "tecs_cluster"
        host_name = "tecs_host"
        port_name = "tecs_port"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,port_name) = args 
        else:
            print "*** invalid number of arguments"
            return
        args = s.split()
        if len(args) != 3:
            print "*** invalid number of arguments"
            return      
        (cluster_name,host_name,port_name) = args

        p = port(self.config_para.server_addr,self.session)
        if p.delete(cluster_name,host_name,port_name) != True:
            ci_exit(-1) 
      
    def do_query_port(self,s):
        ''' query port by cluster name,host name,port name '''       

        cluster_name = "tecs_cluster"
        host_name = "tecs_host"
        port_name = "tecs_port"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,port_name) = args 
        else:
            print "*** invalid number of arguments"
            return

        p = port(self.config_para.server_addr,self.session)
        if p.query(cluster_name,host_name,port_name) != True:
            ci_exit(-1) 
   
    def do_creat_port(self,s):
        ''' creat port by cluster name,host name,portname,
            used,negotiate,speed,duplex,master,delay_up_time 
        '''
        
        cluster_name = "tecs_cluster"
        host_name = "tecs_host"
        port_name = "tecs_port"
        port_paras = {"used":1,
                      "negotiate":0,
                      "speed":2,
                      "duplex":1,
                      "master":1,
                      "delay_up_time":1}

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,port_name) = args  
        elif len(args) == 9: 
            (cluster_name,host_name,port_name) = args[0:3]
            port_paras = {"used":int(args[3]),
                          "negotiate":int(args[4]),
                          "speed":int(args[5]),
                          "duplex":int(args[6]),
                          "master":int(args[7]),
                          "delay_up_time":int(args[8])}
        else:
            print "*** invalid number of arguments"
            return

        p = port(self.config_para.server_addr,self.session)
        if p.set(cluster_name,host_name,port_name,port_paras) != True:
            ci_exit(-1)

    def do_test_port(self,s):
        ''' test port api:creat query del by cluster_name,host_name'''
        cluster_name = "tecs_cluster"
        host_name = "tecs_host"
        port_name = "tecs_port"
        port_paras = {"used":1,
                      "negotiate":0,
                      "speed":2,
                      "duplex":1,
                      "master":1,
                      "delay_up_time":1}

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,port_name) = args 
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        p = port(self.config_para.server_addr,self.session)
        if p.set(cluster_name,host_name,port_name,port_paras) != True:
            ci_exit(-1)

        if p.query(cluster_name,host_name,port_name) != True:
            ci_exit(-1)

        if p.delete(cluster_name,host_name,port_name) != True:
            ci_exit(-1)

    def do_delete_trunk(self,s):
        ''' delete trunk by cluster name host name,trunk name,trunk ports'''

        cluster_name = "tecs_cluster"
        host_name = "tecs_host"
        trunk_name = "tecs_trunk"
        trunk_ports = ["trunk_port_1",
                       "trunk_port_2"]

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,trunk_name) = args 
        elif len(args) > 3:
            (cluster_name,host_name,trunk_name) = args 
            trunk_ports = args[3:]

        t = trunk(self.config_para.server_addr,self.session)
        if t.delete(cluster_name,host_name,trunk_name,trunk_ports) != True:
            ci_exit(-1) 
      
    def do_query_trunk(self,s):
        ''' query trunk by cluster name,host name,trunk name'''

        cluster_name = "tecs_cluster"
        host_name    = "tecs_host"
        trunk_name   = "tecs_trunk"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,trunk_name) = args 

        t = trunk(self.config_para.server_addr,self.session)
        if t.query(cluster_name,host_name,trunk_name) != True:
            ci_exit(-1)
   
    def do_creat_trunk(self,s):
        ''' creat trunk by cluster name,host name,trunk name,trunk_type,trunk_ports'''

        cluster_name = "tecs_cluster"
        host_name    = "tecs_host"
        trunk_name   = "tecs_trunk"
        trunk_type   = 1
        trunk_ports  = ["trunk_port_1",
                        "trunk_port_2"]

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,trunk_name) = args 
        elif len(args) == 4:
            (cluster_name,host_name,trunk_name,trunk_type) = args 
            trunk_type = int(trunk_type)
            trunk_ports = args[3:]
        elif len(args) > 4:
            (cluster_name,host_name,trunk_name,trunk_type) = args 
            trunk_type = int(trunk_type)
            trunk_ports = args[3:]

        t = trunk(self.config_para.server_addr,self.session)
        if t.set(cluster_name,host_name,trunk_name,trunk_type,trunk_ports) != True:
            ci_exit(-1)

    def do_test_trunk(self,s):
        ''' test trunk api:creat query del by cluster_name,host_name'''

        cluster_name = "tecs_cluster"
        host_name    = "tecs_host"
        trunk_name   = "tecs_trunk"
        trunk_type   = 1
        trunk_ports  = ["trunk_port_1",
                        "trunk_port_2"]

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,trunk_name) = args 
        elif len(args) == 4:
            (cluster_name,host_name,trunk_name,trunk_type) = args 
            trunk_type = int(trunk_type)
            trunk_ports = args[3:]
        elif len(args) > 4:
            (cluster_name,host_name,trunk_name,trunk_type) = args 
            trunk_type = int(trunk_type)
            trunk_ports = args[3:]
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return
       
        t = trunk(self.config_para.server_addr,self.session)
        if t.set(cluster_name,host_name,trunk_name,trunk_type,trunk_ports) != True:
            ci_exit(-1)

        if t.query(cluster_name,host_name,trunk_name) != True:
            ci_exit(-1)

        if t.delete(cluster_name,host_name,trunk_name,trunk_ports) != True:
            ci_exit(-1)
   
    def do_delete_netplane(self,s):
        ''' delete netplane by cluster name,netplane name'''

        cluster_name = "tecs_cluster"
        netplane_name = "tecs_netplane"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,netplane_name) = args  
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        np = netplane(self.config_para.server_addr,self.session)
        if np.delete(cluster_name,netplane_name) != True:
            ci_exit(-1)
      
    def do_query_netplane(self,s):
        ''' query netplane by cluster name,start index,query count'''

        cluster_name = "tecs_cluster"
        start_index  = 0L
        query_count  = 200L

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]
        elif len(args) == 2:
            (cluster_name,start_index) = args
            start_index = long(start_index)
        elif len(args) == 3:
            (cluster_name,start_index,query_count) = args
            start_index = long(start_index)
            query_count = long(query_count)
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        np = netplane(self.config_para.server_addr,self.session)
        if np.query(start_index,query_count,cluster_name) != True:
            ci_exit(-1)

    def do_creat_netplane(self,s):
        ''' creat netplane by cluster name,netplane name'''

        cluster_name = "tecs_cluster"
        netplane_name = "tecs_netplane"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,netplane_name) = args  
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        np = netplane(self.config_para.server_addr,self.session)
        if np.set(cluster_name,netplane_name) != True:
            ci_exit(-1)

    def do_test_netplane(self,s):
        ''' 
        test netplane api by cluster name,netplane name
        '''
        cluster_name  = "tecs_cluster"
        netplane_name = "tecs_netplane"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,netplane_name) = args  
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        np = netplane(self.config_para.server_addr,self.session)
        if np.set(cluster_name,netplane_name) != True:
            ci_exit(-1)

        if np.query(cluster_name) != True:
            ci_exit(-1)

        if np.delete(cluster_name,netplane_name) != True:
            ci_exit(-1)

    def do_delete_netplane_map(self,s):
        ''' 
        delete netplane map by cluster name,host name
        '''
        cluster_name  = "tecs_cluster"
        host_name     = "tecs_host"
        nic_name      = "tecs_nic"

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args 
        elif len(args) == 3:
            (cluster_name,host_name,nic_name) = args   
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        nmp = netplane_map(self.config_para.server_addr,self.session)
        if nmp.delete(cluster_name,host_name,nic_name) != True:
            ci_exit(-1)

    def do_query_netplane_map(self,s):
        ''' 
        creat netplane map by cluster name,host name,start index,query count
        '''
        cluster_name = "tecs_cluster"
        host_name    = "tecs_host"
        start_index  = 0L
        query_count  = 200L

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,start_index) = args
            start_index = long(start_index)
        elif len(args) == 4:
            (cluster_name,netplane_name,start_index,query_count) = args
            start_index = long(start_index)
            query_count = long(query_count)
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        nmp = netplane_map(self.config_para.server_addr,self.session)
        if nmp.query(start_index,query_count,cluster_name,host_name) != True:
            ci_exit(-1)

    def do_creat_netplane_map(self,s):
        ''' 
        creat netplane map by cluster name,host name,nic name,netplane name,evb mod
        '''
        cluster_name  = "tecs_cluster"
        host_name     = "tecs_host"
        nic_name      = "tecs_nic"
        netplane_name = "tecs_netplane"
        evb_mod       = 1

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,nic_name) = args 
        elif len(args) == 4:
            (cluster_name,host_name,nic_name,netplane_name) = args 
        elif len(args) == 5:
            (cluster_name,host_name,nic_name,netplane_name,evb_mod) = args 
            evb_mod= int(evb_mod)
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        nmp = netplane_map(self.config_para.server_addr,self.session)
        if nmp.set(cluster_name,host_name,nic_name,netplane_name,evb_mod) != True:
            ci_exit(-1)

    def do_test_netplane_map(self,s):
        ''' 
        test netplane api by cluster name,host name,nic name,netplane name,evb mod
        '''
        cluster_name  = "tecs_cluster"
        host_name = "tecs_netplane"
        nic_name      = "tecs_nic"
        netplane_name = "tecs_netplane"
        evb_mod       = 1

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,nic_name) = args 
        elif len(args) == 4:
            (cluster_name,host_name,nic_name,netplane_name) = args 
        elif len(args) == 5:
            (cluster_name,host_name,nic_name,netplane_name,evb_mod) = args 
            evb_mod= int(evb_mod)
        elif len(args) != 0:
            print "*** invalid number of arguments"
            return 

        nmp = netplane_map(self.config_para.server_addr,self.session)
        if nmp.set(cluster_name,host_name,nic_name,netplane_name,evb_mod) != True:
            ci_exit(-1)

        if nmp.query(0L,200L,cluster_name,host_name) != True:
            ci_exit(-1)

        if nmp.delete(cluster_name,host_name) != True:
            ci_exit(-1)

    def do_query_vlan(self,s):
        ''' 
        creat netplane map by cluster name,host name,start index,query count
        '''
        cluster_name = "tecs_cluster"
        host_name    = "tecs_host"
        start_index  = 0L
        query_count  = 200L

        args = s.split()
        if len(args) == 1:
            cluster_name = args[0]  
        elif len(args) == 2:
            (cluster_name,host_name) = args  
        elif len(args) == 3:
            (cluster_name,host_name,start_index) = args
            start_index= long(start_index)
        elif len(args) == 4:
            (cluster_name,netplane_name,start_index,query_count) = args
            start_index= long(start_index)
            query_count= long(query_count)
        elif len(args) != 0:
            print "*** invalid number of arguments"
            print self.do_query_vlan.__doc__
            return 

        vn = vlan(self.config_para.server_addr,self.session)
        if vn.query(start_index,query_count,cluster_name,host_name) != True:
            ci_exit(-1)

    def do_create_vmtpl(self,s):
        ''' create a new vm template, arg1=template_name, arg2=image_id '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            print self.do_create_vmtpl.__doc__
            return
        template_name=args[0]
        image_id=long(args[1])
        
        cfg = {}
        cfg["base_info"] =  {}
        cfg["base_info"]["cluster"] = "tecscluster"
        cfg["base_info"]["host"] = "tecshost"
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["tpu"] = 1
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["kernel"] = {"id":-1L,"size":0L}
        cfg["base_info"]["ramdisk"] = {"id":-1L,"size":0L}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["hypervisor"] = "xen"		
        disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        cfg["base_info"]["disks"] = [disk1,]
        cfg["vt_name"] =  template_name
        cfg["is_public"] =  True
        v = vmtcfg(self.config_para.server_addr,self.session)
        if v.allocate(cfg) != True:
            ci_exit(-1)

    def do_set_tpl_disk(self,s):
        ''' set a vm template disk, arg=template_name '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            print self.do_set_tpl_disk.__doc__
            return
        template_name=args[0]
        disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        disk2 = {"id":-1L,"size":long(256 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdc","position":2,"fstype":"ext4"}
        disks = [disk1,disk2]
        v = vmtcfg(self.config_para.server_addr,self.session)
        if v.set_disk(template_name,disks) != True:
            ci_exit(-1)
            
    def do_create_vm(self,s):
        ''' create a new vm, arg1=projectname, arg2=vmname, arg3=image_id '''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            print self.do_create_vm.__doc__
            return
        projectname=args[0]
        vmname=args[1]
        image_id=long(args[2])
        
        cfg = {}
        cfg["base_info"] =  {}
        
        #cfg["base_info"]["cluster"] = "tecscluster"
        #cfg["base_info"]["host"] = "tecshost"
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["tpu"] = 1
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["kernel"] = {"id":-1L,"size":0L}
        cfg["base_info"]["ramdisk"] = {"id":-1L,"size":0L}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["hypervisor"] = "xen"				
        cfg["base_info"]["vm_wdtime"] = 0
        
        disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        cfg["base_info"]["disks"] = [disk1,]
        cfg["vm_name"] = vmname
        cfg["project_name"] = projectname
        cfg["vm_num"] = 1
        v = vmcfg(self.config_para.server_addr,self.session)
        if v.allocate(cfg) != True:
            ci_exit(-1)

        # 根据名称获取刚刚创建的虚拟机的id
        vmid = v.get_vm_id(vmname)
        if vmid is None:
            ci_exit(-1,"failed to get vm id!")
        print "id of vm %s is %d!" % (vmname,vmid)
        
        # 获取虚拟机状态
        vmstate = v.get_vm_state(vmid)
        if vmstate is None:
            ci_exit(-1,"failed to get vm state!")   
        print "state of vm %s is %d!" % (vmname,vmstate)

    def do_operate_vm(self,s):
        ''' operate a vm, arg1=vmid, arg2=action '''
        args=s.split()
        if len(args)!=2:
            print "*** invalid number of arguments"
            print self.do_operate_vm.__doc__
            return
        vmid=long(args[0])
        action=args[1]
        
        print "action %s on vm %d ..." %(action,vmid)
        v = vmcfg(self.config_para.server_addr,self.session)
        if v.action(vmid,action) != True:
            ci_exit(-1)

    def do_set_disk(self,s):
        ''' set a vm disk, arg=vmid '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            print self.do_set_disk.__doc__
            return
        vmid=long(args[0])
        disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        disk2 = {"id":-1L,"size":long(256 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdc","position":2,"fstype":"ext4"}
        disks = [disk1,disk2]
        v = vmcfg(self.config_para.server_addr,self.session)
        if v.set_disk(vmid,disks) != True:
            ci_exit(-1)

    def do_modify_vm(self,s):
        ''' modify a vm, arg=vmid '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            print self.do_modify_vm.__doc__
            return
        vmid=long(args[0])
        v = vmcfg(self.config_para.server_addr,self.session)
        old_cfg=v.get_vm_cfg(vmid)
        print old_cfg
        new_cfg={}
        new_cfg["vid"] = vmid
        new_cfg["base_info"] = old_cfg["base_info"]
        new_cfg["vm_name"] = old_cfg["vm_name"]
        new_cfg["project_name"] = old_cfg["project_name"]
        new_cfg["base_info"]["cluster"] = "mycluster"
        new_cfg["base_info"]["host"] = "myhost"
        print new_cfg
        v.set(new_cfg)
        
    def do_show_vm(self,s):
        ''' show a vm, arg=vmid '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            print self.do_show_vm.__doc__
            return
        vmid=long(args[0])
        v = vmcfg(self.config_para.server_addr,self.session)
        v.query(vmid)
    
    def do_query_vm(self,s):
        ''' show a vm, arg1=query_type, arg2=query_user, arg3=query_project, arg4=start_index, arg5=query_count'''
        args=s.split()
        if len(args)!=5:
            print "*** invalid number of arguments"
            print self.do_query_vm.__doc__
            return
        query_type=long(args[0])
        query_user=args[1]
        query_project=args[2]
        start_index=long(args[3])
        query_count=long(args[4])
        v = vmcfg(self.config_para.server_addr,self.session)
        v.fullquery(query_type,query_user,query_project,start_index,query_count)
        
    def do_get_id_by_name(self,s):
        ''' get a vm id by name, arg1=query_name, arg2=query_user'''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            print self.do_get_id_by_name.__doc__
            return
        query_type=long(args[0])
        query_name=args[1]
        query_user=args[2]
        v = vmcfg(self.config_para.server_addr,self.session)
        v.get_id_by_name(query_type,query_name,query_user)
        
    def do_query_vm(self,s):
        ''' show a vm, arg1=query_type, arg2=query_user, arg3=query_project, arg4=start_index, arg5=query_count'''
        args=s.split()
        if len(args)!=5:
            print "*** invalid number of arguments"
            print self.do_query_vm.__doc__
            return
        query_type=long(args[0])
        query_user=args[1]
        query_project=args[2]
        start_index=long(args[3])
        query_count=long(args[4])
        v = vmcfg(self.config_para.server_addr,self.session)
        v.fullquery(query_type,query_user,query_project,start_index,query_count)
        
    def do_get_vmid_by_name(self,s):
        ''' get a vm id by name, arg1=query_name, arg2=query_user'''
        args=s.split()
        if len(args)!=3:
            print "*** invalid number of arguments"
            print self.do_get_id_by_name.__doc__
            return
        query_type=long(args[0])
        query_name=args[1]
        query_user=args[2]
        v = vmcfg(self.config_para.server_addr,self.session)
        v.get_id_by_name(query_type,query_name,query_user)
        
    def do_show_all_cc(self,s):
        ''' display all clusters, no args '''
        cm = clustermanager(self.config_para.server_addr,self.session) 
        cm.query(0L,20L,1)
        
    def do_show_all_hc(self,s):
        ''' show all hosts of a cluster, arg=clustername '''
        args=s.split()
        if len(args)!=1:
            print "*** invalid number of arguments"
            print self.do_show_all_hc.__doc__
            return
        clustername=args[0]
        h = host(self.config_para.server_addr,self.session)
        h.query(0L,100L,clustername,"")
 
    def do_show_vm_syn_info(self,s):
        ''' show vm syn info ,arg vm1_id vm2_id vm3_id .... '''
        args=s.split()
        vm_ids = []
        if len(args) == 0:
            print "*** invalid number of arguments"
            print self.do_show_all_hc.__doc__
            return
        for vm in args:
            vm_ids = vm_ids + [long(vm)]
        v = vmcfg(self.config_para.server_addr,self.session)
        v.query_vm_syn_state(vm_ids)
 
    def do_show_vm_syn_info(self,s):
        ''' show vm syn info ,arg vm1_id vm2_id vm3_id .... '''
        args=s.split()
        vm_ids = []
        if len(args) == 0:
            print "*** invalid number of arguments"
            print self.do_show_all_hc.__doc__
            return
        for vm in args:
            vm_ids = vm_ids + [long(vm)]
        v = vmcfg(self.config_para.server_addr,self.session)
        v.query_vm_syn_state(vm_ids)
 
    def do_shell(self, s):
        ''' run linux shell command '''
        os.system(s)
        
    def do_EOF(self,args):
        print
        sys.exit()

    def emptyline(self):
        '''By default when an empty line is entered, the last command is repeated. 
        You can change this behavior by overriding the emptyline method. 
        For example to disable the repetition of the last command: '''
        pass
        

if __name__ == "__main__":
    file_name = sys.argv[0]
    # 获取配置参数的句柄
    config_para_handler = config_para()
    config_para_handler.get_config(sys.argv[1:])
    # 开始进入CLI
    tecsshell = TecsShell(config_para_handler)
    #默认使用admin账号自动登录tecs
    tecsshell.do_login("admin admin")
    try:
        tecsshell.cmdloop()
    except KeyboardInterrupt:
        print
        sys.exit(0)
    
    
    
    
  