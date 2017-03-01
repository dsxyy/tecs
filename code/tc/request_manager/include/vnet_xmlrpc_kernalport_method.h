/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_xmlrpc_kernalport_method.h
* 文件标识：
* 内容摘要：VNetKernalPortXMLRPC类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：
*
* 修改记录1：
*     修改日期：2013年4月2日
*     版 本 号：V1.0
*     修 改 人：陈志伟
*     修改内容：创建
******************************************************************************/

#ifndef VNET_XMLRPC_KERNALPORT_METHOD_H
#define VNET_XMLRPC_KERNALPORT_METHOD_H

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
    class VNetConfigKernalPortXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigKernalPortXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigKernalPortXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigKernalPortXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:
        //CKerNalPortMsg m_KernalportCFGXMLRPC;

        DISALLOW_COPY_AND_ASSIGN(VNetConfigKernalPortXMLRPC);
    };

    class VNetConfigWildCastVirtualPortXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigWildCastVirtualPortXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigWildCastVirtualPortXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_NETPLANE_OPER;
        }
        ~VNetConfigWildCastVirtualPortXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:
        //CWildcastCreateVPortCfgMsg   m_vportCFGXMLRPC;

        DISALLOW_COPY_AND_ASSIGN(VNetConfigWildCastVirtualPortXMLRPC);
    };

}  /* end namespace zte_tecs */

#endif /* end VNET_XMLRPC_KERNALPORT_METHOD_H */


