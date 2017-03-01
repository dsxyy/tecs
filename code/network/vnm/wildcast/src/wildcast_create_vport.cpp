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
#include "wildcast_create_vport.h"

namespace zte_tecs
{

CDBOperateWildcastCreateVPort * CWildcastCreateVPort::GetDBCreateVPort()
{
    CDBOperateWildcastCreateVPort * pCreateVPort = GetDbIWildcastCreateVPort();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::GetDBCreateVPort: GetDbIWildcastCreateVPort() is NULL.\n");
        return NULL;
    }
    return pCreateVPort;
}

CDBOperateWildcastTaskCreateVPort * CWildcastCreateVPort::GetDBCreateVPortTask()
{
    CDBOperateWildcastTaskCreateVPort * pCreateVPortTask = GetDbIWildcastTaskCreateVPort();
    if(NULL == pCreateVPortTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::GetDBCreateVPortTask: GetDbIWildcastTaskCreateVPort() is NULL.\n");
        return NULL;
    }
    return pCreateVPortTask;
}

STATUS CWildcastCreateVPort::ProcMsg(CWildcastCreateVPortCfgMsg& cMsg)
{
    int32 nRet = WC_CREATE_VPORT_OPER_SUCCESS;
    

    switch(cMsg.m_nOper)
    {
        case EN_ADD_WC_PORT:
        {            
            //入参检查
            if(cMsg.m_strPortName.empty())
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcMsg name is invalid.\n");
                nRet = ERROR_WC_CREATE_VPORT_NAME_EMPTY;
                return nRet;
            }
            nRet = Add(cMsg);
            break;
        }
        case EN_DEL_WC_PORT:
        {
            nRet = Del(cMsg.m_strUuid);
            if(WC_CREATE_VPORT_OPER_SUCCESS == nRet)
            {
                return nRet;    
            }
            break;
        }
#if 0        
        case EN_MOD_WC_PORT:
        
        {
            nRet = Modify(cMsg);
            if(WC_CREATE_VPORT_OPER_SUCCESS == nRet)
            {
                return nRet;    
            }
            break;
        }
#endif         
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcMsg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);  
            nRet = ERROR_WC_CREATE_VPORT_OPERCODE_INVALID;
        }    
    }
    
    return nRet;
}

STATUS CWildcastCreateVPort::CheckUplinkPgKernelPgMtu(string & switchUuid, string &vmpgUuid)
{
    // uplink pg mtu 不能必须大于等于 kernel pg mtu
    CPortGroupMgr * pgMgr = CPortGroupMgr::GetInstance();
    if( pgMgr == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: get port_group instance is NULL.\n");
        return ERROR_WC_CREATE_VPORT_OPERCODE_INVALID;
    }

    // 通过switch 获取upling pg info
    string uppgUuid("");
    CPortGroup upInfo;
    
    CSwitchMgr * pswitchMgr = CSwitchMgr::GetInstance();
    if( pswitchMgr == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: get switch instance is NULL.\n");
        return ERROR_WC_CREATE_VPORT_OPERCODE_INVALID;
    }

    string  switchName("");
    int32 nRet = pswitchMgr->GetSwitchName(switchUuid,switchName);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: get GetSwitchName(%s) failed. ret:%d\n",
            switchUuid.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }
    
    nRet = pswitchMgr->GetSwitchPGInfo(switchName, upInfo);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: get GetSwitchPGInfo(%s) failed. ret:%d\n",
            switchName.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    // vm pg 信息 
    CPortGroup vmInfo;
    vmInfo.SetUuid(vmpgUuid);
        
    nRet = pgMgr->GetPGDetail(vmInfo);
    if( nRet != 0)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: get GetPGDetail(%s) failed. ret:%d\n",
            vmpgUuid.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    if( upInfo.GetMtu() < vmInfo.GetMtu())
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: uplink pg mtu less kernel pg mtu(%s,%d,%s,%d)\n",
            uppgUuid.c_str(),upInfo.GetMtu(),vmpgUuid.c_str(), vmInfo.GetMtu());
        return ERROR_WC_UPLINKPG_MTU_LESS_KNPG_MTU;
    }
    
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

