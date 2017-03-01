/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_switch.h
* �ļ���ʶ�� 
* ����ժҪ��vnm switch��
* ��ǰ�汾��1.0
* ��    �ߣ� 
* ������ڣ� 
*******************************************************************************/
#ifndef WILDCAST_CREATE_VPORT_H
#define WILDCAST_CREATE_VPORT_H
#include "vnet_comm.h"
#include "vnetlib_msg.h"
#include "vnet_msg.h"
#include "vnet_db_wildcast_create_vport.h"
#include "vnet_db_wildcast_task_create_vport.h"
#include "vnetlib_msg.h"
namespace zte_tecs
{


class CWildcastCreateVPort
{

public:
    CWildcastCreateVPort(){};
    virtual ~CWildcastCreateVPort(){};
    
public:
    STATUS ProcMsg(CWildcastCreateVPortCfgMsg& message);
    STATUS Add(CWildcastCreateVPortCfgMsg& cMsg);
    STATUS Del(const string &strUuid);
    //STATUS Modify(const CWildcastCreateVPortCfgMsg& cMsg);

    // switch task 
    STATUS ProcTaskMsg(const string& strTaskUuid);
    int32 NewVnaRegist(const string & strVnaUuid);
    int32 ProcTimeout();
    int32 WildcastTaskMsgAck(const string & strTaskUuid, int32 nResult);
    
    // ���˿�ģ�����
    int32 IsConflict(const string & vnaUuid, const string & portName, int32 & outResult);
    
    void DbgShow();
private:
    int32 IsExistTask(const char* vnaUuid, const char* swUuid, int32 &isExist);           
    int32 SendMsgToPort(CDbWildcastTaskCreateVPort & info);
    
    int32 CheckSwitchUplinkPortIsLoop(string & vnaUuid, string & portName, int32 &outResult);
    int32 CheckUplinkPgKernelPgMtu(string & switchUuid, string &vmpgUuid);
private:
    int32 GetSwitchUplinkPortName(const string & vnaUuid,
        const string & switchUuid,string & sSwitchName, string & swUplinkPortName);
    // �ڲ�ʹ��
    int32 PortNameIsConflict(const char* vnaUuid, const char* portName, int32 & outResult);

    int32 SendMsgToPortKernel(CDbWildcastTaskCreateVPort & info);
    int32 SendMsgToPortUplinkLoop(CDbWildcastTaskCreateVPort & info);

    CDBOperateWildcastCreateVPort* GetDBCreateVPort();
    CDBOperateWildcastTaskCreateVPort * GetDBCreateVPortTask();
       
private:
    DISALLOW_COPY_AND_ASSIGN(CWildcastCreateVPort);
};


}
#endif

