/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_vxlan_agent.cpp
* �ļ���ʶ��
* ����ժҪ��CVxlanAgent��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��8��23��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Zhao.jin
*     �޸����ݣ�����
******************************************************************************/

#include "vna_vxlan_agent.h"
#include "vnet_function.h"
#include "vnet_event.h"
#include "vnet_error.h"
#include "vnet_comm.h"
#include "vna_common.h"
#include "ipv4_pool_mgr.h"
#include <sstream>
#include <stdlib.h>

namespace zte_tecs
{

CVxlanAgent *CVxlanAgent::m_pInstance = NULL;

CVxlanAgent::CVxlanAgent()
{
    m_pMU       = NULL;
    m_pInstance = NULL;     
    m_bOpenDbg  = VNET_FALSE;
}

CVxlanAgent::~CVxlanAgent()
{ 
    m_pMU       = NULL;
    m_pInstance = NULL; 
    m_bOpenDbg  = VNET_FALSE;
};

/*******************************************************************************
* �������ƣ� Init
* ���������� ��ʼ������
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
int32 CVxlanAgent::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::Init: m_pMu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    SendMcGroupInfoReq();

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
void CVxlanAgent::MessageEntry(const MessageFrame& message)
{
    switch(message.option.id())
    {
        case EV_VXLAN_MCGROUP_ACK:
        {
            RcvMcGroupInfo(message);
            break;
        }
        
        default:
            break;
    }
}

/*******************************************************************************
* �������ƣ� RcvMcGroupInfo
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
int32 CVxlanAgent::RcvMcGroupInfo(const MessageFrame &message)
{
    CVxlanMcGroupToVNA cMsg;
    cMsg.deserialize(message.data);

    if(m_bOpenDbg)
    {
        cMsg.Print();
    }

    UpdateVxlanMcGroup(cMsg.m_cVxlanMcGroupTable);
    
    return 0;    
}
/*******************************************************************************
* �������ƣ� DoDeleteMcGroup
* ���������� ����ɾ���鲥����Ϣ
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
void CVxlanAgent::DoDeleteMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    vector<CVxlanMcGroup> vecDelete;    
    vector<CVxlanMcGroup>::iterator iterOld;
    vector<CVxlanMcGroup>::iterator iterNew;
    
    /*�ҳ�Ҫɾ����*/
    iterOld = m_VxlanMcGroupTable.m_vecVxlanMcGroup.begin();
    for(; iterOld!=m_VxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iterOld)
    {
        iterNew = NewVxlanMcGroupTable.m_vecVxlanMcGroup.begin();
        for(; iterNew!=NewVxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iterNew)
        {
            if ( (iterOld->m_strNetplaneId == iterNew->m_strNetplaneId ) &&
                 (iterOld->m_strSegmentId  == iterNew->m_strSegmentId ))
            {
                break;
            }             
        }
        if (iterNew==NewVxlanMcGroupTable.m_vecVxlanMcGroup.end())
        {
            vecDelete.push_back(*iterOld);
        }
    }    

