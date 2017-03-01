/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_schedule.cpp
* 文件标识：
* 内容摘要：CVNMSchedule类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年3月16日
*
* 修改记录1：
*     修改日期：2013/3/16
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
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

namespace zte_tecs
{

int g_schedule_dbg_flag = 0;
CVNMSchedule *CVNMSchedule::s_pInstance = NULL;

//按MTU降序,待分配资源个数降序排列
BOOL SortCompareMTUResNum(CReqNumInfo a,CReqNumInfo b)
{
    if (a.m_nMTU > b.m_nMTU)
    {
        return true;
    }
    
    if (a.m_nSourceNum > b.m_nSourceNum)
    {
        return true;
    }

    return false;
}

/************************************************************
* 函数名称： GetSourceReqNum
* 功能描述： 汇总调度所需资源个数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CScheduleInfo::GetSourceReqNum(vector<CReqNumInfo> &vecMTUNum)
{
    int32 nSourceSum = 0;
    vector<CReqNumInfo>::iterator itrSch = vecMTUNum.begin();
    for ( ; itrSch != vecMTUNum.end(); ++itrSch)
    {
        nSourceSum += itrSch->m_nSourceNum;
    }

    return nSourceSum;
};

/************************************************************
* 函数名称： GetScheduleResult
* 功能描述： 汇总每个VNA满足条件资源个数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CScheduleInfo::GetScheduleResult(CScheduleTaskInfo &cSchTaskInfo, vector<string> &vecScheduledValid)
{
    int32 ret = 0;
    vector<ScheRet> vecScheRet;

    //如果有normal资源调度需求    
    if (0!=(ret=GetScheResult(cSchTaskInfo, m_vecMTUNum, vecScheRet, SCHE_NORMAL)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetScheResult m_vecMTUNum is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }
  
    //如果有SR-IOV bond资源调度需求
    if (0!=(ret=GetScheResult(cSchTaskInfo, m_vecSriovBondMTUNum, vecScheRet, SCHE_BOND)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetScheResult m_vecSriovBondMTUNum is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }
    
    //如果有vxlan资源调度需求
    if (0!=(ret=GetScheResult(cSchTaskInfo, m_vecVxlanMTUNum, vecScheRet, SCHE_VXLAN)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetScheResult m_vecVxlanMTUNum is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    //求普通口和SR-IOV bond口, vxlan调度结果交集
    if (0!=(ret=GetMergeSchResult(cSchTaskInfo.GetScheduleType(), vecScheRet, vecScheduledValid)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetMergeSchResult is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* 函数名称： GetScheResult
* 功能描述： 查询待调度资源DB信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CScheduleInfo::GetScheResult(CScheduleTaskInfo &cSchTaskInfo, 
                                       vector<CReqNumInfo> &vecMTUNum,
                                       vector<ScheRet> &vecScheRet, 
                                       int32 nQueryType)
{
    int32 ret = 0;
    
    if (!vecMTUNum.empty())
    {
        //查询网口调度DB
        ret = QueryDbScheduleInfo(cSchTaskInfo, vecMTUNum, nQueryType);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheResult:  call QueryDbScheduleInfo failed.\n");
            return ret;            
        }
        
        //资源汇总
        ScheRet tScheRet;
        GetDBScheduleResult(cSchTaskInfo.GetScheduleType(), vecMTUNum, 
            tScheRet.vecSchValid, tScheRet.vecVNAValid, tScheRet.vecDBSch);
        if (tScheRet.vecSchValid.empty())
        {
            VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheResult: the result of  GetDBScheduleResult is null.\n");
            return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
        }

        tScheRet.nReqNum = GetSourceReqNum(vecMTUNum);
        vecScheRet.push_back(tScheRet);
    }

    return SCHEDULE_OPER_SUCCESS;
    
}
/************************************************************
* 函数名称： QueryDbScheduleInfo
* 功能描述： 查询待调度资源DB信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CScheduleInfo::QueryDbScheduleInfo(CScheduleTaskInfo &cSchTaskInfo,
                            vector<CReqNumInfo> &vecMTUNum, int32 nQueryType)
{
    int32 ret = 0;    

    if (1 == g_schedule_dbg_flag)
    {
        cout << endl;
        cout << "CScheduleInfo::QueryDbScheduleInfo:"<< endl;
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    }
        
    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::QueryDbScheduleInfo: GetDbISchedule() is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }
    
    vector<CReqNumInfo>::iterator itr = vecMTUNum.begin();
    for ( ; itr != vecMTUNum.end(); ++itr)
    {
        CDbSchedule cDeployInput;
        cDeployInput.SetNetplaneUuid(m_strNetplane.c_str());
        cDeployInput.SetIsSriov(m_nIsSriov);
        cDeployInput.SetIsLoop(m_nIsLoop);
        cDeployInput.SetIsSdn(m_nIsSdn);
        cDeployInput.SetNicNum(itr->m_nSourceNum);
        cDeployInput.SetMtu(itr->m_nMTU);
        cDeployInput.SetIsHasWatchdog(cSchTaskInfo.GetWatchDogEnable());

        if (SCHE_BOND == nQueryType)
        {
            cDeployInput.SetIsBondNic(1);
        }

        if (SCHE_VXLAN == nQueryType)
        {
            cDeployInput.SetIsVxlan(1);
        }        

        //已部署过LOOP或者指定HC调度，需要指定VNA进行资源调度
        if ("" != cSchTaskInfo.GetAppointVNAUuid())
        {
            cDeployInput.SetVnaUuid(cSchTaskInfo.GetAppointVNAUuid().c_str());

            ret = pOper->ScheduleByVna(cDeployInput, itr->vecSchResult);
            if (ret != 0)
            {
                cDeployInput.DbgShow();
                VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::QueryDbScheduleInfo: call ScheduleByVna failed. ret=%d\n", ret);
                return ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL;
            }

            //如果已部署过loop的网络，删除非对应dvs的记录
            if ("" != itr->m_strDeployedDvs)
            {
                vector<CDbSchedule>::iterator itrSch = itr->vecSchResult.begin();
                for ( ; itrSch != itr->vecSchResult.end(); )
                {
                    if (itrSch->GetSwitchUuid() != itr->m_strDeployedDvs) 
                    {
                        itrSch = itr->vecSchResult.erase(itrSch);
                    }
                    else
                    {
                        ++itrSch;
                    }
                }
            }
            
            //无满足条件记录，直接返回调度失败
            if (itr->vecSchResult.empty())
            {
                cDeployInput.DbgShow();
                VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::QueryDbScheduleInfo: the result of  ScheduleByVna is null.\n");
                return ERR_SCHEDULE_FAILED_NO_RESOURCE;
            }
        }
        else
        {
            //"HC调度" 或者"指定CC调度"
            if ((cSchTaskInfo.GetScheduleType()== VNET_SCHEDULE_TYPE_HC) || ("" != cSchTaskInfo.GetAppointCCApp()))
            {
                cDeployInput.SetClusterName(cSchTaskInfo.GetAppointCCApp().c_str());

                ret = pOper->ScheduleByCluster(cDeployInput, itr->vecSchResult);
            }
            //普通CC调度
            else
            {
                ret = pOper->Schedule(cDeployInput, itr->vecSchResult);
            } 

            if (ret != 0)
            {
                VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::QueryDbScheduleInfo: call Schedule failed, ret=%d.\n", ret);
                return ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL;
            }
            //无满足条件记录，直接返回调度失败
            else if (itr->vecSchResult.empty())
            {
                VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::QueryDbScheduleInfo: the result of  Schedule is null.\n");
                return ERR_SCHEDULE_FAILED_NO_RESOURCE;
            }
        }

        if (1 == g_schedule_dbg_flag)
        {
            cout << endl;
            cout << "CDbSchedule info list:"<< endl;
            vector<CDbSchedule>::iterator itrSchDB = itr->vecSchResult.begin();
            for ( ; itrSchDB != itr->vecSchResult.end(); ++itrSchDB)
            {
                cout <<"CDbSchedule item:"<<endl;
                cout <<"<<<<:"<<endl;
                itrSchDB->DbgShow();
                cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
            }
        }
    }

    return SCHEDULE_OPER_SUCCESS;
};

/************************************************************
* 函数名称： GetDBScheduleResult
* 功能描述： 汇总待调度资源DB调度结果信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CScheduleInfo::GetDBScheduleResult(int32 nScheduleType, 
                                vector<CReqNumInfo> &vecMTUNum, 
                                vector<string> &vecSchValid, 
                                vector<string> &vecVNAValid, 
                                vector<CDbSchedule> &vecScheduleFinal)
{
    vector<string> vecVnaFinal;

    //先找出满足条件的VNA,求交集
    vector<CReqNumInfo>::iterator itrSch = vecMTUNum.begin();
    for ( ; itrSch != vecMTUNum.end(); ++itrSch)
    {
        if (itrSch == vecMTUNum.begin())
        {
            vector<CDbSchedule>::iterator itrDB = itrSch->vecSchResult.begin();
            for ( ; itrDB != itrSch->vecSchResult.end(); ++itrDB)
            {
                if (find(vecVnaFinal.begin(),vecVnaFinal.end(),itrDB->GetVnaUuid()) == vecVnaFinal.end())
                {
                    vecVnaFinal.push_back(itrDB->GetVnaUuid());
                }
            }
        }
        else
        {
            MergeScheduleVna(vecVnaFinal, itrSch->vecSchResult);
        }
    }
    
    //统计满足条件的记录，在上面交集的vna的对应资源里面求合集
    itrSch = vecMTUNum.begin();
    for ( ; itrSch != vecMTUNum.end(); ++itrSch)
    {
        MergeScheduleResult(vecVnaFinal, vecScheduleFinal, itrSch->vecSchResult);
        #if 0
        vector<CDbSchedule> vecTempSchedule;
        vector<CDbSchedule>::iterator iter = vecTempSchedule.begin() ;
        set_union(vecScheduleFinal.begin(),
            vecScheduleFinal.end(),
            itrSch->vecSchResult.begin(),
            itrSch->vecSchResult.end(),
            iter,
            CompareCDbSchedule());

        vecScheduleFinal= vecTempSchedule;
        vecTempSchedule.clear();
        #endif
    }

    if (1 == g_schedule_dbg_flag)
    {
        cout << endl;
        cout << "CScheduleInfo::GetDBScheduleResult(): after MergeScheduleResult"<<endl;
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        vector<CDbSchedule>::iterator itrDB = vecScheduleFinal.begin();
        for ( ; itrDB != vecScheduleFinal.end(); ++itrDB)
        {
            cout <<"CDbSchedule item:"<<endl;
            cout <<"<<<<:"<<endl;
            itrDB->DbgShow();
            cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        }
        cout << endl;
    }
    
    //按VNA统计个数
    vector<CVNAFreeSumInfo> vecVNAFreeInfo;

    vector<CDbSchedule>::iterator itr = vecScheduleFinal.begin();
    for ( ; itr != vecScheduleFinal.end(); ++itr)
    {
        CountVNAFreeInfo(vecVNAFreeInfo, *itr);
    }

    if (1 == g_schedule_dbg_flag)
    {
        cout << endl;
        cout << "CScheduleInfo::GetDBScheduleResult(): after CountVNAFreeInfo"<<endl;
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        vector<CVNAFreeSumInfo>::iterator itrDB1 = vecVNAFreeInfo.begin();
        for ( ; itrDB1 != vecVNAFreeInfo.end(); ++itrDB1)
        {
            itrDB1->Print();
        }
    }
    
    //筛选满足条件的VNA
    int32 nReqNum = GetSourceReqNum(vecMTUNum);
    vector<CVNAFreeSumInfo>::iterator itrVNA = vecVNAFreeInfo.begin();

    if (VNET_SCHEDULE_TYPE_CC == nScheduleType)
    {
        for ( ; itrVNA != vecVNAFreeInfo.end(); ++itrVNA)
        {
            if (nReqNum <= itrVNA->m_nFreeSum)
            {
            
                if (find(vecSchValid.begin(),vecSchValid.end(),itrVNA->m_strCCApp) == vecSchValid.end())
                {
                    vecSchValid.push_back(itrVNA->m_strCCApp);
                }

                if (find(vecVNAValid.begin(),vecVNAValid.end(),itrVNA->m_strVnaUuid) == vecVNAValid.end())
                {
                    vecVNAValid.push_back(itrVNA->m_strVnaUuid);
                }
            }
        }
    }
    else
    {
        for ( ; itrVNA != vecVNAFreeInfo.end(); ++itrVNA)
        {
            if (nReqNum <= itrVNA->m_nFreeSum)
            {
                if (find(vecSchValid.begin(),vecSchValid.end(),itrVNA->m_strHCApp) == vecSchValid.end())
                {
                    vecSchValid.push_back(itrVNA->m_strHCApp);
                }

                if (find(vecVNAValid.begin(),vecVNAValid.end(),itrVNA->m_strVnaUuid) == vecVNAValid.end())
                {
                    vecVNAValid.push_back(itrVNA->m_strVnaUuid);
                }
                
            }
        }
    }
        
};

/************************************************************
* 函数名称： GetDVSAllocResult
* 功能描述： 汇总每个VNA满足条件资源个数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CScheduleInfo::GetDVSAllocResult(string &strVNAUuid, int64 nProjectID, CScheduleTaskInfo &cSchTaskInfo, 
                                                 map< string, set<int32> > &mapVnicSriov)
{
    int32 nRet = SCHEDULE_OPER_SUCCESS;
        
    vector<CDbSchedule> vecDbBond; //已经分配的bond资源

    //先做vxlan的资源分配
    if(!m_vecVxlanMTUNum.empty())
    {
        nRet = QueryDbScheduleInfo(cSchTaskInfo, m_vecVxlanMTUNum, SCHE_VXLAN);
        if (0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: call QueryDbScheduleInfo  m_vecVxlanMTUNum failed.\n");
            return nRet;            
        }
        
        //资源汇总
        nRet = AllocDVSRes(strVNAUuid, nProjectID, m_vecVxlanMTUNum, SCHE_VXLAN, mapVnicSriov, vecDbBond);
        if (SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: the result of  AllocDVSRes  m_vecVxlanMTUNum  is null.\n");
            return nRet;            
        }
    }

    //普通口的资源分配        
    if(!m_vecMTUNum.empty())
    {
        nRet = QueryDbScheduleInfo(cSchTaskInfo, m_vecMTUNum, SCHE_NORMAL);
        if (0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: call QueryDbScheduleInfo m_vecMTUNum failed.\n");
            return nRet;            
        }
        
        //资源汇总
        nRet = AllocDVSRes(strVNAUuid, nProjectID, m_vecMTUNum, SCHE_NORMAL, mapVnicSriov, vecDbBond);
        if (SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: the result of  AllocDVSRes is null.\n");
            return nRet;            
        }
    }
    
    //如果有SR-IOV bond资源调度需求
    if (!m_vecSriovBondMTUNum.empty())
    {
        //查询SR-IOV bond网口调度DB
        nRet = QueryDbScheduleInfo(cSchTaskInfo, m_vecSriovBondMTUNum, SCHE_BOND);
        if (0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: call QueryDbScheduleInfo m_vecSriovBondMTUNum failed.\n");
            return nRet;            
        }
    
        //资源汇总
        nRet = AllocDVSRes(strVNAUuid, nProjectID, m_vecSriovBondMTUNum, SCHE_BOND, mapVnicSriov, vecDbBond);
        if (SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: the result of  AllocDVSRes m_vecSriovBondMTUNum is null.\n");
            return nRet;            
        }
    }
    
    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* 函数名称： AllocDVSRes
* 功能描述： 分配dvs资源
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       zj         创建
***************************************************************/
int32 CScheduleInfo::AllocDVSRes(string &strVNAUuid, int64 nProjectID, vector<CReqNumInfo> &vecMTUNum, 
                                     int32 nType, map< string, set<int32> > &mapVnicSriov, vector<CDbSchedule> &vecDbBond)
{
    int32 nRet = SCHEDULE_OPER_SUCCESS;
    
    //环回口先分配
    if(m_nIsLoop)
    {
        nRet = GetLoopDVSRes(strVNAUuid, nProjectID, vecMTUNum, mapVnicSriov);
        if(SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: call GetLoopDVSRes failed.\n");
            return nRet;
        }
    }
    
    //非sriov网卡资源分配
    if(!m_nIsSriov)
    {
        if (SCHE_NORMAL == nType || SCHE_VXLAN == nType)
        {
            nRet = GetSDVSRes(vecMTUNum);
            if(SCHEDULE_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: call GetSDVSRes failed.\n");
                return nRet;
            }
            return SCHEDULE_OPER_SUCCESS;
        }

        VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: nType(%d) error.\n", nType);
        return ERR_SCHEDULE_ALLOT_SDVSRES_FAIL;
    }
    //sriov网卡资源分配
    else
    {    
        //EDVS非BOND资源分配
        if (SCHE_NORMAL == nType)
        {
            nRet = GetEDVSRes(vecMTUNum, mapVnicSriov, vecDbBond);
            if(SCHEDULE_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: call GetEDVSRes failed.\n");
                return nRet;
            }
            return SCHEDULE_OPER_SUCCESS;
        }
        
        //EDVS BOND资源分配
        if (SCHE_BOND == nType)
        {
            nRet = GetEDVSBondRes(vecMTUNum, mapVnicSriov, vecDbBond);
            if(SCHEDULE_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: call GetEDVSBondRes failed.\n");
                return nRet;
            }
            return SCHEDULE_OPER_SUCCESS;
        }
        
        //EDVS VXLAN资源分配        
        if (SCHE_VXLAN == nType)
        {
            //EDVS的VXLAN调度后续再实现     
            return SCHEDULE_OPER_SUCCESS;
        }

        VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: nType(%d) error.\n", nType);
        return ERR_SCHEDULE_ALLOT_SDVSRES_FAIL;      
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* 函数名称： MergeAllocDBResult
* 功能描述： 更新数据库查询结果，是否要重新排序呢?
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       zj         创建
***************************************************************/
void CScheduleInfo::MergeAllocDBResult(vector<CDbSchedule> &vecOldSch, vector<CDbSchedule> &vecNewSch)
{
    if(vecNewSch.empty())
    {
        return;
    }

    vector<CDbSchedule>::iterator itrOld = vecOldSch.begin();
    for(; itrOld != vecOldSch.end(); ++itrOld)
    {
        vector<CDbSchedule>::iterator itrNew = vecNewSch.begin();
        for(; itrNew != vecNewSch.end(); ++itrNew)
        {
            if(itrNew->GetSwitchUuid() == itrOld->GetSwitchUuid())
            {
                itrOld->SetSwitchFreeNum(itrNew->GetSwitchFreeNum());
                itrOld->SetSwitchSriovFreeNum(itrNew->GetSwitchSriovFreeNum());
                itrOld->SetBondValidPhyPortNum(itrNew->GetBondValidPhyPortNum());
                if(itrNew->GetBondPhyPortUuid()==itrOld->GetBondPhyPortUuid())
                {
                    itrOld->SetBondPhyPortFreeNum(itrNew->GetBondPhyPortFreeNum());
                    break;
                }
            }
        }
    }
}

void CScheduleInfo::UpdateDBRes(vector<CDbSchedule> &vecNewSch, CDbSchedule &cDbSch)
{
    vector<CDbSchedule>::iterator itrNew;
    for(itrNew = vecNewSch.begin(); itrNew != vecNewSch.end(); ++itrNew)
    {
        if(itrNew->GetSwitchUuid() == cDbSch.GetSwitchUuid()) 
        {
            itrNew->SetSwitchFreeNum(cDbSch.GetSwitchFreeNum());
            itrNew->SetSwitchSriovFreeNum(cDbSch.GetSwitchSriovFreeNum());
            itrNew->SetBondValidPhyPortNum(cDbSch.GetBondValidPhyPortNum());
            if(itrNew->GetBondPhyPortUuid() == cDbSch.GetBondPhyPortUuid())
            {
                itrNew->SetBondPhyPortFreeNum(cDbSch.GetBondPhyPortFreeNum());
            } 
        }
    }
    //是否需要添加新的记录
    for(itrNew = vecNewSch.begin(); itrNew != vecNewSch.end(); ++itrNew)
    {
        if((itrNew->GetSwitchUuid() == cDbSch.GetSwitchUuid()) &&
            (itrNew->GetBondPhyPortUuid() == cDbSch.GetBondPhyPortUuid()))
        {
            return;
        }
    }
    vecNewSch.push_back(cDbSch);
}
    
//找出bond成员口中MTU最接近，资源个数最多的网卡
void CScheduleInfo::GetBondSchResult(vector<CDbSchedule> &vecRawDB, CDbSchedule &cMaxDB)
{
    vector<CDbSchedule>::iterator itrDB;
    for(itrDB = vecRawDB.begin(); itrDB != vecRawDB.end(); ++itrDB)
    {
        if(cMaxDB.GetSwitchUuid().empty())
        {
            if(itrDB->GetBondPhyPortFreeNum())
            {
                cMaxDB = *itrDB;
            }
            continue;
        }
        else
        {
            if((itrDB->GetSwitchUuid() == cMaxDB.GetSwitchUuid()) && 
                (itrDB->GetBondPhyPortFreeNum() > cMaxDB.GetBondPhyPortFreeNum()))
            {
                cMaxDB = *itrDB;
            }
        }
    }
}     
    
//网桥资源分配
int32 CScheduleInfo::GetSDVSRes(vector<CReqNumInfo> &vecMTUNum)
{
    int32 nSourceNum; //剩余需要分配的网卡个数
    vector<CDbSchedule> vecDBUpdate;      //已经被分配的数据
     
    vector<CReqNumInfo>::iterator itrSch; 
    for (itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {   
        //后面分配，和前面项存在相同DVS时，需要更新当前数据
        MergeAllocDBResult(itrSch->vecSchResult, vecDBUpdate);
        nSourceNum = itrSch->m_nSourceNum;

        //需要从MTU升序开始分配，即MTU最小优先分配
        vector<CDbSchedule>::iterator itrDBSch;     
        for(itrDBSch = itrSch->vecSchResult.begin(); itrDBSch != itrSch->vecSchResult.end(); ++itrDBSch)        
        {
            if(nSourceNum)    
            {
                if(m_nIsLoop)
                {
                    //modified by lvech:如果之前部署过loop，只能部署到之前部署过的交换上
                    if((itrSch->m_strDeployedDvs != "")
                        && itrSch->m_strDeployedDvs != itrDBSch->GetSwitchUuid())
                    {
                        continue;
                    }

                    //loop只能部署到一个交换上
                    if(itrDBSch->GetSwitchFreeNum() < nSourceNum)
                    {
                        continue;
                    }
                }   
                set<vector<CVNetVnicMem>::iterator>::iterator itrSet = itrSch->m_setVnicIter.begin();
                for(; itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
                {
                    if(!((*itrSet)->GetVSwitchUuid()).empty())
                    {
                       // nSourceNum--; 
                        continue; 
                    }
                    //如果还有剩余，剩余多少分配多少
                    if(itrDBSch->GetSwitchFreeNum() >= 1)
                    {
                        (*itrSet)->SetVSwitchUuid(itrDBSch->GetSwitchUuid());
                        itrDBSch->SetSwitchFreeNum(itrDBSch->GetSwitchFreeNum() - 1);
                        UpdateDBRes(vecDBUpdate, *itrDBSch); 
                        nSourceNum--;  
                        continue;
                    }
                    //当前交换中没有资源可配，找下一个交换。
                    break; //end for itrSet
                }
            }
            else
            {
                //vnic分配成功
                break; //end for itrDBSch
            }
        }
            
        //还有网卡未分配资源
        if(nSourceNum)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetSDVSRes: The vnics(%d) still alloc sdvs resources failed.\n", nSourceNum);
            return ERR_SCHEDULE_ALLOT_SDVSRES_FAIL;
        }    
    }
    
    return SCHEDULE_OPER_SUCCESS;
}

//EDVS 非bond资源分配
int32 CScheduleInfo::GetEDVSRes(vector<CReqNumInfo> &vecMTUNum,
                                        map< string, set<int32> > &mapVnicSriov,
                                        vector<CDbSchedule> &vecDbBond)
{
    int32 nSourceNum; //剩余需要分配的网卡个数
    vector<CDbSchedule> vecDBUpdate;  //已经被分配的数据
    
    vector<CReqNumInfo>::iterator itrSch;
    for ( itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {
        nSourceNum = itrSch->m_nSourceNum;
        //后面分配，和前面项存在相同DVS时，按剩余个数取最小值
        MergeAllocDBResult(itrSch->vecSchResult, vecDBUpdate);
            
        vector<CDbSchedule> vecDBSch;     //保存非bond排序结果，按MTU从小到大排列
        vector<CDbSchedule> vecDBBondSch; //保存bond排序结果，按MTU从小到大排列
        vector<CDbSchedule>::iterator itrOldDB;
        for(itrOldDB = itrSch->vecSchResult.begin(); itrOldDB != itrSch->vecSchResult.end(); ++itrOldDB)
        {
            /* DB中非bond在前，bond在后 */
            itrOldDB->DbgShow();
            if(itrOldDB->GetIsBondNic())
            {
                if(itrOldDB->GetBondPhyPortFreeNum())
                {
                    vecDBBondSch.push_back(*itrOldDB);
                }
            }
            else
            {
                if(itrOldDB->GetSwitchSriovFreeNum())
                {
                    vecDBSch.push_back(*itrOldDB);
                }
            }
        }
    
        //优先使用非bond交换
        if(!vecDBSch.empty())
        {
            vector<CDbSchedule>::iterator itrNewDB;
            for(itrNewDB = vecDBSch.begin(); itrNewDB != vecDBSch.end(); ++itrNewDB)
            {
                if(nSourceNum)
                {
                    if(m_nIsLoop)
                    {
                        //modified by lvech:如果之前部署过loop，只能部署到之前部署过的交换上
                        if ((itrSch->m_strDeployedDvs != "")
                            && itrSch->m_strDeployedDvs != itrNewDB->GetSwitchUuid())
                        {
                            continue;
                        }
                    
                        //loop只能部署到一个交换上
                        if(itrNewDB->GetSwitchSriovFreeNum() < nSourceNum)
                        {
                            continue;
                        }
                    }
                    set<vector<CVNetVnicMem>::iterator>::iterator itrSet = itrSch->m_setVnicIter.begin();
                    for(; itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
                    {
                        if(!((*itrSet)->GetVSwitchUuid()).empty())
                        {
                           // nSourceNum--; 
                            continue; //已经分配，不再重复分配。
                        }
                        if(itrNewDB->GetSwitchSriovFreeNum() >= 1)
                        {
                            (*itrSet)->SetVSwitchUuid(itrNewDB->GetSwitchUuid());
                            itrNewDB->SetSwitchSriovFreeNum(itrNewDB->GetSwitchSriovFreeNum() - 1);
                            itrNewDB->SetSwitchFreeNum(itrNewDB->GetSwitchFreeNum() - 1);
                            map< string, set<int32> >::iterator it_map = mapVnicSriov.find(itrNewDB->GetPortUuid());
                            if(it_map != mapVnicSriov.end())
                            {
                                (it_map->second).insert((*itrSet)->GetNicIndex());
                            }
                            else
                            {
                                set<int32> setIndex;
                                setIndex.insert((*itrSet)->GetNicIndex());
                                mapVnicSriov.insert(make_pair(itrNewDB->GetPortUuid(), setIndex));
                            }
                            UpdateDBRes(vecDBUpdate, *itrNewDB);
                            nSourceNum--;
                            continue;
                        }
                        //当前交换中没有资源可配，找下一个交换。
                        break;
                    } 
                }
            }
        }
        
        //非bond资源占用完，开始使用bond资源
        if(!vecDBBondSch.empty())
        {
            if(nSourceNum)
            {
                set<vector<CVNetVnicMem>::iterator>::iterator itrSet = itrSch->m_setVnicIter.begin();
                for(; itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
                {
                    MergeAllocDBResult(vecDBBondSch, vecDbBond); //当前剩余的可用资源
                    if(!((*itrSet)->GetVSwitchUuid()).empty())
                    {
                       // nSourceNum--;
                        continue; 
                    }
                         
                    CDbSchedule cDbSch; //找出MTU最接近，资源个数最多的网卡
                    GetBondSchResult(vecDBBondSch, cDbSch);
                    if(cDbSch.GetBondPhyPortFreeNum() >= 1)  
                    {
                        (*itrSet)->SetVSwitchUuid(cDbSch.GetSwitchUuid());
                        cDbSch.SetSwitchSriovFreeNum(cDbSch.GetSwitchSriovFreeNum() - 1);
                        cDbSch.SetBondPhyPortFreeNum(cDbSch.GetBondPhyPortFreeNum() - 1);
                        cDbSch.SetSwitchFreeNum(cDbSch.GetSwitchSriovFreeNum());
                            
                        //将虚拟网卡对应申请的物理网卡记录下来
                        map< string, set<int32> >::iterator it_map = mapVnicSriov.find(cDbSch.GetBondPhyPortUuid());
                        if(it_map != mapVnicSriov.end())
                        {
                            (it_map->second).insert((*itrSet)->GetNicIndex());
                        }
                        else
                        {
                            set<int32> setIndex;
                            setIndex.insert((*itrSet)->GetNicIndex());
                            mapVnicSriov.insert(make_pair(cDbSch.GetBondPhyPortUuid(), setIndex));
                        }
                        UpdateDBRes(vecDBUpdate, cDbSch);
                        UpdateDBRes(vecDbBond, cDbSch);
                        nSourceNum--;
                    }
                } 
            }
        }
        
        if(nSourceNum)
        {
            //没有资源可用
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetEDVSRes: The vnics(%d) still alloc edvs resources failed.\n", nSourceNum);
            return ERR_SCHEDULE_NO_EDVS_RES;
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}
                        
//EDVS bond资源分配
int32 CScheduleInfo::GetEDVSBondRes(vector<CReqNumInfo> &vecMTUNum, 
                                        map< string, set<int32> > &mapVnicSriov,
                                        vector<CDbSchedule> &vecDbBond)
{
    vector<CDbSchedule> vecDBUpdate;  //已经被分配的数据
    
    //非bond已经分配了部分bond资源
    vector<CDbSchedule>::iterator itBond = vecDbBond.begin();
    for(; itBond != vecDbBond.end(); ++itBond)
    {
        vecDBUpdate.push_back(*itBond);
    }

    vector<CReqNumInfo>::iterator itrSch;
    for (itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {
        MergeAllocDBResult(itrSch->vecSchResult, vecDBUpdate);

        /* 查找交换，规则:
         * 1、先找MTU最小最接近的；
           2、再找VF剩余个数匹配的；
           3、接着找bond有效物理端口数最接近的；
           4、*/
        string strDVSUuid;
        vector<CDbSchedule>::iterator itrDB;  
        for(itrDB = itrSch->vecSchResult.begin(); itrDB != itrSch->vecSchResult.end(); ++itrDB)
        {
            if(itrDB->GetSwitchSriovFreeNum() >= itrSch->m_nSourceNum)
            {
                //暂时只要VF个数满足，不考虑bond成员口最大匹配
                strDVSUuid = itrDB->GetSwitchUuid();
                break;
            }
        }
        if(strDVSUuid.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetEDVSBondRes: no switch to alloc, failed.\n");
            return ERR_SCHEDULE_NO_EDVS_BOND_RES;
        }

        //第二步，找出物理口，有多条记录
        vector<CDbSchedule> outInfo;
        for(itrDB = itrSch->vecSchResult.begin(); itrDB != itrSch->vecSchResult.end(); ++itrDB)
        {
            if(strDVSUuid == itrDB->GetSwitchUuid())
            {
                outInfo.push_back(*itrDB);
            }
        }
        
        //第三步，从多条记录中，VF交叉分配
        set<vector<CVNetVnicMem>::iterator>::iterator itrSet;
        for(itrSet = itrSch->m_setVnicIter.begin(); itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
        {
            if(!((*itrSet)->GetVSwitchUuid()).empty())
            {
                continue; //已经分配，不再重复分配。
            }
                                
            CDbSchedule cMaxSch; 
            vector<CDbSchedule>::iterator itrOutInfo;
            for(itrOutInfo = outInfo.begin(); itrOutInfo != outInfo.end(); ++itrOutInfo)
            {
                //只要有一个资源即可分配
                if(itrOutInfo->GetBondPhyPortFreeNum() >= 1)
                {
                    cMaxSch = *itrOutInfo;
                    break;
                }
            }
                
            //将虚拟网卡对应申请的物理网卡记录下来
            map< string, set<int32> >::iterator it_map = mapVnicSriov.find(cMaxSch.GetBondPhyPortUuid());
            if(it_map != mapVnicSriov.end())
            {
                (it_map->second).insert((*itrSet)->GetNicIndex());
            }    
            else
            {
                set<int32> setIndex;
                setIndex.insert((*itrSet)->GetNicIndex());
                mapVnicSriov.insert(make_pair(cMaxSch.GetBondPhyPortUuid(), setIndex));
            }
            
            //分配结果
            (*itrSet)->SetVSwitchUuid(cMaxSch.GetSwitchUuid());

            //更新数据
            cMaxSch.SetBondPhyPortFreeNum(cMaxSch.GetBondPhyPortFreeNum() - 1);
            cMaxSch.SetSwitchSriovFreeNum(cMaxSch.GetSwitchSriovFreeNum() - 1);
            cMaxSch.SetSwitchFreeNum(cMaxSch.GetSwitchSriovFreeNum());
            if(0 == cMaxSch.GetBondPhyPortFreeNum())
            {
                cMaxSch.SetBondValidPhyPortNum(cMaxSch.GetBondValidPhyPortNum() - 1);
            }
            
            //更新outInfo，先将选中的信息移出
            for(itrOutInfo = outInfo.begin(); itrOutInfo != outInfo.end(); ++itrOutInfo)
            {
                if(itrOutInfo->GetBondPhyPortUuid() == cMaxSch.GetBondPhyPortUuid())
                {
                    outInfo.erase(itrOutInfo);
                    break;
                }
            }
            outInfo.push_back(cMaxSch);
            UpdateDBRes(vecDBUpdate, cMaxSch); //更新已经分配的数据
            UpdateDBRes(outInfo, cMaxSch);   //更新当前bond已分配数据
        }
    }
    
    return SCHEDULE_OPER_SUCCESS;
}

int32 CScheduleInfo::GetLoopDVSRes(string &strVNAUuid, int64 nProjectID, vector<CReqNumInfo> &vecMTUNum, 
                                              map< string, set<int32> > &mapVnicSriov)
{
    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: GetDbISchedule is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }
                
    vector<CReqNumInfo>::iterator itrSch;
    for ( itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {
        //查询是否有已部署的LOOP记录
        CDbDeplySummary cDeployInput;
        cDeployInput.SetProjectId(nProjectID);
        cDeployInput.SetLgNetworkUuid(itrSch->m_strLogicNetwork.c_str());
        cDeployInput.SetIsSriov(m_nIsSriov);
        cDeployInput.SetIsLoop(m_nIsLoop);
        
        vector<CDbDeplySummary> vecDeployOut;
        if (0 != pOper->QueryDeployByProjectLn(cDeployInput, vecDeployOut))
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: call QueryDeployByProjectLn() failed!\n");
            return ERR_SCHEDULE_GET_LOOP_DEPLOYED_FAILED;
        }
        //没部署过
        else if (vecDeployOut.empty())
        {
            continue;
        } 
        //信息异常，多条LOOP已部署的记录
        else if (vecDeployOut.size() > 1)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: call QueryDeployByProjectLn() failed! The result num(%d) > 1\n", vecDeployOut.size());
            return ERR_SCHEDULE_LOOP_DEPLOYED_INFO_INVALID;
        }
        //已部署过
        else
        {
            vector<CDbDeplySummary>::iterator pDeployInfo = vecDeployOut.begin();
            if(strVNAUuid != pDeployInfo->GetVnaUuid())
            {
                VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: VNA is not match, alloc failed.\n");
                return ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT;
            }
                
            //所有网卡用原来网卡
            set<vector<CVNetVnicMem>::iterator>::iterator itrSet = itrSch->m_setVnicIter.begin();
            for(; itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
            {
                if(!((*itrSet)->GetVSwitchUuid()).empty())
                {
                    continue; //已经分配，不再重复分配。
                }

                //检查网卡是否还有资源
                int32 nFind = 0;
                vector<CDbSchedule>::iterator itrDB;  
                for(itrDB = itrSch->vecSchResult.begin(); itrDB != itrSch->vecSchResult.end(); ++itrDB)
                {
                    if((itrDB->GetSwitchUuid() == pDeployInfo->GetSwitchUuid()) &&
                        (itrDB->GetSwitchFreeNum() >= itrSch->m_nSourceNum))
                    {
                        nFind = 1;
                    }
                }
                //资源不够用，返回失败
                if(0 == nFind)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: switch(%s) has no resources.\n", 
                               itrDB->GetSwitchUuid().c_str());
                    return ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT;
                }

                //modified by lvech:此时不能设置SetVSwitchUuid，否则后面会调用GetSDVSRes时会分配失败
                //网桥获取
                //(*itrSet)->SetVSwitchUuid(pDeployInfo->GetSwitchUuid());
                
                //SR-IOV处理
                if(m_nIsSriov)
                {
                    map< string, set<int32> >::iterator it_map = mapVnicSriov.find(pDeployInfo->GetPortUuid());
                    if(it_map != mapVnicSriov.end())
                    {
                        (it_map->second).insert((*itrSet)->GetNicIndex());
                    } 
                    else
                    {
                        set<int32> setIndex;
                        setIndex.insert((*itrSet)->GetNicIndex());
                        mapVnicSriov.insert(make_pair(pDeployInfo->GetPortUuid(), setIndex));
                    }
                }
            } 

            itrSch->m_strDeployedDvs = pDeployInfo->GetSwitchUuid();
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}

/*
相同逻辑网络的单个SR-IOV虚拟网口需求，按如下维度汇总即可，
调度不需要考虑m_strLogicNetwork是否相同等因数
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    int32 m_nMTU;

*/

/*
相同逻辑网络下多个SR-IOV虚拟网口，按如下维度汇总即可，
调度不需要考虑MTU、VLAN等因数
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    string m_strLGNetwork;

调度的查询结果按如下条件给出:
1. 物理bond口剩余SR-IOV资源总数满足条件
2. 按bond口有可用SR-IOV资源的成员口个数从高到底排序
3. 按bond口剩余SR-IOV资源总数从高到低排序
4. 按bond口成员口剩余SR-IOV资源总数从高到低排序

分配资源时:
对于如下条件相同，按相同m_strLGNetwork需求网口个数从高到低
的顺序来分配网口资源
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;

分配的规则是，
1. 先按MTU从大到小来分配
2. 按相同逻辑网络的虚拟网口个数从大到小来分配

具体到每个bond口分配资源是，则按成员口剩余资源从高到低进行轮询分配


对于成员口个数和剩余资源总数都相同的bond口，需要考虑一下其顺序
比如对于如下两个bond
bond0: 114
bond1: 222
    
*/

/************************************************************
* 函数名称： CoutSriovReqInfo
* 功能描述： 汇总SR-IOV相关待调度资源信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CScheduleInfo::CoutSriovReqInfo()
{
    if (0 == m_nIsSriov)
    {
        return;
    }

    //SRIOV LOOP直接赋值  
    if (m_nIsLoop)
    {
        m_vecMTUNum = m_vecTempSriovMTUNum;
        return;
    }    
	
    //SRIOV非LOOP才有bond口需求, SR-IOV的相同逻辑网络的多个loop口不能调度到物理的bond SR-IOV口上
    //同时loop不能按MTU进行汇总
    //统计bond或者loop类型SRIOV资源需求
    vector<CReqNumInfo>::iterator itrSch = m_vecTempSriovMTUNum.begin();
    for ( ; itrSch != m_vecTempSriovMTUNum.end(); )
    {
        if (itrSch->m_nSourceNum > 1) 
        {
            m_vecSriovBondMTUNum.push_back(*itrSch);
            itrSch = m_vecTempSriovMTUNum.erase(itrSch);
        }
        else
        {
            ++itrSch;
        }
    }

    //统计普通类型SRIOV资源需求
    vector<CReqNumInfo>::iterator itrTmpSch = m_vecTempSriovMTUNum.begin();
    for ( ; itrTmpSch != m_vecTempSriovMTUNum.end(); ++itrTmpSch)
    {
        //按MTU统计单SR-IOV口的需求
        vector<CReqNumInfo>::iterator itrReq = m_vecMTUNum.begin();
        for ( ; itrReq != m_vecMTUNum.end(); ++itrReq)
        {
            if (itrReq->m_nMTU == itrTmpSch->m_nMTU)
            {
                itrReq->m_nSourceNum ++;
                itrReq->m_setVnicIter.insert(itrTmpSch->m_setVnicIter.begin(), itrTmpSch->m_setVnicIter.end());
                break;
            }
        }

        if (itrReq == m_vecMTUNum.end())
        {
            CReqNumInfo cMTUNumInfo;
            cMTUNumInfo.m_nMTU = itrTmpSch->m_nMTU;
            cMTUNumInfo.m_nSourceNum++;
            cMTUNumInfo.m_setVnicIter = itrTmpSch->m_setVnicIter;
            
            m_vecMTUNum.push_back(cMTUNumInfo);
        }
    }
}
/************************************************************
* 函数名称： GetMergeSchResult
* 功能描述： 汇总相关待调度资源信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       zj         创建
***************************************************************/
int32 CScheduleInfo::GetMergeSchResult(int32 nScheduleType, vector<ScheRet> &vecScheRet, vector<string> &vecSchValid)
{
    vector<string> vecVnaFinal;
    vector<CDbSchedule> vecDBSchFinal;   
    int32 nReqNum = 0;

    if (1 == g_schedule_dbg_flag)
    {
        cout << endl;
        cout << "CScheduleInfo::GetMergeSchResult(): the input vecScheRet info:"<<endl;
        vector<ScheRet>::iterator itrSche = vecScheRet.begin();
        for ( ; itrSche != vecScheRet.end(); ++itrSche)
        {
            cout << "ScheRet item info:" <<endl;
            cout << ">>>>:" <<endl;
            cout << "nReqNum:"<< itrSche->nReqNum <<endl;                      //需求个数

            cout << "vecSchValid info list:" <<endl;
            vector<string>::iterator itrSchValid = itrSche->vecSchValid.begin();
            for ( ; itrSchValid != itrSche->vecSchValid.end(); ++itrSchValid)
            {
                cout << (string)(*itrSchValid) << endl;
            }

            cout << "vecVNAValid info list:" <<endl;
            vector<string>::iterator itrVNAValid = itrSche->vecVNAValid.begin();
            for ( ; itrVNAValid != itrSche->vecVNAValid.end(); ++itrVNAValid)
            {
                cout << (string)(*itrVNAValid) << endl;
            }

            cout << "vecDBSch info list:" <<endl;
            vector<CDbSchedule>::iterator itrDBSch = itrSche->vecDBSch.begin();
            for ( ; itrDBSch != itrSche->vecDBSch.end(); ++itrDBSch)
            {
                itrDBSch->DbgShow();
            }
            cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;
        }
    }

    //汇总各种MTU
    //要先找出vna，再在此vna的基础找满足的db记录
    vector<ScheRet>::iterator itr = vecScheRet.begin();
    for ( ; itr != vecScheRet.end(); ++itr)
    {
        if (itr == vecScheRet.begin())
        {
            nReqNum     = itr->nReqNum;
            vecVnaFinal = itr->vecVNAValid;
        }
        else
        {
            nReqNum = nReqNum + itr->nReqNum;
            GetVectorInsertion(vecVnaFinal, itr->vecVNAValid);
        }        
    }    

    itr = vecScheRet.begin();
    for ( ; itr != vecScheRet.end(); ++itr)
    {
            MergeScheduleResult(vecVnaFinal, vecDBSchFinal, itr->vecDBSch);            
    }        

    if (vecVnaFinal.empty())
    {
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;
    }

    //按VNA统计个数
    vector<CVNAFreeSumInfo> vecVNAFreeInfo;
    vector<CDbSchedule>::iterator iter = vecDBSchFinal.begin();
    for ( ; iter != vecDBSchFinal.end(); ++iter)
    {
        CountVNAFreeInfo(vecVNAFreeInfo, *iter);
    }
    
    //筛选满足条件的VNA    
    vector<CVNAFreeSumInfo>::iterator itrVNA = vecVNAFreeInfo.begin();
    if (VNET_SCHEDULE_TYPE_CC == nScheduleType)
    {
        for ( ; itrVNA != vecVNAFreeInfo.end(); ++itrVNA)
        {
            if (nReqNum <= itrVNA->m_nFreeSum)
            {
                if (find(vecSchValid.begin(),vecSchValid.end(), itrVNA->m_strCCApp) == vecSchValid.end())
                {
                    vecSchValid.push_back(itrVNA->m_strCCApp);
                }
            }
        }
    }
    else
    {
        for ( ; itrVNA != vecVNAFreeInfo.end(); ++itrVNA)
        {
            if (nReqNum <= itrVNA->m_nFreeSum)
            {
                if (find(vecSchValid.begin(),vecSchValid.end(), itrVNA->m_strHCApp) == vecSchValid.end())
                {
                    vecSchValid.push_back(itrVNA->m_strHCApp);
                }
            }
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* 函数名称： AddMTUNum
* 功能描述： 添加某个逻辑网络资源需求个数，供LOOP口用
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CScheduleInfo::AddMTUNum(string strLGNetworkUuid, int32 nMTU, vector<CVNetVnicMem>::iterator iter, int32 nIsolateType)
{
    //SRIOV类资源需求
    if (m_nIsSriov)
    {
        if (nIsolateType==ISOLATE_VXLAN)
        {
            vector<CReqNumInfo>::iterator itrSch = m_vecVxlanMTUNum.begin();
            for ( ; itrSch != m_vecVxlanMTUNum.end(); ++itrSch)
            {
                if (itrSch->m_strLogicNetwork == strLGNetworkUuid)
                {
                    itrSch->m_nSourceNum ++;
                    itrSch->m_setVnicIter.insert(iter);
                    return;
                }
            }

            if (itrSch == m_vecVxlanMTUNum.end())
            {
                CReqNumInfo cMTUNumInfo;
                cMTUNumInfo.m_nMTU = nMTU;
                cMTUNumInfo.m_strLogicNetwork = strLGNetworkUuid;
                cMTUNumInfo.m_nSourceNum++;
                cMTUNumInfo.m_setVnicIter.insert(iter);

                m_vecVxlanMTUNum.push_back(cMTUNumInfo);
            }
        }     
        else
        {
            vector<CReqNumInfo>::iterator itrSch = m_vecTempSriovMTUNum.begin();
            for ( ; itrSch != m_vecTempSriovMTUNum.end(); ++itrSch)
            {
                if (itrSch->m_strLogicNetwork == strLGNetworkUuid)
                {
                    itrSch->m_nSourceNum ++;
                    itrSch->m_setVnicIter.insert(iter);
                    return;
                }
            }

            if (itrSch == m_vecTempSriovMTUNum.end())
            {
                CReqNumInfo cMTUNumInfo;
                cMTUNumInfo.m_nMTU = nMTU;
                cMTUNumInfo.m_strLogicNetwork = strLGNetworkUuid;
                cMTUNumInfo.m_nSourceNum++;
                cMTUNumInfo.m_setVnicIter.insert(iter);

                m_vecTempSriovMTUNum.push_back(cMTUNumInfo);
            }
        }
    }
    //普通网口LOOP类资源需求
    else
    {
        if (nIsolateType==ISOLATE_VXLAN)
        {
            vector<CReqNumInfo>::iterator itrSch = m_vecVxlanMTUNum.begin();
            for ( ; itrSch != m_vecVxlanMTUNum.end(); ++itrSch)
            {
                if (itrSch->m_strLogicNetwork == strLGNetworkUuid)
                {
                    itrSch->m_nSourceNum ++;
                    itrSch->m_setVnicIter.insert(iter);
                    return;
                }
            }

            if (itrSch == m_vecVxlanMTUNum.end())
            {
                CReqNumInfo cMTUNumInfo;
                cMTUNumInfo.m_nMTU = nMTU;
                cMTUNumInfo.m_strLogicNetwork = strLGNetworkUuid;
                cMTUNumInfo.m_nSourceNum++;
                cMTUNumInfo.m_setVnicIter.insert(iter);

                m_vecVxlanMTUNum.push_back(cMTUNumInfo);
            }
        }     
        else
        {
            vector<CReqNumInfo>::iterator itrSch = m_vecMTUNum.begin();
            for ( ; itrSch != m_vecMTUNum.end(); ++itrSch)
            {
                if (itrSch->m_strLogicNetwork == strLGNetworkUuid)
                {
                    itrSch->m_nSourceNum ++;
                    itrSch->m_setVnicIter.insert(iter);
                    return;
                }
            }

            if (itrSch == m_vecMTUNum.end())
            {
                CReqNumInfo cMTUNumInfo;
                cMTUNumInfo.m_nMTU = nMTU;
                cMTUNumInfo.m_strLogicNetwork = strLGNetworkUuid;
                cMTUNumInfo.m_nSourceNum++;
                cMTUNumInfo.m_setVnicIter.insert(iter);

                m_vecMTUNum.push_back(cMTUNumInfo);
            }
        }
    }
    
}
/************************************************************
* 函数名称： AddMTUNum
* 功能描述： 添加某个MTU网络资源需求个数，供非LOOP口用
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CScheduleInfo::AddMTUNum(int32 nMTU, vector<CVNetVnicMem>::iterator iter, int32 nIsolateType)
{
    if (nIsolateType==ISOLATE_VXLAN)
    {
        vector<CReqNumInfo>::iterator itrSch = m_vecVxlanMTUNum.begin();
        for ( ; itrSch != m_vecVxlanMTUNum.end(); ++itrSch)
        {
            if ((itrSch->m_nMTU == nMTU) && (itrSch->m_strLogicNetwork == ""))
            {
                itrSch->m_nSourceNum ++;
                itrSch->m_setVnicIter.insert(iter);
                return;
            }
        }

        if (itrSch == m_vecVxlanMTUNum.end())
        {
            CReqNumInfo cMTUNumInfo;
            cMTUNumInfo.m_nMTU = nMTU;
            cMTUNumInfo.m_nSourceNum++;
            cMTUNumInfo.m_setVnicIter.insert(iter);

            m_vecVxlanMTUNum.push_back(cMTUNumInfo);
        }        
    }
    else
    {
        vector<CReqNumInfo>::iterator itrSch = m_vecMTUNum.begin();
        for ( ; itrSch != m_vecMTUNum.end(); ++itrSch)
        {
            if ((itrSch->m_nMTU == nMTU) && (itrSch->m_strLogicNetwork == ""))
            {
                itrSch->m_nSourceNum ++;
                itrSch->m_setVnicIter.insert(iter);
                return;
            }
        }

        if (itrSch == m_vecMTUNum.end())
        {
            CReqNumInfo cMTUNumInfo;
            cMTUNumInfo.m_nMTU = nMTU;
            cMTUNumInfo.m_nSourceNum++;
            cMTUNumInfo.m_setVnicIter.insert(iter);

            m_vecMTUNum.push_back(cMTUNumInfo);
        }
    }
}

CVNMSchedule::CVNMSchedule()
{
    m_pMU = NULL;
    m_bOpenDbgInf = TRUE;
    m_pVNetVmDB = NULL;
    m_pVNetVnicDB = NULL;
    m_pVSIProfileMgr = NULL;
    m_pSwitchMgr = NULL;
    m_pVNAMgr = NULL;
    m_pLogicNetworkMgr = NULL;
};

CVNMSchedule::~CVNMSchedule()
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
* 函数名称： Init
* 功能描述： CVNetHeartbeat对象初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::Init()
{
    m_pVNetVmDB = CVNetVmDB::GetInstance();
    if( NULL == m_pVNetVmDB)
    {
        cout << "CVNMSchedule::Init CVNetVmDB::GetInstance() is NULL" << endl;
        return -1;
    }
    
    m_pVNetVnicDB = CVNetVnicDB::GetInstance();
    if( NULL == m_pVNetVnicDB)
    {
        cout << "CVNMSchedule::Init CVNetVnicDB::GetInstance() is NULL" << endl;
        return -1;
    }

    //vsi
    m_pVSIProfileMgr = CVSIProfileMgr::GetInstance();
    if (NULL == m_pVSIProfileMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::Init CVSIProfileMgr::GetInstance().\n");
        return -1;
    }
        
    m_pSwitchMgr = CSwitchMgr::GetInstance();
    if( NULL == m_pSwitchMgr)
    {
        cout << "CVNMSchedule::Init  CSwitchMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pVNAMgr = CVNAManager::GetInstance();
    if( NULL == m_pVNAMgr)
    {
        cout << "CVNMSchedule::Init  CVNAManager::GetInstance() is NULL" << endl;
        return -1;
    }

    m_pLogicNetworkMgr = CLogicNetworkMgr::GetInstance();
    if( NULL == m_pLogicNetworkMgr)
    {
        cout << "CVNMSchedule::Init  CLogicNetworkMgr::GetInstance() is NULL" << endl;
        return -1;
    }

    if (StartMu(MU_VNM_SCHEDULE))
    {
        cout << "CVNMSchedule::StartMu failed!" << endl;
        return -1;
    }

    return 0;
}        
/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
STATUS CVNMSchedule::StartMu(string strMsgUnitName)
{
    if (m_pMU)
    {
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    m_pMU = new MessageUnit(strMsgUnitName);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::StartMu Create mu %s failed!\n", strMsgUnitName.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::StartMu SetHandler mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::StartMu Run mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::StartMu Register mu %s failed! ret = %d\n", strMsgUnitName.c_str(),ret);
        return ret;
    }
    
    //给自己发送上电消息，促使消息单元状态切换到工作态
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
* 函数名称： MessageEntry
* 功能描述： 消息主分发函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::MessageEntry(const MessageFrame &message)
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
                cout << "CVNMSchedule::GetAdoDB failed!" << endl;
            }

            cout << "CVNMSchedule:GetAdoDB()==================="<< pthread_self() <<endl;


            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::MessageEntry %s power on!\n", m_pMU->name().c_str());
            break;
        }
        default:
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::MessageEntry Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_VNM_SCHEDULE_CC:
        {
            ScheduleCC(message);
            break;
        }
        
        case EV_VNM_SCHEDULE_HC:
        {
            ScheduleHC(message);
            break;
        }
        
        case EV_VNM_GET_RESOURCE_FOR_CC:
        {
            GetNetResourceOnCC(message);
            break;
        }
        
        case EV_VNM_FREE_RESOURCE_FOR_CC:
        {
            FreeNetResourceOnCC(message);
            break;
        }

        case EV_VNM_GET_RESOURCE_FOR_HC:
        {
            GetNetResourceOnHC(message);
            break;
        }
        
        case EV_VNM_FREE_RESOURCE_FOR_HC:
        {
            FreeNetResourceOnHC(message);
            break;
        }

        default:
        {
            cout << "CVNMSchedule::MessageEntry: receive an unkown message!"
            << endl;
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::MessageEntry Unknown message: %d.\n", message.option.id());
            break;
        }
        }
        break;
    }

    default:
        break;
    }
}


/************************************************************
* 函数名称： ScheduleCC
* 功能描述： 调度CC信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::ScheduleCC(const MessageFrame& message)
{
    int32 ret = SCHEDULE_OPER_SUCCESS;
    vector<string> vecScheduledCC;
    vector<string>::iterator itrCC;
    
    CVNetSelectCCListMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Receive SelectCCListForVM message:" <<endl;
        cout << cMsg.serialize();
    }
    
    //对于没有调度需求的，统一返回成功，不做任何处理
    if (cMsg.m_VmNicList.empty() && (true != cMsg.m_WatchDogInfo.m_nIsWDGEnable))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: vecVnicCfg is NULL and m_nIsWDGEnable is false.\n");
        ret = SCHEDULE_OPER_SUCCESS;
        goto SEND_ACK;
    }

//暂时不检查
#if 0
    //目前SR-IOV不支持access模式，需要检查
    ret = m_pLogicNetworkMgr->CheckAllVlanTrunkOfSRIOV(cMsg.m_VmNicList);
    if (1 == ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: CheckAllVlanTrunkOfSRIOV failed.\n");
        ret = ERR_SCHEDULE_CHECK_SRIOV_CFG_FAIL;
        goto SEND_ACK;        
    }
    else if (-1 == ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: CheckAllVlanTrunkOfSRIOV failed.\n");
        ret = ERR_SCHEDULE_CHECK_SRIOV_CFG_FAIL_OTHER;
        goto SEND_ACK;        
    }
#endif
    
    //只有wdg需求
    if (cMsg.m_VmNicList.empty())
    {
        /*看门狗调度*/    
        ret = ScheduleWdgCC(vecScheduledCC);
        if (0 != ret)
        {
            cout << "ScheduleCC: call ScheduleWdgCC failed, ret = " << ret << endl;
            goto SEND_ACK;
        }
    }
    //有网卡需求
    else
    {
        //汇总调度资源请求信息
        vector <CScheduleInfo> vecScheduleInfo;
        map<string, int32> mapNetplaneSum;
        map<string, int32> mapLGNetworkSum;  
        ret = GetScheduleReqInfo(cMsg.m_VmNicList, vecScheduleInfo, mapNetplaneSum, mapLGNetworkSum);
        if (0 != ret)
        {
            cout << "call GetScheduleInfo failed" << endl;
            goto SEND_ACK;
        }

        //检查 MAC资源是否满足
        ret = CheckMacRes(mapNetplaneSum);
        if (0 != ret)
        {
            cout << "call CheckMacRes failed" << endl;
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: call CheckMacRes failed, ret=%d.\n", ret);
            goto SEND_ACK;
        }

        //检查 MAC资源是否满足
        ret = CheckIPRes(mapLGNetworkSum);
        if (0 != ret)
        {
            cout << "call CheckIPRes failed" << endl;
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: call CheckIPRes failed, ret=%d.\n", ret);
            goto SEND_ACK;
        }

        //先检查是否有LOOP已部署过的情况
        CScheduleTaskInfo cSchTaskInfo;
        cSchTaskInfo.SetScheduleType(VNET_SCHEDULE_TYPE_CC);
        cSchTaskInfo.SetWatchDogEnable(cMsg.m_WatchDogInfo.m_nIsWDGEnable);
        cSchTaskInfo.SetAppointCCApp(cMsg.m_appointed_cc);

        ret = CheckLoopDeploy(cMsg.m_project_id, cSchTaskInfo, vecScheduleInfo);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: call CheckLoopDeploy failed, ret=%d.\n", ret);
            goto SEND_ACK;
        }    

        /*3.0 按资源进行调度*/    
        ret = ScheduleProc(cSchTaskInfo, 
                vecScheduleInfo, 
                vecScheduledCC);
        if (0 != ret)
        {
            cout << "ScheduleCC: call ScheduleProc failed, ret = " << ret << endl;
            goto SEND_ACK;
        }
    }

    //至此调度成功

SEND_ACK:
    CVNetSelectCCListMsgAck cMsgAck(cMsg.action_id,
                                    ret,
                                    0,
                                    GetVnetErrorInfo(ret),
                                    vecScheduledCC);
                                    
    cMsgAck.progress = (ret == SUCCESS) ? 100 : 0;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    MessageOption option(message.option.sender(), EV_VNM_SCHEDULE_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Send SelectCCListForVM ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： ScheduleHC
* 功能描述： 调度HC信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::ScheduleHC(const MessageFrame& message)
{
    int ret = SCHEDULE_OPER_SUCCESS;
    string strAppointVnaUuid;

    vector<string> vecScheduledHC;
    vector<string>::iterator itrHC;

    CVNetSelectHCListMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Receive CVNetSelectHCListMsg message:" <<endl;
        cout << cMsg.serialize();
    }
    
    //对于没有调度需求的，统一返回成功，不做任何处理
    if (cMsg.m_VmNicList.empty() && (true != cMsg.m_WatchDogInfo.m_nIsWDGEnable))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleHC: vecVnicCfg is NULL.\n");
        ret = SCHEDULE_OPER_SUCCESS;
        goto SEND_ACK;
    }

    if ("" == cMsg.m_cc_application)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleHC: Input valid\n");
        ret = ERR_SCHEDULE_CC_APP_IS_NULL;
        goto SEND_ACK;
    }

    //检查是否有指定HC部署
    if ("" != cMsg.m_appointed_hc)
    {
        CVNAManager * m_pVNAMgr = CVNAManager::GetInstance();
        ret = m_pVNAMgr->QueryVNAUUID(cMsg.m_cc_application, cMsg.m_appointed_hc, strAppointVnaUuid);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleHC: call QueryVNAUUID failed, ret=%d.\n", ret);
            ret = ERR_SCHEDULE_APPOINTED_HC_INVALID;
            goto SEND_ACK;
        }
    }

    //只有wdg需求
    if (cMsg.m_VmNicList.empty())
    {
        /*看门狗调度*/    
        ret = ScheduleWdgHC(vecScheduledHC);
        if (0 != ret)
        {
            cout << "CVNMSchedule::ScheduleHC: call ScheduleProc ScheduleWdgHC failed, ret = " << ret << endl;
            goto SEND_ACK;
        }
    }
    //有网卡需求
    else
    {
        //汇总调度资源请求信息
        vector <CScheduleInfo> vecScheduleInfo;
        map<string, int32> mapNetplaneSum;
        map<string, int32> mapLGNetworkSum;  
        ret = GetScheduleReqInfo(cMsg.m_VmNicList, vecScheduleInfo, mapNetplaneSum, mapLGNetworkSum);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleHC: call GetScheduleReqInfo failed.\n");
            goto SEND_ACK;
        }

        //先检查是否有LOOP已部署过的情况
        CScheduleTaskInfo cSchTaskInfo;
        cSchTaskInfo.SetScheduleType(VNET_SCHEDULE_TYPE_HC);
        cSchTaskInfo.SetWatchDogEnable(cMsg.m_WatchDogInfo.m_nIsWDGEnable);
        cSchTaskInfo.SetAppointCCApp(cMsg.m_cc_application);
        cSchTaskInfo.SetAppointVNAUuid(strAppointVnaUuid);

        ret = CheckLoopDeploy(cMsg.m_project_id, cSchTaskInfo, vecScheduleInfo);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleHC: call CheckLoopDeploy failed.\n");
            goto SEND_ACK;
        }    

        /*3.0 按资源进行调度*/
        ret = ScheduleProc(cSchTaskInfo, 
                vecScheduleInfo, 
                vecScheduledHC);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleHC: call ScheduleProc failed.ret %d\n", ret);
            goto SEND_ACK;
        }
    }

    //至此调度成功
    
SEND_ACK:
    CVNetSelectHCListMsgAck cMsgAck;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = ret;
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (ret == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
    cMsgAck.m_select_hc_list = vecScheduledHC;

    MessageOption option(message.option.sender(), EV_VNM_SCHEDULE_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Send SelectHCListForVM ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
};

/************************************************************
* 函数名称： GetNetResourceOnCC
* 功能描述： TC为VM选定部署CC后，向VNM申请分配网络资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::GetNetResourceOnCC(const MessageFrame& message)
{
    int32 iRet = SUCCESS;
    vector<CVNetVnicMem> vecVnicMem;
    vector<CVNetVnicConfig> vecVnicCfg;
    
    CVNetGetResourceCCMsg cMsg;
    cMsg.deserialize(message.data);
    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Receive GetNetResourceOnCC message:" <<endl;
        cout << cMsg.serialize();
    }

    //格式转换
    CVNetVmMem cVnetVmMem;
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }

    //先查询是否已有对应的资源，可重入，防止重复申请
    iRet = m_pVNetVmDB->GetVm(cVnetVmMem);
    if ((0 != iRet) && !VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call GetVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
        goto RETURN_PROC;
    }

    //SET Cluster
    cVnetVmMem.SetVmId(cMsg.m_vm_id);
    cVnetVmMem.SetProjectId(cMsg.m_project_id);
    cVnetVmMem.SetClusterName(cMsg.m_cc_application);
    cVnetVmMem.SetIsHasWatchdog(cMsg.m_WatchDogInfo.m_nIsWDGEnable);
    cVnetVmMem.SetWatchdogTimeout(cMsg.m_WatchDogInfo.m_nTimeOut);

    // 创建quantum网络
    m_pLogicNetworkMgr->CreateQuantumNetwork(cVnetVmMem);

    //调用VSI接口生成VSI信息,包括VLAN/MAC/IP等资源
    iRet = m_pVSIProfileMgr->AllocVSIResForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call AllocVSIResForVnic failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }

    //vnic入库,包括VSI信息
    iRet = m_pVNetVmDB->InsertVm(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call InsertVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_ADD_VNIC_TO_DB_FAIL;
        goto RETURN_PROC;
    }

    //格式转换
    cVnetVmMem.GetVecVnicMem(vecVnicMem);
    VnicMemTrans2Cfg(vecVnicMem, vecVnicCfg);//cMsgAck.m_VmNicCfgList);
    if (0 == iRet)
    {
        iRet = SUCCESS;
    }
    
RETURN_PROC:
    CVNetGetResourceCCMsgAck cMsgAck;

    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    cMsgAck.state = iRet;   
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
    cMsgAck.m_VmNicCfgList= vecVnicCfg;

    MessageOption option(message.option.sender(),EV_VNM_GET_RESOURCE_FOR_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Send GetNetResourceOnCC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： FreeNetResourceOnCC
* 功能描述： TC向VNM申请为VM释放网络资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::FreeNetResourceOnCC(const MessageFrame& message)
{
    int32 iRet = 0;
    CVNetVmMem cVnetVmMem;
    
    CVNetFreeResourceCCMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Receive FreeNetResourceOnCC message:" <<endl;
        cout << cMsg.serialize();
    }

    CPortGroupMgr *pPortGroupMgr = CPortGroupMgr::GetInstance();
    if( NULL == pPortGroupMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: CPortGroupMgr::GetInstance failed.\n");
        iRet = ERR_VNMVNIC_GET_PG_MGR_INST_FAIL;
        goto RETURN_PROC;
    }
    
    //格式转换
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }
    
    //先查询是否已有对应的资源
    iRet = m_pVNetVmDB->GetVm(cVnetVmMem);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        iRet = SUCCESS;
        goto RETURN_PROC;
    }
    else if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call GetVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
        goto RETURN_PROC;
    }

    //删除VNM上VNIC
    iRet = m_pVNetVmDB->DeleteVm(cMsg.m_vm_id);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: call DeleteVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_DEL_VNIC_FROM_DB_FAIL;
        goto RETURN_PROC;
    }
    
    //调用VSI接口释放VSI信息,包括VLAN/MAC/IP等资源
    iRet = m_pVSIProfileMgr->FreeVSIResForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: call FreeVSIResForVnic failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }

    // 删除quantum端口
    pPortGroupMgr->DeleteQuantumPort(cVnetVmMem);

    // 删除quantum网络
    m_pLogicNetworkMgr->DeleteQuantumNetwork(cVnetVmMem);

RETURN_PROC:
    //计算要求撤销VM流程中失败情况不返回消息
    CVNetFreeResourceCCMsgAck cMsgAck;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet; 
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
    MessageOption option(message.option.sender(),EV_VNM_FREE_RESOURCE_FOR_CC_ACK,0,0);
    m_pMU->Send(cMsgAck,option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Send FreeNetResourceOnCC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }

}

/************************************************************
* 函数名称： GetNetResourceOnHC
* 功能描述： CC为VM选定部署HC后，向VNM申请分配PCI资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::GetNetResourceOnHC(const MessageFrame& message)
{
    int32 iRet = 0;
    string strVNAUuid;
    CVNetVmMem cVnetVmMem;

    CVNetGetResourceHCMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Receive GetNetResourceOnHC message:" <<endl;
        cout << cMsg.serialize();
    }

    CPortGroupMgr *pPortGroupMgr = CPortGroupMgr::GetInstance();
    if( NULL == pPortGroupMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: CPortGroupMgr::GetInstance failed.\n");
        iRet = ERR_VNMVNIC_GET_PG_MGR_INST_FAIL;
        goto RETURN_PROC;
    }
        
    //格式转换
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }
    
    //先查询是否已有对应的资源，可重入，防止重复申请
    iRet = m_pVNetVmDB->GetVm(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call GetVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
        goto RETURN_PROC;
    }

    if (cVnetVmMem.GetClusterName() != cMsg.m_cc_application)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: cVnetVmMem.ClusterName(%s)!= cMsg.m_cc_application(%s).\n", 
            cVnetVmMem.GetClusterName().c_str(), cMsg.m_cc_application.c_str());
        iRet = ERR_VNMVNIC_CC_APP_CONFLICT;
        goto RETURN_PROC;
    }

    //获取VNA uuid
    iRet = m_pVNAMgr->QueryVNAUUID(cMsg.m_cc_application, cMsg.m_hc_application, strVNAUuid);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call QueryVNAUUID failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_GET_VNA_UUID_FAIL;
        goto RETURN_PROC;
    }

    // RESTful接口创建port,network从vsi中获取
    pPortGroupMgr->CreateQuantumPort(cVnetVmMem);

    cVnetVmMem.SetHostName(cMsg.m_hc_application);
    //调用网桥模块接口确定bridge名称或PCI .NO
    iRet = m_pSwitchMgr->AllocDVSRes(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call AllocDVSRes failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_ALLOC_DVS_FAIL;
        goto RETURN_PROC;
    }

    //设置host name
    cVnetVmMem.SetHostName(cMsg.m_hc_application);
    
    //更新信息到DB中,包括PCI和bridge信息
    iRet = m_pVNetVmDB->UpdateVm(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call UpdateVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_UPDATE_VNIC_TO_DB_FAIL;
        goto RETURN_PROC;
    }

RETURN_PROC:
    //向CC回应答消息
    CVNetGetResourceHCMsgAck cMsgAck;
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.state = iRet;   
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;
        
    MessageOption cc_option(message.option.sender(), EV_VNM_GET_RESOURCE_FOR_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,cc_option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Send GetNetResourceOnHC ack message:" <<endl;
        cout << cMsgAck.serialize();
    }
        
    //向对应的VNA下发增加该VM相关的VSI信息
    if (0 == iRet)
    {
        CAddVMVSIInfoMsg cVsiMsg;
        cVsiMsg.m_cVMVSIInfo.m_nVMID = cMsg.m_vm_id;
        cVsiMsg.m_cVMVSIInfo.m_nProjectID = cMsg.m_project_id;
        cVsiMsg.m_cVMVSIInfo.m_cWatchDogInfo = cMsg.m_WatchDogInfo;

        //需要确保从数据库读取的vnic的排序和计算的消息中排序一致
        cVsiMsg.SetVnicInfo(cVnetVmMem);

        iRet = m_pVSIProfileMgr->GetVSIDetailInfo(cMsg.m_vm_id, cVsiMsg.m_cVMVSIInfo);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call GetVSIDetailInfo failed, ret=%d.\n", iRet);
            return;
        }

        MID cVnaMID;
        cVnaMID._application = strVNAUuid;
        cVnaMID._process = PROC_VNA;
        cVnaMID._unit = MU_VNA_CONTROLLER;
            
        MessageOption vna_option(cVnaMID, EV_VNM_NOTIFY_VNA_ADD_VMVSI,0,0);
        m_pMU->Send(cVsiMsg,vna_option);

        //部署vm时，向对应的vna下发vxlan组播信息
        CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
        if (pVxlanMgr)
        {
            pVxlanMgr->SendMcGroupInfo(strVNAUuid);
        }    

        if (m_bOpenDbgInf)
        {
            cout << "CVNMSchedule: Send CAddVMVSIInfoMsg message to VNA(" << strVNAUuid <<"):" <<endl;
            cout << cVsiMsg.serialize();
        }
    }
}

/************************************************************
* 函数名称： FreeNetResourceOnHC
* 功能描述： CC向VNM申请为VM释放网络资源的请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： vmniccfg、HC信息
* 输出参数： pci信息
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::FreeNetResourceOnHC(const MessageFrame& message)
{
    int32 iRet = 0;
    CVNetVmMem cVnetVmMem;
    
    CVNetFreeResourceHCMsg cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Receive FreeNetResourceOnHC message:" <<endl;
        cout << cMsg.serialize();
    }
    
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnHC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }
    
    //先查询是否已有对应的资源，可重入，防止重复申请
    iRet = m_pVNetVmDB->GetVm(cVnetVmMem);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        iRet = SUCCESS;
        goto RETURN_PROC;
    }
    else if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnHC: call GetVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL;
        goto RETURN_PROC;
    }
    
    //调用网桥模块接口释放对bridgename或PCI .NO的占用
    iRet = m_pSwitchMgr->FreeDVSRes(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnHC: call FreeDVSRes failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_FREE_DVS_FAIL;
        goto RETURN_PROC;
    }

    //清除host name
    cVnetVmMem.SetHostName(cMsg.m_hc_application);
    
    //更新信息到DB中，主要是清除VSI中PCI和bridge相关信息
    iRet = m_pVNetVmDB->UpdateVm(cVnetVmMem);
    if (VNET_DB_IS_ITEM_NO_EXIST(iRet))
    {
        iRet = SUCCESS;
        goto RETURN_PROC;
    }
    else if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnHC: call UpdateVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_UPDATE_VNIC_TO_DB_FAIL;
        goto RETURN_PROC;
    }

RETURN_PROC:

    //向CC回应答消息
    CVNetFreeResourceHCMsgAck cMsgAck;
    VNET_SET_ACKMSG_REQUEST_INFO(cMsg, cMsgAck);
    cMsgAck.m_vm_id = cMsg.m_vm_id;
    cMsgAck.state = iRet;   
    cMsgAck.detail = GetVnetErrorInfo(cMsgAck.state);
    cMsgAck.progress = (iRet == SUCCESS) ? 100 : 0;
    cMsgAck.action_id = cMsg.action_id;

    MessageOption cc_option(message.option.sender(),EV_VNM_FREE_RESOURCE_FOR_HC_ACK,0,0);
    m_pMU->Send(cMsgAck,cc_option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNMSchedule: Send FreeNetResourceOnHC ack:" <<endl;
        cout << cMsgAck.serialize();
    }
}

/************************************************************
* 函数名称： ScheduleWdgCC
* 功能描述： 看门狗调度CC
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::ScheduleWdgCC(vector<string> &vecScheduledCC)
{
    int32 ret = 0;

    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleWdgCC: GetDbISchedule() is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }

    vector<CDbScheduleWdg> outInfo;
    ret = pOper->ScheduleWdg(outInfo);
    if (ret != 0)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleWdgCC: call ScheduleWdg failed. ret=%d\n", ret);
        return ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL;
    }

    vector<CDbScheduleWdg>::iterator itrSch = outInfo.begin();
    for ( ; itrSch != outInfo.end(); ++itrSch)
    {
        if (find(vecScheduledCC.begin(),vecScheduledCC.end(), itrSch->GetClusterName()) == vecScheduledCC.end())
        {
            vecScheduledCC.push_back(itrSch->GetClusterName());
        }
    }

    //如果没有交集，返回调度失败
    if (vecScheduledCC.empty())
    {
        VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleWdgCC: the result of  setScheduledCC is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    return 0;
}

/************************************************************
* 函数名称： ScheduleWdgHC
* 功能描述： 看门狗调度HC
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::ScheduleWdgHC(vector<string> &vecScheduledHC)
{
    int32 ret = 0;

    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleWdgHC: GetDbISchedule() is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }

    vector<CDbScheduleWdg> outInfo;
    ret = pOper->ScheduleWdg(outInfo);
    if (ret != 0)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleWdgHC: call ScheduleWdg failed. ret=%d\n", ret);
        return ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL;
    }

    vector<CDbScheduleWdg>::iterator itrSch = outInfo.begin();
    for ( ; itrSch != outInfo.end(); ++itrSch)
    {
        if (find(vecScheduledHC.begin(),vecScheduledHC.end(), itrSch->GetModuleName()) == vecScheduledHC.end())
        {
            vecScheduledHC.push_back(itrSch->GetModuleName());
        }
    }

    //如果没有交集，返回调度失败
    if (vecScheduledHC.empty())
    {
        VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleWdgHC: the result of  setScheduledHC is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    return 0;
}
/************************************************************
* 函数名称： GetScheduleReqInfo
* 功能描述： 汇总调度所需资源信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::GetScheduleReqInfo(vector <CVNetVnicConfig> &vecVnicCfg, 
                                            vector <CScheduleInfo> &vecScheduleInfo,
                                            map<string, int32> &mapNetplaneSum,
                                            map<string, int32> &mapLGNetworkSum)
{
    int32 iRet = SCHEDULE_OPER_SUCCESS;
    
    if (SCHEDULE_OPER_SUCCESS != (iRet=CountMtus(vecVnicCfg, vecScheduleInfo)))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetScheduleReqInfo: call CountMtus failed.ret %d\n", iRet);
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;
    }

    if (SCHEDULE_OPER_SUCCESS != (iRet=CountNetplanes(vecVnicCfg, mapNetplaneSum)))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetScheduleReqInfo: call CountNetplanes failed.ret %d\n", iRet);
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;
    }

    if (SCHEDULE_OPER_SUCCESS != (iRet=CountLogicNetworks(vecVnicCfg, mapLGNetworkSum)))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetScheduleReqInfo: call CountLogicNetworks failed.ret %d\n", iRet);
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;
    }

    return SCHEDULE_OPER_SUCCESS;   
}

/************************************************************
* 函数名称： CountMtus
* 功能描述： 统计mtu
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::CountMtus(vector <CVNetVnicConfig> &vecVnicCfg, vector <CScheduleInfo> &vecScheduleInfo)
{
    int32 iRet = SCHEDULE_OPER_SUCCESS;
            
    vector<CVNetVnicConfig>::iterator itrVnic = vecVnicCfg.begin();
    for ( ; itrVnic != vecVnicCfg.end(); ++itrVnic)
    {
        CPortGroup cPortGroup;
        if (SCHEDULE_OPER_SUCCESS != (iRet=GetPgByLogicNetwork(itrVnic->m_strLogicNetworkID, cPortGroup)))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CountMtus: call GetPgByLogicNetwork failed.ret %d\n", iRet);
            return ERR_SCHEDULE_GET_PG_DETAIL_FAILED;
        }

        //统计MTU资源需求情况
        vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
        for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
        {
            if ((itrSch->GetNetplane() == cPortGroup.GetNetplaneUuid())
                && (itrSch->GetIsLoop() == itrVnic->m_nIsLoop)
                && (itrSch->GetIsSriov() == itrVnic->m_nIsSriov)
                && (itrSch->GetIsSdn() == cPortGroup.GetIsSdn()))
            {
                //loop类资源需要按LogicNetworkID分类统计，由于不同LogicNetworkID的MTU可能相同
                //SR-IOV类资源需要按LogicNetworkID分类统计，由于一个LogicNetworkID的申请个数对资源分配有要求
                if (itrVnic->m_nIsLoop || itrVnic->m_nIsSriov)
                {
                    itrSch->AddMTUNum(itrVnic->m_strLogicNetworkID,cPortGroup.GetMtu(),(vector<CVNetVnicMem>::iterator)NULL,cPortGroup.GetIsolateType());
                }
                //非loop类资源则只需要按MTU分类统计即可
                else
                {
                    itrSch->AddMTUNum(cPortGroup.GetMtu(),(vector<CVNetVnicMem>::iterator)NULL,cPortGroup.GetIsolateType());
                }
                
                break;
            }
        }

        if (itrSch == vecScheduleInfo.end())
        {
            CScheduleInfo cScheduleInfo;
            cScheduleInfo.SetNetplane(cPortGroup.GetNetplaneUuid());
            cScheduleInfo.SetIsSriov(itrVnic->m_nIsSriov);
            cScheduleInfo.SetIsLoop(itrVnic->m_nIsLoop);
            cScheduleInfo.SetIsSdn(cPortGroup.GetIsSdn());
            if (itrVnic->m_nIsLoop || itrVnic->m_nIsSriov)
            {
                cScheduleInfo.AddMTUNum(itrVnic->m_strLogicNetworkID, cPortGroup.GetMtu(),(vector<CVNetVnicMem>::iterator)NULL,cPortGroup.GetIsolateType());
            }
            else
            {
                cScheduleInfo.AddMTUNum(cPortGroup.GetMtu(),(vector<CVNetVnicMem>::iterator)NULL,cPortGroup.GetIsolateType());
            }

            vecScheduleInfo.push_back(cScheduleInfo);
        }        
    }

    //SRIOV资源需要进行二次汇总
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        itrSch->CoutSriovReqInfo();
    }

    //排序
    vector<CScheduleInfo>::iterator itrSchInfo = vecScheduleInfo.begin();
    for( ; itrSchInfo != vecScheduleInfo.end(); ++itrSchInfo)
    {
        if(!itrSchInfo->m_vecMTUNum.empty())
        {
            //按MTU和资源数排序
            sort(itrSchInfo->m_vecMTUNum.begin(), itrSchInfo->m_vecMTUNum.end(), SortCompareMTUResNum);
        }
        
        if(!itrSchInfo->m_vecSriovBondMTUNum.empty())
        {
            //按MTU和资源数排序
            sort(itrSchInfo->m_vecSriovBondMTUNum.begin(), itrSchInfo->m_vecSriovBondMTUNum.end(), SortCompareMTUResNum);
        }

        if(!itrSchInfo->m_vecVxlanMTUNum.empty())
        {
            //按MTU和资源数排序
            sort(itrSchInfo->m_vecVxlanMTUNum.begin(), itrSchInfo->m_vecVxlanMTUNum.end(), SortCompareMTUResNum);
        }
    }    
    
    if (1 == g_schedule_dbg_flag)
    {
        //调试信息
        cout << endl;
        cout << "CVNMSchedule::CountMtus(): "<<endl;
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        vector<CScheduleInfo>::iterator itrDB1 = vecScheduleInfo.begin();
        for ( ; itrDB1 != vecScheduleInfo.end(); ++itrDB1)
        {
            itrDB1->Print();
        }
        cout << endl;
    }

    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* 函数名称： CountMtus
* 功能描述： 统计mtu
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       zj         创建
***************************************************************/
int32 CVNMSchedule::CountMtus(vector <CVNetVnicMem> &vecVnicMem, vector <CScheduleInfo> &vecScheduleInfo)
{
    int32 iRet = SCHEDULE_OPER_SUCCESS;
            
    vector<CVNetVnicMem>::iterator itrVnic = vecVnicMem.begin();
    for ( ; itrVnic != vecVnicMem.end(); ++itrVnic)
    {
        CPortGroup cPortGroup;
        if (SCHEDULE_OPER_SUCCESS != (iRet=GetPgByLogicNetwork(itrVnic->GetLogicNetworkUuid(), cPortGroup)))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CountMtus: call GetPgByLogicNetwork failed.ret %d\n", iRet);
            return ERR_SCHEDULE_GET_PG_DETAIL_FAILED;
        }

        //统计MTU资源需求情况
        vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
        for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
        {
            if ((itrSch->GetNetplane() == cPortGroup.GetNetplaneUuid())
                && (itrSch->GetIsLoop() == itrVnic->GetIsLoop())
                && (itrSch->GetIsSriov() == itrVnic->GetIsSriov())
                && (itrSch->GetIsSdn() == cPortGroup.GetIsSdn()))
            {
                //loop类资源需要按LogicNetworkID分类统计，由于不同LogicNetworkID的MTU可能相同
                //SR-IOV类资源需要按LogicNetworkID分类统计，由于一个LogicNetworkID的申请个数对资源分配有要求
                if (itrVnic->GetIsLoop() || itrVnic->GetIsSriov())
                {
                    itrSch->AddMTUNum(itrVnic->GetLogicNetworkUuid(),cPortGroup.GetMtu(),itrVnic,cPortGroup.GetIsolateType());
                }
                //非loop类资源则只需要按MTU分类统计即可
                else
                {
                    itrSch->AddMTUNum(cPortGroup.GetMtu(),itrVnic,cPortGroup.GetIsolateType());
                }
                
                break;
            }
        }

        if (itrSch == vecScheduleInfo.end())
        {
            CScheduleInfo cScheduleInfo;
            cScheduleInfo.SetNetplane(cPortGroup.GetNetplaneUuid());
            cScheduleInfo.SetIsSriov(itrVnic->GetIsSriov());
            cScheduleInfo.SetIsLoop(itrVnic->GetIsLoop());
            cScheduleInfo.SetIsSdn(cPortGroup.GetIsSdn());
            if (itrVnic->GetIsLoop() || itrVnic->GetIsSriov())
            {
                cScheduleInfo.AddMTUNum(itrVnic->GetLogicNetworkUuid(), cPortGroup.GetMtu(),itrVnic,cPortGroup.GetIsolateType());
            }
            else
            {
                cScheduleInfo.AddMTUNum(cPortGroup.GetMtu(),itrVnic,cPortGroup.GetIsolateType());
            }

            vecScheduleInfo.push_back(cScheduleInfo);
        }        
    }

    //SRIOV资源需要进行二次汇总
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        itrSch->CoutSriovReqInfo();
    }

    //排序
    vector<CScheduleInfo>::iterator itrSchInfo = vecScheduleInfo.begin();
    for( ; itrSchInfo != vecScheduleInfo.end(); ++itrSchInfo)
    {
        if(!itrSchInfo->m_vecMTUNum.empty())
        {
            //按MTU和资源数排序
            sort(itrSchInfo->m_vecMTUNum.begin(), itrSchInfo->m_vecMTUNum.end(), SortCompareMTUResNum);
        }
        
        if(!itrSchInfo->m_vecSriovBondMTUNum.empty())
        {
            //按MTU和资源数排序
            sort(itrSchInfo->m_vecSriovBondMTUNum.begin(), itrSchInfo->m_vecSriovBondMTUNum.end(), SortCompareMTUResNum);
        }

        if(!itrSchInfo->m_vecVxlanMTUNum.empty())
        {
            //按MTU和资源数排序
            sort(itrSchInfo->m_vecVxlanMTUNum.begin(), itrSchInfo->m_vecVxlanMTUNum.end(), SortCompareMTUResNum);
        }
    }    
    
    if (1 == g_schedule_dbg_flag)
    {
        //调试信息
        cout << endl;
        cout << "CVNMSchedule::CountMtus(): "<<endl;
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        vector<CScheduleInfo>::iterator itrDB1 = vecScheduleInfo.begin();
        for ( ; itrDB1 != vecScheduleInfo.end(); ++itrDB1)
        {
            itrDB1->Print();
        }
        cout << endl;
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* 函数名称： CountNetplanes
* 功能描述： 统计网络平面个数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::CountNetplanes(vector <CVNetVnicConfig> &vecVnicCfg, map<string, int32> &mapNetplaneSum)
{
    int32 iRet = SCHEDULE_OPER_SUCCESS;
    
    vector<CVNetVnicConfig>::iterator itrVnic = vecVnicCfg.begin();
    for ( ; itrVnic != vecVnicCfg.end(); ++itrVnic)
    {
        //指定MAC地址的不需要进行统计
        if (0 != itrVnic->m_strMac.size())
        {
            continue;
        }

        CPortGroup cPortGroup;
        if (SCHEDULE_OPER_SUCCESS != (iRet=GetPgByLogicNetwork(itrVnic->m_strLogicNetworkID, cPortGroup)))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CountNetplanes: call GetPgByLogicNetwork failed.ret %d\n", iRet);
            return ERR_SCHEDULE_GET_PG_DETAIL_FAILED;
        }
        
        //统计NETPLANE层次资源需求情况, 即MAC地址的资源需求
        map<string, int32>::iterator itrNP = mapNetplaneSum.find(cPortGroup.GetNetplaneUuid());
        if (itrNP != mapNetplaneSum.end())
        {
            (itrNP->second)++;
        }
        else
        {
            mapNetplaneSum.insert(pair<string, int32> (cPortGroup.GetNetplaneUuid(), 1));
        }        
    }   

    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* 函数名称： CountLogicNetworks
* 功能描述： 统计逻辑网络个数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::CountLogicNetworks(vector <CVNetVnicConfig> &vecVnicCfg, map<string, int32> &mapLGNetworkSum)
{
    vector<CVNetVnicConfig>::iterator itrVnic = vecVnicCfg.begin();
    for ( ; itrVnic != vecVnicCfg.end(); ++itrVnic)
    {
        //指定IP地址的不需要进行统计
        if (0 != itrVnic->m_strIP.size())
        {
            continue;
        }

        //统计LG_NETWORK层次资源需求情况, 即IP地址的资源需求
        map<string, int32>::iterator itrLG = mapLGNetworkSum.find(itrVnic->m_strLogicNetworkID);
        if (itrLG != mapLGNetworkSum.end())
        {
            (itrLG->second)++;
        }
        else
        {
            mapLGNetworkSum.insert(pair<string, int32> (itrVnic->m_strLogicNetworkID, 1));
        }
    }  

    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* 函数名称： GetPgByLogicNetwork
* 功能描述： 通过逻辑网络获取pg
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       赵进         创建
***************************************************************/
int32 CVNMSchedule::GetPgByLogicNetwork(string  m_strLogicNetworkID, CPortGroup &cPortGroup)
{
    int32 iRet = SCHEDULE_OPER_SUCCESS;
    
    CLogicNetworkMgr *pLGMgr = CLogicNetworkMgr::GetInstance();
    if (NULL == pLGMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetPgByLogicNetwork: call CLogicNetworkMgr::GetInstance() failed.\n");
        return ERR_SCHEDULE_GET_LN_MGR_INST_FAIL;
    }

    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if (NULL == pPGMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetPgByLogicNetwork: call CPortGroupMgr::GetInstance() failed.\n");
        return ERR_SCHEDULE_GET_PG_MGR_INST_FAIL;
    }    

    CLogicNetwork cLogicNetwork("");
    iRet = pLGMgr->GetLogicNetwork(m_strLogicNetworkID, cLogicNetwork);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetPgByLogicNetwork: call GetLogicNetwork(%s) failed.\n", m_strLogicNetworkID.c_str());
        return ERR_SCHEDULE_GET_LG_NETWORK_FAILED;
    }
    
    string strPortGroupUUID;
    iRet = pLGMgr->GetPortGroupUUID(m_strLogicNetworkID, strPortGroupUUID);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetPgByLogicNetwork: call GetPortGroupUUID(%s) failed.\n", m_strLogicNetworkID.c_str());
        return ERR_SCHEDULE_GET_PORT_GROUP_FAILED;
    }

    cPortGroup.SetUuid(strPortGroupUUID);
    iRet = pPGMgr->GetPGDetail(cPortGroup);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetPgByLogicNetwork: call GetPGDetail(%s) failed.\n", strPortGroupUUID.c_str());
        return ERR_SCHEDULE_GET_PG_DETAIL_FAILED;
    }

    return SCHEDULE_OPER_SUCCESS;   
}


/************************************************************
* 函数名称： CheckMacRes
* 功能描述： 检查是否MAC资源可用
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::CheckMacRes(map<string, int32> &mapNetplaneSum)
{ 
    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CheckMacRes: GetDbISchedule is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }
    
    map<string, int32>::iterator itrSch = mapNetplaneSum.begin();
    for ( ; itrSch != mapNetplaneSum.end(); ++itrSch)
    {
        int32 iRet = pOper->CheckMacResource(itrSch->first.c_str(), itrSch->second);
        if (VNET_DB_SUCCESS != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVNMSchedule::CheckMacRes: call CheckMacResource(%s, %d) failed, ret=%d.\n", 
                itrSch->first.c_str(), 
                itrSch->second, 
                iRet);
            return ERR_SCHEDULE_CHECK_MAC_FAIL;
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* 函数名称： CheckIPRes
* 功能描述： 检查是否IP资源可用
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::CheckIPRes(map<string, int32> &mapLGNetworkSum)
{ 
    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CheckIPRes: GetDbISchedule is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }
    
    map<string, int32>::iterator itrSch = mapLGNetworkSum.begin();
    for ( ; itrSch != mapLGNetworkSum.end(); ++itrSch)
    {
        int32 iRet = pOper->CheckIpResource(itrSch->first.c_str(), itrSch->second);
        if (VNET_DB_SUCCESS != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CVNMSchedule::CheckIPRes: call CheckIpResource(%s, %d) failed, ret=%d.\n", 
                itrSch->first.c_str(), 
                itrSch->second, 
                iRet);
            return ERR_SCHEDULE_CHECK_IP_FAIL;
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* 函数名称： CheckLoopDeploy
* 功能描述： 检查是否有LOOP已部署过的情况
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::CheckLoopDeploy(int64 nProjectID, CScheduleTaskInfo &cSchTaskInfo, vector <CScheduleInfo> &vecScheduleInfo)
{
    int32 nDeployedFlag = VNET_LOOP_DEPLOYED_FLAG_NO;
    string strDeployedVnaUuid;
    string strDeployedCluster;
    
    CDBOperateSchedule * pOper = GetDbISchedule();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CheckLoopDeploy: GetDbISchedule is NULL\n");
        return ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED;
    }
    
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        if (itrSch->GetIsLoop())
        {
            vector<CReqNumInfo>::iterator itr = itrSch->m_vecMTUNum.begin();
            for ( ; itr != itrSch->m_vecMTUNum.end(); ++itr)
            {
                //查询是否有已部署的LOOP记录
                CDbDeplySummary cDeployInput;
                cDeployInput.SetProjectId(nProjectID);
                cDeployInput.SetLgNetworkUuid(itr->m_strLogicNetwork.c_str());
                cDeployInput.SetIsSriov(itrSch->GetIsSriov());
                cDeployInput.SetIsLoop(itrSch->GetIsLoop());
                
                vector<CDbDeplySummary> vecDeployOut;
                int ret = pOper->QueryDeployByProjectLn(cDeployInput, vecDeployOut);
                if (ret != 0)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CheckLoopDeploy: call QueryDeployByProjectLn() failed!\n");
                    return ERR_SCHEDULE_GET_LOOP_DEPLOYED_FAILED;
                }
                //没部署过
                else if (vecDeployOut.empty())
                {
                    continue;
                }
                //信息异常，多条LOOP已部署的记录
                else if (vecDeployOut.size() > 1)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CheckLoopDeploy: call QueryDeployByProjectLn() failed!The result num(%d) > 1\n", vecDeployOut.size());
                    return ERR_SCHEDULE_LOOP_DEPLOYED_INFO_INVALID;
                }
                //已部署过
                else
                {
                    vector<CDbDeplySummary>::iterator pDeployInfo = vecDeployOut.begin();
                
                    if (VNET_LOOP_DEPLOYED_FLAG_YES == nDeployedFlag)
                    {
                        if (strDeployedVnaUuid != pDeployInfo->GetVnaUuid())
                        {
                            strDeployedVnaUuid = "";
                            strDeployedCluster = "";
                            return ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT;
                        }
                    }
                    else
                    {
                        nDeployedFlag = VNET_LOOP_DEPLOYED_FLAG_YES;
                        strDeployedVnaUuid =  pDeployInfo->GetVnaUuid();
                        strDeployedCluster =  pDeployInfo->GetClusterName();
                    }

                    //设置该logicnetwork对应已部署过的dvs_uuid
                    itr->m_strDeployedDvs = pDeployInfo->GetSwitchUuid();
                }
            }
        }
    }

    if (VNET_LOOP_DEPLOYED_FLAG_NO == nDeployedFlag)
    {
        return SCHEDULE_OPER_SUCCESS;
    }
    else if (("" == strDeployedVnaUuid) || ("" == strDeployedCluster))
    {
        cout << "strLoopVNAUuid is NULL" << endl;
        return ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT;
    }

    if (VNET_SCHEDULE_TYPE_CC == cSchTaskInfo.GetScheduleType())
    {
        if (("" != cSchTaskInfo.GetAppointCCApp()) 
            && (strDeployedCluster != cSchTaskInfo.GetAppointCCApp()))
        {
            cout << "strLoopCluster != cMsg.m_appointed_cc" << endl;
            return ERR_SCHEDULE_LOOP_DEPLOYED_CC_CONFLICT;
        }
        else
        {
            cSchTaskInfo.SetAppointVNAUuid(strDeployedVnaUuid);
        }
    }
    else
    {
        if (strDeployedCluster != cSchTaskInfo.GetAppointCCApp())
        {
            cout << "strLoopVNAUuid("<<strDeployedCluster<<") != strCCApp(" <<cSchTaskInfo.GetAppointCCApp()<<")" << endl;
            return ERR_SCHEDULE_LOOP_DEPLOYED_CC_CONFLICT;
        }

        if ( ("" != cSchTaskInfo.GetAppointVNAUuid())
            && (strDeployedVnaUuid != cSchTaskInfo.GetAppointVNAUuid()))
        {
            cout << "strLoopVNAUuid != cMsg.m_appointed_hc" << endl;
            return ERR_SCHEDULE_LOOP_DEPLOYED_HC_CONFLICT;
        }
        else
        {
            cSchTaskInfo.SetAppointVNAUuid(strDeployedVnaUuid);
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* 函数名称： ScheduleProc
* 功能描述： 调度CC信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int32 CVNMSchedule::ScheduleProc(CScheduleTaskInfo &cSchTaskInfo,
                            vector <CScheduleInfo> &vecScheduleInfo,
                            vector<string> &vecScheduledResult)
{
    int32 ret = SCHEDULE_OPER_SUCCESS;    
    int32 nFirstFlag = 1;
    
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        if (itrSch != vecScheduleInfo.begin())
        {
            nFirstFlag = 0;
        }

        //对每类需求资源分别进行调度
        vector<string> vecScheduleValid;   //存放满足条件的CC或者HC列表
        ret = itrSch->GetScheduleResult(cSchTaskInfo, vecScheduleValid);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleProc: the result of  GetScheduleResult is error,ret:%d.\n",ret);
            return ret;            
        }
        
        //无满足条件资源
        if ( vecScheduleValid.empty())
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleProc: the result of  GetScheduleResult is null.\n");
            return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
        }

        //与已调度的结果求交集        
        GetScheduledNode(nFirstFlag, vecScheduledResult, vecScheduleValid);
        
        //如果没有交集，返回调度失败
        if (vecScheduledResult.empty())
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleProc: the result of  GetScheduledCC or GetScheduledHC is null.\n");
            return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* 函数名称： GetScheduledNode
* 功能描述： 汇总调度CC信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CVNMSchedule::GetScheduledNode(int32 nFirstFlag, vector<string> &vecScheduledFinal, vector<string> &vecScheduledInput)
{
    if (nFirstFlag)
    {
        vecScheduledFinal = vecScheduledInput;

        return;
    }

    GetVectorInsertion(vecScheduledFinal, vecScheduledInput);
};


//求两个vector的交集
void GetVectorInsertion(vector<string> &vecScheduledFinal, vector<string> &vecScheduledInput)
{
    //必须先排序
    vector<string> inputScheduledFinal=vecScheduledFinal;
    vector<string> tmpScheduledFinal;
    sort(inputScheduledFinal.begin(),inputScheduledFinal.end());   
    sort(vecScheduledInput.begin(),vecScheduledInput.end());   

    //求交集 
    set_intersection(inputScheduledFinal.begin(),
        inputScheduledFinal.end(),
        vecScheduledInput.begin(),
        vecScheduledInput.end(),
        back_inserter(tmpScheduledFinal));

    //顺序恢复回去
    vector<string>::iterator itrFinal = vecScheduledFinal.begin();
    for ( ; itrFinal != vecScheduledFinal.end(); )
    {
        if (find(tmpScheduledFinal.begin(), tmpScheduledFinal.end(),(string)*itrFinal) == tmpScheduledFinal.end()) 
        {
            itrFinal = vecScheduledFinal.erase(itrFinal);
        }
        else
        {
            ++itrFinal;
        }
    }
}

/************************************************************
* 函数名称： MergeScheduleResult
* 功能描述： 合并调度资源信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void MergeScheduleResult(vector<string> &vecVnaFinal, vector<CDbSchedule> &vecScheduleFinal, vector<CDbSchedule> &vecScheduleRes)
{        
    vector<CDbSchedule>::iterator itrSch = vecScheduleRes.begin();
    for ( ; itrSch != vecScheduleRes.end(); ++itrSch)
    {
        if (find(vecVnaFinal.begin(), vecVnaFinal.end(),itrSch->GetVnaUuid()) == vecVnaFinal.end())
        {
            continue;
        }
        
        vector<CDbSchedule>::iterator itrFinal = vecScheduleFinal.begin();
        for ( ; itrFinal != vecScheduleFinal.end(); ++itrFinal)
        {
            if ((itrSch->GetVnaUuid() == itrFinal->GetVnaUuid())
                && (itrSch->GetPortUuid() == itrFinal->GetPortUuid())
                && (itrSch->GetBondPhyPortUuid() == itrFinal->GetBondPhyPortUuid()))
            {
                break;
            }
        }

        if (itrFinal == vecScheduleFinal.end())
        {
            vecScheduleFinal.push_back(*itrSch);
        }
    }
};

void MergeScheduleVna(vector<string> &vecVnaFinal, vector<CDbSchedule> &vecScheduleRes)
{
    vector<string> vecNewVnaFinal;
        
    vector<CDbSchedule>::iterator itrSch = vecScheduleRes.begin();
    for ( ; itrSch != vecScheduleRes.end(); ++itrSch)
    {
        if (find(vecVnaFinal.begin(), vecVnaFinal.end(),itrSch->GetVnaUuid()) != vecVnaFinal.end())
        {
            vecNewVnaFinal.push_back(itrSch->GetVnaUuid());
        }
    }

    vecVnaFinal = vecNewVnaFinal;
};


/************************************************************
* 函数名称： CountVNAFreeInfo
* 功能描述： 汇总VNA可用资源信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CountVNAFreeInfo(vector<CVNAFreeSumInfo> &vecVNAFreeInfo, CDbSchedule &cDbSchedule)
{
    vector<CVNAFreeSumInfo>::iterator itrVNA = vecVNAFreeInfo.begin();
    for ( ; itrVNA != vecVNAFreeInfo.end(); ++itrVNA)
    {
        if (itrVNA->m_strVnaUuid == cDbSchedule.GetVnaUuid())
        {
            if (cDbSchedule.GetIsSriov())
            {
                itrVNA->m_nFreeSum += cDbSchedule.GetSwitchSriovFreeNum();
            }
            else
            {
                itrVNA->m_nFreeSum += cDbSchedule.GetSwitchFreeNum();
            }
            
            return;
        }
    }

    if ( itrVNA == vecVNAFreeInfo.end())
    {
        CVNAFreeSumInfo cVNAFreeInfo;

        if (cDbSchedule.GetIsSriov())
        {
            cVNAFreeInfo.m_nFreeSum = cDbSchedule.GetSwitchSriovFreeNum();
        }
        else
        {
            cVNAFreeInfo.m_nFreeSum = cDbSchedule.GetSwitchFreeNum();
        }
        cVNAFreeInfo.m_strVnaUuid = cDbSchedule.GetVnaUuid();
        cVNAFreeInfo.m_strCCApp = cDbSchedule.GetClusterName();
        cVNAFreeInfo.m_strHCApp = cDbSchedule.GetModuleName();

        vecVNAFreeInfo.push_back(cVNAFreeInfo);
    }
}

void VNM_DBG_SET_SCHEDULE_DBG_FLAG(int bSet)
{    
    if ((0 != bSet) && (1 != bSet))
    {
        cout << "Invalid flag parameter, please input 0 or 1." <<endl;
        return ;
    }

   g_schedule_dbg_flag = bSet;
};
DEFINE_DEBUG_FUNC(VNM_DBG_SET_SCHEDULE_DBG_FLAG);

}

