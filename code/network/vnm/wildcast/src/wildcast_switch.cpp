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
#include "vnet_portmgr.h"
#include "vnet_db_schedule.h"
#include "wildcast_switch.h"

namespace zte_tecs
{

CDBOperateWildcastSwitch * CWildcastVSwitch::GetDBVSwitch()
{
    CDBOperateWildcastSwitch * pVSwitch = GetDbIWildcastSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::GetDBVSwitch: GetDbIWildcastSwitch() is NULL.\n");
        return NULL;
    }
    return pVSwitch;
}

CDBOperateWildcastTaskSwitch * CWildcastVSwitch::GetDBVSwitchTask()
{
    CDBOperateWildcastTaskSwitch * pVSwitchTask = GetDbIWildcastTaskSwitch();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::GetDBVSwitchTask: GetDbIWildcastTaskSwitch() is NULL.\n");
        return NULL;
    }
    return pVSwitchTask;
}

STATUS CWildcastVSwitch::ProcSwitchMsg(CWildcastSwitchCfgMsg& cMsg)
{
    int32 nRet = WC_SWITCH_OPER_SUCCESS;
    
    //入参检查
    if(cMsg.m_strName.empty() && cMsg.m_strUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcSwitchReqMsg Switch name/uuid is invalid.\n");
        nRet = ERROR_WC_SWITCH_NAME_EMPTY;
        return nRet;
    }

    switch(cMsg.m_nOper)
    {
        case EN_ADD_SWITCH:
        {
            nRet = AddSwitch(cMsg);
            break;
        }
        case EN_DEL_SWITCH:
        {
            nRet = DelSwitch(cMsg.m_strUuid);
            if(WC_SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet;    
            }
            break;
        }
        case EN_MODIFY_SWITCH:
        {
            nRet = ModifySwitch(cMsg);
            if(WC_SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet;    
            }
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcSwitchMsg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);  
            nRet = ERROR_WC_SWITCH_OPERCODE_INVALID;
        }    
    }
    
    return nRet;
}

STATUS CWildcastVSwitch::AddSwitch(CWildcastSwitchCfgMsg& cMsg)
{
    /* PG不能为空 */
    if(cMsg.m_strPGUuid.empty()) 
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitch PG is Empty.\n");
        return ERROR_WC_SWITCH_PG_EMPTY;
    }
    
    //PG类型必须为UPLINK PG
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if((NULL == pPGMgr)||(0 != pPGMgr->IsUplinkPG(cMsg.m_strPGUuid)))
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitch PG Type is not uplink pg.\n");
        return ERROR_WC_SWITCH_PGTYPE_INVALID;
    }  
    
    //EVB模式检查
    if((cMsg.m_nEvbMode > EN_EVB_MODE_VEPA) && (cMsg.m_nEvbMode < EN_EVB_MODE_NORMAL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitch EVB Mode<%d> is invalid.\n", cMsg.m_nEvbMode);
        return ERROR_WC_SWITCH_EVBMODE_INVALID;        
    }
    
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitch: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 

    vector<CDbWildcastSwitch> outVInfo;
    int32 nRet = pVSwitch->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {   
        vector<CDbWildcastSwitch>::iterator iter = outVInfo.begin();
        for( ; iter != outVInfo.end(); ++iter)
        {
            if( 0 == STRCMP(cMsg.m_strName.c_str(), iter->GetSwitchName().c_str()))
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitch: Switch(%s) is already exist.\n",
                    cMsg.m_strName.c_str());
                return ERROR_WC_SWITCH_ALREADY_EXIST;
            }
        }
    }
    
    CDbWildcastSwitch cDbVSInfo;
    cDbVSInfo.SetSwitchType(cMsg.m_nType);
    cDbVSInfo.SetSwitchName(cMsg.m_strName.c_str());
    cDbVSInfo.SetState(1);
    cDbVSInfo.SetMtuMax(cMsg.m_nMTU);
    cDbVSInfo.SetEvbMode(cMsg.m_nEvbMode);
    cDbVSInfo.SetUplinkPgUuid(cMsg.m_strPGUuid.c_str());
    cDbVSInfo.SetNicMaxNum(cMsg.m_nMaxVnic);
    // cDbVSInfo.SetIsActive(1);

    nRet = pVSwitch->Add(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitch: Add switch to DB failed, ret = %d\n", nRet);
        return ERROR_WC_SWITCH_SET_DB_FAIL;
    }
    cMsg.m_strUuid = cDbVSInfo.GetUuid();
    
    return WC_SWITCH_OPER_SUCCESS;
}

STATUS CWildcastVSwitch::DelSwitch(const string &strUuid)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitch: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 

    int32 nRet = pVSwitch->Del(strUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitch: Del %s failed.\n", strUuid.c_str());
        return ERROR_WC_SWITCH_SET_DB_FAIL;
    }
    return WC_SWITCH_OPER_SUCCESS;
}

STATUS CWildcastVSwitch::ModifySwitch(const CWildcastSwitchCfgMsg& cMsg)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ModifySwitch: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 

    CDbWildcastSwitch cDbVSInfo;    
    cDbVSInfo.SetUuid(cMsg.m_strUuid.c_str());

    // 首先获取该switch信息
    int32 nRet = pVSwitch->Query(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ModifySwitch: query switch %s failed, ret = %d\n",cMsg.m_strUuid.c_str(), nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }
    
    cDbVSInfo.SetSwitchName(cMsg.m_strName.c_str());
    cDbVSInfo.SetSwitchType(cMsg.m_nType);
    cDbVSInfo.SetState(1);
    cDbVSInfo.SetMtuMax(cMsg.m_nMTU);
    cDbVSInfo.SetEvbMode(cMsg.m_nEvbMode);
    cDbVSInfo.SetNicMaxNum(cMsg.m_nMaxVnic);
    cDbVSInfo.SetUplinkPgUuid(cMsg.m_strPGUuid.c_str());

    nRet = pVSwitch->Modify(cDbVSInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ModifySwitch: Modify VSwitch DB failed, Ret = %d\n", nRet);
        return ERROR_WC_SWITCH_SET_DB_FAIL;
    }    

    return WC_SWITCH_OPER_SUCCESS;
}

STATUS CWildcastVSwitch::ProcSwitchPortMsg(CWildcastSwitchPortCfgMsg& cMsg)
{
    int32 nRet =  WC_SWITCH_OPER_SUCCESS;
    
    //入参检查
    if(cMsg.m_strSwName.empty() && cMsg.m_strSwUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcSwitchPortMsg Switch name&uuid is invalid.\n");
        nRet = ERROR_WC_SWITCH_NAME_EMPTY;
        return nRet;
    }
    
    switch(cMsg.m_nOper)
    {
        case EN_ADD_SWITCH_PORT:
        {
            nRet = AddSwitchPort(cMsg);
            if(WC_SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet;  
            }
            break;
        }
        case EN_DEL_SWITCH_PORT:
        {
            nRet = DelSwitchPort(cMsg);
            if(WC_SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet;  
            }
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcSwitchPortMsg: Msg operation code (%d) is invalid.\n", 
            cMsg.m_nOper);  
            nRet = ERROR_WC_SWITCH_OPERCODE_INVALID;
        }    
    }
    
    return nRet;
}

