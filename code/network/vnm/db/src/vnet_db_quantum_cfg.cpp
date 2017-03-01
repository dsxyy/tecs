
#include "vnet_db_quantum_cfg.h"
#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagSdnCfgProc
{
    EN_SDNCFG_PROC_QUERY_SDNCTRLALL = 0,
    EN_SDNCFG_PROC_ADD_SDNCTRL,
    EN_SDNCFG_PROC_MODIFY_SDNCTRL,
    EN_SDNCFG_PROC_DEL_SDNCTRL,

    EN_SDNCFG_PROC_QUERY_QUANTUMALL,
    EN_SDNCFG_PROC_ADD_QUANTUM,
    EN_SDNCFG_PROC_MODIFY_QUANTUM,
    EN_SDNCFG_PROC_DEL_QUANTUM,
    
    EN_SDNCFG_PROC_ALL,
}EN_NETPLANE_PROC;

const char *s_astrSdnCfgProc[EN_SDNCFG_PROC_ALL] = 
{        
    "pf_net_query_sdn_ctrl_cfg_all",
    "pf_net_add_sdn_ctrl_cfg",    
    "pf_net_modify_sdn_ctrl_cfg",  
    "pf_net_del_sdn_ctrl_cfg",  

    "pf_net_query_quatnum_restfull_cfg_all",
    "pf_net_add_quantum_restfull_cfg",    
    "pf_net_modify_quantum_restfull_cfg",  
    "pf_net_del_quantum_restfull_cfg",  
};

    
CDBOperateSdnCfg::CDBOperateSdnCfg(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateSdnCfg::~CDBOperateSdnCfg()
{
    
}

int32 CDBOperateSdnCfg::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_SDNCFG_PROC_QUERY_SDNCTRLALL:
        {
            return QuerySdnCtrAllCallback(prs,nil);
        }
        break;
        case EN_SDNCFG_PROC_QUERY_QUANTUMALL:
        {
            return QueryQuantumAllCallback(prs,nil);
        }
        break;
        case EN_SDNCFG_PROC_ADD_SDNCTRL:
        case EN_SDNCFG_PROC_MODIFY_SDNCTRL:
        case EN_SDNCFG_PROC_DEL_SDNCTRL:
        case EN_SDNCFG_PROC_ADD_QUANTUM:
        case EN_SDNCFG_PROC_MODIFY_QUANTUM:
        case EN_SDNCFG_PROC_DEL_QUANTUM:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateSdnCfg::QuerySdnCtrlAll(vector<CDbSdnCtrlCfg> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrSdnCfgProc[EN_SDNCFG_PROC_QUERY_SDNCTRLALL],\
        vInParam, \
        this,\
        EN_SDNCFG_PROC_QUERY_SDNCTRLALL,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QueryAll IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateSdnCfg::QuerySdnCtrAllCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QueryAllCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbSdnCtrlCfg> *pvOut;
    pvOut = static_cast<vector<CDbSdnCtrlCfg>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::QueryAllCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbSdnCtrlCfg info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetUuid(prs->GetFieldValue(0));
    info.SetProtocolType(prs->GetFieldValue(1));

    int32 values = 0;
/*    
    if( false == prs->GetFieldValue(1,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QuerySdnCtrAllCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetProtocolType(values);
*/
    if( false == prs->GetFieldValue(2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QuerySdnCtrAllCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPort(values);

    info.SetIP(prs->GetFieldValue(3));

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateSdnCfg::QuerySdnCtrl(CDbSdnCtrlCfg & Info)
{
    vector<CDbSdnCtrlCfg> vCfg; 
    
    int32 nRet = QuerySdnCtrlAll(vCfg);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::Query  QueryAll failed(%d).\n",nRet);
        return nRet;
    }

    if(vCfg.size() == 0 )
    {
        return  VNET_DB_ERROR_ITEM_NO_EXIST;  
    }  

    if(vCfg.size() > 1 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::Query  QueryAll size(%d) > 1.\n",vCfg.size());
        return  VNET_DB_ERROR_PARAM_INVALID;  
    } 

    vector<CDbSdnCtrlCfg>::iterator it = vCfg.begin();
    Info = (*it);
    
    return 0;
}

int32 CDBOperateSdnCfg::AddSdnCtrl(CDbSdnCtrlCfg & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::AddSdnCtrl IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inProtocolType(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetProtocolType().c_str());    
    CADOParameter inPort(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPort());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inProtocolType);
    vInParam.push_back(inPort); 
    vInParam.push_back(inIp);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrSdnCfgProc[EN_SDNCFG_PROC_ADD_SDNCTRL],
        vInParam, 
        this,
        EN_SDNCFG_PROC_ADD_SDNCTRL,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::AddSdnCtrl IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::AddSdnCtrl proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateSdnCfg::ModifySdnCtrl(CDbSdnCtrlCfg & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::ModifySdnCtrl IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inProtocolType(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetProtocolType().c_str());    
    CADOParameter inPort(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPort());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inProtocolType);
    vInParam.push_back(inPort); 
    vInParam.push_back(inIp);   

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrSdnCfgProc[EN_SDNCFG_PROC_MODIFY_SDNCTRL],
        vInParam, 
        this,
        EN_SDNCFG_PROC_MODIFY_SDNCTRL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::ModifySdnCtrl IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::ModifySdnCtrl proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateSdnCfg::DelSdnCtrl(const char* in_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelSdnCtrl IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == in_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelSdnCtrl uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)in_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrSdnCfgProc[EN_SDNCFG_PROC_DEL_SDNCTRL],
        vInParam, 
        this,
        EN_SDNCFG_PROC_DEL_SDNCTRL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelSdnCtrl(%s) IProc->Handle failed. ret : %d.\n",
            in_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelSdnCtrl(%s) proc return failed. ret : %d.\n",
            in_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateSdnCfg::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperateSdnCfg::DbgShow()
{

}

void db_query_sdnctrl()
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper )
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    CDbSdnCtrlCfg cfg;
    int ret = pOper->QuerySdnCtrl(cfg);
    if( ret != 0 )
    {
        cout << "QuerySdnCtrl failed." << ret << endl;
        return;
    }
    cfg.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_sdnctrl);

void db_add_sdnctrl(char* p_type,int32 port, char* sIp)
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper || NULL == sIp || NULL ==p_type)
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    CDbSdnCtrlCfg cfg;
    cfg.SetIP(sIp);
    cfg.SetProtocolType(p_type);
    cfg.SetPort(port);
    
    int ret = pOper->AddSdnCtrl(cfg);
    if( ret != 0 )
    {
        cout << "AddSdnCtrl failed." << ret << endl;
        return;
    }
    cout << "AddSdnCtrl success" << endl;
    cfg.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_add_sdnctrl);

