#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：st_comm_lib1.py
# 内容摘要：临时调试库
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012/4/17
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
sys.path.append('/home/ci_server/Tecs_stinit01/ci/pytest')
sys.path.append('/home/ci_agent/script_home/ci/pytest') 
from contrib import xmlrpclibex

from tecs.vmcfg import *
from tecs.user import *
from tecs.usergroup import *
from tecs.clustermanager import *
from tecs.host import *
from tecs.svcpu import *
from tecs.image import *
from tecs.vmcfg import *
from tecs.project import *
from tecs.file import *

def lines():
    try:
            raise Exception
    except:
            return sys.exc_info()[2].tb_frame.f_back.f_lineno

# 001 def:st_exit--异常退出；
#     arg:$1 value:值  $2 line:行号  $3 error:错误信息
#     style:st_exit(value,line,error)            
def st_exit(value=0,line = lines(),error="null",):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "######ST ERROR######tecs st test exit code = %d, line = %d, error string = %s" % (value,line,error)
        sys.exit(value)

def st_time():
    value = time.time()
    value = time.localtime(value)
    value = time.strftime("%Y%m%d %H:%M:%S", value)
    print "######ST TIME######current time is: %s "%value 
    return '['+value+']' 

# 006 def:st_log--日志文件记录执行用例结果
#     arg:$1 value:值  $2 line:行号  $3 info:执行结果信息
#     return: null
#     style:st_log(value,line(),info)
def st_log(value=0,line=lines(),info="null",):
    #filename=os.path.basename(__file__)
    filename =os.path.basename(os.path.realpath(sys.argv[0]))
    logname='/home/tecs/'+filename.replace('.py','.log')
    print logname
    f=open(logname,'a')
    if value == 0:
        print "######ST ERROR######tecs st test exit code = %d, line = %d, error string = %s" % (value,line,info)
    elif value == 1:
        print >> f, st_time(),
        print >>f," PASS (line = %d, %s)"%(line,info)
        print " PASS (line = %d, %s)"%(line,info)
    else:
        print >> f, st_time(),
        print >>f," FAIL (line = %d, %s)"%(line,info)
        print " FAIL (line = %d, %s)"%(line,info)
    f.flush()
    f.close()

# 002 def:login_user--用户登录TECS的WEB；
#     arg:$1 addr:IP地址  $2 account:用户
#     return: $1 encrypted_account_session  $2 server
#     style:login_user(addr,account)
def login_user(addr,account):
    server_addr = addr
    server_port = "8080"
    cloud_user = "admin"
    cloud_passwd = "admin"
    guest_user = account
    guest_passwd = account
  
    server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',cloud_user,':',cloud_passwd,'@',server_addr,':',server_port,'/RPC2')
    print "Connect to xmlrpc url %s ..." % server_url                                                
    try:                                                                                             
        server = xmlrpclibex.ServerProxy(server_url)                                                 
    except:                                                                                          
        st_exit(-1,lines(),"failed to create xmlrpc ServerProxy!") 
        
    server.system.listMethods()
    
    encrypted_root_session = login(server,cloud_user,cloud_passwd)                                   
    if encrypted_root_session is None:                                                               
        st_exit(-1,lines(),"failed to get encrypted root session!")  
        
    # 查询用户帐号是否已经注册                                               
    u = user(server,encrypted_root_session)                                  
    if u.query(0L, 1000L, guest_user) != True:
        # 不存在，则创建普通帐号用户，存在则直接登录
        if u.allocate(account,account,account,3,"st_group") != True:
            st_exit(-1,lines(),"failed to create new account!") 
            
    server_url2 = '%s%s%s%s%s%s%s%s%s' % ('http://',guest_user,':',guest_passwd,'@',server_addr,':',server_port,'/RPC2')   
    print "Connect to xmlrpc url %s ..." % server_url2                                                                  
    try:                                                                                                               
        server = xmlrpclibex.ServerProxy(server_url2)                                                                   
    except:                                                                                                            
        st_exit(-1,lines(),"failed to create xmlrpc ServerProxy!") 
        
    server.system.listMethods()                                                                                        
    st_time() 
    
    encrypted_account_session = login(server,guest_user,guest_passwd)                                                      
    if encrypted_account_session is None:                                                                                
        st_exit(-1,lines(),"failed to get encrypted guest session!")                                                          
    
    print "Connecting to tecs web with %s ..."%(guest_user)    
    return encrypted_account_session, server                                                                    


