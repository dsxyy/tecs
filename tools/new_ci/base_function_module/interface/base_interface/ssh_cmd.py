#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：ssh.py
# 测试内容：vlan模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：高明
# 完成日期：2012/08/24
#
# 修改记录1：
#    修改日期：2012/08/24
#    版 本 号：V1.0
#    修 改 人：高明
#    修改内容：创建
#*******************************************************************************/
import paramiko
import re

class ssh_cmd:
    ''' tecs ssh_cmd methods '''
    def __init__(self,server_addr, port, pwd): 
        self.client = paramiko.SSHClient()    
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy()) 
        self.client.connect(server_addr,port,username='root', password=pwd) 
        
    def __del__(self):
        self.client.close()

    #函数说明：本函数适合如“xm list”、“xm sched-credit”这样的命令，其执行结果
    #像二元关系表一样，如：xm list执行结果：
    #Name         ID   Mem    VCPUs     State    Time(s)
    #Domain-0     0    1024   2        r-----   5970.4
    #instance_1   1    1024   1        -b----     44.3
    #入参：
    #     cmd_str - shell命令,如xm list
    #     key_str - 作为查找依据的key，如Name
    #     value_str - 主键在某一元组中的值，如instance_1(要查找的目标就在该元组中)
    #返回值：类型是列表，列表的每一个元素都是字典结构，字典的key都是相同的，为二元
    #关系表的属性名，value为查找到符合条件的每一行；
    def xm_cmd(self,cmd_str,key_str,value_str):
        count =0
        tgt_list =[]
        stdin, stdout, stderr = self.client.exec_command(cmd_str) 
        for i_line in stdout.readlines(): 
            count = count+1
            if count == 1:
                key_list = i_line.split()
            else:
                val_list = i_line.split()
                tgt_dict = dict(zip(key_list,val_list))          
                if tgt_dict[key_str] == value_str:
                    tgt_list.append(tgt_dict)  
        return tgt_list
        
    #查找ifconfig结果中某个网卡的mtu值
    def ifconfig_MTU(self,nic_name_str):
        pattern = re.compile(r'^%s' %nic_name_str)
        match = None
        stdin, stdout, stderr = self.client.exec_command('ifconfig') 
        for i_line in stdout.readlines():
            if match == None:
                match = pattern.match(i_line)            
            if match:          
                 find = re.findall(r'MTU:(\d*)',i_line)
                 if find != []:
                     return (int(find[0]))
        return None
    
    # 执行简单的命令，输出标准输出
    def do_cmd(self, cmd_str):
        result = []
        print cmd_str
        stdin, stdout, stderr = self.client.exec_command(cmd_str)
        for i_line in stdout.readlines():
            result.append(i_line)
        return result

        
