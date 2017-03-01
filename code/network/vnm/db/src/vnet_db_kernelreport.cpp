

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_kernelreport.h"
    
namespace zte_tecs
{
typedef enum tagKernelReportProc
{
    EN_KERNELREPORT_PROC_QUERY_BY_VNA = 0,
    EN_KERNELREPORT_PROC_QUERY,   
    EN_KERNELREPORT_PROC_CHECK_ADD,
    EN_KERNELREPORT_PROC_ADD,
    EN_KERNELREPORT_PROC_CHECK_MODIFY,
    EN_KERNELREPORT_PROC_MODIFY,
    EN_KERNELREPORT_PROC_CHECK_DEL,
    EN_KERNELREPORT_PROC_DEL,

    EN_KERNELREPORT_PROC_BONDMAP_QUERY_BY_VNA,
    EN_KERNELREPORT_PROC_BONDMAP_QUERY,   
    EN_KERNELREPORT_PROC_BONDMAP_CHECK_ADD,
    EN_KERNELREPORT_PROC_BONDMAP_ADD,
    EN_KERNELREPORT_PROC_BONDMAP_CHECK_DEL,
    EN_KERNELREPORT_PROC_BONDMAP_DEL,
    
    EN_KERNELREPORT_PROC_ALL,
}EN_KERNELREPORT_PROC;

const char *s_astrKernelReportProc[EN_KERNELREPORT_PROC_ALL] = 
{        
    "pf_net_query_kernelreport_by_vna",
    "pf_net_query_kernelreport",
    "pf_net_check_add_kernelreport",  
    "pf_net_add_kernelreport",    
    "pf_net_check_modify_kernelreport", 
    "pf_net_modify_kernelreport", 
    "pf_net_check_del_kernelreport",  
    "pf_net_del_kernelreport",  

    "pf_net_query_kernelreport_bondmap_by_vna",
    "pf_net_query_kernelreport_bondmap",
    "pf_net_check_add_kernelreport_bondmap",
    "pf_net_add_kernelreport_bondmap",
    "pf_net_check_del_kernelreport_bond_map",
    "pf_net_del_kernelreport_bond_map",
};

    
CDBOperateKernelReport::CDBOperateKernelReport(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateKernelReport::~CDBOperateKernelReport()
{
    
}

int32 CDBOperateKernelReport::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_KERNELREPORT_PROC_QUERY_BY_VNA:
        case EN_KERNELREPORT_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_KERNELREPORT_PROC_BONDMAP_QUERY_BY_VNA:
        case EN_KERNELREPORT_PROC_BONDMAP_QUERY:
        {
            return QueryBondMapCallback(prs,nil);
        }
        break;
        case EN_KERNELREPORT_PROC_ADD:
        case EN_KERNELREPORT_PROC_MODIFY:
        case EN_KERNELREPORT_PROC_DEL:
        case EN_KERNELREPORT_PROC_BONDMAP_ADD:
        case EN_KERNELREPORT_PROC_BONDMAP_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_KERNELREPORT_PROC_CHECK_ADD:
        case EN_KERNELREPORT_PROC_CHECK_MODIFY:
        case EN_KERNELREPORT_PROC_CHECK_DEL:
        case EN_KERNELREPORT_PROC_BONDMAP_CHECK_ADD:
        case EN_KERNELREPORT_PROC_BONDMAP_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateKernelReport::QueryByVna(CDbKernelReport & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetVnaUuid().c_str());
    vInParam.push_back(inVnaUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_QUERY_BY_VNA],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryByVna IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateKernelReport::Query(CDbKernelReport & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_QUERY],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateKernelReport::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 
    CDbKernelReport *pOutPort;
    pOutPort = static_cast<CDbKernelReport *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (11));    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    (*pOutPort).SetVnaUuid( prs->GetFieldValue(1) );
    (*pOutPort).SetUuid( prs->GetFieldValue(2) );    
    (*pOutPort).SetSwitchName( prs->GetFieldValue(3) );
    (*pOutPort).SetIp( prs->GetFieldValue(4) );    
    (*pOutPort).SetMask( prs->GetFieldValue(5) );
    
    // port 5        
    int32 values = 0;
    
