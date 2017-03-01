/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_xmlrpc_logicnet_method.h
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

#ifndef VNET_XMLRPC_LOGICNET_METHOD_H
#define VNET_XMLRPC_LOGICNET_METHOD_H

#include "sky.h"
#include "vnet_api.h"
#include "tecs_rpc_method.h"

#include "vnet_libnet.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"

#include "vnet_xmlrpc_netplane_method.h" 

using namespace std;
namespace zte_tecs {

extern  int32   vnet_print_xmlrpc;
#define VNETDBG(name, no, var)    {if(vnet_print_xmlrpc) (cout << "-->" << name << ":" << no << " :: " << var << endl);}
    /**
    @brief 功能描述: NetPlane Data配置信息
    @li @b 其它说明：无
    */
    class VNetConfigLogicNetDataXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigLogicNetDataXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigLogicNetDataXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_LOGIC_NETWORK_OPER;
        }
        ~VNetConfigLogicNetDataXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigLogicNetDataXMLRPC);
    };

    class VNetConfigLogicNetIPXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigLogicNetIPXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigLogicNetIPXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetConfigLogicNetIPXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigLogicNetIPXMLRPC);
    };


    class VNetLogicNetID2NameXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetLogicNetID2NameXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetLogicNetID2NameXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetLogicNetID2NameXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval);

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetLogicNetID2NameXMLRPC);
    };
}  /* end namespace zte_tecs */

#endif /* end VNET_XMLRPC_LOGICNET_METHOD_H */


