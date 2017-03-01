

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_vswitch_map.h"
    
namespace zte_tecs
{
typedef enum tagVSwitchMapProc
{
    EN_VSWITCHMAP_PROC_QUERY_BY_SWITCH = 0,
    EN_VSWITCHMAP_PROC_QUERY_BY_VNA,
    EN_VSWITCHMAP_PROC_QUERY_BY_SWITCHVNA,   
    EN_VSWITCHMAP_PROC_QUERY_BY_PORT, 
    EN_VSWITCHMAP_PROC_CHECK_ADD,
    EN_VSWITCHMAP_PROC_ADD,
    EN_VSWITCHMAP_PROC_CHECK_MODIFY,
    EN_VSWITCHMAP_PROC_MODIFY,
    EN_VSWITCHMAP_PROC_CHECK_DEL,
    EN_VSWITCHMAP_PROC_DEL,
    EN_VSWITCHMAP_PROC_ALL,
}EN_VSWITCH_PROC;

const char *s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_ALL] = 
{        
    "pf_net_query_vswitchmap_by_switch",
    "pf_net_query_vswitchmap_by_vna",
    "pf_net_query_vswitchmap_by_switchvna",
    "pf_net_query_vswitchmap_by_port",    
    "pf_net_can_add_vswitchmap",  
    "pf_net_add_vswitchmap",  
    "pf_net_check_modify_vswitchmap",  
    "pf_net_modify_vswitchmap", 
    "pf_net_can_del_vswitchmap",  
    "pf_net_del_vswitchmap",  
};

    
CDBOperateVSwitchMap::CDBOperateVSwitchMap(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVSwitchMap::~CDBOperateVSwitchMap()
{
    
}

int32 CDBOperateVSwitchMap::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VSWITCHMAP_PROC_QUERY_BY_SWITCH:
        case EN_VSWITCHMAP_PROC_QUERY_BY_VNA:
        case EN_VSWITCHMAP_PROC_QUERY_BY_SWITCHVNA:
        case EN_VSWITCHMAP_PROC_QUERY_BY_PORT:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_VSWITCHMAP_PROC_ADD:
        case EN_VSWITCHMAP_PROC_DEL:
        case EN_VSWITCHMAP_PROC_MODIFY:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_VSWITCHMAP_PROC_CHECK_ADD:
        case EN_VSWITCHMAP_PROC_CHECK_DEL:
        case EN_VSWITCHMAP_PROC_CHECK_MODIFY:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateVSwitchMap::QueryBySwitch(const char* switch_uuid, vector<CDbVSwitchMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryBySwitch IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

     if( NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::QueryBySwitch switch_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_QUERY_BY_SWITCH],\
        vInParam, \
        this,\
        EN_VSWITCHMAP_PROC_QUERY_BY_SWITCH,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryBySwitch(%s) IProc->Handle failed(%d) \n",
                switch_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVSwitchMap::QueryByVna(const char* vna_uuid, vector<CDbVSwitchMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

     if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::QueryByVna vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);    
    
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_QUERY_BY_VNA],\
        vInParam, \
        this,\
        EN_VSWITCHMAP_PROC_QUERY_BY_VNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryByVna(%s) IProc->Handle failed(%d) \n",
                vna_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVSwitchMap::QueryBySwitchVna(const char* switch_uuid,const char* vna_uuid, vector<CDbVSwitchMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryBySwitchVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid || NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::QueryBySwitchVna vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inSWitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);   
    CADOParameter inVanUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);    
    
    vInParam.push_back(inSWitchUuid);
    vInParam.push_back(inVanUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_QUERY_BY_SWITCHVNA],\
        vInParam, \
        this,\
        EN_VSWITCHMAP_PROC_QUERY_BY_SWITCHVNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryBySwitchVna(%s,%s) IProc->Handle failed(%d) \n",
                switch_uuid,vna_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVSwitchMap::QueryByPort(const char* port_uuid, vector<CDbVSwitchMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryByPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid  )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::QueryByPort port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    
    
    vInParam.push_back(inPortUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_QUERY_BY_PORT],\
        vInParam, \
        this,\
        EN_VSWITCHMAP_PROC_QUERY_BY_PORT,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryByPort(%s) IProc->Handle failed(%d) \n",
                port_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}


