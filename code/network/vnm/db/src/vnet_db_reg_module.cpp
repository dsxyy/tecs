

#include "vnet_db_reg_module.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagVnaRegModuleProc
{
    EN_VNA_REG_MODULE_PROC_QUERYSUMMARY = 0,
    EN_VNA_REG_MODULE_PROC_QUERY,     
    EN_VNA_REG_MODULE_PROC_ADD,
    EN_VNA_REG_MODULE_PROC_MODIFY,
    EN_VNA_REG_MODULE_PROC_DEL,
    EN_VNA_REG_MODULE_PROC_ALL,
}EN_VNA_REG_MODULE_PROC;

const char *s_astrVnaRegModuleProc[EN_VNA_REG_MODULE_PROC_ALL] = 
{        
    "pf_net_query_vna_reg_module_summary",
    "pf_net_query_vna_reg_module",
    "pf_net_add_vna_reg_module",    
    "pf_net_modify_vna_reg_module",  
    "pf_net_del_vna_reg_module",  
};

CDBOperateVnaRegModule::CDBOperateVnaRegModule(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVnaRegModule::~CDBOperateVnaRegModule()
{
    
}

int32 CDBOperateVnaRegModule::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VNA_REG_MODULE_PROC_QUERYSUMMARY:
        {
            return QueryRegModuleSummaryCallback(prs,nil);
        }
        break;
        case EN_VNA_REG_MODULE_PROC_QUERY:
        {
            return QueryRegModuleCallback(prs,nil);
        }
        break;
        case EN_VNA_REG_MODULE_PROC_ADD:
        case EN_VNA_REG_MODULE_PROC_MODIFY:
        case EN_VNA_REG_MODULE_PROC_DEL:
        {
            return OperateRegModuleCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateVnaRegModule::QueryRegModuleSummary(vector<CDbVnaRegModuleSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::QueryRegModuleSummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVnaRegModuleProc[EN_VNA_REG_MODULE_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_VNA_REG_MODULE_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::QueryRegModuleSummary IProc->Handle failed(%d)\n ",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVnaRegModule::QueryRegModuleSummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::QueryRegModuleSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVnaRegModuleSummary> *pvOut;
    pvOut = static_cast<vector<CDbVnaRegModuleSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleSummaryCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    CDbVnaRegModuleSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetModuleUuid(prs->GetFieldValue(1));
    info.SetModuleName(prs->GetFieldValue(2));    

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateVnaRegModule::QueryRegModule(CDbVnaRegistModule & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModule IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetModuleUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaRegModuleProc[EN_VNA_REG_MODULE_PROC_QUERY],
        vInParam, 
        this,
        EN_VNA_REG_MODULE_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::QueryRegModule IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVnaRegModule::QueryRegModuleCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::QueryRegModuleCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbVnaRegistModule *pOutVna;
    pOutVna = static_cast<CDbVnaRegistModule *>(nil);

    if( pOutVna == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 8);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutVna).SetVnaUuid( prs->GetFieldValue(1));
    (*pOutVna).SetModuleUuid( prs->GetFieldValue(2));
    (*pOutVna).SetModuleName( prs->GetFieldValue(3));  

    int32 nRole = 0;
    if( false == prs->GetFieldValue(4,nRole ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleCallback GetFieldValue(4) role failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }     
    (*pOutVna).SetRole( nRole); 

    if( nRole == 1 )
    {
        (*pOutVna).SetIsHasHcInfo( true); 
    }

    int32 nIsOnline = 0;
    if( false == prs->GetFieldValue(5,nIsOnline ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleCallback GetFieldValue(5) is_online failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    } 
    (*pOutVna).SetIsOnline(nIsOnline);   

    int32 nIsHasHc = 0;
    if( false == prs->GetFieldValue(6,nIsHasHc ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::QueryRegModuleCallback GetFieldValue(6) is_online failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    } 
    (*pOutVna).SetIsHasHcInfo(nIsHasHc);  
    
    (*pOutVna).SetClusterName( prs->GetFieldValue(7));  
    
    return 0;
}

int32 CDBOperateVnaRegModule::AddRegModule(CDbVnaRegistModule & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::AddRegModule IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetModuleUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inModuleUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetModuleUuid().c_str());    
    CADOParameter inModuleName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetModuleName().c_str());    
    CADOParameter inRole(CADOParameter::paramInt, CADOParameter::paramInput, info.GetRole());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline());    
    CADOParameter inIsHasHc(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasHcInfo());    
    CADOParameter inClusterName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetClusterName().c_str());    
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inModuleUuid);
    vInParam.push_back(inModuleName);
    vInParam.push_back(inRole);
    vInParam.push_back(inIsOnline);    
    vInParam.push_back(inIsHasHc);     
    vInParam.push_back(inClusterName);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaRegModuleProc[EN_VNA_REG_MODULE_PROC_ADD],
        vInParam, 
        this,
        EN_VNA_REG_MODULE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::AddRegModule IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        info.SetModuleUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::AddRegModule proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateVnaRegModule::ModifyRegModule(CDbVnaRegistModule & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::ModifyRegModule IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());
    CADOParameter inModuleUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetModuleUuid().c_str());    
    CADOParameter inModuleName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetModuleName().c_str());    
    CADOParameter inRole(CADOParameter::paramInt, CADOParameter::paramInput, info.GetRole());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline());    
    CADOParameter inIsHasHc(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasHcInfo());    
    CADOParameter inClusterName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetClusterName().c_str());    
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inModuleUuid);
    vInParam.push_back(inModuleName);
    vInParam.push_back(inRole);
    vInParam.push_back(inIsOnline);    
    vInParam.push_back(inIsHasHc);     
    vInParam.push_back(inClusterName);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaRegModuleProc[EN_VNA_REG_MODULE_PROC_MODIFY],
        vInParam, 
        this,
        EN_VNA_REG_MODULE_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::ModifyRegModule IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVnaRegModule::ModifyRegModule proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateVnaRegModule::DelRegModule(const char* module_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::DelRegModule IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == module_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::DelRegModule vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)module_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaRegModuleProc[EN_VNA_REG_MODULE_PROC_DEL],
        vInParam, 
        this,
        EN_VNA_REG_MODULE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::DelRegModule(%s) IProc->Handle failed. ret : %d.\n", 
            module_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::DelRegModule(%s) proc return failed. ret : %d.\n", 
            module_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVnaRegModule::OperateRegModuleCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::OperateRegModuleCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::OperateRegModuleCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::OperateRegModuleCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVnaRegModule::OperateRegModuleCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperateVnaRegModule::DbgShow()
{
    cout << "--------------------------vna Info------------------------" << endl;
    // query summary
    vector<CDbVnaRegModuleSummary> vInfo;
    int ret = 0;
    ret = QueryRegModuleSummary(vInfo);
    if( ret != 0 )
    {
        cout << "QueryRegModuleSummary failed. ret : " << ret << endl;
        return;
    }

    cout << "QueryRegModuleSummary success " << endl;
    cout << "|" <<setw(32) << "uuid" << 
                    "|" << setw(32) << "name" << endl;
    vector<CDbVnaRegModuleSummary>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
    // query 
    cout << "-----------------------------------" << endl;
    it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        CDbVnaRegistModule info;
        info.SetModuleUuid((*it).GetModuleUuid().c_str());
        ret = QueryRegModule(info);
        if( ret != 0 )
        {
            cout << "QueryVnaRegModule failed. ret : " << ret << endl;
            continue;
        }

        info.DbgShow();
        cout << "-----------------" << endl;        
    }
}

