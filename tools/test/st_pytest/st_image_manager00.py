#!/usr/bin/python
# -*- coding: gb2312 -*-
#------------------------------------------------------------------------------
# 文件名称：st_image_manager00.py
# 内容摘要：映像管理模块的公共基本流程。
# 作    者：XXX
# 完成日期：2012/4/17
#*******************************************************************************/
import sys,os,getopt,time,inspect,hashlib
import __builtin__
__builtin__.LOG = "localhost"
from st_comm_lib import *

#tecs目标测试机地址和用户            
server_addr = "localhost"
account_user = "admin"
img_name = "ttylinux0.img"
img_name1 = "windowsxp_0406.img"
img_name2 = "centos6.2_hvm_0405.img"

# 参数解析
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:u:i:v", ["help", "addr=","user="])
except getopt.GetoptError:
    st_exit(-2,lines(),"Get args Error!")    
for o, v in opts:
    if o in ("-a", "--addr"):
        server_addr = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-u", "--user"):
        account_user = v
__builtin__.LOG = server_addr.replace('.','_')

#登录映像管理模块
encrypted_account_session, server = login_user(server_addr,account_user)
i = image(server,encrypted_account_session)
##############################################################
##############################################################
st_log(2,'',"START")

#STC-F-TECS_IMAGE-01001 云管理员admin上传ttylinux映像注册
#用例开始
result = True
#image_id--ttylinux映像的ID
encrypted_account_session, server,image_id = upload_img(server_addr,account_user,img_name)
#查询映像是否上传成功
if i.get_id_by_filename(img_name) == None:
    st_log(-1, lines(), "STC-F-TECS_IMAGE-01001: failed to register ttylinux image!")
    result = False
#用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_IMAGE-01001: success to register ttylinux image!")
#用例结束


#STC-F-TECS_IMAGE-01002 云管理员admin上传windowsxp映像注册
#用例开始
result = True
#image_id1--windowsxp映像的ID
encrypted_account_session, server,image_id1 = upload_img(server_addr,account_user,img_name1)
#查询映像是否上传成功
if i.get_id_by_filename(img_name1) == None:
    st_log(-1, lines(), "STC-F-TECS_IMAGE-01001: failed to register windowsxp image!")
    result = False
#用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_IMAGE-01002: success to register windowsxp image!")
#用例结束


#STC-F-TECS_IMAGE-01003 云管理员admin上传centos映像注册
#用例开始
result = True
#image_id1--windowsxp映像的ID
encrypted_account_session, server,image_id1 = upload_img(server_addr,account_user,img_name2)
#查询映像是否上传成功
if i.get_id_by_filename(img_name2) == None:
    st_log(-1, lines(), "STC-F-TECS_IMAGE-01001: failed to register centos image!")
    result = False
#用例执行成功，调用st_log(1)记录执行结果
if result == True:
    st_log(1,lines(),"STC-F-TECS_IMAGE-01002: success to register centos image!")
#用例结束

st_log(2,'',"END")