    if( false == prs->GetFieldValue(6,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsOnline(values);  
    
    (*pOutPort).SetKernelPortName( prs->GetFieldValue(7) );
    (*pOutPort).SetUplinkPortName( prs->GetFieldValue(8) );  
    
    if( false == prs->GetFieldValue(9,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryCallback GetFieldValue 9 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetUplinkPortType(values); 

    if( false == prs->GetFieldValue(10,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryCallback GetFieldValue 10 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsDhcp(values);


    return 0;
}

int32 CDBOperateKernelReport::CheckAdd(CDbKernelReport & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());   
    CADOParameter inKernelPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetKernelPortName().c_str());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inIp);    
    vInParam.push_back(inMask);         
    vInParam.push_back(inIsOnline);     
    vInParam.push_back(inKernelPortName);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inIsDhcp);    
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
             info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateKernelReport::Add(CDbKernelReport & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());   
    CADOParameter inKernelPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetKernelPortName().c_str());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inIp);    
    vInParam.push_back(inMask);         
    vInParam.push_back(inIsOnline);     
    vInParam.push_back(inKernelPortName);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inIsDhcp); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_ADD],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateKernelReport::CheckModify(CDbKernelReport & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());   
    CADOParameter inKernelPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetKernelPortName().c_str());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inIp);    
    vInParam.push_back(inMask);         
    vInParam.push_back(inIsOnline);     
    vInParam.push_back(inKernelPortName);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inIsDhcp);

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckModify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateKernelReport::Modify(CDbKernelReport & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());   
    CADOParameter inKernelPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetKernelPortName().c_str());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());    
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inIp);    
    vInParam.push_back(inMask);         
    vInParam.push_back(inIsOnline);     
    vInParam.push_back(inKernelPortName);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inIsDhcp);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_MODIFY],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateKernelReport::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDel switch_uuid is NULL.\n");
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
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {        
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDel(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateKernelReport::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::Del uuid is NULL.\n");
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
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_DEL],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::Del(%s) proc return failed. ret : %d.\n",
            uuid, GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateKernelReport::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateKernelReport::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CanOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CanOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CanOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateKernelReport::DbgShow()
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

void db_query_kernelreport(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    CDbKernelReport info;
    info.SetUuid(uuid);
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_query_kernelreport);

void db_query_kernelreport_by_vna(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    CDbKernelReport info;
    info.SetVnaUuid(uuid);
    int ret = pOper->QueryByVna(info);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_query_kernelreport_by_vna);



void db_check_add_kernelreport(char* vna_uuid, char* switch_name, char* ip,char* mask, 
         char* kernel_port_name, char* uplink_port_name, int32 uplink_type, int32 is_online)
{
    if( NULL == vna_uuid || NULL == switch_name || NULL == ip || NULL == mask ||
       NULL == kernel_port_name || NULL == uplink_port_name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    CDbKernelReport info;
    info.SetVnaUuid(vna_uuid);
    info.SetSwitchName(switch_name);
    info.SetIp(ip);
    info.SetMask(mask);
    info.SetKernelPortName(kernel_port_name);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_type);
    info.SetIsOnline(is_online);

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
DEFINE_DEBUG_FUNC(db_check_add_kernelreport);

void db_add_kernelreport(char* vna_uuid, char* switch_name, char* ip,char* mask, 
         char* kernel_port_name, char* uplink_port_name, int32 uplink_type, int32 is_online)
{
    if( NULL == vna_uuid || NULL == switch_name || NULL == ip || NULL == mask ||
       NULL == kernel_port_name || NULL == uplink_port_name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    CDbKernelReport info;
    info.SetVnaUuid(vna_uuid);
    info.SetSwitchName(switch_name);
    info.SetIp(ip);
    info.SetMask(mask);
    info.SetKernelPortName(kernel_port_name);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_type);
    info.SetIsOnline(is_online);

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
DEFINE_DEBUG_FUNC(db_add_kernelreport);



void db_check_mod_kernelreport(char* vna_uuid, char* uuid, char* switch_name, char* ip,char* mask, 
         char* kernel_port_name, char* uplink_port_name, int32 uplink_type)
{
    if( NULL == vna_uuid || NULL == switch_name || NULL == ip || NULL == mask ||
       NULL == kernel_port_name || NULL == uplink_port_name || NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    CDbKernelReport info;
    info.SetVnaUuid(vna_uuid);
    info.SetUuid(uuid);
    info.SetSwitchName(switch_name);
    info.SetIp(ip);
    info.SetMask(mask);
    info.SetKernelPortName(kernel_port_name);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_type);

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
DEFINE_DEBUG_FUNC(db_check_mod_kernelreport);

void db_mod_kernelreport(char* vna_uuid, char* uuid, char* switch_name, char* ip,char* mask, 
         char* kernel_port_name, char* uplink_port_name, int32 uplink_type)
{
    if( NULL == vna_uuid || NULL == switch_name || NULL == ip || NULL == mask ||
       NULL == kernel_port_name || NULL == uplink_port_name || NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    CDbKernelReport info;
    info.SetVnaUuid(vna_uuid);
    info.SetUuid(uuid);
    info.SetSwitchName(switch_name);
    info.SetIp(ip);
    info.SetMask(mask);
    info.SetKernelPortName(kernel_port_name);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_type);

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
DEFINE_DEBUG_FUNC(db_mod_kernelreport);


void db_check_del_kernelreport(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_check_del_kernelreport);

void db_del_kernelreport(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_kernelreport);


int32 CDBOperateKernelReport::QueryBondMapByVna(const char* uuid, vector<CDbKernelReportBondMap> &outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryBondMapByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryBondMapByVna uuid is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inVnaUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_BONDMAP_QUERY_BY_VNA],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_BONDMAP_QUERY_BY_VNA,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryBondMapByVna IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateKernelReport::QueryBondMap(const char* uuid, vector<CDbKernelReportBondMap> &outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryBondMapByVna uuid is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_BONDMAP_QUERY],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_BONDMAP_QUERY,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryBondMap IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateKernelReport::QueryBondMapCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryBondMapCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryBondMapCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 
    vector<CDbKernelReportBondMap>  *pOutPort;
    pOutPort = static_cast<vector<CDbKernelReportBondMap>  *>(nil);
    
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::QueryBondMapCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbKernelReportBondMap info;
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (6));    

    info.SetVnaUuid( prs->GetFieldValue(0) );
    info.SetUuid( prs->GetFieldValue(1) );    
    info.SetSwitchName( prs->GetFieldValue(2) );
    info.SetUplinkPortName( prs->GetFieldValue(3) );  
    
    // port 5        
    int32 values = 0;
    if( false == prs->GetFieldValue(4,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::QueryBondMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondMode(values);  

    info.SetBondPhyName( prs->GetFieldValue(5) ); 
        
    (*pOutPort).push_back(info);  

    return 0;
}

int32 CDBOperateKernelReport::CheckAddBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckAddBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckAddBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_BONDMAP_CHECK_ADD],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_BONDMAP_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckAddBondMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::CheckAddBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateKernelReport::AddBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::AddBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::AddBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_BONDMAP_ADD],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_BONDMAP_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::AddBondMap IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateKernelReport::AddBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateKernelReport::CheckDelBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDelBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDelBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_BONDMAP_CHECK_DEL],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_BONDMAP_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDelBondMap IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::CheckDelBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateKernelReport::DelBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::DelBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::DelBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrKernelReportProc[EN_KERNELREPORT_PROC_BONDMAP_DEL],
        vInParam, 
        this,
        EN_KERNELREPORT_PROC_BONDMAP_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::DelBondMap IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateKernelReport::DelBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}


void db_query_kr_bondmap(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    vector<CDbKernelReportBondMap> vInfo;
    int ret = pOper->QueryBondMap(uuid,vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbKernelReportBondMap>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {        
        cout << "--------------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_kr_bondmap);

void db_query_kr_bondmap_by_vna(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    vector<CDbKernelReportBondMap> vInfo;
    int ret = pOper->QueryBondMapByVna(uuid,vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbKernelReportBondMap>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {        
        cout << "--------------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_kr_bondmap_by_vna);


void db_check_add_krbondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->CheckAddBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "CheckAddBondMap failed." << ret << endl;
        return;
    }
    cout << "CheckAddBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_add_krbondmap);

void db_add_krbondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->AddBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "AddBondMap failed." << ret << endl;
        return;
    }
    cout << "AddBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_add_krbondmap);


void db_check_del_krbondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->CheckDelBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "CheckDelBondMap failed." << ret << endl;
        return;
    }
    cout << "CheckDelBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_krbondmap);

void db_del_krbondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if( NULL == pOper )
    {
        cout << "CDBOperateKernelReport is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->DelBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "DelBondMap failed." << ret << endl;
        return;
    }
    cout << "DelBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_krbondmap);



}



