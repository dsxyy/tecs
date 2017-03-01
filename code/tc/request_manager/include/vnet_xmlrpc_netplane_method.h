/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_xmlrpc_netplane_method.h
* 文件标识：
* 内容摘要：VNetNetplaneXMLRPC类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：
*
* 修改记录1： 
*     修改日期：2013年3月3日
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
******************************************************************************/

#ifndef VNET_XMLRPC_NETPLANE_METHOD_H
#define VNET_XMLRPC_NETPLANE_METHOD_H

#include "sky.h"
#include "vnet_api.h"
#include "tecs_rpc_method.h"
#include "vnet_libnet.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"
#include "vnet_error.h"

#include "vnet_xmlrpc_netplane_method.h"

using namespace std;
namespace zte_tecs {

extern  int32   vnet_print_xmlrpc;
#define VNETDBG(name, no, var)    {if(vnet_print_xmlrpc) (cout << "-->" << name << ":" << no << " :: " << var << endl);}
    /**
    @brief 功能描述: NetPlane Data配置信息
    @li @b 其它说明：无
    */
    class VNetMapValueXMPRPC
    {
        public: 
            VNetMapValueXMPRPC(){}
            virtual ~VNetMapValueXMPRPC(){}

        virtual int VnetRpcError(int inner_error)
        {
            ERR_MAP_BEGIN
                ERR_MAP_PUB
                ERR_MAP(NETPLANE_GET_DB_INTF_ERROR,           TECS_ERROR_VNET_GET_DB_INTF_ERROR)
                ERR_MAP(LOGIC_NETWORK_GET_DB_INTF_ERROR,      TECS_ERROR_VNET_GET_DB_INTF_ERROR)
                ERR_MAP(IPV4_RANGE_GET_LG_NET_DB_INTF_ERROR,  TECS_ERROR_VNET_GET_DB_INTF_ERROR)
                ERR_MAP(IPV4_RANGE_GET_NET_PLANE_DB_INTF_ERROR,TECS_ERROR_VNET_GET_DB_INTF_ERROR)
                ERR_MAP(MAC_RANGE_GET_DB_INTF_ERROR,          TECS_ERROR_VNET_GET_DB_INTF_ERROR)
                ERR_MAP(VLAN_RANGE_GET_DB_INTF_ERROR,         TECS_ERROR_VNET_GET_DB_INTF_ERROR)
                ERR_MAP(SEGMENT_RANGE_GET_DB_INTF_ERROR,      TECS_ERROR_VNET_GET_DB_INTF_ERROR)                
                ERR_MAP(NETPLANE_DATA_NOT_EXIST,              TECS_ERROR_VNET_NETPLANE_DATA_NOT_EXIST)
                ERR_MAP(NETPLANE_DATA_ALREADY_EXIST,          TECS_ERROR_VNET_NETPLANE_DATA_ALREADY_EXIST)
                ERR_MAP(NETPLANE_CFG_PARAM_ERROR,             TECS_ERROR_VNET_NETPLANE_CFG_PARAM_ERROR)
                ERR_MAP(LOGIC_NETWORK_DATA_NOT_EXIST,         TECS_ERROR_VNET_LOGIC_NETWORK_DATA_NOT_EXIST)
                ERR_MAP(LOGIC_NETWORK_DATA_ALREADY_EXIST,     TECS_ERROR_VNET_LOGIC_NETWORK_DATA_ALREADY_EXIST)
                ERR_MAP(LOGIC_NETWORK_CFG_PARAM_ERROR,        TECS_ERROR_VNET_LOGIC_NETWORK_CFG_PARAM_ERROR)
                ERR_MAP(IPV4_RANGE_CFG_PARAM_ERROR,           TECS_ERROR_VNET_IPV4_RANGE_CFG_PARAM_ERROR)
                ERR_MAP(IPV4_RANGE_CALC_ERROR,                TECS_ERROR_VNET_IPV4_RANGE_CALC_ERROR)
                ERR_MAP(IPV4_RANGE_NOT_EXIST,                 TECS_ERROR_VNET_IPV4_RANGE_NOT_EXIST)
                ERR_MAP(IPV4_RANGE_NOT_SAME_SUBNET,           TECS_ERROR_VNET_IPV4_RANGE_NOT_SAME_SUBNET)
                ERR_MAP(IPV4_RANGE_INVALID,                   TECS_ERROR_VNET_IPV4_RANGE_INVALID)
                ERR_MAP(IPV4_RANGE_OCCUPIED,                  TECS_ERROR_VNET_IPV4_RANGE_OCCUPIED)  
                ERR_MAP(MAC_RANGE_CFG_PARAM_ERROR,            TECS_ERROR_VNET_MAC_RANGE_CFG_PARAM_ERROR) 
                ERR_MAP(MAC_RANGE_CALC_ERROR,                 TECS_ERROR_VNET_MAC_RANGE_CALC_ERROR)
                ERR_MAP(MAC_RANGE_NOT_EXIST,                  TECS_ERROR_VNET_MAC_RANGE_NOT_EXIST)
                ERR_MAP(MAC_RANGE_INVALID,                    TECS_ERROR_VNET_MAC_RANGE_INVALID)
                ERR_MAP(MAC_OUI_INVALID,                      TECS_ERROR_VNET_MAC_OUI_INVALID)
                ERR_MAP(MAC_PARSE_ERROR,                      TECS_ERROR_VNET_MAC_PARSE_ERROR)
                ERR_MAP(MAC_RANGE_OCCUPIED,                   TECS_ERROR_VNET_MAC_RANGE_OCCUPIED)
                ERR_MAP(VLAN_RANGE_CFG_PARAM_ERROR,           TECS_ERROR_VNET_VLAN_RANGE_CFG_PARAM_ERROR)
                ERR_MAP(VLAN_RANGE_CALC_ERROR,                TECS_ERROR_VNET_VLAN_RANGE_CALC_ERROR)
                ERR_MAP(VLAN_RANGE_NOT_EXIST,                 TECS_ERROR_VNET_VLAN_RANGE_NOT_EXIST) 
                ERR_MAP(VLAN_RANGE_OCCUPIED,                  TECS_ERROR_VNET_VLAN_RANGE_OCCUPIED)
                ERR_MAP(SEGMENT_RANGE_CFG_PARAM_ERROR,        TECS_ERROR_VNET_SEGMENT_RANGE_CFG_PARAM_ERROR)
                ERR_MAP(SEGMENT_RANGE_CALC_ERROR,             TECS_ERROR_VNET_SEGMENT_RANGE_CALC_ERROR)
                ERR_MAP(SEGMENT_RANGE_NOT_EXIST,              TECS_ERROR_VNET_SEGMENT_RANGE_NOT_EXIST) 
                ERR_MAP(SEGMENT_RANGE_OCCUPIED,               TECS_ERROR_VNET_SEGMENT_RANGE_OCCUPIED)                
                
                ERR_MAP(ERROR_PORTGROUP_OPERCODE_INVALID,     TECS_ERROR_VNET_ERROR_PORTGROUP_OPERCODE_INVALID)
                ERR_MAP(ERROR_PORTGROUP_PG_INVALID  ,         TECS_ERROR_VNET_ERROR_PORTGROUP_PG_INVALID)
                ERR_MAP(ERROR_PORTGROUP_INPARAM_INVALID,      TECS_ERROR_VNET_ERROR_PORTGROUP_INPARAM_INVALID)
                ERR_MAP(ERROR_PORTGROUP_GET_INSTANCE_FAIL,    TECS_ERROR_VNET_ERROR_PORTGROUP_GET_INSTANCE_FAIL)
                ERR_MAP(ERROR_PORTGROUP_GET_VLANRANGE_FAIL,   TECS_ERROR_VNET_ERROR_PORTGROUP_GET_VLANRANGE_FAIL)
                ERR_MAP(ERROR_PORTGROUP_NOT_EXIST,            TECS_ERROR_VNET_ERROR_PORTGROUP_NOT_EXIST)
                ERR_MAP(ERROR_PORTGROUP_IS_EXIST,             TECS_ERROR_VNET_ERROR_PORTGROUP_IS_EXIST)
                ERR_MAP(ERROR_PORTGROUP_IS_INUSING,           TECS_ERROR_VNET_ERROR_PORTGROUP_IS_INUSING)
                ERR_MAP(ERROR_PORTGROUP_OPER_DB_FAIL,         TECS_ERROR_VNET_ERROR_PORTGROUP_OPER_DB_FAIL)
                ERR_MAP(ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL, TECS_ERROR_VNET_ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL)
                ERR_MAP(ERROR_PORTGROUP_ACCESS_VLAN_INVALID,  TECS_ERROR_VNET_ERROR_PORTGROUP_ACCESS_VLAN_INVALID)
                ERR_MAP(ERROR_PORTGROUP_NATIVE_VLAN_INVALID,  TECS_ERROR_VNET_ERROR_PORTGROUP_NATIVE_VLAN_INVALID)
                ERR_MAP(ERROR_PORTGROUP_PRIVATE_VLAN_INVALID, TECS_ERROR_VNET_ERROR_PORTGROUP_PRIVATE_VLAN_INVALID)
                ERR_MAP(ERROR_PORTGROUP_SEGMENT_INVALID,      TECS_ERROR_VNET_ERROR_PORTGROUP_SEGMENT_INVALID)
                ERR_MAP(ERROR_PORTGROUP_PRIVATE_SEGMENT_INVALID, TECS_ERROR_VNET_ERROR_PORTGROUP_PRIVATE_SEGMENT_INVALID)
                 
                ERR_MAP(ERROR_PG_VLANRANGE_INVLAID,           TECS_ERROR_VNET_ERROR_PG_VLANRANGE_INVLAID)
                ERR_MAP(ERROR_PG_VLANRANGE_IS_EXIST,          TECS_ERROR_VNET_ERROR_PG_VLANRANGE_IS_EXIST)
                ERR_MAP(ERROR_PG_VLANRANGE_NOT_EXIST,         TECS_ERROR_VNET_ERROR_PG_VLANRANGE_NOT_EXIST)
                ERR_MAP(ERROR_PG_VLANRANGE_OPER_DB_FAIL,      TECS_ERROR_VNET_ERROR_PG_VLANRANGE_OPER_DB_FAIL)
                ERR_MAP(ERROR_PG_VLANRANGE_CALC_ERROR,        TECS_ERROR_VNET_ERROR_PG_VLANRANGE_CALC_ERROR)
                ERR_MAP(ERROR_PG_VLANRANGE_IS_CONFLICT,       TECS_ERROR_VNET_ERROR_PG_VLANRANGE_IS_CONFLICT)
                
                ERR_MAP(ERROR_SWITCH_OPERCODE_INVALID,        TECS_ERROR_VNET_ERROR_SWITCH_OPERCODE_INVALID)
                ERR_MAP(ERROR_SWITCH_INSTANCE_FAIL,           TECS_ERROR_VNET_ERROR_SWITCH_INSTANCE_FAIL)
                ERR_MAP(ERROR_SWITCH_GET_PORTMGR_FAIL,        TECS_ERROR_VNET_ERROR_SWITCH_GET_PORTMGR_FAIL)
                ERR_MAP(ERROR_SWITCH_GET_VSWITCH_DB_FAIL,     TECS_ERROR_VNET_ERROR_SWITCH_GET_VSWITCH_DB_FAIL)
                ERR_MAP(ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL,  TECS_ERROR_VNET_ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL)
                ERR_MAP(ERROR_SWTICH_CONFLICT_WITH_WILDCAST,  TECS_ERROR_VNET_ERROR_SWTICH_CONFLICT_WITH_WILDCAST)
                ERR_MAP(ERROR_SWTICH_PORT_INVALID,            TECS_ERROR_VNET_ERROR_SWTICH_PORT_INVALID)
                ERR_MAP(ERROR_SWTICH_GET_PGINFO_FAIL,         TECS_ERROR_VNET_ERROR_SWTICH_GET_PGINFO_FAIL)
                ERR_MAP(ERROR_SWITCH_NAME_INVALID,            TECS_ERROR_VNET_ERROR_SWITCH_NAME_INVALID)
                ERR_MAP(ERROR_SWITCH_ALREADY_EXIST,           TECS_ERROR_VNET_ERROR_SWITCH_ALREADY_EXIST)
                ERR_MAP(ERROR_SWITCH_IS_USING,                TECS_ERROR_VNET_ERROR_SWITCH_IS_USING)
                ERR_MAP(ERROR_SWITCH_OPER_DB_FAIL,            TECS_ERROR_VNET_ERROR_SWITCH_OPER_DB_FAIL)
                ERR_MAP(ERROR_SWITCH_PORT_OPER_DB_FAIL,       TECS_ERROR_VNET_ERROR_SWITCH_PORT_OPER_DB_FAIL)
                ERR_MAP(ERROR_SWITCH_DEL_NOT_FINISH,          TECS_ERROR_VNET_ERROR_SWITCH_DEL_NOT_FINISH)
                ERR_MAP(ERROR_SWITCH_GET_PORTINFO_FAIL,       TECS_ERROR_VNET_ERROR_SWITCH_GET_PORTINFO_FAIL)
                ERR_MAP(ERROR_SWITCH_ADD_BOND_FAIL,           TECS_ERROR_VNET_ERROR_SWITCH_ADD_BOND_FAIL)
                ERR_MAP(ERROR_SWITCH_DEL_BOND_FAIL,           TECS_ERROR_VNET_ERROR_SWITCH_DEL_BOND_FAIL)
                ERR_MAP(ERROR_SWITCH_DEL_BONDSLAVE_FAIL,      TECS_ERROR_VNET_ERROR_SWITCH_DEL_BONDSLAVE_FAIL)
                ERR_MAP(ERROR_SWITCH_GET_BONDSLAVE_FAIL,      TECS_ERROR_VNET_ERROR_SWITCH_GET_BONDSLAVE_FAIL)
                ERR_MAP(ERROR_SWITCH_ADD_BRIDGE_FAIL,         TECS_ERROR_VNET_ERROR_SWITCH_ADD_BRIDGE_FAIL)
                ERR_MAP(ERROR_SWITCH_DEL_BRIDGE_FAIL,         TECS_ERROR_VNET_ERROR_SWITCH_DEL_BRIDGE_FAIL)
                ERR_MAP(ERROR_SWITCH_SET_ATTR_FAIL,           TECS_ERROR_VNET_ERROR_SWITCH_SET_ATTR_FAIL)
                ERR_MAP(ERROR_SWITCH_PORT_ISCONFLICT,         TECS_ERROR_VNET_ERROR_SWITCH_PORT_ISCONFLICTT)
                ERR_MAP(ERROR_SWITCH_PORT_ISMIXED,            TECS_ERROR_VNET_ERROR_SWITCH_PORT_ISMIXED)
                ERR_MAP(ERROR_SWITCH_OPER_KERNEL_DVS_FAIL,    TECS_ERROR_VNET_ERROR_SWITCH_OPER_KERNEL_DVS_FAIL)
                ERR_MAP(ERROR_SWITCH_QUERY_VNAUUID_FAIL,      TECS_ERROR_VNET_ERROR_SWITCH_QUERY_VNAUUID_FAIL)
                ERR_MAP(ERROR_SWITCH_MERGE_REQINFO_FAIL,      TECS_ERROR_VNET_ERROR_SWITCH_MERGE_REQINFO_FAIL)
                ERR_MAP(ERROR_SWITCH_GET_SRIOV_VFS_FAIL,      TECS_ERROR_VNET_ERROR_SWITCH_GET_SRIOV_VFS_FAIL)
                ERR_MAP(ERROR_SWITCH_ADD_UPLINK_FAIL,         TECS_ERROR_VNET_ERROR_SWITCH_ADD_UPLINK_FAIL)
                ERR_MAP(ERROR_SWITCH_ADD_EVBPORT_FAIL,        TECS_ERROR_VNET_ERROR_SWITCH_ADD_EVBPORT_FAIL)
                ERR_MAP(ERROR_SWITCH_DEL_EVBPORT_FAIL,        TECS_ERROR_VNET_ERROR_SWITCH_DEL_EVBPORT_FAIL)
                ERR_MAP(ERROR_SWITCH_RENAME_BRIDGE_FAIL,      TECS_ERROR_VNET_ERROR_SWITCH_RENAME_BRIDGE_FAIL)
                ERR_MAP(ERROR_SWITCH_CHECK_PARAM_FAIL,        TECS_ERROR_VNET_ERROR_SWITCH_CHECK_PARAM_FAIL)
                ERR_MAP(ERROR_SWITCH_TIMER_CONFIG_FAIL,       TECS_ERROR_VNET_ERROR_SWITCH_TIMER_CONFIG_FAIL)
                ERR_MAP(ERROR_SWITCH_GET_VNAINFO_FAIL,        TECS_ERROR_VNET_ERROR_SWITCH_GET_VNAINFO_FAIL)
                ERR_MAP(ERROR_SWITCH_MODIFY_FAIL,             TECS_ERROR_VNET_ERROR_SWITCH_MODIFY_FAIL)
                ERR_MAP(ERROR_SWITCH_ADD_VTEP_FAIL,           TECS_ERROR_VNET_ERROR_SWITCH_ADD_VTEP_FAIL)
                ERR_MAP(ERROR_SWITCH_DEL_VTEP_FAIL,           TECS_ERROR_VNET_ERROR_SWITCH_DEL_VTEP_FAIL)
                ERR_MAP(ERROR_SWITCH_QUERY_VTEP_FAIL,         TECS_ERROR_VNET_ERROR_SWITCH_QUERY_VTEP_FAIL)

                ERR_MAP(ERROR_WC_SWITCH_OPERCODE_INVALID,     TECS_ERROR_VNET_WC_SWITCH_OPERCODE_INVALID)
                ERR_MAP(ERROR_WC_SWITCH_INSTANCE_FAIL,        TECS_ERROR_VNET_WC_SWITCH_INSTANCE_FAIL)                
                ERR_MAP(ERROR_WC_SWITCH_NAME_EMPTY,           TECS_ERROR_VNET_WC_SWITCH_NAME_EMPTY)
                ERR_MAP(ERROR_WC_SWITCH_PG_EMPTY,             TECS_ERROR_VNET_WC_SWITCH_PG_EMPTY)                
                ERR_MAP(ERROR_WC_SWITCH_PGTYPE_INVALID,       TECS_ERROR_VNET_WC_SWITCH_PGTYPE_INVALID)
                ERR_MAP(ERROR_WC_SWITCH_EVBMODE_INVALID,      TECS_ERROR_VNET_WC_SWITCH_EVBMODE_INVALID)                
                ERR_MAP(ERROR_WC_SWITCH_ALREADY_EXIST,        TECS_ERROR_VNET_WC_SWITCH_ALREADY_EXIST)
                ERR_MAP(ERROR_WC_SWITCH_GET_DB_FAIL,          TECS_ERROR_VNET_WC_SWITCH_GET_DB_FAIL)                
                ERR_MAP(ERROR_WC_SWITCH_SET_DB_FAIL,          TECS_ERROR_VNET_WC_SWITCH_SET_DB_FAIL)
                ERR_MAP(ERROR_WC_SWITCH_NOTEXIST_PORT,        TECS_ERROR_VNET_WC_SWITCH_NOTEXIST_PORT)                
                ERR_MAP(ERROR_WC_SWITCH_PARAM_INVALID,        TECS_ERROR_VNET_WC_SWITCH_PARAM_INVALID) 
                ERR_MAP(ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT,        TECS_ERROR_VNET_WC_SWITCH_UPLINKPORT_IS_CONFLICT) 
                ERR_MAP(ERROR_WC_SWITCH_MIX_UPLINKPG_MUST_SAME,        TECS_ERROR_VNET_WC_SWITCH_MIX_UPLINKPG_MUST_SAME) 
                ERR_MAP(ERROR_WC_SWITCH_MIX_UPLINK_BONDMODE_MUST_SAME, TECS_ERROR_VNET_WC_SWITCH_MIX_UPLINK_BONDMODE_MUST_SAME) 
                ERR_MAP(ERROR_WC_SWITCH_MIX_EVBMODE_MUST_SAME, TECS_ERROR_VNET_WC_SWITCH_MIX_EVBMODE_MUST_SAME) 

                
                ERR_MAP(ERROR_WC_CREATE_VPORT_GET_DB_FAIL,      TECS_ERROR_VNET_WC_CREATE_VPORT_GET_DB_FAIL)                     
                ERR_MAP(ERROR_WC_CREATE_VPORT_SET_DB_FAIL,      TECS_ERROR_VNET_WC_CREATE_VPORT_SET_DB_FAIL)  
                ERR_MAP(ERROR_WC_CREATE_VPORT_ALREADY_EXIST,    TECS_ERROR_VNET_WC_CREATE_VPORT_ALREADY_EXIST)                  
                ERR_MAP(ERROR_WC_CREATE_VPORT_NAME_EMPTY,       TECS_ERROR_VNET_WC_CREATE_VPORT_NAME_EMPTY)                  
                ERR_MAP(ERROR_WC_CREATE_VPORT_OPERCODE_INVALID, TECS_ERROR_VNET_WC_CREATE_VPORT_OPERCODE_INVALID)                                  
                ERR_MAP(ERROR_WC_GET_PORT_INSTANCE_NULL,        TECS_ERROR_VNET_WC_GET_PORT_INSTANCE_NULL)                                  
                ERR_MAP(ERROR_WC_GET_SWTICH_INSTANCE_NULL,      TECS_ERROR_VNET_WC_GET_SWTICH_INSTANCE_NULL)                                  
                ERR_MAP(ERROR_WC_UPLINKPG_MTU_LESS_KNPG_MTU,    TECS_ERROR_VNET_WC_UPLINKPG_MTU_LESS_KNPG_MTU)                                                  
                
                ERR_MAP(ERROR_WC_LOOPBACK_GET_DB_FAIL,          TECS_ERROR_VNET_WC_LOOPBACK_GET_DB_FAIL)
                ERR_MAP(ERROR_WC_LOOPBACK_SET_DB_FAIL,          TECS_ERROR_VNET_WC_LOOPBACK_SET_DB_FAIL)                
                ERR_MAP(ERROR_WC_LOOPBACK_ALREADY_EXIST,        TECS_ERROR_VNET_WC_LOOPBACK_ALREADY_EXIST)
                ERR_MAP(ERROR_WC_LOOPBACK_NAME_EMPTY,           TECS_ERROR_VNET_WC_LOOPBACK_NAME_EMPTY)                
                ERR_MAP(ERROR_WC_LOOPBACK_OPERCODE_INVALID,     TECS_ERROR_VNET_WC_LOOPBACK_OPERCODE_INVALID)                
                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_GET_DB_FAIL,           TECS_ERROR_VNET_VM_MIGRATE_GET_DB_FAIL)                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_SET_DB_FAIL,           TECS_ERROR_VNET_VM_MIGRATE_SET_DB_FAIL)                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_HAS_EXISTED,           TECS_ERROR_VNET_VM_MIGRATE_HAS_EXISTED)                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_PARAM_INVALID,         TECS_ERROR_VNET_VM_MIGRATE_PARAM_INVALID)                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_INPUTPARAM_DB_NOT_SAME,TECS_ERROR_VNET_VM_MIGRATE_INPUTPARAM_DB_NOT_SAME)                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_INSTANCE_IS_NULL,      TECS_ERROR_VNET_VM_MIGRATE_INSTANCE_IS_NULL)                
                ERR_MAP(ERROR_VNM_VM_MIGRATE_IS_CONFLICT,           TECS_ERROR_VNET_VM_MIGRATE_IS_CONFLICT)                


                ERR_MAP(ERROR_VNA_AGENT_MODULE_OFFLINE,            TECS_ERROR_VNET_VNA_AGENT_MODULE_OFFLINE)                
                ERR_MAP(ERROR_VNA_AGENT_MODULE_NOT_REG,            TECS_ERROR_VNET_VNA_AGENT_MODULE_NOT_REG)                
                ERR_MAP(ERROR_VNA_DELETE_FAILED,                   TECS_ERROR_VNET_VNA_DELETE_FAILED)                
                ERR_MAP(ERROR_VNA_DELETE_DB_OPER_FAILED,           TECS_ERROR_VNET_VNA_DELETE_DB_OPER_FAILED)                
                ERR_MAP(ERROR_VNA_AGENT_MODULE_ONLINE,             TECS_ERROR_VNET_VNA_AGENT_MODULE_ONLINE)                

                ERR_MAP(ERROR_PORT_NAME_EMPTY,                      TECS_ERROR_VNET_PORT_NAME_EMPTY)                
                ERR_MAP(ERROR_PORT_OPERCODE_INVALID,                TECS_ERROR_VNET_PORT_OPERCODE_INVALID)                
                ERR_MAP(ERROR_PORT_PARA_INVALID,                    TECS_ERROR_VNET_PORT_PARA_INVALID)                
                ERR_MAP(ERROR_PORT_OPER_FAIL,                       TECS_ERROR_VNET_PORT_OPER_FAIL)                
                ERR_MAP(ERROR_PORT_OPERDB_FAIL,                     TECS_ERROR_VNET_PORT_OPERDB_FAIL)                
                ERR_MAP(ERROR_PORT_WC_CONFLICT,                     TECS_ERROR_VNET_PORT_WC_CONFLICT)                
                ERR_MAP(ERROR_PORT_CHECKDB_CONFLICT,                TECS_ERROR_VNET_PORT_CHECKDB_CONFLICT)   
                
            ERR_MAP_END
        }
    };
    
    class VNetConfigNetplaneDataXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigNetplaneDataXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigNetplaneDataXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigNetplaneDataXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigNetplaneDataXMLRPC);
    };

    class VNetConfigNetplaneIPResoueceXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigNetplaneIPResoueceXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigNetplaneIPResoueceXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigNetplaneIPResoueceXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigNetplaneIPResoueceXMLRPC);
    };

    class VNetConfigNetplaneMACResoueceXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigNetplaneMACResoueceXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigNetplaneMACResoueceXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigNetplaneMACResoueceXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigNetplaneMACResoueceXMLRPC);
    };

    class VNetConfigNetplaneVLANResoueceXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigNetplaneVLANResoueceXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigNetplaneVLANResoueceXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigNetplaneVLANResoueceXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigNetplaneVLANResoueceXMLRPC);
    };

    class VNetConfigNetplaneSEGMENTResoueceXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigNetplaneSEGMENTResoueceXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigNetplaneSEGMENTResoueceXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigNetplaneSEGMENTResoueceXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigNetplaneSEGMENTResoueceXMLRPC);
    };
    

}  /* end namespace zte_tecs */

#endif /* end TC_VNET_CONFIG_H */


