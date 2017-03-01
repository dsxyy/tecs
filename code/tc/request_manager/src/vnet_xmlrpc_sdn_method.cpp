/******************************************************************************
*   Copyright   (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*   
*   �ļ����ƣ�vnet_xmlrpc_sdn_method.cpp
*   �ļ���ʶ��
*   ����ժҪ��VNetConfigSdnVlanXMLRPC���ʵ���ļ�
*   ��ǰ�汾��1.0
*   ��    �ߣ�
*   ������ڣ�2013��9��6��
*
*   �޸ļ�¼1��
*           �޸����ڣ�2013��9��6��
*           �� ��   �ţ�V1.0
*           �� ��   �ߣ�
*           �޸����ݣ�����
******************************************************************************/

#include "vnet_xmlrpc_sdn_method.h"  
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

#define ACK_VNETLIB_SDN()\
    array_data.push_back(value_int(TECS_ERROR_VNET_NOT_WORKING)); \
    array_data.push_back(value_string("Failed, vnetlib is not answered."));


#define ACK_PARA_INVALID(para) \
    array_data.push_back(value_int(TECS_ERR_PARA_INVALID)); \
    ostringstream stross; \
    stross << "Error, invalide parameter :" << para << "."; \
    array_data.push_back(value_string(stross.str()));

    /*****************************************************************************/
    /**
    @brief ��������: SDN����ע��XML�����ӿ�

    @li @b ����б�
    @verbatim
    paramList        �����XML�������������
    @endverbatim
            
    @li @b �����б�
    @verbatim
    retval          ����Ǹ���������ִ�еĽ��
    @endverbatim
                
    @li @b ����ֵ�б�
    @verbatim
    ��
    @endverbatim
                    
    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /*****************************************************************************/
    void VNetConfigSdnDataXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;

        /************************************************************************
         * sdndata�����ã�
         *      <sdn>
         *          <sdn_uuid> </sdn_uuid>
         *          <sdn_conntype> </sdn_conntype>
         *          <sdn_connport> </sdn_connport>
         *          <sdn_connip> </sdn_connip>
         *          <quantum_uuid> </quantum_uuid>
         *          <quantum_serverip>  </quantum_serverip>
         *          <sdn_oper>  </sdn_oper>
         *      </sdn>
        
         ************************************************************************/
        /* 1. ������Ȩ */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. �������� */
        string sdn_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("sdn_uuid", num_param, sdn_uuid);
        m_cSdnDataCFGXMLRPC.m_strSdnUuid = sdn_uuid; 
        
        string sdn_conntype = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("sdn_conntype", num_param, sdn_conntype);
        m_cSdnDataCFGXMLRPC.m_strSdnConnType = sdn_conntype;

        int32 sdn_connport = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("sdn_connport", num_param, sdn_connport);
        m_cSdnDataCFGXMLRPC.m_nSdnConnPort = sdn_connport;

        string sdn_connip = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("quantum_connip", num_param, sdn_connip);
        m_cSdnDataCFGXMLRPC.m_strSdnConnIP = sdn_connip;

        string quantum_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("quantum_uuid", num_param, quantum_uuid);
        m_cSdnDataCFGXMLRPC.m_strQuantumUuid = quantum_uuid;

        string quantum_serverip = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("quantum_serverip", num_param, quantum_serverip);
        m_cSdnDataCFGXMLRPC.m_strQuantumSvrIP = quantum_serverip;

        int32 sdn_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("sdn_oper", num_param, sdn_oper);
        m_cSdnDataCFGXMLRPC.m_nSdnOper = sdn_oper;

        m_cSdnDataCFGXMLRPC.Print();
        cout<<"*************************************************"<<endl;
        
        /* 3. ��Ϣת������Ӧ�� */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_SDN();
            XML_RPC_RETURN();
        }
        
        int ret = pvnetlib->do_cfg_sdn(m_cSdnDataCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_cSdnDataCFGXMLRPC.GetResult() == 0)
        {
        
            array_data.push_back(value_int(TECS_SUCCESS));
            cout <<"ret value1 is 0"<<endl;
            array_data.push_back(value_string(m_cSdnDataCFGXMLRPC.m_strSdnUuid));
        }
        else
        {
            int result = m_cSdnDataCFGXMLRPC.GetResult();

            cout <<"ret value1 is "<<result<<endl;
            cout <<"ret value2 is "<<VnetRpcError(result)<<endl;

            array_data.push_back(value_int(VnetRpcError(result))); 
            array_data.push_back(value_string(m_cSdnDataCFGXMLRPC.GetResultinfo()));
        }
        /* 4. XML-RPC����  */
        XML_RPC_RETURN();
    }
    
}