STATUS CWildcastVSwitch::GetSwitchPort(CDBOperateWildcastSwitch * pVSwitch,
     CDbWildcastSwitch & info, int32 & nOldPortNum, vector<string> &vOldPort)
{
    if( EN_DB_PORT_TYPE_PHYSICAL == info.GetUplinkPortType() || \
        EN_DB_PORT_TYPE_UPLINKLOOP == info.GetUplinkPortType()  )
    {
        if(0 != STRCMP(info.GetUplinkPortName().c_str(),""))
        {
            nOldPortNum = 1;
            vOldPort.push_back(info.GetUplinkPortName());
        }
    }

    if( EN_DB_PORT_TYPE_BOND == info.GetUplinkPortType() )
    {
        vector<CDbWildcastSwitchBondMap>  vBondMap;
        int32 nRet = pVSwitch->QueryBondMap(info.GetUuid().c_str(),vBondMap);
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::GetSwitchPort: pVSwitch->QueryBondMap() failed %d.\n", nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;            
        }

        nOldPortNum = vBondMap.size();
        vector<CDbWildcastSwitchBondMap>::iterator it = vBondMap.begin();
        for(; it != vBondMap.end(); ++it)
        {
            vOldPort.push_back((*it).GetBondPhyName());
        }
    }
    return WC_SWITCH_OPER_SUCCESS;
}

STATUS CWildcastVSwitch::AddSwitchPortWriteDb(CDbWildcastSwitch & info, CWildcastSwitchPortCfgMsg &cMsg, 
        int32 nOldPortNum, vector<string> & vOldPort,
        vector<string> & vAddPort, CDBOperateWildcastSwitch * pVSwitch)
{
    int32 nRet = 0;
    // 原先没有端口
    if( 0 >= nOldPortNum )
    {
        if(1 == vAddPort.size())
        {
            info.SetUplinkPortType(cMsg.m_nPortType);
            vector<string>::iterator it = vAddPort.begin();
            info.SetUplinkPortName((*it).c_str());
            nRet = pVSwitch->Modify(info);
            if( VNET_DB_SUCCESS != nRet)
            {
                info.DbgShow();
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->Modify() failed %d.\n",nRet);
                return ERROR_WC_SWITCH_SET_DB_FAIL;
            }
            return WC_SWITCH_OPER_SUCCESS;
        }
        else
        {
            info.SetUplinkPortType(EN_DB_PORT_TYPE_BOND);
            info.SetUplinkPortName("");
            info.SetBondMode(cMsg.m_nBondMode);
            nRet = pVSwitch->Modify(info);
            if( VNET_DB_SUCCESS != nRet)
            {
                info.DbgShow();
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->Modify() failed %d.\n",nRet);
                return ERROR_WC_SWITCH_SET_DB_FAIL;
            }

            // 添加bond map
            vector<string>::iterator it = vAddPort.begin();
            for( ; it != vAddPort.end(); ++it)
            {
                nRet = pVSwitch->AddBondMap(cMsg.m_strSwUuid.c_str(), (*it).c_str());
                if( VNET_DB_SUCCESS != nRet)
                {
                    info.DbgShow();
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->AddBondMap() failed %d.\n",nRet);
                    return ERROR_WC_SWITCH_SET_DB_FAIL;
                }
            }
        }        
    }
    else if( 1 == nOldPortNum )
    {
        info.SetUplinkPortType(EN_DB_PORT_TYPE_BOND);
        info.SetUplinkPortName("");
        info.SetBondMode(cMsg.m_nBondMode);
        nRet = pVSwitch->Modify(info);
        if( VNET_DB_SUCCESS != nRet)
        {
            info.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->Modify() failed %d.\n",nRet);
            return ERROR_WC_SWITCH_SET_DB_FAIL;
        }

        // 先添加原先的port
        vector<string>::iterator it_old = vOldPort.begin();        
        nRet = pVSwitch->AddBondMap(cMsg.m_strSwUuid.c_str(), (*it_old).c_str());
        if( VNET_DB_SUCCESS != nRet)
        {
            info.DbgShow();            
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->AddBondMap() failed %d.\n",nRet);
            return ERROR_WC_SWITCH_SET_DB_FAIL;
        }

        // 添加新port
        vector<string>::iterator it = vAddPort.begin();
        for( ; it != vAddPort.end(); ++it)
        {
            nRet = pVSwitch->AddBondMap(cMsg.m_strSwUuid.c_str(), (*it).c_str());
            if( VNET_DB_SUCCESS != nRet)
            {   
                info.DbgShow();                
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->AddBondMap() failed %d.\n",nRet);
                return ERROR_WC_SWITCH_SET_DB_FAIL;
            }
        }
    }
    else
    {
        // 添加新port
        vector<string>::iterator it = vAddPort.begin();
        for( ; it != vAddPort.end(); ++it)
        {
            nRet = pVSwitch->AddBondMap(cMsg.m_strSwUuid.c_str(), (*it).c_str());
            if( VNET_DB_SUCCESS != nRet)
            {
                info.DbgShow();
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPortWriteDb: pVSwitch->AddBondMap() failed %d.\n",nRet);
                return ERROR_WC_SWITCH_SET_DB_FAIL;
            }
        }
    }
    return WC_SWITCH_OPER_SUCCESS;
}

//添加交换端口
STATUS CWildcastVSwitch::AddSwitchPort(CWildcastSwitchPortCfgMsg &cMsg)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPort: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 

    // 获取原先有的接口 
    CDbWildcastSwitch info;
    info.SetUuid(cMsg.m_strSwUuid.c_str());
    int32 nRet = pVSwitch->Query(info);
    if( VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPort: pVSwitch->Query() failed %d.\n",nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    // 需要将BOND模式设置上
    info.SetBondMode(cMsg.m_nBondMode);

    //获取原先接口类型和端口数目
    int32 nOldPortNum = 0;
    vector<string> vOldPort;
    if( WC_SWITCH_OPER_SUCCESS != GetSwitchPort(pVSwitch, info,nOldPortNum,vOldPort) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPort: GetSwitchPort failed.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    int32 bExist = false;
    vector<string> vAddPort; 
    // 是否已经存在
    vector<string>::iterator inIt = cMsg.m_vPort.begin();
    vector<string>::iterator it;
    for(; inIt != cMsg.m_vPort.end(); ++inIt)
    {
        bExist = false;        
        for(it = vOldPort.begin(); it != vOldPort.end(); ++it)
        {
            if( 0 == STRCMP((*inIt).c_str(), (*it).c_str()))
            {
                bExist = true;
                break;

            }
        }

        if( false == bExist )
        {
            vAddPort.push_back((*inIt));
        }
    }

    // 操作表
    if( vAddPort.size() <= 0 )
    {

        return WC_SWITCH_OPER_SUCCESS;
    }

    if(0)
    {
        cout << "-------------begin"<< endl;
        vector<string>::iterator it = vOldPort.begin();
        cout << "vOldPort--------" << endl;
        cout << "nOldPortNum: " << nOldPortNum << endl;
        for(; it != vOldPort.end(); ++it)
        {
            cout << "----------" << endl;
            cout << (*it) << endl;
        }

        it = vAddPort.begin();
        cout << "vAddPort--------" << endl;
        for(; it != vAddPort.end(); ++it)
        {
            cout << "----------" << endl;
            cout << (*it) << endl;
        }
        cout << "-------------end"<< endl;
    }
    
    // 如果多个网口，网口名称不能为loop*
    if( vOldPort.size() > 0  || vAddPort.size() > 1 )
    {
        vector<string>::iterator it = vAddPort.begin();
        string sTmp("");
        for(; it != vAddPort.end(); ++it)
        {
            if( (*it).size() > sizeof("loop") )
            {
                sTmp.assign((*it), 0, sizeof("loop"));
                if( sTmp == "loop")
                {
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPort: failed, vAddPort has loop port.\n");
                    return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
                }
            }
        }
    }

    // 添加switch port 检查 
    nRet = CheckAddSwitchPort(info,vOldPort,vAddPort);
    if( SWITCH_OPER_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPort: AddSwitchPortWriteDb failed %d.\n",nRet);
        return nRet;
    }
    
    // nRet = CheckSdvsEdvsMix(info, vOldPort, vAddPort );
    
    
    nRet = AddSwitchPortWriteDb(info, cMsg, nOldPortNum,vOldPort,
        vAddPort,  pVSwitch);
    if( SWITCH_OPER_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::AddSwitchPort: AddSwitchPortWriteDb failed.\n");
        return ERROR_WC_SWITCH_SET_DB_FAIL;
    }
    
    return WC_SWITCH_OPER_SUCCESS;
}

/*
   1 oldPort = 0 
   1.1 addport = 1 
       1.1.1 addport不能在bond map中
       1.1.2 相同Switch type, uplink port不能已经存在此addport
       1.1.3 如果存在不同switch type addport的情况(混用)，uplink pg 必须相同
*/

STATUS CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne(CDbWildcastSwitch & info, 
    string & addPort)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    int32 nRet = 0;
    
    // 1.1.1 addport 不能在 bondmap中     
    vector<CDbWildcastSwitchBondMap> vDbWsBond;

    // 获取到相同uplink 名称的switch列表(bond)
    nRet =  pVSwitch->QueryBondMapByPhyPortName(addPort.c_str(), vDbWsBond);
    if( VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: pVSwitch->QueryBondMapByPhyPortName(%s) failed. ret:%d\n",
          addPort.c_str(), nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    if( vDbWsBond.size() > 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: uplink port is conflict, bond has the port. \n");
        return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
    }
    
    // 1.1.2 相同Switch type, uplink port不能已经存在此addport
    vector<CDbWildcastSwitch> vDbWs;

    // 获取到相同uplink 名称的switch列表(非bond)
    nRet =  pVSwitch->QueryByPortName(addPort.c_str(), vDbWs);
    if( VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: pVSwitch->QueryByPortName(%s) failed. ret:%d\n",
          addPort.c_str(), nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }
    vector<CDbWildcastSwitch>::iterator itDbws = vDbWs.begin();
    for(; itDbws != vDbWs.end(); ++itDbws)
    {
        // 只判断不同swtich 类型的switch情况 
        if((*itDbws).GetSwitchType() == info.GetSwitchType() )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: uplink port is conflict.\n");
            return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
        }
        else
        {
            // 存在混用情况 
            // 1.1.3 如果存在不同switch type addport的情况(混用)，uplink pg 必须相同
            if( (*itDbws).GetUplinkPgUuid() != info.GetUplinkPgUuid() )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: switch mix, uplink pg must be the same.\n");
                return ERROR_WC_SWITCH_MIX_UPLINKPG_MUST_SAME;
            }

            // 1.1.4 如果存在不同switch type addport的情况(混用)，evb mode 必须相同
            if( (*itDbws).GetEvbMode() != info.GetEvbMode() )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPOne: switch mix, evb mode must be the same.\n");
                return ERROR_WC_SWITCH_MIX_EVBMODE_MUST_SAME;
            }
        }
    }   

    return WC_SWITCH_OPER_SUCCESS;    
}

