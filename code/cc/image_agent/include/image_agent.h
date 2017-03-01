/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_agent.h
* �ļ���ʶ��
* ����ժҪ��image������̵�ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
* �޸ļ�¼2��
*     �޸����ڣ�2013/3/12
*     �� �� �ţ�V2.0
*     �� �� �ˣ���ΰ
*     �޸����ݣ����Ĺ�����and �ܹ�

******************************************************************************/
#ifndef TECS_IMAGE_AGENT_H
#define TECS_IMAGE_AGENT_H
#include "sky.h"
#include "image_url.h"

namespace zte_tecs
{
class ImageAgent:public MessageHandlerTpl<MessageUnit>
{
public:
    static ImageAgent* GetInstance()
    {
        if(NULL == instance)
        {
            instance = new ImageAgent(); 
        }
   
        return instance;
    };
    
    ~ImageAgent(){};
    STATUS Init();
    virtual void MessageEntry(const MessageFrame&);
    
private:
    ImageAgent(){};
    DISALLOW_COPY_AND_ASSIGN(ImageAgent);
    void CalcBlockSize(int64 origin_size, const ImageStoreOption& option, int64& size);
    STATUS CheckTableAndWorkflow(const string &base_uuid,
                                       bool &table,
                                       bool &forward,
                                       bool &rollback,
                                       bool &reverse,
                                       int &progress,
                                       string &workflow_id,
                                       string &rollback_id,
                                       ostringstream &oss);
    STATUS CreateDeployWorkflow(const MID &sender, const DeployBaseImageReq &req, ostringstream &oss, string &workflow_id);
    STATUS CreateCancelWorkflow(const MID &sender, const CancelBaseImageReq &req,ostringstream &oss, string &workflow_id);
    STATUS HandleDeployReq(const MessageFrame &message);
    STATUS HandleCancelReq(const MessageFrame &message);
    STATUS HandleCreateBlockAck(const MessageFrame &message);
    STATUS HandleDeleteBlockAck(const MessageFrame &message);
    STATUS HandleAuthBlockAck(const MessageFrame &message);
    STATUS HandleDeAuthBlockAck(const MessageFrame &message);
    STATUS HandleImageDownloadAck(const MessageFrame &message);
    STATUS HandleCancelImageDownloadAck(const MessageFrame &message);
    STATUS HandleImageUrlPrepareReq(const MessageFrame &message);
    static ImageAgent *instance;
};

STATUS EnableImageAgentOptions();
}
#endif

