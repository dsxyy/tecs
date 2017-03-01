/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_switch.cpp
* 文件标识： 
* 内容摘要：交换管理，操作数据库方法
* 当前版本：1.0
* 作    者：钟春山 
* 完成日期： 
*******************************************************************************/
#include "vnet_error.h"
#include "vnet_db_vswitch.h"
#include "vnet_db_port.h"
#include "vnet_db_mgr.h"

#include "vnm_pub.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnm_switch.h"
#include "vnet_portmgr.h"
#include "vnet_db_schedule.h"
#include "wildcast_loopback.h"

namespace zte_tecs
{

CDBOperateWildcastLoopbackPort * CWildcastLoopback::GetDBLoopback()
{
    CDBOperateWildcastLoopbackPort * pLoop = GetDbIWildcastLoopbackPort();
    if(NULL == pLoop)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::GetDBLoopback: GetDbIWildcastLoopbackPort() is NULL.\n");
        return NULL;
    }
    return pLoop;
}

CDBOperateWildcastTaskLoopback * CWildcastLoopback::GetDBLoopbackTask()
{
    CDBOperateWildcastTaskLoopback * pLoopTask = GetDbIWildcastTaskLoopback();
    if(NULL == pLoopTask )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::GetDBLoopbackTask: GetDbIWildcastTaskLoopback() is NULL.\n");
        return NULL;
    }
    return pLoopTask ;
}

STATUS CWildcastLoopback::ProcMsg(CWildcastLoopbackCfgMsg& cMsg)
{
    int32 nRet = WC_LOOPBACK_OPER_SUCCESS;
        
    switch(cMsg.m_nOper)
    {
        case EN_ADD_WC_LOOPBACK_PORT:
        {
            //入参检查
            if(cMsg.m_strPortName.empty())
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcMsg name is invalid.\n");
                nRet = ERROR_WC_LOOPBACK_NAME_EMPTY;
                return nRet;
            }

            nRet = Add(cMsg);
            break;
        }
        case EN_DEL_WC_LOOPBACK_PORT:
        {
            nRet = Del(cMsg.m_strUuid);
            if(WC_LOOPBACK_OPER_SUCCESS == nRet)
            {
                return nRet;    
            }
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcMsg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);  
            nRet = ERROR_WC_LOOPBACK_OPERCODE_INVALID;
        }    
    }
    
    return nRet;
}

STATUS CWildcastLoopback::Add(CWildcastLoopbackCfgMsg& cMsg)
{        
    CDBOperateWildcastLoopbackPort * pLoop = GetDBLoopback();
    if(NULL == pLoop)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Add: GetDBLoopback() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    } 

    vector<CDbWildcastLoopbackPort> outVInfo;
    int32 nRet = pLoop->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        vector<CDbWildcastLoopbackPort>::iterator iter = outVInfo.begin();
        for( ; iter != outVInfo.end(); ++iter)
        {
            if( 0 == STRCMP(cMsg.m_strPortName.c_str(), iter->GetPortName().c_str()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Add: port(%s) is already exist.\n",
                    cMsg.m_strPortName.c_str());
                return ERROR_WC_LOOPBACK_ALREADY_EXIST;
            }
        }
    }
    
    CDbWildcastLoopbackPort cDbVSInfo;
    cDbVSInfo.SetPortName(cMsg.m_strPortName.c_str());
    cDbVSInfo.SetIsLoop(cMsg.m_nIsLoop);

    nRet = pLoop->Add(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        cDbVSInfo.DbgShow();
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Add: Add to DB failed, ret = %d\n", nRet);
        return ERROR_WC_LOOPBACK_SET_DB_FAIL;
    }
    cMsg.m_strUuid = cDbVSInfo.GetUuid();
    
    return WC_LOOPBACK_OPER_SUCCESS;
}