/*
1 oldPort = 0 
1.2 addport > 1        
       1.2.1 所有addport网口都不能在 相同switch type的uplink port 
       1.2.2 所有addport网口都不能在 相同switch type的bond map中       
       1.2.3 不同switch type (混用), 必须addport 和它一致，uplink pg 必须相同
       1.2.4 bond mode 必须相同
*/
STATUS CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany(CDbWildcastSwitch & info, 
    vector<string> & vAddPort)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    int32 nRet = 0;
    
    // 1.2.1 所有addport网口都不能在 相同switch type的uplink port   
    vector<CDbWildcastSwitch> vDbWs;

    // 获取到相同所有switch  从而获取到 uplink port 
    nRet =  pVSwitch->QuerySummary(vDbWs);
    if( VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: pVSwitch->QuerySummary() failed. ret:%d\n",
           nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }
    vector<CDbWildcastSwitch>::iterator itDbws = vDbWs.begin();
    for(; itDbws != vDbWs.end(); ++itDbws)
    {
        // 只判断不同swtich 类型的switch情况 
        if((*itDbws).GetSwitchType() == info.GetSwitchType() && \
           (*itDbws).GetUplinkPortType() == PORT_TYPE_PHYSICAL )
        {
            if(find(vAddPort.begin(),vAddPort.end(),(*itDbws).GetUplinkPortName()) != vAddPort.end())
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: uplink port is conflict.\n");
                return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
            }                    
        }        
    }   

    // 1.2.2 所有addport网口都不能在 相同switch type的bond map中     
        
    // 获取到相同uplink phy port名称的switch列表(bond)
    vector<string>::iterator itAddP = vAddPort.begin();
    for(; itAddP != vAddPort.end(); ++itAddP)
    {
        vector<CDbWildcastSwitchBondMap> vDbWsBond;
        nRet =  pVSwitch->QueryBondMapByPhyPortName((*itAddP).c_str(), vDbWsBond);    
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: pVSwitch->QueryBondMapByPhyPortName(%s) failed. ret:%d\n",
              (*itAddP).c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }
    
        vector<CDbWildcastSwitchBondMap>::iterator itDbWsB = vDbWsBond.begin();
        for(; itDbWsB != vDbWsBond.end(); ++itDbWsB)
        {
            if( (*itDbWsB).GetSwitchType() == info.GetSwitchType() )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: uplink port is conflict, bond has the port. \n");
                return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
            }
        }
    }

    // 1.2.3 不同switch type (混用), 必须addport 和它一致，uplink pg 必须相同
    itAddP = vAddPort.begin();
    for(; itAddP != vAddPort.end(); ++itAddP)
    {
        vector<CDbWildcastSwitchBondMap> vDbWsBond;
        nRet =  pVSwitch->QueryBondMapByPhyPortName((*itAddP).c_str(), vDbWsBond);    
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: pVSwitch->QueryBondMapByPhyPortName(%s) failed. ret:%d\n",
              (*itAddP).c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        vector<string> vDbPort; 
    
        vector<CDbWildcastSwitchBondMap>::iterator itDbWsB = vDbWsBond.begin();
        for(; itDbWsB != vDbWsBond.end(); ++itDbWsB)
        {
            vDbPort.push_back((*itDbWsB).GetBondPhyName());        
        }
        
        if( vDbPort.size() > 0 )
        {
            // port 完全一致 
            nRet = IsVectorEqua(vAddPort, vDbPort);
            if( nRet != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: uplink port is conflict, IsVectorEqua . \n");
                return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
            }

            // 为方便判断switch evb mode 是否一致，首先获取到switch uuid
            set<string> sSwUuid;

            // uplinkg pg 必须相同
            itDbWsB = vDbWsBond.begin();
            for(; itDbWsB != vDbWsBond.end(); ++itDbWsB)
            {
                if(info.GetUplinkPgUuid() != (*itDbWsB).GetPgUuid() )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: switch mix, uplink pg must be the same. \n");
                    return ERROR_WC_SWITCH_MIX_UPLINKPG_MUST_SAME;
                }                
 
                // 1.2.4 bond mode 必须相同
                if( info.GetBondMode() != (*itDbWsB).GetBondMode())
                {
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: switch mix, uplink port bond mode must be the same. \n");
                    return ERROR_WC_SWITCH_MIX_UPLINK_BONDMODE_MUST_SAME;
                }

                sSwUuid.insert((*itDbWsB).GetUuid());                
            }

            // 1.2.4 evb mode 必须相同 明天继续.....
            set<string>::iterator itSwUuid = sSwUuid.begin();
            for(; itSwUuid != sSwUuid.end(); ++itSwUuid)
            {
                CDbWildcastSwitch dbinfo;
                dbinfo.SetUuid((*itSwUuid).c_str());
                nRet = pVSwitch->Query(dbinfo);
                if( nRet != VNET_DB_SUCCESS)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: query wildcast switch(%s) failed. \n",
                        info.GetUuid().c_str());
                    return ERROR_WC_SWITCH_GET_DB_FAIL;
                }

                cout << " " << dbinfo.GetEvbMode() << " " << info.GetEvbMode() << endl;
                if( dbinfo.GetEvbMode() != info.GetEvbMode() )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_OldPZeroAddPMany: switch mix, evb bond mode must be the same. \n");
                    return ERROR_WC_SWITCH_MIX_EVBMODE_MUST_SAME;
                }
            }
        }
    }
        
    return WC_SWITCH_OPER_SUCCESS;
}

