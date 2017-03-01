/******************************************************************************
*   Copyright   (c) 2011，中兴通讯股份有限公司，All rights reserved.
*   
*   文件名称：vnet_xmlrpc_portgroup_method.cpp
*   文件标识：
*   内容摘要：VNetConfigNetplaneDataXMLRPC类的实现文件
*   当前版本：1.0
*   作      者：陈志伟
*   完成日期：2013年3月5日
*
*   修改记录1：
*           修改日期：2013年3月5日
*           版 本   号：V1.0
*           修 改   者：陈志伟
*           修改内容：创建
******************************************************************************/

#include "vnet_xmlrpc_portgroup_method.h"  
#include "user_pool.h"
#include "authrequest.h"
#include "vnet_libnet.h"

using namespace xmlrpc_c;
namespace   zte_tecs {
   
#define XML_RPC_RETURN() \
    array_result = new value_array(array_data); \
    *retval = *array_result; \
    delete array_result; \
    return;

#define AUTHORIZE(ob, op) \
    AuthRequest ar(get_userid()); \
    ar.AddAuthorize((ob), 0, (op), 0, false); \
    if (-1 == UserPool::Authorize(ar)) \
    { \
        array_data.push_back(value_int(TECS_ERR_AUTHORIZE_FAILED)); \
        array_data.push_back(value_string("Error, failed to authorize usr's operation."));\
        XML_RPC_RETURN(); \
    }


#define WAITE_VNM_MSG_ACK(req, msg_id, tmp_mu_name) \
    MessageUnit temp_mu(TempUnitName(tmp_mu_name)); \
    temp_mu.Register(); \
    MID receiver(MU_VNET_LIBNET); \
    MessageOption option(receiver, (msg_id), 0, 0); \
    temp_mu.Send(req, option); \
    MessageFrame message; \
    if(SUCCESS == temp_mu.Wait(&message, 5000))

#define ACK_VNETLIB_NOTWORK()\
    array_data.push_back(value_int(TECS_ERROR_VNET_NOT_WORKING)); \
    array_data.push_back(value_string("Failed, vnetlib is not answered."));

#define ACK_PARA_INVALID(para) \
    array_data.push_back(value_int(TECS_ERR_PARA_INVALID)); \
    ostringstream stross; \
    stross << "Error, invalide parameter :" << para << "."; \
    array_data.push_back(value_string(stross.str()));


/*****************************************************************************/
/**
@brief 功能描述: portgroup配置注册XML方法接口

@li @b 入参列表：
@verbatim
paramList        这个是XML传入的请求内容
@endverbatim
        
@li @b 出参列表：
@verbatim
retval          这个是该请求内置执行的结果
@endverbatim
            
@li @b 返回值列表：
@verbatim
无
@endverbatim
                
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void VNetConfigPortGroupDataXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
    xmlrpc_c::value *   const   retval)
{
    int                         num_param = 1;
    vector<xmlrpc_c::value>     array_data;
    value_array                *array_result = NULL;
    CPortGroupMsg m_cNetPorgGroupCFGXMLRPC;
/************************************************************************
    <portgroup>
        <portgroup_name></portgroup_name>
        <portgroup_uuid></portgroup _uuid>
        <portgroup_type><portgroup_type>  
        <portgroup_description></portgroup_description>
        <switchport_mode>access/trunk</switchport_mode>
        <netplane_iscfg></netplane_iscfg >
        <netplane_uuid></netplane_uuid >
        <portgroup_sdn></portgroup_sdn>
        <promisc></promisc>
        <default_vlan_id></default_vlan_id>
        <native_vlan></native_vlan>
        <access_vlan></access_vlan>  PGtype  是21 该字段无效 
        <access_isolate_vlan></access_isolate_vlan> 只在PGtype是21有效
        <mtu></mtu>
        <acl></acl>
        <qos></qos>
        <allowed_priorities></allowed_priorities> //先不用
        <receive_bandwidth_limit></receive_bandwidth_limit>//先不用
        <receive_bandwidth_reserve></receive_bandwidth_reserve>//先不用
        <macvlan_filter_enable></macvlan_filter_enable>//先不用
        <allowed_mac_change></allowed_mac_change>//先不用
        <allowed_transmit_macs></allowed_transmit_macs>//先不用
        <allowed_transmit_macvlans></allowed_transmit_macvlans>
        <policy_block_override></policy_block_override>
        <policy_vlan_override></policy_vlan_override>
        <version></version>
        <mgr_id></mgr_id>
        <vsitype_id></vsitype_id>
        <isolate_type></isolate_type>
        <segment_id></segment_id>
        <oper><oper>   操作类型，创建，删除，修改
    </portgroup>
****************************************************************************/
    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

