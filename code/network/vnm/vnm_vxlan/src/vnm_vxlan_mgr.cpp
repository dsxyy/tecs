/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vxlan_mgr.h
* 文件标识： 
* 内容摘要：vxlan管理类CVxlanMgr实现
* 当前版本：1.0
* 作    者：zhaojin 
* 完成日期： 
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
* 函数名称： init
* 功能描述： 初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： mu --消息单元
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： MessageEntry
* 功能描述： 消息入口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： SendMcGroupToVna
* 功能描述： 发送组播组信息到VNA
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin      创 建
*******************************************************************************/
int32 CVxlanMgr::SendMcGroupInfo(const string &strVNAUuid)
{
    CVxlanMcGroupToVNA cMsg;

    /* 从数据库中获取组播组信息*/
    if (VXLAN_ERROR==GetMcGroupInfo(strVNAUuid, cMsg.m_cVxlanMcGroupTable))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanAgent::SendMcGroupInfo: GetMcGroupInfo fail vna:%s.\n", 
            strVNAUuid.c_str());
        return -1;
    }

    /* 向VNA下发组播组信息 */
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
* 函数名称： RcvMcGroupInfoReq
* 功能描述： 收到VNA请求组播组信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
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
* 函数名称： AllocVtepIP
* 功能描述： 分配IP给VETP
* 访问的表： 无
* 修改的表： 无
* 输入参数： const string strNetplaneId
* 输出参数： string &strIP, string &strMask
* 返 回 值： int32
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
*******************************************************************************/
int32 CVxlanMgr::AllocVtepIP(const string strNetplaneId, string &strIP, string &strMask)
{
    if (strNetplaneId.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::AllocVtepIP strNetplaneId is null.\n");
        return -1;
    }

    /* 获取IP信息 */
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
* 函数名称： FreeVtepIP
* 功能描述： VTEP释放IP
* 访问的表： 无
* 修改的表： 无
* 输入参数： const string strVtepName, const string strIP, const string strMask
* 输出参数： 无
* 返 回 值： int32
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
*******************************************************************************/
int32 CVxlanMgr::FreeVtepIP(const string strNetplaneId, const string strIP, const string strMask)
{
    if (strNetplaneId.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CVxlanMgr::FreeVtepIP strNetplaneId is null.\n");
        return -1;
    }
    
    /* 释放IP信息 */
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
* 函数名称： GetNetplaneIdByVtep
* 功能描述： 通过vtep名称获取对应的网络平面
* 访问的表： 无
* 修改的表： 无
* 输入参数： const string strVtepName
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
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
    
    /* 获取PG信息 */
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
* 函数名称： GetVtepNameBySwitchName
* 功能描述： 通过SWITCHNAME获取VTEP端口名字
* 访问的表： 无
* 修改的表： 无
* 输入参数： const string strSwitchName
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
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
* 函数名称： GetVtepNameBySwitchUuid
* 功能描述： 通过switchuuid获取VTEP端口名字
* 访问的表： 无
* 修改的表： 无
* 输入参数： const string strSwitchUuid
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
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
* 函数名称： GetSwitchNameByVtepName
* 功能描述： 通过VTEP端口获取SWITCHNAME名字
* 访问的表： 无
* 修改的表： 无
* 输入参数： const string strVtepName
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhaojin      创 建
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
* 函数名称： GetMcGroupInfo
* 功能描述： 发送组播组信息到VNA
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin      创 建
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
        /*先找出VNA下的一个网络平面中的所有本地VTEP*/
        nRet = pOperSegmentPool->QuerySegIdIplistByVnaNpSeg(strVNAUuid.c_str(), itr->GetUUID().c_str(), -1, vecAllVnaDbLocalVtep);
        if (VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: QuerySegIdIplistByVnaNpSeg failed. vna:%s, netplane:%s, ret: %d.\n",
                strVNAUuid.c_str(), itr->GetUUID().c_str(), nRet);
            return -1;
        }
        /*找出一个VNA下的一个网络平面中的所有SEGMENT,有可能一个SEGMENT下有多个VTEP, 因此需要过滤掉SEGMENT相同的VTEP*/
        sort(vecAllVnaDbLocalVtep.begin(), vecAllVnaDbLocalVtep.end());
        vecAllVnaDbLocalVtep.erase(unique(vecAllVnaDbLocalVtep.begin(), vecAllVnaDbLocalVtep.end()), vecAllVnaDbLocalVtep.end());        
        if (vecAllVnaDbLocalVtep.empty())
        {
            continue;
        }

        /*再根据本地VTEP的网络平面和SEGMENT，找到对应该网络平面和SEGMENT下的LOCAL、REMOTE VTEP*/
        vector<CDbVnaNpSegmentId>::iterator itSegId = vecAllVnaDbLocalVtep.begin();
        for ( ; itSegId != vecAllVnaDbLocalVtep.end(); ++itSegId)
        {
            vector<CDbVnaNpSegmentId> vecDbLocalVtep;
            vector<CDbVnaNpSegmentId> vecDbRemoteVteps;

            /*获取组播组中的该VNA的一个网络平面下一个SEGMENT的所有LOCAL VTEP*/
            nRet = pOperSegmentPool->QuerySegIdIplistByVnaNpSeg(itSegId->GetVnaUuid().c_str(), itSegId->GetNpUuid().c_str(), 
                itSegId->GetSegmentNum(), vecDbLocalVtep);
            if (VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: QuerySegIdIplistByVnaNpSeg failed. vna:%s, netplane:%s, segmentid:%d, ret: %d.\n",
                     itSegId->GetVnaUuid().c_str(), itSegId->GetNpUuid().c_str(), itSegId->GetSegmentNum(), nRet);
                return -1;
            }

            /*先获取组播组中一个网络平面下一个SEGMENT的所有VTEP, 然后去除该VNA下的LOCAL VTEP，得到网络平面下的一个SEGMENT的REMOTE VTEP*/
            nRet = pOperSegmentPool->QuerySegIdIplistByVnaNpSeg("", itSegId->GetNpUuid().c_str(), itSegId->GetSegmentNum(), vecDbRemoteVteps);
            if (VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR,"CVxlanMgr::GetMcGroupInfo: QuerySegIdIplistByVnaNpSeg failed. netplane:%s, segmentid:%d, ret: %d.\n",
                     itSegId->GetNpUuid().c_str(), itSegId->GetSegmentNum(), nRet);
                return -1;
            }
            GetRemoteVteps(vecDbRemoteVteps, vecDbLocalVtep);

            /*填充组播表*/
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
* 函数名称： GetRemoteVteps
* 功能描述： 去除vxlan 组播组中 本地vtep
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： FillVxlanMcGroup
* 功能描述： 填充vxlan组播组
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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

    /*获取网络平面nick name*/
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

    /*获取网络平面segmentid*/
    stringstream s;
    s << itLocalVtep->GetSegmentNum();
    VxlanMcGroup.m_strSegmentId  = s.str();   
    VxlanMcGroup.m_strSegmentId.erase(0,VxlanMcGroup.m_strSegmentId.find_first_not_of(" "));  
    VxlanMcGroup.m_strSegmentId.erase(VxlanMcGroup.m_strSegmentId.find_last_not_of(" ") + 1);        
    
    
    /*填充LOCALVTEP IP*/
    for (itLocalVtep=vecDbLocalVtep.begin(); itLocalVtep!=vecDbLocalVtep.end(); ++itLocalVtep)
    {
        VxlanMcGroup.m_vecLocalVtepIp.push_back(itLocalVtep->GetIp());
    }

    /*填充REMOTE VTEP IP*/
    vector<CDbVnaNpSegmentId>::iterator it;    
    for (it=vecDbRemoteVteps.begin(); it!=vecDbRemoteVteps.end(); ++it)
    {        
        VxlanMcGroup.m_vecVtepIp.push_back(it->GetIp());
    }

    return 0;  
}
/*******************************************************************************
* 函数名称： DbgSetVNAVxlanPrint
* 功能描述： 设置打印开关调试函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_ShowMcGroupInfo
* 功能描述： 显示组播组信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： void
* 输出参数： 无
* 返 回 值： void
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
    
    /* 从数据库中获取组播组信息*/
    CVxlanMcGroupTable  VxlanMcGroupTable;        
    if (VXLAN_ERROR==pVxlanMgr->GetMcGroupInfo(strVNAUuid, VxlanMcGroupTable))
    {
        cout << "Tst_ShowMcGroupInfo: GetMcGroupInfo fail vna:%s.\n" << strVNAUuid << endl;
        return;
    }    

    VxlanMcGroupTable.Print();    
}
/*******************************************************************************
* 函数名称： Tst_GetSwitchNameByVtepName
* 功能描述： 通过vtepname获取交换name
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_GetVtepNameBySwitchName
* 功能描述： 通过交换name获取vtepname
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_GetVtepNameBySwitchUuid
* 功能描述： 通过交换Uuid获取vtepname
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_GetNetplaneIdByVtep
* 功能描述： 通过vtep获取网络平面
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_AllocVtepIP
* 功能描述： 测试分配vtep ip函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_FreeVtepIP
* 功能描述： 测试回收vtep ip函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： Tst_RcvMcGroupInfoReq
* 功能描述： 测试收到VNA请求组播组消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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


