

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_schedule.h"
    
namespace zte_tecs
{
typedef enum tagScheduleProc
{
    EN_SCHEDULE_PROC_CHECK_MAC = 0,
    EN_SCHEDULE_PROC_CHECK_IP,
    EN_SCHEDULE_PROC,
    EN_SCHEDULE_PROC_BY_CLUSTER,
    EN_SCHEDULE_PROC_BY_VNA,    
    EN_SCHEDULE_WDG_PROC,
    EN_SCHEDULE_WDG_PROC_BY_CLUSTER,
    EN_SCHEDULE_WDG_PROC_BY_VNA,  
    EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT_LN, 
    EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT,  
    EN_SCHEDULE_PROC_ALL,
}EN_SCHEDULE_PROC_;

const char *s_astrScheduleProc[EN_SCHEDULE_PROC_ALL] = 
{        
    "pf_net_check_schedule_resource_mac",
    "pf_net_check_schedule_resource_ip",
    "pf_net_schedule",  
    "pf_net_schedule_by_cluster", 
    "pf_net_schedule_by_vna",   
    "pf_net_schedule_wdg",  
    "pf_net_schedule_wdg_by_cluster", 
    "pf_net_schedule_wdg_by_vna", 
    "pf_net_schedule_query_deploy_by_project_ln", 
    "pf_net_schedule_query_deploy_by_project",  
};

    
CDBOperateSchedule::CDBOperateSchedule(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateSchedule::~CDBOperateSchedule()
{
    
}

int32 CDBOperateSchedule::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_SCHEDULE_PROC:
        case EN_SCHEDULE_PROC_BY_CLUSTER:
        case EN_SCHEDULE_PROC_BY_VNA:
        {
            return ScheduleCallback(prs,nil);
        }
        break;
        case EN_SCHEDULE_WDG_PROC:
        case EN_SCHEDULE_WDG_PROC_BY_CLUSTER:
        case EN_SCHEDULE_WDG_PROC_BY_VNA:
        {
            return ScheduleWdgCallback(prs,nil);
        }
        break;
        case EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT:
        case EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT_LN:
        {
            return ScheduleQueryDeployCallback(prs,nil);
        }
        break;
        case EN_SCHEDULE_PROC_CHECK_MAC:
        case EN_SCHEDULE_PROC_CHECK_IP:
        {
            return CheckOperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
int32 CDBOperateSchedule::CheckMacResource(const char* netplane_uuid, int32 nic_num)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::CheckMacResource IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inNetplaneUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)netplane_uuid);
    CADOParameter inNicNum(CADOParameter::paramInt, CADOParameter::paramInput, nic_num);
    
    vInParam.push_back(inNetplaneUuid);
    vInParam.push_back(inNicNum);

    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrScheduleProc[EN_SCHEDULE_PROC_CHECK_MAC],
        vInParam, 
        this,
        EN_SCHEDULE_PROC_CHECK_MAC,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {        
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::CheckMacResource(%s,%d) IProc->Handle failed(%d).\n",
                netplane_uuid, nic_num, ret);
        }
        return ret;
    }
    
    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::CheckMacResource(%s,%d) proc return failed. ret : %d.\n", 
            netplane_uuid, nic_num, GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}
int32 CDBOperateSchedule::CheckIpResource(const char* lgnetwork_uuid, int32 nic_num)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::CheckIpResource IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inLgNetworkUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)lgnetwork_uuid);
    CADOParameter inNicNum(CADOParameter::paramInt, CADOParameter::paramInput, nic_num);
    
    vInParam.push_back(inLgNetworkUuid);
    vInParam.push_back(inNicNum);

    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrScheduleProc[EN_SCHEDULE_PROC_CHECK_IP],
        vInParam, 
        this,
        EN_SCHEDULE_PROC_CHECK_IP,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::CheckIpResource(%s,%d) IProc->Handle failed(%d).\n",
            lgnetwork_uuid,nic_num,ret);
        return ret;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::CheckIpResource(%s,%d) proc return failed. ret : %d.\n",
            lgnetwork_uuid,nic_num,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    
    return 0;
}


