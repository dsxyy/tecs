#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_001.py
# �������ݣ����������������������
# ��ǰ�汾��1.0 
# ��    �ߣ���Т��
# ������ڣ�2012/03/24
#*******************************************************************************/
import sys,os,getopt,time

from  base_function_module.interface.base_interface.rpc.contrib.tecs_xlsx import *
import base_function_module.all_rpc_test
import base_function_module.smoking_function
import base_function_module.vm_action_function_test

# �ĵ����ɺ������
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
