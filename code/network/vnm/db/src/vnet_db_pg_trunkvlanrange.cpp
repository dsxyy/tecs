

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_pg_trunkvlanrange.h"
    
namespace zte_tecs
{
typedef enum tagPgTrunkVlanRangeProc
{
    EN_PG_TRUNKVLANRANGE_PROC_QUERY = 0,     
    EN_PG_TRUNKVLANRANGE_PROC_ADD,
    EN_PG_TRUNKVLANRANGE_PROC_MODIFY,
    EN_PG_TRUNKVLANRANGE_PROC_DEL,
    EN_PG_TRUNKVLANRANGE_PROC_ALL,
}EN_PG_TRUNKVLANRANGE_PROC;

const char *s_astrPgTrunkVlanRangeProc[EN_PG_TRUNKVLANRANGE_PROC_ALL] = 
{        
    "pf_net_query_pgtrunkvlanrange",
    "pf_net_add_pgtrunkvlanrange",    
    "pf_net_modify_pgtrunkvlanrange",  
    "pf_net_del_pgtrunkvlanrange",  
};

    
CDBOperatePgTrunkVlanRange::CDBOperatePgTrunkVlanRange(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePgTrunkVlanRange::~CDBOperatePgTrunkVlanRange()
{
    
}

int32 CDBOperatePgTrunkVlanRange::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PG_TRUNKVLANRANGE_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PG_TRUNKVLANRANGE_PROC_ADD:
        case EN_PG_TRUNKVLANRANGE_PROC_MODIFY:
        case EN_PG_TRUNKVLANRANGE_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePgTrunkVlanRange::Query(const char* vna_uuid,vector<CDbPgTrunkVlanRange> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Query vna_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)vna_uuid);
    vInParam.push_back(inVnaUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPgTrunkVlanRangeProc[EN_PG_TRUNKVLANRANGE_PROC_QUERY],\
        vInParam, \
        this,\
        EN_PG_TRUNKVLANRANGE_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Query(%s) IProc->Handle failed(%d) \n",vna_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePgTrunkVlanRange::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPgTrunkVlanRange> *pvOut;
    pvOut = static_cast<vector<CDbPgTrunkVlanRange>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPgTrunkVlanRange info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
    info.SetPgUuid(prs->GetFieldValue(0));
    
    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVlanBegin(value);

    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVlanEnd(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperatePgTrunkVlanRange::Add(CDbPgTrunkVlanRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inVlanBegin(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanBegin()); 
    CADOParameter inVlanEnd(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanEnd()); 

    vInParam.push_back(inUuid);
    vInParam.push_back(inVlanBegin);
    vInParam.push_back(inVlanEnd);
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPgTrunkVlanRangeProc[EN_PG_TRUNKVLANRANGE_PROC_ADD],
        vInParam, 
        this,
        EN_PG_TRUNKVLANRANGE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Add IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        info.SetPgUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePgTrunkVlanRange::Modify(CDbPgTrunkVlanRange & info, int32 old_vlan_begin, int32 old_vlan_end)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inVlanBegin(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanBegin()); 
    CADOParameter inVlanEnd(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanEnd()); 
    CADOParameter inOldVlanBegin(CADOParameter::paramInt, CADOParameter::paramInput, old_vlan_begin); 
    CADOParameter inOldVlanEnd(CADOParameter::paramInt, CADOParameter::paramInput, old_vlan_end); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inOldVlanBegin);
    vInParam.push_back(inOldVlanEnd);
    vInParam.push_back(inVlanBegin);
    vInParam.push_back(inVlanEnd);
 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPgTrunkVlanRangeProc[EN_PG_TRUNKVLANRANGE_PROC_MODIFY],
        vInParam, 
        this,
        EN_PG_TRUNKVLANRANGE_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePgTrunkVlanRange::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePgTrunkVlanRange::Del(CDbPgTrunkVlanRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inVlanBegin(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanBegin()); 
    CADOParameter inVlanEnd(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanEnd()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVlanBegin);
    vInParam.push_back(inVlanEnd);
    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPgTrunkVlanRangeProc[EN_PG_TRUNKVLANRANGE_PROC_DEL],
        vInParam, 
        this,
        EN_PG_TRUNKVLANRANGE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePgTrunkVlanRange::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePgTrunkVlanRange::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePgTrunkVlanRange::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbPgTrunkVlanRange> vInfo;
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
    vector<CDbPgTrunkVlanRange>::iterator it = vInfo.begin();
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
void db_query_pgtrunkvlanrange(const char* pg_uuid)
{
    if( NULL == pg_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePgTrunkVlanRange * pOper = GetDbIPgTrunkVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperatePgTrunkVlanRange is NULL" << endl;
        return ;
    }

    vector<CDbPgTrunkVlanRange> outVInfo;
    int ret = pOper->Query(pg_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbPgTrunkVlanRange>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_pgtrunkvlanrange);


void db_add_pgtrunkvlanrange(char* pg_uuid,int32 vlan_begin, int32 vlan_end)
{
    if( NULL == pg_uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePgTrunkVlanRange * pOper = GetDbIPgTrunkVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperatePgTrunkVlanRange is NULL" << endl;
        return ;
    }

    CDbPgTrunkVlanRange info;
    info.SetPgUuid(pg_uuid);
    info.SetVlanBegin(vlan_begin);
    info.SetVlanEnd(vlan_end);
    
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
DEFINE_DEBUG_FUNC(db_add_pgtrunkvlanrange);

void db_mod_pgtrunkvlanrange(char* pg_uuid,int32 old_begin, int32 old_end, int32 new_begin,int32 new_end)
{
    if( NULL == pg_uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePgTrunkVlanRange * pOper = GetDbIPgTrunkVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperatePgTrunkVlanRange is NULL" << endl;
        return ;
    }

    CDbPgTrunkVlanRange info;
    info.SetPgUuid(pg_uuid);
    info.SetVlanBegin(new_begin);
    info.SetVlanEnd(new_end);
        
    //info.DbgShow();
    int ret = pOper->Modify(info,old_begin,old_end);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_pgtrunkvlanrange);

void db_del_pgtrunkvlanrange(char* pg_uuid,int32 vlan_begin, int32 vlan_end)
{
    if(pg_uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePgTrunkVlanRange * pOper = GetDbIPgTrunkVlanRange();
    if( NULL == pOper )
    {
        cout << "CDBOperatePgTrunkVlanRange is NULL" << endl;
        return ;
    }
    
    CDbPgTrunkVlanRange info;
    info.SetPgUuid(pg_uuid);
    info.SetVlanBegin(vlan_begin);
    info.SetVlanEnd(vlan_end);

    int ret = pOper->Del(info);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_pgtrunkvlanrange);


}