int32 CDBOperateSchedule::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

int32 CDBOperateSchedule::Schedule(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo)
{
    int32 nicNum = inInfo.GetNicNum();
    // edvs, is_bond = 0, is_loop = 0 需既能调度非BOND的EDVS，也能调度到BOND的EDVS
    if(inInfo.GetIsSriov() && 
      (0 == inInfo.GetIsBondNic()) && 
      (0 == inInfo.GetIsLoop()))
    {
        vector<CDbSchedule> vTmp;
        inInfo.SetIsBondNic(0);
        int32 ret = ScheduleProc(inInfo, outInfo);
        if( ret != VNET_DB_SUCCESS )
        {
            return ret;
        }
        
        inInfo.SetIsBondNic(1);
		// 在此情况下，SetNicNum 必须设置为1
		inInfo.SetNicNum(1);
        ret = ScheduleProc(inInfo, vTmp);
        if( ret != VNET_DB_SUCCESS )
        {
            return ret;
        }

        outInfo.insert(outInfo.end(),vTmp.begin(),vTmp.end()); 

        // 还原IsBondNic
        inInfo.SetIsBondNic(0);
		inInfo.SetNicNum(nicNum);
        return VNET_DB_SUCCESS;
    }    
    
    return ScheduleProc(inInfo, outInfo);
}
int32 CDBOperateSchedule::ScheduleByCluster(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo)
{
    // edvs, is_bond = 0, is_loop = 0 需既能调度非BOND的EDVS，也能调度到BOND的EDVS
	int32 nicNum = inInfo.GetNicNum();
    if(inInfo.GetIsSriov() && 
      (0 == inInfo.GetIsBondNic()) && 
      (0 == inInfo.GetIsLoop()))
    {
        vector<CDbSchedule> vTmp;
        inInfo.SetIsBondNic(0);
        int32 ret = ScheduleByClusterProc(inInfo, outInfo);
        if( ret != VNET_DB_SUCCESS )
        {
            return ret;
        }
        
        inInfo.SetIsBondNic(1);
		// 在此情况下，SetNicNum 必须设置为1
		inInfo.SetNicNum(1);
        ret = ScheduleByClusterProc(inInfo, vTmp);
        if( ret != VNET_DB_SUCCESS )
        {
            return ret;
        }

        outInfo.insert(outInfo.end(),vTmp.begin(),vTmp.end()); 

        // 还原IsBondNic
        inInfo.SetIsBondNic(0);
		inInfo.SetNicNum(nicNum);
        return VNET_DB_SUCCESS;
    }    
    
    return ScheduleByClusterProc(inInfo, outInfo);
}
int32 CDBOperateSchedule::ScheduleByVna(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo)
{
    // edvs, is_bond = 0, is_loop = 0 需既能调度非BOND的EDVS，也能调度到BOND的EDVS
	int32 nicNum = inInfo.GetNicNum();
    if(inInfo.GetIsSriov() && 
      (0 == inInfo.GetIsBondNic()) && 
      (0 == inInfo.GetIsLoop()))
    {
        vector<CDbSchedule> vTmp;
        inInfo.SetIsBondNic(0);
        int32 ret = ScheduleByVnaProc(inInfo, outInfo);
        if( ret != VNET_DB_SUCCESS )
        {
            return ret;
        }
        
        inInfo.SetIsBondNic(1);
		// 在此情况下，SetNicNum 必须设置为1
		inInfo.SetNicNum(1);
        ret = ScheduleByVnaProc(inInfo, vTmp);
        if( ret != VNET_DB_SUCCESS )
        {
            return ret;
        }

        outInfo.insert(outInfo.end(),vTmp.begin(),vTmp.end()); 

        // 还原IsBondNic
        inInfo.SetIsBondNic(0);
		inInfo.SetNicNum(nicNum);
        return VNET_DB_SUCCESS;
    }    
    
    return ScheduleByVnaProc(inInfo, outInfo);
}

