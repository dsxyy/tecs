/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ssh_trust.h
* 文件标识：
* 内容摘要：ssh 认证进程的消息处理函数头文件
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年7月26日
*
* 修改记录1：
*     修改日期：2012年7月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#ifndef SSH_TRUST_H
#define SSH_TRUST_H
#include "sky.h"
#include "event.h"
#include "mid.h"
#include "log_agent.h"

namespace zte_tecs 
{

class SShTrust : public MessageHandler
{
public:
    static SShTrust* GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new SShTrust(); 
        }
        return _instance;
    };

    STATUS Init()
    {
        return StartMu(MU_SSH_TRUST);
    }
    
    virtual ~SShTrust() 
    {
        if (NULL != _mu)
        {
            delete _mu;
        }
        _mu = NULL; 
    }
    virtual void MessageEntry(const MessageFrame &message);
    void MultiCastMyKey();    
    STATUS JoinFriendKey(const string &key);
    void SetMultiCastTimer();
    void JoinGroup(const string &group);

private:
    SShTrust()
    {
        _mu = NULL;
    }
    STATUS StartMu(const string& name);
    static SShTrust* _instance;
    MessageUnit     *_mu;
    string          _my_key;
    vector<string>  _all_keys;
    void DoStartUp();

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(SShTrust);   
};

} // namespace zte_tecs
#endif // #ifndef SSH_TRUST_H