STATUS CWildcastCreateVPort::Add(CWildcastCreateVPortCfgMsg& cMsg)
{        
    CDBOperateWildcastCreateVPort * pCreateVPort = GetDBCreateVPort();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Add: GetDBCreateVPort() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    } 

    vector<CDbWildcastCreateVPort> outVInfo;
    int32 nRet = pCreateVPort->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        vector<CDbWildcastCreateVPort>::iterator iter = outVInfo.begin();
        for( ; iter != outVInfo.end(); ++iter)
        {
            if( 0 == STRCMP(cMsg.m_strPortName.c_str(), iter->GetPortName().c_str()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Add: port(%s) is already exist.\n",
                    cMsg.m_strPortName.c_str());
                return ERROR_WC_CREATE_VPORT_ALREADY_EXIST;
            }
        }
    }
    
    // uplink pg mtu 不能必须大于等于 kernel pg mtu
    if( cMsg.m_nPortType ==  PORT_TYPE_KERNEL)
    {
        if( 0 != CheckUplinkPgKernelPgMtu(cMsg.m_strSwitchUuid,cMsg.m_strKernelPgUuid) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckUplinkPgKernelPgMtu: switch uplink pg mtu less kernel pg mtu\n");
            return ERROR_WC_UPLINKPG_MTU_LESS_KNPG_MTU;        
        }
    }
        
    CDbWildcastCreateVPort cDbVSInfo;
    cDbVSInfo.SetPortName(cMsg.m_strPortName.c_str());
    cDbVSInfo.SetPortType(cMsg.m_nPortType);
    cDbVSInfo.SetIsHasKernelType(cMsg.m_nIsHasKenelType);
    cDbVSInfo.SetKernelType(cMsg.m_nKernelType);
    cDbVSInfo.SetKernelPgUuid(cMsg.m_strKernelPgUuid.c_str());
    cDbVSInfo.SetSwitchUuid(cMsg.m_strSwitchUuid.c_str());

    nRet = pCreateVPort->Add(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        cDbVSInfo.DbgShow();
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::AddSwitch: Add switch to DB failed, ret = %d\n", nRet);
        return ERROR_WC_CREATE_VPORT_SET_DB_FAIL;
    }
    cMsg.m_strUuid = cDbVSInfo.GetUuid();
    
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

STATUS CWildcastCreateVPort::Del(const string &strUuid)
{
    CDBOperateWildcastCreateVPort * pCreateVPort = GetDBCreateVPort();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Del: GetDBCreateVPort() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    } 

    int32 nRet = pCreateVPort->Del(strUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Del: Del %s failed.\n", strUuid.c_str());
        return ERROR_WC_CREATE_VPORT_SET_DB_FAIL;
    }
    return WC_CREATE_VPORT_OPER_SUCCESS;
}
#if 0
STATUS CWildcastCreateVPort::Modify(const CWildcastCreateVPortCfgMsg& cMsg)
{
    CDBOperateWildcastCreateVPort * pCreateVPort = GetDBCreateVPort();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Modify: GetDBCreateVPort() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    } 

    CDbWildcastCreateVPort cDbVSInfo;    
    cDbVSInfo.SetUuid(cMsg.m_strUuid.c_str());

    // 首先获取该switch信息
    int32 nRet = pCreateVPort->Query(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Modify: query switch %s failed, ret = %d\n",cMsg.m_strUuid.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
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
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::Modify: pCreateVPort->Modify DB failed, Ret = %d\n", nRet);
        return ERROR_WC_CREATE_VPORT_SET_DB_FAIL;
    }    

    return WC_CREATE_VPORT_OPER_SUCCESS;
}
#endif 
// task 

STATUS CWildcastCreateVPort::ProcTaskMsg(const string& strTaskUuid)
{
    CDBOperateWildcastTaskCreateVPort * pCreateVPortTask =  GetDBCreateVPortTask();
    if(NULL == pCreateVPortTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcTaskMsg: GetDBCreateVPortTask() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }  
    int32 nRet = pCreateVPortTask->Del(strTaskUuid.c_str());
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcTaskMsg: pCreateVPortTask->Del(%s) failed. %d\n",
            strTaskUuid.c_str(),nRet);
        return ERROR_WC_CREATE_VPORT_SET_DB_FAIL;
    } 

    return WC_CREATE_VPORT_OPER_SUCCESS;
}

