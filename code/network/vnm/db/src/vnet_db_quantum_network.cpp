

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_quantum_network.h"
    
namespace zte_tecs
{
typedef enum tagQuantumNetworkProc
{
    EN_QUANTUMNETWORK_PROC_CHECK_ADD = 0,
    EN_QUANTUMNETWORK_PROC_ADD,
    EN_QUANTUMNETWORK_PROC_CHECK_DEL,
    EN_QUANTUMNETWORK_PROC_DEL,
    EN_QUANTUMNETWORK_PROC_QUERY,       
    EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN_PROJECT,
    EN_QUANTUMNETWORK_PROC_QUERY_ALL, 
    EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN,    
    EN_QUANTUMNETWORK_PROC_ALL,
}EN_VSWITCH_PROC;

const char *s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_ALL] = 
{        
    "pf_net_check_add_quantum_network",  
    "pf_net_add_quantum_network",    
    "pf_net_check_del_quantum_network",  
    "pf_net_del_quantum_network",  
    
    "pf_net_query_quantumnetwork",
    "pf_net_query_quantumnetwork_by_ln_projectid",
    "pf_net_query_quantumnetwork_all",
    "pf_net_query_quantumnetwork_by_ln",
};

    
CDBOperateQuantumNetwork::CDBOperateQuantumNetwork(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateQuantumNetwork::~CDBOperateQuantumNetwork()
{
    
}

int32 CDBOperateQuantumNetwork::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_QUANTUMNETWORK_PROC_QUERY_ALL:
        case EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN:
        {
            return QueryAllCallback(prs,nil);
        }
        break;
        case EN_QUANTUMNETWORK_PROC_QUERY:
        case EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN_PROJECT:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_QUANTUMNETWORK_PROC_ADD:
        case EN_QUANTUMNETWORK_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_QUANTUMNETWORK_PROC_CHECK_ADD:
        case EN_QUANTUMNETWORK_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateQuantumNetwork::QueryByLn(const char* uuid,vector<CDbQuantumNetwork> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryByNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN],\
        vInParam, \
        this,\
        EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryByLn(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateQuantumNetwork::QueryAll(vector<CDbQuantumNetwork> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryAll IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_QUERY_ALL],\
        vInParam, \
        this,\
        EN_QUANTUMNETWORK_PROC_QUERY_ALL,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryAll() IProc->Handle failed(%d) \n",
                ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateQuantumNetwork::QueryAllCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryAllCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbQuantumNetwork> *pvOut;
    pvOut = static_cast<vector<CDbQuantumNetwork>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::QueryAllCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbQuantumNetwork info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
       
    int64 value = 0;
    
    info.SetLnUuid(prs->GetFieldValue(0));
    info.SetUuid(prs->GetFieldValue(1));
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryAllCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetProjectId(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateQuantumNetwork::Query(CDbQuantumNetwork & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_QUERY],
        vInParam, 
        this,
        EN_QUANTUMNETWORK_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {        
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateQuantumNetwork::QueryByLnProjectid(CDbQuantumNetwork & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetLnUuid().c_str());
    CADOParameter inProjectid(CADOParameter::paramInt64, CADOParameter::paramInput, Info.GetProjectId());    
    vInParam.push_back(inUuid);
    vInParam.push_back(inProjectid);    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN_PROJECT],
        vInParam, 
        this,
        EN_QUANTUMNETWORK_PROC_QUERYS_BY_LN_PROJECT,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {        
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateQuantumNetwork::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbQuantumNetwork *pOutPort;
    pOutPort = static_cast<CDbQuantumNetwork *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSwitchBase::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (4));    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetUuid( prs->GetFieldValue(1));
    (*pOutPort).SetLnUuid( prs->GetFieldValue(2));

    int64 value = 0;
    if( false == prs->GetFieldValue(3,value ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    } 
    (*pOutPort).SetProjectId( value);
    
    return 0;
}

int32 CDBOperateQuantumNetwork::CheckAdd(CDbQuantumNetwork & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    
    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam; 
    CADOParameter inLnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLnUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inProject(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetProjectId());    
    
    vInParam.push_back(inLnUuid);
    vInParam.push_back(inUuid);     
    vInParam.push_back(inProject);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_QUANTUMNETWORK_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateQuantumNetwork::Add(CDbQuantumNetwork & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam; 
    CADOParameter inLnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLnUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inProject(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetProjectId());    
    
    vInParam.push_back(inLnUuid);
    vInParam.push_back(inUuid);     
    vInParam.push_back(inProject); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_ADD],
        vInParam, 
        this,
        EN_QUANTUMNETWORK_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumNetwork::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateQuantumNetwork::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CheckDel uuid is NULL.\n");
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
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_QUANTUMNETWORK_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CheckDel(%s) proc return failed. ret : %d.\n", 
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateQuantumNetwork::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::Del uuid is NULL.\n");
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
        s_astrQuantumNetworkProc[EN_QUANTUMNETWORK_PROC_DEL],
        vInParam, 
        this,
        EN_QUANTUMNETWORK_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateQuantumNetwork::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateQuantumNetwork::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumNetwork::CanOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateQuantumNetwork::DbgShow()
{
}

/* test code ------------------------------------------------------------

*/

void db_query_qn_all()
{
    
    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    vector<CDbQuantumNetwork> vinfo;
    int ret = pOper->QueryAll(vinfo);
    if( ret != 0 )
    {
        cout << "CDBOperateQuantumNetwork query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateQuantumNetwork query success." << endl;
    vector<CDbQuantumNetwork>::iterator it = vinfo.begin();
    for(; it != vinfo.end(); ++it)
    {
        (*it).DbgShow();
        cout << "---------" << endl;
    }
    
}
DEFINE_DEBUG_FUNC(db_query_qn_all);

void db_query_qn_all_by_ln(char* uuid)
{
    if( NULL == uuid)
    {
        cout << "uuid is null" << endl;
        return ;
    }
    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    vector<CDbQuantumNetwork> vinfo;
    int ret = pOper->QueryByLn(uuid,vinfo);
    if( ret != 0 )
    {
        cout << "CDBOperateQuantumNetwork query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateQuantumNetwork query success." << endl;
    vector<CDbQuantumNetwork>::iterator it = vinfo.begin();
    for(; it != vinfo.end(); ++it)
    {
        (*it).DbgShow();
        cout << "---------" << endl;
    }    
}
DEFINE_DEBUG_FUNC(db_query_qn_all_by_ln);

void db_query_qn_by_ln_project(const char* uuid, int64 project_id)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    CDbQuantumNetwork outVInfo;
    outVInfo.SetLnUuid(uuid);
    outVInfo.SetProjectId(project_id);
    int ret = pOper->QueryByLnProjectid(outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    outVInfo.DbgShow();
}
DEFINE_DEBUG_FUNC(db_query_qn_by_ln_project);


void db_query_qn(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    CDbQuantumNetwork info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateQuantumNetwork query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateQuantumNetwork query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_qn);


void db_check_add_qn(char* uuid,char* ln_uuid,int64 project_id)
{
    if( NULL == uuid || NULL == ln_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    CDbQuantumNetwork info;
    info.SetUuid(uuid);
    info.SetLnUuid(ln_uuid);
    info.SetProjectId(project_id);

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
DEFINE_DEBUG_FUNC(db_check_add_qn);

void db_add_qn(char* uuid,char* ln_uuid,int64 project_id)
{
    if( NULL == uuid || NULL == ln_uuid)
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    CDbQuantumNetwork info;
    info.SetUuid(uuid);
    info.SetLnUuid(ln_uuid);
    info.SetProjectId(project_id);

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
DEFINE_DEBUG_FUNC(db_add_qn);

void db_check_del_qn(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "check Del failed." << ret << endl;
        return;
    }
    cout << "check Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_qn);

void db_del_qn(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateQuantumNetwork * pOper = GetDbIQuantumNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumNetwork is NULL" << endl;
        return ;
    }

    int ret = pOper->Del(uuid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_qn);


}




