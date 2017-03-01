/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_schedule.cpp
* �ļ���ʶ��
* ����ժҪ��CVNMQuery��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��3��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/3/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
******************************************************************************/
#include "vnet_error.h"
#include "db_config.h"
#include "vnm_pub.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_db_schedule.h"
#include "vnm_schedule.h"
#include "vnm_query.h"

namespace zte_tecs
{

#if 1
int g_vnmquery_dbg_flag = 0;
int g_vnmquery_msg_during_flag = 0;
CVNMQuery *CVNMQuery::s_pInstance = NULL;

CVNMQuery::CVNMQuery()
{
    m_pMU = NULL;
    m_bOpenDbgInf = FALSE;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_pLogicNetworkMgr = NULL;
};

CVNMQuery::~CVNMQuery()
{
    if (NULL != m_pMU)
    {
        delete m_pMU;
    }
    m_pMU = NULL;
    FreeAdoDB();
    
    m_bOpenDbgInf = 0;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_pLogicNetworkMgr = NULL;    
};

/************************************************************
* �������ƣ� Init
* ���������� CVNetHeartbeat�����ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
int32 CVNMQuery::Init()
{
    m_pVNetVmDB = CVNetVmDB::GetInstance();
    if( NULL == m_pVNetVmDB)
    {
        cout << "CVNMQuery::Init CVNetVmDB::GetInstance() is NULL" << endl;
        return -1;
    }
    
    m_pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == m_pVNetVnicDB)
    {
        cout << "CVNMQuery::Init CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    //vsi
    m_pVSIProfileMgr = CVSIProfileMgr::GetInstance();
    if (NULL == m_pVSIProfileMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMQuery::Init CVSIProfileMgr::GetInstance().\n");
        return -1;
    }
        
    m_pSwitchMgr = CSwitchMgr::GetInstance();
    if( NULL == m_pSwitchMgr)
    {
        cout << "CVNMQuery::Init  CSwitchMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pVNAMgr = CVNAManager::GetInstance();
    if( NULL == m_pVNAMgr)
    {
        cout << "CVNMQuery::Init  CVNAManager::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pLogicNetworkMgr = CLogicNetworkMgr::GetInstance();
    if( NULL == m_pLogicNetworkMgr)
    {
        cout << "CVNMQuery::Init  CLogicNetworkMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    if (StartMu(MU_VNM_QUERY))
    {
        cout << "CVNMQuery::StartMu failed!" << endl;
        return -1;
    }

    return 0;
}        
/************************************************************
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
STATUS CVNMQuery::StartMu(string strMsgUnitName)
{
    if (m_pMU)
    {
        return ERROR_DUPLICATED_OP;
    }

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    m_pMU = new MessageUnit(strMsgUnitName);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMQuery::StartMu Create mu %s failed!\n", strMsgUnitName.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMQuery::StartMu SetHandler mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMQuery::StartMu Run mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMQuery::StartMu Register mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }
    
    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = m_pMU->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    m_pMU->Print();
    
    return SUCCESS;
}

/************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ���ַ�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
void CVNMQuery::MessageEntry(const MessageFrame &message)
{
    switch (m_pMU->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            m_pMU->set_state(S_Work);
            if (NULL == GetAdoDB())
            {
                cout << "CVNMQuery::GetAdoDB failed!" << endl;
            }

            cout << "CVNMQuery:GetAdoDB()==================="<< pthread_self() <<endl;


            VNET_LOG(VNET_LOG_INFO, "CVNMQuery::MessageEntry %s power on!\n", m_pMU->name().c_str());
            break;
        }
        default:
            VNET_LOG(VNET_LOG_INFO, "CVNMQuery::MessageEntry Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        
        //TC��VNM����Vnic��ϸ��Ϣ
        case EV_VNETLIB_QUERY_VNICS:
        {       
            VMVnicInfoReq(message);            
        }
        break;

        default:
        {
            cout << "CVNMQuery::MessageEntry: receive an unkown message!"
            << endl;
            VNET_LOG(VNET_LOG_INFO, "CVNMQuery::MessageEntry Unknown message: %d.\n", message.option.id());
            break;
        }
        }
        break;
    }

    default:
        break;
    }
}

void CVNMQuery::VMVnicInfoReq(const MessageFrame& message)
{
    int32 iRet = 0;
    CQueryVmNicInfoMsg cReqMsg;
    cReqMsg.deserialize(message.data);

    if (g_vnmquery_dbg_flag)
    {
        cout << "Receive CVNMQuery message:" <<endl;
        cReqMsg.Print();
    }

    char tmp[32]={0};
    SPRINTF(tmp,"vnm query(%lld) ", cReqMsg.m_vm_id);
    VNET_TIME_DEFINE(tmp);
    if( g_vnmquery_msg_during_flag )
    {
        VNET_TIME_BEGIN();
    }
    
    //�ȴ�DB��ȡVSI�����Ϣ���cMsgAck��
    CQueryVmNicInfoMsgAck cAckMsg;
    cAckMsg.action_id = cReqMsg.action_id;
    cAckMsg.m_vm_id = cReqMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cReqMsg, cAckMsg);

    CVMVSIInfo cVMVSIInfo;
    cVMVSIInfo.m_nVMID = cReqMsg.m_vm_id;

    CVNetVnicDB* pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == pVNetVnicDB)
    {
        cout << "CVNetVnicDB::GetInstance() is NULL" << endl;
        iRet = ERR_VNMVNIC_GET_VNIC_DB_HANDLE_FAIL;
        goto SEND_ACK;
    }

    iRet = pVNetVnicDB->GetVmVnicFromDb(cReqMsg.m_vm_id, cVMVSIInfo.m_vecVnicDetail);
    if ((iRet != 0) && (!VNET_DB_IS_ITEM_NO_EXIST(iRet)))
    {
        cout << "CVNetVnicDB->GetVmVnic failed." << iRet << endl;
        iRet = ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
        goto SEND_ACK;
    }

    iRet = m_pVSIProfileMgr->GetVSIDetailInfo(cReqMsg.m_vm_id, cVMVSIInfo);
    if (!iRet)
    {
        cAckMsg.m_infos.clear();
        vector<CVNetVnicDetail>::iterator itr = cVMVSIInfo.m_vecVnicDetail.begin();
        for( ; itr != cVMVSIInfo.m_vecVnicDetail.end(); ++itr)
        {
            CNicInfo cNicInfo;
            cNicInfo.m_nNicIndex = itr->GetNicIndex();
            cNicInfo.m_nIsSriov = itr->GetIsSriov();
            cNicInfo.m_nIsLoop = itr->GetIsLoop();
            cNicInfo.m_strLogicNetworkID = itr->GetLogicNetworkUuid();
            cNicInfo.m_strAdapterModel = itr->GetPortType();
            cNicInfo.m_strVSIProfileID = itr->GetVSIIDValue();
            cNicInfo.m_strPCIOrder = itr->GetPciOrder();
            cNicInfo.m_strBridgeName = itr->GetSwitchName();
            if (0 != itr->GetAssIp().size())
            {
                cNicInfo.m_strIP = itr->GetAssIp();
                cNicInfo.m_strNetmask  = itr->GetAssMask();
                cNicInfo.m_strGateway = itr->GetAssGateway();
            }
            else
            {
                cNicInfo.m_strIP = itr->GetIPAddress();
                cNicInfo.m_strNetmask  = itr->GetNetmask();
                cNicInfo.m_strGateway = itr->GetGateway();
            }
            if (0 != itr->GetAssMac().size())
            {
                cNicInfo.m_strMac = itr->GetAssMac();
            }
            else
            {
                cNicInfo.m_strMac = itr->GetMacAddress();
            }
            cNicInfo.m_nCvlan = itr->GetAccessCvlan();

            cAckMsg.m_infos.push_back(cNicInfo);
        }
    }

SEND_ACK:
    cAckMsg.state = iRet;
    cAckMsg.detail = GetVnetErrorInfo(cAckMsg.state);
    if(SUCCESS == iRet)
    {    
        cAckMsg.progress = 100;
    }
    
    //MessageOption vna_option(message.option.sender(), EV_VNETLIB_QUERY_VNICS_ACK,0,0);
    MessageOption vna_option(cReqMsg.m_mid, EV_VNETLIB_QUERY_VNICS_ACK,0,0);
    
    m_pMU->Send(cAckMsg,vna_option);

    if (g_vnmquery_dbg_flag)
    {
        cout << "Send CQueryVmNicInfoMsgAck ack:" <<endl;
        cAckMsg.Print();
    }

    if( g_vnmquery_msg_during_flag )
    {
        VNET_TIME_END();
    }
}

void VNM_QUERY_DBG_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

   g_vnmquery_dbg_flag = bSet;
};
DEFINE_DEBUG_FUNC(VNM_QUERY_DBG_FLAG);

void VNM_QUERY_MSG_DURING_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

   g_vnmquery_msg_during_flag = bSet;
};
DEFINE_DEBUG_FUNC(VNM_QUERY_MSG_DURING_FLAG);

#endif 
}