int32 CWildcastCreateVPort::GetSwitchUplinkPortName(const string & vnaUuid,
    const string & switchUuid, string & sSwitchName, string & swUplinkPortName)
{
    // port 需要switch name 需转下 switch uuid ---> switch name
    CSwitchMgr * pSwitch = CSwitchMgr::GetInstance();
    if( pSwitch == NULL )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortKernel: CSwitchMgr::GetInstance() is NULL.\n");
        return ERROR_WC_GET_SWTICH_INSTANCE_NULL;
    }

    int32 nRet = pSwitch->GetSwitchName(switchUuid,sSwitchName );
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::GetSwitchUplinkPortName: pSwitch->GetSwitchName(%s) failed. ret:%d.\n",
            switchUuid.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    //需获取pg uplink 口 
    //根据交换名称获取上行口  
    nRet = pSwitch->GetSwitchPort(vnaUuid, sSwitchName, swUplinkPortName);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::GetSwitchUplinkPortName: pSwitch->GetSwitchPort(%s,%s) failed. ret:%d.\n",
            vnaUuid.c_str(), sSwitchName.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }
    
    return WC_CREATE_VPORT_OPER_SUCCESS;    
}

int32 CWildcastCreateVPort::NewVnaRegist(const string &strVnaUuid)
{
    CDBOperateWildcastCreateVPort * pCreateVPort =  GetDBCreateVPort();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVanRegist: GetDBCreateVPort() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }   

    CDBOperateWildcastTaskCreateVPort * pCreateVPortTask =  GetDBCreateVPortTask();
    if(NULL == pCreateVPortTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVnaRegist: GetDBCreateVPortTask() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }   

    
    CPortMgr * pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVnaRegist: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }  

    vector<CDbWildcastCreateVPort> vWcCreateVPort;
    int32 nRet = pCreateVPort->QuerySummary(vWcCreateVPort);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVanRegist: pCreateVPort->QuerySummary() failed. %d\n",nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }
    
    int32 isExistTask = false;
    int32 isConflict = false;
    string PortUuid("");
    vector<CDbWildcastCreateVPort>::iterator it = vWcCreateVPort.begin();
    for(; it != vWcCreateVPort.end(); ++it)
    {
        isExistTask = false;
        isConflict = false;
        
        //是否已经存在此port uuid 
        if( WC_CREATE_VPORT_OPER_SUCCESS != IsExistTask(strVnaUuid.c_str(), (*it).GetUuid().c_str(),isExistTask) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVanRegist: IsExistTask(%s,%s) failed.\n",
                strVnaUuid.c_str(), (*it).GetUuid().c_str());
            continue;
        }

        if( isExistTask )
        {
            continue;
        }

        // 是否冲突 
        isConflict = 0;
        nRet = PortNameIsConflict(strVnaUuid.c_str(), (*it).GetPortName().c_str(),isConflict);
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_INFO, "CWildcastCreateVPort::NewVanRegist: pPortMgr->PortNameIsConflict(%s,%s) failed.%d\n",
                strVnaUuid.c_str(),(*it).GetPortName().c_str(),nRet);
            continue;
        }        

        if( isConflict )
        {
            VNET_LOG(VNET_LOG_INFO, "CWildcastCreateVPort::NewVanRegist: port(%s,%s) has conflict\n",
                strVnaUuid.c_str(),(*it).GetPortName().c_str());
            continue;
        }

        //kernel 所在的switch uplink port 不能为uplink loop , sriov loop
        if( (*it).GetPortType() == PORT_TYPE_KERNEL )
        {
            int32 isLoopPort = FALSE;
            string vnaUuid = strVnaUuid;
            string portName = (*it).GetPortName();

            string swUpPortName = "";
            string sSwitchName = "";
            nRet = GetSwitchUplinkPortName(vnaUuid, (*it).GetSwitchUuid(),sSwitchName, swUpPortName);
            if( nRet != WC_CREATE_VPORT_OPER_SUCCESS )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVnaRegist: GetSwitchUplinkPortName(%s,%s) failed. ret:%d.\n",
                    vnaUuid.c_str(),(*it).GetSwitchUuid().c_str(), nRet);
                continue;
            } 
            
            nRet = CheckSwitchUplinkPortIsLoop(vnaUuid, swUpPortName,isLoopPort);
            if( nRet != WC_CREATE_VPORT_OPER_SUCCESS )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVnaRegist: CheckSwitchUplinkPortIsLoop(%s,%s) failed. ret:%d.\n",
                    strVnaUuid.c_str(),portName.c_str(), nRet);
                continue;
            }
        
            if( isLoopPort == TRUE)
            {
                continue;
            }
        }

        // 更新task 表 
        CDbWildcastTaskCreateVPort wcTask;
        wcTask.SetVnaUuid(strVnaUuid.c_str());
        wcTask.SetCreateVPortUuid((*it).GetUuid().c_str());
        wcTask.SetState(WILDCAST_TASK_STATE_INIT);
        wcTask.SetSendNum(0);
        nRet = pCreateVPortTask->Add(wcTask);
        if( VNET_DB_SUCCESS != nRet )
        {
            wcTask.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::NewVanRegist: pCreateVPortTask->Add failed. %d\n",nRet);
            continue;
        } 
    }
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

