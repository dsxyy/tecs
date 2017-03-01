/******************************************************************************
*   Copyright   (c) 2011，中兴通讯股份有限公司，All rights reserved.
*   
*   文件名称：vnet_xmlrpc_logicnet_method.cpp
*   文件标识：
*   内容摘要：VNetConfigLogicNetVlanXMLRPC类的实现文件
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

#include "vnet_xmlrpc_logicnet_method.h"  
#include "user_pool.h"
#include "authrequest.h"


using namespace xmlrpc_c;
namespace   zte_tecs {

extern  int32   vnet_print_xmlrpc;
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
    @brief 功能描述: LOGICNET配置注册XML方法接口

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
    void VNetConfigLogicNetDataXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CLogicNetworkMsg m_cLogicNetCFGXMLRPC;
        /************************************************************************
         * logicnetdata的配置：
         *      <logicnet>
         *          <logicnet_name> </logicnet_name>
         *          <logicnet_uuid> </logicnet_uuid>
         *          <portgroup_uuid> </portgroup_uuid>
         *          <IPProtoMode>  </IPProtoMode>
         *          <logicnet_description> </logicnet_description>
         *          <logicnet_oper> </logicnet_oper>
         *          <result> </result>
         *      </logicnet>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  logicnet_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("logicnet_name",num_param, logicnet_name);
        m_cLogicNetCFGXMLRPC.m_strName = logicnet_name;

        string logicnet_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("logicnet_uuid",num_param, logicnet_uuid);
        m_cLogicNetCFGXMLRPC.m_strUUID = logicnet_uuid;

        string portgroup_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("portgroup_uuid",num_param, portgroup_uuid);
        m_cLogicNetCFGXMLRPC.m_strPortGroupUUID = portgroup_uuid;

        int32 IPProtoMode = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("IPProtoMode",num_param, IPProtoMode);
        m_cLogicNetCFGXMLRPC.m_nIPBootProtoMode= IPProtoMode;

        string logicnet_description = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("logicnet_description",num_param, logicnet_description);
        m_cLogicNetCFGXMLRPC.m_strDescription = logicnet_description;   

        int logicnet_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("logicnet_oper",num_param, logicnet_oper);
        m_cLogicNetCFGXMLRPC.m_dwOper = logicnet_oper;     

        /* 3. 检查参数 */
        if(!m_cLogicNetCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("logicnet_oper");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }
        
        int ret = pvnetlib->do_cfg_logicnet(m_cLogicNetCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cLogicNetCFGXMLRPC.GetResult() == 0)
        {
        
            array_data.push_back(value_int(TECS_SUCCESS));
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_string(m_cLogicNetCFGXMLRPC.m_strUUID));
        }
        else
        {
            int result = m_cLogicNetCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

            array_data.push_back(value_int(VnetRpcError(result))); 
            array_data.push_back(value_string(m_cLogicNetCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }


    void VNetConfigLogicNetIPXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CLogicNetworkIPv4RangeMsg m_cLogicNetIPCFGXMLRPC;
        /************************************************************************
         * LogicNetVlanData的配置：
         *      <logicnetVlan>
         *          <logicnet_uuid> </logicnet_uuid>
         *          <logicnet_ipstart> </logicnet_ipstart>
         *          <logicnet_maskstart>  </logicnet_maskstart>
         *          <logicnet_ipend> </logicnet_ipend>
         *          <logicnet_maskend>  </logicnet_maskend>              
         *          <logicnet_oper> </logicnet_oper>
         *      </logicnetVlan>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string logicnet_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_uuid",num_param, logicnet_uuid);
        m_cLogicNetIPCFGXMLRPC.m_strLogicNetworkUUID = logicnet_uuid;

        string logicnet_ipstart = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_ipstart",num_param, logicnet_ipstart);
        m_cLogicNetIPCFGXMLRPC.m_strIPv4Start = logicnet_ipstart;

        string logicnet_maskstart = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_maskstart",num_param, logicnet_maskstart);
        m_cLogicNetIPCFGXMLRPC.m_strIPv4StartMask = logicnet_maskstart;

        string logicnet_ipend = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_ipend",num_param, logicnet_ipend);
        m_cLogicNetIPCFGXMLRPC.m_strIPv4End = logicnet_ipend;

        string logicnet_maskend = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_maskend",num_param, logicnet_maskend);
        m_cLogicNetIPCFGXMLRPC.m_strIPv4EndMask = logicnet_maskend;   

        int oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG("oper",num_param, oper);
        m_cLogicNetIPCFGXMLRPC.m_dwOper= oper;   
 
        /* 3. 检查参数 */
        if (!StringCheck::CheckNormalName(logicnet_uuid, 1, STR_LEN_64))
        {
            ACK_PARA_INVALID("logicnet_uuid");
            XML_RPC_RETURN();
        }

        if(!m_cLogicNetIPCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("logicnet_ip_oper");
            XML_RPC_RETURN();
        }

        if(!m_cLogicNetIPCFGXMLRPC.IsValidRange())
        {
            ACK_PARA_INVALID("logicnet_iprange");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }
        
        int ret = pvnetlib->do_cfg_logicnet_ip(m_cLogicNetIPCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cLogicNetIPCFGXMLRPC.GetResult() == 0)
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            cout <<"ret value1 is 0"<<endl;
        }
        else
        {
            int result = m_cLogicNetIPCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

            array_data.push_back(value_int(VnetRpcError(result)));  
            array_data.push_back(value_string(m_cLogicNetIPCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }

    void VNetLogicNetID2NameXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CVNetLGNetID2Name m_cLogicNetID2Name;
        /************************************************************************
         * VNetLogicNetID2Name的配置：
         *      <VNetLogicNetID2Name>
         *          <logicnet_name> </logicnet_name>     
         *          <logicnet_username> </logicnet_username>
         *      </VNetLogicNetID2Name>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string logicnet_name =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_name",num_param, logicnet_name);
        m_cLogicNetID2Name.m_strLGName = logicnet_name;

        string logicnet_username = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG("logicnet_username",num_param, logicnet_username);
        m_cLogicNetID2Name.m_strUsername = logicnet_username;   

 
        /* 3. 检查参数 */
        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }
        
        int ret = pvnetlib->do_cfg_logicnet_id2name(m_cLogicNetID2Name);
        if(VNET_XMLRPC_SUCCESS == ret && m_cLogicNetID2Name.GetResult() == 0)
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_string(m_cLogicNetID2Name.m_strUuid));
        }
        else
        {
            int result = m_cLogicNetID2Name.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

            array_data.push_back(value_int(VnetRpcError(result)));  
            array_data.push_back(value_string(m_cLogicNetID2Name.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }

}


