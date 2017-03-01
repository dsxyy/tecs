

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_vswitch.h"
    
namespace zte_tecs
{
typedef enum tagVSwitchProc
{
    EN_VSWITCH_PROC_QUERYS_VSI_NUM = 0,
    EN_VSWITCH_PROC_QUERYS_VSI_NUM_BY_SWITCHVNA,

    EN_VSWITCH_PROC_QUERYS_KERNEL_NUM,
    EN_VSWITCH_PROC_QUERYS_KERNEL_NUM_BY_SWITCHVNA,
    
    EN_VSWITCH_PROC_QUERYSUMMARY,
    EN_VSWITCH_PROC_QUERY,   
    EN_VSWITCH_PROC_SDN_QUERYSUMMARY,       
    EN_VSWITCH_PROC_CHECK_ADD,
    EN_VSWITCH_PROC_ADD,
    EN_VSWITCH_PROC_CHECK_MODIFY,
    EN_VSWITCH_PROC_MODIFY,
    EN_VSWITCH_PROC_CHECK_DEL,
    EN_VSWITCH_PROC_DEL,
    EN_VSWITCH_PROC_ALL,
}EN_VSWITCH_PROC;

const char *s_astrVSwitchProc[EN_VSWITCH_PROC_ALL] = 
{        
    "pf_net_query_vswitch_vsi_num",
    "pf_net_query_vswitchvna_vsi_num",  
    "pf_net_query_vswitch_kernel_num",
    "pf_net_query_vswitchvna_kernel_num", 
    "pf_net_query_vswitch_summary",
    "pf_net_query_vswitch",
    "pf_net_query_sdn_vswitch_summary",    
    "pf_net_can_add_vswitch",  
    "pf_net_add_vswitch",    
    "pf_net_can_modify_vswitch", 
    "pf_net_modify_vswitch", 
    "pf_net_can_del_vswitch",  
    "pf_net_del_vswitch",  
};

    
CDBOperateVSwitch::CDBOperateVSwitch(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVSwitch::~CDBOperateVSwitch()
{
    
}

int32 CDBOperateVSwitch::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VSWITCH_PROC_QUERYS_VSI_NUM:
        case EN_VSWITCH_PROC_QUERYS_VSI_NUM_BY_SWITCHVNA:
        case EN_VSWITCH_PROC_QUERYS_KERNEL_NUM:
        case EN_VSWITCH_PROC_QUERYS_KERNEL_NUM_BY_SWITCHVNA:
        {
            return QueryVsiKernelNumCallback(prs,nil);
        }
        break;
        case EN_VSWITCH_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        case EN_VSWITCH_PROC_SDN_QUERYSUMMARY:
        {
            return QuerySdnSummaryCallback(prs,nil);
        }        
        break;
        case EN_VSWITCH_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_VSWITCH_PROC_ADD:
        case EN_VSWITCH_PROC_MODIFY:
        case EN_VSWITCH_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_VSWITCH_PROC_CHECK_ADD:
        case EN_VSWITCH_PROC_CHECK_MODIFY:
        case EN_VSWITCH_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateVSwitch::QuerySummary(vector<CDbSwitchSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVSwitchProc[EN_VSWITCH_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_VSWITCH_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVSwitch::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbSwitchSummary> *pvOut;
    pvOut = static_cast<vector<CDbSwitchSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbSwitchSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
    info.SetUuid(prs->GetFieldValue(0));
    info.SetName(prs->GetFieldValue(1));   
    int32 switch_type = 0;
    if( false == prs->GetFieldValue(2,switch_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchType(switch_type);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateVSwitch::QuerySdnSummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QuerySdnSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbSwitchSummary> *pvOut;
    pvOut = static_cast<vector<CDbSwitchSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QuerySdnSummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbSwitchSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);
    info.SetName(prs->GetFieldValue(0));   

    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateVSwitch::QueryVsiNum( const char* switch_uuid, int32 & outVsiNum )
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiNum switch_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    CDbSwitchVsiKernelNum  Info;
    
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_QUERYS_VSI_NUM],
        vInParam, 
        this,
        EN_VSWITCH_PROC_QUERYS_VSI_NUM,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryVsiNum(%s) IProc->Handle failed(%d).\n",
                switch_uuid,ret);
        }
        return ret;
    }
    
    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiNum(%s) proc return failed. ret : %d.\n", 
                switch_uuid,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置vsi num 值
    outVsiNum = Info.GetVsiNum();
    
    return 0;
}

