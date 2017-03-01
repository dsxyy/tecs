/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_vxlan_agent.cpp
* 文件标识：
* 内容摘要：CVxlanAgent类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年8月23日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Zhao.jin
*     修改内容：创建
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
* 函数名称： Init
* 功能描述： 初始化函数
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
* 函数名称： RcvMcGroupInfo
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
* 函数名称： DoDeleteMcGroup
* 功能描述： 处理删除组播组信息
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
void CVxlanAgent::DoDeleteMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    vector<CVxlanMcGroup> vecDelete;    
    vector<CVxlanMcGroup>::iterator iterOld;
    vector<CVxlanMcGroup>::iterator iterNew;
    
    /*找出要删除的*/
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

    /*对删除的直接destroy网桥*/
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
* 函数名称： DoUpdateMcGroup
* 功能描述： 处理更新组播组信息
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
void CVxlanAgent::DoUpdateMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    map<CVxlanMcGroup, CVxlanMcGroup> mapUpdate;    
    vector<CVxlanMcGroup>::iterator iterOld;
    vector<CVxlanMcGroup>::iterator iterNew;
    
    /*找出update的*/
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

    /*update的条目更新*/
    UpdateVxlanTunnel(mapUpdate);    
}
/*******************************************************************************
* 函数名称： DoAddMcGroup
* 功能描述： 处理添加组播组信息
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
void CVxlanAgent::DoAddMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    vector<CVxlanMcGroup> vecAdd;
    vector<CVxlanMcGroup>::iterator iterOld;
    vector<CVxlanMcGroup>::iterator iterNew;
    
    /*找出要添加的*/
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
    
    /*要创建的添加网桥同时设置vxlan隧道条目*/
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
* 函数名称： UpdateVxlanMcGroup
* 功能描述： 更新vxlan组播表
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
int32 CVxlanAgent::UpdateVxlanMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable)
{
    /*组播表没有变化*/
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
* 函数名称： SendMcGroupInfoReq
* 功能描述： VNA请求组播组信息
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
int32 CVxlanAgent::SendMcGroupInfoReq(void)
{
    string strVnmName;
    //如果没有注册信息，直接返回失败
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
        
    //向VNM请求VXLAN组播组信息
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
* 函数名称： CreateVxlanBridge
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
* 函数名称： DestroyVxlanBridge
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
BOOL  CVxlanAgent::IsVxlanBridgeExist(string strVxlanBridge)
{    
    //网桥是否存在
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
* 函数名称： GetVxlanBridgeName
* 功能描述： 获取VXLAN BRIDGE的名称
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
* 函数名称： PrepareDeployVxlanVm
* 功能描述： 部署vxlan vm前的准备工作，创建vxlan bridge，更新vxlan tunnel等
* 访问的表： 无
* 修改的表： 无
* 输入参数： CVMVSIInfo &vmVSIInfo
* 输出参数： 无
* 返 回 值： int32
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： CreateVxlanTunnel
* 功能描述： 创建与该bridge相关的所有vxlan tunnel信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： string strVxlanBridge
* 输出参数： 无
* 返 回 值： int32
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： GetSegmentId
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
string CVxlanAgent::GetSegmentId(string strVxlanBridge)
{
    if (strVxlanBridge.length() <= 7)
    {
        return "";
    }
    return strVxlanBridge.substr(7, (strVxlanBridge.length() - 7));
}
/*******************************************************************************
* 函数名称： GetNetplaneNickName
* 功能描述： 从bridge中获取网络平面nickname
* 访问的表： 无
* 修改的表： 无
* 输入参数： string strVxlanBridge
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： GetVxlanPortName
* 功能描述： 根据strNetplaneId,strSegmentId,remoteip构造vxpt端口的名称
* 访问的表： 无
* 修改的表： 无
* 输入参数： string strVxlanBridge
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： ConvertNum2string
* 功能描述： 把传入的nNum转换成52进制，52进制的数字用a~Z表示。 
* 访问的表： 无
* 修改的表： 无
* 输入参数： int32 nNum, string &s
* 输出参数： 无
* 返 回 值： string
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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

    /*构造映射表*/
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

    /*查找转换*/
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
* 函数名称： MonitorVxlanTunnel
* 功能描述： 监控vxlan tunnel信息
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
int32 CVxlanAgent::MonitorVxlanTunnel(string &strVxlanBridge)
{
    return 0;
}
/*******************************************************************************
* 函数名称： ShowMcGroupInfo
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
void CVxlanAgent::ShowMcGroupInfo(void)
{     
    m_VxlanMcGroupTable.Print();   
}
/*******************************************************************************
* 函数名称： UpdateVxlanTunnel
* 功能描述： 更新组播组中的tunnel信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： CVxlanMcGroup &VxlanMcGroup
* 输出参数： 无
* 返 回 值： int32
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
* 函数名称： UpdateVxlanTunnel
* 功能描述： 更新组播组中的tunnel信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： map<CVxlanMcGroup, CVxlanMcGroup> &mapUpdate
* 输出参数： 无
* 返 回 值： int32
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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
            /* 新表中没有itold */
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
            /* 老表中没有找到itnew，则说明需要增加 */
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
* 函数名称： IsMemberOfGroup
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
* 函数名称： DbgShowMcGroupInfo
* 功能描述： 显示组播组信息调试函数
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
* 函数名称： DbgShowMcGroupInfo
* 功能描述： 显示组播组信息调试函数
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
void DbgShow52Num(char num)
{
    map<int32, char> mapTable;
    uint32 i = 0;
    
    /*构造映射表*/
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
* 函数名称： Tst_SendMcGroupInfoReq
* 功能描述： 调试函数发送组播组信息请求
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
* 函数名称： Tst_CreateDeleteVxlanBridge
* 功能描述： 调试函数发送组播组信息请求
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
* 函数名称： Tst_GetVxlanPortName
* 功能描述： 调试函数测试构造vxlan端口名
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
* 函数名称： Tst_UpdateVxlanTunnel
* 功能描述： 调试函数更新vxlan隧道
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
* 函数名称： Tst_UpdateVxlanTunnel_Map
* 功能描述： 调试函数更新vxlan隧道
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
* 函数名称： Tst_UpdateVxlanMcGroup
* 功能描述： 调试函数更新组播组
* 访问的表： 无
* 修改的表： 无s
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/8          V1.0       zhaojin         创建
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