STATUS CWildcastLoopback::Del(const string &strUuid)
{
    CDBOperateWildcastLoopbackPort * pLoop = GetDBLoopback();
    if(NULL == pLoop)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Del: GetDBLoopback() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    } 

    int32 nRet = pLoop->Del(strUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Del: Del %s failed.\n", strUuid.c_str());
        return ERROR_WC_LOOPBACK_SET_DB_FAIL;
    }
    return WC_LOOPBACK_OPER_SUCCESS;
}
#if 0
STATUS CWildcastLoopback::Modify(const CWildcastLoopbackCfgMsg& cMsg)
{
    CDBOperateWildcastLoopbackPort * pCreateVPort = GetDBLoopback();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Modify: GetDBLoopback() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    } 

    CDbWildcastLoopbackPort cDbVSInfo;    
    cDbVSInfo.SetUuid(cMsg.m_strUuid.c_str());

    // 首先获取该switch信息
    int32 nRet = pCreateVPort->Query(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Modify: query switch %s failed, ret = %d\n",cMsg.m_strUuid.c_str(), nRet);
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }

    cDbVSInfo.SetUuid(cMsg.m_strUuid.c_str());
    cDbVSInfo.SetPortName(cMsg.m_strPortName.c_str());
    cDbVSInfo.SetPortType(cMsg.m_nPortType);
    cDbVSInfo.SetIsHasKernelType(cMsg.m_nIsHasKenelType);
    cDbVSInfo.SetKernelType(cMsg.m_nKernelType);
    
    nRet = pCreateVPort->Modify(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        cDbVSInfo.DbgShow();
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::Modify: pCreateVPort->Modify DB failed, Ret = %d\n", nRet);
        return ERROR_WC_LOOPBACK_SET_DB_FAIL;
    }    

    return WC_LOOPBACK_OPER_SUCCESS;
}
#endif 

// task 
STATUS CWildcastLoopback::ProcTaskMsg(const string& strTaskUuid)
{
    CDBOperateWildcastTaskLoopback * pLoopTask =  GetDBLoopbackTask();
    if(NULL == pLoopTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcTaskMsg: GetDBLoopbackTask() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    } 

    int32 nRet = pLoopTask->Del(strTaskUuid.c_str());
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcTaskMsg: pCreateVPortTask->Del(%s) failed. %d\n",
            strTaskUuid.c_str(),nRet);
        return ERROR_WC_LOOPBACK_SET_DB_FAIL;
    } 
    
    return WC_LOOPBACK_OPER_SUCCESS;
}
int32 CWildcastLoopback::NewVnaRegist(const string &strVnaUuid)
{
    CDBOperateWildcastLoopbackPort * pLoop =  GetDBLoopback();
    if(NULL == pLoop)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::NewVanRegist: GetDBLoopback() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }   

    CDBOperateWildcastTaskLoopback * pLoopTask =  GetDBLoopbackTask();
    if(NULL == pLoopTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::NewVnaRegist: GetDBLoopbackTask() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }   

    
    CPortMgr * pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::NewVnaRegist: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }  

    vector<CDbWildcastLoopbackPort> vWcCreateVPort;
    int32 nRet = pLoop->QuerySummary(vWcCreateVPort);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::NewVanRegist: pCreateVPort->QuerySummary() failed. %d\n",nRet);
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }
    
    int32 isExistTask = false;
    int32 isConflict = false;
    vector<CDbWildcastLoopbackPort>::iterator it = vWcCreateVPort.begin();
    for(; it != vWcCreateVPort.end(); ++it)
    {
        isExistTask = false;
        isConflict = false;
        
        //是否已经存在此port uuid 
        if( WC_LOOPBACK_OPER_SUCCESS != IsExistTask(strVnaUuid.c_str(),(*it).GetUuid().c_str(),isExistTask) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::NewVanRegist: IsExistTask(%s,%s) failed.\n",
                strVnaUuid.c_str(),(*it).GetUuid().c_str());
            continue;
        }

        if( isExistTask )
        {
            continue;
        }

        // 是否冲突 
        nRet = LoopbackIsConflict(strVnaUuid.c_str(), (*it).GetPortName().c_str(), (*it).GetIsLoop(), isConflict);
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_INFO, "CWildcastLoopback::NewVanRegist: IsConflict(%s,%s,%d) failed.%d\n",
                strVnaUuid.c_str(),(*it).GetPortName().c_str(),(*it).GetIsLoop(),nRet);
            continue;
        }

        // 更新task 表 
        CDbWildcastTaskLoopback wcTask;
        wcTask.SetVnaUuid(strVnaUuid.c_str());
        wcTask.SetLoopbackUuid((*it).GetUuid().c_str());
        wcTask.SetState(WILDCAST_TASK_STATE_INIT);
        wcTask.SetSendNum(0);
        nRet = pLoopTask->Add(wcTask);
        if( VNET_DB_SUCCESS != nRet )
        {
            wcTask.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::NewVanRegist: pCreateVPortTask->Add(%s,%s) failed. %d\n",
                strVnaUuid.c_str(),(*it).GetUuid().c_str(),nRet);
            continue;
        } 
    }
    return WC_LOOPBACK_OPER_SUCCESS;
}

