

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_logicnetwork_iprange.h"
    
namespace zte_tecs
{
typedef enum tagLogicNetworkIpRangeProc
{
    EN_LOGICNETWORK_IPRANGE_PROC_CHECK_ADD = 0,
    EN_LOGICNETWORK_IPRANGE_PROC_ADD,
    EN_LOGICNETWORK_IPRANGE_PROC_DEL,
    EN_LOGICNETWORK_IPRANGE_PROC_QUERY,    
    EN_LOGICNETWORK_IPRANGE_PROC_QUERY_ALLOC, 
    EN_LOGICNETWORK_IPRANGE_PROC_ALL,
}EN_LOGICNETWORK_IPRANGE_PROC;

const char *s_astrLogicNetworkIpRangeProc[EN_LOGICNETWORK_IPRANGE_PROC_ALL] = 
{        
    "pf_net_check_add_logicnetwork_iprange",  
    "pf_net_add_logicnetwork_iprange",    
    "pf_net_del_logicnetwork_iprange", 
    "pf_net_query_logicnetwork_iprange", 
    "pf_net_query_logicnetwork_iprange_alloc",
};

    
CDBOperateLogicNetworkIpRange::CDBOperateLogicNetworkIpRange(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateLogicNetworkIpRange::~CDBOperateLogicNetworkIpRange()
{
    
}

int32 CDBOperateLogicNetworkIpRange::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_LOGICNETWORK_IPRANGE_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_LOGICNETWORK_IPRANGE_PROC_QUERY_ALLOC:
        {
            return QueryAllocNumCallback(prs,nil);
        }
        break;
        case EN_LOGICNETWORK_IPRANGE_PROC_ADD:
        case EN_LOGICNETWORK_IPRANGE_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_LOGICNETWORK_IPRANGE_PROC_CHECK_ADD:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}

int32 CDBOperateLogicNetworkIpRange::QueryAllocNum(const char* uuid, int64 begin, int64 end, int32 &outTotalNum)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::QueryAllocNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryAllocNum uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if( begin > end )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryAllocNum begin > end.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inBegin(CADOParameter::paramInt64, CADOParameter::paramInput, begin);
    CADOParameter inEnd(CADOParameter::paramInt64, CADOParameter::paramInput, end);
    vInParam.push_back(inUuid);
    vInParam.push_back(inBegin);
    vInParam.push_back(inEnd);    

    CDbResourceAllocNum Info;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,\
        s_astrLogicNetworkIpRangeProc[EN_LOGICNETWORK_IPRANGE_PROC_QUERY_ALLOC],\
        vInParam, \
        this,\
        EN_LOGICNETWORK_IPRANGE_PROC_QUERY_ALLOC,\
        static_cast<void*> (&Info));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::QueryAllocNum(%s,%lld,%lld) IProc->Handle failed(%d) \n",
                uuid,begin,end,ret);
        }
        return ret;
    }

    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryAllocNum(%s,%lld,%lld) proc return failed. ret : %d.\n",
                uuid,begin,end,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置 num 值
    outTotalNum = Info.GetNum();
        
    return 0;
}

int32 CDBOperateLogicNetworkIpRange::Query(const char* uuid,vector<CDbLogicNetworkIpRange> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::Query uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrLogicNetworkIpRangeProc[EN_LOGICNETWORK_IPRANGE_PROC_QUERY],\
        vInParam, \
        this,\
        EN_LOGICNETWORK_IPRANGE_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::Query(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateLogicNetworkIpRange::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbLogicNetworkIpRange> *pvOut;
    pvOut = static_cast<vector<CDbLogicNetworkIpRange>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbLogicNetworkIpRange info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 7);
           
    info.SetUuid(prs->GetFieldValue(0));

    int64 v64 = 0;
    if( false == prs->GetFieldValue(1,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::QueryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBeginNum(v64);
    
    if( false == prs->GetFieldValue(2,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetEndNum(v64);

    if( false == prs->GetFieldValue(3,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::QueryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMaskNum(v64);

    info.SetBeginStr(prs->GetFieldValue(4));
    info.SetEndStr(prs->GetFieldValue(5));
    info.SetMaskStr(prs->GetFieldValue(6));
    
    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateLogicNetworkIpRange::CheckAdd(CDbLogicNetworkIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkIpRangeProc[EN_LOGICNETWORK_IPRANGE_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_LOGICNETWORK_IPRANGE_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateLogicNetworkIpRange::Add(CDbLogicNetworkIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkIpRangeProc[EN_LOGICNETWORK_IPRANGE_PROC_ADD],
        vInParam, 
        this,
        EN_LOGICNETWORK_IPRANGE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetworkIpRange::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateLogicNetworkIpRange::Del(CDbLogicNetworkIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkIpRangeProc[EN_LOGICNETWORK_IPRANGE_PROC_DEL],
        vInParam, 
        this,
        EN_LOGICNETWORK_IPRANGE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateLogicNetworkIpRange::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateLogicNetworkIpRange::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

int32 CDBOperateLogicNetworkIpRange::QueryAllocNumCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryAllocNumCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbResourceAllocNum *proc_ret = NULL;     
    proc_ret = static_cast<CDbResourceAllocNum *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryAllocNumCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryAllocNumCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetworkIpRange::QueryAllocNumCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetNum(value);

    return 0;
}


void CDBOperateLogicNetworkIpRange::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbLogicNetworkIpRange> vInfo;
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
    vector<CDbLogicNetworkIpRange>::iterator it = vInfo.begin();
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

void db_query_ln_iprange(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateLogicNetworkIpRange * pOper = GetDbILogicNetworkIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetworkIpRange is NULL" << endl;
        return ;
    }

    vector<CDbLogicNetworkIpRange> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbLogicNetworkIpRange>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_ln_iprange);


void db_query_ln_iprange_alloc(char* uuid, int64 begin, int64 end )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetworkIpRange * pOper = GetDbILogicNetworkIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetworkIpRange is NULL" << endl;
        return ;
    }


    //info.DbgShow();
    int32 num = 0;
    int ret = pOper->QueryAllocNum(uuid,begin,end,num);
    if( ret != 0 )
    {
        cout << "QueryAllocNum failed." << ret << endl;
        return;
    }
    cout << "QueryAllocNum success." << num << endl;
}
DEFINE_DEBUG_FUNC(db_query_ln_iprange_alloc);

void db_check_add_ln_iprange(char* uuid, int32 type,int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetworkIpRange * pOper = GetDbILogicNetworkIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetworkIpRange is NULL" << endl;
        return ;
    }

    CDbLogicNetworkIpRange info;
    info.SetUuid(uuid);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);
    

    //info.DbgShow();
    int ret = pOper->CheckAdd(info);
    if( ret != 0 )
    {
        cout << "can Add failed." << ret << endl;
        return;
    }
    cout << "can Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_ln_iprange);

void db_add_ln_iprange(char* uuid, int32 type,int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetworkIpRange * pOper = GetDbILogicNetworkIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetworkIpRange is NULL" << endl;
        return ;
    }

    CDbLogicNetworkIpRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);

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
DEFINE_DEBUG_FUNC(db_add_ln_iprange);

void db_del_ln_iprange(char * uuid, int32 type,int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetworkIpRange * pOper = GetDbILogicNetworkIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetworkIpRange is NULL" << endl;
        return ;
    }

    CDbLogicNetworkIpRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);
    

    //info.DbgShow();
    int ret = pOper->Del(info);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_del_ln_iprange);


}




