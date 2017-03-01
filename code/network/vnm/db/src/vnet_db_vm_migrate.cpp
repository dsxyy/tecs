

#include "vnet_db_vm_migrate.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagVmMigrateProc
{
    EN_VM_MIGRATE_PROC_QUERYSUMMARY = 0,
    EN_VM_MIGRATE_PROC_QUERY,     
    EN_VM_MIGRATE_PROC_ADD,
    EN_VM_MIGRATE_PROC_MODIFY,
    EN_VM_MIGRATE_PROC_DEL,
    EN_VM_MIGRATE_PROC_ALL,
}EN_VM_MIGRATE_PROC;

const char *s_astrVmMigrateProc[EN_VM_MIGRATE_PROC_ALL] = 
{        
    "pf_net_query_vm_migrate_summary",
    "pf_net_query_vm_migrate",
    "pf_net_add_vm_migrate",   
    "pf_net_modify_vm_migrate",  
    "pf_net_del_vm_migrate",  
};

    
CDBOperateVmMigrate::CDBOperateVmMigrate(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVmMigrate::~CDBOperateVmMigrate()
{
    
}

int32 CDBOperateVmMigrate::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VM_MIGRATE_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_VM_MIGRATE_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_VM_MIGRATE_PROC_ADD:
        case EN_VM_MIGRATE_PROC_MODIFY:
        case EN_VM_MIGRATE_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    

int32 CDBOperateVmMigrate::QuerySummary(vector<CDbVmMigrate> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVmMigrateProc[EN_VM_MIGRATE_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_VM_MIGRATE_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
     
    return 0;
}

int32 CDBOperateVmMigrate::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVmMigrate> *pvOut;
    pvOut = static_cast<vector<CDbVmMigrate>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QuerySummaryCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    CDbVmMigrate info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 6);
    
    int64 oValue = 0;
    if( false == prs->GetFieldValue(0,oValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QuerySummaryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetVmId(oValue);
    info.SetSrcClusterName(prs->GetFieldValue(1));
    info.SetSrcHostName(prs->GetFieldValue(2));
    info.SetDstClusterName(prs->GetFieldValue(3));
    info.SetDstHostName(prs->GetFieldValue(4));

    int32 nValue = 0;
    if( false == prs->GetFieldValue(5,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QuerySummaryCallback GetFieldValue(5) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetState(nValue);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateVmMigrate::Query(CDbVmMigrate & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, Info.GetVmId());
    vInParam.push_back(inVmId);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmMigrateProc[EN_VM_MIGRATE_PROC_QUERY],
        vInParam, 
        this,
        EN_VM_MIGRATE_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVmMigrate::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbVmMigrate *pOut;
    pOut = static_cast<CDbVmMigrate *>(nil);

    if( pOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QueryCallback pOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 7);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    int64 oValue = 0;
    if( false == prs->GetFieldValue(1,oValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QuerySummaryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetVmId(oValue);
    
    (*pOut).SetSrcClusterName(prs->GetFieldValue(2));
    (*pOut).SetSrcHostName(prs->GetFieldValue(3));
    (*pOut).SetDstClusterName(prs->GetFieldValue(4));
    (*pOut).SetDstHostName(prs->GetFieldValue(5));

    int32 nValue = 0;
    if( false == prs->GetFieldValue(6,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::QuerySummaryCallback GetFieldValue(5) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetState(nValue);

    return 0;
}

int32 CDBOperateVmMigrate::Add(CDbVmMigrate & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());
    CADOParameter inSrcCluster(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSrcClusterName().c_str());    
    CADOParameter inSrcHost(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSrcHostName().c_str());    
    CADOParameter inDstCluster(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDstClusterName().c_str());    
    CADOParameter inDstHost(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDstHostName().c_str());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    
    vInParam.push_back(inVmId);
    vInParam.push_back(inSrcCluster);
    vInParam.push_back(inSrcHost);
    vInParam.push_back(inDstCluster);
    vInParam.push_back(inDstHost);    
    vInParam.push_back(inState);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmMigrateProc[EN_VM_MIGRATE_PROC_ADD],
        vInParam, 
        this,
        EN_VM_MIGRATE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Add IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        //info.SetUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVmMigrate::Modify(CDbVmMigrate & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());
    CADOParameter inSrcCluster(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSrcClusterName().c_str());    
    CADOParameter inSrcHost(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSrcHostName().c_str());    
    CADOParameter inDstCluster(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDstClusterName().c_str());    
    CADOParameter inDstHost(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDstHostName().c_str());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    
    vInParam.push_back(inVmId);
    vInParam.push_back(inSrcCluster);
    vInParam.push_back(inSrcHost);
    vInParam.push_back(inDstCluster);
    vInParam.push_back(inDstHost);    
    vInParam.push_back(inState);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmMigrateProc[EN_VM_MIGRATE_PROC_MODIFY],
        vInParam, 
        this,
        EN_VM_MIGRATE_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Modify IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        //info.SetUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVmMigrate::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateVmMigrate::Del(int64 vm_id)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, vm_id);    
    
    vInParam.push_back(inVmId);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmMigrateProc[EN_VM_MIGRATE_PROC_DEL],
        vInParam, 
        this,
        EN_VM_MIGRATE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::Del(%lld) IProc->Handle failed. ret : %d.\n",
            vm_id,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::Del(%lld) proc return failed. ret : %d.\n", 
            vm_id,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVmMigrate::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVmMigrate::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperateVmMigrate::DbgShow()
{
    
}

/* test code ------------------------------------------------------------

*/
void db_query_vm_migrate_summary()
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVmMigrate * pOper = pMgr->GetIVmMigrate();
    if( NULL == pOper )
    {
        cout << "CDBOperateVmMigrate is NULL" << endl;
        return ;
    }

    vector<CDbVmMigrate> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbVmMigrate>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vm_migrate_summary);

void db_query_vm_migrate(int64 vmid)
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVmMigrate * pOper = pMgr->GetIVmMigrate();
    if( NULL == pOper )
    {
        cout << "CDBOperateVmMigrate is NULL" << endl;
        return ;
    }

    CDbVmMigrate info;
    info.SetVmId(vmid);
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateVmMigrate Query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVmMigrate Query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vm_migrate);

void db_add_vm_migrate(int64 vmid, char* src_cluster, char* src_host,
   char* dst_cluster, char* dst_host,int32 state)
{
    if( NULL == src_cluster || NULL == src_host || NULL == dst_cluster ||NULL == dst_host )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVmMigrate * pOper = pMgr->GetIVmMigrate();
    if( NULL == pOper )
    {
        cout << "CDBOperateVmMigrate is NULL" << endl;
        return ;
    }

    CDbVmMigrate info;
    info.SetVmId(vmid);
    info.SetSrcClusterName(src_cluster);
    info.SetSrcHostName(src_host);
    info.SetDstClusterName(dst_cluster);
    info.SetDstHostName(dst_host);
    info.SetState(state);
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_vm_migrate);


void db_del_vm_migrate(int64 vmid)
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVmMigrate * pOper = pMgr->GetIVmMigrate();
    if( NULL == pOper )
    {
        cout << "CDBOperateVmMigrate is NULL" << endl;
        return ;
    }

    int ret = pOper->Del(vmid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_vm_migrate);

}
 // zte_tecs

