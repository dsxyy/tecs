/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_portinfo.h
* 文件标识：
* 内容摘要：VNA端口信息
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年2月22日
********************************************************************************/
#ifndef VNET_PORTINFO_H
#define VNET_PORTINFO_H

#include "message.h"
#include "serialization.h"

using namespace std;
namespace zte_tecs
{
/************** basic port info ********************/
//作为下面类的继承?
class CPortBasicInfo:public Serializable
{
public:
    int32               _vna_flag;
    int32               _port_type;
    string              _name;
    int32               _state;
    int32               _is_broadcast;
    int32               _is_running;
    int32               _is_multicast;
    int32               _promiscuous;
    int32               _mtu;
    int32               _is_master;
    int32               _admin_state;
    string              _ip;
    string              _mask;

    CPortBasicInfo()
    {
        _vna_flag = 0;
        _port_type = 0;
        _name = "";
        _state = 0;
        _is_broadcast = 0;
        _is_running = 0;
        _is_multicast = 0;
        _promiscuous = 0;
        _mtu = 0;
        _is_master = 0;
        _admin_state = 0;
        _ip = "";
        _mask = "";
    };

    void Print()
    {
        cout <<"CPortBasicInfo" <<endl;
        cout <<"    port_name" << _name <<endl;
        cout <<"    port_type" << _port_type <<endl;
        cout <<"    port_state" << _state <<endl;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(CPortBasicInfo);
        WRITE_DIGIT(_vna_flag);
        WRITE_DIGIT(_port_type);
        WRITE_STRING(_name);
        WRITE_DIGIT(_state);
        WRITE_DIGIT(_is_broadcast);
        WRITE_DIGIT(_is_running);
        WRITE_DIGIT(_is_multicast);
        WRITE_DIGIT(_promiscuous);
        WRITE_DIGIT(_mtu);
        WRITE_DIGIT(_is_master);
        WRITE_DIGIT(_admin_state);
        WRITE_STRING(_ip);
        WRITE_STRING(_mask);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CPortBasicInfo);
        READ_DIGIT(_vna_flag);
        READ_DIGIT(_port_type);
        READ_STRING(_name);
        READ_DIGIT(_state);
        READ_DIGIT(_is_broadcast);
        READ_DIGIT(_is_running);
        READ_DIGIT(_is_multicast);
        READ_DIGIT(_promiscuous);
        READ_DIGIT(_mtu);
        READ_DIGIT(_is_master);
        READ_DIGIT(_admin_state);
        READ_STRING(_ip);
        READ_STRING(_mask);
        DESERIALIZE_END();
    };
};


/************** Bond ********************/
class CBondLacpReport:public Serializable
{
public:
    int32               _vna_flag;
    int32               _state;
    string              _aggregate_id;
    string              _partner_mac;
    //注意赋值时候clear
    vector<string>      _suc_nics;
    vector<string>      _fail_nics;

    CBondLacpReport()
    {
        _vna_flag = 0;
        _state = 0;
        _aggregate_id = "";
        _partner_mac = "";
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(CBondLacpReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_DIGIT(_state);
        WRITE_STRING(_aggregate_id);
        WRITE_STRING(_partner_mac);
        WRITE_STRING_VECTOR(_suc_nics);
        WRITE_STRING_VECTOR(_fail_nics);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CBondLacpReport);
        READ_DIGIT(_vna_flag);
        READ_DIGIT(_state);
        READ_STRING(_aggregate_id);
        READ_STRING(_partner_mac);
        READ_STRING_VECTOR(_suc_nics);
        READ_STRING_VECTOR(_fail_nics);
        DESERIALIZE_END();
    };
};


class CBondBackupReport:public Serializable
{
public:
    int32               _vna_flag;
    vector<string>   _other_nic;
    string           _active_nic;

    CBondBackupReport()
    {
        _vna_flag = 0;
        _active_nic = "";
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(CBondBackupReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_STRING_VECTOR(_other_nic);
        WRITE_STRING(_active_nic);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CBondBackupReport);
        READ_DIGIT(_vna_flag);
        READ_STRING_VECTOR(_other_nic);
        READ_STRING(_active_nic);
        DESERIALIZE_END();
    };
};


class CBondReport:public Serializable
{
public:
    int32               _vna_flag;

    CPortBasicInfo _basic_info;

    string             _bond_name;
    int32              _bond_report_mode;

    CBondLacpReport    _bond_lacp;
    CBondBackupReport  _bond_backup;

