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
#ifndef WILDCAST_LOOPBACK_H
#define WILDCAST_LOOPBACK_H
#include "vnet_comm.h"
#include "vnetlib_msg.h"
#include "vnet_msg.h"
#include "vnet_db_wildcast_loopback_port.h"
#include "vnet_db_wildcast_task_loopback.h"
#include "vnetlib_msg.h"
namespace zte_tecs
{


class CWildcastLoopback
{

public:
    CWildcastLoopback(){};
    virtual ~CWildcastLoopback(){};
    
public:
    STATUS ProcMsg(CWildcastLoopbackCfgMsg& message);
    STATUS Add(CWildcastLoopbackCfgMsg& cMsg);
    STATUS Del(const string &strUuid);
    //STATUS Modify(const CWildcastLoopbackCfgMsg& cMsg);

    //loopback task 
    STATUS ProcTaskMsg(const string& strTaskUuid);
    int32 NewVnaRegist(const string & strVnaUuid);
    int32 ProcTimeout();
    int32 WildcastTaskMsgAck(const string & strTaskUuid, int32 nResult);
    
    // ���˿�ģ�����
    int32 IsConflict(const string & vnaUuid, const string & portName, int32 & outResult);
    
    void DbgShow();
private:
    int32 IsExistTask(const char* vnaUuid,const char* swUuid, int32 &isExist);           
    int32 SendMsgToPort(CDbWildcastTaskLoopback & info);
    
private:
    // �ڲ�ʹ��
    int32 LoopbackIsConflict(const char* vnaUuid, const char* portName, int32 isLoop, int32 & outResult);

    CDBOperateWildcastLoopbackPort* GetDBLoopback();
    CDBOperateWildcastTaskLoopback * GetDBLoopbackTask();
       
private:
    DISALLOW_COPY_AND_ASSIGN(CWildcastLoopback);
};


}
#endif

