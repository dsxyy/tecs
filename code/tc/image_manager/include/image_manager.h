/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ImageManager��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
* �޸ļ�¼2��
*    �޸����ڣ�2013/3/26
*    �� �� �ţ�V2.0
*    �� �� �ˣ���ΰ
*    �޸����ݣ����Ĺ�����and �ܹ�

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
@brief ��������: ӳ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
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
        // ���� ImagePool����
        _ipool = ImagePool::GetInstance();

        //������Ϣ��Ԫ�����߳�
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