/* test code ------------------------------------------------------------

*/
void db_query_vnaregmodulesummary()
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVnaRegModule * pOper = pMgr->GetIVnaRegModule();
    if( NULL == pOper )
    {
        cout << "CDBOperateVnaRegModule is NULL" << endl;
        return ;
    }

    vector<CDbVnaRegModuleSummary> outVInfo;
    int ret = pOper->QueryRegModuleSummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QueryRegModuleSummary failed." << ret << endl;
        return;
    }
    cout << "QueryRegModuleSummary success." << endl;
    vector<CDbVnaRegModuleSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vnaregmodulesummary);

void db_query_vnaregmodule(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVnaRegModule * pOper = pMgr->GetIVnaRegModule();
    if( NULL == pOper )
    {
        cout << "CDBOperateVnaRegModule is NULL" << endl;
        return ;
    }

    CDbVnaRegistModule info;
    info.SetModuleUuid(uuid);
    int ret = pOper->QueryRegModule(info);
    if( ret != 0 )
    {
        cout << "QueryRegModule failed." << ret << endl;
        return;
    }
    cout << "QueryRegModule success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vnaregmodule);

void db_add_vnaregmodule(char* vna_uuid, char* module_name,int32 role,
    int32 is_online, int32 is_has_cluster,char * cluster_name)
{
    if( NULL == vna_uuid || NULL == module_name ||NULL == cluster_name )
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
    CDBOperateVnaRegModule * pOper = pMgr->GetIVnaRegModule();
    if( NULL == pOper )
    {
        cout << "CDBOperateVnaRegModule is NULL" << endl;
        return ;
    }

    CDbVnaRegistModule info;
    info.SetVnaUuid(vna_uuid);
    info.SetModuleName(module_name);
    //info.SetModuleUuid(module_uuid);
    info.SetRole(role);
    info.SetIsOnline(is_online);
    info.SetIsHasHcInfo(is_has_cluster);    
    info.SetClusterName(cluster_name);
    int ret = pOper->AddRegModule(info);
    if( ret != 0 )
    {
        cout << "AddRegModule failed." << ret << endl;
        return;
    }
    cout << "AddRegModule success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_vnaregmodule);

void db_mod_vnaregmodule(char * vna_uuid, char* module_uuid, char* module_name,
      int32 role,int32 is_online, int32 is_has_cluster, char * cluster_name)
{
    if( NULL == vna_uuid || NULL == module_uuid || NULL == module_name ||NULL == cluster_name )
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
    CDBOperateVnaRegModule * pOper = pMgr->GetIVnaRegModule();
    if( NULL == pOper )
    {
        cout << "CDBOperateVnaRegModule is NULL" << endl;
        return ;
    }

    CDbVnaRegistModule info;
    info.SetVnaUuid(vna_uuid);
    info.SetModuleName(module_name);
    info.SetModuleUuid(module_uuid);
    info.SetRole(role);
    info.SetIsOnline(is_online);
    info.SetIsHasHcInfo(is_has_cluster); 
    info.SetClusterName(cluster_name);
    
    int ret = pOper->ModifyRegModule(info);
    if( ret != 0 )
    {
        cout << "ModifyRegModule failed." << ret << endl;
        return;
    }
    cout << "ModifyRegModule success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_vnaregmodule);

void db_del_vnaregmodule(char * uuid)
{
    if(uuid == NULL )
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
    CDBOperateVnaRegModule * pOper = pMgr->GetIVnaRegModule();
    if( NULL == pOper )
    {
        cout << "CDBOperateVnaRegModule is NULL" << endl;
        return ;
    }

    int ret = pOper->DelRegModule(uuid);
    if( ret != 0 )
    {
        cout << "DelRegModule failed." << ret << endl;
        return;
    }
    cout << "DelRegModule success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_vnaregmodule);

}
 // zte_tecs