    CBondReport()
    {
        _vna_flag = 0;
        _bond_name = "";
        _bond_report_mode = 0;

    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CBondReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_OBJECT(_basic_info);
        WRITE_STRING(_bond_name);
        WRITE_DIGIT(_bond_report_mode);
        WRITE_OBJECT(_bond_lacp);
        WRITE_OBJECT(_bond_backup);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CBondReport);
        READ_DIGIT(_vna_flag);
        READ_OBJECT(_basic_info);
        READ_STRING(_bond_name);
        READ_DIGIT(_bond_report_mode);
        READ_OBJECT(_bond_lacp);
        READ_OBJECT(_bond_backup);
        DESERIALIZE_END();
    };
};

/************** SRIOV ********************/
class CSriovPortReport:public Serializable
{
public:
    int32               _vna_flag;

    string _sriov_phyport;
    string _vf;
    string _pci;
    int32  _vlan_num;

    CSriovPortReport()
    {
        _vna_flag = 0;

        _sriov_phyport = "";
        _vf = "";
        _pci = "";
        _vlan_num = 0;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(CSriovPortReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_STRING(_sriov_phyport);
        WRITE_STRING(_vf);
        WRITE_STRING(_pci);
        WRITE_DIGIT(_vlan_num);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CSriovPortReport);
        READ_DIGIT(_vna_flag);
        READ_STRING(_sriov_phyport);
        READ_STRING(_vf);
        READ_STRING(_pci);
        READ_DIGIT(_vlan_num);
        DESERIALIZE_END();
    };
};

/************** PhyPort ********************/
class CPhyPortReport:public Serializable
{
public:
    int32               _vna_flag;

    CPortBasicInfo _basic_info;

    string  _supported_ports;
    string  _supported_link_modes;
    int32   _is_support_auto_negotiation;
    string  _advertised_link_modes;
    string  _advertised_pause_frame_use;
    int32   _is_advertised_auto_negotiation;
    string  _speed;
    string  _duplex;
    string  _port;
    string  _phyad;
    string  _transceiver;
    string  _auto_negotiate;
    string  _supports_wake_on;
    string  _wake_on;
    string  _current_msg_level;
    int32   _is_linked;

    //extend
    int32   _is_rx_checksumming;
    int32   _is_tx_checksumming;
    int32   _is_scatter_gather;
    int32   _is_tcp_segmentation_offload;
    int32   _is_udp_fragmentation_offload;
    int32   _is_generic_segmentation_offload;
    int32   _is_generic_receive_offload;
    int32   _is_large_receive_offload;

    //sriov
    int32   _is_sriov;
    int32   _is_loop_report;
    int32   _is_loop_cfg;
    int32   _total_vf_num;
    int32   _free_vf_num;

    //sriov pci
    vector <CSriovPortReport> _sriov_port;

    CPhyPortReport()
    {
        _vna_flag = 0;
        _supported_ports = "";
        _supported_link_modes = "";
        _is_support_auto_negotiation = 0;
        _advertised_link_modes = "";
        _advertised_pause_frame_use = "";
        _is_advertised_auto_negotiation = 0;


        _speed = "";
        _duplex = "";

        _port = "";
        _phyad = "";
        _transceiver = "";
        _auto_negotiate = "";
        _supports_wake_on = "";
        _wake_on = "";
        _current_msg_level = "";
        _is_linked = 0;

        _is_rx_checksumming = 0;
        _is_tx_checksumming = 0;
        _is_scatter_gather = 0;
        _is_tcp_segmentation_offload = 0;
        _is_udp_fragmentation_offload = 0;
        _is_generic_segmentation_offload = 0;
        _is_generic_receive_offload = 0;
        _is_large_receive_offload = 0;

        _is_sriov = 0;
        _is_loop_report = 0;
        _is_loop_cfg = 0;
        _total_vf_num = 0;
        _free_vf_num = 0;


    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CPhyPortReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_OBJECT(_basic_info);
        WRITE_STRING(_supported_ports);
        WRITE_STRING(_supported_link_modes);
        WRITE_DIGIT(_is_support_auto_negotiation);
        WRITE_STRING(_advertised_link_modes);
        WRITE_STRING(_advertised_pause_frame_use);
        WRITE_DIGIT(_is_advertised_auto_negotiation);
        WRITE_STRING(_speed);
        WRITE_STRING(_duplex);
        WRITE_STRING(_port);
        WRITE_STRING(_phyad);
        WRITE_STRING(_transceiver);
        WRITE_STRING(_auto_negotiate);
        WRITE_STRING(_supports_wake_on);
        WRITE_STRING(_wake_on);
        WRITE_STRING(_current_msg_level);
        WRITE_DIGIT(_is_linked);

        WRITE_DIGIT(_is_rx_checksumming);
        WRITE_DIGIT(_is_tx_checksumming);
        WRITE_DIGIT(_is_scatter_gather);
        WRITE_DIGIT(_is_tcp_segmentation_offload);
        WRITE_DIGIT(_is_udp_fragmentation_offload);
        WRITE_DIGIT(_is_generic_segmentation_offload);
        WRITE_DIGIT(_is_generic_receive_offload);
        WRITE_DIGIT(_is_large_receive_offload);

        WRITE_DIGIT(_is_sriov);
        WRITE_DIGIT(_is_loop_report);
        WRITE_DIGIT(_is_loop_cfg);
        WRITE_DIGIT(_total_vf_num);
        WRITE_DIGIT(_free_vf_num);
        WRITE_OBJECT_VECTOR(_sriov_port);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CPhyPortReport);
        READ_DIGIT(_vna_flag);
        READ_OBJECT(_basic_info);
        READ_STRING(_supported_ports);
        READ_STRING(_supported_link_modes);
        READ_DIGIT(_is_support_auto_negotiation);
        READ_STRING(_advertised_link_modes);
        READ_STRING(_advertised_pause_frame_use);
        READ_DIGIT(_is_advertised_auto_negotiation);
        READ_STRING(_speed);
        READ_STRING(_duplex);
        READ_STRING(_port);
        READ_STRING(_phyad);
        READ_STRING(_transceiver);
        READ_STRING(_auto_negotiate);
        READ_STRING(_supports_wake_on);
        READ_STRING(_wake_on);
        READ_STRING(_current_msg_level);
        READ_DIGIT(_is_linked);