void db_modify_sdnctrl(char* uuid, char* p_type,int32 port, char* sIp)
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper || NULL == sIp || NULL == uuid)
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    CDbSdnCtrlCfg cfg;
    cfg.SetUuid(uuid);
    cfg.SetIP(sIp);
    cfg.SetProtocolType(p_type);
    cfg.SetPort(port);
    
    int ret = pOper->ModifySdnCtrl(cfg);
    if( ret != 0 )
    {
        cout << "ModifySdnCtrl failed." << ret << endl;
        return;
    }
    cout << "ModifySdnCtrl success" << endl;
    cfg.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_modify_sdnctrl);

void db_del_sdnctrl(char* uuid)
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper || NULL == uuid)
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    
    int ret = pOper->DelSdnCtrl(uuid);
    if( ret != 0 )
    {
        cout << "DelSdnCtrl failed." << ret << endl;
        return;
    }
    cout << "DelSdnCtrl success" << endl;
    
}
DEFINE_DEBUG_FUNC(db_del_sdnctrl);




int32 CDBOperateSdnCfg::QueryQuantumRestfullAll(vector<CDbQuantumRestfulCfg> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QueryQuantumRestfullAll IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrSdnCfgProc[EN_SDNCFG_PROC_QUERY_QUANTUMALL],\
        vInParam, \
        this,\
        EN_SDNCFG_PROC_QUERY_QUANTUMALL,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QueryQuantumRestfullAll IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateSdnCfg::QueryQuantumAllCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::QueryQuantumAllCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbQuantumRestfulCfg> *pvOut;
    pvOut = static_cast<vector<CDbQuantumRestfulCfg>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::QueryQuantumAllCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbQuantumRestfulCfg info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);
    info.SetUuid(prs->GetFieldValue(0));
    info.SetIP(prs->GetFieldValue(1));

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateSdnCfg::QueryQuantum(CDbQuantumRestfulCfg & Info)
{
    vector<CDbQuantumRestfulCfg> vCfg; 
    
    int32 nRet = QueryQuantumRestfullAll(vCfg);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::QueryQuantum  QueryAll failed(%d).\n",nRet);
        return nRet;
    }

    if(vCfg.size() == 0 )
    {
        return  VNET_DB_ERROR_ITEM_NO_EXIST;  
    }  

    if(vCfg.size() > 1 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::QueryQuantum  QueryAll size(%d) > 1.\n",vCfg.size());
        return  VNET_DB_ERROR_PARAM_INVALID;  
    } 

    vector<CDbQuantumRestfulCfg>::iterator it = vCfg.begin();
    Info = (*it);
    
    return 0;
}

