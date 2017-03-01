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

#include "vnet_xmlrpc_switch_method.h"  
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


int put_string_to_vec(string text,vector<string>& vec)
{   
    if(text.empty())
    {
        return 0;
    }
    
    vector<string>::iterator itor;
    string word;
    while(1)                
    {               
        int pos = text.find(',');
        if( pos==0)
        {       
            text=text.substr(1);
            continue;
        }       
        if( pos<0)
        {
           //cout << "will push_back "<<text<<endl;
            vec.push_back(text);
            break;
        }       
        word = text.substr(0,pos);
        text = text.substr(pos+1);
        vec.push_back(word);
    }
    for(itor = vec.begin(); itor!= vec.end();itor++ )
    {
        cout << "vec item is "<<*itor <<endl;
    }
    return 0;
}

    /*****************************************************************************/
    /**
    @brief 功能描述: SWITCH基本配置注册XML方法接口

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
    void VNetConfigSwitchXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CSwitchCfgMsg       m_cNetSwitchDataCFGXMLRPC;
        CSwitchCfgAckMsg    m_cNetSwitchAckDataCFGXMLRPC;

        /************************************************************************
         * Switch的基本配置：
         *      <switch_base>
         *          <switch_name> </switch_name>
         *          <switch_uuid> </switch_uuid>
         *          <switch_type> </switch_type>
         #*         <switch_mtu>  </switch_mtu>  去掉
         *          <switch_oper> </switch_oper>
         *          <evb_mode>    </evb_mode>
         *          <max_nics>    </max_nics>
         *          <switch_pguuid> </switch_pguuid>
         *      </switch_base>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  switch_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_name",num_param, switch_name);
        m_cNetSwitchDataCFGXMLRPC.m_strName = switch_name;

        string switch_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_uuid",num_param, switch_uuid);
        m_cNetSwitchDataCFGXMLRPC.m_strUuid = switch_uuid;

        int32 switch_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_type",num_param, switch_type);
        m_cNetSwitchDataCFGXMLRPC.m_nType = switch_type;

#if 0
        int32 switch_mtu = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_mtu",num_param, switch_mtu);
        m_cNetSwitchDataCFGXMLRPC.m_nMTU= switch_mtu;
#endif

        int32 switch_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_oper",num_param, switch_oper);
        m_cNetSwitchDataCFGXMLRPC.m_nOper= switch_oper;

        int32 evb_mode = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("evb_mode",num_param, evb_mode);
        m_cNetSwitchDataCFGXMLRPC.m_nEvbMode= evb_mode; 

        int32 max_nics = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("max_nics",num_param, max_nics);
        m_cNetSwitchDataCFGXMLRPC.m_nMaxVnic= max_nics; 

        string switch_pguuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_pguuid",num_param, switch_uuid);
        m_cNetSwitchDataCFGXMLRPC.m_strPGUuid = switch_pguuid;
        
        
        /* 3. 检查参数 */

        if(EN_ADD_SWITCH > switch_oper || EN_MODIFY_SWITCH < switch_oper)
        {
            ACK_PARA_INVALID("switch_oper");
            XML_RPC_RETURN();
        }

#if 0  
        if(SOFT_VIR_SWITCH_TYPE > switch_type || PHYSICAL_SWITCH_TYPE < switch_type)
        {
            ACK_PARA_INVALID("switch_type");
            XML_RPC_RETURN();
        }
