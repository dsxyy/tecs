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

#ifndef VNET_XMLRPC_SWITCH_METHOD_H
#define VNET_XMLRPC_SWITCH_METHOD_H

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
    @brief 功能描述: switch Data配置信息
    @li @b 其它说明：无
    */
    class VNetConfigSwitchXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigSwitchXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigSwitchXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetConfigSwitchXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigSwitchXMLRPC);
    };

    class VNetConfigSwitchPortXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigSwitchPortXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigSwitchPortXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetConfigSwitchPortXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigSwitchPortXMLRPC);
    };

    class VNetConfigWildCastSwitchXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigWildCastSwitchXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigWildCastSwitchXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetConfigWildCastSwitchXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigWildCastSwitchXMLRPC);
    };

    class VNetConfigWildcastSwitchPortXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
        /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigWildcastSwitchPortXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigWildcastSwitchPortXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetConfigWildcastSwitchPortXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigWildcastSwitchPortXMLRPC);
    };

    class VNetConfigWildcastTaskSwitchXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
        /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigWildcastTaskSwitchXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigWildcastTaskSwitchXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetConfigWildcastTaskSwitchXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:
        CWildcastTaskDelMsg         m_wc_taskdel;
        CWildcastTaskDelAckMsg      m_wc_taskdel_ack;

        DISALLOW_COPY_AND_ASSIGN(VNetConfigWildcastTaskSwitchXMLRPC);
    };

    class VNetVNAClearXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
        /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetVNAClearXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetVNAClearXMLRPC";
            _user_id = -1;
            _upool = upool;
        }
        ~VNetVNAClearXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetVNAClearXMLRPC);
    };
}  /* end namespace zte_tecs */

#endif /* end TC_VNET_CONFIG_H */