int32 CWildcastVSwitch::IsVectorEqua(vector<string> new_vec, vector<string> old_vec)
{
    if(new_vec.size() != old_vec.size())
    {
        return -1;
    }
    vector<string>::iterator it = new_vec.begin();
    for(; it != new_vec.end(); ++it)
    {
        if(find(old_vec.begin(),old_vec.end(),*it) == old_vec.end())
        {
            return -1;
        }
    }
    return 0;
}

/*
   2 oldPort != 0  不可能存在混用情况 
   2.1 oldPort = 1 
       2.1.0 oldport 是否存在混用情况，如果是 不允许配置
       2.1.1 addport 不能在bond map中 
       2.1.2 addport 不能在switch uplink port中 
        
   2.2 oldPort > 1 
       2.1.0 oldport 是否存在混用情况，如果是 不允许配置
       addport 不能在bond map中 
       addport 不能在switch uplink port中 
*/
STATUS CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero(CDbWildcastSwitch & info, 
    vector<string> & vOldPort, vector<string> & vAddPort)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    int32 nRet = 0;

    // 2.1.0 oldport 是否存在混用情况，如果是 不允许配置
    if( vOldPort.size() == 1 )
    {
        vector<string>::iterator itOldP = vOldPort.begin();
        vector<CDbWildcastSwitch> vDbWs;
        nRet =  pVSwitch->QueryByPortName((*itOldP).c_str(), vDbWs);
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: pVSwitch->QueryByPortName(%s) failed. ret:%d\n",
              (*itOldP).c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        vector<CDbWildcastSwitch>::iterator itDbWs = vDbWs.begin();
        for(; itDbWs != vDbWs.end(); ++itDbWs)
        {
            if( (*itDbWs).GetSwitchType() != info.GetSwitchType() )
            {
                // 存在混用情况
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: uplink port is conflict 1. \n");
                return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
            }            
        }        
    }
    else
    {
        vector<string>::iterator itOldP = vOldPort.begin();
        for(; itOldP != vOldPort.end(); ++itOldP)
        {
            vector<CDbWildcastSwitchBondMap> vDbWsBond;
            nRet =  pVSwitch->QueryBondMapByPhyPortName((*itOldP).c_str(), vDbWsBond);    
            if( VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: pVSwitch->QueryBondMapByPhyPortName(%s) failed. ret:%d\n",
                  (*itOldP).c_str(), nRet);
                return ERROR_WC_SWITCH_GET_DB_FAIL;
            }
                        
            vector<CDbWildcastSwitchBondMap>::iterator itDbWsB = vDbWsBond.begin();
            for(; itDbWsB != vDbWsBond.end(); ++itDbWsB)
            {
                if( (*itDbWsB).GetSwitchType() != info.GetSwitchType() )
                {
                    // 存在混用情况
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: uplink port is conflict 2. \n");
                    return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
                }            
            }    
        }        
    }
    
    
    vector<string>::iterator itAddP = vAddPort.begin();
    for(; itAddP != vAddPort.end(); ++itAddP)
    {
        // 2.1.1 addport 不能在bond map中  
        // 获取到相同uplink phy port名称的switch列表(bond)
        vector<CDbWildcastSwitchBondMap> vDbWsBond;
        nRet =  pVSwitch->QueryBondMapByPhyPortName((*itAddP).c_str(), vDbWsBond);    
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: pVSwitch->QueryBondMapByPhyPortName(%s) failed. ret:%d\n",
              (*itAddP).c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        if( vDbWsBond.size() > 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: uplink port is conflict, bond has the port. \n");
            return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
        }
    
        //2.1.2 addport 不能在switch uplink port中 
        vector<CDbWildcastSwitch> vDbWs;
        nRet =  pVSwitch->QueryByPortName((*itAddP).c_str(), vDbWs);
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: pVSwitch->QueryByPortName(%s) failed. ret:%d\n",
              (*itAddP).c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        if( vDbWsBond.size() > 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort_oldPNotZero: uplink port is conflict, other switch has the port. \n");
            return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
        }
    }

    return WC_SWITCH_OPER_SUCCESS;
}
    
int32 CWildcastVSwitch::CheckSwitchPortIsExistSame(vector<string> & vOldPort, vector<string> & vAddPort)
{
    set<string> setPort;
    
    vector<string>::iterator itPort = vOldPort.begin();
    for(; itPort != vOldPort.end(); itPort++)
    {
        setPort.insert(*itPort);
    }

    itPort = vAddPort.begin();
    for(; itPort != vAddPort.end(); itPort++)
    {
        setPort.insert(*itPort);
    }

    if( setPort.size() != (vOldPort.size() + vAddPort.size()) )
    {
        return true;        
    }
    
    return false;
}

// 添加switch port 检查函数 
/*
   1 oldPort = 0 
   1.1 addport = 1 
       1.1.1 addport不能在bond map中
       1.1.2 相同Switch type, uplink port不能已经存在此addport
       1.1.3 如果存在不同switch type addport的情况(混用)，uplink pg 必须相同,evb mode 必须相同
   1.2 addport > 1        
       1.2.1 所有addport网口都不能在 相同switch type的uplink port 
       1.2.2 所有addport网口都不能在 相同switch type的bond map中       
       1.2.3 不同switch type (混用), 必须addport 和它一致，uplink pg 必须相同,evb mode 必须相同
       1.2.4 bond mode 必须相同
       
   2 oldPort != 0  不可能存在混用情况 
   2.1 oldPort = 1 
       2.1.0 oldport 是否存在混用情况，如果是 不允许配置
       2.1.1 addport 不能在bond map中 
       2.1.2 addport 不能在switch uplink port中 
       bond mode 必须相同
        
   2.2 oldPort > 1 
       2.1.0 oldport 是否存在混用情况，如果是 不允许配置
       addport 不能在bond map中 
       addport 不能在switch uplink port中 
       bond mode 必须相同
*/

STATUS CWildcastVSwitch::CheckAddSwitchPort(CDbWildcastSwitch & info, 
    vector<string> & vOldPort, vector<string> & vAddPort)
{
    // 判断是否存在混用情况 
    if( vAddPort.size() == 0 )
    {
        return WC_SWITCH_OPER_SUCCESS;
    }
    
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    // vOldPort  vAddPort 不能相同 
    if( 0 != CheckSwitchPortIsExistSame( vOldPort,  vAddPort) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckAddSwitchPort: switch is exist same port.\n");
        return ERROR_WC_SWITCH_PARAM_INVALID;
    }

    // 1 oldPort = 0
    if( vOldPort.size() == 0 )
    {
        // 1.1 addport = 1 
        if( vAddPort.size() == 1 )
        {            
            vector<string>::iterator it = vAddPort.begin();
            string addport = (*it);
            return CheckAddSwitchPort_OldPZeroAddPOne(info, addport);
        }
        else
        {
            return CheckAddSwitchPort_OldPZeroAddPMany(info, vAddPort);
        }
    }
    else
    {
        return CheckAddSwitchPort_oldPNotZero(info,vOldPort,vAddPort);
    }
        
    return WC_SWITCH_OPER_SUCCESS;
}