        READ_DIGIT(_is_rx_checksumming);
        READ_DIGIT(_is_tx_checksumming);
        READ_DIGIT(_is_scatter_gather);
        READ_DIGIT(_is_tcp_segmentation_offload);
        READ_DIGIT(_is_udp_fragmentation_offload);
        READ_DIGIT(_is_generic_segmentation_offload);
        READ_DIGIT(_is_generic_receive_offload);
        READ_DIGIT(_is_large_receive_offload);

        READ_DIGIT(_is_sriov);
        READ_DIGIT(_is_loop_report);
        READ_DIGIT(_is_loop_cfg);
        READ_DIGIT(_total_vf_num);
        READ_DIGIT(_free_vf_num);
        READ_OBJECT_VECTOR(_sriov_port);
        DESERIALIZE_END();
    };
};

/***************kernel tap report *********************/
class CKernelPortReport: public Serializable
{
public:
    int32               _vna_flag;

    CPortBasicInfo _basic_info;

    //uplink
    string _name;
    int32 _type;

    //tap
    string _krport_name;
    string _switch_name;
    string _ip;
    string _mask;

    //bond attribute
    int32  _bond_mode;
    vector<string>  _bond_map;

    CKernelPortReport()
    {
        _vna_flag = 0;
        _name = "";
        _type = 0;
        _krport_name = "";
        _switch_name = "";
        _ip ="";
        _mask = "";
        _bond_mode = 0;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CKernelPortReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_OBJECT(_basic_info);
        WRITE_STRING(_name);
        WRITE_DIGIT(_type);
        WRITE_STRING(_krport_name);
        WRITE_STRING(_switch_name);
        WRITE_STRING(_ip);
        WRITE_STRING(_mask);
        WRITE_DIGIT(_bond_mode);
        WRITE_STRING_VECTOR(_bond_map);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CKernelPortReport);
        READ_DIGIT(_vna_flag);
        READ_OBJECT(_basic_info);
        READ_STRING(_name);
        READ_DIGIT(_type);
        READ_STRING(_krport_name);
        READ_STRING(_switch_name);
        READ_STRING(_ip);
        READ_STRING(_mask);
        READ_DIGIT(_bond_mode);
        READ_STRING_VECTOR(_bond_map);
        DESERIALIZE_END();
    };
};

/***************kernel cfg report *********************/
class CKernelPortCfgReport: public Serializable
{
public:
    int32               _vna_flag;

    CPortBasicInfo _basic_info;

    string _krport_name;
    string _switch_name;
    string _ip;
    string _mask;

    CKernelPortCfgReport()
    {
        _vna_flag = 0;
        _krport_name = "";
        _switch_name = "";
        _ip ="";
        _mask = "";
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CKernelPortCfgReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_OBJECT(_basic_info);
        WRITE_STRING(_krport_name);
        WRITE_STRING(_switch_name);
        WRITE_STRING(_ip);
        WRITE_STRING(_mask);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CKernelPortCfgReport);
        READ_DIGIT(_vna_flag);
        READ_OBJECT(_basic_info);
        READ_STRING(_krport_name);
        READ_STRING(_switch_name);
        READ_STRING(_ip);
        READ_STRING(_mask);		
        DESERIALIZE_END();
    };
};

