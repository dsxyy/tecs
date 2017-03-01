/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_xmlrpc_kernalport_method.h
* �ļ���ʶ��
* ����ժҪ��VNetKernalPortXMLRPC��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2013��4��2��
*     �� �� �ţ�V1.0
*     �� �� �ˣ���־ΰ
*     �޸����ݣ�����
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
    @brief ��������: NetPlane Data������Ϣ
    @li @b ����˵������
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