// 内部封装port loop 冲突检查函数 
int32 CWildcastLoopback::LoopbackIsConflict(const char* vnaUuid, const char* strPortName, int32 isLoop, int32 & nResult)
{
    // 内部调用 port loop (sriov)
    // 直接调用CDBOperatePortPhysical CheckSetSriovLoop 
#if 1 
    string strPortUuid = "";
    CDBOperatePortPhysical* pDbPhyPort = GetDbIPortPhy();
    if( NULL == pDbPhyPort )
    {
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }

    int32 ret = pDbPhyPort->QueryUuid(vnaUuid,strPortName,PORT_TYPE_PHYSICAL,strPortUuid);
    if( ret != VNET_DB_SUCCESS)
    {
        if(VNET_DB_IS_ITEM_NO_EXIST(ret))
        {
            VNET_LOG(VNET_LOG_INFO, "CWildcastLoopback::LoopbackIsConflict: vna(%s) port(%s) not exist.\n",
             vnaUuid,strPortName);
            return ret;
        }
        VNET_LOG(VNET_LOG_INFO, "CWildcastLoopback::LoopbackIsConflict: vna(%s) query port(%s) failed(%d).\n",
             vnaUuid,strPortName,ret);
        return ret;             
    }
    
    ret= pDbPhyPort->CheckSetSriovLoop(strPortUuid.c_str());
    if ( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastLoopback::LoopbackIsConflict: CheckSetSriovLoop(%s,%s,%s) failed(%d).\n",
             vnaUuid,strPortName,strPortUuid.c_str(),ret);
        return ret;
    }
#endif     
    return 0;
}

int32 CWildcastLoopback::IsExistTask(const char* vnaUuid, const char* loopUuid, int32 &isExist)
{
   
    CDBOperateWildcastTaskLoopback * pLoopTask =  GetDBLoopbackTask();
    if(NULL == pLoopTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::IsExistTask: GetDBLoopbackTask() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }   

    if(NULL == loopUuid || NULL == vnaUuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::IsExistTask: loopUuid or vnaUUid is NULL.\n");
        return ERROR_WC_SWITCH_PARAM_INVALID;
    }

    vector<CDbWildcastTaskLoopback>  outVInfo;
    int32 nRet = pLoopTask->QuerySummary(outVInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::IsExistTask: QuerySummary() failed.%d\n",nRet);
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskLoopback>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if( (0 == STRCMP((*it).GetLoopbackUuid().c_str(), loopUuid)) && \
            (0 == STRCMP((*it).GetVnaUuid().c_str(), vnaUuid)))
        {
            isExist = true;
            break;
        }
    }

    return WC_LOOPBACK_OPER_SUCCESS;
}