#endif          
        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_switch_base(m_cNetSwitchDataCFGXMLRPC,m_cNetSwitchAckDataCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetSwitchAckDataCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(m_cNetSwitchAckDataCFGXMLRPC.m_strUuid));
        }
        else
        {
           int result = m_cNetSwitchAckDataCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

           array_data.push_back(value_int(VnetRpcError(result))); 
           array_data.push_back(value_string(m_cNetSwitchAckDataCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }


    /*****************************************************************************/
    /**
    @brief 功能描述: SWITCH PORT注册XML方法接口

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
    void VNetConfigSwitchPortXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        
        CSwitchPortCfgMsg m_cNetSwitchPortCFGXMLRPC;
        CSwitchCfgAckMsg  m_cNetSwitchAckDataCFGXMLRPC;

        /************************************************************************
         * Switch port的基本配置：
         *      <switch_port>
         *          <switch_name> </switch_name>
         *          <switch_uuid> </switch_uuid>
         *          <switch_type> </switch_type>
         *          <switch_vnauuid></switch_vnauuid>
         *          <switch_bondmode> </switch_bondmode>
         *          <switch_oper> </switch_oper>
         *          <switch_reqid></switch_reqid>
         *          <port_num></port_num>
         *          <port_item>
         *               <port_name></port_name>
         *          </port_item>
         
         *      </switch_port>
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  switch_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_name",num_param, switch_name);
        m_cNetSwitchPortCFGXMLRPC.m_strSwName  = switch_name;

        string switch_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_uuid",num_param, switch_uuid);
        m_cNetSwitchPortCFGXMLRPC.m_strSwUuid = switch_uuid;

        int32 switch_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_type",num_param, switch_type);
        m_cNetSwitchPortCFGXMLRPC.m_nSwType = switch_type;

        string switch_vnauuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_vnauuid",num_param, switch_vnauuid);
        m_cNetSwitchPortCFGXMLRPC.m_strVnaUuid = switch_vnauuid;

        int32 switch_bondmode = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_bondmode",num_param, switch_bondmode);
        m_cNetSwitchPortCFGXMLRPC.m_nBondMode= switch_bondmode; 

        int32 switch_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_oper",num_param, switch_oper);
        m_cNetSwitchPortCFGXMLRPC.m_nOper= switch_oper; 

        string switch_reqid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_uuid",num_param, switch_uuid);
        m_cNetSwitchPortCFGXMLRPC.m_strReqId = switch_reqid;

        int32 port_num = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("port_num",num_param, port_num);
        m_cNetSwitchPortCFGXMLRPC.m_nPortNum = port_num;

        m_cNetSwitchPortCFGXMLRPC.m_vecHostPort.clear();
        
        cout <<"will get sw port!"<<endl;
 
        string port_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("port_name",num_param, port_name);
        put_string_to_vec(port_name,m_cNetSwitchPortCFGXMLRPC.m_vecHostPort);

        cout <<"will check oper"<<endl;
        /* 3. 检查参数 */
        if(EN_ADD_SWITCH_PORT > switch_oper || EN_DEL_SWITCH_PORT < switch_oper)
        {
            ACK_PARA_INVALID("switch_oper");
            XML_RPC_RETURN();
        }
        
#if 0
        if(SOFT_VIR_SWITCH_TYPE > switch_type || PHYSICAL_SWITCH_TYPE < switch_type)
        {
            ACK_PARA_INVALID("switch_type");
            XML_RPC_RETURN();
        }
#endif      
        cout <<"will CVNetLibNet::GetInstance!"<<endl; 

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            cout <<"CVNetLibNet::GetInstance failed"<<endl;
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }
        
        cout<<"will pvnetlib->do_cfg_switch_port"<<endl;
        int ret = pvnetlib->do_cfg_switch_port(m_cNetSwitchPortCFGXMLRPC,m_cNetSwitchAckDataCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetSwitchAckDataCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
           int result = m_cNetSwitchAckDataCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

           array_data.push_back(value_int(VnetRpcError(result))); 
            array_data.push_back(value_string(m_cNetSwitchAckDataCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }

    /*****************************************************************************/
    /**
    @brief 功能描述: SWITCH通配基本配置注册XML方法接口

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
    void VNetConfigWildCastSwitchXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;

        CWildcastSwitchCfgMsg       m_cNetSwitchDataCFGXMLRPC;
        CWildcastSwitchCfgAckMsg    m_cNetSwitchAckDataCFGXMLRPC;

        /************************************************************************
         * Switch的基本配置：
         *      <switch_base>
         *          <switch_name> </switch_name>
         *          <switch_uuid> </switch_uuid>
         *          <switch_type> </switch_type>
         *          <switch_mtu>  </switch_mtu>
         *          <switch_oper> </switch_oper>
         *          <evb_mode>    </evb_mode>
         *          <max_nics>    </max_nics>
         *          <switch_pguuid> </switch_pguuid>
         *      </switch_base>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  switch_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_name",num_param, switch_name);
        m_cNetSwitchDataCFGXMLRPC.m_strName = switch_name;

        string switch_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_uuid",num_param, switch_uuid);
        m_cNetSwitchDataCFGXMLRPC.m_strUuid = switch_uuid;

        int32 switch_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_type",num_param, switch_type);
        m_cNetSwitchDataCFGXMLRPC.m_nType = switch_type;

#if 0
        int32 switch_mtu = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_mtu",num_param, switch_mtu);
        m_cNetSwitchDataCFGXMLRPC.m_nMTU= switch_mtu;
#endif       

        int32 switch_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("switch_oper",num_param, switch_oper);
        m_cNetSwitchDataCFGXMLRPC.m_nOper= switch_oper;

        int32 evb_mode = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("evb_mode",num_param, evb_mode);
        m_cNetSwitchDataCFGXMLRPC.m_nEvbMode= evb_mode; 

        int32 max_nics = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("max_nics",num_param, max_nics);
        m_cNetSwitchDataCFGXMLRPC.m_nMaxVnic= max_nics; 

        string switch_pguuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_pguuid",num_param, switch_pguuid);
        m_cNetSwitchDataCFGXMLRPC.m_strPGUuid = switch_pguuid;
        
        
        /* 3. 检查参数 */

        if(EN_ADD_SWITCH > switch_oper || EN_MODIFY_SWITCH < switch_oper)
        {
            ACK_PARA_INVALID("switch_oper");
            XML_RPC_RETURN();
        }
  
        if(SOFT_VIR_SWITCH_TYPE > switch_type || PHYSICAL_SWITCH_TYPE < switch_type)
        {
            ACK_PARA_INVALID("switch_type");
            XML_RPC_RETURN();
        }
          
        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_wildcast_switch_base(m_cNetSwitchDataCFGXMLRPC,m_cNetSwitchAckDataCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cNetSwitchAckDataCFGXMLRPC.GetResult() == 0)
        {
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(m_cNetSwitchAckDataCFGXMLRPC.m_strUuid));
        }
        else
        {
           int result = m_cNetSwitchAckDataCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

           array_data.push_back(value_int(VnetRpcError(result))); 
           array_data.push_back(value_string(m_cNetSwitchAckDataCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }

    /*****************************************************************************/
    /**
    @brief 功能描述: SWITCH通配端口注册XML方法接口

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
                    VNetConfigWildcastSwitchPortXMLRPC
    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    void VNetConfigWildcastSwitchPortXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
        {
            int                         num_param = 1;
            vector<xmlrpc_c::value>     array_data;
            value_array                *array_result = NULL;
            CWildcastSwitchPortCfgMsg   m_cNetSwitchPortCFGXMLRPC;
            CWildcastSwitchCfgAckMsg    m_cNetSwitchAckDataCFGXMLRPC;
            /************************************************************************
             * Switch port的基本配置：
             *      <switch_port>
             *          <switch_name> </switch_name>
             *          <switch_uuid> </switch_uuid>
             *          <switch_type> </switch_type>
             *          <switch_PortType><switch_PortType>
             *          <switch_bondmode> </switch_bondmode>
             *          <switch_oper> </switch_oper>
             
             *          <port_num></port_num>
             *          <port_item>
             *               <port_name></port_name>
             *          </port_item>
             
             *      </switch_port>
             ************************************************************************/
            /* 1. 操作授权 */
            AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);
        
            /* 2. 解析参数 */
            string  switch_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
            VNETDBG_XMLRPC("switch_name",num_param, switch_name);
            m_cNetSwitchPortCFGXMLRPC.m_strSwName  = switch_name;
        
            string switch_uuid =  xmlrpc_c::value_string(paramList.getString(num_param ++));
            VNETDBG_XMLRPC("switch_uuid",num_param, switch_uuid);
            m_cNetSwitchPortCFGXMLRPC.m_strSwUuid = switch_uuid;
        
            int32 switch_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
            VNETDBG_XMLRPC("switch_type",num_param, switch_type);
            m_cNetSwitchPortCFGXMLRPC.m_nSwType = switch_type;
        
            int32 switch_PortType = xmlrpc_c::value_int(paramList.getInt(num_param ++));
            VNETDBG_XMLRPC("switch_PortType",num_param, switch_PortType);
            m_cNetSwitchPortCFGXMLRPC.m_nPortType = switch_PortType;
        
            int32 switch_bondmode = xmlrpc_c::value_int(paramList.getInt(num_param ++));
            VNETDBG_XMLRPC("switch_bondmode",num_param, switch_bondmode);
            m_cNetSwitchPortCFGXMLRPC.m_nBondMode= switch_bondmode; 
        
            int32 switch_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
            VNETDBG_XMLRPC("switch_oper",num_param, switch_oper);
            m_cNetSwitchPortCFGXMLRPC.m_nOper= switch_oper; 
        
#if 0
            string switch_reqid = xmlrpc_c::value_string(paramList.getString(num_param ++));
            VNETDBG_XMLRPC("switch_uuid",num_param, switch_uuid);
#endif   

            m_cNetSwitchPortCFGXMLRPC.m_strReqId = "0";
                
            int32 port_num = xmlrpc_c::value_int(paramList.getInt(num_param ++));
            VNETDBG_XMLRPC("port_num",num_param, port_num);
            m_cNetSwitchPortCFGXMLRPC.m_nPortNum = port_num;

            m_cNetSwitchPortCFGXMLRPC.m_vPort.clear();
            
            cout <<"will get sw port!"<<endl;
        
                string port_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
                VNETDBG_XMLRPC("port_name",num_param, port_name);
            put_string_to_vec(port_name,m_cNetSwitchPortCFGXMLRPC.m_vPort);



            cout <<"will CVNetLibNet::GetInstance!"<<endl;  
            /* 4. 消息转发并等应答 */
            CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
            if(pvnetlib == NULL)
            {
                cout<<"CVNetLibNet::GetInstance is null"<<endl;
                ACK_VNETLIB_NOTWORK();
                XML_RPC_RETURN();
            }

            cout<<"will pvnetlib->do_cfg_wildcast_switch_port"<<endl;
            int ret = pvnetlib->do_cfg_wildcast_switch_port(m_cNetSwitchPortCFGXMLRPC,m_cNetSwitchAckDataCFGXMLRPC);
            if(VNET_XMLRPC_SUCCESS == ret && m_cNetSwitchAckDataCFGXMLRPC.GetResult() == 0)
            {
                cout <<"ret value1 is 0"<<endl;
                array_data.push_back(value_int(TECS_SUCCESS));
            }
            else
            {
               int result = m_cNetSwitchAckDataCFGXMLRPC.GetResult();
        
                cout <<"ret value1 is "<<result<<endl;
                cout <<"ret value2 is "<<VnetRpcError(result)<<endl;
        
               array_data.push_back(value_int(VnetRpcError(result))); 
                array_data.push_back(value_string(m_cNetSwitchAckDataCFGXMLRPC.GetResultinfo()));
            }
            /* 5. XML-RPC返回  */
            XML_RPC_RETURN();
        }


    void VNetConfigWildcastTaskSwitchXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
        {
            int                         num_param = 1;
            vector<xmlrpc_c::value>     array_data;
            value_array                *array_result = NULL;
            
            /************************************************************************
             * Del Switch Tarsk的基本配置：
             *      <del_switch_task>
             *          <task_type><task_type>
             *          <task_uuid><task_uuid>
             *      </del_switch_task>
             ************************************************************************/
             /* 1. 操作授权 */
            AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);
        
            /* 2. 解析参数 */
            int     task_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
            VNETDBG_XMLRPC("task_type",num_param, task_type);

            if(task_type != VNET_LIB_WC_TASK_SWITCH && 
               task_type != VNET_LIB_WC_TASK_VPORT  && 
               task_type != VNET_LIB_WC_TASK_LOOP)
            {
                ACK_PARA_INVALID("task_type");
                XML_RPC_RETURN();
            }

            
            string  task_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
            VNETDBG_XMLRPC("task_uuid",num_param, task_uuid);
            m_wc_taskdel.m_strUuid  = task_uuid;

            m_wc_taskdel.m_nOper = 2;

            /* 4. 消息转发并等应答 */
            CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
            if(pvnetlib == NULL)
            {
                cout<<"CVNetLibNet::GetInstance is null"<<endl;
                ACK_VNETLIB_NOTWORK();
                XML_RPC_RETURN();
            }

            int ret = pvnetlib->do_cfg_wildcast_delTask(m_wc_taskdel,m_wc_taskdel_ack,task_type);
            if(VNET_XMLRPC_SUCCESS == ret && m_wc_taskdel_ack.GetResult() == 0)
            {
                cout <<"ret value1 is 0"<<endl;
                array_data.push_back(value_int(TECS_SUCCESS));
            }
            else
            {
               int result = m_wc_taskdel_ack.GetResult();
        
                cout <<"ret value1 is "<<result<<endl;
                cout <<"ret value2 is "<<VnetRpcError(result)<<endl;
        
               array_data.push_back(value_int(VnetRpcError(result))); 
               array_data.push_back(value_string(m_wc_taskdel_ack.GetResultinfo()));
            }
            /* 5. XML-RPC返回  */
            XML_RPC_RETURN();
            
        }
        
       void VNetVNAClearXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
             xmlrpc_c::value *   const   retval)
        {
            int                         num_param = 1;
            vector<xmlrpc_c::value>     array_data;
            value_array                *array_result = NULL;
            CVNetVnaDelete          m_VNADelete;
            CVNetVnaDeleteAck       m_VNADeleteAck;
            /************************************************************************
             * VNA DELETE的基本配置：
             *      <delete_vna>
             *          <vna_uuid><vna_uuid>
             *      </delete_vna>
             ************************************************************************/
             /* 1. 操作授权 */
            AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);
        
            /* 2. 解析参数 */
            string  vna_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
            VNETDBG_XMLRPC("vna_uuid",num_param, vna_uuid);
            m_VNADelete.m_strUuid= vna_uuid;


            /* 4. 消息转发并等应答 */
            CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
            if(pvnetlib == NULL)
            {
                cout<<"CVNetLibNet::GetInstance is null"<<endl;
                ACK_VNETLIB_NOTWORK();
                XML_RPC_RETURN();
            }

            int ret = pvnetlib->do_vna_clear(m_VNADelete,m_VNADeleteAck);
            if(VNET_XMLRPC_SUCCESS == ret && m_VNADeleteAck.GetResult() == 0)
            {
                cout <<"ret value1 is 0"<<endl;
                array_data.push_back(value_int(TECS_SUCCESS));
            }
            else
            {
               int result = m_VNADeleteAck.GetResult();
        
                cout <<"ret value1 is "<<result<<endl;
                cout <<"ret value2 is "<<VnetRpcError(result)<<endl;
        
               array_data.push_back(value_int(VnetRpcError(result))); 
               array_data.push_back(value_string(m_VNADeleteAck.GetResultinfo()));
            }
            /* 5. XML-RPC返回  */
            XML_RPC_RETURN();
            
        }
}

