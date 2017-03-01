

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_port_vswitch.h"
    
namespace zte_tecs
{
typedef enum tagPortVSwitchProc
{
    EN_PORT_VSWITCH_PROC_QUERYSUMMARY = 0,
    EN_PORT_VSWITCH_PROC_QUERY,     
    EN_PORT_VSWITCH_PROC_ADD,
    EN_PORT_VSWITCH_PROC_MODIFY,
    EN_PORT_VSWITCH_PROC_DEL,
    EN_PORT_VSWITCH_PROC_ALL,
}EN_PORT_VSWITCH_PROC;

const char *s_astrPortVSwitchProc[EN_PORT_VSWITCH_PROC_ALL] = 
{        
    "pf_net_query_vswitchport_summary",
    "pf_net_query_vswitchport",
    "pf_net_add_vswitchport",    
    "pf_net_modify_vswitchport",  
    "pf_net_del_vswitchport",  
};

    
CDBOperatePortVSwitch::CDBOperatePortVSwitch(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortVSwitch::~CDBOperatePortVSwitch()
{
    
}

int32 CDBOperatePortVSwitch::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORT_VSWITCH_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_PORT_VSWITCH_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORT_VSWITCH_PROC_ADD:
        case EN_PORT_VSWITCH_PROC_MODIFY:
        case EN_PORT_VSWITCH_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePortVSwitch::QuerySummary(const char* vna_uuid,vector<CDbPortSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::QuerySummary vna_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)vna_uuid);
    vInParam.push_back(inVnaUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortVSwitchProc[EN_PORT_VSWITCH_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_PORT_VSWITCH_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePortVSwitch::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortSummary> *pvOut;
    pvOut = static_cast<vector<CDbPortSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPortSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetName(prs->GetFieldValue(1));    
    info.SetUuid(prs->GetFieldValue(2));
    int32 port_type = 0;
    if( false == prs->GetFieldValue(3,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(port_type);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperatePortVSwitch::Query(CDbPort & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVSwitchProc[EN_PORT_VSWITCH_PROC_QUERY],
        vInParam, 
        this,
        EN_PORT_VSWITCH_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgPortShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortVSwitch::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (DB_PORT_TABLE_FIELD_NUM ));    

    // 1 获取port 信息
    int32 proc_ret = 0;
    proc_ret = QueryPortBaseCallback(prs,nil);
    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::QueryCallback QueryPortBaseCallback proc failed(%d).\n", (proc_ret));
        return (proc_ret);
    }

    return 0;
}

int32 CDBOperatePortVSwitch::Add(CDbPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, EN_DB_PORT_TYPE_VSWITCH);    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inBroadcast(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsBroadcast());    
    CADOParameter inRunning(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsRunning());    
    CADOParameter inMult(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMulticast());    
    CADOParameter inPromi(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPromiscuous());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());        
    CADOParameter inMaster(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMaster()); 
    CADOParameter inAdminState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAdminState()); 
    CADOParameter inOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    CADOParameter inIsPortCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsPortCfg()); 
    CADOParameter inIsConsistency(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsConsistency()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inPortType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inBroadcast);    
    vInParam.push_back(inRunning);    
    vInParam.push_back(inMult);    
    vInParam.push_back(inPromi);    
    vInParam.push_back(inMtu);    
    vInParam.push_back(inMaster);    
    vInParam.push_back(inAdminState);    
    vInParam.push_back(inOnline);
    vInParam.push_back(inIsPortCfg); 
    vInParam.push_back(inIsConsistency);
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVSwitchProc[EN_PORT_VSWITCH_PROC_ADD],
        vInParam, 
        this,
        EN_PORT_VSWITCH_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgPortShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Add IProc->Handle failed(%d).\n", ret);
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
            info.DbgPortShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortVSwitch::Modify(CDbPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_PORT_TYPE_VSWITCH != info.GetPortType() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Modify type(%d) is invalid.\n",info.GetPortType());
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inBroadcast(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsBroadcast());    
    CADOParameter inRunning(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsRunning());    
    CADOParameter inMult(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMulticast());    
    CADOParameter inPromi(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPromiscuous());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());        
    CADOParameter inMaster(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMaster()); 
    CADOParameter inAdminState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAdminState()); 
    CADOParameter inOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    CADOParameter inIsPortCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsPortCfg()); 
    CADOParameter inIsConsistency(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsConsistency()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inPortType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inBroadcast);    
    vInParam.push_back(inRunning);    
    vInParam.push_back(inMult);    
    vInParam.push_back(inPromi);    
    vInParam.push_back(inMtu);    
    vInParam.push_back(inMaster);    
    vInParam.push_back(inAdminState);    
    vInParam.push_back(inOnline); 
    vInParam.push_back(inIsPortCfg); 
    vInParam.push_back(inIsConsistency);
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVSwitchProc[EN_PORT_VSWITCH_PROC_MODIFY],
        vInParam, 
        this,
        EN_PORT_VSWITCH_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgPortShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Modify IProc->Handle failed(%d).\n", ret);
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
            info.DbgPortShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVSwitch::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortVSwitch::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::Del Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Port_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVSwitchProc[EN_PORT_VSWITCH_PROC_DEL],
        vInParam, 
        this,
        EN_PORT_VSWITCH_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::Del(%s) IProc->Handle failed. ret : %d.\n",
            Port_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::Del(%s) proc return failed. ret : %d.\n",
            Port_uuid, GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortVSwitch::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVSwitch::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePortVSwitch::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbPortSummary> vInfo;
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
    vector<CDbPortSummary>::iterator it = vInfo.begin();
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
void db_query_vswitchportsummary(const char* vna_uuid)
{
    if( NULL == vna_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortVSwitch * pOper = GetDbIPortVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVSwitch is NULL" << endl;
        return ;
    }

    vector<CDbPortSummary> outVInfo;
    int ret = pOper->QuerySummary(vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vswitchportsummary);


void db_query_vswitchport(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePortVSwitch * pOper = GetDbIPortVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVSwitch is NULL" << endl;
        return ;
    }

    CDbPort info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperatePortVSwitch query failed." << ret << endl;
        return;
    }
    cout << "CDBOperatePortVSwitch query success." << endl;
    info.DbgPortShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vswitchport);

void db_add_vswitchport(char* vna_uuid,int32 port_type, char* name, int32 is_online)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortVSwitch * pOper = GetDbIPortVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVSwitch is NULL" << endl;
        return ;
    }

    CDbPort info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(port_type);
    info.SetName(name);
    info.SetIsOnline(is_online);

    //info.SetIsCfg(is_cfg);
    
    //info.DbgShow();
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgPortShow();
}
DEFINE_DEBUG_FUNC(db_add_vswitchport);

void db_mod_vswitchport(char* port_uuid, char* vna_uuid, char* name, int32 is_online)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortVSwitch * pOper = GetDbIPortVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVSwitch is NULL" << endl;
        return ;
    }

    CDbPort info;
    info.SetUuid(port_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_VSWITCH);
    info.SetVnaUuid(vna_uuid);
    info.SetName(name);
    info.SetIsOnline(is_online);
        
    //info.DbgShow();
    int ret = pOper->Modify(info);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgPortShow();
}
DEFINE_DEBUG_FUNC(db_mod_vswitchport);

void db_del_vswitchport(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePortVSwitch * pOper = GetDbIPortVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVSwitch is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_vswitchport);


}