// 定时处理task任务 
int32 CWildcastLoopback::ProcTimeout()
{   
    CDBOperateWildcastTaskLoopback * pLoopTask =  GetDBLoopbackTask();
    if(NULL == pLoopTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcTimeout: GetDBLoopbackTask() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }   

    vector<CDbWildcastTaskLoopback>  outVInfo;
    int32 nRet = pLoopTask->QuerySummary(outVInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcTimeout: QuerySummary() failed.%d\n",nRet);
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskLoopback>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if( (*it).GetSendNum() <= WILDCAST_TASK_SEND_NUM_MAX )
        {           
            //调用Port模块接口            
            nRet = SendMsgToPort((*it) );
            if(WC_LOOPBACK_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_INFO, "CWildcastLoopback::ProcTimeout: SendMsgToPort(%s) failed.\n",
                 (*it).GetLoopbackUuid().c_str());
            }

            // 是否更新DB
            CDbWildcastTaskLoopback wctaskInfo;
            wctaskInfo.SetSendNum((*it).GetSendNum() + 1);
            if(nRet == WC_LOOPBACK_OPER_SUCCESS )
            {
                wctaskInfo.SetState(WILDCAST_TASK_STATE_WAIT_ACK);
            }
            else
            {
                wctaskInfo.SetState(WILDCAST_TASK_STATE_SEND_MSG_FAILED);
            }

            wctaskInfo.SetLoopbackUuid((*it).GetLoopbackUuid().c_str());
            wctaskInfo.SetUuid((*it).GetUuid().c_str());
            wctaskInfo.SetVnaUuid((*it).GetVnaUuid().c_str());
            
            nRet = pLoopTask->Modify(wctaskInfo);
            if( VNET_DB_SUCCESS != nRet )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::ProcTimeout: pCreateVPortTask->Modify(%s) failed.%d\n",
                    (*it).GetUuid().c_str(),nRet);
                continue;
            }
        }

    }
    return WC_LOOPBACK_OPER_SUCCESS;
}

int32 CWildcastLoopback::SendMsgToPort(CDbWildcastTaskLoopback & info)
{
    CPortCfgMsgToVNA msg;
    
    msg.m_strWCTaskUuid = info.GetUuid();
    msg.m_strName = info.GetPortName();
    msg.m_strVnaUuid = info.GetVnaUuid();

    if(info.GetIsLoop() )
    {
        msg.m_nOper = EN_SET_LOOPPORT;
    }
    else
    {
        msg.m_nOper = EN_UNSET_LOOPPORT;
    }
    
    // 调用port 接口 
    CPortMgr *pPMgr = CPortMgr::GetInstance();
    if(NULL == pPMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::SendMsgToPort: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_WC_GET_PORT_INSTANCE_NULL;
    }
    
    VNET_LOG(VNET_LOG_INFO,"VNet wildcastloopback module set sriov port loopback send msg(%s,%s,%s,%d) to PortMgr.\n",\
       msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper );
    
    int32 nRet = pPMgr->ProcLoopBackPortWildcast(msg);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::SendMsgToPort: pPMgr->ProcLoopBackPortWildcast(%s,%s,%s,%d) failed. ret=%d.\n",
            msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper, nRet);
        return nRet;
    }

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastloopback module set sriov port loopback send msg(%s,%s,%s,%d) success.\n",\
       msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper );

    return WC_LOOPBACK_OPER_SUCCESS;
}

int32 CWildcastLoopback::WildcastTaskMsgAck(const string & strTaskUuid, int32 nResult)
{
    CDBOperateWildcastTaskLoopback * pLoop =  GetDBLoopbackTask();
    if(NULL == pLoop)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::WildcastTaskMsgAck: GetDBLoopbackTask() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    } 

    CDbWildcastTaskLoopback info;
    info.SetUuid(strTaskUuid.c_str());
    int32 nRet = pLoop->Query(info);
    
    // 没有找到，说明没有task，可能前面已经处理成功了。
    if( VNET_DB_IS_ITEM_NO_EXIST(nRet))
    {
        if(DbgGetDBPrint())
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::WildcastTaskMsgAck: pLoop->Query(%s) failed.%d\n",
              info.GetUuid().c_str(), nRet);
        }
        return WC_LOOPBACK_OPER_SUCCESS;
    }
    
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::WildcastTaskMsgAck: pCreateVPort->Query(%s) failed.%d\n",
          info.GetUuid().c_str(), nRet);
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }

    if( 0 == nResult )
    {
        // 成功后，可以将此记录删除
        nRet = pLoop->Del(strTaskUuid.c_str());
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::WildcastTaskMsgAck: pCreateVPort->Del(%s) failed.%d\n",
              strTaskUuid.c_str(), nRet);
            return ERROR_WC_LOOPBACK_SET_DB_FAIL;
        }
    }
    else
    {
        info.SetState(WILDCAST_TASK_STATE_ACK_FAILED);
        nRet = pLoop->Modify(info);
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::SwitchMsgAck: pCreateVPort->Modify(%s) failed.%d\n",
              strTaskUuid.c_str(), nRet);
            return ERROR_WC_LOOPBACK_SET_DB_FAIL;
        }
    }
    return WC_LOOPBACK_OPER_SUCCESS;
}