/*
    vDelPort必须在vOldPort中，已在DelSwitchPort中已作检查
    
    1 vOldPort - vDelPort = 0 
      不作任何限制
    2 vOldPort - vDelPort > 0 
      查看下vDelPort是否存在混用情况 
*/
int32 CWildcastVSwitch::CheckDelSwitchPort(CDbWildcastSwitch & info, 
    vector<string> & vOldPort, vector<string> & vDelPort)
{
    int32 nRet = 0;
    int32 nSurplus = vOldPort.size() - vDelPort.size();
    if( nSurplus <= 0 )
    {
        return WC_SWITCH_OPER_SUCCESS; 
    }

    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckDelSwitchPort: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    // 说明不是全部删除
    vector<string>::iterator it = vDelPort.begin();
    for(; it != vDelPort.end(); ++it)
    {
        // 获取到相同uplink phy port名称的switch列表(bond)
        vector<CDbWildcastSwitchBondMap> vDbWsBond;
        nRet =  pVSwitch->QueryBondMapByPhyPortName((*it).c_str(), vDbWsBond);    
        if( VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckDelSwitchPort: pVSwitch->QueryBondMapByPhyPortName(%s) failed. ret:%d\n",
              (*it).c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        vector<CDbWildcastSwitchBondMap>::iterator itDbWsB = vDbWsBond.begin();
        for(; itDbWsB != vDbWsBond.end(); ++itDbWsB)
        {
            if( (*itDbWsB).GetSwitchType() != info.GetSwitchType() )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::CheckDelSwitchPort: uplink port is conflict, bond has the port. \n");
                return ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT;
            }
        }
    }
    

    return WC_SWITCH_OPER_SUCCESS;
}

STATUS CWildcastVSwitch::DelSwitchPort(CWildcastSwitchPortCfgMsg &cMsg)
{
    CDBOperateWildcastSwitch * pVSwitch = GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    // 获取原先有的接口 
    CDbWildcastSwitch info;
    info.SetUuid(cMsg.m_strSwUuid.c_str());
    int32 nRet = pVSwitch->Query(info);
    if( VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: pVSwitch->Query() failed %d.\n",nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    //获取原先接口类型和端口数目
    int32 nOldPortNum = 0;
    vector<string> vOldPort;
    if( SWITCH_OPER_SUCCESS != GetSwitchPort(pVSwitch,info,nOldPortNum,vOldPort) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: GetSwitchPort failed.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    int32 bExist = false;
    vector<string>::iterator it;
    
    vector<string>::iterator itIn = cMsg.m_vPort.begin();
    for(; itIn != cMsg.m_vPort.end(); ++itIn)
    {
        bExist = false;        
        for(it = vOldPort.begin(); it != vOldPort.end(); ++it)
        {
            if( 0 == STRCMP((*it).c_str(), (*itIn).c_str()))
            {
                bExist = true;
            }
        }

        if( false == bExist )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: port(%s) is no exist.\n",
                (*itIn).c_str());
            return ERROR_WC_SWITCH_NOTEXIST_PORT;
        }
    }

    // 操作表 
    if( cMsg.m_vPort.size() <= 0 )
    {
        return WC_SWITCH_OPER_SUCCESS;
    }

    // 删除交换端口检查 
    nRet = CheckDelSwitchPort(info, vOldPort, cMsg.m_vPort);
    if( nRet != VNET_DB_SUCCESS)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: CheckDelSwitchPort failed. %d\n",
               nRet);
        return nRet;
    }

    int32 nSurplus = (vOldPort.size() - cMsg.m_vPort.size());
    if( nSurplus == 0 )
    {
        if( vOldPort.size() > 1)

        {
            for(itIn = cMsg.m_vPort.begin(); itIn != cMsg.m_vPort.end(); ++itIn)
            {
                nRet = pVSwitch->CheckDelBondMap(cMsg.m_strSwUuid.c_str(), (*itIn).c_str());
                if( VNET_DB_IS_ITEM_NO_EXIST(nRet))
                {
                    continue;
                }
                nRet = pVSwitch->DelBondMap(cMsg.m_strSwUuid.c_str(), (*itIn).c_str());
                if( nRet != VNET_DB_SUCCESS)
                {                
                    VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: pVSwitch->DelBondMap(%s,%s) failed. %d\n",
                    cMsg.m_strSwUuid.c_str(), (*itIn).c_str(), nRet);
                    return ERROR_WC_SWITCH_SET_DB_FAIL;
                }
            }
        }

        // 将switch表中port清空
        info.SetUplinkPortType(EN_DB_PORT_TYPE_PHYSICAL);
        info.SetUplinkPortName("");
        nRet = pVSwitch->Modify(info);
        if( nRet != VNET_DB_SUCCESS)
        {
            info.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: pVSwitch->Modify() failed. %d\n",
               nRet);
            return ERROR_WC_SWITCH_SET_DB_FAIL;
        }
    }
    else if( nSurplus == 1 )
    {
        // 先全部删除bondmap
        for(it = vOldPort.begin(); it != vOldPort.end(); ++it)
        {
            nRet = pVSwitch->DelBondMap(cMsg.m_strSwUuid.c_str(), (*it).c_str());
            if( nRet != VNET_DB_SUCCESS)
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: pVSwitch->DelBondMap(%s,%s) failed. %d\n",
                cMsg.m_strSwUuid.c_str(), (*it).c_str(), nRet);
                return ERROR_WC_SWITCH_SET_DB_FAIL;
            }
        }

        //找出多余的那个网口
        string strPortTmp("");
        for(it = vOldPort.begin(); it != vOldPort.end(); ++it)
        {
            bExist = false;
            for(itIn = cMsg.m_vPort.begin(); itIn != cMsg.m_vPort.end(); ++itIn)
            {
                if( 0 == STRCMP((*it).c_str(), (*itIn).c_str()))
                {
                    bExist = true;
                    break;
                }
            }
            if( bExist == false)
            {
                strPortTmp = (*it);
                break;
            }
        }

        // 将switch表中port更新
        info.SetUplinkPortType(EN_DB_PORT_TYPE_PHYSICAL);
        info.SetUplinkPortName(strPortTmp.c_str());
        nRet = pVSwitch->Modify(info);
        if( nRet != VNET_DB_SUCCESS)
        {
            info.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: pVSwitch->Modify() failed. %d\n",
               nRet);
            return ERROR_WC_SWITCH_SET_DB_FAIL;
        }
    }
    else if( nSurplus >= 2 ) 
    {
        for(itIn = cMsg.m_vPort.begin(); itIn != cMsg.m_vPort.end(); ++itIn)
        {
            nRet = pVSwitch->DelBondMap(cMsg.m_strSwUuid.c_str(), (*itIn).c_str());
            if( nRet != VNET_DB_SUCCESS)
            {
                info.DbgShow();
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::DelSwitchPort: pVSwitch->DelBondMap(%s,%s) failed. %d\n",
                cMsg.m_strSwUuid.c_str(), (*itIn).c_str(), nRet);
                return ERROR_WC_SWITCH_SET_DB_FAIL;
            }
        }
    }
    
    return WC_SWITCH_OPER_SUCCESS;
}

// task 
int32 CWildcastVSwitch::ProcSwitchTaskMsg(const string& strTaskUuid)
{
    // 目前仅仅只有del task msg
    CDBOperateWildcastTaskSwitch * pVSwitchTask =  GetDBVSwitchTask();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcSwitchTaskMsg: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 
    int32 nRet = pVSwitchTask->Del(strTaskUuid.c_str());
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcSwitchTaskMsg: pVSwitchTask->Del(%s) failed. %d\n",
            strTaskUuid.c_str(),nRet);
        return ERROR_WC_SWITCH_SET_DB_FAIL;
    }
    return WC_SWITCH_OPER_SUCCESS;
}

