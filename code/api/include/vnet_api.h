/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_api.h
* 文件标识：
* 内容摘要：虚拟网络管理模块对外接口
* 当前版本：1.0
* 作    者：wangazhong
* 完成日期：2011年10月19日
*
* 修改记录1：
*    修改日期：2011/10/19
*    版 本 号：V1.0
*    修 改 人：wangazhong
*    修改内容：创建
*******************************************************************************/
#ifndef TECS_VNET_API_H
#define TECS_VNET_API_H
#include "rpcable.h"
#include "api_error.h"
#include "api_const.h"
#include "api_pub.h"

class VnetNetplane: public Rpcable
{
public:
    VnetNetplane() {};
    VnetNetplane(int _netplane_id, string _netplane_name)
    {
        netplane_id   = _netplane_id;
        netplane_name = _netplane_name;
    };
    ~VnetNetplane() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(netplane_id);
        TO_VALUE(netplane_name);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(netplane_id);
        FROM_VALUE(netplane_name);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     网络平面id
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     netplane_id;

    /**
    @brief 参数描述:     网络平面name
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  netplane_name;
};


class VnetNetplaneList: public Rpcable
{
public:
    VnetNetplaneList() {};
    ~VnetNetplaneList() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(netplane_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(netplane_list);
        FROM_RPC_END();
    };

public:
    int AddNetPlane(const VnetNetplane &cData)
    {
        netplane_list.push_back(cData);
        return 0;
    }
    
    vector <VnetNetplane> & GetData(void){ return netplane_list; }
    

private:
    /**
    @brief 参数描述:     网络平面id
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetNetplane>    netplane_list;
};


class VnetNetplaneMap: public Rpcable
{
public:
    VnetNetplaneMap() {};
    ~VnetNetplaneMap() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(phynic_name);
        TO_VALUE(netplane_name);
        TO_VALUE(priority);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(phynic_name);
        FROM_VALUE(netplane_name);
        FROM_VALUE(priority);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     物理网卡名
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  phynic_name;

    /**
    @brief 参数描述:     网络平面name
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  netplane_name;

    /**
    @brief 参数描述:     配置优先级
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     priority;
};


class VnetNetplaneMapList: public Rpcable
{
public:
    VnetNetplaneMapList() {};
    ~VnetNetplaneMapList() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(host_name);
        TO_STRUCT_ARRAY(netplane_map_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(host_name);
        FROM_STRUCT_ARRAY(netplane_map_list);
        FROM_RPC_END();
    };

public:
    int AddNetPlaneMap(const VnetNetplaneMap &cData)
    {
        netplane_map_list.push_back(cData);
        return 0;
    }
    
    vector <VnetNetplaneMap> & GetData(void){ return netplane_map_list; }
    

public:
    /**
    @brief 参数描述:     主机name
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  host_name;

private:
    /**
    @brief 参数描述:     网络平面列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetNetplaneMap>    netplane_map_list;
};


class VnetNetplaneMapLists: public Rpcable
{
public:
    VnetNetplaneMapLists() {};
    ~VnetNetplaneMapLists() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(netplane_map_lists);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(netplane_map_lists);
        FROM_RPC_END();
    };

public:
    int AddNetPlaneMapList(const VnetNetplaneMapList &cData)
    {
        netplane_map_lists.push_back(cData);
        return 0;
    }
    
    vector <VnetNetplaneMapList> & GetData(void){ return netplane_map_lists; }
    

private:
    /**
    @brief 参数描述:     主机网络平面对应关系列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetNetplaneMapList>    netplane_map_lists;
};


class VnetVlanMap: public Rpcable
{
public:
    VnetVlanMap() {};
    ~VnetVlanMap() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(project_id);
        TO_VALUE(user_vid);
        TO_VALUE(cvid);
        TO_VALUE(svid);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(project_id);
        FROM_VALUE(user_vid);
        FROM_VALUE(cvid);
        FROM_VALUE(svid);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     项目id
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    long long  project_id;

    /**
    @brief 参数描述:     用户配置的vlanid
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     user_vid;
    
    /**
    @brief 参数描述:     系统自动分配的vlanid
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明:     系统自动分配的，无需用户关心
    */          
    int     cvid;