# 003 def:upload_img--用户上传image到TECS；
#     arg:$1 addr:IP地址  $2 account:用户  $3 img:映像名
#     return:$1 encrypted_account_session  $2 server  $3 image_id
#     style:upload_img(addr,account,img)
def upload_img(addr,account,img):
    image_home = "/home/ci_server/images/"
    test_image = image_home + img
		    
    encrypted_account_session, server = login_user(addr,account)
    
    i = image(server,encrypted_account_session)
    image_id = i.get_id_by_filename(img)
    if image_id is None:
        upload_image_file(addr,account,account,test_image)
        image_id = i.get_id_by_filename(img)
        
    return encrypted_account_session, server, image_id

    
# 004 def:reg_img--用户注册image到TECS；
#     arg:$1 addr:IP地址  $2 account:用户  $3 img:映像名
#     return:$1 encrypted_account_session  $2 server  $3 image_id
#     style:reg_img(addr,account,img)
def reg_img(addr,account,img):   
    img_name = img.partition('.')[0]
    img_des = img_name + " system image!"
    
    encrypted_account_session, server, image_id = upload_img(addr,account,img)

    i = image(server,encrypted_account_session)
    if i.query_state_by_id(image_id) != 1:
        #id,name,type,arch,is_public,is_available,description
        if i.register(image_id,img_name,"machine","x86_64",False,True,img_des) != True:
            st_exit(-1,lines(),"failed to register new image!")

    return encrypted_account_session, server, image_id


# 005 def:cfg_vm--用户配置虚拟机到TECS；
#     arg:$1 addr:IP地址  $2 account:用户  $3 vmname:虚拟机名称  
#         $4 img:映像名  $5 num:数量
#     return:$1 encrypted_account_session  $2 server  $3 vm_id
#     style:cfg_vm(addr,account,vmname,img,num)
def cfg_vm(addr,account,vmname,img,num):
    default_group = "default"
    default_vcpu = 2
    default_mem = long(128 * 1024 * 1024)

    encrypted_account_session, server, image_id = reg_img(addr,account,img)
    cfg = {}
    cfg["vm_name"] = vmname
    cfg["project_name"] = default_group
    cfg["vm_num"] = num
    cfg["vcpu"] = default_vcpu
    if img.find('tty',0) == -1:
        cfg["memory"] = default_mem * 4
    else:
        cfg["memory"] = default_mem
    cfg["machine"] = {"id":image_id,"size":0L,"bus":"ide","type":"machine","target":"/dev/hda"}
    cfg["kernel"] = {"id":-1L,"size":0L}
    cfg["ramdisk"] = {"id":-1L,"size":0L}
    cfg["virt_type"] = "hvm"
    #cfg["root_device"] = 
    #cfg["kernel_command"] = 
    #cfg["rawdata"] = 

    v = vmcfg(server,encrypted_account_session)
    vm_id = v.get_vm_id(vmname)
    if vm_id is None:
        p = project(server,encrypted_account_session)
        if p.query_count(0L,100L,-5L,default_group,account) != 1:    
            if p.allocate(default_group,"sytem test default project description") != True:
                st_exit(-1,lines(),"failed to create default project!")
        if v.allocate_quick(cfg) != True:
            st_exit(-1,lines(),"failed to config new vm!")
        vm_id = v.get_vm_id(vmname)
        if vm_id is None:
            st_exit(-1,lines(),"failed to get vm id!")
    print "the first vm %s id is: %s"%(vmname,vm_id)
    
    return encrypted_account_session, server, vm_id

        
    
