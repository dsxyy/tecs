

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_quantum_port.h"
    
namespace zte_tecs
{
typedef enum tagQuantumPortProc
{
    EN_QUANTUMNETPORT_PROC_CHECK_ADD = 0,
    EN_QUANTUMNETPORT_PROC_ADD,
    EN_QUANTUMNETPORT_PROC_CHECK_DEL,
    EN_QUANTUMNETPORT_PROC_DEL,
    EN_QUANTUMNETPORT_PROC_QUERY,       
    EN_QUANTUMNETPORT_PROC_QUERY_ALL, 
    EN_QUANTUMNETPORT_PROC_ALL,
}EN_VSWITCH_PROC;

const char *s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_ALL] = 
{        
    "pf_net_check_add_quantum_port",  
    "pf_net_add_quantum_port",    
    "pf_net_check_del_quantum_port",  
    "pf_net_del_quantum_port",  
    
    "pf_net_query_quantumport",
    "pf_net_query_quantumport_all",
};

    
CDBOperateQuantumPort::CDBOperateQuantumPort(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateQuantumPort::~CDBOperateQuantumPort()
{
    
}

int32 CDBOperateQuantumPort::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_QUANTUMNETPORT_PROC_QUERY_ALL:
        {
            return QueryAllCallback(prs,nil);
        }
        break;
        case EN_QUANTUMNETPORT_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_QUANTUMNETPORT_PROC_ADD:
        case EN_QUANTUMNETPORT_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_QUANTUMNETPORT_PROC_CHECK_ADD:
        case EN_QUANTUMNETPORT_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateQuantumPort::QueryAll(vector<CDbQuantumPort> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::QueryAll IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_QUERY_ALL],\
        vInParam, \
        this,\
        EN_QUANTUMNETPORT_PROC_QUERY_ALL,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {        
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::QueryAll() IProc->Handle failed(%d) \n",
                ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateQuantumPort::QueryAllCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::QueryAllCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbQuantumPort> *pvOut;
    pvOut = static_cast<vector<CDbQuantumPort>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::QueryAllCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbQuantumPort info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);
           
    info.SetQNUuid(prs->GetFieldValue(0));
    info.SetUuid(prs->GetFieldValue(1));

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateQuantumPort::Query(CDbQuantumPort & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_QUERY],
        vInParam, 
        this,
        EN_QUANTUMNETPORT_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {        
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateQuantumPort::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbQuantumPort *pOutPort;
    pOutPort = static_cast<CDbQuantumPort *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSwitchBase::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (3));    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetQNUuid( prs->GetFieldValue(1));
    (*pOutPort).SetUuid( prs->GetFieldValue(2));
 
    return 0;
}

int32 CDBOperateQuantumPort::CheckAdd(CDbQuantumPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    
    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam; 
    CADOParameter inQnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQNUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    
    vInParam.push_back(inQnUuid);
    vInParam.push_back(inUuid); 
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_QUANTUMNETPORT_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateQuantumPort::Add(CDbQuantumPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam; 
    CADOParameter inQnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQNUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    
    vInParam.push_back(inQnUuid);
    vInParam.push_back(inUuid);   
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_ADD],
        vInParam, 
        this,
        EN_QUANTUMNETPORT_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateQuantumPort::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateQuantumPort::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CheckDel uuid is NULL.\n");
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
        s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_QUANTUMNETPORT_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CheckDel(%s) proc return failed. ret : %d.\n", 
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateQuantumPort::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::Del uuid is NULL.\n");
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
        s_astrQuantumPortProc[EN_QUANTUMNETPORT_PROC_DEL],
        vInParam, 
        this,
        EN_QUANTUMNETPORT_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateQuantumPort::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateQuantumPort::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateQuantumPort::CanOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateQuantumPort::DbgShow()
{
}

/* test code ------------------------------------------------------------

*/

void db_query_qp_all()
{
    
    CDBOperateQuantumPort * pOper = GetDbIQuantumPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumPort is NULL" << endl;
        return ;
    }

    vector<CDbQuantumPort> vinfo;
    int ret = pOper->QueryAll(vinfo);
    if( ret != 0 )
    {
        cout << "CDBOperateQuantumPort query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateQuantumPort query success." << endl;
    vector<CDbQuantumPort>::iterator it = vinfo.begin();
    for(; it != vinfo.end(); ++it)
    {
        (*it).DbgShow();
        cout << "---------" << endl;
    }
    
}
DEFINE_DEBUG_FUNC(db_query_qp_all);

void db_query_qp(char* uuid)
{
    if( NULL == uuid)
    {
        cout << "uuid is null" << endl;
        return ;
    }
    CDBOperateQuantumPort * pOper = GetDbIQuantumPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumPort is NULL" << endl;
        return ;
    }

    CDbQuantumPort info;
    info.SetUuid(uuid);
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateQuantumPort query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateQuantumPort query success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_query_qp);


void db_check_add_qp(char* uuid,char* qn_uuid)
{
    if( NULL == uuid || NULL == qn_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateQuantumPort * pOper = GetDbIQuantumPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumPort is NULL" << endl;
        return ;
    }

    CDbQuantumPort info;
    info.SetUuid(uuid);
    info.SetQNUuid(qn_uuid);

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
DEFINE_DEBUG_FUNC(db_check_add_qp);

void db_add_qp(char* uuid,char* qn_uuid)
{
    if( NULL == uuid || NULL == qn_uuid)
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateQuantumPort * pOper = GetDbIQuantumPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumPort is NULL" << endl;
        return ;
    }

    CDbQuantumPort info;
    info.SetUuid(uuid);
    info.SetQNUuid(qn_uuid);

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
DEFINE_DEBUG_FUNC(db_add_qp);

void db_check_del_qp(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateQuantumPort * pOper = GetDbIQuantumPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumPort is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_check_del_qp);

void db_del_qp(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateQuantumPort * pOper = GetDbIQuantumPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateQuantumPort is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_qp);


}