    /**
    @brief 参数描述:     外层vlanid，多通道时使用.
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明:     系统自动分配的，无需用户关心
    */          
    int     svid;
};


class VnetVlanMapList: public Rpcable
{
public:
    VnetVlanMapList() {};
    ~VnetVlanMapList() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(netplane_name);
        TO_STRUCT_ARRAY(vlan_map_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(netplane_name);
        FROM_STRUCT_ARRAY(vlan_map_list);
        FROM_RPC_END();
    };

public:
    int AddVlanMap(const VnetVlanMap &cData)
    {
        vlan_map_list.push_back(cData);
        return 0;
    }
    
    vector <VnetVlanMap> & GetData(void){ return vlan_map_list; }
    

public:
    /**
    @brief 参数描述:     网络平面name
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string                 netplane_name;

private:
    /**
    @brief 参数描述:     vlan信息列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetVlanMap>    vlan_map_list;
};


class VnetVlanMapLists: public Rpcable
{
public:
    VnetVlanMapLists() {};
    ~VnetVlanMapLists() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(vlan_map_lists);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(vlan_map_lists);
        FROM_RPC_END();
    };

public:
    int AddVlanMap(const VnetVlanMapList &cData)
    {
        vlan_map_lists.push_back(cData);
        return 0;
    }
    
    vector <VnetVlanMapList> & GetData(void){ return vlan_map_lists; }
    

private:
    /**
    @brief 参数描述:     各个主机vlan信息对应关系列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetVlanMapList>    vlan_map_lists;
};


class VnetIpMac: public Rpcable
{
public:
    VnetIpMac() {};
    VnetIpMac(int _vm_ip, string _vm_mac,int _type)
    {
        vm_ip   = _vm_ip;
        vm_mac  = _vm_mac;
        type = _type;
    };
    ~VnetIpMac() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(vm_ip);
        TO_VALUE(vm_mac);
        TO_VALUE(type);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(vm_ip);
        FROM_VALUE(vm_mac);
        FROM_VALUE(type);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     虚拟机的ip地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     vm_ip;

    /**
    @brief 参数描述:     虚拟机的mac地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  vm_mac;

   /**
    @brief 参数描述:     IP地址类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int  type;
};


class VnetIpMacList: public Rpcable
{
public:
    VnetIpMacList() {};
    ~VnetIpMacList() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(ip_mac_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(ip_mac_list);
        FROM_RPC_END();
    };

public:
    int AddIpMac(const VnetIpMac &cData)
    {
        ip_mac_list.push_back(cData);
        return 0;
    }
    
    vector <VnetIpMac> & GetData(void){ return ip_mac_list; }
    

private:
    /**
    @brief 参数描述:     vm IP/MAC对应列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetIpMac>    ip_mac_list;
};


class VnetDhcpRange: public Rpcable
{
public:
    VnetDhcpRange() {};
    ~VnetDhcpRange() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(ip_start);
        TO_VALUE(ip_end);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(ip_start);
        FROM_VALUE(ip_end);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     range的起始地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     ip_start;

    /**
    @brief 参数描述:     range的结束地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     ip_end;
};


class VnetDhcpSubnet: public Rpcable
{
public:
    VnetDhcpSubnet() {};
    ~VnetDhcpSubnet() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(subnet);
        TO_VALUE(subnet_mask);
        TO_VALUE(gateway);
        TO_VALUE(client_mask);
        TO_STRUCT_ARRAY(dhcp_range_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(subnet);
        FROM_VALUE(subnet_mask);
        FROM_VALUE(gateway);
        FROM_VALUE(client_mask);
        FROM_STRUCT_ARRAY(dhcp_range_list);
        FROM_RPC_END();
    };
    
public:
    int AddDhcpRange(const VnetDhcpRange &cData)
    {
        dhcp_range_list.push_back(cData);
        return 0;
    }
    
    vector <VnetDhcpRange> & GetRangeData(void){ return dhcp_range_list; }

public:
    /**
    @brief 参数描述:     subnet网段
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     subnet;

    /**
    @brief 参数描述:     subnet网段mask
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     subnet_mask;
    
    /**
    @brief 参数描述:     网关地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     gateway;

    /**
    @brief 参数描述:     客户端网段mask
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     client_mask;

private:
    /**
    @brief 参数描述:     subnet下range列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetDhcpRange>   dhcp_range_list;
    
};


class VnetDhcpHost: public Rpcable
{
public:
    VnetDhcpHost() {};
    ~VnetDhcpHost() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(vm_ip);
        TO_VALUE(vm_mac);
        TO_VALUE(gateway);
        TO_VALUE(client_mask);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(vm_ip);
        FROM_VALUE(vm_mac);
        FROM_VALUE(gateway);
        FROM_VALUE(client_mask);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     虚拟机的ip地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     vm_ip;

    /**
    @brief 参数描述:     虚拟机的mac地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  vm_mac;

    /**
    @brief 参数描述:     网关地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     gateway;

    /**
    @brief 参数描述:     客户端网段mask
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     client_mask;
};


class VnetDhcpPxe: public Rpcable
{
public:
    VnetDhcpPxe() {};
    ~VnetDhcpPxe() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(pxe_file);
        TO_VALUE(pxe_server_address);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(pxe_file);
        FROM_VALUE(pxe_server_address);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     PXE 文件
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     pxe_file;

    /**
    @brief 参数描述:     PXE 服务器IP地址
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int  pxe_server_address;

};


class VnetDhcpVlan: public Rpcable
{
public:
    VnetDhcpVlan() {};
    ~VnetDhcpVlan() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(cvid);
        TO_STRUCT_ARRAY(dhcp_subnet_list);
        TO_STRUCT_ARRAY(dhcp_host_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(cvid);
        FROM_STRUCT_ARRAY(dhcp_subnet_list);
        FROM_STRUCT_ARRAY(dhcp_host_list);
        FROM_RPC_END();
    };
    
public:
    int AddDhcpSubnet(const VnetDhcpSubnet &cData)
    {
        dhcp_subnet_list.push_back(cData);
        return 0;
    }
    
    vector <VnetDhcpSubnet> & GetSubnetData(void){ return dhcp_subnet_list; }
    
    int AddDhcpHost(const VnetDhcpHost &cData)
    {
        dhcp_host_list.push_back(cData);
        return 0;
    }
    
    vector <VnetDhcpHost> & GetHostData(void){ return dhcp_host_list; }
    

public:
    /**
    @brief 参数描述:     DHCP只关心cvid
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int     cvid;

private:
    /**
    @brief 参数描述:     vlan下subnet列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetDhcpSubnet>   dhcp_subnet_list;
    
    /**
    @brief 参数描述:     vlan下host列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetDhcpHost>    dhcp_host_list;
    
};


class VnetDhcp: public Rpcable
{
public:
    VnetDhcp() {};
    ~VnetDhcp() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(shell_path);
        TO_VALUE(user_name);
        TO_VALUE(port_name);
        TO_VALUE(pxe_file);
        TO_VALUE(pxe_server_address);
        TO_STRUCT_ARRAY(dhcp_vlan_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(shell_path);
        FROM_VALUE(user_name);
        FROM_VALUE(port_name);
        FROM_VALUE(pxe_file);
        FROM_VALUE(pxe_server_address);
        FROM_STRUCT_ARRAY(dhcp_vlan_list);
        FROM_RPC_END();
    };
    

public:
    int AddDhcpVlan(const VnetDhcpVlan &cData)
    {
        dhcp_vlan_list.push_back(cData);
        return 0;
    }
    
    vector <VnetDhcpVlan> & GetVlanData(void){ return dhcp_vlan_list; }


public:
    /**
    @brief 参数描述:     DHCP Service Shell_path
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string      shell_path;

    /**
    @brief 参数描述:     DHCP Service User Name
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string      user_name;

    /**
    @brief 参数描述:     DHCP Service Port
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string      port_name;

    /**
    @brief 参数描述:     DHCP Service PXE file
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string      pxe_file;

   /**
    @brief 参数描述:     DHCP Service PXE server address
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int      pxe_server_address;
private:
    /**
    @brief 参数描述:     DHCP下vlan列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetDhcpVlan>    dhcp_vlan_list;
    
};

class VnetVlanRange: public Rpcable
{
public:
    VnetVlanRange() {};
    ~VnetVlanRange() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(netplane_name);
        TO_VALUE(vlan_begin);
        TO_VALUE(vlan_end);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(netplane_name);
        FROM_VALUE(vlan_begin);
        FROM_VALUE(vlan_end);
        FROM_RPC_END();
    };
    

public:
    /**
    @brief 参数描述:     物理平面名
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  netplane_name;

    /**
    @brief 参数描述:     vlan范围开始
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int  vlan_begin;

    /**
    @brief 参数描述:     vlan范围结束
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    int  vlan_end;
};

class VnetVlanRangeList: public Rpcable
{
public:
    VnetVlanRangeList() {};
    ~VnetVlanRangeList() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(netplane_name);
        TO_STRUCT_ARRAY(vlan_range_list);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(netplane_name);
        FROM_STRUCT_ARRAY(vlan_range_list);
        FROM_RPC_END();
    };

public:
    int AddVlanRange(const VnetVlanRange &cData)
    {
        vlan_range_list.push_back(cData);
        return 0;
    }
    
    vector <VnetVlanRange> & GetData(void){ return vlan_range_list; }
    

public:
    /**
    @brief 参数描述:     物理平面名
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    string  netplane_name;

private:
    /**
    @brief 参数描述:     VLAN范围列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetVlanRange>    vlan_range_list;
};


class VnetVlanRangeLists: public Rpcable
{
public:
    VnetVlanRangeLists() {};
    ~VnetVlanRangeLists() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(vlan_range_lists);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(vlan_range_lists);
        FROM_RPC_END();
    };

public:
    int AddVlanRangeList(const VnetVlanRangeList &cData)
    {
        vlan_range_lists.push_back(cData);
        return 0;
    }
    
    vector <VnetVlanRangeList> & GetData(void){ return vlan_range_lists; }
    

private:
    /**
    @brief 参数描述:     网络平面VLAN范围列表
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 
    @li @b 其它说明: 
    */          
    vector<VnetVlanRangeList>    vlan_range_lists;
};

