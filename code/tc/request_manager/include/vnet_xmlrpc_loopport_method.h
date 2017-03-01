/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_xmlrpc_loopport_method.h
* �ļ���ʶ��
* ����ժҪ��VNetNetplaneXMLRPC��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2013��3��3��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�
*     �޸����ݣ�����
******************************************************************************/

#ifndef VNET_XMLRPC_LOOPPORT_METHOD_H
#define VNET_XMLRPC_LOOPPORT_METHOD_H

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
    @brief ��������: LoopPort Data������Ϣ
    @li @b ����˵������
    */
    class VNetConfigLoopPortXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigLoopPortXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigLoopPortXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_LOOPPORT_OPER;
        }
        ~VNetConfigLoopPortXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigLoopPortXMLRPC);
    };

   class VNetConfigWildCastLoopPortXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigWildCastLoopPortXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigWildCastLoopPortXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_LOOPPORT_OPER;
        }
        ~VNetConfigWildCastLoopPortXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:

        DISALLOW_COPY_AND_ASSIGN(VNetConfigWildCastLoopPortXMLRPC);
    };

}  /* end namespace zte_tecs */

#endif /* end TC_VNET_CONFIG_H */