    /*��ɾ����ֱ��destroy����*/
    vector<CVxlanMcGroup>::iterator iter;
    for(iter=vecDelete.begin(); iter!=vecDelete.end(); ++iter)
    {
        string strVxlanBridgeName = GetVxlanBridgeName(iter->m_strNetplaneId, iter->m_strSegmentId);
        if (strVxlanBridgeName.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::DoDeleteMcGroup: strVxlanBridgeName.empty()\n");
            continue;
        }

        if (VXLAN_SUCCESS!=DestroyVxlanBridge(strVxlanBridgeName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::DoDeleteMcGroup: vecDelete DestroyVxlanBridge %s fail.\n", 
                strVxlanBridgeName.c_str());
        }        
    }     
}
/*******************************************************************************
* �������ƣ� DoUpdateMcGroup
* ���������� ��������鲥����Ϣ
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
void CVxlanAgent::DoUpdateMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    map<CVxlanMcGroup, CVxlanMcGroup> mapUpdate;    
    vector<CVxlanMcGroup>::iterator iterOld;
    vector<CVxlanMcGroup>::iterator iterNew;
    
    /*�ҳ�update��*/
    iterOld = m_VxlanMcGroupTable.m_vecVxlanMcGroup.begin();
    for(; iterOld!=m_VxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iterOld)
    {
        iterNew = NewVxlanMcGroupTable.m_vecVxlanMcGroup.begin();
        for(; iterNew!=NewVxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iterNew)
        {
            if ( (iterOld->m_strNetplaneId == iterNew->m_strNetplaneId ) &&
                 (iterOld->m_strSegmentId  == iterNew->m_strSegmentId ))
            {
                mapUpdate.insert(make_pair(*iterOld, *iterNew));
                break;
            }             
        }
    }    

    /*update����Ŀ����*/
    UpdateVxlanTunnel(mapUpdate);    
}
/*******************************************************************************
* �������ƣ� DoAddMcGroup
* ���������� ��������鲥����Ϣ
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
void CVxlanAgent::DoAddMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    vector<CVxlanMcGroup> vecAdd;
    vector<CVxlanMcGroup>::iterator iterOld;
    vector<CVxlanMcGroup>::iterator iterNew;
    
    /*�ҳ�Ҫ��ӵ�*/
    iterNew = NewVxlanMcGroupTable.m_vecVxlanMcGroup.begin();
    for(; iterNew!=NewVxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iterNew)
    {
        iterOld = m_VxlanMcGroupTable.m_vecVxlanMcGroup.begin();
        for(; iterOld!=m_VxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iterOld)
        {
            if ( (iterNew->m_strNetplaneId == iterOld->m_strNetplaneId ) &&
                 (iterNew->m_strSegmentId  == iterOld->m_strSegmentId ))
            {
                break;
            }             
        }
        if (iterOld== m_VxlanMcGroupTable.m_vecVxlanMcGroup.end())
        {
            vecAdd.push_back(*iterNew);
        }
    }
    
    /*Ҫ�������������ͬʱ����vxlan�����Ŀ*/
    vector<CVxlanMcGroup>::iterator iter;    
    for(iter=vecAdd.begin(); iter!=vecAdd.end(); ++iter)
    {
        string strVxlanBridgeName = GetVxlanBridgeName(iter->m_strNetplaneId, iter->m_strSegmentId);
        if (strVxlanBridgeName.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::DoAddMcGroup: strVxlanBridgeName.empty()\n");
            continue;
        }
        
        if (VXLAN_SUCCESS!=CreateVxlanBridge(strVxlanBridgeName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::DoAddMcGroup: vecAdd CreateVxlanBridge %s fail.\n", 
                strVxlanBridgeName.c_str());
        }
        else
        {
            UpdateVxlanTunnel(*iter);
        }            
    }   
}
/*******************************************************************************
* �������ƣ� UpdateVxlanMcGroup
* ���������� ����vxlan�鲥��
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
int32 CVxlanAgent::UpdateVxlanMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    /*�鲥��û�б仯*/
    if (m_VxlanMcGroupTable == NewVxlanMcGroupTable)
    {
        return VXLAN_SUCCESS;        
    }

    DoDeleteMcGroup(NewVxlanMcGroupTable);
   
    DoUpdateMcGroup(NewVxlanMcGroupTable);

    DoAddMcGroup(NewVxlanMcGroupTable);

    m_VxlanMcGroupTable = NewVxlanMcGroupTable;
    
    return VXLAN_SUCCESS;    
}
/*******************************************************************************
* �������ƣ� SendMcGroupInfoReq
* ���������� VNA�����鲥����Ϣ
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
int32 CVxlanAgent::SendMcGroupInfoReq(void)
{
    string strVnmName;
    //���û��ע����Ϣ��ֱ�ӷ���ʧ��
    if (ERROR == GetRegisteredSystem(strVnmName))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::ReqMcGroupInfoToVnm: call GetRegisteredSystem failed.\n");
        return ERROR_VXLAN_REQ_MCGROUP_FAIL;
    }

    string strVNAApp;    
    if (GetVNAUUIDByVNA(strVNAApp))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::ReqMcGroupInfoToVnm: call GetVNAUUIDByVNA failed.\n");
        return ERROR_VXLAN_REQ_MCGROUP_FAIL;
    }    
        
    //��VNM����VXLAN�鲥����Ϣ
    MID receiver;
    receiver._application = strVnmName;
    receiver._process = PROC_VNM;
    receiver._unit = MU_VNM;
    MessageOption option(receiver, EV_VXLAN_MCGROUP_REQ, 0, 0);
    m_pMU->Send(strVNAApp, option);

    VNET_LOG(VNET_LOG_INFO, "CVxlanAgent::ReqMcGroupInfoToVnm: message has been sent out.\n");

    return VXLAN_SUCCESS;
    
}
/*******************************************************************************
* �������ƣ� CreateVxlanBridge
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
int32 CVxlanAgent::CreateVxlanBridge(string strVxlanBridge)
{
    if (strVxlanBridge.empty())
    {
        return ERROR_VXLAN_BRIDGE_CREATE_FAIL;
    }
    
    if (IsVxlanBridgeExist(strVxlanBridge))
    {
        return VXLAN_SUCCESS;
    }
    
    if (VNET_PLUGIN_SUCCESS!=VNetCreateBridge(strVxlanBridge))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::CreateVxlanBridge: call VNetCreateBridge failed.\n"); 
        return ERROR_VXLAN_BRIDGE_CREATE_FAIL;
    }
    
    VNET_LOG(VNET_LOG_INFO, "CVxlanAgent::CreateVxlanBridge: create vxlan bridge %s.\n", strVxlanBridge.c_str());

    return VXLAN_SUCCESS;
}
/*******************************************************************************
* �������ƣ� DestroyVxlanBridge
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
int32 CVxlanAgent::DestroyVxlanBridge(string strVxlanBridge)
{
    if (!IsVxlanBridgeExist(strVxlanBridge))
    {
        return VXLAN_SUCCESS;
    }    
    
    if (VNET_PLUGIN_SUCCESS!=VNetDeleteBridge(strVxlanBridge))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::CreateVxlanSdvs: call VNetCreateBridge failed.\n"); 
        return ERROR_VXLAN_BRIDGE_DELETE_FAIL;
    }

    VNET_LOG(VNET_LOG_INFO, "CVxlanAgent::CreateVxlanBridge: destroy vxlan bridge %s.\n", strVxlanBridge.c_str());
    
    return VXLAN_SUCCESS;
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
BOOL  CVxlanAgent::IsVxlanBridgeExist(string strVxlanBridge)
{    
    //�����Ƿ����
    vector<string> vecBridge;
    VNetGetAllBridges(vecBridge); 
    
    vector<string>::iterator iter = vecBridge.begin();
    for (; iter != vecBridge.end(); ++iter)
    {
        if (0 == strVxlanBridge.compare(*iter))
        {
            return true;
        }
    } 

    VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::IsVxlanBridgeExist: Bridge <%s> is not exist.\n", strVxlanBridge.c_str()); 
    return false;
    
}
/*******************************************************************************
* �������ƣ� GetVxlanBridgeName
* ���������� ��ȡVXLAN BRIDGE������
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
string CVxlanAgent::GetVxlanBridgeName(string strNetplaneNickName, string strSegmentId)
{
    strNetplaneNickName.erase(0,strNetplaneNickName.find_first_not_of(" "));  
    strNetplaneNickName.erase(strNetplaneNickName.find_last_not_of(" ") + 1);  

    strSegmentId.erase(0,strSegmentId.find_first_not_of(" "));  
    strSegmentId.erase(strSegmentId.find_last_not_of(" ") + 1);      
        
    if (strNetplaneNickName.empty() || strSegmentId.empty())
    {
        return "";
    }
    
    return "vxl" + strNetplaneNickName + "_" + strSegmentId;
}
/*******************************************************************************
* �������ƣ� PrepareDeployVxlanVm
* ���������� ����vxlan vmǰ��׼������������vxlan bridge������vxlan tunnel��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� CVMVSIInfo &vmVSIInfo
* ��������� ��
* �� �� ֵ�� int32
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanAgent::PrepareDeployVxlanVm(CVMVSIInfo &vmVSIInfo)
{
    vector<CVNetVnicDetail>::iterator itrVnic = vmVSIInfo.m_vecVnicDetail.begin();
    for (; itrVnic != vmVSIInfo.m_vecVnicDetail.end(); ++itrVnic)
    {
        if (ISOLATE_VXLAN != itrVnic->GetIsolateType())
        {
            continue;
        }
        
        char buf[10]; 
        sprintf(buf, "%-8d", itrVnic->GetSegmentId()); 
        
        string strSegmentId        = buf;  
        string strNetplaneNickName = itrVnic->GetNetplaneNickName();        
        string strVxlanBridge      = GetVxlanBridgeName(strNetplaneNickName, strSegmentId);
        
        itrVnic->SetSwitchName(strVxlanBridge);

        if (VXLAN_SUCCESS==CreateVxlanBridge(strVxlanBridge))
        {
            CreateVxlanTunnel(strVxlanBridge);
        }        
    }

    return VXLAN_SUCCESS;
}
/*******************************************************************************
* �������ƣ� CreateVxlanTunnel
* ���������� �������bridge��ص�����vxlan tunnel��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� string strVxlanBridge
* ��������� ��
* �� �� ֵ�� int32
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanAgent::CreateVxlanTunnel(string strVxlanBridge)
{
    string strNetplaneNickName = GetNetplaneNickName(strVxlanBridge);
    string strSegmentId        = GetSegmentId(strVxlanBridge);

    vector<CVxlanMcGroup>::iterator iter = m_VxlanMcGroupTable.m_vecVxlanMcGroup.begin();
    for(; iter!=m_VxlanMcGroupTable.m_vecVxlanMcGroup.end(); ++iter)
    {
        if ( (iter->m_strNetplaneId == strNetplaneNickName) && 
             (iter->m_strSegmentId  == strSegmentId))
        {
            UpdateVxlanTunnel(*iter);
        }
    }   

    return VXLAN_SUCCESS;
    
}
/*******************************************************************************
* �������ƣ� GetSegmentId
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
string CVxlanAgent::GetSegmentId(string strVxlanBridge)
{
    if (strVxlanBridge.length() <= 7)
    {
        return "";
    }
    return strVxlanBridge.substr(7, (strVxlanBridge.length() - 7));
}
/*******************************************************************************
* �������ƣ� GetNetplaneNickName
* ���������� ��bridge�л�ȡ����ƽ��nickname
* ���ʵı� ��
* �޸ĵı� ��
* ��������� string strVxlanBridge
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
string CVxlanAgent::GetNetplaneNickName(string strVxlanBridge)
{
    if (strVxlanBridge.length() <= 3)
    {
        return "";
    }
    
    if (strVxlanBridge.length() <= 6)
    {
        return strVxlanBridge.substr(3, (strVxlanBridge.length() - 3));
    }
    
    return strVxlanBridge.substr(3,3);
}
/*******************************************************************************
* �������ƣ� GetVxlanPortName
* ���������� ����strNetplaneId,strSegmentId,remoteip����vxpt�˿ڵ�����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� string strVxlanBridge
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
string CVxlanAgent::GetVxlanPortName(string strNetplaneId, string strSegmentId, string strRemoteIP)
{
    if (strNetplaneId.empty() || strSegmentId.empty() || strRemoteIP.empty())
    {
        return "";
    }
    
    string sSegmentId;
    int32 nSegmentId = atoi(strSegmentId.c_str());
    if (0!=ConvertNum2string((unsigned int)nSegmentId,sSegmentId))
    {
        return "";
    }

    stringstream strRemote(strRemoteIP);
    int a,b,c,d;
    char ch;
    unsigned int nRemote;
    strRemote >> a >> ch >> b >> ch >> c >> ch >> d;
    nRemote = a << 24 | b << 16 | c << 8 | d;

    string sRemoteIP;
    if (0!=ConvertNum2string(nRemote,sRemoteIP))
    {
        return "";
    }    

    return strNetplaneId + "_" + sSegmentId + sRemoteIP;
}
/*******************************************************************************
* �������ƣ� ConvertNum2string
* ���������� �Ѵ����nNumת����52���ƣ�52���Ƶ�������a~Z��ʾ�� 
* ���ʵı� ��
* �޸ĵı� ��
* ��������� int32 nNum, string &s
* ��������� ��
* �� �� ֵ�� string
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanAgent::ConvertNum2string(unsigned int nNum, string &s)
{
    if(nNum > 4294967295)
    {
        return -1;
    }
    s = "";
    map<int32, char> mapTable;
    uint32 i = 0;

    /*����ӳ���*/
    char c = 'a';
    for (i=0; i<26; i++)
    {
        mapTable.insert(pair<int32, char> (i, c+i));
    }
    c = 'A';
    for (i=26; i<52; i++)
    {
        mapTable.insert(pair<int32, char> (i, c+i-26));
    }

    /*����ת��*/
    do 
    {
        map<int32, char>::iterator pos = mapTable.find((nNum%52));
        if (mapTable.end() == pos)
        {
            return -1;
        }
        s = (char)(pos->second) + s;            
        nNum = nNum/52;
    }while(nNum);

    return 0;
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
string CVxlanAgent::GetVtepNameBySwitchName(const string strSwitchName)
{
    if (strSwitchName.length() <= 5)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::GetVtepNameBySwitchName: strSwitchName (%s) is invalid!\n", 
            strSwitchName.c_str());
        return "";
    }

    if ("sdvs" != strSwitchName.substr(0,4))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::GetVtepNameBySwitchName: strSwitchName (%s) is invalid!\n", 
            strSwitchName.c_str());
        return "";        
    }    

    if ("sdvsw" == strSwitchName.substr(0,5))
    {
        if (strSwitchName.length() <= 6)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::GetVtepNameBySwitchName: strSwitchName (%s) is invalid!\n", 
                strSwitchName.c_str());
            return "";
        }

        return "vtepw_" + strSwitchName.substr(6, (strSwitchName.length() - 6)); 
    }
    
    return "vtep_" + strSwitchName.substr(5, (strSwitchName.length() - 5));
}
/*******************************************************************************
* �������ƣ� MonitorVxlanTunnel
* ���������� ���vxlan tunnel��Ϣ
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
int32 CVxlanAgent::MonitorVxlanTunnel(string &strVxlanBridge)
{
    return 0;
}
/*******************************************************************************
* �������ƣ� ShowMcGroupInfo
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
void CVxlanAgent::ShowMcGroupInfo(void)
{     
    m_VxlanMcGroupTable.Print();   
}
/*******************************************************************************
* �������ƣ� UpdateVxlanTunnel
* ���������� �����鲥���е�tunnel��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� CVxlanMcGroup &VxlanMcGroup
* ��������� ��
* �� �� ֵ�� int32
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanAgent::UpdateVxlanTunnel(CVxlanMcGroup &VxlanMcGroup)
{    
    map<CVxlanMcGroup, CVxlanMcGroup> mapUpdate;
    CVxlanMcGroup VxlanMcGroupOld;

    VxlanMcGroupOld.m_strNetplaneId = VxlanMcGroup.m_strNetplaneId;
    VxlanMcGroupOld.m_strSegmentId  = VxlanMcGroup.m_strSegmentId;    
    mapUpdate.insert(make_pair(VxlanMcGroupOld, VxlanMcGroup));

    return UpdateVxlanTunnel(mapUpdate);
}

/*******************************************************************************
* �������ƣ� UpdateVxlanTunnel
* ���������� �����鲥���е�tunnel��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� map<CVxlanMcGroup, CVxlanMcGroup> &mapUpdate
* ��������� ��
* �� �� ֵ�� int32
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
int32 CVxlanAgent::UpdateVxlanTunnel(map<CVxlanMcGroup, CVxlanMcGroup> &mapUpdate)
{    
    map<CVxlanMcGroup, CVxlanMcGroup>::iterator iter;    
    for (iter = mapUpdate.begin(); iter!=mapUpdate.end(); iter++)
    {
        string strVxlanBridgeName = GetVxlanBridgeName(iter->first.m_strNetplaneId, iter->first.m_strSegmentId);
        if (strVxlanBridgeName.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::UpdateVxlanTunnel: netplane:%s,segmentid:%s strVxlanBridgeName is empty!!\n",
                iter->first.m_strNetplaneId.c_str(), iter->first.m_strSegmentId.c_str());
            continue;
        }
        
        if (!IsVxlanBridgeExist(strVxlanBridgeName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::UpdateVxlanTunnel: netplane:%s,segmentid:%s strVxlanBridgeName not exist!!\n",
                iter->first.m_strNetplaneId.c_str(), iter->first.m_strSegmentId.c_str());
            continue;
        }

        vector<string>::iterator itold; 
        vector<string>::iterator itnew;
        CVxlanMcGroup  VxlanMcGroupOld = iter->first;
        CVxlanMcGroup  VxlanMcGroupNew = iter->second;
        
        itold = VxlanMcGroupOld.m_vecVtepIp.begin();
        for (; itold!=VxlanMcGroupOld.m_vecVtepIp.end(); ++itold)
        {
            /* �±���û��itold */
            if(false==IsMemberOfGroup(*itold, VxlanMcGroupNew.m_vecVtepIp))
            {          
                string strRemoteIP = *itold;
                string strVxlanPortName = GetVxlanPortName(VxlanMcGroupOld.m_strNetplaneId, VxlanMcGroupOld.m_strSegmentId, strRemoteIP);
                VNetDelVxlanNetIf(strVxlanBridgeName, strVxlanPortName);            
            }   
        }

        itnew = VxlanMcGroupNew.m_vecVtepIp.begin();
        for(; itnew!=VxlanMcGroupNew.m_vecVtepIp.end(); ++itnew)
        {
            /* �ϱ���û���ҵ�itnew����˵����Ҫ���� */
            if(false==IsMemberOfGroup(*itnew, VxlanMcGroupOld.m_vecVtepIp))
            {
                string strRemoteIP = *itnew;
                string strVxlanPortName = GetVxlanPortName(VxlanMcGroupNew.m_strNetplaneId, VxlanMcGroupNew.m_strSegmentId, strRemoteIP);                
                VNetAddVxlanNetIf(strVxlanBridgeName, strVxlanPortName, iter->first.m_strSegmentId, strRemoteIP);      
            }
        }      
    } 

    return VXLAN_SUCCESS;
}
/*******************************************************************************
* �������ƣ� IsMemberOfGroup
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
BOOL CVxlanAgent::IsMemberOfGroup(string strIp, vector<string>& vecVtepIp)
{
    vector<string>::iterator it = vecVtepIp.begin(); 
    for (; it!=vecVtepIp.end(); ++it)
    {
        if (*it==strIp)
        {
            return true;
        }
    }

    return false;    
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
void DbgSetVNAVxlanPrint(BOOL bOpen)
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }    

    pAgent->SetDbgPrintFlag(bOpen);
    if (bOpen)
    {
        cout << "vna vxlan mgr print is opened." << endl;
    }
    else
    {
        cout << "vna vxlan mgr print is closed." << endl;
    }
    
}
/*******************************************************************************
* �������ƣ� DbgShowMcGroupInfo
* ���������� ��ʾ�鲥����Ϣ���Ժ���
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
void DbgShowMcGroupInfo()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }    

    pAgent->ShowMcGroupInfo();
}
/*******************************************************************************
* �������ƣ� DbgShowMcGroupInfo
* ���������� ��ʾ�鲥����Ϣ���Ժ���
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
void DbgShow52Num(char num)
{
    map<int32, char> mapTable;
    uint32 i = 0;
    
    /*����ӳ���*/
    char c = 'a';
    for (i=0; i<26; i++)
    {
        mapTable.insert(pair<int32, char> (i, c+i));
    }
    c = 'A';
    for (i=26; i<52; i++)
    {
        mapTable.insert(pair<int32, char> (i, c+i-26));
    }    

    map<int32, char>::iterator pos = mapTable.find((num));
    if (mapTable.end() == pos)
    {
        cout << "num " << num << " can not find!" << endl; 
        return;
    }
    cout << "num " << num << " is " << pos->second << endl;            
    
}
/*******************************************************************************
* �������ƣ� Tst_SendMcGroupInfoReq
* ���������� ���Ժ��������鲥����Ϣ����
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
void Tst_SendMcGroupInfoReq()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }    

    pAgent->SendMcGroupInfoReq();
}
/*******************************************************************************
* �������ƣ� Tst_CreateDeleteVxlanBridge
* ���������� ���Ժ��������鲥����Ϣ����
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
void Tst_CreateDeleteVxlanBridge()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }

    string strVxlanBridge;    
    strVxlanBridge = "";
    VNET_ASSERT(0==pAgent->CreateVxlanBridge(strVxlanBridge));

    strVxlanBridge = "vxlabc_10000000";
    VNET_ASSERT(0==pAgent->CreateVxlanBridge(strVxlanBridge));
    VNET_ASSERT(true==pAgent->IsVxlanBridgeExist(strVxlanBridge));

    VNET_ASSERT(0==pAgent->DestroyVxlanBridge(strVxlanBridge));
    VNET_ASSERT(false==pAgent->IsVxlanBridgeExist(strVxlanBridge)); 

    strVxlanBridge = "";
    VNET_ASSERT(false==pAgent->DestroyVxlanBridge(strVxlanBridge));
    
}
/*******************************************************************************
* �������ƣ� Tst_GetVxlanPortName
* ���������� ���Ժ������Թ���vxlan�˿���
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
void Tst_GetVxlanPortName()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }

    string strNetplaneId = "a";
    string strSegmentId  = "50001";
    string strRemoteIP   = "1.1.1.10";
    string result = pAgent->GetVxlanPortName(strNetplaneId, strSegmentId, strRemoteIP);
    cout << "result:" << result << endl;

    strNetplaneId = "a";
    strSegmentId  = "50001";
    strRemoteIP   = "1.1.1.157";
    result = pAgent->GetVxlanPortName(strNetplaneId, strSegmentId, strRemoteIP);
    cout << "result:" << result << endl;

    
}
/*******************************************************************************
* �������ƣ� Tst_UpdateVxlanTunnel
* ���������� ���Ժ�������vxlan���
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
void Tst_UpdateVxlanTunnel()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }
    
    CVxlanMcGroup VxlanMcGroup;
    stringstream s;
    
    VxlanMcGroup.m_strNetplaneId = "abc";
    VxlanMcGroup.m_strSegmentId  = "1000";
    pAgent->CreateVxlanBridge(pAgent->GetVxlanBridgeName(VxlanMcGroup.m_strNetplaneId, VxlanMcGroup.m_strSegmentId));

    int i;
    for(i=1;i<=2;i++)
    {
        s.str("");
        s.clear();
        s << "192.168.1." << i;
        VxlanMcGroup.m_vecLocalVtepIp.push_back(s.str());
        VxlanMcGroup.m_vecVtepIp.push_back(s.str());
    }
    
    for(i=10;i<=20;i++)
    {
        s.str("");
        s.clear();
        s << "192.168.1." << i;
        VxlanMcGroup.m_vecVtepIp.push_back(s.str());
    }
    
    VNET_ASSERT(VXLAN_SUCCESS==pAgent->UpdateVxlanTunnel(VxlanMcGroup));  
    
    WAIT();
    
    pAgent->DestroyVxlanBridge(pAgent->GetVxlanBridgeName(VxlanMcGroup.m_strNetplaneId, VxlanMcGroup.m_strSegmentId));
    cout << "UpdateVxlanTunnel(CVxlanMcGroup &VxlanMcGroup) test over!" << endl;
    
}
/*******************************************************************************
* �������ƣ� Tst_UpdateVxlanTunnel_Map
* ���������� ���Ժ�������vxlan���
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
void Tst_UpdateVxlanTunnel_Map()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }

    map<CVxlanMcGroup, CVxlanMcGroup> mapUpdate;
    CVxlanMcGroup VxlanMcGroup, VxlanMcGroupNew;
    stringstream s;    

    int i,j,k;
    for (i=0;i<10;i++)
    {
        VxlanMcGroup.m_vecLocalVtepIp.clear();
        VxlanMcGroup.m_vecVtepIp.clear();
        VxlanMcGroupNew.m_vecLocalVtepIp.clear();
        VxlanMcGroupNew.m_vecVtepIp.clear();

        s.str("");
        s.clear();
        s << i;
        VxlanMcGroup.m_strNetplaneId = "ab" + s.str();
        VxlanMcGroup.m_strSegmentId  = "1000" + s.str();
        pAgent->CreateVxlanBridge(pAgent->GetVxlanBridgeName(VxlanMcGroup.m_strNetplaneId, VxlanMcGroup.m_strSegmentId));

        for(j=1;j<=2;j++)
        {
            s.str("");
            s.clear();
            s << "192.168." << i << "." << j;
            VxlanMcGroup.m_vecLocalVtepIp.push_back(s.str());
            VxlanMcGroup.m_vecVtepIp.push_back(s.str());
        }
        
        for(k=10;k<=20;k++)
        {
            s.str("");
            s.clear();
            s << "192.168." << i << "." << k;
            VxlanMcGroup.m_vecVtepIp.push_back(s.str());
        }

        s.str("");
        s.clear();
        s << i;
        VxlanMcGroupNew.m_strNetplaneId = "ab" + s.str();
        VxlanMcGroupNew.m_strSegmentId  = "1000" + s.str();

        for(j=10;j<=13;j++)
        {
            s.str("");
            s.clear();
            s << "192.168." << i << "." << j;
            VxlanMcGroupNew.m_vecLocalVtepIp.push_back(s.str());
            VxlanMcGroupNew.m_vecVtepIp.push_back(s.str());
        }
        
        for(k=14;k<=20;k++)
        {
            s.str("");
            s.clear();
            s << "192.168." << i << "." << k;
            VxlanMcGroupNew.m_vecVtepIp.push_back(s.str());
        }

        mapUpdate.insert(make_pair(VxlanMcGroup, VxlanMcGroupNew));        
    }

    VNET_ASSERT(VXLAN_SUCCESS==pAgent->UpdateVxlanTunnel(mapUpdate));  

    WAIT();
    
    for (i=0;i<10;i++)
    {       
        s.str("");
        s.clear();
        s << i;
        VxlanMcGroup.m_strNetplaneId = "ab" + s.str();
        VxlanMcGroup.m_strSegmentId  = "1000" + s.str();    
        pAgent->DestroyVxlanBridge(pAgent->GetVxlanBridgeName(VxlanMcGroup.m_strNetplaneId, VxlanMcGroup.m_strSegmentId));
    }
    
    cout << "UpdateVxlanTunnel(map<CVxlanMcGroup, CVxlanMcGroup> &mapUpdate) test over!" << endl;
    
}
/*******************************************************************************
* �������ƣ� Tst_UpdateVxlanMcGroup
* ���������� ���Ժ��������鲥��
* ���ʵı� ��
* �޸ĵı� ��s
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         ����
*******************************************************************************/
void Tst_UpdateVxlanMcGroup()
{
    CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
    if(!pAgent)
    {
        cout << "CVxlanAgent::GetInstance() is NULL!" << endl;
        return;
    }

    CVxlanMcGroupTable VxlanMcGroupTable;
    CVxlanMcGroup VxlanMcGroup;
    int i;
    stringstream s;

    VxlanMcGroup.m_strNetplaneId = "abc";
    VxlanMcGroup.m_strSegmentId  = "1000";
    for(i=1;i<=2;i++)
    {
        s.str("");
        s.clear();
        s << "192.168.1." << i;
        VxlanMcGroup.m_vecLocalVtepIp.push_back(s.str());
        VxlanMcGroup.m_vecVtepIp.push_back(s.str());
    }    
    for(i=10;i<=20;i++)
    {
        s.str("");
        s.clear();
        s << "192.168.1." << i;
        VxlanMcGroup.m_vecVtepIp.push_back(s.str());
    }
    VxlanMcGroupTable.m_vecVxlanMcGroup.push_back(VxlanMcGroup);
    VNET_ASSERT(VXLAN_SUCCESS==pAgent->UpdateVxlanMcGroup(VxlanMcGroupTable));     
    pAgent->ShowMcGroupInfo();
    WAIT();
    
    VxlanMcGroup.m_strNetplaneId = "abc";
    VxlanMcGroup.m_strSegmentId  = "1000";
    VxlanMcGroup.m_vecLocalVtepIp.clear();
    VxlanMcGroup.m_vecVtepIp.clear();
    VxlanMcGroupTable.m_vecVxlanMcGroup.clear(); 
    for(i=1;i<=1;i++)
    {
        s.str("");
        s.clear();
        s << "192.168.1." << i;
        VxlanMcGroup.m_vecLocalVtepIp.push_back(s.str());
        VxlanMcGroup.m_vecVtepIp.push_back(s.str());
    }    
    for(i=15;i<=25;i++)
    {
        s.str("");
        s.clear();
        s << "192.168.1." << i;
        VxlanMcGroup.m_vecVtepIp.push_back(s.str());
    }
    VxlanMcGroupTable.m_vecVxlanMcGroup.push_back(VxlanMcGroup);

    VxlanMcGroup.m_strNetplaneId = "abd";
    VxlanMcGroup.m_strSegmentId  = "1001";
    VxlanMcGroup.m_vecLocalVtepIp.clear();
    VxlanMcGroup.m_vecVtepIp.clear();
    VxlanMcGroupTable.m_vecVxlanMcGroup.push_back(VxlanMcGroup);
    
    VNET_ASSERT(VXLAN_SUCCESS==pAgent->UpdateVxlanMcGroup(VxlanMcGroupTable)); 
    pAgent->ShowMcGroupInfo();    
    WAIT();

    VxlanMcGroupTable.m_vecVxlanMcGroup.clear();    
    VNET_ASSERT(VXLAN_SUCCESS==pAgent->UpdateVxlanMcGroup(VxlanMcGroupTable)); 
    pAgent->ShowMcGroupInfo();   
    
    cout << "UpdateVxlanMcGroup test over!" << endl;
    
}

DEFINE_DEBUG_FUNC(DbgSetVNAVxlanPrint);
DEFINE_DEBUG_FUNC(DbgShowMcGroupInfo);
DEFINE_DEBUG_FUNC(DbgShow52Num);
DEFINE_DEBUG_FUNC(Tst_SendMcGroupInfoReq);
DEFINE_DEBUG_FUNC(Tst_CreateDeleteVxlanBridge);
DEFINE_DEBUG_FUNC(Tst_GetVxlanPortName);
DEFINE_DEBUG_FUNC(Tst_UpdateVxlanTunnel);
DEFINE_DEBUG_FUNC(Tst_UpdateVxlanTunnel_Map);
DEFINE_DEBUG_FUNC(Tst_UpdateVxlanMcGroup);



}// end namespace zte_tecs

