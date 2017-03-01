

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_iprange.h"
#include "vnet_db_netplane_ippool.h"
    
namespace zte_tecs
{
typedef enum tagNetplaneIpPoolProc
{
    EN_NETPLANE_IPPOOL_PROC_CHECK_ADD = 0,
    EN_NETPLANE_IPPOOL_PROC_ADD,
    EN_NETPLANE_IPPOOL_PROC_CHECK_DEL,
    EN_NETPLANE_IPPOOL_PROC_DEL,
    EN_NETPLANE_IPPOOL_PROC_QUERY,  
    EN_NETPLANE_IPPOOL_PROC_QUERY_BY_NPRANGE,
    EN_NETPLANE_IPPOOL_PROC_ALL,
}EN_VSWITCH_PROC;

const char *s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_ALL] = 
{        
    "pf_net_check_add_netplane_ippool",  
    "pf_net_add_netplane_ippool",    
    "pf_net_check_del_netplane_ippool",     
    "pf_net_del_netplane_ippool", 
    "pf_net_query_netplane_ippool", 
    "pf_net_query_netplane_ippool_by_nprange", 
};

    
CDBOperateNetplaneIpPool::CDBOperateNetplaneIpPool(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplaneIpPool::~CDBOperateNetplaneIpPool()
{
    
}

int32 CDBOperateNetplaneIpPool::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_NETPLANE_IPPOOL_PROC_QUERY:
        case EN_NETPLANE_IPPOOL_PROC_QUERY_BY_NPRANGE:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_IPPOOL_PROC_ADD:
        case EN_NETPLANE_IPPOOL_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_IPPOOL_PROC_CHECK_ADD:
        case EN_NETPLANE_IPPOOL_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    

int32 CDBOperateNetplaneIpPool::Query(const char* uuid,vector<CDbNetplaneIpPool> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_QUERY],\
        vInParam, \
        this,\
        EN_NETPLANE_IPPOOL_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {        
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
           VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::Query(%s) IProc->Handle failed(%d) \n",uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneIpPool::QueryByNpRange(const char* uuid, int64 begin, int64 end, vector<CDbNetplaneIpPool> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::QueryByLnRange IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inBegin(CADOParameter::paramInt64, CADOParameter::paramInput, begin);
    CADOParameter inEnd(CADOParameter::paramInt64, CADOParameter::paramInput, end);
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inBegin);
    vInParam.push_back(inEnd);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_QUERY_BY_NPRANGE],\
        vInParam, \
        this,\
        EN_NETPLANE_IPPOOL_PROC_QUERY_BY_NPRANGE,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::QueryByLnRange(%s,%lld,%lld, IProc->Handle failed(%d) \n",
                uuid,begin,end,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneIpPool::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneIpPool> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneIpPool>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbNetplaneIpPool info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 5);
       
    info.SetUuid(prs->GetFieldValue(0));

    int64 v64 = 0;
    if( false == prs->GetFieldValue(1,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::QueryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIpNum(v64);
    
    if( false == prs->GetFieldValue(2,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMaskNum(v64);

    info.SetIpStr(prs->GetFieldValue(3));
    info.SetMaskStr(prs->GetFieldValue(4));
    
    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateNetplaneIpPool::CheckAdd(CDbNetplaneIpPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inIpNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetIpNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inIpStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIpStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inIpNum);    
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inIpStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_NETPLANE_IPPOOL_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneIpPool::Add(CDbNetplaneIpPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inIpNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetIpNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inIpStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIpStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inIpNum);    
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inIpStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_IPPOOL_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneIpPool::CheckDel(CDbNetplaneIpPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inIpNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetIpNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inIpStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIpStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inIpNum);    
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inIpStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_NETPLANE_IPPOOL_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::CheckDel IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpPool::CheckDel proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneIpPool::Del(CDbNetplaneIpPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inIpNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetIpNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inIpStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIpStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inIpNum);    
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inIpStr);    
    vInParam.push_back(inMaskStr);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpPoolProc[EN_NETPLANE_IPPOOL_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_IPPOOL_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneIpPool::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateNetplaneIpPool::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::CanOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::CanOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpPool::CanOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateNetplaneIpPool::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbNetplaneIpPool> vInfo;
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
    vector<CDbNetplaneIpPool>::iterator it = vInfo.begin();
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

void db_query_netplane_ippool(const char* uuid, int32 type)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneIpPool * pOper = GetDbINetplaneIpPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneIpPool> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneIpPool>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_netplane_ippool);

void db_query_netplane_ippool_by_range(const char* uuid, int64 begin, int64 end)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneIpPool * pOper = GetDbINetplaneIpPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneIpPool> outVInfo;
    int ret = pOper->QueryByNpRange(uuid,begin, end, outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByLnRange failed." << ret << endl;
        return;
    }
    cout << "QueryByLnRange success." << endl;
    vector<CDbNetplaneIpPool>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_netplane_ippool_by_range);

void db_check_add_netplane_ippool(char* uuid, int32 type,int64 ip, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpPool * pOper = GetDbINetplaneIpPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpPool is NULL" << endl;
        return ;
    }

    CDbNetplaneIpPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetIpNum(ip);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",ip);        
    info.SetIpStr(buf);
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
DEFINE_DEBUG_FUNC(db_check_add_netplane_ippool);

void db_check_del_netplane_ippool(char* uuid, int32 type,int64 ip, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpPool * pOper = GetDbINetplaneIpPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpPool is NULL" << endl;
        return ;
    }

    CDbNetplaneIpPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetIpNum(ip);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",ip);        
    info.SetIpStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);
    
    //info.DbgShow();
    int ret = pOper->CheckDel(info);
    if( ret != 0 )
    {
        cout << "can Add failed." << ret << endl;
        return;
    }
    cout << "can Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_del_netplane_ippool);

void db_add_netplane_ippool(char* uuid, int32 type,int64 ip,  int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpPool * pOper = GetDbINetplaneIpPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpPool is NULL" << endl;
        return ;
    }

    CDbNetplaneIpPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetIpNum(ip);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",ip);        
    info.SetIpStr(buf);
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
DEFINE_DEBUG_FUNC(db_add_netplane_ippool);

void db_del_netplane_ippool(char * uuid, int32 type,int64 ip, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpPool * pOper = GetDbINetplaneIpPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpPool is NULL" << endl;
        return ;
    }

    CDbNetplaneIpPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetIpNum(ip);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",ip);        
    info.SetIpStr(buf);
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
DEFINE_DEBUG_FUNC(db_del_netplane_ippool);


}




