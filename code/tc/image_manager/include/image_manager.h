/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：ImageManager类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
* 修改记录2：
*    修改日期：2013/3/26
*    版 本 号：V2.0
*    修 改 人：颜伟
*    修改内容：整改工作流and 架构

*******************************************************************************/
#ifndef IMAGE_MANGER_IMAGE_MANAGER_H        
#define IMAGE_MANGER_IMAGE_MANAGER_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "user_pool.h"
#include "vm_messages.h" 
#include "image_manager_with_api_method.h"
#include "image_db.h"
#include "image_pool.h"

namespace zte_tecs
{

/**
@brief 功能描述: 映像资源池的实体类\n
@li @b 其它说明: 无
*/
class ImageManager: public MessageHandler
{
public:
    static ImageManager* GetInstance()
        {
        if(NULL == instance)
        {
            instance = new ImageManager(); 
        }

        return instance;
    };

    ~ImageManager( )
    {
        if (NULL != _ipool)
        {
            delete _ipool;
        }

        if (NULL != _mu)
        {
            delete _mu;
        } 
    }

    STATUS Init()
    {
        // 创建 ImagePool对象
        _ipool = ImagePool::GetInstance();

        //启动消息单元工作线程
        return StartMu(MU_IMAGE_MANAGER);
    }

    bool Authorize(int64 opt_uid, Image *buf, int64 self_id, int oprate);

private:
    ImageManager();
    static ImageManager *instance;
    STATUS StartMu(const string& name);
    virtual void MessageEntry(const MessageFrame &message);
    STATUS DealRegister(const ImageRegisterMsg  &request);
    STATUS DealDeregister(const ImageDeregisterMsg  &request);
    STATUS DealDelete(const ImageDeleteMsg &request);
    STATUS DealEnable(const ImageEnableMsg &request);
    STATUS DealPublish(const ImagePublishMsg &request);
    STATUS DealModify(const ImageModifyMsg  &request);
    STATUS DealQuery(const ImageQueryMsg  &request);
    STATUS DealMetadataReq(const ImageMetadataReq& req);
    STATUS DealUploadReq(const ImageMetadataReq& req);
    STATUS DealFtpSvrInfoReq(const ImageFtpSvrInfoGetMsg& req);
    STATUS CheckTableAndWorkflow(const string& base_uuid,
                                       const string& cluster_name,
                                       bool &table,
                                       bool &forward,
                                       bool &rollback,
                                       bool &reverse,
                                       int &progress,
                                       string &workflow_id,
                                       string &rollback_id,
                                       ostringstream &oss);
    STATUS CheckTableAndWorkflow(int64 image_id,
                                       const string& cluster_name,
                                       int &table,
                                       int &forward,
                                       int &rollback,
                                       int &reverse,
                                       int &progress,
                                       ostringstream &oss);
    STATUS CreateDeployWorkflow(const MID &sender, const ImageDeployMsg &req, ostringstream &oss, string &workflow_id);
    STATUS CreateCancelWorkflow(const MID &sender, const ImageCancelMsg &req, ostringstream &oss, string &workflow_id);
    bool IsSenderSelf(const MID& sender);
    STATUS DealDeploy(const ImageDeployMsg  &request);
    STATUS DealCancel(const ImageCancelMsg  &request);
    STATUS DealPrepare(const ImagePrepareMsg  &request);
    STATUS DealDeployAck(const DeployBaseImageAck &ack);
    STATUS DealCancelAck(const CancelBaseImageAck &ack);
    STATUS GetSpace(const ImageGetSpaceMsg &request);
    STATUS AddImage(Image& image);
    STATUS RemoveImage(int64 iid);
    int64  GetImageIdByLocation(const string& location);
    STATUS GetImageRefCount(int64 iid);
    MessageUnit *_mu;
    ImagePool   *_ipool;
    DISALLOW_COPY_AND_ASSIGN(ImageManager); 
};

}
#endif /* IMAGE_MANGER_IMAGE_MANAGER_H */

