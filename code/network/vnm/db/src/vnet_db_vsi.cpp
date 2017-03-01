

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_vsi.h"
    
namespace zte_tecs
{
typedef enum tagVsiProc
{
    EN_VSI_PROC_CHECK_ADD = 0,
    EN_VSI_PROC_ADD,
    EN_VSI_PROC_CHECK_MODIFY,
    EN_VSI_PROC_MODIFY,
    EN_VSI_PROC_CHECK_DEL,
    EN_VSI_PROC_DEL,
    EN_VSI_PROC_DEL_BY_VMID,

    // QUERY    
    EN_VSI_PROC_QUERY,   
    EN_VSI_PROC_QUERY_BY_VMVNIC,  
    EN_VSI_PROC_QUERYSUMMARY,  
    EN_VSI_PROC_QUERYSUMMARY_BY_VM,  
    EN_VSI_PROC_ALL,
}EN_VSI_PROC;

const char *s_astrVsiProc[EN_VSI_PROC_ALL] = 
{        
    "pf_net_check_add_vsi",  
    "pf_net_add_vsi",    
    "pf_net_check_modify_vsi", 
    "pf_net_modify_vsi", 
    "pf_net_check_del_vsi",  
    "pf_net_del_vsi",  
    "pf_net_del_vsi_by_vmid",  
    
    // QUERY        
    "pf_net_query_vsi",
    "pf_net_query_vsi_by_vmnic",
    "pf_net_query_vsisummary",
    "pf_net_query_vsisummary_by_vm",
};

    
CDBOperateVsi::CDBOperateVsi(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVsi::~CDBOperateVsi()
{
    
}

int32 CDBOperateVsi::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VSI_PROC_QUERY:
        case EN_VSI_PROC_QUERY_BY_VMVNIC:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_VSI_PROC_QUERYSUMMARY:
        case EN_VSI_PROC_QUERYSUMMARY_BY_VM:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_VSI_PROC_ADD:
        case EN_VSI_PROC_MODIFY:
        case EN_VSI_PROC_DEL:
        case EN_VSI_PROC_DEL_BY_VMID:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_VSI_PROC_CHECK_ADD:
        case EN_VSI_PROC_CHECK_MODIFY:
        case EN_VSI_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}

int32 CDBOperateVsi::QueryByVsi(CDbVsiInfo & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QueryByVsi IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetVsiIdValue().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_QUERY],
        vInParam, 
        this,
        EN_VSI_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryByVsi IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVsi::QueryByVmVnic(CDbVsiInfo & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QueryByVmVnic IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, Info.GetVmId());
    CADOParameter inNicIndex(CADOParameter::paramInt, CADOParameter::paramInput, Info.GetNicIndex());
    vInParam.push_back(inVmid);
    vInParam.push_back(inNicIndex);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_QUERY_BY_VMVNIC],
        vInParam, 
        this,
        EN_VSI_PROC_QUERY_BY_VMVNIC,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryByVmVnic IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVsi::QuerySummaryByVm(int64 vmid, vector<CDbVsiInfoSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QuerySummaryByVm IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, vmid);
    vInParam.push_back(inVmid);  
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrVsiProc[EN_VSI_PROC_QUERYSUMMARY_BY_VM],
        vInParam, 
        this,
        EN_VSI_PROC_QUERYSUMMARY_BY_VM,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummaryByVm(%lld) IProc->Handle failed(%d).\n",
                vmid,ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVsi::QuerySummary(vector<CDbVsiInfoSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrVsiProc[EN_VSI_PROC_QUERYSUMMARY],
        vInParam, 
        this,
        EN_VSI_PROC_QUERYSUMMARY,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummary IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVsi::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVsiInfoSummary> *pvOut;
    pvOut = static_cast<vector<CDbVsiInfoSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbVsiInfoSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 5);
       
    // vm id       
    int64 value64  = 0;
    if( false == prs->GetFieldValue(0,value64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummaryCallback GetFieldValue 0 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVmId(value64); 
    
    // nic index        
    int32 values = 0;
    if( false == prs->GetFieldValue(1,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummaryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetNicIndex(values);  

    // project id
    if( false == prs->GetFieldValue(2,value64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetProjectId(value64); 

    info.SetVsiIdValue( prs->GetFieldValue(3));

    if( false == prs->GetFieldValue(4,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QuerySummaryCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVsiIdFormat(values); 
   
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateVsi::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    //1 VSI 信息
    CDbVsiInfo *pOutPort;
    pOutPort = static_cast<CDbVsiInfo *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (24));    

    // proc ret
    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    // vm id       
    int64 value64  = 0;
    if( false == prs->GetFieldValue(1,value64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetVmId(value64); 
            
    int32 values = 0;
        
    // nic index    
    if( false == prs->GetFieldValue(2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetNicIndex(values);  
    
    if( false == prs->GetFieldValue(3,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsSriov(values);  

    if( false == prs->GetFieldValue(4,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 7 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsLoop(values); 

    (*pOutPort).SetLogicNetworkUuid( prs->GetFieldValue(5));
    (*pOutPort).SetAssIP( prs->GetFieldValue(6));
    (*pOutPort).SetAssMask( prs->GetFieldValue(7));
    (*pOutPort).SetAssGateway( prs->GetFieldValue(8));
    (*pOutPort).SetAssMac( prs->GetFieldValue(9));
    (*pOutPort).SetPortType( prs->GetFieldValue(10));
        
    (*pOutPort).SetVsiIdValue( prs->GetFieldValue(11));

    if( false == prs->GetFieldValue(12,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 15 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetVsiIdFormat(values); 

    (*pOutPort).SetVmPgUuid( prs->GetFieldValue(13));
    (*pOutPort).SetMac( prs->GetFieldValue(14));
    
    if( false == prs->GetFieldValue(15,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 18 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsHasIp(values); 
    
    (*pOutPort).SetIp( prs->GetFieldValue(16));
    (*pOutPort).SetMask( prs->GetFieldValue(17));

    if( false == prs->GetFieldValue(18,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 21 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsHasVSwitch(values); 

    (*pOutPort).SetVSwitchUuid( prs->GetFieldValue(19));

    if( false == prs->GetFieldValue(20,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::QueryCallback GetFieldValue 22 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsHasSriovVf(values); 

    (*pOutPort).SetSriovVfPortUuid( prs->GetFieldValue(21));

    (*pOutPort).SetQuantumNetworkUuid( prs->GetFieldValue(22));

    (*pOutPort).SetQuantumPortUuid( prs->GetFieldValue(23));
    
    return 0;
}

int32 CDBOperateVsi::CheckAdd(CDbVsiInfo & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetVsiIdValue(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());    
    CADOParameter inNicIndex(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicIndex());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop());   
    CADOParameter inLogicNetworkUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLogicNetworkUuid().c_str());
    CADOParameter inAssIP(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssIP().c_str());
    CADOParameter inAssMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMask().c_str());
    CADOParameter inAssGateway(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssGateway().c_str());
    CADOParameter inAssMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMac().c_str());
    CADOParameter inPortType(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortType().c_str());    
    
    CADOParameter inVsiIdValue(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVsiIdValue().c_str()); 
    CADOParameter inVsiIdFormat(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVsiIdFormat()); 
    CADOParameter inVmPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVmPgUuid().c_str());     
    CADOParameter inMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMac().c_str()); 
    CADOParameter inIsHasIp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasIp()); 
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str()); 
    //CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());     
    CADOParameter inIsHasVSwitch(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasVSwitch()); 
    CADOParameter inVSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVSwitchUuid().c_str()); 
    CADOParameter inIsHasSriovVf(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasSriovVf()); 
    CADOParameter inSriovVfPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSriovVfPortUuid().c_str()); 
    CADOParameter inQnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumNetworkUuid().c_str()); 
    CADOParameter inQpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumPortUuid().c_str()); 
    
                           
    vInParam.push_back(inVmid);
    vInParam.push_back(inNicIndex);   
    vInParam.push_back(inIsSriov);    
    vInParam.push_back(inIsLoop);        
    vInParam.push_back(inLogicNetworkUuid);
    vInParam.push_back(inAssIP);
    vInParam.push_back(inAssMask);
    vInParam.push_back(inAssGateway);
    vInParam.push_back(inAssMac);
    vInParam.push_back(inPortType);    
    
    vInParam.push_back(inVsiIdValue);   
    vInParam.push_back(inVsiIdFormat);   
    vInParam.push_back(inVmPgUuid);   
    vInParam.push_back(inMac);   
    vInParam.push_back(inIsHasIp);   
    vInParam.push_back(inIp);   
    //vInParam.push_back(inMask);   
    vInParam.push_back(inIsHasVSwitch);   
    vInParam.push_back(inVSwitchUuid);   
    vInParam.push_back(inIsHasSriovVf);   
    vInParam.push_back(inSriovVfPortUuid);  
    vInParam.push_back(inQnUuid);
    vInParam.push_back(inQpUuid);
    
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_VSI_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVsi::Add(CDbVsiInfo & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    string vsiUuid = GenerateUUID();
    string::iterator  it;  
    for (it =vsiUuid.begin(); it != vsiUuid.end(); ) 
    {  
        if ( *it == '-') 
        {  
            it = vsiUuid.erase(it); 
        } 
        else
        {
            ++it;
        }
    }  

    info.SetVsiIdValue(vsiUuid.c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());    
    
    CADOParameter inNicIndex(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicIndex());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop());   
    CADOParameter inLogicNetworkUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLogicNetworkUuid().c_str());
    CADOParameter inAssIP(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssIP().c_str());
    CADOParameter inAssMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMask().c_str());
    CADOParameter inAssGateway(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssGateway().c_str());
    CADOParameter inAssMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMac().c_str());
    CADOParameter inPortType(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortType().c_str());    
    
    CADOParameter inVsiIdValue(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVsiIdValue().c_str()); 
    CADOParameter inVsiIdFormat(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVsiIdFormat()); 
    CADOParameter inVmPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVmPgUuid().c_str());     
    CADOParameter inMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMac().c_str()); 
    CADOParameter inIsHasIp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasIp()); 
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str()); 
    //CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());     
    CADOParameter inIsHasVSwitch(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasVSwitch()); 
    CADOParameter inVSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVSwitchUuid().c_str()); 
    CADOParameter inIsHasSriovVf(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasSriovVf()); 
    CADOParameter inSriovVfPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSriovVfPortUuid().c_str()); 
    CADOParameter inQnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumNetworkUuid().c_str()); 
    CADOParameter inQpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumPortUuid().c_str()); 
    
    
    vInParam.push_back(inVmid);  
    vInParam.push_back(inNicIndex);    
    vInParam.push_back(inIsSriov);    
    vInParam.push_back(inIsLoop);        
    vInParam.push_back(inLogicNetworkUuid);
    vInParam.push_back(inAssIP);
    vInParam.push_back(inAssMask);
    vInParam.push_back(inAssGateway);
    vInParam.push_back(inAssMac);
    vInParam.push_back(inPortType);  
    
    vInParam.push_back(inVsiIdValue);   
    vInParam.push_back(inVsiIdFormat);   
    vInParam.push_back(inVmPgUuid);   
    vInParam.push_back(inMac);   
    vInParam.push_back(inIsHasIp);   
    vInParam.push_back(inIp);   
    //vInParam.push_back(inMask);   
    vInParam.push_back(inIsHasVSwitch);   
    vInParam.push_back(inVSwitchUuid);   
    vInParam.push_back(inIsHasSriovVf);   
    vInParam.push_back(inSriovVfPortUuid);   
    vInParam.push_back(inQnUuid);
    vInParam.push_back(inQpUuid);
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_ADD],
        vInParam, 
        this,
        EN_VSI_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::Add IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        info.SetVsiIdValue(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVsi::CheckModify(CDbVsiInfo & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());    
    CADOParameter inNicIndex(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicIndex());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop());   
    CADOParameter inLogicNetworkUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLogicNetworkUuid().c_str());
    CADOParameter inAssIP(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssIP().c_str());
    CADOParameter inAssMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMask().c_str());
    CADOParameter inAssGateway(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssGateway().c_str());
    CADOParameter inAssMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMac().c_str());
    CADOParameter inPortType(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortType().c_str());    
    
    CADOParameter inVsiIdValue(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVsiIdValue().c_str()); 
    CADOParameter inVsiIdFormat(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVsiIdFormat()); 
    CADOParameter inVmPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVmPgUuid().c_str());     
    CADOParameter inMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMac().c_str()); 
    CADOParameter inIsHasIp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasIp()); 
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str()); 
    //CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());     
    CADOParameter inIsHasVSwitch(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasVSwitch()); 
    CADOParameter inVSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVSwitchUuid().c_str()); 
    CADOParameter inIsHasSriovVf(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasSriovVf()); 
    CADOParameter inSriovVfPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSriovVfPortUuid().c_str()); 
    CADOParameter inQnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumNetworkUuid().c_str()); 
    CADOParameter inQpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumPortUuid().c_str()); 
  
    
    vInParam.push_back(inVmid);
    vInParam.push_back(inNicIndex);   
    vInParam.push_back(inIsSriov);    
    vInParam.push_back(inIsLoop);        
    vInParam.push_back(inLogicNetworkUuid);
    vInParam.push_back(inAssIP);
    vInParam.push_back(inAssMask);
    vInParam.push_back(inAssGateway);
    vInParam.push_back(inAssMac);
    vInParam.push_back(inPortType);  
    
    vInParam.push_back(inVsiIdValue);   
    vInParam.push_back(inVsiIdFormat);   
    vInParam.push_back(inVmPgUuid);   
    vInParam.push_back(inMac);   
    vInParam.push_back(inIsHasIp);   
    vInParam.push_back(inIp);   
    //vInParam.push_back(inMask);   
    vInParam.push_back(inIsHasVSwitch);   
    vInParam.push_back(inVSwitchUuid);   
    vInParam.push_back(inIsHasSriovVf);   
    vInParam.push_back(inSriovVfPortUuid);   
    vInParam.push_back(inQnUuid);
    vInParam.push_back(inQpUuid);
    
    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_VSI_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::CheckModify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateVsi::Modify(CDbVsiInfo & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
     CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetVmId());    
    
    CADOParameter inNicIndex(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicIndex());
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSriov());
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLoop());   
    CADOParameter inLogicNetworkUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLogicNetworkUuid().c_str());
    CADOParameter inAssIP(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssIP().c_str());
    CADOParameter inAssMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMask().c_str());
    CADOParameter inAssGateway(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssGateway().c_str());
    CADOParameter inAssMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAssMac().c_str());
    CADOParameter inPortType(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortType().c_str());    
    
    CADOParameter inVsiIdValue(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVsiIdValue().c_str()); 
    CADOParameter inVsiIdFormat(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVsiIdFormat()); 
    CADOParameter inVmPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVmPgUuid().c_str());     
    CADOParameter inMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMac().c_str()); 
    CADOParameter inIsHasIp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasIp()); 
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str()); 
    //CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());     
    CADOParameter inIsHasVSwitch(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasVSwitch()); 
    CADOParameter inVSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVSwitchUuid().c_str()); 
    CADOParameter inIsHasSriovVf(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasSriovVf()); 
    CADOParameter inSriovVfPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSriovVfPortUuid().c_str()); 
    CADOParameter inQnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumNetworkUuid().c_str()); 
    CADOParameter inQpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQuantumPortUuid().c_str()); 
      
    vInParam.push_back(inVmid);    
    vInParam.push_back(inNicIndex);   
    vInParam.push_back(inIsSriov);    
    vInParam.push_back(inIsLoop);        
    vInParam.push_back(inLogicNetworkUuid);
    vInParam.push_back(inAssIP);
    vInParam.push_back(inAssMask);
    vInParam.push_back(inAssGateway);
    vInParam.push_back(inAssMac);
    vInParam.push_back(inPortType);  
    
    vInParam.push_back(inVsiIdValue);   
    vInParam.push_back(inVsiIdFormat);   
    vInParam.push_back(inVmPgUuid);   
    vInParam.push_back(inMac);   
    vInParam.push_back(inIsHasIp);   
    vInParam.push_back(inIp);   
    //vInParam.push_back(inMask);   
    vInParam.push_back(inIsHasVSwitch);   
    vInParam.push_back(inVSwitchUuid);   
    vInParam.push_back(inIsHasSriovVf);   
    vInParam.push_back(inSriovVfPortUuid); 
    vInParam.push_back(inQnUuid);
    vInParam.push_back(inQpUuid);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_MODIFY],
        vInParam, 
        this,
        EN_VSI_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVsi::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateVsi::CheckDel(const char* vsi_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vsi_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CheckDel vsi_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vsi_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_VSI_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            vsi_uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CheckDel(%s) proc return failed. ret : %d.\n",
            vsi_uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateVsi::Del(const char* vsi_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vsi_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::Del vsi_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vsi_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_DEL],
        vInParam, 
        this,
        EN_VSI_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::Del(%s) IProc->Handle failed. ret : %d.\n",
            vsi_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::Del(%s) proc return failed. ret : %d.\n",
            vsi_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVsi::DelByVmId(int64 vm_id)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVmid(CADOParameter::paramInt64, CADOParameter::paramInput, vm_id);    
    
    vInParam.push_back(inVmid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVsiProc[EN_VSI_PROC_DEL_BY_VMID],
        vInParam, 
        this,
        EN_VSI_PROC_DEL_BY_VMID,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::DelByVmId(%lld) IProc->Handle failed. ret : %d.\n",vm_id, ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::DelByVmId(%lld) proc return failed. ret : %d.\n",vm_id, GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVsi::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateVsi::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CanOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CanOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVsi::CanOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateVsi::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbSwitchSummary> vInfo;
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
    vector<CDbSwitchSummary>::iterator it = vInfo.begin();
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
#if 0
void db_query_vswitchsummary()
{    
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    vector<CDbSwitchSummary> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbSwitchSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_vswitchsummary);
#endif 

void db_query_vsi(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    CDbVsiInfo info;
    info.SetVsiIdValue(uuid);  
    int ret = pOper->QueryByVsi(info);
    if( ret != 0 )
    {
        cout << "CDBOperateVsi query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVsi query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vsi);

void db_query_vsi_by_vmnic(int64 vm_id, int32 nic_index)
{
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    CDbVsiInfo info;
    info.SetVmId(vm_id);  
    info.SetNicIndex(nic_index); 
    int ret = pOper->QueryByVmVnic(info);
    if( ret != 0 )
    {
        cout << "CDBOperateVsi query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVsi query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vsi_by_vmnic);


void db_query_vsisummary()
{
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    vector<CDbVsiInfoSummary> vinfo;
    int ret = pOper->QuerySummary( vinfo);
    if( ret != 0 )
    {
        cout << "CDBOperateVsi QuerySummary failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVsi QuerySummary success." << endl;
    vector<CDbVsiInfoSummary>::iterator it = vinfo.begin();
    for(; it != vinfo.end(); ++it)
    {  
        cout << "-----------------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vsisummary);

void db_query_vsisummary_by_vm(int64 vm_id)
{
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    vector<CDbVsiInfoSummary> vinfo;
    int ret = pOper->QuerySummaryByVm(vm_id, vinfo);
    if( ret != 0 )
    {
        cout << "CDBOperateVsi QuerySummaryByVm failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVsi QuerySummaryByVm success." << endl;
    vector<CDbVsiInfoSummary>::iterator it = vinfo.begin();
    for(; it != vinfo.end(); ++it)
    {  
        cout << "-----------------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vsisummary_by_vm);

/*
void db_check_add_vsi( int32 nic_index, char* ln_uuid, 
        char * vm_pg_uuid , char* mac, int32 is_has_switch, char * switch_uuid )
        */
void db_check_add_vsi( int32 nic_index, char* ln_uuid, 
        char * vm_pg_uuid , char* mac, int32 is_has_switch, char * qn_uuid,char * qp_uuid )        
{
  //  if( NULL == ln_uuid || NULL == vm_pg_uuid || NULL == mac || NULL == switch_uuid )
    if( NULL == ln_uuid || NULL == vm_pg_uuid || NULL == mac || NULL == qn_uuid || NULL == qp_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    CDbVsiInfo info;
    info.SetVmId(10);
    info.SetNicIndex(nic_index);
    info.SetLogicNetworkUuid(ln_uuid);
    info.SetVmPgUuid(vm_pg_uuid);
    info.SetMac(mac);
    info.SetIsHasVSwitch(is_has_switch);    
    //info.SetVSwitchUuid(switch_uuid);

    info.SetQuantumNetworkUuid(qn_uuid);
    info.SetQuantumPortUuid(qp_uuid);

    info.SetVsiIdFormat(5);

    //info.DbgShow();
    int ret = pOper->CheckAdd(info);
    if( ret != 0 )
    {
        cout << "CheckAdd failed." << ret << endl;
        return;
    }
    cout << "CheckAdd success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_vsi);

/*
void db_add_vsi( int32 nic_index,  char* ln_uuid, 
        char * vm_pg_uuid , char* mac, int32 is_has_switch, char * switch_uuid )
        */
void db_add_vsi( int32 nic_index,  char* ln_uuid, 
        char * vm_pg_uuid , char* mac, int32 is_has_switch, char * qn_uuid,char * qp_uuid )        
{
    // if( NULL == ln_uuid || NULL == vm_pg_uuid || NULL == mac || NULL == switch_uuid )
    if( NULL == ln_uuid || NULL == vm_pg_uuid || NULL == mac || NULL == qn_uuid || NULL == qp_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    CDbVsiInfo info;
    info.SetVmId(10);
    info.SetNicIndex(nic_index);
    info.SetLogicNetworkUuid(ln_uuid);
    info.SetVmPgUuid(vm_pg_uuid);
    info.SetMac(mac);
    info.SetIsHasVSwitch(is_has_switch);    
    //info.SetVSwitchUuid(switch_uuid);
    info.SetQuantumNetworkUuid(qn_uuid);
    info.SetQuantumPortUuid(qp_uuid);

    info.SetVsiIdFormat(5);

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
DEFINE_DEBUG_FUNC(db_add_vsi);
/*
void db_check_mod_vsi(char* vsiid, int32 nic_index,  char* ln_uuid, 
        char * vm_pg_uuid , char* mac, int32 is_has_switch, char * switch_uuid )
*/        
void db_check_mod_vsi(char* vsiid, int32 nic_index,  char* ln_uuid, 
        char * vm_pg_uuid , char* mac, char * qn_uuid,char * qp_uuid )        
{
    if( NULL == vsiid || NULL == ln_uuid || NULL == vm_pg_uuid || NULL == mac || NULL == qn_uuid || NULL == qp_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    CDbVsiInfo info;
    info.SetVmId(10);
    info.SetNicIndex(nic_index);
    info.SetLogicNetworkUuid(ln_uuid);
    info.SetVmPgUuid(vm_pg_uuid);
    info.SetMac(mac);
/*    
    info.SetIsHasVSwitch(is_has_switch);    
    info.SetVSwitchUuid(switch_uuid);
*/    
    info.SetQuantumNetworkUuid(qn_uuid);
    info.SetQuantumPortUuid(qp_uuid);
    
    info.SetVsiIdValue(vsiid);

    info.SetVsiIdFormat(5);
        
    //info.DbgShow();
    int ret = pOper->CheckModify(info);
    if( ret != 0 )
    {
        cout << "can Modify failed." << ret << endl;
        return;
    }
    cout << "can Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_vsi);

/*
void db_mod_vsi(char* vsiid, int32 nic_index, char* ln_uuid, 
        char * vm_pg_uuid , char* mac, int32 is_has_switch, char * switch_uuid )
*/        
void db_mod_vsi(char* vsiid, int32 nic_index, char* ln_uuid, 
        char * vm_pg_uuid , char* mac, char * qn_uuid,char * qp_uuid  )        
{
    if( NULL == vsiid || NULL == ln_uuid || NULL == vm_pg_uuid || NULL == mac || NULL == qn_uuid || NULL == qp_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    CDbVsiInfo info;
    info.SetVmId(10);
    info.SetNicIndex(nic_index);
    info.SetLogicNetworkUuid(ln_uuid);
    info.SetVmPgUuid(vm_pg_uuid);
    info.SetMac(mac);
/*    
    info.SetIsHasVSwitch(is_has_switch);    
    info.SetVSwitchUuid(switch_uuid);
*/    
    info.SetQuantumNetworkUuid(qn_uuid);
    info.SetQuantumPortUuid(qp_uuid);
    
    info.SetVsiIdValue(vsiid);

    info.SetVsiIdFormat(5);
        
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
DEFINE_DEBUG_FUNC(db_mod_vsi);

void db_check_del_vsi(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "CheckDel failed." << ret << endl;
        return;
    }
    cout << "CheckDel success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_vsi);

void db_del_vsi(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_vsi);

void db_del_vsi_by_vm(int64 vm_id)
{

    CDBOperateVsi * pOper = GetDbIVsi();
    if( NULL == pOper )
    {
        cout << "CDBOperateVsi is NULL" << endl;
        return ;
    }

    int ret = pOper->DelByVmId(vm_id);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_vsi_by_vm);


}