    /* 2. 解析参数 */
    string  portgroup_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("portgroup_name",num_param, portgroup_name);
    m_cNetPorgGroupCFGXMLRPC.m_strName = portgroup_name;

    string portgroup_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("portgroup_uuid",num_param, portgroup_uuid);
    m_cNetPorgGroupCFGXMLRPC.m_strUUID= portgroup_uuid;

    int32 portgroup_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("portgroup_type",num_param, portgroup_type);
    m_cNetPorgGroupCFGXMLRPC.m_nPGType= portgroup_type;

    string port_description = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("portgroup_description",num_param, port_description);
    m_cNetPorgGroupCFGXMLRPC.m_strdescription= port_description;

    int32 switchport_mode = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("switchport_mode",num_param, switchport_mode);
    m_cNetPorgGroupCFGXMLRPC.m_nSwitchPortMode= switchport_mode;

    int32 netplane_iscfg = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("netplane_iscfg",num_param, netplane_iscfg);
    m_cNetPorgGroupCFGXMLRPC.m_nIsCfgNetPlane = netplane_iscfg;

    string netplane_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("netplane_uuid",num_param, netplane_uuid);
    m_cNetPorgGroupCFGXMLRPC.m_strNetPlaneUUID= netplane_uuid;

    int32 portgroup_sdn = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("portgroup_sdn",num_param, portgroup_sdn);
    m_cNetPorgGroupCFGXMLRPC.m_nIsSdn = portgroup_sdn;

    int32 promisc = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("promisc",num_param, promisc);
    m_cNetPorgGroupCFGXMLRPC.m_nPromisc = promisc;

    int32 default_vlan_id = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("default_vlan_id",num_param, default_vlan_id);
    m_cNetPorgGroupCFGXMLRPC.m_nDefaultVlan = default_vlan_id;

    int32 native_vlan_id = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("native_vlan_id",num_param, native_vlan_id);
    m_cNetPorgGroupCFGXMLRPC.m_nTrunkNativeVlan = native_vlan_id;

    int32 access_vlan = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("access_vlan",num_param, access_vlan);
    m_cNetPorgGroupCFGXMLRPC.m_nAccessVlanNum= access_vlan;

    int32 access_isolate_vlan = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("access_isolate_vlan",num_param, access_isolate_vlan);
    m_cNetPorgGroupCFGXMLRPC.m_nAccessIsolateNum = access_isolate_vlan;

    int32 mtu = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("mtu",num_param, mtu);
    m_cNetPorgGroupCFGXMLRPC.m_nMTU= mtu;

    string acl = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("acl",num_param, acl);
    m_cNetPorgGroupCFGXMLRPC.m_strACL = acl;

    string qos = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("qos",num_param, qos);
    m_cNetPorgGroupCFGXMLRPC.m_strQOS = qos;

    string Prior = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("Prior",num_param, Prior);
    m_cNetPorgGroupCFGXMLRPC.m_strPrior = Prior;

