#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_001.py
# 测试内容：测试虚拟机创建部署流程
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/03/24
#*******************************************************************************/
import sys,os,getopt,time

from  base_function_module.interface.base_interface.rpc.contrib.tecs_xlsx import *
import base_function_module.all_rpc_test
import base_function_module.smoking_function
import base_function_module.vm_action_function_test

# 文档生成函数入口
class MakeDoc():
    def __init__(self):
        save_file = "tecs_ci_test"
        self.doc_ctrl = cidoc(save_file)
        pass;
        
    def GetDoc(self):
        base_function_module.all_rpc_test.DocEntery(self.doc_ctrl)
        base_function_module.smoking_function.DocEntery(self.doc_ctrl)
        base_function_module.vm_action_function_test.DocEntery(self.doc_ctrl)




if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        MakeDoc().GetDoc()