int32 CDBOperateVSwitch::QueryVsiNumBySwitchVna( const char* switch_uuid, const char* vna_uuid, int32 & outVsiNum ) 
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiNumBySwitchVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid || NULL == vna_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiNumBySwitchVna switch_uuid/vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    CDbSwitchVsiKernelNum  Info;
    
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_QUERYS_VSI_NUM_BY_SWITCHVNA],
        vInParam, 
        this,
        EN_VSWITCH_PROC_QUERYS_VSI_NUM_BY_SWITCHVNA,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryVsiNumBySwitchVna(%s,%s) IProc->Handle failed(%d).\n",
                switch_uuid,vna_uuid,ret);
        }
        return ret;
    }
    
    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(Info.GetRet()) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiNumBySwitchVna(%s) proc return failed. ret : %d.\n", 
                switch_uuid,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置vsi num 值
    outVsiNum = Info.GetVsiNum();
    
    return 0;
}



int32 CDBOperateVSwitch::QueryKernelNum( const char* switch_uuid, int32 & outVsiNum )
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryKernelNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryKernelNum switch_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    CDbSwitchVsiKernelNum  Info;
    
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_QUERYS_KERNEL_NUM],
        vInParam, 
        this,
        EN_VSWITCH_PROC_QUERYS_KERNEL_NUM,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryKernelNum(%s) IProc->Handle failed(%d).\n",
                switch_uuid,ret);
        }
        return ret;
    }
    
    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryKernelNum(%s) proc return failed. ret : %d.\n", 
                switch_uuid,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置vsi num 值
    outVsiNum = Info.GetVsiNum();
    
    return 0;
}


int32 CDBOperateVSwitch::QueryKernelNumBySwitchVna( const char* switch_uuid, const char* vna_uuid, int32 & outVsiNum ) 
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryKernelNumBySwitchVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid || NULL == vna_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryKernelNumBySwitchVna switch_uuid/vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    CDbSwitchVsiKernelNum  Info;
    
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);
    vInParam.push_back(inUuid);
    vInParam.push_back(inVnaUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_QUERYS_KERNEL_NUM_BY_SWITCHVNA],
        vInParam, 
        this,
        EN_VSWITCH_PROC_QUERYS_KERNEL_NUM_BY_SWITCHVNA,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryKernelNumBySwitchVna(%s,%s) IProc->Handle failed(%d).\n",
                switch_uuid,vna_uuid,ret);
        }
        return ret;
    }
    
    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryKernelNumBySwitchVna(%s,%s) proc return failed. ret : %d.\n", 
                switch_uuid,vna_uuid,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置vsi num 值
    outVsiNum = Info.GetVsiNum();
    
    return 0;
}

