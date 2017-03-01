

#include "vnet_db_vna.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_port_ip.h"

namespace zte_tecs
{
typedef enum tagPortIpProc
{
    EN_PORTIP_PROC_QUERY_BY_VNA = 0,
    EN_PORTIP_PROC_QUERY,     
    EN_PORTIP_PROC_ADD,
    EN_PORTIP_PROC_MOD,
    EN_PORTIP_PROC_DEL,
    EN_PORTIP_PROC_DEL_BY_PORT,
    EN_PORTIP_PROC_ALL,
}EN_PORTIP_PROC;

const char *s_astrPortIpProc[EN_PORTIP_PROC_ALL] = 
{        
    "pf_net_query_portip_by_vna",
    "pf_net_query_portip",
    "pf_net_add_portip",   
    "pf_net_modify_portip",  
    "pf_net_del_portip",  
    "pf_net_del_portip_by_port",
};

    
CDBOperatePortIp::CDBOperatePortIp(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortIp::~CDBOperatePortIp()
{
    
}

int32 CDBOperatePortIp::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORTIP_PROC_QUERY_BY_VNA:
        case EN_PORTIP_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORTIP_PROC_ADD:
        case EN_PORTIP_PROC_MOD:
        case EN_PORTIP_PROC_DEL:
        case EN_PORTIP_PROC_DEL_BY_PORT:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePortIp::QueryPortIp(const char* port_uuid,vector<CDbPortIp> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryPortIp IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::QueryPortIp port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortIpProc[EN_PORTIP_PROC_QUERY],\
        vInParam, \
        this,\
        EN_PORTIP_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryPortIp(%s) IProc->Handle failed(%d) \n",
                port_uuid,ret);
        }
        return ret;
    }
    
    return 0;
}


int32 CDBOperatePortIp::QueryPortIpByVna(const char* uuid,vector<CDbPortIp> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryPortIpByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::QueryPortIpByVna vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortIpProc[EN_PORTIP_PROC_QUERY_BY_VNA],\
        vInParam, \
        this,\
        EN_PORTIP_PROC_QUERY_BY_VNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryPortIpByVna(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortIp::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortIp> *pvOut;
    pvOut = static_cast<vector<CDbPortIp>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::QueryCallback pOutVna is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    CDbPortIp info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 9);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetPortUuid(prs->GetFieldValue(1));
    info.SetPortName(prs->GetFieldValue(2));

    int32 value = 0;
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(value);
    
    info.SetPortIpName(prs->GetFieldValue(4));
    info.SetIp(prs->GetFieldValue(5));
    info.SetMask(prs->GetFieldValue(6));

    if( false == prs->GetFieldValue(7,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsCfg(value);

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsOnline(value);
    
    (*pvOut).push_back(info);

    return 0;
}



int32 CDBOperatePortIp::Add(const char* port_uuid,const char* port_ip_name,const char* ip, const char* mask, int32 iscfg, int32 isOnline)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == port_ip_name || NULL == ip || NULL == mask )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::Add port_uuid OR mac is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    CADOParameter inPortIpName(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_ip_name);    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)ip);    
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)mask);    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, iscfg);        
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, isOnline);       
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inPortIpName);
    vInParam.push_back(inIp);
    vInParam.push_back(inMask);
    vInParam.push_back(inIsCfg);
    vInParam.push_back(inIsOnline);
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortIpProc[EN_PORTIP_PROC_ADD],
        vInParam, 
        this,
        EN_PORTIP_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::Add(%s,%s,%s,%s,%d,%d) IProc->Handle failed(%d).\n", 
            port_uuid,port_ip_name,ip,mask,iscfg,isOnline,ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::Add(%s,%s,%s,%s,%d,%d) proc return failed. ret : %d.\n",
            port_uuid,port_ip_name,ip,mask,iscfg,isOnline,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortIp::Modify(const char* port_uuid,const char* port_ip_name,const char* ip, const char* mask, int32 iscfg, int32 isOnline)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == port_ip_name || NULL == ip || NULL == mask )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::Modify OR mac is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    //info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    CADOParameter inPortIpName(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_ip_name);    
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)ip);    
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)mask);    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, iscfg);        
    CADOParameter inIsOnline(CADOParameter::paramInt, CADOParameter::paramInput, isOnline);       
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inPortIpName);
    vInParam.push_back(inIp);
    vInParam.push_back(inMask);
    vInParam.push_back(inIsCfg);
    vInParam.push_back(inIsOnline);
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortIpProc[EN_PORTIP_PROC_MOD],
        vInParam, 
        this,
        EN_PORTIP_PROC_MOD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::Modify(%s,%s,%s,%s,%d,%d) IProc->Handle failed(%d).\n",
            port_uuid,port_ip_name,ip,mask,iscfg,isOnline,ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortIp::Modify(%s,%s,%s,%s,%d,%d) proc return failed. ret : %d.\n",
            port_uuid,port_ip_name,ip,mask,iscfg,isOnline,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortIp::Del(const char* port_uuid,const char* port_ip_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid || NULL == port_ip_name )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::Del port_uuid OR mac is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_uuid);
    CADOParameter inPortIpName(CADOParameter::paramText, CADOParameter::paramInput, (char *)port_ip_name);    
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inPortIpName);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortIpProc[EN_PORTIP_PROC_DEL],
        vInParam, 
        this,
        EN_PORTIP_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::Del(%s,%s) IProc->Handle failed. ret : %d.\n", 
            port_uuid,port_ip_name,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::Del(%s,%s) proc return failed. ret : %d.\n", 
            port_uuid,port_ip_name,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortIp::DelByPort(const char* port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::DelByPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::DelByPort port_uuid is NULL.\n");
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
        s_astrPortIpProc[EN_PORTIP_PROC_DEL_BY_PORT],
        vInParam, 
        this,
        EN_PORTIP_PROC_DEL_BY_PORT,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::DelByPort(%s) IProc->Handle failed. ret : %d.\n",
            port_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::DelByPort(%s) proc return failed. ret : %d.\n", 
            port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortIp::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortIp::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePortIp::DbgShow()
{
   
}

/* test code ------------------------------------------------------------

*/
void db_query_portip(char* uuid)
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
    CDBOperatePortIp * pOper = pMgr->GetIPortIp();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortIp is NULL" << endl;
        return ;
    }

    vector<CDbPortIp> outVInfo;
    int ret = pOper->QueryPortIp(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryPortIp failed." << ret << endl;
        return;
    }
    cout << "QueryPortIp success." << endl;
    vector<CDbPortIp>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_portip);