int32 CWildcastVSwitch::NewVnaRegist(const string &strVnaUuid)
{
    CDBOperateWildcastSwitch * pVSwitch =  GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    CDBOperateWildcastTaskSwitch * pVSwitchTask =  GetDBVSwitchTask();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    
    CPortMgr * pPortMgr = CPortMgr::GetInstance();
    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: CPortMgr::GetInstance() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }  

    vector<CDbWildcastSwitch> vWcSwitch;
    int32 nRet = pVSwitch->QuerySummary(vWcSwitch);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: pVSwitch->QuerySummary() failed. %d\n",nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    // 为获取SRIOV 网口是否是loop 
    CDBOperatePortPhysical* pDbPhy = GetDbIPortPhy();
    if( NULL == pDbPhy)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: GetDbIPortPhy() is NULL. \n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    vector<CDbWildcastSwitch>::iterator it = vWcSwitch.begin();
    for(; it != vWcSwitch.end(); ++it)
    {
        //是否已经存在此switch uuid 
        int32 isExistTask = false;
        if( WC_SWITCH_OPER_SUCCESS != IsExistSwitchTask(strVnaUuid.c_str(), (*it).GetUuid().c_str(),isExistTask) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: IsExistSwitchTask(%s,%s) failed.\n",
                strVnaUuid.c_str(), (*it).GetUuid().c_str() );
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        if( isExistTask )
        {
            continue;
        }
    
        int32 nPortNum = 0;
        vector<string> vPort; 
        if( WC_SWITCH_OPER_SUCCESS != GetSwitchPort(pVSwitch,(*it),nPortNum, vPort) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: GetSwitchPort(%s,%s) failed.\n",
                (*it).GetUuid().c_str(),(*it).GetSwitchName().c_str());
            continue;
        }

        if(nPortNum == 0 || vPort.size() <= 0 )
        {
            VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::NewVanRegist: WC switch(%s,%s) port is empty.\n",
              (*it).GetUuid().c_str(), (*it).GetSwitchName().c_str(),nRet);
            continue;
        }
        
        //获取该port name, port_type, vna uuid,---> port uuid          
        string portUuid;
        int32 portType = (*it).GetUplinkPortType();
        
        vector<string>::iterator itPort = vPort.begin();
        if( vPort.size() == 1 )
        {
            if( 0 != pPortMgr->GetPortUUID(strVnaUuid, (*itPort), portType,portUuid))
            {
                VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::NewVanRegist: WC switch(%s,%s) port(%s,%d) no exist.\n",
                 strVnaUuid.c_str(), (*it).GetSwitchName().c_str(),(*itPort).c_str(),portType );
                continue;
            }
        }
        else
        {        
            int32 allPortOk = true;
            // 多个网口的情况，只能是physical port
            for(; itPort != vPort.end(); ++itPort)
            {
                if( 0 != pPortMgr->GetPortUUID(strVnaUuid, (*itPort), EN_DB_PORT_TYPE_PHYSICAL,portUuid))
                {
                    allPortOk = false;
                    break;
                }

                // 如果有一个网口为sriov loop 也不能进行bond操作 
                CDbPortPhysical dbphyInfo;
                dbphyInfo.SetUuid(portUuid.c_str());
                if( 0 != pDbPhy->Query(dbphyInfo) )
                {
                    VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::NewVanRegist: pDbPhy->Query(%s) failed.\n",
                          portUuid.c_str() );
                    allPortOk = false;
                    break;
                }
                else
                {
                    // 只要上报或者配置为loop，则认为其为loop 
                    if( dbphyInfo.GetIsloopCfg() || dbphyInfo.GetIsloopRep() )
                    {
                        VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::NewVanRegist: pDbPhy->Query(%s) is loop.\n",
                          portUuid.c_str() );
                        allPortOk = false;
                        break;
                    }
                }
            }

            // 只要一个网口不满足，即忽略
            if( false == allPortOk)
            {
                VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::NewVanRegist: WC switch(%s,%s) ports not all ok.\n",
                  (*it).GetUuid().c_str(), (*it).GetSwitchName().c_str() );
                continue;
            }
        }
        
        //调用switch，判断是否可以创建task
        if( 0 != SwitchCheckIsConflict(strVnaUuid.c_str(), (*it), vPort) )
        {
            VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::NewVanRegist: IsConflictSwitch(%s,%s) failed.\n",
              (*it).GetUuid().c_str(), (*it).GetSwitchName().c_str());
        }
        else
        {
            // 更新task 表 
            CDbWildcastTaskSwitch wcTask;
            wcTask.SetVnaUuid(strVnaUuid.c_str());
            wcTask.SetSwitchUuid((*it).GetUuid().c_str());
            wcTask.SetState(WILDCAST_TASK_STATE_INIT);
            wcTask.SetSendNum(0);
            nRet = pVSwitchTask->Add(wcTask);
            if( VNET_DB_SUCCESS != nRet )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::NewVanRegist: pVSwitchTask->Add failed. %d\n",nRet);
                continue;
            }
        }        
    }
    return WC_SWITCH_OPER_SUCCESS;
}

// 内部封装switch 冲突检查函数 
int32 CWildcastVSwitch::SwitchCheckIsConflict(const char* vnaUuid, CDbWildcastSwitch & info,
          vector<string> & vPort)
{
    CSwitchMgr * pSwitchMgr = CSwitchMgr::GetInstance();
    if( NULL == pSwitchMgr )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchIsConflict: CSwitchMgr::GetInstance() is null.\n");
        return -1;
    }

    CSwitchCfgMsgToVNA msg;
    msg.m_strName = info.GetSwitchName();
    msg.m_nType = info.GetSwitchType();
    msg.m_nMTU = info.GetMtuMax();
    msg.m_nEvbMode = info.GetEvbMode();
    msg.m_nMaxVnic = info.GetNicMaxNum();
    msg.m_strPGUuid = info.GetUplinkPgUuid();
    msg.m_vecPort = vPort;
    msg.m_nBondMode = info.GetBondMode();
    msg.m_strVnaUuid = vnaUuid;
    // msg.m_strSwIf = portType;
    // msg.m_nOper = 
    // msg.m_strWCTaskUuid = info.
   
    //调用
    return pSwitchMgr->CheckWildCast(msg);
}

int32 CWildcastVSwitch::IsExistSwitchTask(const char* vnaUuid, const char* swUuid, int32 &isExist)
{   
    CDBOperateWildcastTaskSwitch * pVSwitchTask =  GetDBVSwitchTask();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsExistSwitchTask: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    if(NULL == swUuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsExistSwitchTask: swUuid is NULL.\n");
        return ERROR_WC_SWITCH_PARAM_INVALID;
    }

    vector<CDbWildcastTaskSwitchDetail>  outVInfo;
    int32 nRet = pVSwitchTask->QuerySummary(outVInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsExistSwitchTask: QuerySummary() failed.%d\n",nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskSwitchDetail>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if( (0 == STRCMP((*it).GetSwitchUuid().c_str(), swUuid) )&& \
            (0 == STRCMP((*it).GetVnaUuid().c_str(), vnaUuid) ))
        {
            isExist = true;
            break;
        }
    }

    return WC_SWITCH_OPER_SUCCESS;
}

// 定时处理task任务 
int32 CWildcastVSwitch::ProcTimeout()
{   
    CDBOperateWildcastTaskSwitch * pVSwitchTask =  GetDBVSwitchTask();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcTimeout: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    vector<CDbWildcastTaskSwitchDetail>  outVInfo;
    int32 nRet = pVSwitchTask->QuerySummary(outVInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcTimeout: QuerySummary() failed.%d\n",nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskSwitchDetail>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if( (*it).GetTaskSendNum() <= WILDCAST_TASK_SEND_NUM_MAX )
        {           
            //调用switch模块接口            
           nRet = SendMsgToSwitch((*it).GetUuid().c_str());
            if(WC_SWITCH_OPER_SUCCESS != nRet  )
            {
                VNET_LOG(VNET_LOG_INFO, "CWildcastVSwitch::ProcTimeout: SendMsgToSwitch(%s) failed.\n",
                 (*it).GetUuid().c_str());
            }

            // 是否更新DB
            CDbWildcastTaskSwitch wctaskInfo;
            wctaskInfo.SetSendNum((*it).GetTaskSendNum() + 1);
            if(WC_SWITCH_OPER_SUCCESS == nRet  )
            {
                wctaskInfo.SetState(WILDCAST_TASK_STATE_WAIT_ACK);
            }
            else
            {
                wctaskInfo.SetState(WILDCAST_TASK_STATE_SEND_MSG_FAILED);
            }

            wctaskInfo.SetSwitchUuid((*it).GetSwitchUuid().c_str());
            wctaskInfo.SetUuid((*it).GetUuid().c_str());
            wctaskInfo.SetVnaUuid((*it).GetVnaUuid().c_str());
            
            nRet = pVSwitchTask->Modify(wctaskInfo);
            if( VNET_DB_SUCCESS != nRet )
            {
                VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::ProcTimeout: pVSwitchTask->Modify(%s) failed.%d\n",
                    (*it).GetUuid().c_str(),nRet);
                continue;
            }
        }

    }
    return WC_SWITCH_OPER_SUCCESS;
}

