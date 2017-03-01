/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_switch.h
* 文件标识： 
* 内容摘要：vnm switch类
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#ifndef WILDCAST_SWITCH_H
#define WILDCAST_SWITCH_H
#include "vnet_comm.h"
#include "vnetlib_msg.h"
#include "vnet_msg.h"
#include "vnet_db_wildcast_switch.h"
#include "vnet_db_wildcast_task_switch.h"
namespace zte_tecs
{


class CWildcastVSwitch
{

public:
    CWildcastVSwitch(){};
    virtual ~CWildcastVSwitch(){};
    
public:
    STATUS ProcSwitchMsg(CWildcastSwitchCfgMsg& message);
    STATUS AddSwitch(CWildcastSwitchCfgMsg& cMsg);
    STATUS DelSwitch(const string &strUuid);
    STATUS ModifySwitch(const CWildcastSwitchCfgMsg& cMsg);

    STATUS ProcSwitchPortMsg(CWildcastSwitchPortCfgMsg& message);
    STATUS AddSwitchPort( CWildcastSwitchPortCfgMsg &cMsg);
    STATUS DelSwitchPort( CWildcastSwitchPortCfgMsg &cMsg);

    // switch task 
    int32 ProcSwitchTaskMsg(const string& strTaskUuid);
    int32 NewVnaRegist(const string & strVnaUuid);
    int32 ProcTimeout();
    int32 WildcastTaskMsgAck(const string & strTaskUuid, int32 nResult);

    int32 IsConflictSwitchName(const string & swName, int32 & outResult);
    int32 IsConflictSwitchPort(const string & vnaUuid, const string & swName, int32 swType, 
            vector<string> & vPortName, int32 & outResult);
    
    // 添加switch port时 判断 
    int32 CheckAddSwitchPort(CDbWildcastSwitch & info, 
        vector<string> & vOldPort, vector<string> & vAddPort);
    int32 CheckDelSwitchPort(CDbWildcastSwitch & info, 
       vector<string> & vOldPort, vector<string> & vDelPort);
    
    void DbgShow();
private:
    int32 CheckAddSwitchPort_oldPNotZero(CDbWildcastSwitch & info, 
        vector<string> & vOldPort,vector<string> & vAddPort);
    int32 IsVectorEqua(vector<string> new_vec, vector<string> old_vec);
    int32 CheckAddSwitchPort_OldPZeroAddPMany(CDbWildcastSwitch & info, 
        vector<string> & vAddPort);
    int32 CheckAddSwitchPort_OldPZeroAddPOne(CDbWildcastSwitch & info, 
        string & addPort);    
    
private:
    STATUS GetSwitchPort(CDBOperateWildcastSwitch * pVSwitch,CDbWildcastSwitch & info,
        int32 & nOldPortNum, vector<string> &vOldPort);
    STATUS AddSwitchPortWriteDb(CDbWildcastSwitch & info, CWildcastSwitchPortCfgMsg &cMsg, int32 nOldPortNum, vector<string> & vOldPort,
        vector<string> & vAddPort, CDBOperateWildcastSwitch * pVSwitch);

    int32 SwitchCheckIsConflict(const char* vnaUuid, CDbWildcastSwitch & info, vector<string> & vPortUuid);
    int32 IsExistSwitchTask(const char* vnaUuid,const char* swUuid, int32 &isExist); 
    int32 SendMsgToSwitch(const char* swUuid);

    int32 CheckSwitchPortIsExistSame(vector<string> & vOldPort, vector<string> & vAddPort);
    
private:
    CDBOperateWildcastSwitch * GetDBVSwitch();
    CDBOperateWildcastTaskSwitch * GetDBVSwitchTask();
       
private:
    DISALLOW_COPY_AND_ASSIGN(CWildcastVSwitch);
};


}
#endif