    string rcv_bandwidth_limit= xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("rcv_bandwidth_limit",num_param, rcv_bandwidth_limit);
    m_cNetPorgGroupCFGXMLRPC.m_strRcvBWLimit = rcv_bandwidth_limit;

    string rcv_bandwidth_reserve = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("rcv_bandwidth_reserve",num_param, rcv_bandwidth_reserve);
    m_cNetPorgGroupCFGXMLRPC.m_strRcvBWRsv= rcv_bandwidth_reserve; 

    int32 macvlan_filter_enable = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("macvlan_filter_enable",num_param, macvlan_filter_enable);
    m_cNetPorgGroupCFGXMLRPC.m_nMacVlanFilter = macvlan_filter_enable;

    int32 allow_mac_change = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("allow_mac_change",num_param, allow_mac_change);
    m_cNetPorgGroupCFGXMLRPC.m_nMacChange= allow_mac_change;

    string allowed_tx_macs = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("allowed_tx_macs",num_param, allowed_tx_macs);
    m_cNetPorgGroupCFGXMLRPC.m_strTxMacs = allowed_tx_macs;

    string allowed_tx_macvlans = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("allowed_tx_macsvlans",num_param, allowed_tx_macvlans);
    m_cNetPorgGroupCFGXMLRPC.m_strTxMacvlans = allowed_tx_macvlans;

    string policy_block_override = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("policy_block_override",num_param, policy_block_override);
    m_cNetPorgGroupCFGXMLRPC.m_strPolicyBlkOver = policy_block_override;

    string policy_vlan_override = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("policy_vlan_override",num_param, policy_vlan_override);
    m_cNetPorgGroupCFGXMLRPC.m_strPolicyVlanOver = policy_vlan_override;

    int32 version =  xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("version",num_param, version);
    m_cNetPorgGroupCFGXMLRPC.m_nVersion = version;

    string mgr_id = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("mgr_id",num_param, mgr_id);
    m_cNetPorgGroupCFGXMLRPC.m_strMgrId = mgr_id;

    string vsitype_id = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("vsitype_id",num_param, vsitype_id);
    m_cNetPorgGroupCFGXMLRPC.m_strVSIType = vsitype_id;

    int32 isolate_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("isolate_type",num_param, isolate_type);
    m_cNetPorgGroupCFGXMLRPC.m_nIsolateType = isolate_type;

    int32 segment_id = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("segment_id",num_param, segment_id);
    m_cNetPorgGroupCFGXMLRPC.m_nSegmentId = segment_id;

    int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("oper",num_param, oper);
    m_cNetPorgGroupCFGXMLRPC.m_nOper = oper;
    
    /* 3. 检查参数 */
    if(oper <EN_PORT_GROUP_OPER_BEGIN  || oper > EN_PORT_GROUP_OPER_END)
    {
        ACK_PARA_INVALID("port_group oper");
        XML_RPC_RETURN();
    }
    
    if(switchport_mode !=  EN_SWITCHPORTMODE_TYPE_TRUNK &&
       switchport_mode !=  EN_SWITCHPORTMODE_TYPE_ACCESS)  
    {
        ACK_PARA_INVALID("switchport_mode");
        XML_RPC_RETURN();
    }

    if(isolate_type !=  EN_ISOLATE_TYPE_VLAN &&
       isolate_type !=  EN_ISOLATE_TYPE_VXLAN && 
       isolate_type !=  EN_ISOLATE_TYPE_VXLAN_VLAN)  
    {
        ACK_PARA_INVALID("isolate_technology");
        XML_RPC_RETURN();
    }  
    
    /* 4. 消息转发并等应答 */
    CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
    if(pvnetlib == NULL)
    {
        ACK_VNETLIB_NOTWORK();
        XML_RPC_RETURN();
    }
        
