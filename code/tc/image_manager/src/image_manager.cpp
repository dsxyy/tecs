/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：image_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：ImageManager类的实现文件
* 当前版本：2.0
* 作    者：lichun
* 完成日期：2012年10月19日
*
* 修改记录1：
*    修改日期：2012/10/19
*    版 本 号：V1.0
*    修 改 人：lichun
*    修改内容：将image_pool改为非缓存模式
* 修改记录2：
*    修改日期：2013/3/26
*    版 本 号：V2.0
*    修 改 人：颜伟
*    修改内容：整改工作流and 架构
*******************************************************************************/
#include "image_manager.h"
#include "sky.h"
#include "user_pool.h"
#include "authrequest.h"
#include "authmanager.h"
#include "tecs_errcode.h"
#include "ftpd.h"
#include "base_image.h"
#include "db_config.h"
#include "image_store.h"
#include "base_image.h"

namespace zte_tecs
{

ImageManager* ImageManager::instance = NULL;
/******************************************************************************/
/* ImageManager :: Constructor/Destructor                                   */
/******************************************************************************/
ImageManager::ImageManager()
{
    _mu = NULL;
};

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS ImageManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }

    _mu->Print();
    return SUCCESS;
}

/******************************************************************************/
void ImageManager:: MessageEntry(const MessageFrame &message)
{
    int  return_val;

    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            if (SUCCESS != Transaction::Enable(GetDB()))
            {
                OutPut(SYS_ERROR,"ImageManager enable transaction fail!");
                SkyExit(-1, "ImageManager::MessageEntry: call Transaction::Enable(GetDB()) failed.");
            }
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            break;
        }
        break;
    }

    case S_Work:
    {
    switch (message.option.id())
    {
    case EV_IMAGE_DELETE_REQ:
    {
        ImageDeleteMsg msg;
        msg.deserialize(message.data);
        return_val = DealDelete(msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealDelete failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_ENABLE_REQ:
    {
        ImageEnableMsg enable_msg;
        enable_msg.deserialize(message.data);
        return_val = DealEnable(enable_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealEnable failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_PUBLISH_REQ:
    {
        ImagePublishMsg publish_msg;
        publish_msg.deserialize(message.data);
        return_val = DealPublish(publish_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealPublish failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_MODIFY_REQ:
    {
        ImageModifyMsg modify_msg;
        modify_msg.deserialize(message.data);
        return_val = DealModify(modify_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealModify failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_QUERY_REQ:
    {
        ImageQueryMsg query_msg;
        query_msg.deserialize(message.data);
            return_val = DealQuery(query_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealQuery failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_METADATA_REQ:
    {
        ImageMetadataReq req;
        req.deserialize(message.data);
        return_val = DealMetadataReq(req);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealMetadataReq failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_GET_URL_REQ:
    {
        ImageMetadataReq req;
        req.deserialize(message.data);
        return_val = DealUploadReq(req);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealUploadReq failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_FTP_SVR_INFO_REQ:
    {
        ImageFtpSvrInfoGetMsg req;
        req.deserialize(message.data);

        return_val = DealFtpSvrInfoReq(req);
        if (0 != return_val)
        {
            OutPut(SYS_ERROR,"ImageManager::DealFtpSvrInfoReq failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_DEPLOY_REQ:
    {
        ImageDeployMsg deploy_msg;
        deploy_msg.deserialize(message.data);
        return_val = DealDeploy(deploy_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealDeploy failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_CANCEL_REQ:
    {
        ImageCancelMsg cancel_msg;
        cancel_msg.deserialize(message.data);
        return_val = DealCancel(cancel_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealCancel failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_PREPARE_REQ:
    {
        ImagePrepareMsg prepare_msg;
        prepare_msg.deserialize(message.data);
        return_val = DealPrepare(prepare_msg);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealPrepare failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_DEPLOY_ACK:
    {
        OutPut(SYS_DEBUG,"DeployBaseImageAck:%s\n",message.data.c_str());
        DeployBaseImageAck ack;
        ack.deserialize(message.data);
        return_val = DealDeployAck(ack);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealDeployAck failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_CANCEL_ACK:
    {
        OutPut(SYS_DEBUG,"CancelBaseImageAck:%s\n",message.data.c_str());
        CancelBaseImageAck ack;
        ack.deserialize(message.data);
        return_val = DealCancelAck(ack);
        if (0 != return_val)
        {
                OutPut(SYS_ERROR,"ImageManager::DealCancelAck failed! ret = %d\n",return_val);
        }
        break;
    }

    case EV_IMAGE_GET_SPACE_REQ:
    {
        ImageGetSpaceMsg request_msg;
        request_msg.deserialize(message.data);
        return_val = GetSpace(request_msg);
        if (0 != return_val)
        {
            OutPut(SYS_ERROR,"ImageManager::GetSpace failed! ret = %d\n",return_val);
        }
        break;
    }
    
    case EV_ACTION_TIMEOUT:
    {
        break;
    }

    default:
    {
            OutPut(SYS_WARNING,"ImageManager rcv unexpected mesg !, id = %u\n",message.option.id() );
        break;
    }
    }
        break;
    }

    default:
        break;
    }
}

/******************************************************************************/
STATUS ImageManager::DealDelete(const ImageDeleteMsg &request)
{
    ostringstream      oss;
    MessageOption option(_mu->CurrentMessageFrame()->option.sender(),EV_IMAGE_OPERATE_ACK,0,0);

    ImageOperateAckMsg ack_msg(request._uid, ERROR, "");

    /* 有该image? */
    Image image;
    int ret = _ipool->GetImageByImageId(request._image_id, image);
    if (ERROR == ret)
    {
        oss << "Image (" << request._image_id << ") is not exist !" << endl;
        ack_msg._ret_code = ERR_OBJECT_NOT_EXIST;
        ack_msg._result = oss.str();
        _mu->Send(ack_msg,option);

        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(request._uid, &image, 0, AuthRequest::DELETE) == false)
    {
        oss<<"Authorize Fail! user id = " << request._uid << ", image id = " << request._image_id;
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
        ack_msg._result = oss.str();
        _mu->Send(ack_msg,option);

        return ERR_AUTHORIZE_FAILED;
    }

    if (image.get_running_vms() != 0)
    {
        oss <<"image (" << request._image_id << ") is using !" << endl;
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        ack_msg._ret_code = ERR_IMAGE_IS_USING;
        ack_msg._result = oss.str();
        _mu->Send(ack_msg,option);

        return ERR_IMAGE_IS_USING;
    }

    BaseImagePool* baseimg = BaseImagePool::GetInstance();
    vector<BaseImage>  vec_var;
    string where = " image_id = " + to_string<int>(request._image_id,dec);

    ret = baseimg->Select(vec_var, where);
    if(ERROR_OBJECT_NOT_EXIST != ret)
    {
        if (SUCCESS == ret)
        {
            oss <<"DealDelete: image (" << request._image_id << ") is deployed !" << endl;
            OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
            ack_msg._ret_code = ERR_IMAGE_IS_DEPLOYED;
            ack_msg._result = oss.str();
            _mu->Send(ack_msg,option);

            return ERR_IMAGE_IS_DEPLOYED;
        }

        oss<<"DealDelete: Get baseimage (imageid="<< request._image_id <<  ") from table failed! ret= "<< ret  <<  "." << endl ;
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        ack_msg._ret_code = ERROR_DB_SELECT_FAIL;
        ack_msg._result = oss.str();
        _mu->Send(ack_msg,option);
		
        return ERROR_DB_SELECT_FAIL;
    }

    string location = image.get_location();
    int rc = _ipool->Delete(image.get_iid());

    if (0 != rc)
    {
        oss <<"Drop image ("<< request._image_id <<") failed !" << endl;
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        ack_msg._ret_code = ERR_OBJECT_DROP_FAILED;
        ack_msg._result = oss.str();
        _mu->Send(ack_msg,option);

        return ERR_OBJECT_DROP_FAILED;
    }

    oss << "Delete Image (" << request._image_id << ") success !" << endl;
    OutPut(SYS_NOTICE,"remove image file %s ...\n",location.c_str());
    remove(location.c_str());

    ack_msg._result   = oss.str();
    ack_msg._ret_code = SUCCESS;

    return _mu->Send(ack_msg, option);
}

/******************************************************************************/
STATUS ImageManager::DealEnable(const ImageEnableMsg &request)
{
    ostringstream      oss;
    int                rc = -1;

    int64  uid         = request._uid;
    int64  image_id    = request._image_id;
    bool   enable_flag = request._enable_flag;

    MessageOption option(_mu->CurrentMessageFrame()->option.sender(),EV_IMAGE_OPERATE_ACK,0,0);

    ImageOperateAckMsg ack_msg(uid, ERROR, "");

    Image image;
    int ret = _ipool->GetImageByImageId(request._image_id, image);

    if (ERROR == ret)
    {
        ack_msg._ret_code = ERR_OBJECT_NOT_EXIST;
        oss<<"Get image ("<< image_id<<") failed! "<< endl;
        goto enable_error_get;
    }

    /* 授权 */
    if (!Authorize(request._uid, &image, 0, AuthRequest::MANAGE))
    {
        ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
        oss<<"Authorize Fail!user id = " << request._uid << ", image id = " << request._image_id;
        goto enable_error_common;
    }

    /* 只有管理员才能设置enable属性 */
    if (!Authorize(uid, NULL, 0, AuthRequest::MANAGE))
    {
        /* 进来是租户,不能修改enable */
        ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
        oss<<"Authorize Fail";
        goto enable_error_common;
    }
    #if 0 //611003842141 【TECS】禁止被虚拟机使用的镜像提示，失败, 镜像正在被虚拟机模板使用
    if(enable_flag == false)
    {
        //有模板在使用该映像时，禁止将enable变为disable（提示用户先修改模板）
        //检查是否有模板在使用该映像
        int count = _ipool->GetVmTemplateCount(image_id);
        if(count == -1)
        {
            ack_msg._ret_code = ERROR_DB_SELECT_FAIL;
            oss<<"Get image vmtemplate count("<< image_id << ") failed!"<< endl;
            goto enable_error_common;
        }

        //611003191686 【TECS】已被运行的虚拟机使用的镜像还可以被禁用。
        if((count > 0)||(image.get_running_vms() > 0))
        {
            ack_msg._ret_code = ERR_IMAGE_IS_USING;
            oss<<"image("<<image_id<<") is used by vmtemplate!"<< endl;
            goto enable_error_common;
        }
    }
    #endif
    image.set_enable(enable_flag);

    rc = _ipool->Update(image);
    if ( rc < 0 )
    {
        ack_msg._ret_code = ERR_OBJECT_UPDATE_FAILED;
        if (true == enable_flag)
        {
            oss<<"Error trying to ENABLE ";
        }
        else
        {
            oss<<"Error trying to DISABLE ";
        }
        oss<<"image ("<<image_id<< ")." ;
        goto enable_error_common;
    }

    if (true == enable_flag)
    {
        oss<<"Success to ENABLE ";
    }
    else
    {
        oss<<"Success to DISABLE ";
    }

    oss<<"image ("<<image_id<< ")." <<endl;
    ack_msg._ret_code = SUCCESS ;
    ack_msg._result   = oss.str();

    return _mu->Send(ack_msg, option);

enable_error_common:

enable_error_get:
    ack_msg._result = oss.str();

    return _mu->Send(ack_msg, option);
}

/******************************************************************************/
STATUS ImageManager::DealPublish(const ImagePublishMsg &request)
{
    int                rc = -1;
    ostringstream      oss;
    int64  uid          = request._uid;
    int64  image_id     = request._image_id;
    bool   publish_flag = request._publish_flag;

    MessageOption option(_mu->CurrentMessageFrame()->option.sender(),EV_IMAGE_OPERATE_ACK,0,0);
    ImageOperateAckMsg ack_msg(uid,  ERROR, "");


    Image image;
    int ret = _ipool->GetImageByImageId(request._image_id, image);
    if (ERROR == ret)
    {
        ack_msg._ret_code = ERR_OBJECT_NOT_EXIST;
        oss<<"Get image ("<< image_id << ") failed! "<< endl;
        goto publish_error_get;
    }

    /* 授权 */
    if (!Authorize(request._uid, &image, 0, AuthRequest::MANAGE))
    {
        ack_msg._ret_code = ERR_AUTHORIZE_FAILED ;
        oss<<"Authorize Fail!user id = " << request._uid << ", image id = " << request._image_id;
        goto publish_error_common;
    }

    /* 只有管理员才能注册public属性，下面判断是判断当前uid的身份的 */
    if (!Authorize(uid, NULL, 0, AuthRequest::MANAGE))
    {
        /* 进来是租户,不能修改public */
        ack_msg._ret_code = ERR_AUTHORIZE_FAILED ;
        oss<<"Authorize Fail";
        goto publish_error_common;
    }

    if(!publish_flag)
    {
        //当有属性为public的模板在使用映像时，禁止设为非public（提示用户先修改模板）
        int count = _ipool->GetVmTemplateCount(image_id,true);
        if(count == -1)
        {
            ack_msg._ret_code = ERROR_DB_SELECT_FAIL;
            oss<<"Get image vmtemplate count("<< image_id << ") failed!"<< endl;
            goto publish_error_common;
        }

        if((count > 0)||(image.get_running_vms() > 0))
        {
            ack_msg._ret_code = ERR_IMAGE_IS_USING;
            oss<<"image("<<image_id<<") is used by public vmtemplate!"<< endl;
            goto publish_error_common;
        }
    }

    image.set_public(publish_flag);

    rc = _ipool->Update(image);
    if ( rc < 0 )
    {
        ack_msg._ret_code = ERR_OBJECT_UPDATE_FAILED;
        if (true == publish_flag)
        {
            oss<<"Failed to PUBLIC ";
        }
        else
        {
            oss<<"Failed to PRIVATE ";
        }
        oss<<"image ("<<image_id<< ")." <<endl;
        goto publish_error_common;
    }

    if (true == publish_flag)
    {
        oss<<"Success to PUBLIC ";
    }
    else
    {
        oss<<"Success to PRIVATE ";
    }
    oss<<"image ("<<image_id<< ")." <<endl;


    ack_msg._ret_code = SUCCESS ;
    ack_msg._result = oss.str();
    return _mu->Send(ack_msg, option);

publish_error_common:

publish_error_get:

    ack_msg._result = oss.str();
    return _mu->Send(ack_msg, option);
}


/******************************************************************************/
STATUS ImageManager::DealModify(const ImageModifyMsg &request)
{
    int              rc = -1;
    ostringstream    oss;

    /* 构造回应消息 */
    MessageOption option(_mu->CurrentMessageFrame()->option.sender(),EV_IMAGE_OPERATE_ACK,0,0);

    ImageOperateAckMsg ack_msg(request._uid,  ERROR,"");

    /* 要修改的参数 */
    int64   image_id = request._image_id;
    string  name     = request._name;
    string  type     = request._type;
    string  arch     = request._arch;
    string  bus      = request._bus;
    string  os_type  = request._os_type;
    string  description = request._description;
    string  container_format = request._container_format;

    /* 从image pool中获取image对象 */

    Image image;
    int ret = _ipool->GetImageByImageId(request._image_id, image);
    if (ERROR == ret)
    {
        ack_msg._ret_code = ERR_OBJECT_NOT_EXIST;
        oss<<"Get image ("<< image_id << ") failed! "<< endl ;
        goto modify_error_get;
    }

    /* 授权 */
    if (Authorize(request._uid, &image, 0, AuthRequest::MANAGE) == false)
    {
        ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
        oss<<"Authorize Fail!user id = " << request._uid << ", image id = " << request._image_id;
        goto modify_error_common;
    }

    /* 先刷新当前的使用状态，才能对映像进行删除处理 */
    if (image.get_running_vms() != 0)
    {
        /* 已经被使用的映像，是不能设置bus的 */
        if ((image.get_running_vms() != 0)||(0 != _ipool->GetVmTemplateCount(image_id)))
        {
            if (image.get_bus() != bus)
            {
                ack_msg._ret_code = ERR_IMAGE_IS_USING;
                oss <<"image ("<<image_id<<") is using !" << endl;
                goto modify_error_common;
            }
        }

        /* 已经被使用的映像，是不能设置arch的 */
        if (!type.empty())
        {
            if (image.get_type() != type)
            {
                ack_msg._ret_code = ERR_IMAGE_IS_USING;
                oss <<"image ("<<image_id<<") is using !" << endl;
                goto modify_error_common;
            }
        }

        if (!arch.empty())
        {
            if (image.get_arch() != arch)
            {
                ack_msg._ret_code = ERR_IMAGE_IS_USING;
                oss <<"image ("<<image_id<<") is using !" << endl;
                goto modify_error_common;
            }
        }
    }

    /* 逐项更新对象属性 */
    if (!name.empty())
    {
        image.set_name(name);
    }

    if (!type.empty())
    {
        image.set_type(type);
    }

    if (!arch.empty())
    {
        image.set_arch(arch);
    }

    if (!bus.empty())
    {
        image.set_bus(bus);
    }
    image.set_os_type(os_type);
    image.set_description(description);
    image.set_container_format(container_format);

    /* 更新数据库 */
    rc = _ipool->Update(image);
    if ( rc < 0 )
    {
        ack_msg._ret_code = ERR_OBJECT_UPDATE_FAILED;
        oss<<"Error trying to MODIFY image ("<<image_id<< ") Attribute." <<endl;
        goto modify_error_common;
    }


    ack_msg._ret_code = SUCCESS;
    ack_msg._result   = oss.str();
    return _mu->Send(ack_msg, option);

modify_error_common:


modify_error_get:

    ack_msg._result = oss.str();
    return _mu->Send(ack_msg, option);
}

/******************************************************************************/
STATUS ImageManager::DealQuery(const ImageQueryMsg &request)
{
    int                rc       = -1;
    ostringstream      where_string;
    ostringstream      where_string_max;
    ostringstream      oss;
    ostringstream      tmp_oss;
    vector<ImageInfo>  image_info;
    int                query_flag = false;
    int64              out_num;
    int64              i;
    int64              max_query_num;

    int64  operator_uid = request._uid;
    int64  query_uid   = request._query_uid;

    MessageOption option(_mu->CurrentMessageFrame()->option.sender(),EV_IMAGE_OPERATE_ACK,0,0);
    ImageQueryAckMsg   ack_msg(operator_uid, ERROR, "");

    switch (static_cast <ImageQueryMsg::ImageQueryType>(request._query_type) )
    {
        /* 该用户注册的映像文件,只能查询自己  */
    case ImageQueryMsg::USER_REGISTER:
    {
        where_string << " uid = " << operator_uid <<\
        " order by image_id limit " <<request._query_count <<\
        " offset " <<request._start_index ;
        where_string_max << " uid = " << operator_uid;
        query_flag = true;
        break;
    }

    /* 该用户可见的映像,只能查询自己 */
    case ImageQueryMsg::USER_VISIBLE:
    {
        where_string << "( uid = " << operator_uid << " OR is_public = 1 )"<<\
        " order by image_id limit " <<request._query_count <<\
        " offset " <<request._start_index ;
        where_string_max << " uid = " << operator_uid << " OR is_public = 1 ";
        query_flag = true;
        break;
    }

    /* 用户可见的公共映像,只能查询自己  */
    case ImageQueryMsg::USER_PUBLIC:
    {
        where_string << " is_public = 1 "<<\
        " order by image_id  limit " <<request._query_count <<\
        " offset " <<request._start_index ;
        where_string_max << " is_public = 1 ";
        query_flag = true;
        break;
    }

    /* 指定用户查询该用户对应的image  */
    case ImageQueryMsg::APPOINTED:
    {
        /* 授权 */
        if (Authorize(operator_uid, NULL, query_uid, AuthRequest::INFO) == false)
        {
            oss<<"Authorize Fail";
            ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
            goto query_error_common;
        }

        where_string << " uid = " << query_uid << \
        " order by image_id  limit " <<request._query_count <<\
        " offset " <<request._start_index ;
        where_string_max << " uid = " << query_uid ;
        query_flag = true;
        break;
    }

    case ImageQueryMsg::APPOINT_IMAGE_ID:
    {
        where_string << " image_id = " << request._query_image_id;
        where_string_max << " image_id = " << request._query_image_id;

        /* 授权 */
        Image image;
        int ret = _ipool->GetImageByImageId(request._query_image_id, image);
        if (ERROR == ret)
        {
            ack_msg._ret_code = ERR_OBJECT_NOT_EXIST;
            oss<<"Get image ("<< request._query_image_id << ") failed! "<< endl ;
            goto query_error_common;
        }

        /* 只有管理员才能查询所有 */
        if (!Authorize(operator_uid, NULL, 0, AuthRequest::INFO))
        {
            /* 进来是租户 */
            if (!image.get_public() && \
                    image.get_uid() != operator_uid)
            {
                oss<<"Authorize Fail";
                ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
                goto query_error_common;
            }
        }
        break;
    }

    case ImageQueryMsg::QUERY_ALL_IMAGE:
    {
        /* 授权 */
        if (Authorize(operator_uid, NULL, 0, AuthRequest::INFO) == false)
        {
            oss<<"Authorize Fail";
            ack_msg._ret_code = ERR_AUTHORIZE_FAILED;
            goto query_error_common;
        }

        where_string << " 1=1 "  \
        " order by image_id  limit " <<request._query_count <<\
        " offset " <<request._start_index ;
        where_string_max << " 1=1 " ;
        query_flag = true;
        break;
    }

    case ImageQueryMsg::APPOINT_IMAGE_NAME:
    {

        //只能查询操作者自己的镜像 ,模式匹配:location %filename
        where_string << " uid = " << operator_uid << " AND location LIKE '%/"<<request._query_filename<<"'";
        where_string_max << " uid = " << operator_uid << " AND location LIKE '%/"<<request._query_filename<<"'";
        query_flag = true;
        break;
    }

    default:
    {
        oss<<"Incorrect query type: "<< request._query_type <<endl;
        ack_msg._ret_code = ERR_IMAGE_INVALID_QUERY_TYPE;
        goto  query_error_common;
        break;
    }
    }

    // 调用image_pool的Show方法
    rc = _ipool->Show(image_info, where_string.str());
    if (rc < 0)
    {
        oss<<"Query failed !" ;
        ack_msg._ret_code = ERROR_DB_SELECT_FAIL;
        goto  query_error_common;
    }

    /* 对下包起始位置进行设置 */
    if (query_flag == false)
    {
        ack_msg._next_index = -1;
        out_num = image_info.size();
    }
    else
    {
        if (image_info.size() >= (uint64)(request._query_count))
        {
            ack_msg._next_index = request._start_index + request._query_count;
            out_num = request._query_count;
        }
        else
        {
            ack_msg._next_index = -1;
            out_num = image_info.size();
        }
    }

    /* 把查询的结果放到消息中 */
    for (i = 0; i < out_num; i++)
    {
        ack_msg._image_info.push_back(image_info.at(i));
    }


    /* 查询符合条件的总数 */
    max_query_num = _ipool->GetImageCountByWhere(where_string_max.str());
    if (max_query_num < 0)
    {
        oss<<"Query failed !" ;
        ack_msg._ret_code = ERROR_DB_SELECT_FAIL;
        goto  query_error_common;
    }
    ack_msg._max_count = max_query_num;

    ack_msg._ret_code = SUCCESS;
    return _mu->Send(ack_msg, option);

query_error_common:
    ack_msg._err_str = oss.str();
    return _mu->Send(ack_msg, option);
}

/******************************************************************************/
int ImageManager::DealMetadataReq(const ImageMetadataReq& req)
{
    ImageMetadataAck ack;
    Image tmp;
    ack._image._image_id = req._image_id;
    ack._result = _ipool->CheckImageAuth(req._image_id,req._uid,tmp);
    if (SUCCESS == ack._result)
    {
        ack._image._is_public = tmp.get_public();
        ack._image._arch = tmp.get_arch();
        ack._image._name = tmp.get_name();
        ack._image._size = tmp.get_size();
        ack._image._disk_size = tmp.get_disk_size();
        tmp.get_file_url(ack._image._url);
        ack._image._type = tmp.get_type();
        ack._image._uid = tmp.get_uid();
        ack._image._bus = tmp.get_bus();
        ack._image._os_type= tmp.get_os_type();
        ack._image._disk_format = tmp.get_disk_format();
    }

    MessageOption option(_mu->CurrentMessageFrame()->option.sender(), EV_IMAGE_METADATA_ACK, 0,0);
    return _mu->Send(ack,option);
}


/******************************************************************************/
STATUS ImageManager::DealUploadReq(const ImageMetadataReq& req)
{
    ImageUploadAck ack;

    // 构建上传的方式和地址，主要是为了实现将来的兼容
    GetUpLoadUrl(ack._url);
    ack._result = SUCCESS;
    // 其他参数为NULL的话，采用默认值
    // 用户名和密码如果为NULL的话，在在RPC的地方进行填充
    MessageOption option(_mu->CurrentMessageFrame()->option.sender(), EV_IMAGE_GET_URL_ACK, 0,0);
    return _mu->Send(ack,option);
}

/******************************************************************************/
STATUS ImageManager::DealFtpSvrInfoReq(const ImageFtpSvrInfoGetMsg& req)
{
    ImageFtpSvrInfoAckMsg ack(req._uid);
    ostringstream      oss;
    int port = 0;
    string ip;

    // 构建上传的方式和地址，主要是为了实现将来的兼容
    STATUS ret = GetFtpSvrInfo(port,ip);

    if(ERROR == ret)
    {
        ack._ret_code = ERROR;
        oss << "Get image ftp server failed"<<endl;
        ack._err_str = oss.str();
    }
    else
    {
        ack._image_ftp_svr_info._ftp_svr_port = port;
        ack._image_ftp_svr_info._ftp_svr_ip = ip;
        ack._ret_code = SUCCESS;
    }

    MessageOption option(_mu->CurrentMessageFrame()->option.sender(), EV_IMAGE_FTP_SVR_INFO_ACK, 0,0);
    return _mu->Send(ack,option);
}
/******************************************************************************/
STATUS ImageManager::CheckTableAndWorkflow(const string& base_uuid,
                                                   const string& cluster_name,
                                                   bool &table,
                                                   bool &forward,
                                                   bool &rollback,
                                                   bool &reverse,
                                                   int &progress,
                                                   string &workflow_id,
                                                   string &rollback_id,
                                                   ostringstream &oss)
{
    int rc = ERROR;
    vector<BaseImageClusterWithImageId> vec_tables;

    rc = GetBaseImageFromTable(base_uuid,cluster_name,vec_tables);
    if (SUCCESS != rc && ERROR_OBJECT_NOT_EXIST != rc)
    {
        oss<<"Get base image ("<< base_uuid <<","<< cluster_name << ") from table failed! "<< endl ;
        return rc;
    }

    table = (0 == vec_tables.size())?false:true;

    ostringstream where;
    where << "label_string_1 = '" << base_uuid <<
             "' AND label_string_2 = '" << cluster_name + "'";

    vector<string> workflows;
    //找正向
    rc = FindWorkflow(workflows,IMAGE_CATEGORY, IM_DEPLOY_IMAGE,where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find work flow ("<< IMAGE_CATEGORY <<","<< IM_DEPLOY_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        forward = true;
        workflow_id = workflows[0];
        progress = GetWorkflowProgress(workflow_id);
        oss<<"Work flow "<< workflow_id <<"("<< IMAGE_CATEGORY <<","<< IM_DEPLOY_IMAGE << ") exist,progress = " << progress <<endl;
        rc = GetRollbackWorkflow(workflow_id, rollback_id);
        if (SUCCESS != rc)
        {
            oss<<"Get rollback work flow of "<< workflow_id << ") failed! "<< endl ;
            return rc;
        }
        //有正向 不需要再去找回滚和反向了
        return SUCCESS;
    }
    //找回滚和反向
    rc = FindWorkflow(workflows,IMAGE_CATEGORY, IM_CANCEL_IMAGE,where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find work flow ("<< IMAGE_CATEGORY <<","<< IM_CANCEL_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        if (IsRollbackWorkflow(workflows[0]))
        {
            rollback = true;
            rollback_id = workflows[0];
        }
        else
        {
            reverse = true;
            workflow_id = workflows[0];
        }
        progress = GetWorkflowProgress(workflows[0]);
        oss<<"Work flow "<< workflows[0] <<"("<< IMAGE_CATEGORY <<","<< IM_CANCEL_IMAGE << ") exist,progress = " << progress <<endl;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS ImageManager::CheckTableAndWorkflow(int64 image_id,
                                                   const string& cluster_name,
                                                   int &tables,
                                                   int &forwards,
                                                   int &rollbacks,
                                                   int &reverses,
                                                   int &progress,
                                                   ostringstream &oss)
{
    int rc = ERROR;
    vector<BaseImageClusterWithImageId> vec_tables;

    rc = GetBaseImageFromTable(image_id,cluster_name,vec_tables);
    if (SUCCESS != rc && ERROR_OBJECT_NOT_EXIST != rc)
    {
        oss<<"Get base image ("<< image_id << ","<< cluster_name << ") from table failed! "<< endl ;
        return rc;
    }

    tables = vec_tables.size();

    ostringstream where;
    where << "label_int64_1 = " << image_id <<
             " AND label_string_2 = '" << cluster_name + "'";

    vector<string> workflows;
    vector<string>::iterator it;
    rc = FindWorkflow(workflows,IMAGE_CATEGORY, IM_DEPLOY_IMAGE,where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find work flow ("<< IMAGE_CATEGORY <<","<< IM_DEPLOY_IMAGE << ") failed! "<< endl ;
        return rc;
    }

    if (0 != workflows.size())
    {
        int max_progress = 0;
        string workflow_id;
        for(it = workflows.begin(); it != workflows.end(); it++)
        {
            progress = GetWorkflowProgress(*it);
            if (progress > max_progress)
            {
                max_progress = progress;
                workflow_id = *it;
            }
        }
        oss<<"Work flow "<< workflow_id <<"("<< IMAGE_CATEGORY <<","<< IM_DEPLOY_IMAGE << ") exist,max progress = " << progress <<endl;
    }
    forwards = workflows.size();
    workflows.clear();
    rc = FindWorkflow(workflows,IMAGE_CATEGORY, IM_CANCEL_IMAGE,where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find work flow ("<< IMAGE_CATEGORY <<","<< IM_CANCEL_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        for(it = workflows.begin(); it != workflows.end(); it++)
        {
            if (IsRollbackWorkflow(*it))
            {
                rollbacks++;
            }
            else
            {
                reverses++;
            }
        }
        oss<<"Work flow " << "("<< IMAGE_CATEGORY <<","<< IM_CANCEL_IMAGE << ") rollback:"<<rollbacks<<", reverse:"<<reverses<<" exist" << endl;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS ImageManager::CreateDeployWorkflow(const MID &sender, const ImageDeployMsg &req, ostringstream &oss, string & workflow_id)
{
    STATUS ret = ERROR;
    ImageStoreOption option;

    ret = _ipool->GetImageStoreOption(req._cluster_name, option);
    if (SUCCESS != ret)
    {
        oss<<"Get image store option ("<< req._cluster_name << ") failed! ret:"<< ret << endl ;
        return ret;
    }

    if (IMAGE_USE_NOSNAPSHOT == option._share_img_usage)
    {//默认为SNAPSHOT
        oss<<"No-Snapshot Mode Not Support Image Deploy!"<< endl ;
        return ERROR_NOT_SUPPORT;
    }

    /* 从image pool中获取image对象 */
    Image image;
    ret = _ipool->GetImageByImageId(req._image_id, image);
    if (SUCCESS != ret)
    {
        oss<<"Get image ("<< req._image_id << ") failed! ret:"<< ret << endl ;
        return ret;
    }

    if (!image.get_enable())
    {//禁用的镜像不支持部署
        oss<<"Disabled Image ("<< req._image_id << ") Not Support Image Deploy! "<< endl ;
        return ERROR_NOT_SUPPORT;
    }

    /* 根据cluster_name查询共享存储的剩余空间，并判断是否满足部署要求 */
    int64 share_disk_size = 0;
    ret = _ipool->GetShareDiskInfo(req._cluster_name, share_disk_size);
    if ((ERROR == ret)||(image.get_size() > MBToBytes(share_disk_size)))
    {
        oss<< "Image_id ("<<req._image_id<<") share disk not enough: "<< image.get_size() << " needed , "<< MBToBytes(share_disk_size)<< " free! "<< endl ;
        return ERROR_NO_SPACE;
    }

    Action deploy_action(IMAGE_CATEGORY,IM_DEPLOY_IMAGE,EV_IMAGE_DEPLOY_REQ,MID(_mu->name()),MID(req._cluster_name,PROC_IMAGE_AGENT,MU_IMAGE_AGENT));
    FileUrl url;
    image.get_file_url(url);
    int64 base_id;
    GetBaseImageIndex(base_id);
    DeployBaseImageReq deploy_req(deploy_action.get_id(), req._base_uuid, req._cluster_name, base_id,image.get_iid(), image.get_size(), image.get_disk_size(), url, option);
    deploy_action.message_req = deploy_req.serialize();

    Action rollback_action(IMAGE_CATEGORY,IM_CANCEL_IMAGE,EV_IMAGE_CANCEL_REQ,MID(_mu->name()),MID(req._cluster_name,PROC_IMAGE_AGENT,MU_IMAGE_AGENT));
    CancelBaseImageReq cancel_req(rollback_action.get_id(), req._base_uuid, req._cluster_name, base_id,image.get_iid(), image.get_size(), image.get_disk_size(), url, option);
    rollback_action.message_req = cancel_req.serialize();

    deploy_action.rollback_action_id = rollback_action.get_id();
    deploy_action.timeout_feedback = false;
    rollback_action.timeout_feedback = false;

    BaseImageActionLabels a_labels;
    a_labels._base_uuid = req._base_uuid;
    a_labels._image_id = req._image_id;
    a_labels._cluster_name = req._cluster_name;
    deploy_action.labels = a_labels;
    rollback_action.labels = a_labels;

    Workflow w_rollback(IMAGE_CATEGORY,IM_CANCEL_IMAGE);
    w_rollback.Add(&rollback_action);

    Workflow w(IMAGE_CATEGORY,IM_DEPLOY_IMAGE);
    w.Add(&deploy_action);

    if(!req._user_name.empty())
    {
        w.originator = req._user_name;
    }

    w.upstream_action_id = req.action_id;
    w.upstream_sender = sender;
    w_rollback.upstream_action_id = req.action_id;
    w_rollback.upstream_sender = sender;

    BaseImageLabels labels;
    labels._uid = req._uid;
    labels._base_uuid = req._base_uuid;
    labels._image_id = req._image_id;
    labels._cluster_name = req._cluster_name;
    labels._base_id = base_id;
    w.labels = labels;
    w_rollback.labels = labels;

    workflow_id = w.get_id();

    ret = CreateWorkflow(w,w_rollback);
    if(SUCCESS != ret)
    {
        oss<<"Create Deploy Image Workflow Failed!" << endl ;
    }

    return ret;

}
/******************************************************************************/
STATUS ImageManager::CreateCancelWorkflow(const MID &sender, const ImageCancelMsg &req, ostringstream &oss, string & workflow_id)
{
    STATUS ret = ERROR;
    ImageStoreOption option;

    ret = _ipool->GetImageStoreOption(req._cluster_name, option);
    if (SUCCESS != ret)
    {
        oss<<"Get image store option ("<< req._cluster_name << ") failed! ret:"<< ret << endl ;
        return ret;
    }
    if (IMAGE_USE_NOSNAPSHOT == option._share_img_usage)
    {//默认为SNAPSHOT
        oss<<"Non-Snapshot Mode Not Support Image Deploy!"<< endl ;
        return ERROR_NOT_SUPPORT;
    }
    vector <BaseImageClusterWithImageId> vec_var;
    ret = GetBaseImageFromTable(req._base_uuid, req._cluster_name, vec_var);
    if (SUCCESS != ret)
    {
        oss<<"Get base image ("<< req._base_uuid << "," << req._cluster_name << ") failed! ret:"<< ret << endl ;
        return ret;
    }

    /* 从image pool中获取image对象 */
    Image image;
    ret = _ipool->GetImageByImageId(vec_var[0]._image_id, image);
    if (SUCCESS != ret)
    {
        oss<<"Get image ("<< vec_var[0]._image_id << ") failed! ret:"<< ret << endl ;
        return ret;
    }

    Action a(IMAGE_CATEGORY,IM_CANCEL_IMAGE,EV_IMAGE_CANCEL_REQ,MID(_mu->name()),MID(req._cluster_name,PROC_IMAGE_AGENT,MU_IMAGE_AGENT));
    FileUrl url;
    image.get_file_url(url);
    CancelBaseImageReq cancel_req(a.get_id(), req._base_uuid, req._cluster_name, vec_var[0]._base_id, image.get_iid(), image.get_size(), image.get_disk_size(), url, option);
    a.message_req = cancel_req.serialize();

    a.timeout_feedback = false;

    BaseImageActionLabels a_labels;
    a_labels._base_uuid = req._base_uuid;
    a_labels._image_id = vec_var[0]._image_id;
    a_labels._cluster_name = req._cluster_name;
    a.labels = a_labels;

    Workflow w(IMAGE_CATEGORY,IM_CANCEL_IMAGE);
    w.Add(&a);

    if(!req._user_name.empty())
    {
        w.originator = req._user_name;
    }

    w.upstream_sender = sender;
    w.upstream_action_id = req.action_id;

    BaseImageLabels labels;
    labels._uid = req._uid;
    labels._base_uuid = req._base_uuid;
    labels._image_id = vec_var[0]._image_id;
    labels._cluster_name = req._cluster_name;
    labels._base_id = vec_var[0]._base_id;
    w.labels = labels;

    workflow_id = w.get_id();

    ret = CreateWorkflow(w);
    if(SUCCESS != ret)
    {
        oss<<"Create Cancel Image Workflow Failed,ret:" << ret << endl ;
    }

    return ret;
}
/******************************************************************************/
bool ImageManager::IsSenderSelf(const MID& sender)
{
    MID self;
    _mu->GetSelfMID(self);
    return (self._application == sender._application
            && self._process == sender._process
            && self._unit == sender._unit);
}
/******************************************************************************/
STATUS ImageManager::DealDeploy(const ImageDeployMsg &req)
{
    STATUS rc = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
    ImageDeployAckMsg ack_msg(req.action_id, req._uid, req._base_uuid, req._cluster_name);

    bool table = false,forward = false,rollback = false,reverse = false;
    int progress = 0;
    string workflow_id,rollback_id;

    rc = CheckTableAndWorkflow(req._base_uuid,
                               req._cluster_name,
                               table,
                               forward,
                               rollback,
                               reverse,
                               progress,
                               workflow_id,
                               rollback_id,
                               oss);
    if (SUCCESS != rc)
    {//查询
        ack_msg.state = rc;
    }
    else
    {

        if (forward)
        {//有正向
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Deploying Image Exist"<<endl ;
        }
        else if (rollback)
        {//有回滚
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Rollbacking Image Exist"<<endl ;
        }
        else if (reverse)
        {//有反向
            ack_msg.state = ERROR_ACTION_CANCELLED;
            ack_msg.progress = 0;
            oss<<"Canceling Image Exist"<<endl ;
        }
        else if (table)
        {//有表
            ack_msg.state = SUCCESS;
            ack_msg.progress = 100;
            oss<<"Deployed Image Exist"<<endl ;

        }
        else if (!table)
        {//无表
            if (IsUuidDeleted(req._base_uuid))
            {//uuid 已经入回收站了
                ack_msg.state = ERROR_DUPLICATED_OP;
                ack_msg.progress = 0;
                oss<<"BaseUuid " << req._base_uuid << " Is Discarded"<<endl ;
            }
            else
            {
                rc = CreateDeployWorkflow(sender, req, oss, ack_msg._workflow_id);
                if (SUCCESS != rc)
                {
                    ack_msg.state = rc;
                    oss<<"Create Deploy Workflow Failed,ret:"<<rc<<endl ;
                }
                else
                {
                    ack_msg.state = ERROR_ACTION_RUNNING;
                    ack_msg.progress = 0;
                    oss<<"Ready To Deploy Image"<<endl ;
                }
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Deploy Base(%s) To %s Success,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Deploy Base(%s) To %s Failed,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    //如果sender就是自己就不发应答了
    if (!IsSenderSelf(sender))
    {
        GetWorkflowEngine(ack_msg._workflow_id,ack_msg._workflow_engine);

        OutPut(SYS_ERROR,"engine target %s _workflow_id %s\n",
                         ack_msg._workflow_engine.c_str(),
                         ack_msg._workflow_id.c_str());
        rc = _mu->Send(ack_msg, ack_option);
    }
    return rc;
}
/******************************************************************************/
STATUS ImageManager::DealCancel(const ImageCancelMsg &req)
{
    int rc = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_IMAGE_CANCEL_ACK, 0, 0);
    ImageCancelAckMsg ack_msg(req.action_id, req._uid, req._base_uuid, req._cluster_name);

    bool table = false,forward = false,rollback = false,reverse = false;
    int progress = 0;
    string workflow_id,rollback_id;

    rc = CheckTableAndWorkflow(req._base_uuid,
                               req._cluster_name,
                               table,
                               forward,
                               rollback,
                               reverse,
                               progress,
                               workflow_id,
                               rollback_id,
                               oss);
    if (SUCCESS != rc)
    {//查询
        ack_msg.state = rc;
    }
    else
    {
        if (forward)
        {//有正向
            rc = RollbackWorkflow(workflow_id);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Rollback Image Deploy Failed,ret:"<< rc <<endl ;
            }
            else
            {
                oss<<"Ready To Rollback Image Deploy"<<endl ;
                rc = UnsetRollbackAttr(rollback_id);
                if (SUCCESS != rc)
                {
                    ack_msg.state = rc;
                    oss<<"Unset Rollback Attr Failed,ret:"<< rc <<endl ;
                }
                else
                {
                    oss<<"Unset Rollback Attr Success"<<endl ;
                    ack_msg.state = ERROR_ACTION_RUNNING;
                    ack_msg.progress = progress;
                    oss<<"Canceling Image"<<endl ;
                }
            }
        }
        else if (rollback)
        {//有回滚
            rc = UnsetRollbackAttr(rollback_id, sender, req.action_id);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Unset Rollback Attr Failed,ret:"<< rc <<endl ;
            }
            else
            {
                oss<<"Unset Rollback Attr Success"<<endl ;
                ack_msg.state = ERROR_ACTION_RUNNING;
                ack_msg.progress = progress;
                oss<<"Canceling Image"<<endl ;
            }
        }
        else if (reverse)
        {//有反向
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Canceling Image"<<endl ;
        }
        else if (!table)
        {//无表
            ack_msg.state = SUCCESS;
            ack_msg.progress = 100;
            oss<<"Cancel Image Success For Canceled"<<endl ;
        }
        else if (table)
        {//有表
            rc = CreateCancelWorkflow(sender,req,oss, ack_msg._workflow_id);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Create Cancel Workflow Failed,ret:"<< rc <<endl ;
            }
            else
            {
                ack_msg.state = ERROR_ACTION_RUNNING;
                ack_msg.progress = progress;
                oss<<"Ready To Cancel Image"<<endl ;
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Cancel Base(%s) To %s Success,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Cancel Base(%s) To %s Failed,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }

    GetWorkflowEngine(ack_msg._workflow_id,ack_msg._workflow_engine);
    return _mu->Send(ack_msg, ack_option);
}
/******************************************************************************/
STATUS ImageManager::DealPrepare(const ImagePrepareMsg &req)
{
    STATUS rc = ERROR;
    ostringstream oss;

    MessageOption ack_option(_mu->CurrentMessageFrame()->option.sender(), EV_IMAGE_PREPARE_ACK, 0, 0);
    ImagePrepareAckMsg ack_msg(req.action_id,req._uid,req._image_id,req._cluster_name);

    MessageOption deploy_option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), EV_IMAGE_DEPLOY_REQ,0,0);
    ImageDeployMsg deploy_msg;

    int tables = 0,forwards = 0,rollbacks = 0,reverses = 0;
    int progress = 0;

    rc = CheckTableAndWorkflow(req._image_id,
                               req._cluster_name,
                               tables,
                               forwards,
                               rollbacks,
                               reverses,
                               progress,
                               oss);
    if (SUCCESS != rc)
    {//查询
        ack_msg.state = rc;
    }
    else
    {
        if (reverses < tables)
        {//表中的多于反向的
            ack_msg.state = SUCCESS;
            ack_msg.progress = 100;
            oss<<"Deployed Image Exist"<<endl ;
        }
        else if (0 != forwards)
        {//有正向
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Deploying Image Exist"<<endl ;
        }
        else if(rollbacks > 0)
        {
            ack_msg.state = ERROR_DEVICE_BUSY;
            ack_msg.progress = 0;
            oss<<"Too Many Rollbacks"<<endl ;
        }
        else
        {//无表无正向或者表中的小于等于反向的
            ImageStoreOption option;
            if (SUCCESS != _ipool->GetImageStoreOption(req._cluster_name, option))
            {
                oss<<"Get image store option ("<< req._cluster_name << ") failed!" << endl ;
                ack_msg.state = ERROR_DB_SELECT_FAIL;
                ack_msg.progress = 0;
            }
            else if (IMAGE_USE_NOSNAPSHOT == option._share_img_usage)
            {
                oss<<"Non-Snapshot Mode Not Support Image Deploy!"<< endl ;
                ack_msg.state = SUCCESS;
                ack_msg.progress = 100;
            }
            else
            {
                deploy_msg.action_id = GenerateUUID();
                deploy_msg._base_uuid = GenerateUUID();
                deploy_msg._cluster_name = req._cluster_name;
                deploy_msg._image_id = req._image_id;
                deploy_msg._uid = req._uid;
                rc = _mu->Send(deploy_msg, deploy_option);
                if (SUCCESS != rc)
                {
                    ack_msg.state = ERROR_MESSAGE_FAIL;
                    oss<<"Send Message To Me Failed" <<endl;
                }
                else
                {
                    ack_msg.state = ERROR_ACTION_RUNNING;
                    ack_msg.progress = 0;
                    oss<<"Ready To Deploy Image"<<endl ;
                }
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Prepare Base(%ld) To %s Success,state:%d,detail:%s\n",
                         req._image_id,
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Prepare Base(%ld) To %s Failed,state:%d,detail:%s\n",
                         req._image_id,
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    return _mu->Send(ack_msg, ack_option);
}
/******************************************************************************/
STATUS ImageManager::DealDeployAck(const DeployBaseImageAck &ack)
{
    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageManager receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    BaseImageLabels labels;
    string upstream_action_id;
    MID sender;

    switch(ack.state)
    {
        case SUCCESS:
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            rc = GetUpstreamActionId(ack.action_id,upstream_action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            rc = GetUpstreamSender(ack.action_id,sender);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            rc = Transaction::Begin();
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            rc = AllocateBaseImage(labels._base_uuid,labels._cluster_name,labels._image_id,labels._base_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = Transaction::Commit();
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            //sender是自己就不发送应答了
            if (!IsSenderSelf(sender))
            {
                ImageDeployAckMsg up_ack(upstream_action_id,labels._uid,labels._base_uuid,labels._cluster_name);
                up_ack.state = SUCCESS;
                up_ack.progress = 100;
                up_ack.detail = "OK";
                MessageOption up_ack_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
                rc = _mu->Send(up_ack, up_ack_option);
            }
            break;
        case ERROR_ACTION_RUNNING:
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        default:
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RollbackWorkflow(ack.action_id);
            break;
    }

    return rc;
}
STATUS ImageManager::DealCancelAck(const CancelBaseImageAck &ack)
{
    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageManager receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    BaseImageLabels labels;
    string upstream_action_id;
    MID sender;

    switch(ack.state)
    {
        case SUCCESS:
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            rc = GetUpstreamActionId(ack.action_id,upstream_action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            rc = GetUpstreamSender(ack.action_id,sender);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            if (IsRollbackWorkflow(ack.action_id))
            {//正向回滚成功 finishaction 并返回部署失败
                rc = FinishAction(ack.action_id);
                if (SUCCESS != rc)
                {
                    SkyAssert(false);
                    break;
                }
                //发部署失败应答
                if (!IsSenderSelf(sender))
                {
                    ImageDeployAckMsg deploy_ack(upstream_action_id,labels._uid,labels._base_uuid,labels._cluster_name);
                    deploy_ack.state = ERROR;
                    MessageOption deploy_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
                    rc = _mu->Send(deploy_ack, deploy_option);
                }
                else
                {
                    rc = ERROR;
                }
            }
            else
            {//反向成功
                rc = Transaction::Begin();
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    break;
                }
                rc = DeleteBaseImage(labels._base_uuid,labels._cluster_name);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = FinishAction(ack.action_id);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = DeleteUuid(labels._base_uuid);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = Transaction::Commit();
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                //发撤销成功应答
                ImageCancelAckMsg cancel_ack(upstream_action_id,labels._uid,labels._base_uuid,labels._cluster_name);
                cancel_ack.state = SUCCESS;
                cancel_ack.progress = 100;
                MessageOption cancel_option(sender, EV_IMAGE_CANCEL_ACK, 0, 0);
                rc = _mu->Send(cancel_ack, cancel_option);
            }
            break;
        case ERROR_ACTION_RUNNING:
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        default:
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RestartAction(ack.action_id);
            break;
    }
    return rc;
}
/******************************************************************************/
bool ImageManager::Authorize(int64 opt_uid, Image *buf, int64 self_id, int operate)
{
    /* 获取授权 */
    AuthRequest ar(opt_uid);
    int64 image_id = 0;
    int64 self_uid;

    if (buf == NULL)
    {
        //image_id = 0;
        self_uid = self_id;
    }
    else
    {
        //image_id = buf->get_image_id();
        self_uid = buf->get_uid();
    }

    ar.AddAuthorize(
        AuthRequest::IMAGE,              //授权对象类型,用user做例子
        image_id,                             //对象的ID,image_id
        (AuthRequest::Operation)operate,  //操作类型
        self_uid,                        //对象拥有者的用户ID
        false                            //此对象是否为公有
    );

    if (-1 == UserPool::Authorize(ar))
    {
        /* 授权失败 */
        return false;
    }
    return true;
}

/******************************************************************************/
/********* 查看镜像服务器剩余空间 ********/
STATUS ImageManager::GetSpace(const ImageGetSpaceMsg &request)
{
    int64   freespace = 0;
    int64   totalspace = 0;
    int64   operator_uid = request._uid;
    
    	
    MessageOption option(_mu->CurrentMessageFrame()->option.sender(),EV_IMAGE_GET_SPACE_ACK,0,0);
    ImageGetSpaceAckMsg   ack_msg(operator_uid, ERROR, 0, 0);

    ImageStore* store = ImageStore::GetInstance();
    STATUS ret = store->GetSpace(freespace, totalspace);
    if(SUCCESS == ret)
    {
        ack_msg._ret_code = SUCCESS;
        ack_msg._freespace = freespace;
        ack_msg._totalspace = totalspace;
    } 
    else
    {
        ack_msg._ret_code = ERR_IMAGE_INVALID_QUERY_TYPE;
    }

    return _mu->Send(ack_msg, option);
}

STATUS ImageManager::AddImage(Image& image)
{
    //注意，这里可能需要对image pool加锁，因为它被ftpd并行调用了
    return SUCCESS;
}

STATUS ImageManager::RemoveImage(int64 iid)
{
    return SUCCESS;
}

int64 ImageManager::GetImageIdByLocation(const string& location)
{
    return SUCCESS;
}

STATUS ImageManager::GetImageRefCount(int64 iid)
{
    return SUCCESS;
}

void DbgRegisterImage(const char *access_type,const char *path,int64 size,int64 uid,int64 disk_size,const char*disk_format)
{
    if (!access_type || !path)
    {
        cerr << "invalid args!" << endl;
        return;
    }

    string _access_type(access_type);

#if 0
    if (_access_type != IMAGE_ACCESS_SNAPSHOT && _access_type != IMAGE_ACCESS_DIRECT)
    {
        cerr << "this debug function only support direct and snapshot image access!" << endl;
        return;
    }
#endif

    FileUrl url(path,_access_type,"");
    Image image(uid,INVALID_OID);
    image.set_name("image");
    image.set_size(size);
    image.set_file_url(url);
    image.set_location(path);
    image.set_public(true);
    image.set_type("machine");
    image.set_bus("ide");
    image.set_enable(true);
    image.set_disk_size(disk_size);
    image.set_disk_format(disk_format);
    Datetime dt;
    dt.refresh();
    image.set_register_time(dt.serialize());
    string error_str;
    ImagePool* ipool = ImagePool::GetInstance();
    if (!ipool)
    {
        return;
    }

    if (0 != ipool->Allocate(image,error_str))
    {
        cerr << "allocate failed! error = " << error_str << endl;
        return;
    }
    cout << "success! image id = " << image.get_iid();
    return;
}
DEFINE_DEBUG_FUNC(DbgRegisterImage);

void DbgCacheImage(int64 iid,const char* cluster)
{
    if (!cluster || iid == INVALID_OID)
    {
        cerr << "invalid args!" << endl;
        return;
    }

    ImagePool* ipool = ImagePool::GetInstance();
    if (!ipool)
    {
        cerr << "ImagePool not ready!" << endl;
        return;
    }

    Image temp;
    int result = ipool->GetImageByImageId(iid, temp);
    if (ERROR == result)
    {
        cerr << "error! no image id = " << iid << endl;
        return;
    }

    VmDiskConfig image;
    image._id = iid;
    image._size = temp.get_size();
    temp.get_file_url(image._url);

    MessageUnit temp_mu(TempUnitName("DbgCacheImage"));
    temp_mu.Register();
    MessageOption option(MID(cluster,PROC_IMAGE_AGENT,MU_IMAGE_AGENT), EV_IMAGE_CACHE_REQ,0,0);
    STATUS ret = temp_mu.Send(image, option);
    if (SUCCESS != ret)
    {
        cerr << "fail to send image cache request! ret = " << ret << endl;
    }
    else
    {
        cout << "image cache request has been sent to cluster " << cluster << endl;
    }
}
DEFINE_DEBUG_FUNC(DbgCacheImage);

void DbgDeployImage(int64 image_id,const char* cluster_name)
{
    if (INVALID_IID == image_id)
    {
        cout << "Invalid imageid " << image_id <<endl;
        return;
    }

    if (NULL == cluster_name)
    {
        cout << "cluster name null" <<endl;
        return;
    }

    ImageDeployMsg msg;
    msg._uid = 0;
    msg._image_id = image_id;
    msg._cluster_name = cluster_name;
    msg._base_uuid = GenerateUUID();

    MessageUnit temp_mu(TempUnitName("DbgDeployImage"));
    temp_mu.Register();
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), EV_IMAGE_DEPLOY_REQ,0,0);
    STATUS ret = temp_mu.Send(msg, option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send image deploy request! ret = " << ret << endl;
    }
    else
    {
        cout << "Image deploy request send success" << endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        ImageDeployAckMsg  ack;
        ack.deserialize(message.data);
        ack.Print();
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgDeployImage);

void DbgCancelImage(const char* base_uuid,const char* cluster_name)
{
    if (NULL == base_uuid)
    {
        cout << "base uuid null" << endl;
        return;
    }

    if (NULL == cluster_name)
    {
        cout << "cluster name null" << endl;
        return;
    }

    ImageCancelMsg msg;
    msg._cluster_name = cluster_name;
    msg._base_uuid = base_uuid;

    MessageUnit temp_mu(TempUnitName("DbgCancelImage"));
    temp_mu.Register();
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), EV_IMAGE_CANCEL_REQ,0,0);
    STATUS ret = temp_mu.Send(msg, option);
    if (SUCCESS != ret)
    {
        cerr << "fail to send image cancel request! ret = " << ret << endl;
    }
    else
    {
        cout << "image cancel request send success" << endl;
    }
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        ImageCancelAckMsg  ack;
        ack.deserialize(message.data);
        ack.Print();
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }

}
DEFINE_DEBUG_FUNC(DbgCancelImage);

void DbgPrepareImage(int64 image_id,const char* cluster_name)
{
    if (INVALID_IID == image_id)
    {
        cout << "Invalid imageid " << image_id <<endl;
        return;
    }

    if (NULL == cluster_name)
    {
        cout << "cluster name null" << endl;
        return;
    }

    ImagePrepareMsg msg;
    msg._cluster_name = cluster_name;
    msg._image_id = image_id;

    MessageUnit temp_mu(TempUnitName("DbgPrepareImage"));
    temp_mu.Register();
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), EV_IMAGE_PREPARE_REQ,0,0);
    STATUS ret = temp_mu.Send(msg, option);
    if (SUCCESS != ret)
    {
        cerr << "fail to send image prepare request! ret = " << ret << endl;
    }
    else
    {
        cout << "image prepare request send success" << endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        ImagePrepareAckMsg  ack;
        ack.deserialize(message.data);
        ack.Print();
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }

}
DEFINE_DEBUG_FUNC(DbgPrepareImage);

void DbgGetImageIdByLocation(const char* location)
{
    if (!location)
    {
        cerr << "invalid args!" << endl;
        return;
    }

    ImagePool* ipool = ImagePool::GetInstance();
    if (!ipool)
    {
        cerr << "ImagePool not ready!" << endl;
        return;
    }

    int64 image_id = ipool->GetImageIdByLocation(location);
    if (INVALID_OID == image_id)
    {
        cerr << "error! image_id = -1" << endl;
        return;
    }

    cout<<"image_id is :"<<image_id<<endl;
    return;
}
DEFINE_DEBUG_FUNC(DbgGetImageIdByLocation);


void DbgGetVmsByLocation(const char* location)
{
    if (!location)
    {
        cerr << "invalid args!" << endl;
        return;
    }

    ImagePool* ipool = ImagePool::GetInstance();
    if (!ipool)
    {
        cerr << "ImagePool not ready!" << endl;
        return;
    }

    int vms = ipool->GetVmsByLocation(location);
    if (0 == vms)
    {
        cerr << "vms is 0" << endl;
        return;
    }

    cout<<"vms is :"<<vms<<endl;
    return;
}
DEFINE_DEBUG_FUNC(DbgGetVmsByLocation);

VOID DbgShowImage(int64 image_id)
{
    Image image;
    int ret = (ImagePool::GetInstance())->GetImageByImageId(image_id, image);
    if (ERROR == ret)
    {
        cout << "Image (" << image_id << ") is not exist !" << endl;
        return;
    }
    image.Print();
    return;
}
DEFINE_DEBUG_FUNC(DbgShowImage);

}


