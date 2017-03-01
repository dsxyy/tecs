

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_port_kernel.h"
    
namespace zte_tecs
{
typedef enum tagPortKernelProc
{
    EN_PORT_KERNEL_PROC_QUERYSUMMARY = 0,
    EN_PORT_KERNEL_PROC_QUERY,     
    EN_PORT_KERNEL_PROC_ADD,
    EN_PORT_KERNEL_PROC_MODIFY,
    EN_PORT_KERNEL_PROC_DEL,
    EN_PORT_KERNEL_PROC_ALL,
}EN_PORT_KERNEL_PROC;

const char *s_astrPortKernelProc[EN_PORT_KERNEL_PROC_ALL] = 
{        
    "pf_net_query_kernelport_summary",
    "pf_net_query_kernelport",
    "pf_net_add_kernelport",    
    "pf_net_modify_kernelport",  
    "pf_net_del_kernelport",  
};

    
CDBOperatePortKernel::CDBOperatePortKernel(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortKernel::~CDBOperatePortKernel()
{
    
}

int32 CDBOperatePortKernel::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORT_KERNEL_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_PORT_KERNEL_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORT_KERNEL_PROC_ADD:
        case EN_PORT_KERNEL_PROC_MODIFY:
        case EN_PORT_KERNEL_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePortKernel::QuerySummary(const char* vna_uuid,vector<CDbPortSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QuerySummary vna_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)vna_uuid);
    vInParam.push_back(inVnaUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortKernelProc[EN_PORT_KERNEL_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_PORT_KERNEL_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePortKernel::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryPortSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortSummary> *pvOut;
    pvOut = static_cast<vector<CDbPortSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::QueryPortSummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPortSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetUuid(prs->GetFieldValue(1));
    info.SetName(prs->GetFieldValue(2));    
    int32 port_type = 0;
    if( false == prs->GetFieldValue(3,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryPortSummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(port_type);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperatePortKernel::Query(CDbPortKernel & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortKernelProc[EN_PORT_KERNEL_PROC_QUERY],
        vInParam, 
        this,
        EN_PORT_KERNEL_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortKernel::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (DB_PORT_TABLE_FIELD_NUM +7));    

    // 1 获取port 信息
    int32 proc_ret = 0;
    proc_ret = QueryPortBaseCallback(prs,nil);
    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::QueryCallback QueryPortBaseCallback proc failed(%d).\n", (proc_ret));
        return (proc_ret);
    }

    // 2 获取kernel 信息
    CDbPortKernel *pOutPort;
    pOutPort = static_cast<CDbPortKernel *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // port 14        
    int32 values = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryCallback GetFieldValue 0 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsCfgVswitch(values);  
    (*pOutPort).SetVswitchUuid( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 1));
    
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsCfgPg(values); 
    (*pOutPort).SetPgUuid( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 3));

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 4,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetType(values); 

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 5,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsCfg(values); 

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 6,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::QueryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsDhcp(values); 

    return 0;
}

int32 CDBOperatePortKernel::Add(CDbPortKernel & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_KERNEL_PG_TYPE_MGR != info.GetType() && 
        EN_DB_KERNEL_PG_TYPE_STORAGE != info.GetType())
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Add type(%d) is invalid.\n",info.GetType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, EN_DB_PORT_TYPE_KERNEL);    
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

    // uplink loop port 
    CADOParameter inIsCfgVS(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfgVswitch());
    CADOParameter inVsUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVswitchUuid().c_str());
    CADOParameter inIsCfgPg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfgPg());
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());
    CADOParameter inType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetType());    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfg());
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp());
    
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
 
    vInParam.push_back(inIsCfgVS); 
    vInParam.push_back(inVsUuid); 
    vInParam.push_back(inIsCfgPg); 
    vInParam.push_back(inPgUuid); 
    vInParam.push_back(inType); 
    vInParam.push_back(inIsCfg); 
    vInParam.push_back(inIsDhcp);     
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortKernelProc[EN_PORT_KERNEL_PROC_ADD],
        vInParam, 
        this,
        EN_PORT_KERNEL_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortKernel::Modify(CDbPortKernel & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_PORT_TYPE_KERNEL != info.GetPortType() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Modify type(%d) is invalid.\n",info.GetPortType());
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if( EN_DB_KERNEL_PG_TYPE_MGR != info.GetType() && 
        EN_DB_KERNEL_PG_TYPE_STORAGE != info.GetType())
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Modify type(%d) is invalid.\n",info.GetType() );
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


    // uplink loop port 
    CADOParameter inIsCfgVS(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfgVswitch());
    CADOParameter inVsUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVswitchUuid().c_str());
    CADOParameter inIsCfgPg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfgPg());
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());
    CADOParameter inType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetType());    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfg());
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp());
    
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
 
    vInParam.push_back(inIsCfgVS); 
    vInParam.push_back(inVsUuid); 
    vInParam.push_back(inIsCfgPg); 
    vInParam.push_back(inPgUuid); 
    vInParam.push_back(inType); 
    vInParam.push_back(inIsCfg);
    vInParam.push_back(inIsDhcp);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortKernelProc[EN_PORT_KERNEL_PROC_MODIFY],
        vInParam, 
        this,
        EN_PORT_KERNEL_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortKernel::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortKernel::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::Del Port_uuid is NULL.\n");
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
        s_astrPortKernelProc[EN_PORT_KERNEL_PROC_DEL],
        vInParam, 
        this,
        EN_PORT_KERNEL_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::Del(%s) IProc->Handle failed. ret : %d.\n",Port_uuid, ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::Del(%s) proc return failed. ret : %d.\n", Port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortKernel::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortKernel::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePortKernel::DbgShow()
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
void db_query_kernelportsummary(const char* vna_uuid)
{
    if( NULL == vna_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortKernel is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_query_kernelportsummary);

void db_query_kernelport(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortKernel is NULL" << endl;
        return ;
    }

    CDbPortKernel info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperatePortKernel query failed." << ret << endl;
        return;
    }
    cout << "CDBOperatePortKernel query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_kernelport);

void db_add_kernelport(char* vna_uuid,int32 port_type, char* name, int32 is_online,
        int32 type, int32 is_cfg)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortKernel is NULL" << endl;
        return ;
    }

    CDbPortKernel info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(port_type);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetType(type);
    info.SetIsCfg(is_cfg);
    
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
DEFINE_DEBUG_FUNC(db_add_kernelport);

void db_mod_kernelport(char* port_uuid, char* vna_uuid,char* name, int32 is_online,
        int32 type,int32 is_cfg)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortKernel is NULL" << endl;
        return ;
    }

    CDbPortKernel info;
    info.SetUuid(port_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_KERNEL);
    info.SetVnaUuid(vna_uuid);
    info.SetName(name);
    info.SetIsOnline(is_online);
    
    info.SetType(type);
    info.SetIsCfg(is_cfg);
        
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
DEFINE_DEBUG_FUNC(db_mod_kernelport);

void db_del_kernelport(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortKernel is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_kernelport);


}


