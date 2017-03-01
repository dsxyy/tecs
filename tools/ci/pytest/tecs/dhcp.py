#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�dhcp.py
# �������ݣ�dhcpģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ���ѩ��
# ������ڣ�2012/04/12
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/04/12
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ѩ��
#    �޸����ݣ�����
#
# �޸ļ�¼2��
#    �޸����ڣ�2012/04/17
#    �� �� �ţ�V1.1
#    �� �� �ˣ���־ΰ
#    �޸����ݣ�����PXE,RANGE,SUBNET�Ƚӿ�
#*******************************************************************************/
def Ip2Int(ip):
    import struct,socket
    value = (struct.unpack("!I",socket.inet_aton(ip))[0])
    if (value >= 0x40000000):
        value = value -0x100000000
    return value

def Int2Ip(i):
    import socket,struct
    if (i<0):
        i=i+0x100000000	
    return (socket.inet_ntoa(struct.pack("!I",i)))	
	
class dhcp:
    ''' tecs dhcp methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session  
    
    def set_pxe(self,clustername,pxe_file,pxe_server_address):
        a = Ip2Int(pxe_server_address)
        result = self.apisvr.tecs.dhcp_pxe.set(self.session,clustername,pxe_file,a) 
        if result[0] == 0:
            print "dhcp_pxe:set pxe %s ,service address:%s success!" % (pxe_file,pxe_server_address)
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False  
			
    def get_pxe_file(self,start_index,query_count,clustername,vlanid):
        result = self.apisvr.tecs.dhcp.query(self.session,start_index,query_count,clustername,vlanid)
        if result[0] != 0:
            print "failed to get dhcp pxe file for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to get pxe for cluster:%s!" %(clustername)
            VnetDhcp = result[3]
            serveraddr = Int2Ip(VnetDhcp["pxe_server_address"])			
            return 	VnetDhcp["pxe_file"]

    def get_pxe_serveraddr(self,start_index,query_count,clustername,vlanid):
        result = self.apisvr.tecs.dhcp.query(self.session,start_index,query_count,clustername,vlanid) 
        if result[0] != 0:
            print "failed to get dhcp pxe server addr for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to get pxe for cluster:%s!" %(clustername)
            VnetDhcp = result[3]
            serveraddr = Int2Ip(VnetDhcp["pxe_server_address"])
            print "pxe server address %s" % serveraddr				
            return 	serveraddr
			
    def set_range(self,clustername,vlan,subnet,mask,ip_start,ip_end):
        int_subnet = Ip2Int(subnet)
        int_mask = Ip2Int(mask)
        int_ip_start = Ip2Int(ip_start)
        int_ip_end = Ip2Int(ip_end)
        result = self.apisvr.tecs.dhcp_range.set(self.session,clustername,vlan,int_subnet,int_mask,int_ip_start,int_ip_end) 
        if result[0] == 0:
            print "dhcp_pxe:set range vlan %d subnet %s ,mask %s,ip start %s ,ip end %s success!" % (vlan,int_subnet,int_mask,int_ip_start,int_ip_end)
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False    
         		
	def get_cvid_list(self,start_index,query_count,clustername,vlanid):
	    result = self.apisvr.tecs.dhcp.query(self.session,start_index,query_count,clustername,vlanid) 
        if result[0] != 0:
            print "failed to get dhcp ip range for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to get dhcp range for cluster:%s!" %(clustername)
            print " next index %d" % result[1]
            print " maxcount %d" % result[2]
            VnetDhcp = result[3]
            if len(VnetDhcp["dhcp_vlan_list"]) == 0:
			    return None
            '''  ע�������VnetDhcp["dhcp_vlan_list"] ��һ������'''	
            dhcp_vlan_list = VnetDhcp["dhcp_vlan_list"]				
            if len(dhcp_vlan_list) == 0:
		        return None
            vid_list = []
            for d in dhcp_vlan_list:
                vid_list.append(d["cvid"])
            return vid_list
			
    ''' ����ֵ��2ά���飬ÿ������Ԫ���� subnet subnetmask gateway clientmask��ɵ�����'''	
    def get_subnet_turple_List(self,start_index,query_count,clustername,vlanid):
        result = self.apisvr.tecs.dhcp.query(self.session,start_index,query_count,clustername,vlanid) 
        if result[0] != 0:
            print "failed to get dhcp pxe server addr for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to get_subnet_turple_List  for cluster:%s!" %(clustername)
            VnetDhcp = result[3]
            dhcp_vlan_list = VnetDhcp["dhcp_vlan_list"] #dhcp_vlan_list��һ������
            a_subnet_turple = []
            subnet_turple_list = []
            for d in dhcp_vlan_list:
                dhcp_subnet_list = d["dhcp_subnet_list"]#dhcp_subnet_listҲ��һ������
                for k in dhcp_subnet_list:
                    subnet = Int2Ip(k["subnet"])
                    subnet_mask = Int2Ip(k["subnet_mask"])
                    gateway =  Int2Ip(k["gateway"])
                    client_mask = Int2Ip(k["client_mask"])
                    a_subnet_turple.append(subnet)
                    a_subnet_turple.append(subnet_mask)
                    a_subnet_turple.append(gateway)
                    a_subnet_turple.append(client_mask)
                    subnet_turple_list.append(a_subnet_turple)
                    a_subnet_turple = [] #��յ�for����һ�θ�ֵ'''
            return subnet_turple_list		
     
    def get_range_turple_list(self,start_index,query_count,clustername,vlanid):
        result = self.apisvr.tecs.dhcp.query(self.session,start_index,query_count,clustername,vlanid) 
        if result[0] != 0:
            print "failed to get dhcp pxe server addr for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to get pxe for cluster:%s!" %(clustername)
            VnetDhcp = result[3]
            dhcp_vlan_list = VnetDhcp["dhcp_vlan_list"] #dhcp_vlan_list��һ������
            a_range_turple = []
            range_turple_list = []
            for d in dhcp_vlan_list:
                dhcp_subnet_list = d["dhcp_subnet_list"]
                for k in dhcp_subnet_list:
                    dhcp_range_list = k["dhcp_range_list"]
                    for m in dhcp_range_list:
                        ip_start = Int2Ip(m["ip_start"])
                        ip_end = Int2Ip(m["ip_end"])
                        a_range_turple.append(ip_start)
                        a_range_turple.append(ip_end)
                        range_turple_list.append(a_range_turple)
                        a_range_turple = [] #��յ�for����һ�θ�ֵ
            return range_turple_list	 
        	
			
    def set_subnet(self,clustername,vlan,subnet,submask,gateway,gatemask):
        int_subnet = Ip2Int(subnet)
        int_submask = Ip2Int(submask)
        int_gateway = Ip2Int(gateway)
        int_gatemask = Ip2Int(gatemask)
        result = self.apisvr.tecs.dhcp_subnet.set(self.session,clustername,vlan,int_subnet,int_submask,int_gateway,int_gatemask) 
        if result[0] != 0:
            print "failed to set dhcp subnet for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set subnet for cluster:%s!" %(clustername)
            return True  
					
    def delete_subnet(self,clustername,vlan,subnet,submask):
        int_subnet = Ip2Int(subnet)
        int_submask = Ip2Int(submask)
        result = self.apisvr.tecs.dhcp_subnet.delete(self.session,clustername,vlan,int_subnet,int_submask) 
        if result[0] != 0:
            print "failed to delete dhcp subnet for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete subnet for cluster:%s,subnet addr is %s!" %(clustername,subnet)
            return True 
    
    def get_dhcp_port_name(self,clustername,vlanid):
        ''' ����ָ���ļ�Ⱥ����vlan id��ȡ��ǰport name���  '''
        result = self.apisvr.tecs.dhcp.query(self.session,1L,100L,clustername,mvlanid) 
        if result[0] != 0:
            print "failed to query dhcp information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            if len(result[3]) == 0:
                print "No dhcp cfg info"
                return None
                
            dhcp_vlan_list = result[3]
            for d in dhcp_vlan_list:
                if len(d["port_name"]) == 0:
                    break
                return d["port_name"]
            return None
					
    def start(self,clustername):
        ''' ����ָ���ļ�Ⱥ������dhcp '''
        result = self.apisvr.tecs.dhcp.start(self.session,clustername) 
        if result[0] != 0:
            print "failed to start dhcp for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to start dhcp for cluster:%s" %(clustername)
            return True  

    def stop(self,clustername):
        ''' ����ָ���ļ�Ⱥ��ֹͣdhcp '''
        result = self.apisvr.tecs.dhcp.stop(self.session,clustername) 
        if result[0] != 0:
            print "failed to stop dhcp for cluster:%s! error code = %d" % (clustername,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to stop dhcp for cluster:%s" %(clustername)
            return True  
    	
    def	get_dhcp_state(self,start_index,query_count,clustername):
        ''' ��ѯDHCP��״̬'''
        result = self.apisvr.tecs.dhcp.query(self.session,start_index,query_count,clustername) 
        if result[0] != 0:
            print "failed to query dhcp state information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            return result[3]
			