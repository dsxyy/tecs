/******************************************************************************
*   Copyright   (c) 2011，中兴通讯股份有限公司，All rights reserved.
*   
*   文件名称：vnet_xmlrpc_loopport_method.cpp
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

#include "vnet_xmlrpc_loopport_method.h"  
#include "user_pool.h"
#include "authrequest.h"


using namespace xmlrpc_c;
namespace   zte_tecs {

extern int32   vnet_print_xmlrpc ;
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
    @brief 功能描述: LoopPort配置注册XML方法接口

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
    void VNetConfigLoopPortXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CLoopPortMsg m_cLoopPortCFGXMLRPC;
        /************************************************************************
         * LoopPortData的配置：
         *      <loop_port>
         *          <vnauuid>      </vnauuid>
         *          <portuuid>     </portuuid>
         *          <name>         </name>
         *          <oper>         </oper>
         *      </loop_port>
        
         ************************************************************************/
        /* 1. 操作授权 */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. 解析参数 */
        string  vnauuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("vnauuid",num_param, vnauuid);
        m_cLoopPortCFGXMLRPC.m_strvnaUUID = vnauuid;

        string  portuuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("portuuid",num_param, portuuid);
        m_cLoopPortCFGXMLRPC.m_strportUUID = portuuid;

        string  name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("name",num_param, name);
        m_cLoopPortCFGXMLRPC.m_strName = name;

        int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("oper",num_param, oper);
        m_cLoopPortCFGXMLRPC.m_dwOper= oper;

        /* 3. 检查参数 */
        if(0 == m_cLoopPortCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("loopport_oper");
            XML_RPC_RETURN();
        }

        /* 4. 消息转发并等应答 */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_loop_port(m_cLoopPortCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cLoopPortCFGXMLRPC.GetResult() == 0)
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
           int result = m_cLoopPortCFGXMLRPC.GetResult();
           array_data.push_back(value_int(VnetRpcError(result)));  
           array_data.push_back(value_string(m_cLoopPortCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }


  /*****************************************************************************/
  /**
  @brief 功能描述: 通配LoopPort配置注册XML方法接口

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
  void VNetConfigWildCastLoopPortXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
      xmlrpc_c::value *   const   retval)
  {
      int                         num_param = 1;
      vector<xmlrpc_c::value>     array_data;
      value_array                *array_result = NULL;
      CWildcastLoopbackCfgMsg m_cLoopPortCFGXMLRPC;
      /************************************************************************
       * WildCastLoopPortData的配置：
       *      <wildcastloop_port>
       *          <portuuid>      </portuuid>
       *          <portname>      </portname>
       *          <isloop>        </isloop>
       *          <oper>          </oper>
       *          <reqid>          </reqid>
       *      </wildcastloop_port>
      
       ************************************************************************/
      /* 1. 操作授权 */
      AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

      /* 2. 解析参数 */
      string  portuuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
      VNETDBG_XMLRPC("portuuid",num_param, portuuid);
      m_cLoopPortCFGXMLRPC.m_strUuid = portuuid;

      string  portname = xmlrpc_c::value_string(paramList.getString(num_param ++));
      VNETDBG_XMLRPC("portname",num_param, portname);
      m_cLoopPortCFGXMLRPC.m_strPortName = portname;

      int32  isloop = xmlrpc_c::value_int(paramList.getInt(num_param ++));
      VNETDBG_XMLRPC("isloop",num_param, isloop);
      m_cLoopPortCFGXMLRPC.m_nIsLoop = isloop;

      int32 oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
      VNETDBG_XMLRPC("oper",num_param, oper);
      m_cLoopPortCFGXMLRPC.m_nOper= oper;


      /* 3. 检查参数 */
      if(0 == m_cLoopPortCFGXMLRPC.IsValidOper())
      {
          ACK_PARA_INVALID("wildcast loopport_oper");
          XML_RPC_RETURN();
      }

      /* 4. 消息转发并等应答 */
      CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
      if(pvnetlib == NULL)
      {
          ACK_VNETLIB_NOTWORK();
          XML_RPC_RETURN();
      }

      int ret = pvnetlib->do_cfg_wildcast_loop_port(m_cLoopPortCFGXMLRPC);
      if(VNET_XMLRPC_SUCCESS == ret && m_cLoopPortCFGXMLRPC.GetResult() == 0)
      {
          array_data.push_back(value_int(TECS_SUCCESS));
          array_data.push_back(value_string(m_cLoopPortCFGXMLRPC.m_strUuid));
      }
      else
      {
          int result = m_cLoopPortCFGXMLRPC.GetResult();
          array_data.push_back(value_int(VnetRpcError(result)));  
          array_data.push_back(value_string(m_cLoopPortCFGXMLRPC.GetResultinfo()));
      }
      /* 5. XML-RPC返回  */
      XML_RPC_RETURN();
  }

}

