/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_xmlrpc_sdn_method.h
* �ļ���ʶ��
* ����ժҪ��VNetSdnXMLRPC��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�2013��9��6��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�
*     �޸����ݣ�����
******************************************************************************/

#ifndef VNET_XMLRPC_SDN_METHOD_H
#define VNET_XMLRPC_SDN_METHOD_H

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
    @brief ��������: Sdn Data������Ϣ
    @li @b ����˵������
    */
    class VNetConfigSdnDataXMLRPC :  public TecsRpcMethod,public VNetMapValueXMPRPC
    {
      /*******************************************************************************
        * 1. public section
        *******************************************************************************/
    public:
        VNetConfigSdnDataXMLRPC(UserPool *upool=0)
        {
            _method_name = "VNetConfigSdnDataXMLRPC";
            _user_id = -1;
            _upool = upool;
            m_iAction = EN_INVALID_SDN_OPER;
        }
        ~VNetConfigSdnDataXMLRPC(){};

        void MethodEntry( xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retval);

    public:
        int32 m_iAction;

    private:
        CSdnCfgMsg m_cSdnDataCFGXMLRPC;

        DISALLOW_COPY_AND_ASSIGN(VNetConfigSdnDataXMLRPC);
    };    

}  /* end namespace zte_tecs */

#endif /* end TC_VNET_CONFIG_H */


