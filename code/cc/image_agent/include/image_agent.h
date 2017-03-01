/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_agent.h
* 文件标识：
* 内容摘要：image代理进程的头文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年3月12日
*
* 修改记录1：
*     修改日期：2012/3/12
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
* 修改记录2：
*     修改日期：2013/3/12
*     版 本 号：V2.0
*     修 改 人：颜伟
*     修改内容：整改工作流and 架构

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