int32 CDBOperateVSwitchMap::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVSwitchMap> *pvOut;
    pvOut = static_cast<vector<CDbVSwitchMap>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbVSwitchMap info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 13);
    info.SetVSwitchUuid(prs->GetFieldValue(0));
    info.SetVSwitchName(prs->GetFieldValue(1));   
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchType(value);

    info.SetVnaUuid(prs->GetFieldValue(3)); 
    info.SetVnaApp(prs->GetFieldValue(4));
    info.SetVnmApp(prs->GetFieldValue(5));
    info.SetHostName(prs->GetFieldValue(6));

    info.SetPortUuid(prs->GetFieldValue(7));
    info.SetPortName(prs->GetFieldValue(8));
    if( false == prs->GetFieldValue(9,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryCallback GetFieldValue 9 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(value);

    if( false == prs->GetFieldValue(10,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryCallback GetFieldValue 10 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchMapIsCfg(value);

    if( false == prs->GetFieldValue(11,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryCallback GetFieldValue 11 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchMapIsSelected(value);

    if( false == prs->GetFieldValue(12,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::QueryCallback GetFieldValue 12 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchMapIsConsistency(value);

    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateVSwitchMap::CheckAdd(const char* switch_uuid, const char* port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckAdd switch_uuid or port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    
      
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inPortUuid);
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_VSWITCHMAP_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {       
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::CheckAdd(%s,%s) IProc->Handle failed(%d).\n",
            switch_uuid,port_uuid,ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::CheckAdd(%s,%s) proc return failed. ret : %d.\n",
            switch_uuid,port_uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVSwitchMap::Add(const char* switch_uuid, const char* port_uuid, int32 is_cfg, int32 is_selected, int32 is_consistency)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::Add switch_uuid or port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, is_cfg);    
    CADOParameter inIsSelected(CADOParameter::paramInt, CADOParameter::paramInput, is_selected);  
    CADOParameter inIsConsistency(CADOParameter::paramInt, CADOParameter::paramInput, is_consistency);  
     
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inPortUuid);
    vInParam.push_back(inIsCfg);
    vInParam.push_back(inIsSelected);
    vInParam.push_back(inIsConsistency);    
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_ADD],
        vInParam, 
        this,
        EN_VSWITCHMAP_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::Add(%s,%s,%d,%d,%d) IProc->Handle failed(%d).\n", 
            switch_uuid,port_uuid,is_cfg,is_selected,is_consistency, ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::Add(%s,%s,%d,%d,%d)  proc return failed. ret : %d.\n",
            switch_uuid,port_uuid,is_cfg,is_selected,is_consistency,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateVSwitchMap::CheckModify(const char* switch_uuid, const char* port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckModify switch_uuid or port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    
      
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inPortUuid);
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_VSWITCHMAP_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::CheckModify(%s,%s) IProc->Handle failed(%d).\n",
            switch_uuid,port_uuid,ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::CheckModify(%s,%s) proc return failed. ret : %d.\n",
            switch_uuid,port_uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVSwitchMap::Modify(const char* switch_uuid, const char* port_uuid, int32 is_cfg, int32 is_selected, int32 is_consistency)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::Modify switch_uuid or port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, is_cfg);    
    CADOParameter inIsSelected(CADOParameter::paramInt, CADOParameter::paramInput, is_selected);  
    CADOParameter inIsConsistency(CADOParameter::paramInt, CADOParameter::paramInput, is_consistency);  
     
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inPortUuid);
    vInParam.push_back(inIsCfg);
    vInParam.push_back(inIsSelected);
    vInParam.push_back(inIsConsistency);    
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_MODIFY],
        vInParam, 
        this,
        EN_VSWITCHMAP_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::Modify(%s,%s,%d,%d,%d) IProc->Handle failed(%d).\n",
            switch_uuid,port_uuid,is_cfg, is_selected,is_consistency,ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVSwitchMap::Modify(%s,%s,%d,%d,%d) proc return failed. ret : %d.\n",
            switch_uuid,port_uuid,is_cfg, is_selected,is_consistency,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateVSwitchMap::CheckDel(const char* switch_uuid, const char * port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid || NULL == port_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckDel switch_uuid or port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    

    
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inPortUuid);    

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_VSWITCHMAP_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckDel(%s,%s) IProc->Handle failed. ret : %d.\n",
            switch_uuid,port_uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckDel(%s,%s) proc return failed. ret : %d.\n",   
           switch_uuid,port_uuid, GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateVSwitchMap::Del(const char* switch_uuid, const char * port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid || NULL == port_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::Del switch_uuid or port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    CADOParameter inPortUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);    

    
    vInParam.push_back(inSwitchUuid);
    vInParam.push_back(inPortUuid);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVSwitchMapProc[EN_VSWITCHMAP_PROC_DEL],
        vInParam, 
        this,
        EN_VSWITCHMAP_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::Del(%s,%s) IProc->Handle failed. ret : %d.\n",
            switch_uuid,port_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::Del(%s,%s) proc return failed. ret : %d.\n",
            switch_uuid,port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVSwitchMap::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateVSwitchMap::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVSwitchMap::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateVSwitchMap::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbVSwitchMap> vInfo;
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
    vector<CDbVSwitchMap>::iterator it = vInfo.begin();
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
void db_query_vswitchmap_by_switch(const char* switch_uuid)
{
    if( NULL == switch_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    vector<CDbVSwitchMap> outVInfo;
    int ret = pOper->QueryBySwitch(switch_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryBySwitch failed." << ret << endl;
        return;
    }
    cout << "QueryBySwitch success." << endl;
    vector<CDbVSwitchMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vswitchmap_by_switch);

void db_query_vswitchmap_by_vna(const char* vna_uuid)
{
    if( NULL == vna_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    vector<CDbVSwitchMap> outVInfo;
    int ret = pOper->QueryByVna(vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByVna failed." << ret << endl;
        return;
    }
    cout << "QueryByVna success." << endl;
    vector<CDbVSwitchMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vswitchmap_by_vna);

void db_query_vswitchmap_by_switchvna(const char* switch_uuid, const char* vna_uuid)
{
    if( NULL == switch_uuid || NULL == vna_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    vector<CDbVSwitchMap> outVInfo;
    int ret = pOper->QueryBySwitchVna(switch_uuid,vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryBySwitchVna failed." << ret << endl;
        return;
    }
    cout << "QueryBySwitchVna success." << endl;
    vector<CDbVSwitchMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vswitchmap_by_switchvna);


void db_query_vswitchmap_by_port( const char* port_uuid)
{
    if( NULL == port_uuid)
    {
        cout << "port_uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    vector<CDbVSwitchMap> outVInfo;
    int ret = pOper->QueryByPort(port_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByPort failed." << ret << endl;
        return;
    }
    cout << "QueryByPort success." << endl;
    vector<CDbVSwitchMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_vswitchmap_by_port);

void db_can_add_vswitchmap(char* switch_uuid,const char* port_uuid)
{
    if( NULL == switch_uuid || NULL == port_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    
    //info.DbgShow();
    int ret = pOper->CheckAdd(switch_uuid,port_uuid);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
}
DEFINE_DEBUG_FUNC(db_can_add_vswitchmap);



void db_add_vswitchmap(char* switch_uuid,const char* port_uuid)
{
    if( NULL == switch_uuid || NULL == port_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    
    //info.DbgShow();
    int ret = pOper->Add(switch_uuid,port_uuid,0,0,1);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
}
DEFINE_DEBUG_FUNC(db_add_vswitchmap);

void db_can_del_vswitchmap(char* switch_uuid,const char* port_uuid)
{
    if( NULL == switch_uuid || NULL == port_uuid)
    {
        cout << "port uuid is NULL" << endl;
        return;
    }
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }
    
    //info.DbgShow();
    int ret = pOper->CheckDel(switch_uuid,port_uuid);
    if( ret != 0 )
    {
        cout << "del failed." << ret << endl;
        return;
    }
    cout << "del success." << endl;
}
DEFINE_DEBUG_FUNC(db_can_del_vswitchmap);

void db_del_vswitchmap(char* switch_uuid,const char* port_uuid)
{
    if( NULL == switch_uuid || NULL == port_uuid)
    {
        cout << "port uuid is NULL" << endl;
        return;
    }
    CDBOperateVSwitchMap * pOper = GetDbIVSwitchMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVSwitchMap is NULL" << endl;
        return ;
    }

    
    //info.DbgShow();
    int ret = pOper->Del(switch_uuid,port_uuid);
    if( ret != 0 )
    {
        cout << "del failed." << ret << endl;
        return;
    }
    cout << "del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_vswitchmap);


}



