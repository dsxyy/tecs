

#include "vnet_db_vna.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_port_mac.h"

namespace zte_tecs
{
typedef enum tagPortMacProc
{
    EN_PORTMAC_PROC_QUERY_BY_VNA = 0,
    EN_PORTMAC_PROC_QUERY,     
    EN_PORTMAC_PROC_ADD,
    EN_PORTMAC_PROC_DEL,
    EN_PORTMAC_PROC_DEL_BY_PORT,
    EN_PORTMAC_PROC_ALL,
}EN_PORTMAC_PROC;

const char *s_astrPortMacProc[EN_PORTMAC_PROC_ALL] = 
{        
    "pf_net_query_port_mac_by_vna",
    "pf_net_query_port_mac",
    "pf_net_add_port_mac",   
    "pf_net_del_port_mac",  
    "pf_net_del_port_mac_by_port",
};

    
CDBOperatePortMac::CDBOperatePortMac(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortMac::~CDBOperatePortMac()
{
    
}

int32 CDBOperatePortMac::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORTMAC_PROC_QUERY_BY_VNA:
        case EN_PORTMAC_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORTMAC_PROC_ADD:
        case EN_PORTMAC_PROC_DEL:
        case EN_PORTMAC_PROC_DEL_BY_PORT:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePortMac::QueryPortMac(const char* port_uuid,vector<CDbPortMac> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::QueryPortMac IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::QueryPortMac port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortMacProc[EN_PORTMAC_PROC_QUERY],\
        vInParam, \
        this,\
        EN_PORTMAC_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::QueryPortMac(%s) IProc->Handle failed(%d) \n",
                port_uuid, ret);
        }
        return ret;
    }
    
    return 0;
}


int32 CDBOperatePortMac::QueryPortMacByVna(const char* uuid,vector<CDbPortMac> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::QueryPortMacByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::QueryPortMacByVna vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortMacProc[EN_PORTMAC_PROC_QUERY_BY_VNA],\
        vInParam, \
        this,\
        EN_PORTMAC_PROC_QUERY_BY_VNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::QueryPortMacByVna(%s) IProc->Handle failed(%d) \n",
                uuid, ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortMac::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortMac> *pvOut;
    pvOut = static_cast<vector<CDbPortMac>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::QueryCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    CDbPortMac info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 5);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetPortUuid(prs->GetFieldValue(1));
    info.SetPortName(prs->GetFieldValue(2));

    int32 value = 0;
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(value);
    
    info.SetMac(prs->GetFieldValue(4));

    (*pvOut).push_back(info);

    return 0;
}



int32 CDBOperatePortMac::Add(const char* port_uuid,const char* mac)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == mac )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::Add port_uuid OR mac is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    CADOParameter inMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)mac);    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inMac);
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortMacProc[EN_PORTMAC_PROC_ADD],
        vInParam, 
        this,
        EN_PORTMAC_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::Add(%s,%s) IProc->Handle failed(%d).\n",
           port_uuid,mac, ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortMac::Add(%s,%s) proc return failed. ret : %d.\n",
            port_uuid,mac,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortMac::Del(const char* port_uuid,const char* mac)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == mac )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::Del port_uuid OR mac is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    CADOParameter inMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)mac);    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inMac);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortMacProc[EN_PORTMAC_PROC_DEL],
        vInParam, 
        this,
        EN_PORTMAC_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::Del(%s,%s) IProc->Handle failed. ret : %d.\n", 
            port_uuid,mac,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::Del(%s,%s) proc return failed. ret : %d.\n",
            port_uuid,mac,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortMac::DelByPort(const char* port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::DelByPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::DelByPort port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortMacProc[EN_PORTMAC_PROC_DEL_BY_PORT],
        vInParam, 
        this,
        EN_PORTMAC_PROC_DEL_BY_PORT,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::DelByPort(%s) IProc->Handle failed. ret : %d.\n", 
            port_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::DelByPort(%s) proc return failed. ret : %d.\n", 
            port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortMac::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortMac::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePortMac::DbgShow()
{
   
}

/* test code ------------------------------------------------------------

*/
void db_query_portmac(char* uuid)
{
    if(NULL == uuid)
    {
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr )
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperatePortMac * pOper = pMgr->GetIPortMac();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortMac is NULL" << endl;
        return ;
    }

    vector<CDbPortMac> outVInfo;
    int ret = pOper->QueryPortMac(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryPortMac failed." << ret << endl;
        return;
    }
    cout << "QueryPortMac success." << endl;
    vector<CDbPortMac>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_portmac);

void db_query_portmac_by_vna(char* uuid)
{
    if(NULL == uuid)
    {
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr )
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperatePortMac * pOper = pMgr->GetIPortMac();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortMac is NULL" << endl;
        return ;
    }

    vector<CDbPortMac> outVInfo;
    int ret = pOper->QueryPortMacByVna(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryPortMacByVna failed." << ret << endl;
        return;
    }
    cout << "QueryPortMacByVna success." << endl;
    vector<CDbPortMac>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_portmac_by_vna);


void db_add_portmac(char* port_uuid, char* mac)
{
    if( NULL == port_uuid || NULL == mac  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperatePortMac * pOper = pMgr->GetIPortMac();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortMac is NULL" << endl;
        return ;
    }

    int ret = pOper->Add(port_uuid,mac);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
}
DEFINE_DEBUG_FUNC(db_add_portmac);



void db_del_portmac(char* port_uuid, char* mac)
{
    if( NULL == port_uuid || NULL == mac  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperatePortMac * pOper = pMgr->GetIPortMac();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortMac is NULL" << endl;
        return ;
    }

    int ret = pOper->Del(port_uuid,mac);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_portmac);

void db_del_portmac_by_port(char* port_uuid)
{
    if( NULL == port_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if( NULL == pMgr)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return ;
    }
    CDBOperatePortMac * pOper = pMgr->GetIPortMac();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortMac is NULL" << endl;
        return ;
    }

    int ret = pOper->DelByPort(port_uuid);
    if( ret != 0 )
    {
        cout << "DelByPort failed." << ret << endl;
        return;
    }
    cout << "DelByPort success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_portmac_by_port);

}
 // zte_tecs

