

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_wildcast_task_switch.h"
    
namespace zte_tecs
{
typedef enum tagWildcastTaskSwitchProc
{
    EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY = 0,
    EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY_BY_VNA,
    EN_WILDCAST_TASK_SWITCH_PROC_QUERY,   
    EN_WILDCAST_TASK_SWITCH_PROC_CHECK_ADD,
    EN_WILDCAST_TASK_SWITCH_PROC_ADD,
    EN_WILDCAST_TASK_SWITCH_PROC_CHECK_MODIFY,
    EN_WILDCAST_TASK_SWITCH_PROC_MODIFY,
    EN_WILDCAST_TASK_SWITCH_PROC_CHECK_DEL,
    EN_WILDCAST_TASK_SWITCH_PROC_DEL,
    EN_WILDCAST_TASK_SWITCH_PROC_ALL,
}EN_WILDCAST_TASK_SWITCH_PROC;

const char *s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_ALL] = 
{        
    "pf_net_query_wildcast_task_switch_summary",
    "pf_net_query_wildcast_task_switch_summary_by_vna",
    "pf_net_query_wildcast_task_switch",
    "pf_net_check_add_wildcast_task_switch",  
    "pf_net_add_wildcast_task_switch",    
    "pf_net_check_modify_wildcast_task_switch", 
    "pf_net_modify_wildcast_task_switch", 
    "pf_net_check_del_wildcast_task_switch",  
    "pf_net_del_wildcast_task_switch",  
};

    
CDBOperateWildcastTaskSwitch::CDBOperateWildcastTaskSwitch(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateWildcastTaskSwitch::~CDBOperateWildcastTaskSwitch()
{
    
}

int32 CDBOperateWildcastTaskSwitch::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY:
        case EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY_BY_VNA:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_TASK_SWITCH_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_TASK_SWITCH_PROC_ADD:
        case EN_WILDCAST_TASK_SWITCH_PROC_MODIFY:
        case EN_WILDCAST_TASK_SWITCH_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_TASK_SWITCH_PROC_CHECK_ADD:
        case EN_WILDCAST_TASK_SWITCH_PROC_CHECK_MODIFY:
        case EN_WILDCAST_TASK_SWITCH_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}

int32 CDBOperateWildcastTaskSwitch::QuerySummaryByVna(const char* uuid, vector<CDbWildcastTaskSwitchDetail> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryByVna uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)uuid);    

    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY_BY_VNA],\
        vInParam, \
        this,\
        EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY_BY_VNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
           VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryByVna(%s) IProc->Handle failed(%d) \n",
            uuid,ret);
        }
        return ret;
    }
        
    return 0;
}
    
