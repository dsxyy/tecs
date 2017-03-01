#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：tecs_common.py
# 测试内容：
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/11/27
#*******************************************************************************/
import sys,os,time

from tecs_common import *
from  openpyxl.reader.excel  import  load_workbook

from  openpyxl.workbook  import  Workbook   
#万恶的ExcelWriter，妹的封装好了不早说，封装了很强大的excel写的功能    
from  openpyxl.writer.excel  import  ExcelWriter   
#一个eggache的数字转为列字母的方法    
from  openpyxl.cell  import  get_column_letter  

class xlsx_sheet():
    def __init__(self, file_name):
        # 先检测文件是否存在
        try:
            wb = load_workbook(filename = file_name + '.xlsx')
        #print "load workbook success"
        except:    
            wb = Workbook()

        if wb == None:
            wb = Workbook()
        
        self.wb = wb
        # 建立一个excel的写入器    
        ew = ExcelWriter(workbook = wb)
        self.ew = ew
        
        # 设置输出路径
        dest_filename = file_name + '.xlsx'
        self.file_name = dest_filename
        

           
    def open(self, sheet_name):
        self.sheet_name = sheet_name
        # 检测表是否存在
        ws = self.wb.get_sheet_by_name(sheet_name.decode("gb2312"))
        self.ws = ws
        if ws == None:
            if self.wb.worksheets[0].title == "Sheet":
                ws = self.wb.worksheets[0]
            else:
                ws = self.wb.create_sheet()
            ws.title = sheet_name.decode("gb2312")
            
            ws.cell(row=0, column=0).value = "一级测试条目".decode("gb2312")
            ws.cell(row=0, column=1).value = "二级测试条目".decode("gb2312")
            ws.cell(row=0, column=2).value = "测试意图".decode("gb2312")
            ws.cell(row=0, column=3).value = "测试流程".decode("gb2312")
            ws.cell(row=0, column=4).value = "是否实现".decode("gb2312")
            self.ws = ws
           
    def __del__(self):
        self.ew.save(filename = self.file_name)
        
    def write_xlsx_columu(self, column_n, write_num, record1="NULL", record2="NULL", recode3="NULL"):
        if write_num == 1:
            self.ws.cell(row=self.ws.get_highest_row(), column=column_n).value = record1.decode("gb2312")
        else:
            # 指定列写入数据
            write_row = self.ws.get_highest_row()
            self.ws.cell(row=write_row, column=column_n).value = record1.decode("gb2312")
            self.ws.cell(row=write_row, column=column_n+1).value = record2.decode("gb2312")
            self.ws.cell(row=write_row, column=column_n+2).value = recode3.decode("gb2312")

    # 写入一级测试条目函数    
    def write_level_1(self, level1):    
        self.write_xlsx_columu(0, 1, level1)

    # 写入二级测试条目函数     
    def write_level_2(self, level2):    
        self.write_xlsx_columu(1, 1, level2)    

    # 写入测试意图和测试步骤
    def write_level_3(self, test_mean, test_step, is_do):    
        self.write_xlsx_columu(2, 3, test_mean, test_step, is_do)   

class cidoc():
    def __init__(self, file_name):
        # 先获取EXCEL的句柄
        self.xlsx = xlsx_sheet(file_name)
        
    def write_doc(self, test_class, sheet_name):
        '''把测试用例打印输出出去，可以从外面看到当前进行了那些测试'''
        self.xlsx.open(sheet_name)
        self.xlsx.write_level_1(test_class.__doc__.replace(" ", "").lstrip())
        num = 0
        for k,v in  test_class.__dict__.items():
            if type(getattr(test_class,k)) == type(test_class.__init__) and k.find("test_level_2") != -1:
                if getattr(test_class,k).__doc__ == None:
                    ci_exit(-1,"%s %s: find test function does not comment!" % (test_class, k))
                self.xlsx.write_level_2(getattr(test_class,k).__doc__.replace(" ", "").lstrip())
                level_2_name = str(k)
                level_3_name = level_2_name[:len(level_2_name)-1] + '3'
                for k1,v1 in  test_class.__dict__.items():
                    if type(getattr(test_class,k1)) == type(test_class.__init__) and k1.find(level_3_name) != -1:
                        # 获取注释字符串
                        level_3_doc = getattr(test_class,k1).__doc__
                        if level_3_doc == None:
                            ci_exit(-1,"%s %s:find test function does not comment!" % (test_class,k1))
                        # 检测注释的格式是否符合条件
                        if level_3_doc.count("：") != 3:
                            ci_exit_ex(-1, line(), "%s %s: test note format error!" % (test_class,k1))
                        # 获取是否实现
                        start_postion =  level_3_doc.find("：") + 2
                        end_postion =  level_3_doc.find("\n", start_postion)
                        is_do = level_3_doc[start_postion:end_postion].lstrip()
                        # 获取测试意图                    
                        start_postion =  level_3_doc.find("：", end_postion) + 2
                        end_postion =  level_3_doc.find("：", start_postion) - 8
                        test_mean = level_3_doc[start_postion:end_postion].replace(" ", "").lstrip()
                        # 获取测试步骤
                        start_postion =  level_3_doc.find("：", end_postion) + 2
                        test_step = level_3_doc[start_postion:].replace(" ", "").lstrip()
                        # 写入EXCEL
                        self.xlsx.write_level_3(test_mean, test_step, is_do) 
                        num += 1
                        print "write test itme doc %d" % num                    

        

        