class CVnetPortGroupInfo : public Rpcable
{
public:
    string pg_name;
    string pg_uuid;
    int32  pg_type;
    int32  switch_port_mode;   // 0--access, 1--trunk
    int32  is_cfg_netplane;
    string  netplane_uuid;
    int32  mtu;
    int32  access_vlan_num;
    int32  access_isolate_num; 
    int32  trunk_native_vlan;
    int32  isolate_type;
    int32  segment_id;
    int32  m_nVxlanIsolateNo;
    int32  m_nIsSdn;

    CVnetPortGroupInfo()
    {
    }
    ~CVnetPortGroupInfo()
    {
    }

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(pg_name);
        TO_VALUE(pg_uuid);
        TO_VALUE(pg_type);
        TO_VALUE(switch_port_mode);
        TO_VALUE(is_cfg_netplane);
        TO_VALUE(netplane_uuid);
        TO_VALUE(mtu);
        TO_VALUE(access_vlan_num);
        TO_VALUE(access_isolate_num);
        TO_VALUE(trunk_native_vlan);
        TO_VALUE(isolate_type);
        TO_VALUE(segment_id);
        TO_VALUE(m_nVxlanIsolateNo);
        TO_VALUE(m_nIsSdn);

        TO_RPC_END();
    }
    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(pg_name);
        FROM_VALUE(pg_uuid);
        FROM_VALUE(pg_type);
        FROM_VALUE(switch_port_mode);
        FROM_VALUE(is_cfg_netplane);
        FROM_VALUE(netplane_uuid);
        FROM_VALUE(mtu);
        FROM_VALUE(access_vlan_num);
        FROM_VALUE(access_isolate_num);
        FROM_VALUE(trunk_native_vlan);
        FROM_VALUE(isolate_type);
        FROM_VALUE(segment_id);
        FROM_VALUE(m_nVxlanIsolateNo);
        FROM_VALUE(m_nIsSdn);
        FROM_RPC_END();
    };

};


#endif