int32 CDBOperateSdnCfg::AddQuantum(CDbQuantumRestfulCfg & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::AddQuantum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inIp);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrSdnCfgProc[EN_SDNCFG_PROC_ADD_QUANTUM],
        vInParam, 
        this,
        EN_SDNCFG_PROC_ADD_QUANTUM,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::AddQuantum IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::AddQuantum proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateSdnCfg::ModifyQuantum(CDbQuantumRestfulCfg & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::ModifyQuantum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inIp);   

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrSdnCfgProc[EN_SDNCFG_PROC_MODIFY_QUANTUM],
        vInParam, 
        this,
        EN_SDNCFG_PROC_MODIFY_QUANTUM,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::ModifyQuantum IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSdnCfg::ModifyQuantum proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateSdnCfg::DelQuantum(const char* in_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelQuantum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == in_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelQuantum uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)in_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrSdnCfgProc[EN_SDNCFG_PROC_DEL_QUANTUM],
        vInParam, 
        this,
        EN_SDNCFG_PROC_MODIFY_QUANTUM,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelQuantum(%s) IProc->Handle failed. ret : %d.\n",
            in_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSdnCfg::DelQuantum(%s) proc return failed. ret : %d.\n",
            in_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}



void db_query_quantum()
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper )
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    CDbQuantumRestfulCfg cfg;
    int ret = pOper->QueryQuantum(cfg);
    if( ret != 0 )
    {
        cout << "QuerySdnCtrl failed." << ret << endl;
        return;
    }
    cfg.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_quantum);

void db_add_quantum(char* sIp)
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper || NULL == sIp)
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    CDbQuantumRestfulCfg cfg;
    cfg.SetIP(sIp);
    
    int ret = pOper->AddQuantum(cfg);
    if( ret != 0 )
    {
        cout << "AddQuantum failed." << ret << endl;
        return;
    }
    cout << "AddQuantum success" << endl;
    cfg.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_add_quantum);

void db_modify_quantum(char* uuid, char* sIp)
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper || NULL == sIp || NULL == uuid)
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    CDbQuantumRestfulCfg cfg;
    cfg.SetUuid(uuid);
    cfg.SetIP(sIp);
    
    int ret = pOper->ModifyQuantum(cfg);
    if( ret != 0 )
    {
        cout << "ModifyQuantum failed." << ret << endl;
        return;
    }
    cout << "ModifyQuantum success" << endl;
    cfg.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_modify_quantum);

void db_del_quantum(char* uuid)
{    
    CDBOperateSdnCfg * pOper = GetDbISdnCfg();
    if( NULL == pOper || NULL == uuid)
    {
        cout << "GetDbISdnCfg is NULL" << endl;
        return ;
    }

    
    int ret = pOper->DelQuantum(uuid);
    if( ret != 0 )
    {
        cout << "DelQuantum failed." << ret << endl;
        return;
    }
    cout << "DelQuantum success" << endl;
    
}
DEFINE_DEBUG_FUNC(db_del_quantum);




}

