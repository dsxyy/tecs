

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_vlanrange.h"
    
namespace zte_tecs
{
typedef enum tagNetplaneVlanRangeProc
{
    EN_NETPLANE_VLANRANGE_PROC_CHECK_ADD = 0,
    EN_NETPLANE_VLANRANGE_PROC_ADD,
    EN_NETPLANE_VLANRANGE_PROC_DEL,
    EN_NETPLANE_VLANRANGE_PROC_QUERY_ALLOC,
    EN_NETPLANE_VLANRANGE_PROC_QUERY,     
    EN_NETPLANE_VLANRANGE_PROC_QUERY_VMSHAREVLANS,   
    EN_NETPLANE_VLANRANGE_PROC_ALL,
}EN_NETPLANE_VLANRANGE_PROC;

const char *s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_ALL] = 
{        
    "pf_net_check_add_netplane_vlanrange",  
    "pf_net_add_netplane_vlanrange",    
    "pf_net_del_netplane_vlanrange", 
    "pf_net_query_netplane_vlanrange_alloc",
    "pf_net_query_netplane_vlanrange", 
    "pf_net_query_netplane_vmshare_vlans", 
};

    
CDBOperateNetplaneVlanRange::CDBOperateNetplaneVlanRange(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplaneVlanRange::~CDBOperateNetplaneVlanRange()
{
    
}

int32 CDBOperateNetplaneVlanRange::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_NETPLANE_VLANRANGE_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_VLANRANGE_PROC_QUERY_ALLOC:
        {
            return QueryAllocNumCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_VLANRANGE_PROC_QUERY_VMSHAREVLANS:
        {
            return QueryVmShareVlansCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_VLANRANGE_PROC_ADD:
        case EN_NETPLANE_VLANRANGE_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_VLANRANGE_PROC_CHECK_ADD:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}


int32 CDBOperateNetplaneVlanRange::QueryAllocNum(const char* uuid, int32 begin, int32 end, int32 &outTotalNum)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryAllocNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNum uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if( begin > end )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNum begin > end.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inBegin(CADOParameter::paramInt, CADOParameter::paramInput, begin);
    CADOParameter inEnd(CADOParameter::paramInt, CADOParameter::paramInput, end);
    vInParam.push_back(inUuid);
    vInParam.push_back(inBegin);
    vInParam.push_back(inEnd);    

    CDbResourceAllocNum Info;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,\
        s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_QUERY_ALLOC],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANRANGE_PROC_QUERY_ALLOC,\
        static_cast<void*> (&Info));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryAllocNum(%s,%lld,%lld) IProc->Handle failed(%d) \n",
                uuid, begin, end,ret);
        }
        return ret;
    }

    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNum(%s,%lld,%lld) proc return failed. ret : %d.\n", 
                uuid, begin, end,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置 num 值
    outTotalNum = Info.GetNum();
        
    return 0;
}


int32 CDBOperateNetplaneVlanRange::QueryAllocNumCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNumCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbResourceAllocNum *proc_ret = NULL;     
    proc_ret = static_cast<CDbResourceAllocNum *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNumCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNumCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryAllocNumCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetNum(value);

    return 0;
}

int32 CDBOperateNetplaneVlanRange::QueryVmShareVlans(const char* np_uuid, string & outVlans)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryVmShareVlans IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( np_uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryVmShareVlans uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)np_uuid);
    vInParam.push_back(inUuid);

    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,\
        s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_QUERY_VMSHAREVLANS],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANRANGE_PROC_QUERY_VMSHAREVLANS,\
        static_cast<void*> (&outVlans));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryVmShareVlans(%s) IProc->Handle failed(%d) \n",
                np_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneVlanRange::QueryVmShareVlansCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryVmShareVlansCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    string *pvOut = NULL;
    pvOut = static_cast<string *>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryVmShareVlansCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);
    
    (*pvOut) = prs->GetFieldValue(0);

    return 0;
}



int32 CDBOperateNetplaneVlanRange::Query(const char* uuid,vector<CDbNetplaneVlanRange> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::Query uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_QUERY],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANRANGE_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::Query(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneVlanRange::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneVlanRange> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneVlanRange>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbNetplaneVlanRange info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
           
    info.SetUuid(prs->GetFieldValue(0));

    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBeginNum(value);
    
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetEndNum(value);
    
    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateNetplaneVlanRange::CheckAdd(CDbNetplaneVlanRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEndNum());
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_NETPLANE_VLANRANGE_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneVlanRange::Add(CDbNetplaneVlanRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEndNum());
   
    vInParam.push_back(inUuid);    
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_VLANRANGE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanRange::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateNetplaneVlanRange::Del(CDbNetplaneVlanRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEndNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanRangeProc[EN_NETPLANE_VLANRANGE_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_VLANRANGE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneVlanRange::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateNetplaneVlanRange::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanRange::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateNetplaneVlanRange::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbNetplaneVlanRange> vInfo;
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
    vector<CDbNetplaneVlanRange>::iterator it = vInfo.begin();
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

void db_query_np_vlanrange(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneVlanRange * pOper = GetDbINetplaneVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanRange is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneVlanRange> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneVlanRange>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_np_vlanrange);

void db_query_np_vlanrange_alloc(char* uuid, int32 begin, int32 end )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanRange * pOper = GetDbINetplaneVlanRange();
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
DEFINE_DEBUG_FUNC(db_query_np_vlanrange_alloc);


void db_query_np_vmshare_vlans(char* uuid )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanRange * pOper = GetDbINetplaneVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetworkIpRange is NULL" << endl;
        return ;
    }


    //info.DbgShow();
    string vVlans;
    int ret = pOper->QueryVmShareVlans(uuid,vVlans);
    if( ret != 0 )
    {
        cout << "QueryVmShareVlans failed." << ret << endl;
        return;
    }
    cout << "QueryVmShareVlans success."  << vVlans << endl;

}
DEFINE_DEBUG_FUNC(db_query_np_vmshare_vlans);

void db_check_add_np_vlanrange(char* uuid, int32 begin, int32 end )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanRange * pOper = GetDbINetplaneVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanRange is NULL" << endl;
        return ;
    }

    CDbNetplaneVlanRange info;
    info.SetUuid(uuid);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
  

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
DEFINE_DEBUG_FUNC(db_check_add_np_vlanrange);

void db_add_np_vlanrange(char* uuid, int32 begin, int32 end )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanRange * pOper = GetDbINetplaneVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanRange is NULL" << endl;
        return ;
    }

    CDbNetplaneVlanRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);

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
DEFINE_DEBUG_FUNC(db_add_np_vlanrange);

void db_del_np_vlanrange(char * uuid, int32 begin, int32 end )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanRange * pOper = GetDbINetplaneVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanRange is NULL" << endl;
        return ;
    }

    CDbNetplaneVlanRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);

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
DEFINE_DEBUG_FUNC(db_del_np_vlanrange);


}




