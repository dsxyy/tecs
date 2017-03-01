

#include "vnet_db_vna.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagVnaProc
{
    EN_VNA_PROC_QUERYSUMMARY = 0,
    EN_VNA_PROC_QUERY,     
    EN_VNA_PROC_ADD,
    EN_VNA_PROC_MODIFY,
    EN_VNA_PROC_DEL,
    EN_VNA_PROC_ALL,
}EN_VNA_PROC;

const char *s_astrVnaProc[EN_VNA_PROC_ALL] = 
{        
    "pf_net_query_vna_summary",
    "pf_net_query_vna",
    "pf_net_add_vna",    
    "pf_net_modify_vna",  
    "pf_net_del_vna",  
};

    
CDBOperateVna::CDBOperateVna(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVna::~CDBOperateVna()
{
    
}

int32 CDBOperateVna::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VNA_PROC_QUERYSUMMARY:
        {
            return QueryVnaSummaryCallback(prs,nil);
        }
        break;
        case EN_VNA_PROC_QUERY:
        {
            return QueryVnaCallback(prs,nil);
        }
        break;
        case EN_VNA_PROC_ADD:
        case EN_VNA_PROC_MODIFY:
        case EN_VNA_PROC_DEL:
        {
            return OperateVnaCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateVna::QueryVnaSummary(vector<CDbVnaSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::QueryVnaSummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVnaProc[EN_VNA_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_VNA_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::QueryVnaSummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
     
    return 0;
}

int32 CDBOperateVna::QueryVnaSummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::QueryVnaSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVnaSummary> *pvOut;
    pvOut = static_cast<vector<CDbVnaSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::QueryVnaSummaryCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    CDbVnaSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);
    info.SetUuid(prs->GetFieldValue(0));
    info.SetApp(prs->GetFieldValue(1));

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateVna::QueryVna(CDbVna & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::QueryVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaProc[EN_VNA_PROC_QUERY],
        vInParam, 
        this,
        EN_VNA_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::QueryVna IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVna::QueryVnaCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::QueryVnaCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbVna *pOutVna;
    pOutVna = static_cast<CDbVna *>(nil);

    if( pOutVna == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::QueryVnaCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 7);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::QueryVnaCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::QueryVnaCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutVna).SetUuid( prs->GetFieldValue(1));
    (*pOutVna).SetVnaApp( prs->GetFieldValue(2));
    (*pOutVna).SetVnmApp( prs->GetFieldValue(3));    
    (*pOutVna).SetHostName( prs->GetFieldValue(4));  
    (*pOutVna).SetOsName( prs->GetFieldValue(5));      
    int32 iscfg = 0;
    if( false == prs->GetFieldValue(6,iscfg) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::QueryVnaCallback(6) GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutVna).SetIsOnline(iscfg);

    return 0;
}

int32 CDBOperateVna::AddVna(CDbVna & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::AddVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVnaApp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaApp().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnmApp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnmApp().c_str());    
    CADOParameter inHostName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetHostName().c_str());    
    CADOParameter inOsName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetOsName().c_str());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline());    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaApp);
    vInParam.push_back(inVnmApp);
    vInParam.push_back(inHostName);
    vInParam.push_back(inOsName);    
    vInParam.push_back(inIsOnline);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaProc[EN_VNA_PROC_ADD],
        vInParam, 
        this,
        EN_VNA_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::AddVna IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::AddVna proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateVna::ModifyVna(CDbVna & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::ModifyVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVnaApp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaApp().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVnmApp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnmApp().c_str());    
    CADOParameter inHostName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetHostName().c_str());    
    CADOParameter inOsName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetOsName().c_str());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline());    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaApp);
    vInParam.push_back(inVnmApp);
    vInParam.push_back(inHostName);
    vInParam.push_back(inOsName);    
    vInParam.push_back(inIsOnline);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaProc[EN_VNA_PROC_MODIFY],
        vInParam, 
        this,
        EN_VNA_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::ModifyVna IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVna::ModifyVna proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateVna::DelVna(const char* vna_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::DelVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::DelVna vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVnaProc[EN_VNA_PROC_DEL],
        vInParam, 
        this,
        EN_VNA_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::DelVna(%s) IProc->Handle failed. ret : %d.\n",
            vna_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::DelVna(%s) proc return failed. ret : %d.\n", 
            vna_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVna::OperateVnaCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::OperateVnaCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::OperateVnaCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::OperateVnaCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVna::OperateVnaCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperateVna::DbgShow()
{
    cout << "--------------------------vna Info------------------------" << endl;
    // query summary
    vector<CDbVnaSummary> vInfo;
    int ret = 0;
    ret = QueryVnaSummary(vInfo);
    if( ret != 0 )
    {
        cout << "QueryVnaSummary failed. ret : " << ret << endl;
        return;
    }

    cout << "QueryVnaSummary success " << endl;
    cout << "|" <<setw(32) << "uuid" << 
                    "|" << setw(32) << "name" << endl;
    vector<CDbVnaSummary>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
    // query 
    cout << "-----------------------------------" << endl;
    it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        CDbVna info;
        info.SetUuid((*it).GetUuid().c_str());
        ret = QueryVna(info);
        if( ret != 0 )
        {
            cout << "QueryVna failed. ret : " << ret << endl;
            continue;
        }

        info.DbgShow();
        cout << "-----------------" << endl;        
    }
}

