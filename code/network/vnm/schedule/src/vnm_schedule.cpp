/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_schedule.cpp
* �ļ���ʶ��
* ����ժҪ��CVNMSchedule��ʵ���ļ�
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

namespace zte_tecs
{

int g_schedule_dbg_flag = 0;
CVNMSchedule *CVNMSchedule::s_pInstance = NULL;

//��MTU����,��������Դ������������
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
* �������ƣ� GetSourceReqNum
* ���������� ���ܵ���������Դ����
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
* �������ƣ� GetScheduleResult
* ���������� ����ÿ��VNA����������Դ����
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
int32 CScheduleInfo::GetScheduleResult(CScheduleTaskInfo &cSchTaskInfo, vector<string> &vecScheduledValid)
{
    int32 ret = 0;
    vector<ScheRet> vecScheRet;

    //�����normal��Դ��������    
    if (0!=(ret=GetScheResult(cSchTaskInfo, m_vecMTUNum, vecScheRet, SCHE_NORMAL)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetScheResult m_vecMTUNum is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }
  
    //�����SR-IOV bond��Դ��������
    if (0!=(ret=GetScheResult(cSchTaskInfo, m_vecSriovBondMTUNum, vecScheRet, SCHE_BOND)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetScheResult m_vecSriovBondMTUNum is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }
    
    //�����vxlan��Դ��������
    if (0!=(ret=GetScheResult(cSchTaskInfo, m_vecVxlanMTUNum, vecScheRet, SCHE_VXLAN)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetScheResult m_vecVxlanMTUNum is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    //����ͨ�ں�SR-IOV bond��, vxlan���Ƚ������
    if (0!=(ret=GetMergeSchResult(cSchTaskInfo.GetScheduleType(), vecScheRet, vecScheduledValid)))
    {
        VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheduleResult: the result of  GetMergeSchResult is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    return SCHEDULE_OPER_SUCCESS;
}
/************************************************************
* �������ƣ� GetScheResult
* ���������� ��ѯ��������ԴDB��Ϣ
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
int32 CScheduleInfo::GetScheResult(CScheduleTaskInfo &cSchTaskInfo, 
                                       vector<CReqNumInfo> &vecMTUNum,
                                       vector<ScheRet> &vecScheRet, 
                                       int32 nQueryType)
{
    int32 ret = 0;
    
    if (!vecMTUNum.empty())
    {
        //��ѯ���ڵ���DB
        ret = QueryDbScheduleInfo(cSchTaskInfo, vecMTUNum, nQueryType);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::GetScheResult:  call QueryDbScheduleInfo failed.\n");
            return ret;            
        }
        
        //��Դ����
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
* �������ƣ� QueryDbScheduleInfo
* ���������� ��ѯ��������ԴDB��Ϣ
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

        //�Ѳ����LOOP����ָ��HC���ȣ���Ҫָ��VNA������Դ����
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

            //����Ѳ����loop�����磬ɾ���Ƕ�Ӧdvs�ļ�¼
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
            
            //������������¼��ֱ�ӷ��ص���ʧ��
            if (itr->vecSchResult.empty())
            {
                cDeployInput.DbgShow();
                VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::QueryDbScheduleInfo: the result of  ScheduleByVna is null.\n");
                return ERR_SCHEDULE_FAILED_NO_RESOURCE;
            }
        }
        else
        {
            //"HC����" ����"ָ��CC����"
            if ((cSchTaskInfo.GetScheduleType()== VNET_SCHEDULE_TYPE_HC) || ("" != cSchTaskInfo.GetAppointCCApp()))
            {
                cDeployInput.SetClusterName(cSchTaskInfo.GetAppointCCApp().c_str());

                ret = pOper->ScheduleByCluster(cDeployInput, itr->vecSchResult);
            }
            //��ͨCC����
            else
            {
                ret = pOper->Schedule(cDeployInput, itr->vecSchResult);
            } 

            if (ret != 0)
            {
                VNET_LOG(VNET_LOG_INFO, "CScheduleInfo::QueryDbScheduleInfo: call Schedule failed, ret=%d.\n", ret);
                return ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL;
            }
            //������������¼��ֱ�ӷ��ص���ʧ��
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
* �������ƣ� GetDBScheduleResult
* ���������� ���ܴ�������ԴDB���Ƚ����Ϣ
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
void CScheduleInfo::GetDBScheduleResult(int32 nScheduleType, 
                                vector<CReqNumInfo> &vecMTUNum, 
                                vector<string> &vecSchValid, 
                                vector<string> &vecVNAValid, 
                                vector<CDbSchedule> &vecScheduleFinal)
{
    vector<string> vecVnaFinal;

    //���ҳ�����������VNA,�󽻼�
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
    
    //ͳ�����������ļ�¼�������潻����vna�Ķ�Ӧ��Դ������ϼ�
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
    
    //��VNAͳ�Ƹ���
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
    
    //ɸѡ����������VNA
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
* �������ƣ� GetDVSAllocResult
* ���������� ����ÿ��VNA����������Դ����
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
int32 CScheduleInfo::GetDVSAllocResult(string &strVNAUuid, int64 nProjectID, CScheduleTaskInfo &cSchTaskInfo, 
                                                 map< string, set<int32> > &mapVnicSriov)
{
    int32 nRet = SCHEDULE_OPER_SUCCESS;
        
    vector<CDbSchedule> vecDbBond; //�Ѿ������bond��Դ

    //����vxlan����Դ����
    if(!m_vecVxlanMTUNum.empty())
    {
        nRet = QueryDbScheduleInfo(cSchTaskInfo, m_vecVxlanMTUNum, SCHE_VXLAN);
        if (0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: call QueryDbScheduleInfo  m_vecVxlanMTUNum failed.\n");
            return nRet;            
        }
        
        //��Դ����
        nRet = AllocDVSRes(strVNAUuid, nProjectID, m_vecVxlanMTUNum, SCHE_VXLAN, mapVnicSriov, vecDbBond);
        if (SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: the result of  AllocDVSRes  m_vecVxlanMTUNum  is null.\n");
            return nRet;            
        }
    }

    //��ͨ�ڵ���Դ����        
    if(!m_vecMTUNum.empty())
    {
        nRet = QueryDbScheduleInfo(cSchTaskInfo, m_vecMTUNum, SCHE_NORMAL);
        if (0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: call QueryDbScheduleInfo m_vecMTUNum failed.\n");
            return nRet;            
        }
        
        //��Դ����
        nRet = AllocDVSRes(strVNAUuid, nProjectID, m_vecMTUNum, SCHE_NORMAL, mapVnicSriov, vecDbBond);
        if (SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: the result of  AllocDVSRes is null.\n");
            return nRet;            
        }
    }
    
    //�����SR-IOV bond��Դ��������
    if (!m_vecSriovBondMTUNum.empty())
    {
        //��ѯSR-IOV bond���ڵ���DB
        nRet = QueryDbScheduleInfo(cSchTaskInfo, m_vecSriovBondMTUNum, SCHE_BOND);
        if (0 != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetDVSAllocResult: call QueryDbScheduleInfo m_vecSriovBondMTUNum failed.\n");
            return nRet;            
        }
    
        //��Դ����
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
* �������ƣ� AllocDVSRes
* ���������� ����dvs��Դ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       zj         ����
***************************************************************/
int32 CScheduleInfo::AllocDVSRes(string &strVNAUuid, int64 nProjectID, vector<CReqNumInfo> &vecMTUNum, 
                                     int32 nType, map< string, set<int32> > &mapVnicSriov, vector<CDbSchedule> &vecDbBond)
{
    int32 nRet = SCHEDULE_OPER_SUCCESS;
    
    //���ؿ��ȷ���
    if(m_nIsLoop)
    {
        nRet = GetLoopDVSRes(strVNAUuid, nProjectID, vecMTUNum, mapVnicSriov);
        if(SCHEDULE_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: call GetLoopDVSRes failed.\n");
            return nRet;
        }
    }
    
    //��sriov������Դ����
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
    //sriov������Դ����
    else
    {    
        //EDVS��BOND��Դ����
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
        
        //EDVS BOND��Դ����
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
        
        //EDVS VXLAN��Դ����        
        if (SCHE_VXLAN == nType)
        {
            //EDVS��VXLAN���Ⱥ�����ʵ��     
            return SCHEDULE_OPER_SUCCESS;
        }

        VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::AllocDVSRes: nType(%d) error.\n", nType);
        return ERR_SCHEDULE_ALLOT_SDVSRES_FAIL;      
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* �������ƣ� MergeAllocDBResult
* ���������� �������ݿ��ѯ������Ƿ�Ҫ����������?
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       zj         ����
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
    //�Ƿ���Ҫ����µļ�¼
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
    
//�ҳ�bond��Ա����MTU��ӽ�����Դ������������
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
    
//������Դ����
int32 CScheduleInfo::GetSDVSRes(vector<CReqNumInfo> &vecMTUNum)
{
    int32 nSourceNum; //ʣ����Ҫ�������������
    vector<CDbSchedule> vecDBUpdate;      //�Ѿ������������
     
    vector<CReqNumInfo>::iterator itrSch; 
    for (itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {   
        //������䣬��ǰ���������ͬDVSʱ����Ҫ���µ�ǰ����
        MergeAllocDBResult(itrSch->vecSchResult, vecDBUpdate);
        nSourceNum = itrSch->m_nSourceNum;

        //��Ҫ��MTU����ʼ���䣬��MTU��С���ȷ���
        vector<CDbSchedule>::iterator itrDBSch;     
        for(itrDBSch = itrSch->vecSchResult.begin(); itrDBSch != itrSch->vecSchResult.end(); ++itrDBSch)        
        {
            if(nSourceNum)    
            {
                if(m_nIsLoop)
                {
                    //modified by lvech:���֮ǰ�����loop��ֻ�ܲ���֮ǰ������Ľ�����
                    if((itrSch->m_strDeployedDvs != "")
                        && itrSch->m_strDeployedDvs != itrDBSch->GetSwitchUuid())
                    {
                        continue;
                    }

                    //loopֻ�ܲ���һ��������
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
                    //�������ʣ�࣬ʣ����ٷ������
                    if(itrDBSch->GetSwitchFreeNum() >= 1)
                    {
                        (*itrSet)->SetVSwitchUuid(itrDBSch->GetSwitchUuid());
                        itrDBSch->SetSwitchFreeNum(itrDBSch->GetSwitchFreeNum() - 1);
                        UpdateDBRes(vecDBUpdate, *itrDBSch); 
                        nSourceNum--;  
                        continue;
                    }
                    //��ǰ������û����Դ���䣬����һ��������
                    break; //end for itrSet
                }
            }
            else
            {
                //vnic����ɹ�
                break; //end for itrDBSch
            }
        }
            
        //��������δ������Դ
        if(nSourceNum)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetSDVSRes: The vnics(%d) still alloc sdvs resources failed.\n", nSourceNum);
            return ERR_SCHEDULE_ALLOT_SDVSRES_FAIL;
        }    
    }
    
    return SCHEDULE_OPER_SUCCESS;
}

//EDVS ��bond��Դ����
int32 CScheduleInfo::GetEDVSRes(vector<CReqNumInfo> &vecMTUNum,
                                        map< string, set<int32> > &mapVnicSriov,
                                        vector<CDbSchedule> &vecDbBond)
{
    int32 nSourceNum; //ʣ����Ҫ�������������
    vector<CDbSchedule> vecDBUpdate;  //�Ѿ������������
    
    vector<CReqNumInfo>::iterator itrSch;
    for ( itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {
        nSourceNum = itrSch->m_nSourceNum;
        //������䣬��ǰ���������ͬDVSʱ����ʣ�����ȡ��Сֵ
        MergeAllocDBResult(itrSch->vecSchResult, vecDBUpdate);
            
        vector<CDbSchedule> vecDBSch;     //�����bond����������MTU��С��������
        vector<CDbSchedule> vecDBBondSch; //����bond����������MTU��С��������
        vector<CDbSchedule>::iterator itrOldDB;
        for(itrOldDB = itrSch->vecSchResult.begin(); itrOldDB != itrSch->vecSchResult.end(); ++itrOldDB)
        {
            /* DB�з�bond��ǰ��bond�ں� */
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
    
        //����ʹ�÷�bond����
        if(!vecDBSch.empty())
        {
            vector<CDbSchedule>::iterator itrNewDB;
            for(itrNewDB = vecDBSch.begin(); itrNewDB != vecDBSch.end(); ++itrNewDB)
            {
                if(nSourceNum)
                {
                    if(m_nIsLoop)
                    {
                        //modified by lvech:���֮ǰ�����loop��ֻ�ܲ���֮ǰ������Ľ�����
                        if ((itrSch->m_strDeployedDvs != "")
                            && itrSch->m_strDeployedDvs != itrNewDB->GetSwitchUuid())
                        {
                            continue;
                        }
                    
                        //loopֻ�ܲ���һ��������
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
                            continue; //�Ѿ����䣬�����ظ����䡣
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
                        //��ǰ������û����Դ���䣬����һ��������
                        break;
                    } 
                }
            }
        }
        
        //��bond��Դռ���꣬��ʼʹ��bond��Դ
        if(!vecDBBondSch.empty())
        {
            if(nSourceNum)
            {
                set<vector<CVNetVnicMem>::iterator>::iterator itrSet = itrSch->m_setVnicIter.begin();
                for(; itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
                {
                    MergeAllocDBResult(vecDBBondSch, vecDbBond); //��ǰʣ��Ŀ�����Դ
                    if(!((*itrSet)->GetVSwitchUuid()).empty())
                    {
                       // nSourceNum--;
                        continue; 
                    }
                         
                    CDbSchedule cDbSch; //�ҳ�MTU��ӽ�����Դ������������
                    GetBondSchResult(vecDBBondSch, cDbSch);
                    if(cDbSch.GetBondPhyPortFreeNum() >= 1)  
                    {
                        (*itrSet)->SetVSwitchUuid(cDbSch.GetSwitchUuid());
                        cDbSch.SetSwitchSriovFreeNum(cDbSch.GetSwitchSriovFreeNum() - 1);
                        cDbSch.SetBondPhyPortFreeNum(cDbSch.GetBondPhyPortFreeNum() - 1);
                        cDbSch.SetSwitchFreeNum(cDbSch.GetSwitchSriovFreeNum());
                            
                        //������������Ӧ���������������¼����
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
            //û����Դ����
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetEDVSRes: The vnics(%d) still alloc edvs resources failed.\n", nSourceNum);
            return ERR_SCHEDULE_NO_EDVS_RES;
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}
                        
//EDVS bond��Դ����
int32 CScheduleInfo::GetEDVSBondRes(vector<CReqNumInfo> &vecMTUNum, 
                                        map< string, set<int32> > &mapVnicSriov,
                                        vector<CDbSchedule> &vecDbBond)
{
    vector<CDbSchedule> vecDBUpdate;  //�Ѿ������������
    
    //��bond�Ѿ������˲���bond��Դ
    vector<CDbSchedule>::iterator itBond = vecDbBond.begin();
    for(; itBond != vecDbBond.end(); ++itBond)
    {
        vecDBUpdate.push_back(*itBond);
    }

    vector<CReqNumInfo>::iterator itrSch;
    for (itrSch = vecMTUNum.begin(); itrSch != vecMTUNum.end(); ++itrSch)
    {
        MergeAllocDBResult(itrSch->vecSchResult, vecDBUpdate);

        /* ���ҽ���������:
         * 1������MTU��С��ӽ��ģ�
           2������VFʣ�����ƥ��ģ�
           3��������bond��Ч����˿�����ӽ��ģ�
           4��*/
        string strDVSUuid;
        vector<CDbSchedule>::iterator itrDB;  
        for(itrDB = itrSch->vecSchResult.begin(); itrDB != itrSch->vecSchResult.end(); ++itrDB)
        {
            if(itrDB->GetSwitchSriovFreeNum() >= itrSch->m_nSourceNum)
            {
                //��ʱֻҪVF�������㣬������bond��Ա�����ƥ��
                strDVSUuid = itrDB->GetSwitchUuid();
                break;
            }
        }
        if(strDVSUuid.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetEDVSBondRes: no switch to alloc, failed.\n");
            return ERR_SCHEDULE_NO_EDVS_BOND_RES;
        }

        //�ڶ������ҳ�����ڣ��ж�����¼
        vector<CDbSchedule> outInfo;
        for(itrDB = itrSch->vecSchResult.begin(); itrDB != itrSch->vecSchResult.end(); ++itrDB)
        {
            if(strDVSUuid == itrDB->GetSwitchUuid())
            {
                outInfo.push_back(*itrDB);
            }
        }
        
        //���������Ӷ�����¼�У�VF�������
        set<vector<CVNetVnicMem>::iterator>::iterator itrSet;
        for(itrSet = itrSch->m_setVnicIter.begin(); itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
        {
            if(!((*itrSet)->GetVSwitchUuid()).empty())
            {
                continue; //�Ѿ����䣬�����ظ����䡣
            }
                                
            CDbSchedule cMaxSch; 
            vector<CDbSchedule>::iterator itrOutInfo;
            for(itrOutInfo = outInfo.begin(); itrOutInfo != outInfo.end(); ++itrOutInfo)
            {
                //ֻҪ��һ����Դ���ɷ���
                if(itrOutInfo->GetBondPhyPortFreeNum() >= 1)
                {
                    cMaxSch = *itrOutInfo;
                    break;
                }
            }
                
            //������������Ӧ���������������¼����
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
            
            //������
            (*itrSet)->SetVSwitchUuid(cMaxSch.GetSwitchUuid());

            //��������
            cMaxSch.SetBondPhyPortFreeNum(cMaxSch.GetBondPhyPortFreeNum() - 1);
            cMaxSch.SetSwitchSriovFreeNum(cMaxSch.GetSwitchSriovFreeNum() - 1);
            cMaxSch.SetSwitchFreeNum(cMaxSch.GetSwitchSriovFreeNum());
            if(0 == cMaxSch.GetBondPhyPortFreeNum())
            {
                cMaxSch.SetBondValidPhyPortNum(cMaxSch.GetBondValidPhyPortNum() - 1);
            }
            
            //����outInfo���Ƚ�ѡ�е���Ϣ�Ƴ�
            for(itrOutInfo = outInfo.begin(); itrOutInfo != outInfo.end(); ++itrOutInfo)
            {
                if(itrOutInfo->GetBondPhyPortUuid() == cMaxSch.GetBondPhyPortUuid())
                {
                    outInfo.erase(itrOutInfo);
                    break;
                }
            }
            outInfo.push_back(cMaxSch);
            UpdateDBRes(vecDBUpdate, cMaxSch); //�����Ѿ����������
            UpdateDBRes(outInfo, cMaxSch);   //���µ�ǰbond�ѷ�������
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
        //��ѯ�Ƿ����Ѳ����LOOP��¼
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
        //û�����
        else if (vecDeployOut.empty())
        {
            continue;
        } 
        //��Ϣ�쳣������LOOP�Ѳ���ļ�¼
        else if (vecDeployOut.size() > 1)
        {
            VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: call QueryDeployByProjectLn() failed! The result num(%d) > 1\n", vecDeployOut.size());
            return ERR_SCHEDULE_LOOP_DEPLOYED_INFO_INVALID;
        }
        //�Ѳ����
        else
        {
            vector<CDbDeplySummary>::iterator pDeployInfo = vecDeployOut.begin();
            if(strVNAUuid != pDeployInfo->GetVnaUuid())
            {
                VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: VNA is not match, alloc failed.\n");
                return ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT;
            }
                
            //����������ԭ������
            set<vector<CVNetVnicMem>::iterator>::iterator itrSet = itrSch->m_setVnicIter.begin();
            for(; itrSet != itrSch->m_setVnicIter.end(); ++itrSet)
            {
                if(!((*itrSet)->GetVSwitchUuid()).empty())
                {
                    continue; //�Ѿ����䣬�����ظ����䡣
                }

                //��������Ƿ�����Դ
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
                //��Դ�����ã�����ʧ��
                if(0 == nFind)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CScheduleInfo::GetLoopDVSRes: switch(%s) has no resources.\n", 
                               itrDB->GetSwitchUuid().c_str());
                    return ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT;
                }

                //modified by lvech:��ʱ��������SetVSwitchUuid�������������GetSDVSResʱ�����ʧ��
                //���Ż�ȡ
                //(*itrSet)->SetVSwitchUuid(pDeployInfo->GetSwitchUuid());
                
                //SR-IOV����
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
��ͬ�߼�����ĵ���SR-IOV�����������󣬰�����ά�Ȼ��ܼ��ɣ�
���Ȳ���Ҫ����m_strLogicNetwork�Ƿ���ͬ������
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    int32 m_nMTU;

*/

/*
��ͬ�߼������¶��SR-IOV�������ڣ�������ά�Ȼ��ܼ��ɣ�
���Ȳ���Ҫ����MTU��VLAN������
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    string m_strLGNetwork;

���ȵĲ�ѯ�����������������:
1. ����bond��ʣ��SR-IOV��Դ������������
2. ��bond���п���SR-IOV��Դ�ĳ�Ա�ڸ����Ӹߵ�������
3. ��bond��ʣ��SR-IOV��Դ�����Ӹߵ�������
4. ��bond�ڳ�Ա��ʣ��SR-IOV��Դ�����Ӹߵ�������

������Դʱ:
��������������ͬ������ͬm_strLGNetwork�������ڸ����Ӹߵ���
��˳��������������Դ
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;

����Ĺ����ǣ�
1. �Ȱ�MTU�Ӵ�С������
2. ����ͬ�߼�������������ڸ����Ӵ�С������

���嵽ÿ��bond�ڷ�����Դ�ǣ��򰴳�Ա��ʣ����Դ�Ӹߵ��ͽ�����ѯ����


���ڳ�Ա�ڸ�����ʣ����Դ��������ͬ��bond�ڣ���Ҫ����һ����˳��
���������������bond
bond0: 114
bond1: 222
    
*/

/************************************************************
* �������ƣ� CoutSriovReqInfo
* ���������� ����SR-IOV��ش�������Դ��Ϣ
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
void CScheduleInfo::CoutSriovReqInfo()
{
    if (0 == m_nIsSriov)
    {
        return;
    }

    //SRIOV LOOPֱ�Ӹ�ֵ  
    if (m_nIsLoop)
    {
        m_vecMTUNum = m_vecTempSriovMTUNum;
        return;
    }    
	
    //SRIOV��LOOP����bond������, SR-IOV����ͬ�߼�����Ķ��loop�ڲ��ܵ��ȵ������bond SR-IOV����
    //ͬʱloop���ܰ�MTU���л���
    //ͳ��bond����loop����SRIOV��Դ����
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

    //ͳ����ͨ����SRIOV��Դ����
    vector<CReqNumInfo>::iterator itrTmpSch = m_vecTempSriovMTUNum.begin();
    for ( ; itrTmpSch != m_vecTempSriovMTUNum.end(); ++itrTmpSch)
    {
        //��MTUͳ�Ƶ�SR-IOV�ڵ�����
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
* �������ƣ� GetMergeSchResult
* ���������� ������ش�������Դ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       zj         ����
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
            cout << "nReqNum:"<< itrSche->nReqNum <<endl;                      //�������

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

    //���ܸ���MTU
    //Ҫ���ҳ�vna�����ڴ�vna�Ļ����������db��¼
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

    //��VNAͳ�Ƹ���
    vector<CVNAFreeSumInfo> vecVNAFreeInfo;
    vector<CDbSchedule>::iterator iter = vecDBSchFinal.begin();
    for ( ; iter != vecDBSchFinal.end(); ++iter)
    {
        CountVNAFreeInfo(vecVNAFreeInfo, *iter);
    }
    
    //ɸѡ����������VNA    
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
* �������ƣ� AddMTUNum
* ���������� ���ĳ���߼�������Դ�����������LOOP����
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
void CScheduleInfo::AddMTUNum(string strLGNetworkUuid, int32 nMTU, vector<CVNetVnicMem>::iterator iter, int32 nIsolateType)
{
    //SRIOV����Դ����
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
    //��ͨ����LOOP����Դ����
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
* �������ƣ� AddMTUNum
* ���������� ���ĳ��MTU������Դ�������������LOOP����
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
STATUS CVNMSchedule::StartMu(string strMsgUnitName)
{
    if (m_pMU)
    {
        return ERROR_DUPLICATED_OP;
    }

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
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
* �������ƣ� ScheduleCC
* ���������� ����CC��Ϣ
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
    
    //����û�е�������ģ�ͳһ���سɹ��������κδ���
    if (cMsg.m_VmNicList.empty() && (true != cMsg.m_WatchDogInfo.m_nIsWDGEnable))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: vecVnicCfg is NULL and m_nIsWDGEnable is false.\n");
        ret = SCHEDULE_OPER_SUCCESS;
        goto SEND_ACK;
    }

//��ʱ�����
#if 0
    //ĿǰSR-IOV��֧��accessģʽ����Ҫ���
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
    
    //ֻ��wdg����
    if (cMsg.m_VmNicList.empty())
    {
        /*���Ź�����*/    
        ret = ScheduleWdgCC(vecScheduledCC);
        if (0 != ret)
        {
            cout << "ScheduleCC: call ScheduleWdgCC failed, ret = " << ret << endl;
            goto SEND_ACK;
        }
    }
    //����������
    else
    {
        //���ܵ�����Դ������Ϣ
        vector <CScheduleInfo> vecScheduleInfo;
        map<string, int32> mapNetplaneSum;
        map<string, int32> mapLGNetworkSum;  
        ret = GetScheduleReqInfo(cMsg.m_VmNicList, vecScheduleInfo, mapNetplaneSum, mapLGNetworkSum);
        if (0 != ret)
        {
            cout << "call GetScheduleInfo failed" << endl;
            goto SEND_ACK;
        }

        //��� MAC��Դ�Ƿ�����
        ret = CheckMacRes(mapNetplaneSum);
        if (0 != ret)
        {
            cout << "call CheckMacRes failed" << endl;
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: call CheckMacRes failed, ret=%d.\n", ret);
            goto SEND_ACK;
        }

        //��� MAC��Դ�Ƿ�����
        ret = CheckIPRes(mapLGNetworkSum);
        if (0 != ret)
        {
            cout << "call CheckIPRes failed" << endl;
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleCC: call CheckIPRes failed, ret=%d.\n", ret);
            goto SEND_ACK;
        }

        //�ȼ���Ƿ���LOOP�Ѳ���������
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

        /*3.0 ����Դ���е���*/    
        ret = ScheduleProc(cSchTaskInfo, 
                vecScheduleInfo, 
                vecScheduledCC);
        if (0 != ret)
        {
            cout << "ScheduleCC: call ScheduleProc failed, ret = " << ret << endl;
            goto SEND_ACK;
        }
    }

    //���˵��ȳɹ�

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
* �������ƣ� ScheduleHC
* ���������� ����HC��Ϣ
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
    
    //����û�е�������ģ�ͳһ���سɹ��������κδ���
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

    //����Ƿ���ָ��HC����
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

    //ֻ��wdg����
    if (cMsg.m_VmNicList.empty())
    {
        /*���Ź�����*/    
        ret = ScheduleWdgHC(vecScheduledHC);
        if (0 != ret)
        {
            cout << "CVNMSchedule::ScheduleHC: call ScheduleProc ScheduleWdgHC failed, ret = " << ret << endl;
            goto SEND_ACK;
        }
    }
    //����������
    else
    {
        //���ܵ�����Դ������Ϣ
        vector <CScheduleInfo> vecScheduleInfo;
        map<string, int32> mapNetplaneSum;
        map<string, int32> mapLGNetworkSum;  
        ret = GetScheduleReqInfo(cMsg.m_VmNicList, vecScheduleInfo, mapNetplaneSum, mapLGNetworkSum);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::ScheduleHC: call GetScheduleReqInfo failed.\n");
            goto SEND_ACK;
        }

        //�ȼ���Ƿ���LOOP�Ѳ���������
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

        /*3.0 ����Դ���е���*/
        ret = ScheduleProc(cSchTaskInfo, 
                vecScheduleInfo, 
                vecScheduledHC);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleHC: call ScheduleProc failed.ret %d\n", ret);
            goto SEND_ACK;
        }
    }

    //���˵��ȳɹ�
    
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
* �������ƣ� GetNetResourceOnCC
* ���������� TCΪVMѡ������CC����VNM�������������Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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

    //��ʽת��
    CVNetVmMem cVnetVmMem;
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }

    //�Ȳ�ѯ�Ƿ����ж�Ӧ����Դ�������룬��ֹ�ظ�����
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

    // ����quantum����
    m_pLogicNetworkMgr->CreateQuantumNetwork(cVnetVmMem);

    //����VSI�ӿ�����VSI��Ϣ,����VLAN/MAC/IP����Դ
    iRet = m_pVSIProfileMgr->AllocVSIResForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call AllocVSIResForVnic failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }

    //vnic���,����VSI��Ϣ
    iRet = m_pVNetVmDB->InsertVm(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnCC: call InsertVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_ADD_VNIC_TO_DB_FAIL;
        goto RETURN_PROC;
    }

    //��ʽת��
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
* �������ƣ� FreeNetResourceOnCC
* ���������� TC��VNM����ΪVM�ͷ�������Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //��ʽת��
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }
    
    //�Ȳ�ѯ�Ƿ����ж�Ӧ����Դ
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

    //ɾ��VNM��VNIC
    iRet = m_pVNetVmDB->DeleteVm(cMsg.m_vm_id);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: call DeleteVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_DEL_VNIC_FROM_DB_FAIL;
        goto RETURN_PROC;
    }
    
    //����VSI�ӿ��ͷ�VSI��Ϣ,����VLAN/MAC/IP����Դ
    iRet = m_pVSIProfileMgr->FreeVSIResForVnic(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnCC: call FreeVSIResForVnic failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }

    // ɾ��quantum�˿�
    pPortGroupMgr->DeleteQuantumPort(cVnetVmMem);

    // ɾ��quantum����
    m_pLogicNetworkMgr->DeleteQuantumNetwork(cVnetVmMem);

RETURN_PROC:
    //����Ҫ����VM������ʧ�������������Ϣ
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
* �������ƣ� GetNetResourceOnHC
* ���������� CCΪVMѡ������HC����VNM�������PCI��Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
        
    //��ʽת��
    iRet = VnicCfgTrans2Mem(cMsg.m_vm_id,cMsg.m_project_id, cMsg.m_VmNicCfgList, cMsg.m_WatchDogInfo, cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call VnicCfgTrans2Mem failed, ret=%d.\n", iRet);
        goto RETURN_PROC;
    }
    
    //�Ȳ�ѯ�Ƿ����ж�Ӧ����Դ�������룬��ֹ�ظ�����
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

    //��ȡVNA uuid
    iRet = m_pVNAMgr->QueryVNAUUID(cMsg.m_cc_application, cMsg.m_hc_application, strVNAUuid);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call QueryVNAUUID failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_GET_VNA_UUID_FAIL;
        goto RETURN_PROC;
    }

    // RESTful�ӿڴ���port,network��vsi�л�ȡ
    pPortGroupMgr->CreateQuantumPort(cVnetVmMem);

    cVnetVmMem.SetHostName(cMsg.m_hc_application);
    //��������ģ��ӿ�ȷ��bridge���ƻ�PCI .NO
    iRet = m_pSwitchMgr->AllocDVSRes(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call AllocDVSRes failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_ALLOC_DVS_FAIL;
        goto RETURN_PROC;
    }

    //����host name
    cVnetVmMem.SetHostName(cMsg.m_hc_application);
    
    //������Ϣ��DB��,����PCI��bridge��Ϣ
    iRet = m_pVNetVmDB->UpdateVm(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::GetNetResourceOnHC: call UpdateVm failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_UPDATE_VNIC_TO_DB_FAIL;
        goto RETURN_PROC;
    }

RETURN_PROC:
    //��CC��Ӧ����Ϣ
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
        
    //���Ӧ��VNA�·����Ӹ�VM��ص�VSI��Ϣ
    if (0 == iRet)
    {
        CAddVMVSIInfoMsg cVsiMsg;
        cVsiMsg.m_cVMVSIInfo.m_nVMID = cMsg.m_vm_id;
        cVsiMsg.m_cVMVSIInfo.m_nProjectID = cMsg.m_project_id;
        cVsiMsg.m_cVMVSIInfo.m_cWatchDogInfo = cMsg.m_WatchDogInfo;

        //��Ҫȷ�������ݿ��ȡ��vnic������ͼ������Ϣ������һ��
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

        //����vmʱ�����Ӧ��vna�·�vxlan�鲥��Ϣ
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
* �������ƣ� FreeNetResourceOnHC
* ���������� CC��VNM����ΪVM�ͷ�������Դ��������Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� vmniccfg��HC��Ϣ
* ��������� pci��Ϣ
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
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
    
    //�Ȳ�ѯ�Ƿ����ж�Ӧ����Դ�������룬��ֹ�ظ�����
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
    
    //��������ģ��ӿ��ͷŶ�bridgename��PCI .NO��ռ��
    iRet = m_pSwitchMgr->FreeDVSRes(cVnetVmMem);
    if (0 != iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::FreeNetResourceOnHC: call FreeDVSRes failed, ret=%d.\n", iRet);
        iRet = ERR_VNMVNIC_FREE_DVS_FAIL;
        goto RETURN_PROC;
    }

    //���host name
    cVnetVmMem.SetHostName(cMsg.m_hc_application);
    
    //������Ϣ��DB�У���Ҫ�����VSI��PCI��bridge�����Ϣ
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

    //��CC��Ӧ����Ϣ
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
* �������ƣ� ScheduleWdgCC
* ���������� ���Ź�����CC
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

    //���û�н��������ص���ʧ��
    if (vecScheduledCC.empty())
    {
        VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleWdgCC: the result of  setScheduledCC is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    return 0;
}

/************************************************************
* �������ƣ� ScheduleWdgHC
* ���������� ���Ź�����HC
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

    //���û�н��������ص���ʧ��
    if (vecScheduledHC.empty())
    {
        VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleWdgHC: the result of  setScheduledHC is null.\n");
        return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
    }

    return 0;
}
/************************************************************
* �������ƣ� GetScheduleReqInfo
* ���������� ���ܵ���������Դ��Ϣ
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
* �������ƣ� CountMtus
* ���������� ͳ��mtu
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

        //ͳ��MTU��Դ�������
        vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
        for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
        {
            if ((itrSch->GetNetplane() == cPortGroup.GetNetplaneUuid())
                && (itrSch->GetIsLoop() == itrVnic->m_nIsLoop)
                && (itrSch->GetIsSriov() == itrVnic->m_nIsSriov)
                && (itrSch->GetIsSdn() == cPortGroup.GetIsSdn()))
            {
                //loop����Դ��Ҫ��LogicNetworkID����ͳ�ƣ����ڲ�ͬLogicNetworkID��MTU������ͬ
                //SR-IOV����Դ��Ҫ��LogicNetworkID����ͳ�ƣ�����һ��LogicNetworkID�������������Դ������Ҫ��
                if (itrVnic->m_nIsLoop || itrVnic->m_nIsSriov)
                {
                    itrSch->AddMTUNum(itrVnic->m_strLogicNetworkID,cPortGroup.GetMtu(),(vector<CVNetVnicMem>::iterator)NULL,cPortGroup.GetIsolateType());
                }
                //��loop����Դ��ֻ��Ҫ��MTU����ͳ�Ƽ���
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

    //SRIOV��Դ��Ҫ���ж��λ���
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        itrSch->CoutSriovReqInfo();
    }

    //����
    vector<CScheduleInfo>::iterator itrSchInfo = vecScheduleInfo.begin();
    for( ; itrSchInfo != vecScheduleInfo.end(); ++itrSchInfo)
    {
        if(!itrSchInfo->m_vecMTUNum.empty())
        {
            //��MTU����Դ������
            sort(itrSchInfo->m_vecMTUNum.begin(), itrSchInfo->m_vecMTUNum.end(), SortCompareMTUResNum);
        }
        
        if(!itrSchInfo->m_vecSriovBondMTUNum.empty())
        {
            //��MTU����Դ������
            sort(itrSchInfo->m_vecSriovBondMTUNum.begin(), itrSchInfo->m_vecSriovBondMTUNum.end(), SortCompareMTUResNum);
        }

        if(!itrSchInfo->m_vecVxlanMTUNum.empty())
        {
            //��MTU����Դ������
            sort(itrSchInfo->m_vecVxlanMTUNum.begin(), itrSchInfo->m_vecVxlanMTUNum.end(), SortCompareMTUResNum);
        }
    }    
    
    if (1 == g_schedule_dbg_flag)
    {
        //������Ϣ
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
* �������ƣ� CountMtus
* ���������� ͳ��mtu
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       zj         ����
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

        //ͳ��MTU��Դ�������
        vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
        for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
        {
            if ((itrSch->GetNetplane() == cPortGroup.GetNetplaneUuid())
                && (itrSch->GetIsLoop() == itrVnic->GetIsLoop())
                && (itrSch->GetIsSriov() == itrVnic->GetIsSriov())
                && (itrSch->GetIsSdn() == cPortGroup.GetIsSdn()))
            {
                //loop����Դ��Ҫ��LogicNetworkID����ͳ�ƣ����ڲ�ͬLogicNetworkID��MTU������ͬ
                //SR-IOV����Դ��Ҫ��LogicNetworkID����ͳ�ƣ�����һ��LogicNetworkID�������������Դ������Ҫ��
                if (itrVnic->GetIsLoop() || itrVnic->GetIsSriov())
                {
                    itrSch->AddMTUNum(itrVnic->GetLogicNetworkUuid(),cPortGroup.GetMtu(),itrVnic,cPortGroup.GetIsolateType());
                }
                //��loop����Դ��ֻ��Ҫ��MTU����ͳ�Ƽ���
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

    //SRIOV��Դ��Ҫ���ж��λ���
    vector<CScheduleInfo>::iterator itrSch = vecScheduleInfo.begin();
    for ( ; itrSch != vecScheduleInfo.end(); ++itrSch)
    {
        itrSch->CoutSriovReqInfo();
    }

    //����
    vector<CScheduleInfo>::iterator itrSchInfo = vecScheduleInfo.begin();
    for( ; itrSchInfo != vecScheduleInfo.end(); ++itrSchInfo)
    {
        if(!itrSchInfo->m_vecMTUNum.empty())
        {
            //��MTU����Դ������
            sort(itrSchInfo->m_vecMTUNum.begin(), itrSchInfo->m_vecMTUNum.end(), SortCompareMTUResNum);
        }
        
        if(!itrSchInfo->m_vecSriovBondMTUNum.empty())
        {
            //��MTU����Դ������
            sort(itrSchInfo->m_vecSriovBondMTUNum.begin(), itrSchInfo->m_vecSriovBondMTUNum.end(), SortCompareMTUResNum);
        }

        if(!itrSchInfo->m_vecVxlanMTUNum.empty())
        {
            //��MTU����Դ������
            sort(itrSchInfo->m_vecVxlanMTUNum.begin(), itrSchInfo->m_vecVxlanMTUNum.end(), SortCompareMTUResNum);
        }
    }    
    
    if (1 == g_schedule_dbg_flag)
    {
        //������Ϣ
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
* �������ƣ� CountNetplanes
* ���������� ͳ������ƽ�����
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
int32 CVNMSchedule::CountNetplanes(vector <CVNetVnicConfig> &vecVnicCfg, map<string, int32> &mapNetplaneSum)
{
    int32 iRet = SCHEDULE_OPER_SUCCESS;
    
    vector<CVNetVnicConfig>::iterator itrVnic = vecVnicCfg.begin();
    for ( ; itrVnic != vecVnicCfg.end(); ++itrVnic)
    {
        //ָ��MAC��ַ�Ĳ���Ҫ����ͳ��
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
        
        //ͳ��NETPLANE�����Դ�������, ��MAC��ַ����Դ����
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
* �������ƣ� CountLogicNetworks
* ���������� ͳ���߼��������
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
int32 CVNMSchedule::CountLogicNetworks(vector <CVNetVnicConfig> &vecVnicCfg, map<string, int32> &mapLGNetworkSum)
{
    vector<CVNetVnicConfig>::iterator itrVnic = vecVnicCfg.begin();
    for ( ; itrVnic != vecVnicCfg.end(); ++itrVnic)
    {
        //ָ��IP��ַ�Ĳ���Ҫ����ͳ��
        if (0 != itrVnic->m_strIP.size())
        {
            continue;
        }

        //ͳ��LG_NETWORK�����Դ�������, ��IP��ַ����Դ����
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
* �������ƣ� GetPgByLogicNetwork
* ���������� ͨ���߼������ȡpg
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       �Խ�         ����
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
* �������ƣ� CheckMacRes
* ���������� ����Ƿ�MAC��Դ����
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
* �������ƣ� CheckIPRes
* ���������� ����Ƿ�IP��Դ����
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
* �������ƣ� CheckLoopDeploy
* ���������� ����Ƿ���LOOP�Ѳ���������
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
                //��ѯ�Ƿ����Ѳ����LOOP��¼
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
                //û�����
                else if (vecDeployOut.empty())
                {
                    continue;
                }
                //��Ϣ�쳣������LOOP�Ѳ���ļ�¼
                else if (vecDeployOut.size() > 1)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNMSchedule::CheckLoopDeploy: call QueryDeployByProjectLn() failed!The result num(%d) > 1\n", vecDeployOut.size());
                    return ERR_SCHEDULE_LOOP_DEPLOYED_INFO_INVALID;
                }
                //�Ѳ����
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

                    //���ø�logicnetwork��Ӧ�Ѳ������dvs_uuid
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
* �������ƣ� ScheduleProc
* ���������� ����CC��Ϣ
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

        //��ÿ��������Դ�ֱ���е���
        vector<string> vecScheduleValid;   //�������������CC����HC�б�
        ret = itrSch->GetScheduleResult(cSchTaskInfo, vecScheduleValid);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleProc: the result of  GetScheduleResult is error,ret:%d.\n",ret);
            return ret;            
        }
        
        //������������Դ
        if ( vecScheduleValid.empty())
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleProc: the result of  GetScheduleResult is null.\n");
            return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
        }

        //���ѵ��ȵĽ���󽻼�        
        GetScheduledNode(nFirstFlag, vecScheduledResult, vecScheduleValid);
        
        //���û�н��������ص���ʧ��
        if (vecScheduledResult.empty())
        {
            VNET_LOG(VNET_LOG_INFO, "CVNMSchedule::ScheduleProc: the result of  GetScheduledCC or GetScheduledHC is null.\n");
            return ERR_SCHEDULE_FAILED_NO_RESOURCE;            
        }
    }

    return SCHEDULE_OPER_SUCCESS;
}

/************************************************************
* �������ƣ� GetScheduledNode
* ���������� ���ܵ���CC��Ϣ
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
void CVNMSchedule::GetScheduledNode(int32 nFirstFlag, vector<string> &vecScheduledFinal, vector<string> &vecScheduledInput)
{
    if (nFirstFlag)
    {
        vecScheduledFinal = vecScheduledInput;

        return;
    }

    GetVectorInsertion(vecScheduledFinal, vecScheduledInput);
};


//������vector�Ľ���
void GetVectorInsertion(vector<string> &vecScheduledFinal, vector<string> &vecScheduledInput)
{
    //����������
    vector<string> inputScheduledFinal=vecScheduledFinal;
    vector<string> tmpScheduledFinal;
    sort(inputScheduledFinal.begin(),inputScheduledFinal.end());   
    sort(vecScheduledInput.begin(),vecScheduledInput.end());   

    //�󽻼� 
    set_intersection(inputScheduledFinal.begin(),
        inputScheduledFinal.end(),
        vecScheduledInput.begin(),
        vecScheduledInput.end(),
        back_inserter(tmpScheduledFinal));

    //˳��ָ���ȥ
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
* �������ƣ� MergeScheduleResult
* ���������� �ϲ�������Դ��Ϣ
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
* �������ƣ� CountVNAFreeInfo
* ���������� ����VNA������Դ��Ϣ
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

