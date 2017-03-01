/******************************************************************************
*   Copyright   (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*   
*   �ļ����ƣ�vnet_xmlrpc_kernalport_method.cpp
*   �ļ���ʶ��
*   ����ժҪ��VNetConfigKernalPortXMLRPC���ʵ���ļ�
*   ��ǰ�汾��1.0
*   ��      �ߣ���־ΰ
*   ������ڣ�2013��3��5��
*
*   �޸ļ�¼1��
*           �޸����ڣ�2013��3��5��
*           �� ��   �ţ�V1.0
*           �� ��   �ߣ���־ΰ
*           �޸����ݣ�����
******************************************************************************/

#include "vnet_xmlrpc_kernalport_method.h"  
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

    /*****************************************************************************/
    /**
    @brief ��������: KERLNALPORT����ע��XML�����ӿ�

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
    void VNetConfigKernalPortXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CKerNalPortMsg             m_KernalportCFGXMLRPC;
        /************************************************************************
         * KernalPort�����ã�
         *      <KernalPort>
         *          <kernal_name>        </kernal_name>  
         *          <kernal_uuid>        </kernal_uuid> 
         *          <kernal_vnauuid>        </kernal_vnauuid>
         *          <is_dhcp>            </is_dhcp>
         *          <pg_uuid>            </pg_uuid>
         *          <kernal_ip>          </kernal_ip>
         *          <kernal_mask>        </kernal_mask>
         *          <kernal_type>        </kernal_type>
         *          <kernal_oper>        </kernal_oper>
         
         *          <switch_name>     </switch_name>  
         *      </KernalPort>
        
         ************************************************************************/
        /* 1. ������Ȩ */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. �������� */
        string  kernal_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("kernal_name",num_param, kernal_name);
        m_KernalportCFGXMLRPC.m_strName = kernal_name;

        string  kernal_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("kernal_uuid",num_param, kernal_uuid);
        m_KernalportCFGXMLRPC.m_strUUID = kernal_uuid;

        string  vna_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("vna_uuid",num_param, vna_uuid);
        m_KernalportCFGXMLRPC.m_strVNAUUID= vna_uuid;

        string pg_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("pg_uuid",num_param, pg_uuid);
        m_KernalportCFGXMLRPC.m_strPGUUID= pg_uuid;

        string kernal_ip = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("kernal_ip",num_param, kernal_ip);
        m_KernalportCFGXMLRPC.m_strip= kernal_ip;

        string kernal_mask = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("kernal_mask",num_param, kernal_mask);
        m_KernalportCFGXMLRPC.m_strmask= kernal_mask;

        int kernal_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("kernal_type",num_param, kernal_type);
        m_KernalportCFGXMLRPC.m_nType = kernal_type;

        int kernal_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("kernal_oper",num_param, kernal_oper);
        m_KernalportCFGXMLRPC.m_dwOper = kernal_oper;

        string switch_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("switch_name",num_param, switch_name);
        m_KernalportCFGXMLRPC.m_strSwitchName= switch_name;

        if(kernal_ip == "")
        {
            m_KernalportCFGXMLRPC.m_isdhcp=1;
        }
        else
        {
            m_KernalportCFGXMLRPC.m_isdhcp=0;
        }
        cout <<"will check para!"<<endl;
        /* 3. ������ */
        if(0 == m_KernalportCFGXMLRPC.IsValidOper())
        {
            ACK_PARA_INVALID("kernal_oper");
            XML_RPC_RETURN();
        }

        cout <<"will CVNetLibNet::GetInstance!"<<endl;
        /* 4. ��Ϣת������Ӧ�� */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        cout <<"will do_cfg_kernal_port!"<<endl;
        int ret = pvnetlib->do_cfg_kernal_port(m_KernalportCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_KernalportCFGXMLRPC.GetResult() == 0)
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
           int result = m_KernalportCFGXMLRPC.GetResult();
           array_data.push_back(value_int(VnetRpcError(result)));  
           array_data.push_back(value_string(m_KernalportCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC����  */
        XML_RPC_RETURN();
    }


    /*****************************************************************************/
    /**
    @brief ��������: virtual portͨ������ע��XML�����ӿ�

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
    void VNetConfigWildCastVirtualPortXMLRPC::MethodEntry(xmlrpc_c::paramList const& paramList, 
        xmlrpc_c::value *   const   retval)
    {
        int                         num_param = 1;
        vector<xmlrpc_c::value>     array_data;
        value_array                *array_result = NULL;
        CWildcastCreateVPortCfgMsg  m_vportCFGXMLRPC;
        CWildcastCreateVPortCfgAckMsg  m_ackvportCFGXMLRPC;
        /************************************************************************
         * virtualportͨ��
         *      <virtualPort>
         *          <port_name>        </port_name>  
         *          <port_uuid>        </port_uuid>  
         *          <port_type>        </port_type>
         *          <kernal_type>        </kernal_type>
         *          <KernelPgUuid>     </KernelPgUuid>
         *          <SwitchUuid>       <SwitchUuid>
         *          <kernal_oper>        </kernal_oper>
         *      </virtualPort>
        
         ************************************************************************/
        /* 1. ������Ȩ */
        AUTHORIZE(AuthRequest::NET, AuthRequest::CREATE);

        /* 2. �������� */
        string  port_name = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("port_name",num_param, port_name);
        m_vportCFGXMLRPC.m_strPortName = port_name;

        string  port_uuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("port_uuid",num_param, port_uuid);
        m_vportCFGXMLRPC.m_strUuid = port_uuid;

        int  port_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("port_type",num_param, port_type);
        m_vportCFGXMLRPC.m_nPortType = port_type;

        int kernal_type = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("kernal_type",num_param, kernal_type);
        m_vportCFGXMLRPC.m_nKernelType = kernal_type;

        if(port_type == 3)
        {
            m_vportCFGXMLRPC.m_nIsHasKenelType = 1;
        }
        else
        {
            m_vportCFGXMLRPC.m_nIsHasKenelType = 0;
        }
        
        string KernelPgUuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("KernelPgUuid",num_param, KernelPgUuid);
        m_vportCFGXMLRPC.m_strKernelPgUuid= KernelPgUuid;

        string SwitchUuid = xmlrpc_c::value_string(paramList.getString(num_param ++));
        VNETDBG_XMLRPC("SwitchUuid",num_param, SwitchUuid);
        m_vportCFGXMLRPC.m_strSwitchUuid= SwitchUuid;

        m_vportCFGXMLRPC.m_nIsDhcp=1;
            
        int port_oper = xmlrpc_c::value_int(paramList.getInt(num_param ++));
        VNETDBG_XMLRPC("port_oper",num_param, port_oper);
        m_vportCFGXMLRPC.m_nOper = port_oper;


        /* 3. ������ */

        /* 4. ��Ϣת������Ӧ�� */
        CVNetLibNet *pvnetlib = CVNetLibNet::GetInstance();
        if(pvnetlib == NULL)
        {
            ACK_VNETLIB_NOTWORK();
            XML_RPC_RETURN();
        }

        int ret = pvnetlib->do_cfg_wildcast_vport(m_vportCFGXMLRPC,m_ackvportCFGXMLRPC);
        if(VNET_XMLRPC_SUCCESS == ret && m_ackvportCFGXMLRPC.GetResult() == 0)
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(m_ackvportCFGXMLRPC.m_strUuid));
        }
        else
        {
            int result = m_ackvportCFGXMLRPC.GetResult();
            array_data.push_back(value_int(VnetRpcError(result)));  
            array_data.push_back(value_string(m_ackvportCFGXMLRPC.GetResultinfo()));
        }
        /* 5. XML-RPC����  */
        XML_RPC_RETURN();
    }
}

