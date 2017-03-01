/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vxlan_mgr.h
* �ļ���ʶ�� 
* ����ժҪ��vxlan������CVxlanMgrʵ��
* ��ǰ�汾��1.0
* ��    �ߣ�zhaojin 
* ������ڣ� 
*******************************************************************************/
#include "vnet_comm.h"
#include "vnm_pub.h"
#include "vnet_mid.h"
#include "vnet_error.h"
#include "vnet_event.h"
#include "vnet_vnic.h"

namespace zte_tecs
{

CVxlanMgr *CVxlanMgr::m_pInstance = NULL;

CVxlanMgr::CVxlanMgr()
{
    m_pMU       = NULL;
    m_pInstance = NULL;     
    m_bOpenDbg  = VNET_FALSE;
}

CVxlanMgr::~CVxlanMgr()
{
    m_pMU       = NULL;
    m_pInstance = NULL; 
    m_bOpenDbg  = VNET_FALSE;
}

/*******************************************************************************
* �������ƣ� init
* ���������� ��ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� mu --��Ϣ��Ԫ
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanMgr::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::Init: pMu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    m_pDbMgr = CVNetDbMgr::GetInstance();
    if(NULL == m_pDbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::Init: CVNetDbMgr::GetInstance() is NULL.\n");
        return -1;
    }    

    return 0;
}
/*******************************************************************************
* �������ƣ� MessageEntry
* ���������� ��Ϣ��ں���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void CVxlanMgr::MessageEntry(const MessageFrame &message)
{
    switch(message.option.id())
    {
        case EV_VXLAN_MCGROUP_REQ:
        {
            RcvMcGroupInfoReq(message);
        }
        default:
            break;
    }
}
/*******************************************************************************
* �������ƣ� SendMcGroupToVna
* ���������� �����鲥����Ϣ��VNA
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin      �� ��
*******************************************************************************/
int32 CVxlanMgr::SendMcGroupInfo(const string &strVNAUuid)
{
    CVxlanMcGroupToVNA cMsg;

    /* �����ݿ��л�ȡ�鲥����Ϣ*/
    if (VXLAN_ERROR==GetMcGroupInfo(strVNAUuid, cMsg.m_cVxlanMcGroupTable))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::SendMcGroupInfo: GetMcGroupInfo fail vna:%s.\n", 
            strVNAUuid.c_str());
        return -1;
    }

    /* ��VNA�·��鲥����Ϣ */
    MID receiver;
    receiver._application = strVNAUuid;
    receiver._process = PROC_VNA;
    receiver._unit = MU_VNA_CONTROLLER;
    MessageOption option(receiver, EV_VXLAN_MCGROUP_ACK, 0, 0);
    m_pMU->Send(cMsg, option);

    if(m_bOpenDbg)
    {
        cout << "CVxlanAgent::SendMcGroupInfo: message has been sent out." << endl;
    }
    return 0;
}
/*******************************************************************************
* �������ƣ� RcvMcGroupInfoReq
* ���������� �յ�VNA�����鲥����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
int32 CVxlanMgr::RcvMcGroupInfoReq(const MessageFrame &message)
{
    string strVNAUuid = message.data;

    if(m_bOpenDbg)
    {
        cout << "VNA: " << strVNAUuid << " request mc group info!" << endl;    
    }
    
    return SendMcGroupInfo(strVNAUuid);
}
/*******************************************************************************
* �������ƣ� AllocVtepIP
* ���������� ����IP��VETP
* ���ʵı� ��
* �޸ĵı� ��
* ��������� const string strNetplaneId
* ��������� string &strIP, string &strMask
* �� �� ֵ�� int32
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
int32 CVxlanMgr::AllocVtepIP(const string strNetplaneId, string &strIP, string &strMask)
{
    if (strNetplaneId.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::AllocVtepIP strNetplaneId is null.\n");
        return -1;
    }

    /* ��ȡIP��Ϣ */
    CIPv4PoolMgr *pIPv4PoolMgr = CIPv4PoolMgr::GetInstance();
    if(NULL == pIPv4PoolMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::AllocVtepIP CIPv4PoolMgr::GetInstance() fail.\n");
        return -1;
    }
    if (0!=pIPv4PoolMgr->NetplaneAllocIPAddr(strNetplaneId, strIP, strMask))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::AllocVtepIP AllocIPAddrForNic fail.\n");
        return -1;
    }

    return 0;

}
/*******************************************************************************
* �������ƣ� FreeVtepIP
* ���������� VTEP�ͷ�IP
* ���ʵı� ��
* �޸ĵı� ��
* ��������� const string strVtepName, const string strIP, const string strMask
* ��������� ��
* �� �� ֵ�� int32
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
int32 CVxlanMgr::FreeVtepIP(const string strNetplaneId, const string strIP, const string strMask)
{
    if (strNetplaneId.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FreeVtepIP strNetplaneId is null.\n");
        return -1;
    }
    
    /* �ͷ�IP��Ϣ */
    CIPv4PoolMgr *pIPv4PoolMgr = CIPv4PoolMgr::GetInstance();
    if(NULL == pIPv4PoolMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FreeVtepIP CIPv4PoolMgr::GetInstance() fail.\n");
        return -1;
    }
    
    if (0!=pIPv4PoolMgr->NetplaneFreeIPAddr(strNetplaneId, strIP, strMask))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FreeVtepIP AllocIPAddrForNic fail.\n");
        return -1;
    }

    return 0;
}
/*******************************************************************************
* �������ƣ� GetNetplaneIdByVtep
* ���������� ͨ��vtep���ƻ�ȡ��Ӧ������ƽ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� const string strVtepName
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
string CVxlanMgr::GetNetplaneIdByVtep(const string strVtepName)
{
    int32 nRet;
    CPortGroup cPGInfo;  

    string strSwName = GetSwitchNameByVtepName(strVtepName);
    if(0==strSwName.size())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetNetplaneIdByVtep: strSwName is NULL, strVtepName is %s!\n", strVtepName.c_str());
        return "";
    }
    
    /* ��ȡPG��Ϣ */
    CSwitchMgr *pSwitchMgr = CSwitchMgr::GetInstance();
    if(NULL==pSwitchMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetNetplaneIdByVtep: pSwitchMgr is NULL\n");
        return "";
    }
    nRet = pSwitchMgr->GetSwitchPGInfo(strSwName,cPGInfo);
    if(VXLAN_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetNetplaneIdByVtep: GetSwitchPGInfo failed, switch name %s, ret = %d\n", 
            strSwName.c_str(), nRet);
        return "";
    }
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if(NULL == pPGMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetNetplaneIdByVtep CPortGroupMgr::GetInstance() fail.\n");
        return "";
    }    
    nRet = pPGMgr->GetPGDetail(cPGInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetNetplaneIdByVtep: GetPGDetail failed, ret = %d\n", nRet);
        return "";
    }

    return cPGInfo.GetNetplaneUuid();
    
}
/*******************************************************************************
* �������ƣ� GetVtepNameBySwitchName
* ���������� ͨ��SWITCHNAME��ȡVTEP�˿�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� const string strSwitchName
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
string CVxlanMgr::GetVtepNameBySwitchName(const string strSwitchName)
{
    if (strSwitchName.length() <= 5)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetVtepNameBySwitchName: strSwitchName (%s) is invalid!\n", 
            strSwitchName.c_str());
        return "";
    }

    if ("sdvs" != strSwitchName.substr(0,4))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetVtepNameBySwitchName: strSwitchName (%s) is invalid!\n", 
            strSwitchName.c_str());
        return "";        
    }    

    if ("sdvsw" == strSwitchName.substr(0,5))
    {
        if (strSwitchName.length() <= 6)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetVtepNameBySwitchName: strSwitchName (%s) is invalid!\n", 
                strSwitchName.c_str());
            return "";
        }

        return "vtepw_" + strSwitchName.substr(6, (strSwitchName.length() - 6)); 
    }
    
    return "vtep_" + strSwitchName.substr(5, (strSwitchName.length() - 5));
}
/*******************************************************************************
* �������ƣ� GetVtepNameBySwitchUuid
* ���������� ͨ��switchuuid��ȡVTEP�˿�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� const string strSwitchUuid
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
string CVxlanMgr::GetVtepNameBySwitchUuid(const string strSwitchUuid)
{
    string strSwitchName;

    CSwitchMgr *pSwitchMgr = CSwitchMgr::GetInstance();
    if (!pSwitchMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetVtepNameBySwitchUuid: CSwitchMgr::GetInstance() fail! strSwitchUuid (%s)\n", 
            strSwitchUuid.c_str());        
        return "";
    }
    
    if (0!=pSwitchMgr->GetSwitchName(strSwitchUuid, strSwitchName))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetVtepNameBySwitchUuid: pSwitchMgr->GetSwitchName fail! strSwitchUuid (%s)\n", 
            strSwitchUuid.c_str());          
        return "";
    }

    return GetVtepNameBySwitchName(strSwitchName);    
}
/*******************************************************************************
* �������ƣ� GetSwitchNameByVtepName
* ���������� ͨ��VTEP�˿ڻ�ȡSWITCHNAME����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� const string strVtepName
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      �� ��
*******************************************************************************/
string CVxlanMgr::GetSwitchNameByVtepName(const string strVtepName)
{
    if (strVtepName.length() <= 5)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetSwitchNameByVtepName: strVtepName (%s) is invalid!\n", 
            strVtepName.c_str());
        return "";
    }

    if ("vtep" != strVtepName.substr(0,4))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetSwitchNameByVtepName: strVtepName (%s) is invalid!\n", 
            strVtepName.c_str());
        return "";        
    }

    if ("vtepw" == strVtepName.substr(0,5))
    {
        if (strVtepName.length() <= 6)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetSwitchNameByVtepName: strVtepName (%s) is invalid!\n", 
                strVtepName.c_str());
            return "";
        }

        return "sdvsw_" + strVtepName.substr(6, (strVtepName.length() - 6)); 
    }    

    return "sdvs_" + strVtepName.substr(5, (strVtepName.length() - 5));
}

