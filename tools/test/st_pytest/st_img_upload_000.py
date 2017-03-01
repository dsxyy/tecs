#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：st_img_upload_000.py
# 文件标识：
# 内容摘要：上传映像ttylinux，centos和winxp等3个映像到tc。
# 参数说明：-a address, -u user, -i image
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012/4/17
#*******************************************************************************/
import sys,os,getopt
sys.path.append('/home/ci_server/Tecs_stinit00/ci/pytest')
from contrib import xmlrpclibex
from ftplib import FTP
#from tecs.file import *
from tecs.user import *
from tecs.usergroup import *

#用于测试的tc xmlrpc服务器地址
server_addr = "localhost"
#用于测试的tc xmlrpc服务器端口号
server_port = "8080"

#默认云管理员
cloud_user = "admin"
cloud_passwd = "admin"
#用户账号和密码
st_user = "admin"
st_user_passwd = "admin"
#映像文件存放主目录
image_home = "/home/ci_server/images/"

#用于上传，创建虚拟机的映像文件路径
test_image = "ttylinux.img"

def usage():
    ''' 打印帮助信息'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -u,--user, the tecs user name"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)
    
def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)

def upload_image_file(server,user_name,password,image):
    ftp = FTP()
    try:
        ftp.connect(server,21)
        ftp.login(user_name,password)
    except:
        ci_exit(-1,"ftp login failed!")

    print ftp.getwelcome()
    try:
        cmd='%s %s' % ("stor",os.path.basename(image))
        ftp.storbinary(cmd,open(image,'rb'),1024)
    except:
       ci_exit(-1,"ftp upload failed!")

# 解析处理命令行参数
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user=","image="])
except getopt.GetoptError:
    # print help information and exit:
    usage()
    ci_exit(-2,"GetoptError")

for o, v in opts:
    #用-h或--help打印帮助
    if o in ("-h", "--help"):
        usage()
    #用-a或--addr可以指定tc的xmlrpc服务器地址，默认为localhost
    if o in ("-a", "--addr"):
        server_addr = v
    if o in ("-u", "--user"):
        st_user = v
        st_user_passwd = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-i", "--image"):
        test_image = image_home + v
        print test_image
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)

        
# 自动化系统测试开始!!!
print "===================TECS CI TEST START==================="   
         
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',cloud_user,':',cloud_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s again..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")
        
# 列出当前支持的所有xmlrpc接口    
server.system.listMethods()
    
# 登录tecs，获取管理员账户加密后的session
encrypted_root_session = login(server,cloud_user,cloud_passwd)
if encrypted_root_session is None:
    ci_exit(-1,"failed to get encrypted root session!")   
    
# 查询用户帐号是否已经注册
u = user(server,encrypted_root_session)
    
    # 创建一个类型为1的用户组，用于容纳普通租户的账号 
    print "=============================================="
    ug = usergroup(server,encrypted_root_session)
    if ug.allocate(guest,1) != True:
        ci_exit(-1)
        
    # 创建一个普通账号用于后续模拟来自租户的操作 
    u = user(server,encrypted_root_session)
    if u.allocate(st_user,st_user_passwd,st_user_passwd,3,guest) != True:
        ci_exit(-1)
    
upload_image_file(server_addr,st_user,st_user_passwd,test_image)

        
    
    
  