    int ret = pvnetlib->do_cfg_portgroup(m_cNetPorgGroupCFGXMLRPC);
    if(VNET_XMLRPC_SUCCESS == ret && m_cNetPorgGroupCFGXMLRPC.GetResult() == 0)
    {
        array_data.push_back(value_int(TECS_SUCCESS));

        cout <<"ret value1 is 0"<<endl;

        array_data.push_back(value_string(m_cNetPorgGroupCFGXMLRPC.m_strUUID));
    }
    else
    {
       int result = m_cNetPorgGroupCFGXMLRPC.GetResult();

        cout <<"ret value1 is "<<result<<endl;
        cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

        array_data.push_back(value_int(VnetRpcError(result))); 
        array_data.push_back(value_string(m_cNetPorgGroupCFGXMLRPC.GetResultinfo()));
    }
    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/*****************************************************************************/
/**
@brief 功能描述: portgroup配置VLAN注册XML方法接口

@li @b 入参列表：
@verbatim
paramList        这个是XML传入的请求内容
@endverbatim
        
@li @b 出参列表：
@verbatim
retval          这个是该请求内置执行的结果
@endverbatim
            
@li @b 返回值列表：
@verbatim
无
@endverbatim
                
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void VNetConfigPortGroupVlanXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
    xmlrpc_c::value *   const   retval)
{
    int                         num_param = 1;
    vector<xmlrpc_c::value>     array_data;
    value_array                *array_result = NULL;
    CPGTrunkVlanRangeMsg m_cNetPorgGroupVlanCFGXMLRPC;
/************************************************************************
    <portgroup>
        <portgroup_uuid></portgroup_uuid>
        <vlan_begin></vlan_begin>
        <vlan_end></vlan_end>
        <oper></oper>   操作类型，创建，删除，修改
    </portgroup>
****************************************************************************/
    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

    /* 2. 解析参数 */
    string portgroup_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("portgroup_uuid",num_param, portgroup_uuid);
    m_cNetPorgGroupVlanCFGXMLRPC.m_strPGUuid = portgroup_uuid;

    int32 vlan_begin = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("vlan_begin",num_param, vlan_begin);
    m_cNetPorgGroupVlanCFGXMLRPC.m_nVlanBegin= vlan_begin;

    int32 vlan_end = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("vlan_end",num_param, vlan_end);
    m_cNetPorgGroupVlanCFGXMLRPC.m_nVlanEnd= vlan_end;

    int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("oper",num_param, oper);
    m_cNetPorgGroupVlanCFGXMLRPC.m_nOper= oper;
    
    
    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(portgroup_uuid, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("portgroup_uuid");
        XML_RPC_RETURN();
    }

    if(oper <EN_ADD_PG_TRUNK_VLANRANGE  || oper > EN_DEL_PG_TRUNK_VLANRANGE)
    {
        ACK_PARA_INVALID("vlan oper");
        XML_RPC_RETURN();
    }
       
    /* 4. 消息转发并等应答 */
    CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
    if(pvnetlib == NULL)
    {
        ACK_VNETLIB_NOTWORK();
        XML_RPC_RETURN();
    }
    
    int ret = pvnetlib->do_cfg_portgroupVlan(m_cNetPorgGroupVlanCFGXMLRPC);
    if(VNET_XMLRPC_SUCCESS == ret && m_cNetPorgGroupVlanCFGXMLRPC.GetResult() == 0)
    {
         cout <<"ret value1 is 0"<<endl;
        array_data.push_back(value_int(TECS_SUCCESS));
    }
    else
    {
        int result = m_cNetPorgGroupVlanCFGXMLRPC.GetResult();

        cout <<"ret value1 is "<<result<<endl;
        cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

        array_data.push_back(value_int(VnetRpcError(result))); 
        array_data.push_back(value_string(m_cNetPorgGroupVlanCFGXMLRPC.GetResultinfo())); 
    }
    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/*****************************************************************************/
/**
@brief 功能描述: portgroup配置查询XML方法接口

@li @b 入参列表：
@verbatim
paramList        这个是XML传入的请求内容
@endverbatim
        
@li @b 出参列表：
@verbatim
retval          这个是该请求内置执行的结果
@endverbatim
            
@li @b 返回值列表：
@verbatim
无
@endverbatim
                
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void VNetPortGroupQueryXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
    xmlrpc_c::value *   const   retval)
{
    int                         num_param = 1;
    value_array                *array_result = NULL;
    vector<value>     array_data;
    CVNetPortGroupQuery req_data;
    CVnetPortGroupLists      ack_pg;
/************************************************************************
    <CVNetPortGroupQuery>
        <start_index></start_index>
        <count></count>
    </CVNetPortGroupQuery>
****************************************************************************/
    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

