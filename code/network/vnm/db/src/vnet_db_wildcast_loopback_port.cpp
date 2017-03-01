

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_wildcast_loopback_port.h"
    
namespace zte_tecs
{
typedef enum tagWildcastLoopbackPortProc
{
    EN_WILDCAST_LOOPBACK_PORT_PROC_QUERYSUMMARY = 0,
    EN_WILDCAST_LOOPBACK_PORT_PROC_QUERY,   
    EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_ADD,
    EN_WILDCAST_LOOPBACK_PORT_PROC_ADD,
    EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_MODIFY,
    EN_WILDCAST_LOOPBACK_PORT_PROC_MODIFY,
    EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_DEL,
    EN_WILDCAST_LOOPBACK_PORT_PROC_DEL,
    EN_WILDCAST_LOOPBACK_PORT_PROC_ALL,
}EN_WILDCAST_LOOPBACK_PORT_PROC;

const char *s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_ALL] = 
{        
    "pf_net_wildcast_loopback_port_summary",
    "pf_net_wildcast_query_loopback_port",
    "pf_net_check_add_wildcast_loopback_port",  
    "pf_net_add_wildcast_loopback_port",    
    "pf_net_check_modify_wildcast_loopback_port", 
    "pf_net_modify_wildcast_loopback_port", 
    "pf_net_check_del_wildcast_loopback_port",  
    "pf_net_del_wildcast_loopback_port",  
};

    
CDBOperateWildcastLoopbackPort::CDBOperateWildcastLoopbackPort(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateWildcastLoopbackPort::~CDBOperateWildcastLoopbackPort()
{
    
}

int32 CDBOperateWildcastLoopbackPort::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_WILDCAST_LOOPBACK_PORT_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_LOOPBACK_PORT_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_LOOPBACK_PORT_PROC_ADD:
        case EN_WILDCAST_LOOPBACK_PORT_PROC_MODIFY:
        case EN_WILDCAST_LOOPBACK_PORT_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_ADD:
        case EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_MODIFY:
        case EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateWildcastLoopbackPort::QuerySummary(vector<CDbWildcastLoopbackPort> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_WILDCAST_LOOPBACK_PORT_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateWildcastLoopbackPort::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbWildcastLoopbackPort> *pvOut;
    pvOut = static_cast<vector<CDbWildcastLoopbackPort>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbWildcastLoopbackPort info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
    info.SetUuid(prs->GetFieldValue(0));
    info.SetPortName(prs->GetFieldValue(1));   
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsLoop(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateWildcastLoopbackPort::Query(CDbWildcastLoopbackPort & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_QUERY],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastLoopbackPort::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 2 获取kernel 信息
    CDbWildcastLoopbackPort *pOutPort = NULL;
    pOutPort = static_cast<CDbWildcastLoopbackPort *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetUuid( prs->GetFieldValue(1));
    (*pOutPort).SetPortName( prs->GetFieldValue(2));

    
    int32 value = 0;
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsLoop(value);
    
    return 0;
}

int32 CDBOperateWildcastLoopbackPort::CheckAdd(CDbWildcastLoopbackPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inIsLoop);
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastLoopbackPort::Add(CDbWildcastLoopbackPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;     
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inIsLoop);  
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_ADD],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    info.SetUuid(proc_ret.GetUuid().c_str());

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastLoopbackPort::CheckModify(CDbWildcastLoopbackPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inIsLoop);  

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::CheckModify IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastLoopbackPort::Modify(CDbWildcastLoopbackPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inIsLoop); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastLoopbackPort::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateWildcastLoopbackPort::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckDel uuid is NULL.\n");
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
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckDel(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateWildcastLoopbackPort::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::Del uuid is NULL.\n");
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
        s_astrWildcastLoopbackPortProc[EN_WILDCAST_LOOPBACK_PORT_PROC_DEL],
        vInParam, 
        this,
        EN_WILDCAST_LOOPBACK_PORT_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::Del(%s) IProc->Handle failed. ret : %d.\n", 
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateWildcastLoopbackPort::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateWildcastLoopbackPort::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastLoopbackPort::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateWildcastLoopbackPort::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbWildcastLoopbackPort> vInfo;
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
    vector<CDbWildcastLoopbackPort>::iterator it = vInfo.begin();
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

void db_query_wildcast_loopbackport_summary()
{    
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }

    vector<CDbWildcastLoopbackPort> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbWildcastLoopbackPort>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_wildcast_loopbackport_summary);

void db_query_wildcast_loopbackport(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }

    CDbWildcastLoopbackPort info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateWildcastLoopbackPort query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateWildcastLoopbackPort query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_wildcast_loopbackport);


void db_check_add_wc_loopback_port(char* uuid,char* name, int32 is_loop )
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }

    CDbWildcastLoopbackPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetIsLoop(is_loop);

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
DEFINE_DEBUG_FUNC(db_check_add_wc_loopback_port);


void db_add_wc_loopback_port(char* uuid,char* name, int32 is_loop )
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }

    CDbWildcastLoopbackPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetIsLoop(is_loop);

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
DEFINE_DEBUG_FUNC(db_add_wc_loopback_port);


void db_check_mod_wc_loopback_port(char* uuid,char* name, int32 is_loop )
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }

    CDbWildcastLoopbackPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetIsLoop(is_loop);

    //info.DbgShow();
    int ret = pOper->CheckModify(info);
    if( ret != 0 )
    {
        cout << "CheckModify failed." << ret << endl;
        return;
    }
    cout << "CheckModify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_wc_loopback_port);


void db_mod_wc_loopback_port(char* uuid,char* name, int32 is_loop )
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }

    CDbWildcastLoopbackPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetIsLoop(is_loop);

    //info.DbgShow();
    int ret = pOper->Modify(info);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_wc_loopback_port);


void db_check_del_wc_loopback_port(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }
    //info.DbgShow();
    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "CheckDel failed." << ret << endl;
        return;
    }
    cout << "CheckDel success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_wc_loopback_port);

void db_del_wc_loopback_port(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastLoopbackPort * pOper = GetDbIWildcastLoopbackPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastLoopbackPort is NULL" << endl;
        return ;
    }
    //info.DbgShow();
    int ret = pOper->Del(uuid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_wc_loopback_port);

}