// 内部封装port 冲突检查函数 
int32 CWildcastCreateVPort::PortNameIsConflict(const char* vnaUuid, const char* strPortName, int32 & nResult)
{
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }
    
    int ret= pOper->CheckPortNameIsConflict(vnaUuid,strPortName,nResult);

    if ( ret != VNET_DB_SUCCESS )
    {
        return ret;
    }
    
    return 0;
}

int32 CWildcastCreateVPort::IsExistTask(const char* vnaUuid, const char* cvPortUuid, int32 &isExist)
{   
    CDBOperateWildcastTaskCreateVPort * pCreateVPortTask =  GetDBCreateVPortTask();
    if(NULL == pCreateVPortTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::IsExistTask: GetDBCreateVPortTask() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }   

    if(NULL == cvPortUuid || NULL == vnaUuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::IsExistTask: cvPortUuid or vnaUuid is NULL.\n");
        return ERROR_WC_SWITCH_PARAM_INVALID;
    }

    vector<CDbWildcastTaskCreateVPort>  outVInfo;
    int32 nRet = pCreateVPortTask->QuerySummary(outVInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::IsExistTask: QuerySummary() failed.%d\n",nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskCreateVPort>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if((0 == STRCMP((*it).GetCreateVPortUuid().c_str(), cvPortUuid)) && \
           (0 == STRCMP((*it).GetVnaUuid().c_str(), vnaUuid)))
        {
            isExist = true;
            break;
        }
    }

    return WC_CREATE_VPORT_OPER_SUCCESS;
}

// 定时处理task任务 
int32 CWildcastCreateVPort::ProcTimeout()
{   
    CDBOperateWildcastTaskCreateVPort * pCreateVPortTask =  GetDBCreateVPortTask();
    if(NULL == pCreateVPortTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcTimeout: GetDBCreateVPortTask() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }   

    vector<CDbWildcastTaskCreateVPort>  outVInfo;
    int32 nRet = pCreateVPortTask->QuerySummary(outVInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcTimeout: QuerySummary() failed.%d\n",nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskCreateVPort>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if( (*it).GetSendNum() <= WILDCAST_TASK_SEND_NUM_MAX )
        {           
            //调用Port模块接口            
            nRet = SendMsgToPort((*it));
            if(WC_CREATE_VPORT_OPER_SUCCESS != nRet )
            {
                VNET_LOG(VNET_LOG_INFO, "CWildcastCreateVPort::ProcTimeout: SendMsgToPort(%s) failed.\n",
                 (*it).GetCreateVPortUuid().c_str());
            }

            // 是否更新DB
            CDbWildcastTaskCreateVPort wctaskInfo;
            wctaskInfo.SetSendNum((*it).GetSendNum() + 1);
            if( nRet == WC_CREATE_VPORT_OPER_SUCCESS )
            {
                wctaskInfo.SetState(WILDCAST_TASK_STATE_WAIT_ACK);
            }
            else
            {
                wctaskInfo.SetState(WILDCAST_TASK_STATE_SEND_MSG_FAILED);
            }
            wctaskInfo.SetCreateVPortUuid((*it).GetCreateVPortUuid().c_str());
            wctaskInfo.SetUuid((*it).GetUuid().c_str());
            wctaskInfo.SetVnaUuid((*it).GetVnaUuid().c_str());
            
            nRet = pCreateVPortTask->Modify(wctaskInfo);
            if( VNET_DB_SUCCESS != nRet )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::ProcTimeout: pCreateVPortTask->Modify(%s) failed.%d\n",
                    (*it).GetUuid().c_str(),nRet);
                continue;
            }
        }

    }
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

