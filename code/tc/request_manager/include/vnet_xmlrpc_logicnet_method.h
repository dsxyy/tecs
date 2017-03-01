/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_xmlrpc_logicnet_method.h
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
    @brief ��������: NetPlane Data������Ϣ
    @li @b ����˵������
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


