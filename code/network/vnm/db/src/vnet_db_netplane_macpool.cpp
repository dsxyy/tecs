

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_macrange.h"
#include "vnet_db_netplane_macpool.h"
    
namespace zte_tecs
{
typedef enum tagNetplaneMacPoolPoolProc
{
    EN_NETPLANE_MACPOOL_PROC_CHECK_ADD = 0,
    EN_NETPLANE_MACPOOL_PROC_ADD,
    EN_NETPLANE_MACPOOL_PROC_CHECK_DEL,
    EN_NETPLANE_MACPOOL_PROC_DEL,
    EN_NETPLANE_MACPOOL_PROC_QUERY,   
    EN_NETPLANE_MACPOOL_PROC_QUERY_BY_NPRANGE,
    EN_NETPLANE_MACPOOL_PROC_ALL,
}EN_NETPLANE_MACPOOL_PROC;

const char *s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_ALL] = 
{        
    "pf_net_check_add_netplane_macpool",  
    "pf_net_add_netplane_macpool",    
    "pf_net_check_del_netplane_macpool",  
    "pf_net_del_netplane_macpool", 
    "pf_net_query_netplane_macpool", 
    "pf_net_query_netplane_macpool_by_nprange", 
};

    
CDBOperateNetplaneMacPool::CDBOperateNetplaneMacPool(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplaneMacPool::~CDBOperateNetplaneMacPool()
{
    
}

int32 CDBOperateNetplaneMacPool::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_NETPLANE_MACPOOL_PROC_QUERY:
        case EN_NETPLANE_MACPOOL_PROC_QUERY_BY_NPRANGE:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_MACPOOL_PROC_ADD:
        case EN_NETPLANE_MACPOOL_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_MACPOOL_PROC_CHECK_ADD:
        case EN_NETPLANE_MACPOOL_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    

int32 CDBOperateNetplaneMacPool::Query(const char* uuid,vector<CDbNetplaneMacPool> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_QUERY],\
        vInParam, \
        this,\
        EN_NETPLANE_MACPOOL_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::Query(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneMacPool::QueryByNpRange(const char* np_uuid, int64 mac_begin, int64 mac_end, vector<CDbNetplaneMacPool> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::QueryByNpRange IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)np_uuid);
    CADOParameter inBegin(CADOParameter::paramInt64, CADOParameter::paramInput, mac_begin);
    CADOParameter inEnd(CADOParameter::paramInt64, CADOParameter::paramInput, mac_end);
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inBegin);
    vInParam.push_back(inEnd);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_QUERY_BY_NPRANGE],\
        vInParam, \
        this,\
        EN_NETPLANE_MACPOOL_PROC_QUERY_BY_NPRANGE,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::QueryByNpRange(%s,%lld,%lld) IProc->Handle failed(%d) \n",
                np_uuid,mac_begin,mac_end,ret);
        }
        return ret;
    }
    return 0;
}

    

int32 CDBOperateNetplaneMacPool::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneMacPool> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneMacPool>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbNetplaneMacPool info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
       
    info.SetUuid(prs->GetFieldValue(0));

    int64 v64 = 0;
    if( false == prs->GetFieldValue(1,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMacNum(v64);
    

    info.SetMacStr(prs->GetFieldValue(2));
    
    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateNetplaneMacPool::CheckAdd(CDbNetplaneMacPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inMacNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMacNum());
    
    CADOParameter inMacStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMacStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inMacNum);    
    vInParam.push_back(inMacStr);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_NETPLANE_MACPOOL_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneMacPool::Add(CDbNetplaneMacPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inMacNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMacNum());
    CADOParameter inMacStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMacStr().c_str());    
   
    vInParam.push_back(inUuid);   
    vInParam.push_back(inMacNum);     
    vInParam.push_back(inMacStr);  
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_MACPOOL_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneMacPool::CheckDel(CDbNetplaneMacPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inMacNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMacNum());
    
    CADOParameter inMacStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMacStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inMacNum);    
    vInParam.push_back(inMacStr);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_NETPLANE_MACPOOL_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::CheckDel IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneMacPool::CheckDel proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneMacPool::Del(CDbNetplaneMacPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inMacNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMacNum());
    CADOParameter inMacStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMacStr().c_str());    
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inMacNum);    
    vInParam.push_back(inMacStr);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneMacPoolProc[EN_NETPLANE_MACPOOL_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_MACPOOL_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneMacPool::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateNetplaneMacPool::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneMacPool::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateNetplaneMacPool::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbNetplaneMacPool> vInfo;
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
    vector<CDbNetplaneMacPool>::iterator it = vInfo.begin();
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

void db_query_np_macpool(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneMacPool * pOper = GetDbINetplaneMacPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneMacPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneMacPool> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneMacPool>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_np_macpool);

void db_query_np_macpool_by_range(const char* uuid, int64 begin, int64 end)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneMacPool * pOper = GetDbINetplaneMacPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneMacPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneMacPool> outVInfo;
    int ret = pOper->QueryByNpRange(uuid,begin,end,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByNpRange failed." << ret << endl;
        return;
    }
    cout << "QueryByNpRange success." << endl;
    vector<CDbNetplaneMacPool>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_np_macpool_by_range);

void db_check_add_np_macpool(char* uuid, int64 mac)
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneMacPool * pOper = GetDbINetplaneMacPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneMacPool is NULL" << endl;
        return ;
    }

    CDbNetplaneMacPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetMacNum(mac);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",mac);        
    info.SetMacStr(buf);
    

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
DEFINE_DEBUG_FUNC(db_check_add_np_macpool);

void db_add_np_macpool(char* uuid, int64 mac )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneMacPool * pOper = GetDbINetplaneMacPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneMacPool is NULL" << endl;
        return ;
    }

    CDbNetplaneMacPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetMacNum(mac);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",mac);        
    info.SetMacStr(buf);

    
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
DEFINE_DEBUG_FUNC(db_add_np_macpool);

void db_del_np_macpool(char * uuid, int64 mac )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneMacPool * pOper = GetDbINetplaneMacPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneMacPool is NULL" << endl;
        return ;
    }

    CDbNetplaneMacPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetMacNum(mac);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",mac);        
    info.SetMacStr(buf);
    

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
DEFINE_DEBUG_FUNC(db_del_np_macpool);


}