/* test code ------------------------------------------------------------

*/
void db_query_vnasummary()
{
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperateVna * pOper = pMgr->GetIVna();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    vector<CDbVnaSummary> outVInfo;
    int ret = pOper->QueryVnaSummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QueryVnaSummary failed." << ret << endl;
        return;
    }
    cout << "QueryVnaSummary success." << endl;
    vector<CDbVnaSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vnasummary);

void db_query_vna(char * uuid)
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
    CDBOperateVna * pOper = pMgr->GetIVna();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    CDbVna info;
    info.SetUuid(uuid);
    int ret = pOper->QueryVna(info);
    if( ret != 0 )
    {
        cout << "CDBOperateVna failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVna success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vna);

void db_add_vna(char* vna_app, char* vnm_app, char* host_name,char *os_name,int32 is_online)
{
    if( NULL == vna_app || NULL == vnm_app || NULL == host_name ||NULL == os_name )
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
    CDBOperateVna * pOper = pMgr->GetIVna();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    CDbVna info;
    info.SetUuid(GenerateUUID().c_str());
    info.SetVnaApp(vna_app);
    info.SetVnmApp(vnm_app);
    info.SetHostName(host_name);
    info.SetOsName(os_name);
    info.SetIsOnline(is_online);
    int ret = pOper->AddVna(info);
    if( ret != 0 )
    {
        cout << "AddVna failed." << ret << endl;
        return;
    }
    cout << "AddVna success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_vna);

void db_mod_vna(char * uuid,char* vna_app, char* vnm_app, char* host_name,char *os_name,int32 is_online)
{
    if( NULL == uuid || NULL == vna_app || NULL == vnm_app || NULL == host_name ||NULL == os_name )
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
    CDBOperateVna * pOper = pMgr->GetIVna();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    CDbVna info;
    info.SetUuid(uuid);
    info.SetVnaApp(vna_app);
    info.SetVnmApp(vnm_app);
    info.SetHostName(host_name);
    info.SetOsName(os_name);
    info.SetIsOnline(is_online);
    
    int ret = pOper->ModifyVna(info);
    if( ret != 0 )
    {
        cout << "ModifyVna failed." << ret << endl;
        return;
    }
    cout << "ModifyVna success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_vna);

void db_del_vna(char * uuid)
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
    CDBOperateVna * pOper = pMgr->GetIVna();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    int ret = pOper->DelVna(uuid);
    if( ret != 0 )
    {
        cout << "DelVna failed." << ret << endl;
        return;
    }
    cout << "DelVna success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_vna);

}
 // zte_tecs