int32 CWildcastCreateVPort::SendMsgToPortUplinkLoop(CDbWildcastTaskCreateVPort & info)
{
    CPortCfgMsgToVNA msg;

    msg.m_strWCTaskUuid = info.GetUuid();
    msg.m_strName = info.GetPortName();
    msg.m_strVnaUuid = info.GetVnaUuid();

    msg.m_nOper = EN_ADD_LOOPPORT;
       
    // 调用端口模块接口
    CPortMgr *pPMgr = CPortMgr::GetInstance();
    if(NULL == pPMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortUplinkLoop: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_WC_GET_PORT_INSTANCE_NULL;
    } 

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastCreateVPort module create uplinkloop port send msg(%s,%s,%s,%d) to PortMgr.\n",\
       msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper );
    

    int32 nRet = pPMgr->ProcLoopBackPortWildcast(msg);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortUplinkLoop: pPMgr->ProcLoopBackPortWildcast(%s,%s,%s) failed. ret=%d.\n",
            msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(), nRet);
        return nRet;
    }

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastCreateVPort create uplinkloop port(%s,%s,%s,%d) success.\n",\
       msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper );
            
    return WC_CREATE_VPORT_OPER_SUCCESS; 
}

int32 CWildcastCreateVPort::CheckSwitchUplinkPortIsLoop(string & vnaUuid, string & portName, int32 &outResult)
{
    // 判断一个端口 是否是LOOP, 
    CDBOperatePortPhysical* pPhyPort  = GetDbIPortPhy();
    if( NULL == pPhyPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckSwitchUplinkPortIsLoop: GetDbIPortPhy() is NULL.\n");
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }

    string portUuid("");
    outResult = FALSE;
    
    // sriov physical loop 
    int32 nRet = pPhyPort->QueryUuid(vnaUuid.c_str(), portName.c_str(),PORT_TYPE_PHYSICAL, portUuid);
    if( nRet == VNET_DB_SUCCESS)
    {
        CDbPortPhysical portInfo;
        portInfo.SetUuid(portUuid.c_str());
        nRet = pPhyPort->Query(portInfo);
        if( nRet == VNET_DB_SUCCESS)
        {
            // 不管是上报还是配置，只要存在一个loop，即认为是loop
            if(portInfo.GetIsloopCfg() || portInfo.GetIsloopRep() )
            {
                outResult = TRUE;
            }
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR,"VNet wildcastCreateVPort::CheckSwitchUplinkPortIsLoop query port(%s) failed. ret:%d\n",\
             portUuid.c_str(),nRet ); 
            return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
        }

        return WC_CREATE_VPORT_OPER_SUCCESS;
    }
    
    if( (nRet != VNET_DB_SUCCESS ) && \
        !VNET_DB_IS_ITEM_NO_EXIST(nRet))
    {
        // 不存在此网口的情况，忽略 
        VNET_LOG(VNET_LOG_ERROR,"VNet wildcastCreateVPort::CheckSwitchUplinkPortIsLoop query physiacl portname(%s,%s) failed. ret:%d\n",\
             vnaUuid.c_str(),portName.c_str(),nRet ); 
            return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    // uplink loop port
    CDBOperatePortUplinkLoop* pUplinkloopPort  = GetDbIPortUplinkLoop();
    if( NULL == pUplinkloopPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckSwitchUplinkPortIsLoop: GetDbIPortUplinkLoop() is NULL.\n");
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }
    nRet = pUplinkloopPort->QueryUuid(vnaUuid.c_str(), portName.c_str(),PORT_TYPE_UPLINKLOOP, portUuid);
    if( nRet == VNET_DB_SUCCESS)
    {
        outResult = TRUE;
        return WC_CREATE_VPORT_OPER_SUCCESS;
    }
    
    if( (nRet != VNET_DB_SUCCESS ) && \
        !VNET_DB_IS_ITEM_NO_EXIST(nRet))
    {
        // 这个时候 还获取不到，说明此端口不应该是uplink port, 还有可能是bond
        VNET_LOG(VNET_LOG_ERROR,"VNet wildcastCreateVPort::CheckSwitchUplinkPortIsLoop query uplinkloop loop portname(%s,%s) failed. ret:%d\n",\
             vnaUuid.c_str(),portName.c_str(),nRet ); 
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    // bond port
    CDBOperateBond* pBodnPort  = GetDbIBond();
    if( NULL == pBodnPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::CheckSwitchUplinkPortIsLoop: GetDbIBond() is NULL.\n");
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }
    nRet = pUplinkloopPort->QueryUuid(vnaUuid.c_str(), portName.c_str(),PORT_TYPE_BOND, portUuid);
    if( nRet == VNET_DB_SUCCESS)
    {
        // uplink 是bond的情况下，肯定为非环回
        return WC_CREATE_VPORT_OPER_SUCCESS;
    }
    else
    {
        // 这个时候 还获取不到，说明此端口不应该是uplink port,
        // 因为前面已经对physical，uplink loop port 都已经作了判断
        VNET_LOG(VNET_LOG_ERROR,"VNet wildcastCreateVPort::CheckSwitchUplinkPortIsLoop query uplinkloop bond portname(%s,%s) failed. ret:%d\n",\
             vnaUuid.c_str(),portName.c_str(),nRet ); 
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }
    
    return WC_CREATE_VPORT_OPER_SUCCESS; 
}

int32 CWildcastCreateVPort::SendMsgToPortKernel(CDbWildcastTaskCreateVPort & info)
{
    CPortCfgMsgToVNA msg;        

    msg.m_strWCTaskUuid = info.GetUuid();
    msg.m_strName = info.GetPortName();
    msg.m_strVnaUuid = info.GetVnaUuid();

    msg.m_nType = info.GetKernelType();
    msg.m_strPgUuid = info.GetKernelPgUuid();

    msg.m_nIsDhcp = true;  // wildcast 肯定是dhcp方式 
    int32 nRet = 0;

    // 根据vna switch uuid 获取uplink portname , switch name    
    nRet = GetSwitchUplinkPortName(msg.m_strVnaUuid, info.GetSwitchUuid(),msg.m_strSwitchName, msg.m_strUplinkPort);
    if( nRet != WC_CREATE_VPORT_OPER_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortKernel: GetSwitchUplinkPortName(%s,%s) failed. ret:%d.\n",
            msg.m_strVnaUuid.c_str(), info.GetSwitchUuid().c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    //kernel 所在的switch uplink port 不能为uplink loop , sriov loop
    int32 isLoopPort = FALSE;
    nRet = CheckSwitchUplinkPortIsLoop(msg.m_strVnaUuid, msg.m_strUplinkPort,isLoopPort);
    if( nRet != WC_CREATE_VPORT_OPER_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortKernel: CheckSwitchUplinkPortIsLoop(%s,%s) failed. ret:%d.\n",
            msg.m_strVnaUuid.c_str(), msg.m_strUplinkPort.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    if( isLoopPort == TRUE)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortKernel: CheckSwitchUplinkPortIsLoop(%s,%s) is loop..\n",
            msg.m_strVnaUuid.c_str(), msg.m_strUplinkPort.c_str());
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    //获取pg属性[mtu vlan]
    CPortGroupMgr *pPgHandle = CPortGroupMgr::GetInstance();
    if (pPgHandle)
    {
        CPortGroup cPGInfo;
        cPGInfo.SetUuid(msg.m_strPgUuid);
        pPgHandle->GetPGDetail(cPGInfo);
        //判断模式
        if (EN_SWITCHPORT_MODE_ACCESS == cPGInfo.m_nSwitchPortMode)
        {
            msg.m_nVlan = cPGInfo.m_nAccessVlanNum;
            msg.m_nMtu  = cPGInfo.m_nMTU;
        }
        
        if (EN_SWITCHPORT_MODE_TRUNK == cPGInfo.m_nSwitchPortMode)
        {
            msg.m_nVlan = cPGInfo.m_nTrunkNativeVlan;
            msg.m_nMtu  = cPGInfo.m_nMTU;
        }
    }    

    msg.m_nOper = EN_ADD_KERNALPORT;

    //调用PORT模块接口
    CPortMgr *pPMgr = CPortMgr::GetInstance();
    if(NULL == pPMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortKernel: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_WC_GET_PORT_INSTANCE_NULL;
    }

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastCreateVPort module create kernel port send msg(%s,%s,%s,%d,%d,%s,%s) to PortMgr.\n",\
       msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper,
       msg.m_nType,msg.m_strPgUuid.c_str(),msg.m_strSwitchName.c_str());

    nRet = pPMgr->ProcKernelPortWildcast(msg);
    if( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SendMsgToPortKernel: pPMgr->ProcKernelPortWildcast(%s,%s,%d,%s,%s,%s) failed. ret=%d.\n",
            msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_nType,msg.m_strVnaUuid.c_str(),\
            msg.m_strPgUuid.c_str(),msg.m_strSwitchName.c_str(), nRet);
        return nRet;
    }    

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastCreateVPort module create kernel port(%s,%s,%s,%d,%d,%s,%s) success.\n",\
       msg.m_strWCTaskUuid.c_str(),msg.m_strName.c_str(),msg.m_strVnaUuid.c_str(),msg.m_nOper,
       msg.m_nType,msg.m_strPgUuid.c_str(),msg.m_strSwitchName.c_str());
    
    return WC_CREATE_VPORT_OPER_SUCCESS; 
}

