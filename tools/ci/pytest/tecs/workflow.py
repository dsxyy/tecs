#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：port.py
# 测试内容：port模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张业兵
# 完成日期：2012/04/10
#
# 修改记录1：
#    修改日期：2012/04/10
#    版 本 号：V1.0
#    修 改 人：张业兵
#    修改内容：创建
#*******************************************************************************/
#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
NO_PORT_NAME = ""
#端口使用情况 0-状态未知 1-使用
PORT_USED_UNKNOWN = 0
PORT_USED_YES = 1

#****************************************************************************#

class worflow:
    ''' tecs worflow methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session
      
    def query(self,workflow_addr):
        result = self.apisvr.tecs.workflow.query(self.session,workflow_addr) 
        if result[0] != 0:
            print "fail to query  %s" % (workflow_addr)
            print "error string = %s" % result[1] 
            return None
        workflow_states =  result[1]["workflow_states"]
        print "'work state %s ' " % (result[1])
        #for ws in workflow_states:
           #print "'work state %s ' " % (ws)
        return workflow_states 
   
