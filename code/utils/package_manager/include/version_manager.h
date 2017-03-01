/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_manager.h
* 文件标识：
* 内容摘要：版本管理进程
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年9月26日
*
* 修改记录1：
*     修改日期：2012年9月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/

#ifndef HM_VERSION_MANAGER_H
#define HM_VERSION_MANAGER_H
#include "sky.h"
#include "tecs_errcode.h"
#include "msg_version_with_api_method.h"
#include "version_task.h"


namespace zte_tecs
{
class VersionManager : public MessageHandler,Callbackable
{
public:
    static VersionManager* GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new VersionManager();
        }

        return _instance;
    }
    
    VersionManager()
    {
        _mu = NULL;
        _qtpool = NULL;
        _utpool = NULL;
    }

    virtual ~VersionManager()
    {
         _qtpool->DestroyPool();
         _utpool->DestroyPool();
    };  
    virtual void MessageEntry(const MessageFrame &message); 

    int InitCallback(void *nil, SqlColumn *pColumns);

    STATUS Init();
    void DoSetRepoAddrReq(const MessageFrame &message);
    void DoGetRepoAddrReq(const MessageFrame &message);
    void DoUpgradeReq(const MessageFrame &message);
    void DoQueryReq(const MessageFrame &message);
    void DoCancelReq(const MessageFrame &message);
    void DoUpgradeAck(STATUS result,int64 taskid,const MID&receiver);
    void DoQueryAck(STATUS result,const MID&receiver);
    void DoCancelAck(STATUS result,const MID&receiver);
private:
    static VersionManager *_instance;
    STATUS StartMu(const string& name);
    void DoStartUp();    
    string      _repo_addr;
    MessageUnit        *_mu;
    ThreadPool  *_qtpool;
    ThreadPool  *_utpool;
    
    DISALLOW_COPY_AND_ASSIGN(VersionManager);
};
} // namespace zte_tecs
#endif // #ifndef HM_VERSION_MANAGER_H


