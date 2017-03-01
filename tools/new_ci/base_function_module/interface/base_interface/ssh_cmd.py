#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�ssh.py
# �������ݣ�vlanģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ�����
# ������ڣ�2012/08/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/08/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ�����
#    �޸����ݣ�����
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

    #����˵�����������ʺ��硰xm list������xm sched-credit�������������ִ�н��
    #���Ԫ��ϵ��һ�����磺xm listִ�н����
    #Name         ID   Mem    VCPUs     State    Time(s)
    #Domain-0     0    1024   2        r-----   5970.4
    #instance_1   1    1024   1        -b----     44.3
    #��Σ�
    #     cmd_str - shell����,��xm list
    #     key_str - ��Ϊ�������ݵ�key����Name
    #     value_str - ������ĳһԪ���е�ֵ����instance_1(Ҫ���ҵ�Ŀ����ڸ�Ԫ����)
    #����ֵ���������б��б��ÿһ��Ԫ�ض����ֵ�ṹ���ֵ��key������ͬ�ģ�Ϊ��Ԫ
    #��ϵ�����������valueΪ���ҵ�����������ÿһ�У�
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
        
    #����ifconfig�����ĳ��������mtuֵ
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
    
    # ִ�м򵥵���������׼���
    def do_cmd(self, cmd_str):
        result = []
        print cmd_str
        stdin, stdout, stderr = self.client.exec_command(cmd_str)
        for i_line in stdout.readlines():
            result.append(i_line)
        return result

        