    /* 2. 解析参数 */
    int start_index = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("start_index",num_param, start_index);
    req_data.start_index= start_index;

    int count = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("count",num_param, count);
    req_data.count= count;
/*
    long long type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
    VNETDBG_XMLRPC("type",num_param, type);
    req_data.type= type;

    string appointed_pg = xmlrpc_c::value_string(paramList.getString(num_param ++));
    VNETDBG_XMLRPC("appointed_pg",num_param, appointed_pg);
    req_data.appointed_pg = appointed_pg;
*/
    
    /* 3. 检查参数 */
    /*if (!StringCheck::CheckNormalName(appointed_pg, 0, STR_LEN_64))
    {
        ACK_PARA_INVALID("appointed_pg");
        XML_RPC_RETURN();
    }

    if(type <0  || type > 1)
    {
        ACK_PARA_INVALID("type");
        XML_RPC_RETURN();
    }*/
    if(count > 200 || count < 1)
    {
        ACK_PARA_INVALID("count");
        XML_RPC_RETURN();
    }
       
    /* 4. 消息转发并等应答 */
    CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
    if(pvnetlib == NULL)
    {
        ACK_VNETLIB_NOTWORK();
        XML_RPC_RETURN();
    }
    
    int ret = pvnetlib->do_portgroup_query(req_data,ack_pg);
    if(VNET_XMLRPC_SUCCESS == ret && ack_pg.ret_code == 0)
    {
         cout <<"ret value1 is 0"<<endl;
         
        vector<value>                          array_pg;
        vector<CVnetPortGroup>::const_iterator iter;
        array_data.push_back(value_int(TECS_SUCCESS)); 
        array_data.push_back(value_i8(ack_pg.next_index));
        array_data.push_back(value_i8(ack_pg.max_count));

        vector<CVnetPortGroup>::const_iterator it = ack_pg.pg_info.begin();
        for (; it != ack_pg.pg_info.end(); ++it)
        {
            CVnetPortGroupInfo pg;
            pg.access_isolate_num = it->access_isolate_num;
            pg.access_vlan_num = it->access_vlan_num;
            pg.isolate_type = it->isolate_type;
            pg.mtu = it->mtu;
            pg.netplane_uuid = it->netplane_uuid;
            pg.pg_name = it->pg_name; 
            pg.pg_type = it->pg_type;
            pg.pg_uuid = it->pg_uuid;
            pg.segment_id = it->segment_id;
            pg.switch_port_mode = it->switch_port_mode;
            pg.trunk_native_vlan = it->trunk_native_vlan;
            pg.is_cfg_netplane = it->is_cfg_netplane;
            pg.m_nVxlanIsolateNo = it->m_nVxlanIsolateNo;
            pg.m_nIsSdn = it->m_nIsSdn;

            array_pg.push_back(pg.to_rpc());
        }
        array_data.push_back(value_array(array_pg));
    }
    else
    {
        int result = ack_pg.GetResult();

        cout <<"ret value1 is "<<result<<endl;
        cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

        array_data.push_back(value_int(VnetRpcError(result))); 
        array_data.push_back(value_string(ack_pg.GetResultinfo())); 
    }
    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}
}