/***************vtep cfg report *********************/
class CVtepPortCfgReport: public Serializable
{
public:
    int32               _vna_flag;
    CPortBasicInfo      _basic_info;

    string _vtep_name;
    string _switch_name;
    string _ip;
    string _mask;

    CVtepPortCfgReport()
    {
        _vna_flag = 0;
        _vtep_name = "";
        _switch_name = "";
        _ip ="";
        _mask = "";
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVtepPortCfgReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_OBJECT(_basic_info);
        WRITE_STRING(_vtep_name);
        WRITE_STRING(_switch_name);
        WRITE_STRING(_ip);
        WRITE_STRING(_mask);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVtepPortCfgReport);
        READ_DIGIT(_vna_flag);
        READ_OBJECT(_basic_info);
        READ_STRING(_vtep_name);
        READ_STRING(_switch_name);
        READ_STRING(_ip);
        READ_STRING(_mask);		
        DESERIALIZE_END();
    };
};

class CUplinkLoopPortReport: public Serializable
{
public:
    int32               _vna_flag;
    CPortBasicInfo      _basic_info;

    CUplinkLoopPortReport()
    {
        _vna_flag = 0;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CUplinkLoopPortReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_OBJECT(_basic_info);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CUplinkLoopPortReport);
        READ_DIGIT(_vna_flag);
        READ_OBJECT(_basic_info);
        DESERIALIZE_END();
    };
};


/************** 端口信息上报 ********************/
class CMessageVNAPortInfoReport : public Serializable
{
public:
    int32                         _vna_flag;
    string                        _vna_id;
    vector<CPhyPortReport>        _phy_port;
    vector<CBondReport>           _bond_info;
    vector<CUplinkLoopPortReport> _uplink_info;
    vector<CKernelPortCfgReport>  _kernel_cfgport;
    vector<CVtepPortCfgReport>    _vtep_cfgport;
    CKernelPortReport             _kernel_port;

    CMessageVNAPortInfoReport()
    {
        _vna_flag = 0;
        _vna_id = "";
    };

    void Print()
    {
        cout <<"CMessageVNAPortInfoReport:" <<endl;
        cout <<"    _vna_flag" << _vna_flag <<endl;
        cout <<"    _vna_id" << _vna_id <<endl;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(CMessageVNAPortInfoReport);
        WRITE_DIGIT(_vna_flag);
        WRITE_STRING(_vna_id);
        WRITE_OBJECT_VECTOR(_phy_port);
        WRITE_OBJECT_VECTOR(_bond_info);
        WRITE_OBJECT_VECTOR(_uplink_info);
        WRITE_OBJECT_VECTOR(_kernel_cfgport);
        WRITE_OBJECT_VECTOR(_vtep_cfgport);
        WRITE_OBJECT(_kernel_port);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CMessageVNAPortInfoReport);
        READ_DIGIT(_vna_flag);
        READ_STRING(_vna_id);
        READ_OBJECT_VECTOR(_phy_port);
        READ_OBJECT_VECTOR(_bond_info);
        READ_OBJECT_VECTOR(_uplink_info);
        READ_OBJECT_VECTOR(_kernel_cfgport);
        READ_OBJECT_VECTOR(_vtep_cfgport);
        READ_OBJECT(_kernel_port);
        DESERIALIZE_END();
    };
};

class CEvbPortInfo: public Serializable
{
public:
    string               m_strName;
    int32                m_nStatus;
    int32                m_nType;

    CEvbPortInfo()
    {
        m_strName = "";
        m_nStatus = 0;
        m_nType = 0;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(CEvbPortInfo);
        WRITE_STRING(m_strName);
        WRITE_DIGIT(m_nStatus);
        WRITE_DIGIT(m_nType);
        SERIALIZE_END();
    };
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CEvbPortInfo);
        READ_STRING(m_strName);
        READ_DIGIT(m_nStatus);
        READ_DIGIT(m_nType);
        DESERIALIZE_END();
    };
};

/******************************************************/
typedef enum tagPortType
{
    PORT_TYPE_PHYSICAL = 0,
    PORT_TYPE_BOND,     
    PORT_TYPE_SRIOVVF,
    PORT_TYPE_KERNEL,
    PORT_TYPE_UPLINKLOOP,
    PORT_TYPE_VTEP,
}PORT_TYPE;

} //namespace zte_tecs
#endif //VNET_PORTINFO_H

