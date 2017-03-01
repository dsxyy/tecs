/******************************************************************************
*   Copyright   (c) 2011，中兴通讯股份有限公司，All rights reserved.
*   
*   文件名称：vnet_xmlrpc_netplane_method.cpp
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

#include "vnet_xmlrpc_netplane_method.h"  
#include "user_pool.h"
#include "authrequest.h"


using namespace xmlrpc_c;
namespace   zte_tecs {

int32   vnet_print_xmlrpc = 1;
#define VNETDBG_XMLRPC(name, no, var)    {if(vnet_print_xmlrpc) (cout << "-->" << name << ":" << no << " :: " << var << endl);}



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
    @brief 功能描述: NETPLANE配置注册XML方法接口

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
    void VNetConfigNetplaneDataXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CNetplaneMsg m_cNetPlaneCFGXMLRPC;
        /************************************************************************
         * NetPlaneData的配置：
         *      <net_plane>
         *          <netplane_name>        </netplane_name>
         *          <netplane_uuid>        </netplane_uuid>
         *          <netplane_description> </netplane_description>
         *          <netplane_mtu>         </netplane_mtu>
         *          <netplane_oper>        </netplane_oper>
         *      </net_plane>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  netplane_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_name",num_param, netplane_name);
        m_cNetPlaneCFGXMLRPC.m_strName = netplane_name;

        string  netplane_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_uuid",num_param, netplane_uuid);
        m_cNetPlaneCFGXMLRPC.m_strUUID = netplane_uuid;

        string netplane_description =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_description",num_param, netplane_description);
        m_cNetPlaneCFGXMLRPC.m_strDescription = netplane_description;

        int32 netplane_mtu = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("netplane_mtu",num_param, netplane_mtu);
        m_cNetPlaneCFGXMLRPC.m_dwMTU = netplane_mtu;

        int32 netplane_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("netplane_oper",num_param, netplane_oper);
        m_cNetPlaneCFGXMLRPC.m_dwOper= netplane_oper;

        /* 3. 检查参数 */
        if(0 == m_cNetPlaneCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("netplane_oper");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_netplane(m_cNetPlaneCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetPlaneCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(m_cNetPlaneCFGXMLRPC.m_strUUID));
        }
        else
        {
           int result = m_cNetPlaneCFGXMLRPC.GetResult();
           cout <<"ret value1 is "<<result<<endl;
           cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

           array_data.push_back(value_int(VnetRpcError(m_cNetPlaneCFGXMLRPC.GetResult()))); 
           array_data.push_back(value_string(m_cNetPlaneCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }
 
    void VNetConfigNetplaneIPResoueceXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CNetplaneIPv4RangeMsg m_cNetPlaneIPCFGXMLRPC;
/************************************************************************        
* NetPlaneResoueceData的配置：       
*      <net_plane_resouece>        
*          <netplane_uuid>        </netplane_uuid>    
*          <IPv4Start> </IPv4Start>        
*          <IPv4End>   </IPv4End>		 
*          <IPv4StartMask> </IPv4StartMask>		 
*          <IPv4EndMask>  </IPv4EndMask>		 
*          <MACStart>     </MACStart>		 
*          <MACEnd>     </MACEnd>		 
*          <vlanStart>     </vlanStart>		 
*          <vlanEnd>     </vlanStart>    
*          <segmentStart>     </segmentStart>		 
*          <segmentEnd>     </segmentEnd>         
*           <oper>        </oper>         
*      </net_plane_resouece>                 
************************************************************************/
/* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  netplane_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_uuid",num_param, netplane_uuid);

        m_cNetPlaneIPCFGXMLRPC.m_strNetplaneUUID= netplane_uuid;

        string IPv4Start= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4Start",num_param, IPv4Start);
        m_cNetPlaneIPCFGXMLRPC.m_strIPv4Start = IPv4Start;

        string IPv4End= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4End",num_param, IPv4End);
        m_cNetPlaneIPCFGXMLRPC.m_strIPv4End = IPv4End;

        string IPv4StartMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4StartMask",num_param, IPv4StartMask);
        m_cNetPlaneIPCFGXMLRPC.m_strIPv4StartMask = IPv4StartMask;

        string IPv4EndMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4EndMask",num_param, IPv4EndMask);
        m_cNetPlaneIPCFGXMLRPC.m_strIPv4EndMask = IPv4EndMask;

        string MACStart= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACStart",num_param, MACStart);

        string MACEnd= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACEnd",num_param, MACEnd);

        int32 VlanStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanStart",num_param, VlanStart);

        int32 VlanEnd  = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanEnd",num_param, VlanEnd);

        int32 SegmentStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentStart",num_param, SegmentStart);

        int32 SegmentEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentEnd",num_param, SegmentEnd);               

        int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("oper",num_param, VlanEnd);
        m_cNetPlaneIPCFGXMLRPC.m_dwOper = oper;


        /* 3. 检查参数 */
        if(!m_cNetPlaneIPCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("netplaneip_oper_type");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_netplaneIP(m_cNetPlaneIPCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetPlaneIPCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            int result = m_cNetPlaneIPCFGXMLRPC.GetResult();
            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

           array_data.push_back(value_int(VnetRpcError(m_cNetPlaneIPCFGXMLRPC.GetResult()))); 
            array_data.push_back(value_string(m_cNetPlaneIPCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
}

void VNetConfigNetplaneMACResoueceXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
{
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CMACRangeMsg          m_cNetPlaneMACCFGXMLRPC; 
/************************************************************************        
* NetPlaneResoueceData的配置：       
*      <net_plane_resouece>        
*          <netplane_uuid>        </netplane_uuid>    
*          <IPv4Start> </IPv4Start>        
*          <IPv4End>   </IPv4End>		 
*          <IPv4StartMask> </IPv4StartMask>		 
*          <IPv4EndMask>  </IPv4EndMask>		 
*          <MACStart>     </MACStart>		 
*          <MACEnd>     </MACEnd>		 
*          <vlanStart>     </vlanStart>		 
*          <vlanEnd>     </vlanStart>    
*          <segmentStart>     </segmentStart>		 
*          <segmentEnd>     </segmentEnd>         
*          <oper>        </oper>         
*      </net_plane_resouece>                 
************************************************************************/
/* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  netplane_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_uuid",num_param, netplane_uuid);

        m_cNetPlaneMACCFGXMLRPC.m_strNetplaneUUID= netplane_uuid;

        string IPv4Start= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4Start",num_param, IPv4Start);

        string IPv4End= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4End",num_param, IPv4End);

        string IPv4StartMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4StartMask",num_param, IPv4StartMask);

        string IPv4EndMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4EndMask",num_param, IPv4EndMask);

        string MACStart= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACStart",num_param, MACStart);
        m_cNetPlaneMACCFGXMLRPC.m_strMACStart= MACStart;

        string MACEnd= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACEnd",num_param, MACEnd);
        m_cNetPlaneMACCFGXMLRPC.m_strMACEnd= MACEnd;

        int32 VlanStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanStart",num_param, VlanStart);

        int32 VlanEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanEnd",num_param, VlanEnd);

        int32 SegmentStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentStart",num_param, SegmentStart);

        int32 SegmentEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentEnd",num_param, SegmentEnd);               

        int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("oper",num_param, VlanEnd);
        m_cNetPlaneMACCFGXMLRPC.m_dwOper = oper;

        /* 3. 检查参数 */
        if(!m_cNetPlaneMACCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("netplanemac_oper_type");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_netplaneMAC(m_cNetPlaneMACCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetPlaneMACCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            int result = m_cNetPlaneMACCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

            array_data.push_back(value_int(VnetRpcError(result))); 
            array_data.push_back(value_string(m_cNetPlaneMACCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
}

void VNetConfigNetplaneVLANResoueceXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
{
        int num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CVlanRangeMsg         m_cNetPlaneVLANCCFGXMLRPC;
/************************************************************************        
* NetPlaneResoueceData的配置：       
*      <net_plane_resouece>        
*          <netplane_uuid>        </netplane_uuid>    
*          <IPv4Start> </IPv4Start>        
*          <IPv4End>   </IPv4End>		 
*          <IPv4StartMask> </IPv4StartMask>		 
*          <IPv4EndMask>  </IPv4EndMask>		 
*          <MACStart>     </MACStart>		 
*          <MACEnd>     </MACEnd>		 
*          <vlanStart>     </vlanStart>		 
*          <vlanEnd>     </vlanStart>    
*          <segmentStart>     </segmentStart>		 
*          <segmentEnd>     </segmentEnd>         
*          <oper>        </oper>         
*      </net_plane_resouece>                 
************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  netplane_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_uuid",num_param, netplane_uuid);

        m_cNetPlaneVLANCCFGXMLRPC.m_strNetplaneUUID= netplane_uuid;

        string IPv4Start= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4Start",num_param, IPv4Start);

        string IPv4End= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4End",num_param, IPv4End);

        string IPv4StartMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4StartMask",num_param, IPv4StartMask);

        string IPv4EndMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4EndMask",num_param, IPv4EndMask);

        string MACStart= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACStart",num_param, MACStart);

        string MACEnd= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACEnd",num_param, MACEnd);

        int32 VlanStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanStart",num_param, VlanStart);
        m_cNetPlaneVLANCCFGXMLRPC.m_nVlanStart= VlanStart;

        int32  VlanEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanEnd",num_param, VlanEnd);
        m_cNetPlaneVLANCCFGXMLRPC.m_nVlanEnd= VlanEnd;

        int32 SegmentStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentStart",num_param, SegmentStart);

        int32 SegmentEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentEnd",num_param, SegmentEnd);       

        int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("oper",num_param, VlanEnd);
        m_cNetPlaneVLANCCFGXMLRPC.m_dwOper = oper;

        /* 3. 检查参数 */
        if(!m_cNetPlaneVLANCCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("netplanevlan_oper_type");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_netplaneVLAN(m_cNetPlaneVLANCCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetPlaneVLANCCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
        int result = m_cNetPlaneVLANCCFGXMLRPC.GetResult();

        cout <<"ret value1 is "<<result<<endl;
        cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

        array_data.push_back(value_int(VnetRpcError(result))); 
        array_data.push_back(value_string(m_cNetPlaneVLANCCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
}

void VNetConfigNetplaneSEGMENTResoueceXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
{
        int num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CSegmentRangeMsg         m_cNetPlaneSEGMENTCCFGXMLRPC;
/************************************************************************        
* NetPlaneResoueceData的配置：       
*      <net_plane_resouece>        
*          <netplane_uuid>        </netplane_uuid>    
*          <IPv4Start> </IPv4Start>        
*          <IPv4End>   </IPv4End>		 
*          <IPv4StartMask> </IPv4StartMask>		 
*          <IPv4EndMask>  </IPv4EndMask>		 
*          <MACStart>     </MACStart>		 
*          <MACEnd>     </MACEnd>		 
*          <vlanStart>     </vlanStart>		 
*          <vlanEnd>     </vlanStart>    
*          <segmentStart>     </segmentStart>		 
*          <segmentEnd>     </segmentStart>         
*          <oper>        </oper>         
*      </net_plane_resouece>                 
************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  netplane_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("netplane_uuid",num_param, netplane_uuid);

        m_cNetPlaneSEGMENTCCFGXMLRPC.m_strNetplaneUUID= netplane_uuid;

        string IPv4Start= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4Start",num_param, IPv4Start);

        string IPv4End= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4End",num_param, IPv4End);

        string IPv4StartMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4StartMask",num_param, IPv4StartMask);

        string IPv4EndMask= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("IPv4EndMask",num_param, IPv4EndMask);

        string MACStart= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACStart",num_param, MACStart);

        string MACEnd= xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("MACEnd",num_param, MACEnd);

        int32 VlanStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanStart",num_param, VlanStart);

        int32  VlanEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("VlanEnd",num_param, VlanEnd);

        int32 SegmentStart= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentStart",num_param, SegmentStart);
        m_cNetPlaneSEGMENTCCFGXMLRPC.m_nSegmentStart= SegmentStart;

        int32 SegmentEnd= xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("SegmentEnd",num_param, SegmentEnd);
        m_cNetPlaneSEGMENTCCFGXMLRPC.m_nSegmentEnd= SegmentEnd;        

        int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("oper",num_param, VlanEnd);
        m_cNetPlaneSEGMENTCCFGXMLRPC.m_dwOper = oper;

        /* 3. 检查参数 */
        if(!m_cNetPlaneSEGMENTCCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("netplanesegment_oper_type");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_netplaneSEGMENT(m_cNetPlaneSEGMENTCCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetPlaneSEGMENTCCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            int result = m_cNetPlaneSEGMENTCCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

            array_data.push_back(value_int(VnetRpcError(result))); 
            array_data.push_back(value_string(m_cNetPlaneSEGMENTCCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
}

}