int32 CWildcastVSwitch::SendMsgToSwitch(const char* TaskUuid)
{
    CDBOperateWildcastTaskSwitch * pTask =  GetDBVSwitchTask();
    if(NULL == pTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    CDBOperateWildcastSwitch * pVSwitch =  GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    if(NULL == TaskUuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: TaskUuid is NULL.\n");
        return ERROR_WC_SWITCH_PARAM_INVALID;
    }   

    // 获取task信息
    CDbWildcastTaskSwitchDetail wcswTaskInfo;
    wcswTaskInfo.SetUuid(TaskUuid);
    int32 nRet = pTask->Query(wcswTaskInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: pTask->Query(%s) failed.%d\n",
          TaskUuid, nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    // 获取switch信息 
    CDbWildcastSwitch wcswInfo;
    wcswInfo.SetUuid(wcswTaskInfo.GetSwitchUuid().c_str());
    nRet = pVSwitch->Query(wcswInfo);
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: pVSwitch->Query(%s) failed.%d\n",
          wcswTaskInfo.GetSwitchUuid().c_str(), nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    // 获取所有switch的port
    int32 nPortNum = 0;
    vector<string> vPort; 
    if( WC_SWITCH_OPER_SUCCESS != GetSwitchPort(pVSwitch,wcswInfo,nPortNum, vPort) )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: GetSwitchPort() failed.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    if(vPort.size() <= 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: switch(%s) port not exist.\n",
            wcswTaskInfo.GetSwitchUuid().c_str() );        
        return ERROR_WC_SWITCH_NOTEXIST_PORT;
    }   


    // 判断物理/bond端口是否是混用switch
    int32 isEdvsSdvsMix = false;
    string strMixBr = "";
    vector<string>::iterator itPort = vPort.begin();
    string stmpPort("");
    for(; itPort != vPort.end(); ++itPort)
    {
        stmpPort = (*itPort);
        vector<CDbWildcastSwitchBondMap> vInfo;
        int ret = pVSwitch->QueryBondMapByPhyPortName(stmpPort.c_str(),vInfo);
        if( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: QueryBondMapByPhyPortName(%s) failed.\n",
                stmpPort.c_str());
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        vector<CDbWildcastSwitchBondMap>::iterator itInfo = vInfo.begin();
        for(; itInfo != vInfo.end(); ++itInfo)
        {
            if( wcswInfo.GetSwitchType() != (*itInfo).GetSwitchType() )
            {
                isEdvsSdvsMix = true;
                strMixBr = (*itInfo).GetSwitchName();
                break;
            }
        }

        if( isEdvsSdvsMix )
        {
            break;
        }
    }

    // 调用switch 接口 

    CSwitchMgr * pSwitchMgr = CSwitchMgr::GetInstance();
    if( NULL == pSwitchMgr )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SendMsgToSwitch: CSwitchMgr::GetInstance() is null.\n");
        return ERROR_WC_SWITCH_INSTANCE_FAIL;
    }

    CSwitchCfgMsgToVNA msg;
    msg.m_strName = wcswInfo.GetSwitchName();
    msg.m_nType = wcswInfo.GetSwitchType();
    msg.m_nMTU = wcswInfo.GetMtuMax();
    msg.m_nEvbMode = wcswInfo.GetEvbMode();
    msg.m_nMaxVnic = wcswInfo.GetNicMaxNum();
    msg.m_strPGUuid = wcswInfo.GetUplinkPgUuid();
    msg.m_vecPort = vPort;
    msg.m_nBondMode = wcswInfo.GetBondMode();
    msg.m_strVnaUuid = wcswTaskInfo.GetVnaUuid();
    // msg.m_strSwIf = portType;
    // msg.m_nOper = 
    msg.m_strWCTaskUuid = wcswTaskInfo.GetUuid();

    if(isEdvsSdvsMix)
    {
        msg.m_nIsMixed = 1; // mixed;
        msg.m_strMixedBridge = strMixBr;
    }

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastswitch module create switch send msg(%s) to switch.\n",msg.m_strName.c_str() );
    msg.Print();

    nRet = pSwitchMgr->DoSwitchWildcast(msg);
    if( 0 != nRet)
    {
        msg.Print();
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchIsConflict: pSwitchMgr->DoSwitchWildcast() failed. %d\n",nRet);
        return nRet;
    }

    VNET_LOG(VNET_LOG_INFO,"VNet wildcastswitch module create switch send msg(%s) success.\n",msg.m_strName.c_str() );
    
    return WC_SWITCH_OPER_SUCCESS;
}

int32 CWildcastVSwitch::WildcastTaskMsgAck(const string & strTaskUuid, int32 nResult)
{
    CDBOperateWildcastTaskSwitch * pVSwitch =  GetDBVSwitchTask();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchMsgAck: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 

    CDbWildcastTaskSwitchDetail info;
    info.SetUuid(strTaskUuid.c_str());
    int32 nRet = pVSwitch->Query(info);

    // 没有找到，说明没有task，可能前面已经处理成功了。
    if( VNET_DB_IS_ITEM_NO_EXIST(nRet))
    {
        if(DbgGetDBPrint())
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchMsgAck: pVSwitch->Query(%s) failed.%d\n",
              info.GetUuid().c_str(), nRet);
        }
        return WC_SWITCH_OPER_SUCCESS;
    }
    
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchMsgAck: pVSwitch->Query(%s) failed.%d\n",
          info.GetUuid().c_str(), nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    if( 0 == nResult )
    {
        // 成功后，可以将此记录删除
        nRet = pVSwitch->Del(strTaskUuid.c_str());
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchMsgAck: pVSwitch->Del(%s) failed.%d\n",
              strTaskUuid.c_str(), nRet);
            return ERROR_WC_SWITCH_SET_DB_FAIL;
        }
    }
    else
    {
        // 是否更新DB
        CDbWildcastTaskSwitch wctaskInfo;
        wctaskInfo.SetSendNum(info.GetTaskSendNum());
        wctaskInfo.SetState(WILDCAST_TASK_STATE_ACK_FAILED);
        wctaskInfo.SetSwitchUuid(info.GetSwitchUuid().c_str());
        wctaskInfo.SetUuid(info.GetUuid().c_str());
        wctaskInfo.SetVnaUuid(info.GetVnaUuid().c_str());
        
        nRet = pVSwitch->Modify(wctaskInfo);
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::SwitchMsgAck: pVSwitch->Modify(%s) failed.%d\n",
              strTaskUuid.c_str(), nRet);
            return ERROR_WC_SWITCH_SET_DB_FAIL;
        }
    }
    return WC_SWITCH_OPER_SUCCESS;
}

