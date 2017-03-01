

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_wildcast_task_loopback.h"
    
namespace zte_tecs
{
typedef enum tagWildcastTaskLoopbackProc
{
    EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY = 0,
    EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY_BY_VNA,
    EN_WILDCAST_TASK_LOOPBACK_PROC_QUERY,   
    EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_ADD,
    EN_WILDCAST_TASK_LOOPBACK_PROC_ADD,
    EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_MODIFY,
    EN_WILDCAST_TASK_LOOPBACK_PROC_MODIFY,
    EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_DEL,
    EN_WILDCAST_TASK_LOOPBACK_PROC_DEL,
    EN_WILDCAST_TASK_LOOPBACK_PROC_ALL,
}EN_WILDCAST_TASK_LOOPBACK_PROC;

const char *s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_ALL] = 
{        
    "pf_net_query_wildcast_task_loopback_summary",
    "pf_net_query_wildcast_task_loopback_summary_by_vna",
    "pf_net_query_wildcast_task_loopback",
    "pf_net_check_add_wildcast_task_loopback",  
    "pf_net_add_wildcast_task_loopback",    
    "pf_net_check_modify_wildcast_task_loopback", 
    "pf_net_modify_wildcast_task_loopback", 
    "pf_net_check_del_wildcast_task_loopback",  
    "pf_net_del_wildcast_task_loopback",  
};

    
CDBOperateWildcastTaskLoopback::CDBOperateWildcastTaskLoopback(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateWildcastTaskLoopback::~CDBOperateWildcastTaskLoopback()
{
    
}

int32 CDBOperateWildcastTaskLoopback::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY:
        case EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY_BY_VNA:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_TASK_LOOPBACK_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_TASK_LOOPBACK_PROC_ADD:
        case EN_WILDCAST_TASK_LOOPBACK_PROC_MODIFY:
        case EN_WILDCAST_TASK_LOOPBACK_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_ADD:
        case EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_MODIFY:
        case EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}

int32 CDBOperateWildcastTaskLoopback::QuerySummaryByVna(const char* uuid, vector<CDbWildcastTaskLoopback> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryByVna uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)uuid);    

    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY_BY_VNA],\
        vInParam, \
        this,\
        EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY_BY_VNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryByVna IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}
    