int32 CDBOperateWildcastTaskSwitch::QuerySummary(vector<CDbWildcastTaskSwitchDetail> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_WILDCAST_TASK_SWITCH_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateWildcastTaskSwitch::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbWildcastTaskSwitchDetail> *pvOut;
    pvOut = static_cast<vector<CDbWildcastTaskSwitchDetail>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbWildcastTaskSwitchDetail info;
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 14);
    info.SetUuid(prs->GetFieldValue(0));  
    
    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    info.SetTaskState(value);
    
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetTaskSendNum(value);
    
    info.SetSwitchUuid(prs->GetFieldValue(3)); 
    info.SetSwitchName(prs->GetFieldValue(4)); 

    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchType(value);

    info.SetPgUuid(prs->GetFieldValue(6)); 

    if( false == prs->GetFieldValue(7,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMtuMax(value);

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetNicMaxNum(value);

    info.SetPortName(prs->GetFieldValue(9)); 

    if( false == prs->GetFieldValue(10,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(value);

    if( false == prs->GetFieldValue(11,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondMode(value);
    
    info.SetVnaUuid(prs->GetFieldValue(12)); 

    if( false == prs->GetFieldValue(13,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVnaIsOnline(value);
    
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateWildcastTaskSwitch::Query(CDbWildcastTaskSwitchDetail & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_QUERY],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastTaskSwitch::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 2 获取kernel 信息
    CDbWildcastTaskSwitchDetail *pOutPort = NULL;
    pOutPort = static_cast<CDbWildcastTaskSwitchDetail *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 15);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetUuid(prs->GetFieldValue(1));  
    
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    (*pOutPort).SetTaskState(value);
    
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetTaskSendNum(value);
    
    (*pOutPort).SetSwitchUuid(prs->GetFieldValue(4)); 
    (*pOutPort).SetSwitchName(prs->GetFieldValue(5)); 

    if( false == prs->GetFieldValue(6,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetSwitchType(value);

    (*pOutPort).SetPgUuid(prs->GetFieldValue(7)); 

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 8 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMtuMax(value);

    if( false == prs->GetFieldValue(9,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 9 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetNicMaxNum(value);

    (*pOutPort).SetPortName(prs->GetFieldValue(10)); 

    if( false == prs->GetFieldValue(11,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 11 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPortType(value);

    if( false == prs->GetFieldValue(12,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 12 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetBondMode(value);
    
    (*pOutPort).SetVnaUuid(prs->GetFieldValue(13)); 

    if( false == prs->GetFieldValue(14,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::QuerySummaryCallback GetFieldValue 14 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetVnaIsOnline(value);

    return 0;
}

int32 CDBOperateWildcastTaskSwitch::CheckAdd(CDbWildcastTaskSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    // vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum);    
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::CheckAdd IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastTaskSwitch::Add(CDbWildcastTaskSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;     
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_ADD],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Add IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        info.SetUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastTaskSwitch::CheckModify(CDbWildcastTaskSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum);  

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::CheckModify IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastTaskSwitch::Modify(CDbWildcastTaskSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Modify IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskSwitch::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateWildcastTaskSwitch::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckDel uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckDel(%s) IProc->Handle failed. ret : %d.\n", 
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckDel(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateWildcastTaskSwitch::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::Del uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskSwitchProc[EN_WILDCAST_TASK_SWITCH_PROC_DEL],
        vInParam, 
        this,
        EN_WILDCAST_TASK_SWITCH_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::Del(%s) proc return failed. ret : %d.\n", 
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateWildcastTaskSwitch::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateWildcastTaskSwitch::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskSwitch::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateWildcastTaskSwitch::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbWildcastTaskSwitch> vInfo;
    int ret = 0;
    ret = QuerySummary(vInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed. ret : " << ret << endl;
        return;
    }

    cout << "QuerySummary success " << endl;
    cout << "|" <<setw(32) << "uuid" << 
                    "|" << setw(32) << "name" << endl;
    vector<CDbWildcastTaskSwitch>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
    // query 
    cout << "-----------------------------------" << endl;
    it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        CDbPort info;
        info.SetUuid((*it).GetUuid().c_str());
        ret = Query(info);
        if( ret != 0 )
        {
            cout << "Query failed. uuid : " << info.GetUuid() << " ret : " << ret << endl;
            continue;
        }

        info.DbgShow();
        cout << "-----------------" << endl;        
    }
#endif     
}

/* test code ------------------------------------------------------------

*/
void db_query_wc_task_switch_summary()
{    
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }

    vector<CDbWildcastTaskSwitchDetail> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbWildcastTaskSwitchDetail>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_wc_task_switch_summary);

void db_query_wc_task_switch(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastTaskSwitchDetail info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateWildcastTaskSwitch query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateWildcastTaskSwitch query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_wc_task_switch);


void db_check_add_wc_task_switch(char* vna_uuid,char* switch_uuid, int32 state,int32 send_num )
{
    if( NULL == vna_uuid || NULL == switch_uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastTaskSwitch info;
    info.SetVnaUuid(vna_uuid);
    info.SetSwitchUuid(switch_uuid);
    info.SetState(state);
    info.SetSendNum(send_num);

    //info.DbgShow();
    int ret = pOper->CheckAdd(info);
    if( ret != 0 )
    {
        cout << "CheckAdd failed." << ret << endl;
        return;
    }
    cout << "CheckAdd success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_wc_task_switch);


void db_add_wc_task_switch(char* vna_uuid,char* switch_uuid, int32 state,int32 send_num )
{
    if( NULL == vna_uuid || NULL == switch_uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastTaskSwitch info;
    info.SetVnaUuid(vna_uuid);
    info.SetSwitchUuid(switch_uuid);
    info.SetState(state);
    info.SetSendNum(send_num);

    //info.DbgShow();
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_wc_task_switch);


void db_check_mod_wc_task_switch(char* uuid, char* vna_uuid,char* switch_uuid, int32 state,int32 send_num )
{
    if( NULL == uuid || NULL == vna_uuid || NULL == switch_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastTaskSwitch info;
    info.SetUuid(uuid);
    info.SetVnaUuid(vna_uuid);
    info.SetSwitchUuid(switch_uuid);
    info.SetState(state);
    info.SetSendNum(send_num);

    //info.DbgShow();
    int ret = pOper->CheckModify(info);
    if( ret != 0 )
    {
        cout << "CheckModify failed." << ret << endl;
        return;
    }
    cout << "CheckModify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_wc_task_switch);


void db_mod_wc_task_switch(char* uuid, char* vna_uuid,char* switch_uuid, int32 state,int32 send_num )
{
    if( NULL == uuid || NULL == vna_uuid || NULL == switch_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastTaskSwitch info;
    info.SetUuid(uuid);
    info.SetVnaUuid(vna_uuid);
    info.SetSwitchUuid(switch_uuid);
    info.SetState(state);
    info.SetSendNum(send_num);

    //info.DbgShow();
    int ret = pOper->Modify(info);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_wc_task_switch);


void db_check_del_wc_task_switch(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }
    //info.DbgShow();
    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "CheckDel failed." << ret << endl;
        return;
    }
    cout << "CheckDel success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_wc_task_switch);

void db_del_wc_task_switch(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskSwitch * pOper = GetDbIWildcastTaskSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskSwitch is NULL" << endl;
        return ;
    }
    //info.DbgShow();
    int ret = pOper->Del(uuid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_wc_task_switch);

}



