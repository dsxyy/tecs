

#include "vnet_db_vm.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagVmProc
{
    EN_VM_PROC_QUERYSUMMARY = 0,
    EN_VM_PROC_QUERY,     
    EN_VM_PROC_ADD,
    EN_VM_PROC_MODIFY,    
    EN_VM_PROC_DEL,
    EN_VM_PROC_ALL,
}EN_VM_PROC;

const char *s_astrVmProc[EN_VM_PROC_ALL] = 
{        
    "pf_net_query_vm_summary",
    "pf_net_query_vm",
    "pf_net_add_vm",    
    "pf_net_modify_vm",    
    "pf_net_del_vm",  
};

    
CDBOperateVm::CDBOperateVm(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVm::~CDBOperateVm()
{
    
}

int32 CDBOperateVm::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VM_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_VM_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_VM_PROC_ADD:
        case EN_VM_PROC_MODIFY:
        case EN_VM_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateVm::QuerySummary(vector<CDbVm> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVmProc[EN_VM_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_VM_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
     
    return 0;
}

int32 CDBOperateVm::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVm> *pvOut;
    pvOut = static_cast<vector<CDbVm>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    CDbVm info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 8);
    
    int64 oValue = 0;
    if( false == prs->GetFieldValue(0,oValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetVmId(oValue);

    if( false == prs->GetFieldValue(1,oValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetProjectId(oValue);

    int32 nValue = 0;
    if( false == prs->GetFieldValue(2,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetIsEnableWdg(nValue);

    if( false == prs->GetFieldValue(3,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback GetFieldValue(2) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetWdgTimeout(nValue);

    if( false == prs->GetFieldValue(4,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback GetFieldValue(3) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetIsHasCluster(nValue);
    
    info.SetClusterName(prs->GetFieldValue(5));

    if( false == prs->GetFieldValue(6,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QuerySummaryCallback GetFieldValue(5) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    info.SetIsHasHost(nValue);
    
    info.SetHostName(prs->GetFieldValue(7));

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateVm::Query(CDbVm & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, Info.GetVmId());
    vInParam.push_back(inVmId);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmProc[EN_VM_PROC_QUERY],
        vInParam, 
        this,
        EN_VM_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVm::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbVm *pOut;
    pOut = static_cast<CDbVm *>(nil);

    if( pOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback pOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 9);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    int64 oValue = 0;
    if( false == prs->GetFieldValue(1,oValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetVmId(oValue);

    if( false == prs->GetFieldValue(2,oValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetProjectId(oValue);
    
    int32 nValue = 0;
    if( false == prs->GetFieldValue(3,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(2) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetIsEnableWdg(nValue);

    if( false == prs->GetFieldValue(4,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(3) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetWdgTimeout(nValue);

    if( false == prs->GetFieldValue(5,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(4) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetIsHasCluster(nValue);
    
    (*pOut).SetClusterName(prs->GetFieldValue(6));

    if( false == prs->GetFieldValue(7,nValue ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::QueryCallback GetFieldValue(6) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    (*pOut).SetIsHasHost(nValue);
    
    (*pOut).SetHostName(prs->GetFieldValue(8));

    return 0;
}

int32 CDBOperateVm::Add(CDbVm & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetProjectId());
    CADOParameter inIsWdg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsEnableWdg());    
    CADOParameter inWdgTimeout(CADOParameter::paramInt, CADOParameter::paramInput, info.GetWdgTimeout());    
    CADOParameter inIsHasCluster(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasCluster());    
    CADOParameter inCluster(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetClusterName().c_str());    
    CADOParameter inIsHasHost(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasHost());    
    CADOParameter inHost(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetHostName().c_str());    
    
    vInParam.push_back(inVmId);
    vInParam.push_back(inProjectId);
    vInParam.push_back(inIsWdg);
    vInParam.push_back(inWdgTimeout);
    vInParam.push_back(inIsHasCluster);
    vInParam.push_back(inCluster);    
    vInParam.push_back(inIsHasHost);    
    vInParam.push_back(inHost);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmProc[EN_VM_PROC_ADD],
        vInParam, 
        this,
        EN_VM_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateVm::Modify(CDbVm & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    
    CADOParameter inVmId(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetProjectId());
    CADOParameter inIsWdg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsEnableWdg());    
    CADOParameter inWdgTimeout(CADOParameter::paramInt, CADOParameter::paramInput, info.GetWdgTimeout());    
    CADOParameter inIsHasCluster(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasCluster());    
    CADOParameter inCluster(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetClusterName().c_str());    
    CADOParameter inIsHasHost(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasHost());    
    CADOParameter inHost(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetHostName().c_str());    
    
    vInParam.push_back(inVmId);
    vInParam.push_back(inProjectId);
    vInParam.push_back(inIsWdg);
    vInParam.push_back(inWdgTimeout);
    vInParam.push_back(inIsHasCluster);
    vInParam.push_back(inCluster);    
    vInParam.push_back(inIsHasHost);    
    vInParam.push_back(inHost);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVmProc[EN_VM_PROC_MODIFY],
        vInParam, 
        this,
        EN_VM_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVm::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVm::Del(int64 vm_id)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::Del IProc is NULL.\n");
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
        s_astrVmProc[EN_VM_PROC_DEL],
        vInParam, 
        this,
        EN_VM_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::Del(%lld) IProc->Handle failed. ret : %d.\n", 
            vm_id,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::Del(%lld) proc return failed. ret : %d.\n", 
            vm_id,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVm::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVm::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperateVm::DbgShow()
{
    
}

/* test code ------------------------------------------------------------

*/
void db_query_vm_summary()
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVm * pOper = pMgr->GetIVm();
    if( NULL == pOper )
    {
        cout << "CDBOperateVm is NULL" << endl;
        return ;
    }

    vector<CDbVm> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbVm>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vm_summary);

void db_query_vm(int64 vmid)
{    
    CDBOperateVm * pOper = GetDbIVm();
    if( NULL == pOper )
    {
        cout << "CDBOperateVm is NULL" << endl;
        return ;
    }

    CDbVm info;
    info.SetVmId(vmid);
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateVm Query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVm Query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vm);

void db_add_vm(int64 vmid, int32 is_wdg, int32 wdg_timeout, int32 is_has_cluster,
   char* cluster_name, int32 is_has_host,char* host_name)
{
    if( NULL == cluster_name || NULL == host_name  )
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
    CDBOperateVm * pOper = pMgr->GetIVm();
    if( NULL == pOper )
    {
        cout << "CDBOperateVm is NULL" << endl;
        return ;
    }

    CDbVm info;
    info.SetVmId(vmid);
    info.SetIsEnableWdg(is_wdg);
    info.SetWdgTimeout(wdg_timeout);
    info.SetIsHasCluster(is_has_cluster);
    info.SetClusterName(cluster_name);
    info.SetIsHasHost(is_has_host);
    info.SetHostName(host_name);
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_vm);


void db_modify_vm(int64 vmid, int32 is_wdg, int32 wdg_timeout, int32 is_has_cluster,
   char* cluster_name, int32 is_has_host,char* host_name)
{
    if( NULL == cluster_name || NULL == host_name  )
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
    CDBOperateVm * pOper = pMgr->GetIVm();
    if( NULL == pOper )
    {
        cout << "CDBOperateVm is NULL" << endl;
        return ;
    }

    CDbVm info;
    info.SetVmId(vmid);
    info.SetIsEnableWdg(is_wdg);
    info.SetWdgTimeout(wdg_timeout);
    info.SetIsHasCluster(is_has_cluster);
    info.SetClusterName(cluster_name);
    info.SetIsHasHost(is_has_host);
    info.SetHostName(host_name);
    int ret = pOper->Modify(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_modify_vm);

void db_del_vm(int64 vmid)
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVm * pOper = pMgr->GetIVm();
    if( NULL == pOper )
    {
        cout << "CDBOperateVm is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_vm);

}
 // zte_tecs