int32 CDBOperateWildcastTaskLoopback::QuerySummary(vector<CDbWildcastTaskLoopback> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_WILDCAST_TASK_LOOPBACK_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateWildcastTaskLoopback::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbWildcastTaskLoopback> *pvOut;
    pvOut = static_cast<vector<CDbWildcastTaskLoopback>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbWildcastTaskLoopback info;
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 8);
    info.SetUuid(prs->GetFieldValue(0));  
    
    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    info.SetState(value);
    
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSendNum(value);
    
    info.SetLoopbackUuid(prs->GetFieldValue(3)); 
    info.SetPortName(prs->GetFieldValue(4)); 

    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsLoop(value);
        
    info.SetVnaUuid(prs->GetFieldValue(6)); 

    if( false == prs->GetFieldValue(7,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 7 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVnaIsOnline(value);
    
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateWildcastTaskLoopback::Query(CDbWildcastTaskLoopback & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_QUERY],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastTaskLoopback::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 2 获取kernel 信息
    CDbWildcastTaskLoopback *pOutPort = NULL;
    pOutPort = static_cast<CDbWildcastTaskLoopback *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 9);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetUuid(prs->GetFieldValue(1));  
    
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    (*pOutPort).SetState(value);
    
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetSendNum(value);
    
    (*pOutPort).SetLoopbackUuid(prs->GetFieldValue(4)); 
    (*pOutPort).SetPortName(prs->GetFieldValue(5)); 

    if( false == prs->GetFieldValue(6,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsLoop(value);
    
    (*pOutPort).SetVnaUuid(prs->GetFieldValue(7)); 

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::QuerySummaryCallback GetFieldValue 8 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetVnaIsOnline(value);

    return 0;
}

int32 CDBOperateWildcastTaskLoopback::CheckAdd(CDbWildcastTaskLoopback & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    //CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inLoopBackUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLoopbackUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    // vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inLoopBackUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum);    
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastTaskLoopback::Add(CDbWildcastTaskLoopback & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;     
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inLoopBackUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLoopbackUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inLoopBackUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_ADD],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastTaskLoopback::CheckModify(CDbWildcastTaskLoopback & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inLoopBackUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLoopbackUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inLoopBackUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum);  

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::CheckModify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastTaskLoopback::Modify(CDbWildcastTaskLoopback & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inLoopBackUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLoopbackUuid().c_str());
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState()); 
    CADOParameter inSendNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSendNum()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inLoopBackUuid);
    vInParam.push_back(inState);
    vInParam.push_back(inSendNum);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastTaskLoopback::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateWildcastTaskLoopback::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckDel uuid is NULL.\n");
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
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckDel(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateWildcastTaskLoopback::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::Del uuid is NULL.\n");
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
        s_astrWildcastTaskLoopbackProc[EN_WILDCAST_TASK_LOOPBACK_PROC_DEL],
        vInParam, 
        this,
        EN_WILDCAST_TASK_LOOPBACK_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateWildcastTaskLoopback::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateWildcastTaskLoopback::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastTaskLoopback::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateWildcastTaskLoopback::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbWildcastTaskLoopback> vInfo;
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
    vector<CDbWildcastTaskLoopback>::iterator it = vInfo.begin();
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
void db_query_wc_task_loopback_summary()
{    
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
        return ;
    }

    vector<CDbWildcastTaskLoopback> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbWildcastTaskLoopback>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_wc_task_loopback_summary);

void db_query_wc_task_loopback(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
        return ;
    }

    CDbWildcastTaskLoopback info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateWildcastTaskLoopback query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateWildcastTaskLoopback query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_wc_task_loopback);


void db_check_add_wc_task_loopback(char* vna_uuid,char* loopback_uuid, int32 state,int32 send_num )
{
    if( NULL == vna_uuid || NULL == loopback_uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
        return ;
    }

    CDbWildcastTaskLoopback info;
    info.SetVnaUuid(vna_uuid);
    info.SetLoopbackUuid(loopback_uuid);
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
DEFINE_DEBUG_FUNC(db_check_add_wc_task_loopback);


void db_add_wc_task_loopback(char* vna_uuid,char* loopback_uuid, int32 state,int32 send_num )
{
    if( NULL == vna_uuid || NULL == loopback_uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
        return ;
    }

    CDbWildcastTaskLoopback info;
    info.SetVnaUuid(vna_uuid);
    info.SetLoopbackUuid(loopback_uuid);
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
DEFINE_DEBUG_FUNC(db_add_wc_task_loopback);


void db_check_mod_wc_task_loopback(char* uuid, char* vna_uuid,char* loopback_uuid, int32 state,int32 send_num )
{
    if( NULL == uuid || NULL == vna_uuid || NULL == loopback_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
        return ;
    }

    CDbWildcastTaskLoopback info;
    info.SetUuid(uuid);
    info.SetVnaUuid(vna_uuid);
    info.SetLoopbackUuid(loopback_uuid);
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
DEFINE_DEBUG_FUNC(db_check_mod_wc_task_loopback);


void db_mod_wc_task_loopback(char* uuid, char* vna_uuid,char* loopback_uuid, int32 state,int32 send_num )
{
    if( NULL == uuid || NULL == vna_uuid || NULL == loopback_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
        return ;
    }

    CDbWildcastTaskLoopback info;
    info.SetUuid(uuid);
    info.SetVnaUuid(vna_uuid);
    info.SetLoopbackUuid(loopback_uuid);
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
DEFINE_DEBUG_FUNC(db_mod_wc_task_loopback);


void db_check_del_wc_task_loopback(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_check_del_wc_task_loopback);

void db_del_wc_task_loopback(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastTaskLoopback * pOper = GetDbIWildcastTaskLoopback();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastTaskLoopback is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_wc_task_loopback);

}