// 供port module 调用 
int32 CWildcastLoopback::IsConflict(const string & vnaUuid, const string & portName, int32 & outResult)
{
    outResult = false;
    CDBOperateWildcastTaskLoopback * pLoopTask =  GetDBLoopbackTask();
    if(NULL == pLoopTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::isConflict: GetDBLoopbackTask() is NULL.\n");
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }   
    
    vector<CDbWildcastTaskLoopback>  outVInfo;
    int32 nRet = pLoopTask->QuerySummaryByVna(vnaUuid.c_str(),outVInfo );
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastLoopback::isConflict: pCreateVPort->QuerySummaryByVna(%s) failed.%d\n",
          vnaUuid.c_str(), nRet);
        return ERROR_WC_LOOPBACK_GET_DB_FAIL;
    }

    outResult = false;
    vector<CDbWildcastTaskLoopback>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if(0 == STRCMP((*it).GetPortName().c_str(), portName.c_str()))
        {
            outResult = true;
            return WC_LOOPBACK_OPER_SUCCESS;
        }
    }    
    
    return WC_LOOPBACK_OPER_SUCCESS;
}

void CWildcastLoopback::DbgShow()
{
    
}


    
// test code 
void wc_loopback(char * uuid, char * PortName, int32 IsLoop, int32 oper)
{
    CWildcastLoopbackCfgMsg cMsg;
    CWildcastLoopback loopback;

    if(NULL == uuid || NULL == PortName )
    {
        cout << "param invalid." << endl;
        return;
    }

    cMsg.m_strUuid = uuid;
    cMsg.m_strPortName = PortName;
    cMsg.m_nIsLoop= IsLoop;
    cMsg.m_nOper = oper;
        
    int32 nRet = WC_LOOPBACK_OPER_SUCCESS;

    cMsg.Print();
    if( EN_ADD_WC_PORT == oper)
    {
        nRet = loopback.Add(cMsg) ;
        if( WC_LOOPBACK_OPER_SUCCESS == nRet)
        {    
            cout << "Add success" << endl;
        }
        else
        {
            cout << "Add failed."<< nRet << endl;            
            cMsg.Print();
        }
    }
   
    if( EN_DEL_WC_PORT == oper)
    {
        nRet = loopback.Del(cMsg.m_strUuid) ;
        if( WC_LOOPBACK_OPER_SUCCESS == nRet)
        {    
            cout << "Del success" << endl;
            cMsg.Print();
        }
        else
        {
            cout << "Del failed."<< nRet << endl;
        }
    }   
}
DEFINE_DEBUG_FUNC(wc_loopback);

void wc_loopback_newvna(char * uuid)
{
    CWildcastLoopback loopback;
    string vnauuid(uuid);
    int32 nRet = loopback.NewVnaRegist(vnauuid);
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_loopback_newvna failed. " << nRet << endl;        
    }
    else
    {
        cout << "wc_loopback_newvna success" << endl;
    }
}DEFINE_DEBUG_FUNC(wc_loopback_newvna);

void wc_loopback_timeout()
{
    CWildcastLoopback loopback;
    int32 nRet = loopback.ProcTimeout();
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_loopback_timeout failed. " << nRet << endl;
    }
    else
    {
        cout << "wc_loopback_timeout success" << endl;
    }
     
}DEFINE_DEBUG_FUNC(wc_loopback_timeout);

void wc_loopback_isconflict(char* vna_uuid, char* portname)
{
    CWildcastLoopback loopback;
    string strVnauuid(vna_uuid);
    string strPortname(portname);
    int32 nResult = 0;
    
    int32 nRet = loopback.IsConflict(strVnauuid,strPortname,nResult);
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_loopback_isconflict failed. " << nRet << endl;
    }
    else
    {
        cout << "wc_loopback_isconflict success " << nResult << endl;
    }
     
}DEFINE_DEBUG_FUNC(wc_loopback_isconflict);

} // namespace 