int32 CDBOperateSchedule::ScheduleProc(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleProc IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inNetplaneUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)inInfo.GetNetplaneUuid().c_str());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsLoop());
    CADOParameter inNicNum(CADOParameter::paramInt , CADOParameter::paramInput, inInfo.GetNicNum());
    CADOParameter inIsBondNic(CADOParameter::paramInt , CADOParameter::paramInput, inInfo.GetIsBondNic());
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetMtu());
    CADOParameter inIsHasWatchdog(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsHasWatchdog());
    CADOParameter inIsSdn(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSdn());
    CADOParameter inIsVxlan(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsVxlan());    
    
    vInParam.push_back(inNetplaneUuid);
    vInParam.push_back(inIsSriov);
    vInParam.push_back(inIsLoop);
    vInParam.push_back(inNicNum);
    vInParam.push_back(inIsBondNic);    
    vInParam.push_back(inMtu);
    vInParam.push_back(inIsHasWatchdog);   
    vInParam.push_back(inIsSdn); 
    vInParam.push_back(inIsVxlan); 
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_PROC],
        vInParam, 
        this,
        EN_SCHEDULE_PROC,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            inInfo.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleProc IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateSchedule::ScheduleByClusterProc(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleByClusterProc IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if(0 == STRCMP(inInfo.GetClusterName().c_str(), "") )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleByClusterProc cluster_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
   
    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inNetplaneUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)inInfo.GetNetplaneUuid().c_str());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsLoop());
    CADOParameter inNicNum(CADOParameter::paramInt , CADOParameter::paramInput, inInfo.GetNicNum());
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetMtu());
    CADOParameter inIsBondNic(CADOParameter::paramInt , CADOParameter::paramInput, inInfo.GetIsBondNic());
    CADOParameter inClusterName(CADOParameter::paramText, CADOParameter::paramInput, (char*)inInfo.GetClusterName().c_str());
    CADOParameter inIsHasWatchdog(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsHasWatchdog());
    CADOParameter inIsSdn(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSdn());
    CADOParameter inIsVxlan(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsVxlan());    
    
    vInParam.push_back(inNetplaneUuid);
    vInParam.push_back(inIsSriov);
    vInParam.push_back(inIsLoop);
    vInParam.push_back(inNicNum);
    vInParam.push_back(inIsBondNic);    
    vInParam.push_back(inMtu);
    vInParam.push_back(inClusterName); 
    vInParam.push_back(inIsHasWatchdog);    
    vInParam.push_back(inIsSdn); 
    vInParam.push_back(inIsVxlan); 
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_PROC_BY_CLUSTER],
        vInParam, 
        this,
        EN_SCHEDULE_PROC_BY_CLUSTER,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            inInfo.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleByClusterProc IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateSchedule::ScheduleByVnaProc(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleByVnaProc IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if(0 == STRCMP(inInfo.GetVnaUuid().c_str(), "") )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleByVnaProc vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
   
    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inNetplaneUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)inInfo.GetNetplaneUuid().c_str());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsLoop());
    CADOParameter inNicNum(CADOParameter::paramInt , CADOParameter::paramInput, inInfo.GetNicNum());
    CADOParameter inIsBondNic(CADOParameter::paramInt , CADOParameter::paramInput, inInfo.GetIsBondNic());
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetMtu());
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)inInfo.GetVnaUuid().c_str());
    CADOParameter inIsHasWatchdog(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsHasWatchdog());
    CADOParameter inIsSdn(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSdn());
    CADOParameter inIsVxlan(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsVxlan());    
    
    vInParam.push_back(inNetplaneUuid);
    vInParam.push_back(inIsSriov);
    vInParam.push_back(inIsLoop);
    vInParam.push_back(inNicNum);
    vInParam.push_back(inIsBondNic);    
    vInParam.push_back(inMtu);
    vInParam.push_back(inVnaUuid); 
    vInParam.push_back(inIsHasWatchdog);
    vInParam.push_back(inIsSdn); 
    vInParam.push_back(inIsVxlan);     
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_PROC_BY_VNA],
        vInParam, 
        this,
        EN_SCHEDULE_PROC_BY_VNA,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            inInfo.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleByVnaProc IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateSchedule::ScheduleCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbSchedule> *pvOut;
    pvOut = static_cast<vector<CDbSchedule>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbSchedule info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 22);

    info.SetNetplaneUuid( prs->GetFieldValue(0) );

    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsSriov(value);

    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsLoop(value);

    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetNicNum(value);

    if( false == prs->GetFieldValue(4,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsBondNic(value);

    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMtu(value);

    if( false == prs->GetFieldValue(6,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsHasWatchdog(value);

    if( false == prs->GetFieldValue(7,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsSdn(value);

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsVxlan(value);

    
    info.SetClusterName( prs->GetFieldValue(9) );
    info.SetVnaUuid( prs->GetFieldValue(10) );
    info.SetModuleName( prs->GetFieldValue(11) );
    info.SetSwitchUuid( prs->GetFieldValue(12) );

    if( false == prs->GetFieldValue(13,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 11 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchSriovFreeNum(value);

    if( false == prs->GetFieldValue(14,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 12 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchFreeNum(value);

    if( false == prs->GetFieldValue(15,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 13 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetUplinkPgMtu(value);

    info.SetPortUuid( prs->GetFieldValue(16) );

    if( false == prs->GetFieldValue(17,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 15 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(value);

    if( false == prs->GetFieldValue(18,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 16 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondTotalPhyPortNum(value);

    if( false == prs->GetFieldValue(19,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 17 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondValidPhyPortNum(value);

    info.SetBondPhyPortUuid( prs->GetFieldValue(20) );

    if( false == prs->GetFieldValue(21,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleCallback GetFieldValue 19 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondPhyPortFreeNum(value);
   
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateSchedule::ScheduleWdg(vector<CDbScheduleWdg> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleWdg IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
       
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_WDG_PROC],
        vInParam, 
        this,
        EN_SCHEDULE_WDG_PROC,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {       
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleWdg IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}
int32 CDBOperateSchedule::ScheduleWdgByCluster(const string& strClusterName,vector<CDbScheduleWdg> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleWdgByCluster IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inClusterName(CADOParameter::paramText, CADOParameter::paramInput, (char*)strClusterName.c_str());
    
    vInParam.push_back(inClusterName);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_WDG_PROC_BY_CLUSTER],
        vInParam, 
        this,
        EN_SCHEDULE_WDG_PROC_BY_CLUSTER,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleWdgByCluster(%s) IProc->Handle failed(%d).\n",
            strClusterName.c_str(),ret);
        return ret;
    }
    
    return 0;
}
int32 CDBOperateSchedule::ScheduleWdgByVna(const string& strVnaUuid,vector<CDbScheduleWdg> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleWdgByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVnaName(CADOParameter::paramText, CADOParameter::paramInput, (char*)strVnaUuid.c_str());
    
    vInParam.push_back(inVnaName);
       
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_WDG_PROC_BY_VNA],
        vInParam, 
        this,
        EN_SCHEDULE_WDG_PROC_BY_VNA,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleWdgByVna(%s) IProc->Handle failed(%d).\n",
            strVnaUuid.c_str(), ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateSchedule::ScheduleWdgCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleWdgCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbScheduleWdg> *pvOut;
    pvOut = static_cast<vector<CDbScheduleWdg>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleWdgCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbScheduleWdg info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);

    info.SetVnaUuid( prs->GetFieldValue(0) );
    info.SetModuleName( prs->GetFieldValue(1) );
    info.SetClusterName( prs->GetFieldValue(2) );
   
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateSchedule::QueryDeployByProjectLn(CDbDeplySummary & inInfo, vector<CDbDeplySummary> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::QueryDeployByProjectLn IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
   
    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, inInfo.GetProjectId());
    CADOParameter inLgNetworkUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)inInfo.GetLgNetworkUuid().c_str());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsLoop());
    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inLgNetworkUuid);    
    vInParam.push_back(inIsSriov);
    vInParam.push_back(inIsLoop);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT_LN],
        vInParam, 
        this,
        EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT_LN,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            inInfo.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::QueryDeployByProjectLn IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}


int32 CDBOperateSchedule::QueryDeployByProject(CDbDeplySummary & inInfo, vector<CDbDeplySummary> &outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::QueryDeployByProject IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
   
    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, inInfo.GetProjectId());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, inInfo.GetIsLoop());
    
    vInParam.push_back(inProjectId);   
    vInParam.push_back(inIsSriov);
    vInParam.push_back(inIsLoop);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrScheduleProc[EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT],
        vInParam, 
        this,
        EN_SCHEDULE_PROC_QUERY_DEPLOY_BY_PROJECT,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            inInfo.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::QueryDeployByProject IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}


int32 CDBOperateSchedule::ScheduleQueryDeployCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleQueryDeployCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbDeplySummary> *pvOut;
    pvOut = static_cast<vector<CDbDeplySummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSchedule::ScheduleQueryDeployCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbDeplySummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 9);

    int64 value64 = 0;
    if( false == prs->GetFieldValue(0,value64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleQueryDeployCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetProjectId(value64);

    info.SetLgNetworkUuid( prs->GetFieldValue(1) );
    
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleQueryDeployCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsSriov(value);

    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSchedule::ScheduleQueryDeployCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsLoop(value);
    
    info.SetClusterName( prs->GetFieldValue(4) );
    info.SetVnaUuid( prs->GetFieldValue(5) );
    info.SetModuleName( prs->GetFieldValue(6) );
    info.SetSwitchUuid( prs->GetFieldValue(7) );

    info.SetPortUuid( prs->GetFieldValue(8) );
   
    (*pvOut).push_back(info);

    return 0;
}

void CDBOperateSchedule::DbgShow()
{
}

/* test code ------------------------------------------------------------

*/
void db_schedule_check_mac(char* netplane_uuid,  int32 nic_num)
{
    if( NULL == netplane_uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckMacResource(netplane_uuid,nic_num);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule CheckMacResource failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule CheckMacResource success." << endl;    
}
DEFINE_DEBUG_FUNC(db_schedule_check_mac);

void db_schedule_check_ip(char* lgnetwork_uuid,  int32 nic_num)
{
    if( NULL == lgnetwork_uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckIpResource(lgnetwork_uuid,nic_num);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule CheckIpResource failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule CheckIpResource success." << endl;
    
}
DEFINE_DEBUG_FUNC(db_schedule_check_ip);

/*void db_schedule(char* netplane_uuid, int32 is_sriov, int32 is_loop, int32 nic_num, int32 mtu,
     int32 is_bond, int32 is_watchdog)*/
void db_schedule(char* netplane_uuid, int32 is_sriov, int32 is_loop, int32 nic_num, int32 mtu,
     int32 is_bond, int32 is_vxlan, int32 is_sdn)     
{
    if( NULL == netplane_uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbSchedule> vOut;
    CDbSchedule info;
    info.SetNetplaneUuid(netplane_uuid);  
    info.SetIsSriov(is_sriov);  
    info.SetIsLoop(is_loop);  
    info.SetIsBondNic(is_bond);
    info.SetNicNum(nic_num);  
    info.SetMtu(mtu);     
//    info.SetIsHasWatchdog(is_watchdog);
    info.SetIsSdn(is_sdn);
    info.SetIsVxlan(is_vxlan);    
    int ret = pOper->Schedule(info,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule Schedule failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule Schedule success." << endl;

    vector<CDbSchedule>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_schedule);


void db_schedule_by_cluster(char* netplane_uuid, int32 is_sriov, int32 is_loop, int32 nic_num, int32 mtu,
     char* cluster_name,int32 is_bond , int32 is_watchdog)
{
    if( NULL == netplane_uuid || NULL == cluster_name)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbSchedule> vOut;
    CDbSchedule info;
    info.SetNetplaneUuid(netplane_uuid);  
    info.SetIsSriov(is_sriov);  
    info.SetIsLoop(is_loop);  
    info.SetNicNum(nic_num);  
    info.SetIsBondNic(is_bond);
    info.SetMtu(mtu);     
    info.SetClusterName(cluster_name); 
    info.SetIsHasWatchdog(is_watchdog);
    int ret = pOper->ScheduleByCluster(info,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule ScheduleByCluster failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule ScheduleByCluster success." << endl;

    vector<CDbSchedule>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_schedule_by_cluster);


void db_schedule_by_vna(char* netplane_uuid, int32 is_sriov, int32 is_loop, int32 nic_num,
    int32 mtu, char* vna_uuid,int32 is_bond, int32 is_watchdog)
{
    if( NULL == netplane_uuid || NULL == vna_uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbSchedule> vOut;
    CDbSchedule info;
    info.SetNetplaneUuid(netplane_uuid);  
    info.SetIsSriov(is_sriov);  
    info.SetIsLoop(is_loop);  
    info.SetNicNum(nic_num);  
    info.SetIsBondNic(is_bond);
    info.SetMtu(mtu);     
    info.SetVnaUuid(vna_uuid);     
    info.SetIsHasWatchdog(is_watchdog);
    int ret = pOper->ScheduleByVna(info,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule ScheduleByVna failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule ScheduleByVna success." << endl;

    vector<CDbSchedule>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_schedule_by_vna);



void db_schedule_query_deploy_by_project(int64 project_id, int32 is_sriov, int32 is_loop)
{
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbDeplySummary> vOut;
    CDbDeplySummary info;
    info.SetProjectId(project_id);  
    info.SetIsSriov(is_sriov);  
    info.SetIsLoop(is_loop);     
    int ret = pOper->QueryDeployByProject(info,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule QueryDeployByProject failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule QueryDeployByProject success." << endl;

    vector<CDbDeplySummary>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_schedule_query_deploy_by_project);

void db_schedule_query_deploy_by_project_ln(int64 project_id, char* ln, int32 is_sriov, int32 is_loop)
{
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    if( NULL == ln)
    {
        cout << "ln is NULL" << endl;
        return;
    }

    vector<CDbDeplySummary> vOut;
    CDbDeplySummary info;
    info.SetProjectId(project_id); 
    info.SetLgNetworkUuid(ln); 
    info.SetIsSriov(is_sriov);  
    info.SetIsLoop(is_loop);     
    int ret = pOper->QueryDeployByProjectLn(info,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule QueryDeployByProject failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule QueryDeployByProject success." << endl;

    vector<CDbDeplySummary>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_schedule_query_deploy_by_project_ln);


void db_schedule_wdg()
{
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbScheduleWdg> vOut;
    int ret = pOper->ScheduleWdg(vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule ScheduleWdg failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule ScheduleWdg success." << endl;

    vector<CDbScheduleWdg>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_schedule_wdg);

void db_schedule_wdg_by_cluster(char* clustername)
{
    if( NULL == clustername)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbScheduleWdg> vOut;
    string str(clustername);
    int ret = pOper->ScheduleWdgByCluster(str,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule ScheduleWdg failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule ScheduleWdg success." << endl;

    vector<CDbScheduleWdg>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_schedule_wdg_by_cluster);

void db_schedule_wdg_by_vna(char* vna_uuid)
{
    if( NULL == vna_uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateSchedule * pOper = GetDbISchedule();
    if( NULL == pOper )
    {
        cout << "CDBOperateSchedule is NULL" << endl;
        return ;
    }

    vector<CDbScheduleWdg> vOut;
    string str(vna_uuid);
    int ret = pOper->ScheduleWdgByVna(str,vOut);
    if( ret != 0 )
    {
        cout << "CDBOperateSchedule ScheduleWdg failed." << ret << endl;
        return;
    }
    cout << "CDBOperateSchedule ScheduleWdg success." << endl;

    vector<CDbScheduleWdg>::iterator it = vOut.begin();
    for(; it != vOut.end(); ++it)
    {
        cout << "--------------------"<< endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_schedule_wdg_by_vna);


}