void db_query_portip_by_vna(char* uuid)
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
    CDBOperatePortIp * pOper = pMgr->GetIPortIp();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortIp is NULL" << endl;
        return ;
    }

    vector<CDbPortIp> outVInfo;
    int ret = pOper->QueryPortIpByVna(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryPortIpByVna failed." << ret << endl;
        return;
    }
    cout << "QueryPortIpByVna success." << endl;
    vector<CDbPortIp>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_portip_by_vna);


void db_add_portip(char* port_uuid, char* port_ip_name, char* ip, char* mask, int32 iscfg, int32 isonline)
{
    if( NULL == port_uuid || NULL == port_ip_name || NULL == ip || NULL == mask )
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
    CDBOperatePortIp * pOper = pMgr->GetIPortIp();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortIp is NULL" << endl;
        return ;
    }

    int ret = pOper->Add(port_uuid,port_ip_name,ip,mask,iscfg,isonline);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
}
DEFINE_DEBUG_FUNC(db_add_portip);

void db_mod_portip(char* port_uuid, char* port_ip_name, char* ip, char* mask, int32 iscfg, int32 isonline)
{
    if( NULL == port_uuid || NULL == port_ip_name || NULL == ip || NULL == mask )
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
    CDBOperatePortIp * pOper = pMgr->GetIPortIp();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortIp is NULL" << endl;
        return ;
    }

    int ret = pOper->Modify(port_uuid,port_ip_name,ip,mask,iscfg,isonline);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
}
DEFINE_DEBUG_FUNC(db_mod_portip);


void db_del_portip(char* port_uuid, char* port_ip_name)
{
    if( NULL == port_uuid || NULL == port_ip_name  )
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
    CDBOperatePortIp * pOper = pMgr->GetIPortIp();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortIp is NULL" << endl;
        return ;
    }

    int ret = pOper->Del(port_uuid,port_ip_name);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_portip);

void db_del_portip_by_port(char* port_uuid)
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
    CDBOperatePortIp * pOper = pMgr->GetIPortIp();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortIp is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_portip_by_port);

}
 // zte_tecs