int32 CDBOperateVSwitch::Query(CDbVSwitch & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_QUERY],
        vInParam, 
        this,
        EN_VSWITCH_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateVSwitch::SdnQuerySummary(vector<CDbSwitchSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::SdnQuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVSwitchProc[EN_VSWITCH_PROC_SDN_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_VSWITCH_PROC_SDN_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::SdnQuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
     
    return 0;
}


int32 CDBOperateVSwitch::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (DB_SWITCH_TABLE_FIELD_NUM +5));    

    // 1 获取port 信息
    int32 proc_ret = 0;
    proc_ret = QuerySwitchBaseCallback(prs,nil);
    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryCallback QueryPortBaseCallback proc failed(%d).\n", (proc_ret));
        return (proc_ret);
    }

    // 2 获取kernel 信息
    CDbVSwitch *pOutPort;
    pOutPort = static_cast<CDbVSwitch *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // port 5        
    int32 values = 0;
    if( false == prs->GetFieldValue(DB_SWITCH_TABLE_FIELD_NUM,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryCallback GetFieldValue 0 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetEvbMode(values);  
    (*pOutPort).SetPgUuid( prs->GetFieldValue(DB_SWITCH_TABLE_FIELD_NUM + 1));
    
    if( false == prs->GetFieldValue(DB_SWITCH_TABLE_FIELD_NUM + 2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetNicMaxNumCfg(values); 

    if( false == prs->GetFieldValue(DB_SWITCH_TABLE_FIELD_NUM + 3,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::QueryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsActive(values); 

    (*pOutPort).SetSdnCfgUuid( prs->GetFieldValue(DB_SWITCH_TABLE_FIELD_NUM + 4));

    return 0;
}



int32 CDBOperateVSwitch::CheckAdd(CDbVSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_SWITCH_TYPE_EDVS!= info.GetSwitchType() && 
        EN_DB_SWITCH_TYPE_SDVS != info.GetSwitchType())
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckAdd type(%d) is invalid.\n",info.GetSwitchType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());
    CADOParameter inMaxMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMaxMtu());   
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());   
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inNicMaxNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicMaxNumCfg());    
    CADOParameter inIsActive(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsActive()); 
    CADOParameter inSdnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSdnCfgUuid().c_str());    
    
    vInParam.push_back(inSwitchType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inMaxMtu);        
    vInParam.push_back(inEvbMode);    
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inIsActive);   
    vInParam.push_back(inSdnUuid);     
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_VSWITCH_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVSwitch::Add(CDbVSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_SWITCH_TYPE_EDVS!= info.GetSwitchType() && 
        EN_DB_SWITCH_TYPE_SDVS != info.GetSwitchType())
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Add type(%d) is invalid.\n",info.GetSwitchType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());
    CADOParameter inMaxMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMaxMtu());   
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());   
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inNicMaxNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicMaxNumCfg());    
    CADOParameter inIsActive(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsActive()); 
    CADOParameter inSdnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSdnCfgUuid().c_str());    
    
    vInParam.push_back(inSwitchType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inMaxMtu);        
    vInParam.push_back(inEvbMode);    
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inIsActive);  
    vInParam.push_back(inSdnUuid); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_ADD],
        vInParam, 
        this,
        EN_VSWITCH_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVSwitch::CheckModify(CDbVSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_SWITCH_TYPE_EDVS!= info.GetSwitchType() && 
        EN_DB_SWITCH_TYPE_SDVS != info.GetSwitchType())
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckModify type(%d) is invalid.\n",info.GetSwitchType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }


    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());
    CADOParameter inMaxMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMaxMtu());   
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());   
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inNicMaxNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicMaxNumCfg());    
    CADOParameter inIsActive(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsActive()); 
    CADOParameter inSdnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSdnCfgUuid().c_str());    
    
    vInParam.push_back(inSwitchType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inMaxMtu);        
    vInParam.push_back(inEvbMode);    
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inIsActive); 
    vInParam.push_back(inSdnUuid); 

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_VSWITCH_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckModify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateVSwitch::Modify(CDbVSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_SWITCH_TYPE_EDVS!= info.GetSwitchType() && 
        EN_DB_SWITCH_TYPE_SDVS != info.GetSwitchType())
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Modify type(%d) is invalid.\n",info.GetSwitchType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if(0)
    {
       CDbVSwitch  tmpinfo;
       tmpinfo.SetUuid(info.GetUuid().c_str());
       int32 ret = Query(tmpinfo);
       if( ret == 0 )
       {    
            cout << "CDBOperateVSwitch::Modify ===== begin query" << endl;
            tmpinfo.DbgShow();
            cout << "CDBOperateVSwitch::Modify ===== end query" << endl;            
       }

       cout << "CDBOperateVSwitch::Modify" << endl;
       info.DbgShow();
       cout << "CDBOperateVSwitch::Modify" << endl;      
    }
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());
    CADOParameter inMaxMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMaxMtu());   
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());   
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inNicMaxNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetNicMaxNumCfg());    
    CADOParameter inIsActive(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsActive()); 
    CADOParameter inSdnUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSdnCfgUuid().c_str());    
    
    vInParam.push_back(inSwitchType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inMaxMtu);        
    vInParam.push_back(inEvbMode);    
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inIsActive);  
    vInParam.push_back(inSdnUuid); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_MODIFY],
        vInParam, 
        this,
        EN_VSWITCH_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitch::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateVSwitch::CheckDel(const char* switch_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckDel switch_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_VSWITCH_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            switch_uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckDel(%s) proc return failed. ret : %d.\n", 
            switch_uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateVSwitch::Del(const char* switch_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::Del switch_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchProc[EN_VSWITCH_PROC_DEL],
        vInParam, 
        this,
        EN_VSWITCH_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::Del(%s) IProc->Handle failed. ret : %d.\n",
            switch_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::Del(%s) proc return failed. ret : %d.\n",
            switch_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVSwitch::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateVSwitch::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


int32 CDBOperateVSwitch::QueryVsiKernelNumCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiKernelNumCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbSwitchVsiKernelNum *proc_ret = NULL;     
    proc_ret = static_cast<CDbSwitchVsiKernelNum *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiKernelNumCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiKernelNumCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitch::QueryVsiKernelNumCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetVsiNum(value);

    return 0;
}


void CDBOperateVSwitch::DbgShow()
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
void db_query_vswitch_vsi_num(const char* switch_uuid)
{
    if( NULL == switch_uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    int32 vsi_num = 0;
    int ret = pOper->QueryVsiNum(switch_uuid,vsi_num);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << vsi_num << endl;
}
DEFINE_DEBUG_FUNC(db_query_vswitch_vsi_num);

void db_query_vswitchvna_vsi_num(const char* switch_uuid, const char* vna_uuid)
{
    if( NULL == switch_uuid || NULL == vna_uuid)
    {
        cout << "switch_uuid/vna_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    int32 vsi_num = 0;
    int ret = pOper->QueryVsiNumBySwitchVna(switch_uuid,vna_uuid,vsi_num);
    if( ret != 0 )
    {
        cout << "QueryVsiNumBySwitchVna failed." << ret << endl;
        return;
    }
    cout << "QueryVsiNumBySwitchVna success." << vsi_num << endl;
}
DEFINE_DEBUG_FUNC(db_query_vswitchvna_vsi_num);


void db_query_vswitch_kernel_num(const char* switch_uuid)
{
    if( NULL == switch_uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    int32 vsi_num = 0;
    int ret = pOper->QueryKernelNum(switch_uuid,vsi_num);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << vsi_num << endl;
}
DEFINE_DEBUG_FUNC(db_query_vswitch_kernel_num);

void db_query_vswitchvna_kernel_num(const char* switch_uuid, const char* vna_uuid)
{
    if( NULL == switch_uuid || NULL == vna_uuid)
    {
        cout << "switch_uuid/vna_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    int32 vsi_num = 0;
    int ret = pOper->QueryKernelNumBySwitchVna(switch_uuid,vna_uuid,vsi_num);
    if( ret != 0 )
    {
        cout << "QueryVsiNumBySwitchVna failed." << ret << endl;
        return;
    }
    cout << "QueryVsiNumBySwitchVna success." << vsi_num << endl;
}
DEFINE_DEBUG_FUNC(db_query_vswitchvna_kernel_num);

void db_query_vswitchsummary()
{    
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
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

void db_query_vswitch(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    CDbVSwitch info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateVSwitch query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateVSwitch query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vswitch);


void db_can_add_vswtich(int32 switch_type, char* name, char* uuid,int32 state, int32 maxMtu, 
        char * pgUuid, int32 nicMaxNum, char* sdnUuid )
{
    if( NULL == uuid || NULL == name || NULL == pgUuid || NULL == sdnUuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    CDbVSwitch info;
    info.SetSwitchType(switch_type);
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetState(state);
    info.SetMaxMtu(maxMtu);

    info.SetPgUuid(pgUuid);
    info.SetNicMaxNumCfg(nicMaxNum);
    info.SetSdnCfgUuid(sdnUuid);

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
DEFINE_DEBUG_FUNC(db_can_add_vswtich);

void db_add_vswtich(int32 switch_type, char* name,int32 state, int32 maxMtu, 
        char * pgUuid, int32 nicMaxNum,char* sdnUuid )
{
    if(  NULL == name || NULL == pgUuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    CDbVSwitch info;
    info.SetSwitchType(switch_type);
    info.SetName(name);
    info.SetState(state);
    info.SetMaxMtu(maxMtu);

    info.SetPgUuid(pgUuid);
    info.SetNicMaxNumCfg(nicMaxNum);
    info.SetSdnCfgUuid(sdnUuid);

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
DEFINE_DEBUG_FUNC(db_add_vswtich);

void db_can_mod_vswtich(int32 switch_type, char* name, char* uuid,int32 state, int32 maxMtu, 
        char * pgUuid, int32 nicMaxNum,char* sdnUuid )
{
    if( NULL == uuid || NULL == name || NULL == pgUuid || NULL == sdnUuid)
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    CDbVSwitch info;
    info.SetSwitchType(switch_type);
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetState(state);
    info.SetMaxMtu(maxMtu);

    info.SetPgUuid(pgUuid);
    info.SetNicMaxNumCfg(nicMaxNum);
    info.SetSdnCfgUuid(sdnUuid);
        
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
DEFINE_DEBUG_FUNC(db_can_mod_vswtich);

void db_mod_vswtich(int32 switch_type, char* name, char* uuid,int32 state, int32 maxMtu, 
        char * pgUuid, int32 nicMaxNum,char* sdnUuid  )
{
    if( NULL == uuid || NULL == name || NULL == pgUuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    CDbVSwitch info;
    info.SetSwitchType(switch_type);
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetState(state);
    info.SetMaxMtu(maxMtu);

    info.SetPgUuid(pgUuid);
    info.SetNicMaxNumCfg(nicMaxNum);
    info.SetSdnCfgUuid(sdnUuid);
        
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
DEFINE_DEBUG_FUNC(db_mod_vswtich);

void db_can_del_vswtich(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "can Del failed." << ret << endl;
        return;
    }
    cout << "can Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_can_del_vswtich);

void db_del_vswtich(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateVSwitch * pOper = GetDbIVSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitch is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_vswtich);


}



