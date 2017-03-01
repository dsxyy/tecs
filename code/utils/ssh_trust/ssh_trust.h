/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ssh_trust.h
* �ļ���ʶ��
* ����ժҪ��ssh ��֤���̵���Ϣ������ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��7��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��7��26��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
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

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(SShTrust);   
};

} // namespace zte_tecs
#endif // #ifndef SSH_TRUST_H