// 供switch module 调用 
int32 CWildcastVSwitch::IsConflictSwitchName(const string & swName, int32 & outResult)
{
    outResult = false;
    CDBOperateWildcastTaskSwitch * pVSwitchTask =  GetDBVSwitchTask();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchName: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    vector<CDbWildcastTaskSwitchDetail>  outVInfo;
    int32 nRet = pVSwitchTask->QuerySummary(outVInfo );
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchName: pVSwitch->QuerySummary() failed.%d\n",
           nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskSwitchDetail>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if(0 == STRCMP((*it).GetSwitchName().c_str(), swName.c_str()))
        {
            outResult = true;
            return WC_SWITCH_OPER_SUCCESS;
        }
    }

    return WC_SWITCH_OPER_SUCCESS;
}

int32 CWildcastVSwitch::IsConflictSwitchPort(const string & vnaUuid, const string & swName, int32 swType, 
            vector<string> & vPortName, int32 & outResult)
{

    outResult = false;
    CDBOperateWildcastTaskSwitch * pVSwitchTask =  GetDBVSwitchTask();
    if(NULL == pVSwitchTask)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchPort: GetDBVSwitchTask() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }   

    CDBOperateWildcastSwitch * pVSwitch =  GetDBVSwitch();
    if(NULL == pVSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchPort: GetDBVSwitch() is NULL.\n");
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    } 
    
    vector<CDbWildcastTaskSwitchDetail>  outVInfo;
    int32 nRet = pVSwitchTask->QuerySummaryByVna(vnaUuid.c_str(),outVInfo );
    if( VNET_DB_SUCCESS != nRet )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchPort: pVSwitch->QuerySummaryByVna(%s) failed.%d\n",
          vnaUuid.c_str(), nRet);
        return ERROR_WC_SWITCH_GET_DB_FAIL;
    }

    vector<CDbWildcastTaskSwitchDetail>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        if(0 == STRCMP((*it).GetSwitchName().c_str(), swName.c_str()))
        {
            outResult = true;
            return WC_SWITCH_OPER_SUCCESS;
        }

        CDbWildcastSwitch wcswInfo;
        wcswInfo.SetUuid((*it).GetSwitchUuid().c_str());
        int32 nRet = pVSwitch->Query(wcswInfo);
        if( VNET_DB_SUCCESS != nRet )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchPort: pVSwitch->Query(%s) failed.%d\n",
              (*it).GetSwitchUuid().c_str(), nRet);
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }
    
        int32 nPortNum = 0;
        vector<string> vDbPort; 
        if( WC_SWITCH_OPER_SUCCESS != GetSwitchPort(pVSwitch,wcswInfo,nPortNum, vDbPort) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CWildcastVSwitch::IsConflictSwitchPort: GetSwitchPort() failed.\n");
            return ERROR_WC_SWITCH_GET_DB_FAIL;
        }

        // sdvs edvs能够混用 
        if( swType ==  (*it).GetSwitchType() )
        {
            vector<string>::iterator itDbPort = vDbPort.begin();
            for(; itDbPort != vDbPort.end(); ++itDbPort)
            {
                vector<string>::iterator itInPort = vPortName.begin();
                for(; itInPort != vPortName.end(); ++itInPort)
                {
                    if( 0 == STRCMP((*itDbPort).c_str(), (*itInPort).c_str()))
                    {
                        outResult = true;
                        return WC_SWITCH_OPER_SUCCESS;
                    }
                }
            }
        }        
    }    
    
    return WC_SWITCH_OPER_SUCCESS;
}

void CWildcastVSwitch::DbgShow()
{
    
}

// test code 
void wc_switch(char* uuid, char * wcSwName,char * pgUuid,int32 wcSwType, int32 mtu, int32 oper)
{
    CWildcastSwitchCfgMsg cMsg;
    CWildcastVSwitch vswitch;

    cMsg.m_strUuid = uuid;
    cMsg.m_strName = wcSwName;
    cMsg.m_strPGUuid = pgUuid;
    cMsg.m_nType = wcSwType;
    cMsg.m_nOper = oper;

    cMsg.m_nEvbMode = EN_EVB_MODE_VEPA;
    cMsg.m_nMaxVnic = 32;
    cMsg.m_nMTU = mtu;
        
    int32 nRet = WC_SWITCH_OPER_SUCCESS;

    cMsg.Print();
    if( EN_ADD_SWITCH == oper)
    {
        nRet = vswitch.AddSwitch(cMsg) ;
        if( WC_SWITCH_OPER_SUCCESS == nRet)
        {    
            cout << "AddSwitch success" << endl;
        }
        else
        {
            cout << "AddSwitch failed."<< nRet << endl;
        }
    }

    if( EN_MODIFY_SWITCH == oper)
    {
        nRet = vswitch.ModifySwitch(cMsg) ;
        if( WC_SWITCH_OPER_SUCCESS == nRet)
        {    
            cout << "ModifySwitch success" << endl;
        }
        else
        {
            cout << "ModifySwitch failed."<< nRet << endl;
        }
    }

    if( EN_DEL_SWITCH == oper)
    {
        nRet = vswitch.DelSwitch(cMsg.m_strUuid) ;
        if( WC_SWITCH_OPER_SUCCESS == nRet)
        {    
            cout << "DelSwitch success" << endl;
        }
        else
        {
            cout << "DelSwitch failed."<< nRet << endl;
        }
    }   
}
DEFINE_DEBUG_FUNC(wc_switch);

void wc_switch_port( char * wcSwUuid,char * portName, int32 oper)
{
    CWildcastSwitchPortCfgMsg cMsg;
    CWildcastVSwitch vswitch;

    cMsg.m_strSwUuid = wcSwUuid;
    cMsg.m_vPort.push_back(portName);
    cMsg.m_nOper = oper;
        
    int32 nRet = WC_SWITCH_OPER_SUCCESS;

    cMsg.Print();
    if( EN_ADD_SWITCH_PORT == oper)
    {
        nRet = vswitch.AddSwitchPort(cMsg) ;
        if( WC_SWITCH_OPER_SUCCESS == nRet)
        {    
            cout << "AddSwitchPort success" << endl;
            cMsg.Print();
        }
        else
        {
            cout << "AddSwitchPort failed."<< nRet << endl;
        }
    }

    if( EN_DEL_SWITCH_PORT == oper)
    {
        nRet = vswitch.DelSwitchPort(cMsg) ;
        if( WC_SWITCH_OPER_SUCCESS == nRet)
        {    
            cout << "DelSwitchPort success" << endl;
            cMsg.Print();
        }
        else
        {
            cout << "DelSwitchPort failed."<< nRet << endl;
        }
    }   
}
DEFINE_DEBUG_FUNC(wc_switch_port);

void wc_switch_newvna(char * uuid)
{
    CWildcastVSwitch vswitch;
    string vnauuid(uuid);
    int32 nRet = vswitch.NewVnaRegist(vnauuid);
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_switch_newvna failed. " << nRet << endl;        
    }
    else
    {
        cout << "wc_switch_newvna success" << endl;
    }
}DEFINE_DEBUG_FUNC(wc_switch_newvna);

void wc_switch_timeout()
{
    CWildcastVSwitch vswitch;
    int32 nRet = vswitch.ProcTimeout();
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_switch_timeout failed. " << nRet << endl;
    }
    else
    {
        cout << "wc_switch_timeout success" << endl;
    }
     
}DEFINE_DEBUG_FUNC(wc_switch_timeout);

void wc_switch_isconflict(char* vnaUuid, char * swName, int32 swType, char* portName)
{
    CWildcastVSwitch vswitch;
    string strVnaUuid(vnaUuid);
    string strSwName(swName);
    
    vector<string> vPort;
    vPort.push_back(portName);
    int32 outResult;
            
    int32 nRet = vswitch.IsConflictSwitchPort(strVnaUuid,strSwName,swType, vPort,outResult);
    if(WC_SWITCH_OPER_SUCCESS != nRet )
    {
        cout << "wc_switch_isconflict failed. " << nRet << endl;
    }
    else
    {
        cout << "wc_switch_isconflict success " << outResult << endl;
    }
     
}DEFINE_DEBUG_FUNC(wc_switch_isconflict);


} // namespace 