int32 CWildcastCreateVPort::SendMsgToPort(CDbWildcastTaskCreateVPort & info)
{    
    // create uplinkloop port
    if( info.GetPortType() == PORT_TYPE_UPLINKLOOP )
    {
        return SendMsgToPortUplinkLoop(info);
    }

    // create kernel port
    if( info.GetPortType() == PORT_TYPE_KERNEL )
    {
        return SendMsgToPortKernel(info);
    }
    
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

int32 CWildcastCreateVPort::WildcastTaskMsgAck(const string & strTaskUuid, int32 nResult)
{
    CDBOperateWildcastTaskCreateVPort * pCreateVPort =  GetDBCreateVPortTask();
    if(NULL == pCreateVPort)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::WildcastTaskMsgAck: GetDBCreateVPortTask() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    } 

    CDbWildcastTaskCreateVPort info;
    info.SetUuid(strTaskUuid.c_str());
    int32 nRet = pCreateVPort->Query(info);
    
    // 没有找到，说明没有task，可能前面已经处理成功了。
    if( VNET_DB_IS_ITEM_NO_EXIST(nRet))
    {
        if(DbgGetDBPrint())
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::WildcastTaskMsgAck: pCreateVPort->Query(%s) failed.%d\n",
              info.GetUuid().c_str(), nRet);
        }
        return WC_CREATE_VPORT_OPER_SUCCESS;
    }
    
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::WildcastTaskMsgAck: pCreateVPort->Query(%s) failed.%d\n",
          info.GetUuid().c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    if( 0 == nResult )
    {
        // 成功后，可以将此记录删除
        nRet = pCreateVPort->Del(strTaskUuid.c_str());
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::WildcastTaskMsgAck: pCreateVPort->Del(%s) failed.%d\n",
              strTaskUuid.c_str(), nRet);
            return ERROR_WC_CREATE_VPORT_SET_DB_FAIL;
        }
    }
    else
    {
        info.SetState(WILDCAST_TASK_STATE_ACK_FAILED);
        nRet = pCreateVPort->Modify(info);
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::SwitchMsgAck: pCreateVPort->Modify(%s) failed.%d\n",
              strTaskUuid.c_str(), nRet);
            return ERROR_WC_CREATE_VPORT_SET_DB_FAIL;
        }
    }
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