/*******************************************************************************
* �������ƣ� GetMcGroupInfo
* ���������� �����鲥����Ϣ��VNA
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin      �� ��
*******************************************************************************/
int32 CVxlanMgr::GetMcGroupInfo(const string &strVNAUuid, CVxlanMcGroupTable &VxlanMcGroupTable)
{
    CDBOperateNetplaneSegmentPool *pOperSegmentPool = GetDbINetplaneSegmentPool();
    if (NULL == pOperSegmentPool)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetMcGroupInfo: GetDbINetplaneSegmentPool is NULL.\n");
        return -1;
    }    

    CNetplaneMgr *NetplaneMgr = CNetplaneMgr::GetInstance();
    if (NULL == NetplaneMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetMcGroupInfo: CNetplaneMgr::GetInstance is NULL.\n");
        return -1;
    }   

    vector<CNetplane> vecNetplane;
    if (0!=NetplaneMgr->GetAllNetplane(vecNetplane))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetMcGroupInfo: CNetplaneMgr::GetAllNetplane is NULL.\n");
        return -1;        
    }
    
    if (strVNAUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::GetMcGroupInfo: strVNAUuid is empty.\n");
        return -1;        
    }    

    vector<CNetplane>::iterator itr = vecNetplane.begin();
    for ( ; itr != vecNetplane.end(); ++itr)
    {
        int32 nRet;    
        vector<CDbVnaNpSegmentId> vecAllVnaDbLocalVtep;
        /*���ҳ�VNA�µ�һ������ƽ���е����б���VTEP*/
        nRet = pOperSegmentPool->QuerySegIdIplistByVnaNpSeg(strVNAUuid.c_str(), itr->GetUUID().c_str(), -1, vecAllVnaDbLocalVtep);
        if (VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: QuerySegIdIplistByVnaNpSeg failed. vna:%s, netplane:%s, ret: %d.\n",
                strVNAUuid.c_str(), itr->GetUUID().c_str(), nRet);
            return -1;
        }
        /*�ҳ�һ��VNA�µ�һ������ƽ���е�����SEGMENT,�п���һ��SEGMENT���ж��VTEP, �����Ҫ���˵�SEGMENT��ͬ��VTEP*/
        sort(vecAllVnaDbLocalVtep.begin(), vecAllVnaDbLocalVtep.end());
        vecAllVnaDbLocalVtep.erase(unique(vecAllVnaDbLocalVtep.begin(), vecAllVnaDbLocalVtep.end()), vecAllVnaDbLocalVtep.end());        
        if (vecAllVnaDbLocalVtep.empty())
        {
            continue;
        }

        /*�ٸ��ݱ���VTEP������ƽ���SEGMENT���ҵ���Ӧ������ƽ���SEGMENT�µ�LOCAL��REMOTE VTEP*/
        vector<CDbVnaNpSegmentId>::iterator itSegId = vecAllVnaDbLocalVtep.begin();
        for ( ; itSegId != vecAllVnaDbLocalVtep.end(); ++itSegId)
        {
            vector<CDbVnaNpSegmentId> vecDbLocalVtep;
            vector<CDbVnaNpSegmentId> vecDbRemoteVteps;

            /*��ȡ�鲥���еĸ�VNA��һ������ƽ����һ��SEGMENT������LOCAL VTEP*/
            nRet = pOperSegmentPool->QuerySegIdIplistByVnaNpSeg(itSegId->GetVnaUuid().c_str(), itSegId->GetNpUuid().c_str(), 
                itSegId->GetSegmentNum(), vecDbLocalVtep);
            if (VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: QuerySegIdIplistByVnaNpSeg failed. vna:%s, netplane:%s, segmentid:%d, ret: %d.\n",
                     itSegId->GetVnaUuid().c_str(), itSegId->GetNpUuid().c_str(), itSegId->GetSegmentNum(), nRet);
                return -1;
            }

            /*�Ȼ�ȡ�鲥����һ������ƽ����һ��SEGMENT������VTEP, Ȼ��ȥ����VNA�µ�LOCAL VTEP���õ�����ƽ���µ�һ��SEGMENT��REMOTE VTEP*/
            nRet = pOperSegmentPool->QuerySegIdIplistByVnaNpSeg("", itSegId->GetNpUuid().c_str(), itSegId->GetSegmentNum(), vecDbRemoteVteps);
            if (VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: QuerySegIdIplistByVnaNpSeg failed. netplane:%s, segmentid:%d, ret: %d.\n",
                     itSegId->GetNpUuid().c_str(), itSegId->GetSegmentNum(), nRet);
                return -1;
            }
            GetRemoteVteps(vecDbRemoteVteps, vecDbLocalVtep);

            /*����鲥��*/
            CVxlanMcGroup VxlanMcGroup;
            if (0!=FillVxlanMcGroup(VxlanMcGroup, vecDbLocalVtep, vecDbRemoteVteps))
            {
                VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: FillVxlanMcGroup failed\n");
                return -1;                
            }            

            VxlanMcGroupTable.m_vecVxlanMcGroup.push_back(VxlanMcGroup);            
        }
    }
       
    return VXLAN_SUCCESS;
}
/*******************************************************************************
* �������ƣ� GetRemoteVteps
* ���������� ȥ��vxlan �鲥���� ����vtep
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void CVxlanMgr::GetRemoteVteps(vector<CDbVnaNpSegmentId> &vecDbMcVteps, vector<CDbVnaNpSegmentId> vecDbLocalVtep)
{   
    vector<CDbVnaNpSegmentId>::iterator itlocal = vecDbLocalVtep.begin();
    for ( ; itlocal != vecDbLocalVtep.end(); ++itlocal)
    {
        if (vecDbMcVteps.empty())
        {
            break;
        }
        
        vector<CDbVnaNpSegmentId>::iterator itmc = vecDbMcVteps.begin();
        while (itmc != vecDbMcVteps.end())
        {
            if ((itlocal->GetIp()  == itmc->GetIp()) &&
                (itlocal->GetMask() == itmc->GetMask()))
            {
                itmc = vecDbMcVteps.erase(itmc);
            }
            else
            {            
                ++itmc;
            }
        }        
    }
}
/*******************************************************************************
* �������ƣ� FillVxlanMcGroup
* ���������� ���vxlan�鲥��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanMgr::FillVxlanMcGroup(CVxlanMcGroup &VxlanMcGroup, 
                                      vector<CDbVnaNpSegmentId> vecDbLocalVtep, 
                                      vector<CDbVnaNpSegmentId> vecDbRemoteVteps)
{    
    if (vecDbLocalVtep.empty())
    {
        return 0;
    }

    CNetplaneMgr *NetplaneMgr = CNetplaneMgr::GetInstance();
    if (NULL == NetplaneMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FillVxlanMcGroup: CNetplaneMgr::GetInstance is NULL.\n");
        return -1;
    }   

    /*��ȡ����ƽ��nick name*/
    int32 nId;
    vector<CDbVnaNpSegmentId>::iterator itLocalVtep = vecDbLocalVtep.begin();
    if(0!=NetplaneMgr->GetNetplaneId(itLocalVtep->GetNpUuid(),nId))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FillVxlanMcGroup: (Netplane %s)  GetNetplaneId fail.\n", 
            itLocalVtep->GetNpUuid().c_str());
        return -1;
    }
    string strNickName;
    if(0!=NetplaneMgr->ConvertIdToNickName(nId, strNickName))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FillVxlanMcGroup: (Netplane %s, nId %d)ConvertIdToNickName fail.\n", 
            itLocalVtep->GetNpUuid().c_str(), nId);
        return -1;
    }
    VxlanMcGroup.m_strNetplaneId = strNickName;

    /*��ȡ����ƽ��segmentid*/
    stringstream s;
    s << itLocalVtep->GetSegmentNum();
    VxlanMcGroup.m_strSegmentId  = s.str();   
    VxlanMcGroup.m_strSegmentId.erase(0,VxlanMcGroup.m_strSegmentId.find_first_not_of(" "));  
    VxlanMcGroup.m_strSegmentId.erase(VxlanMcGroup.m_strSegmentId.find_last_not_of(" ") + 1);        
    
    
    /*���LOCALVTEP IP*/
    for (itLocalVtep=vecDbLocalVtep.begin(); itLocalVtep!=vecDbLocalVtep.end(); ++itLocalVtep)
    {
        VxlanMcGroup.m_vecLocalVtepIp.push_back(itLocalVtep->GetIp());
    }

    /*���REMOTE VTEP IP*/
    vector<CDbVnaNpSegmentId>::iterator it;    
    for (it=vecDbRemoteVteps.begin(); it!=vecDbRemoteVteps.end(); ++it)
    {        
        VxlanMcGroup.m_vecVtepIp.push_back(it->GetIp());
    }

    return 0;  
}
/*******************************************************************************
* �������ƣ� DbgSetVNAVxlanPrint
* ���������� ���ô�ӡ���ص��Ժ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void DbgSetVNMVxlanPrint(BOOL bOpen)
{
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        return;
    }
    
    pVxlanMgr->SetDbgPrintFlag(bOpen);
    if (bOpen)
    {
        cout << "vnm vxlan mgr print is opened." << endl;
    }
    else
    {
        cout << "vnm vxlan mgr print is closed." << endl;
    }
}
/*******************************************************************************
* �������ƣ� Tst_ShowMcGroupInfo
* ���������� ��ʾ�鲥����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� void
* ��������� ��
* �� �� ֵ�� void
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void DbgShowMcGroupInfo(const char *pstrVNAUuid)
{  
    if (NULL==pstrVNAUuid)
    {
        return;
    }  
    
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        return;
    }

    string strVNAUuid;
    strVNAUuid.assign(pstrVNAUuid);    
    
    /* �����ݿ��л�ȡ�鲥����Ϣ*/
    CVxlanMcGroupTable  VxlanMcGroupTable;        
    if (VXLAN_ERROR==pVxlanMgr->GetMcGroupInfo(strVNAUuid, VxlanMcGroupTable))
    {
        cout << "Tst_ShowMcGroupInfo: GetMcGroupInfo fail vna:%s.\n" << strVNAUuid << endl;
        return;
    }    

    VxlanMcGroupTable.Print();    
}
/*******************************************************************************
* �������ƣ� Tst_GetSwitchNameByVtepName
* ���������� ͨ��vtepname��ȡ����name
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_GetSwitchNameByVtepName()
{
    string strVtepName;
        
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        return;
    }
    
    strVtepName = "";
    VNET_ASSERT(""==pVxlanMgr->GetSwitchNameByVtepName(strVtepName));
    
    strVtepName = "vtep_";
    VNET_ASSERT(""==pVxlanMgr->GetSwitchNameByVtepName(strVtepName));

    strVtepName = "sdfsfsdfsdf";
    VNET_ASSERT(""==pVxlanMgr->GetSwitchNameByVtepName(strVtepName));     

    strVtepName = "vtep_001";
    VNET_ASSERT("sdvs_001"==pVxlanMgr->GetSwitchNameByVtepName(strVtepName));  

    strVtepName = "vtepw_";
    VNET_ASSERT(""==pVxlanMgr->GetSwitchNameByVtepName(strVtepName));    

    strVtepName = "vtepw_001";
    VNET_ASSERT("sdvsw_001"==pVxlanMgr->GetSwitchNameByVtepName(strVtepName));
}
/*******************************************************************************
* �������ƣ� Tst_GetVtepNameBySwitchName
* ���������� ͨ������name��ȡvtepname
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_GetVtepNameBySwitchName()
{
    string strSwName;
        
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        return;
    }
    
    strSwName = "";
    VNET_ASSERT(""==pVxlanMgr->GetVtepNameBySwitchName(strSwName));
    
    strSwName = "sdvs_";
    VNET_ASSERT(""==pVxlanMgr->GetVtepNameBySwitchName(strSwName));

    strSwName = "sdfsfsdfsdf";
    VNET_ASSERT(""==pVxlanMgr->GetVtepNameBySwitchName(strSwName));     

    strSwName = "sdvs_001";
    VNET_ASSERT("vtep_001"==pVxlanMgr->GetVtepNameBySwitchName(strSwName)); 

    strSwName = "sdvsw_";
    VNET_ASSERT(""==pVxlanMgr->GetVtepNameBySwitchName(strSwName));

    strSwName = "sdvsw_001";
    VNET_ASSERT("vtepw_001"==pVxlanMgr->GetVtepNameBySwitchName(strSwName));           
    
}
/*******************************************************************************
* �������ƣ� Tst_GetVtepNameBySwitchUuid
* ���������� ͨ������Uuid��ȡvtepname
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_GetVtepNameBySwitchUuid(const char *pSwitchUuid)
{     
    if (NULL==pSwitchUuid)
    {
        return;
    }    
    
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        return;
    }
 
    string strSwUuid;
    strSwUuid.assign(pSwitchUuid);

    string strVtepName = pVxlanMgr->GetVtepNameBySwitchUuid(strSwUuid);
    cout << " strVtepName:" << strVtepName << endl;
    VNET_ASSERT(""!=strVtepName);
    
}

/*******************************************************************************
* �������ƣ� Tst_GetNetplaneIdByVtep
* ���������� ͨ��vtep��ȡ����ƽ��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_GetNetplaneIdByVtep(const char *pVtepName, const char *pNetplaneId)
{    
    if (NULL==pVtepName || NULL==pNetplaneId)
    {
        return;
    }    
    
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        return;
    }

    string strVtepName;
    strVtepName.assign(pVtepName);
    
    string strNetplaneId;
    strNetplaneId.assign(pNetplaneId);

    VNET_ASSERT(strNetplaneId==pVxlanMgr->GetNetplaneIdByVtep(strVtepName));
}
/*******************************************************************************
* �������ƣ� Tst_AllocVtepIP
* ���������� ���Է���vtep ip����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_AllocVtepIP()
{        
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {    
        return;
    }

    string strIP;
    string strMask;
    string strVtepName;   

    strVtepName = "";
    VNET_ASSERT(0!=pVxlanMgr->AllocVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask));
    
    strVtepName = "vtep_";
    VNET_ASSERT(0!=pVxlanMgr->AllocVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask));
    
    strVtepName = "vtep_00";
    VNET_ASSERT(0==pVxlanMgr->AllocVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask));
    cout << "ip:" << strIP << " mask:" << strMask << " is allocated!" << endl;
    pVxlanMgr->FreeVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask);
    
}
/*******************************************************************************
* �������ƣ� Tst_FreeVtepIP
* ���������� ���Ի���vtep ip����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_FreeVtepIP()
{        
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if(!pVxlanMgr)
    {    
        return;
    }

    string strIP = "";
    string strMask = "";
    string strVtepName = "";        
    VNET_ASSERT(0!=pVxlanMgr->FreeVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask));
    
    strVtepName = "vtep_";
    VNET_ASSERT(0!=pVxlanMgr->FreeVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask));

    strVtepName = "vtep_00";
    pVxlanMgr->AllocVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask);   
    cout << "Allocate test ip:" << strIP << " mask:" << strMask << endl;        
    VNET_ASSERT(0==pVxlanMgr->FreeVtepIP(pVxlanMgr->GetNetplaneIdByVtep(strVtepName), strIP, strMask));
}

/*******************************************************************************
* �������ƣ� Tst_RcvMcGroupInfoReq
* ���������� �����յ�VNA�����鲥����Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_RcvMcGroupInfoReq(const char* pstrVnaUuid)
{        
    if(NULL == pstrVnaUuid)           
    {
        return ;
    }
    
    MessageUnit tempmu(TempUnitName("TestRcvMcGroupInfoReq"));
    tempmu.Register();
    string strVnaUuid;
    strVnaUuid.assign(pstrVnaUuid);
    
    MessageOption option(MU_VNM, EV_VXLAN_MCGROUP_REQ, 0, 0);
    tempmu.Send(strVnaUuid,option);    
}

DEFINE_DEBUG_FUNC(DbgSetVNMVxlanPrint);
DEFINE_DEBUG_FUNC(DbgShowMcGroupInfo);
DEFINE_DEBUG_FUNC(Tst_GetSwitchNameByVtepName);
DEFINE_DEBUG_FUNC(Tst_GetVtepNameBySwitchName);
DEFINE_DEBUG_FUNC(Tst_GetVtepNameBySwitchUuid);
DEFINE_DEBUG_FUNC(Tst_GetNetplaneIdByVtep);
DEFINE_DEBUG_FUNC(Tst_AllocVtepIP);
DEFINE_DEBUG_FUNC(Tst_FreeVtepIP);
DEFINE_DEBUG_FUNC(Tst_RcvMcGroupInfoReq);

}