// 供port module 调用 
int32 CWildcastCreateVPort::IsConflict(const string & vnaUuid, const string & portName, int32 & outResult)
{
    outResult = false;
    CDBOperateWildcastTaskCreateVPort * pCreateVPortTask =  GetDBCreateVPortTask();
    if(NULL == pCreateVPortTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::isConflict: GetDBCreateVPortTask() is NULL.\n");
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }   
    
    vector<CDbWildcastTaskCreateVPort>  outVInfo;
    int32 nRet = pCreateVPortTask->QuerySummaryByVna(vnaUuid.c_str(),outVInfo );
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastCreateVPort::isConflict: pCreateVPort->QuerySummaryByVna(%s) failed.%d\n",
          vnaUuid.c_str(), nRet);
        return ERROR_WC_CREATE_VPORT_GET_DB_FAIL;
    }

    outResult = false;
    vector<CDbWildcastTaskCreateVPort>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if(0 == STRCMP((*it).GetPortName().c_str(), portName.c_str()))
        {
            outResult = true;
            return WC_CREATE_VPORT_OPER_SUCCESS;
        }
    }    
    
    return WC_CREATE_VPORT_OPER_SUCCESS;
}

void CWildcastCreateVPort::DbgShow()
{
    
}
    
// test code 
void wc_createvport(char * uuid, char * PortName,int32 PortType, int32 IsHasKernelType, 
      int32 KernelType, char* kernelPgUuid, char* switchUuid, int32 oper)
{
    CWildcastCreateVPortCfgMsg cMsg;
    CWildcastCreateVPort createvport;

    if(NULL == uuid || NULL == PortName )
    {
        cout << "param invalid." << endl;
        return;
    }

    cMsg.m_strUuid = uuid;
    cMsg.m_strPortName = PortName;
    cMsg.m_nPortType = PortType;
    cMsg.m_nIsHasKenelType= IsHasKernelType;
    cMsg.m_nKernelType =KernelType;
    cMsg.m_strKernelPgUuid=kernelPgUuid;
    cMsg.m_strSwitchUuid=switchUuid;    
    cMsg.m_nOper = oper;
        
    int32 nRet = WC_CREATE_VPORT_OPER_SUCCESS;

    cMsg.Print();
    if( EN_ADD_WC_PORT == oper)
    {
        nRet = createvport.Add(cMsg) ;
        if( WC_CREATE_VPORT_OPER_SUCCESS == nRet)
        {    
            cout << "AddSwitch success" << endl;
            cMsg.Print();
        }
        else
        {
            cout << "AddSwitch failed."<< nRet << endl;
        }
    }
   
    if( EN_DEL_WC_PORT == oper)
    {
        nRet = createvport.Del(cMsg.m_strUuid) ;
        if( WC_CREATE_VPORT_OPER_SUCCESS == nRet)
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
DEFINE_DEBUG_FUNC(wc_createvport);


void wc_createvport_newvna(char * uuid)
{
    CWildcastCreateVPort createvport;
    string vnauuid(uuid);
    int32 nRet = createvport.NewVnaRegist(vnauuid);
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_createvport_newvna failed. " << nRet << endl;        
    }
    else
    {
        cout << "wc_createvport_newvna success" << endl;
    }
}DEFINE_DEBUG_FUNC(wc_createvport_newvna);

void wc_createvport_timeout()
{
    CWildcastCreateVPort createvport;
    int32 nRet = createvport.ProcTimeout();
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_createvport_timeout failed. " << nRet << endl;
    }
    else
    {
        cout << "wc_createvport_timeout success" << endl;
    }
     
}DEFINE_DEBUG_FUNC(wc_createvport_timeout);

void wc_createvport_isconflict(char* vna_uuid, char* portname)
{
    CWildcastCreateVPort createvport;
    string strVnauuid(vna_uuid);
    string strPortname(portname);
    int32 nResult = 0;
    
    int32 nRet = createvport.IsConflict(strVnauuid,strPortname,nResult);
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_createvport_isconflict failed. " << nRet << endl;
    }
    else
    {
        cout << "wc_createvport_isconflict success " << nResult << endl;
    }
     
}DEFINE_DEBUG_FUNC(wc_createvport_isconflict);

} // namespace 

