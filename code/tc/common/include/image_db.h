/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：image_db.h
* 文件标识：见配置管理计划书
* 内容摘要：虚拟机映像数据库管理
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年03月09日
*
* 修改记录1：
*     修改日期：2012/03/09
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#ifndef TECS_IMAGE_DB_H
#define TECS_IMAGE_DB_H
#include "sky.h"
#include "tecs_pub.h"
#include "image_store.h"
#include "image_manager_with_api_method.h"

namespace zte_tecs
{
class ImageMetadata:public Serializable
{
public:
    int64   _image_id; //映像文件ID
    int64   _uid;
    string  _name;
    string  _type;
    string  _arch;
    bool    _is_public;
    int64   _size;
    int64   _disk_size;
    FileUrl _url;
    string  _bus;
    string  _os_type;
    string  _disk_format; 

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageMetadata);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_type);
        WRITE_VALUE(_arch);
        WRITE_VALUE(_is_public);
        WRITE_VALUE(_size);
        WRITE_VALUE(_disk_size);
        WRITE_OBJECT(_url);
        WRITE_VALUE(_bus);
        WRITE_VALUE(_os_type);
        WRITE_VALUE(_disk_format);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageMetadata);
        READ_VALUE(_image_id);
        READ_VALUE(_uid);
        READ_VALUE(_name);
        READ_VALUE(_type);
        READ_VALUE(_arch);
        READ_VALUE(_is_public);
        READ_VALUE(_size);
        READ_VALUE(_disk_size);
        READ_OBJECT(_url);
        READ_VALUE(_bus);
        READ_VALUE(_os_type);
        READ_VALUE(_disk_format);
        DESERIALIZE_END();
    };
};

class ImageMetadataReq: public Serializable
{
public:
    int64 _image_id;
    int64 _uid;

    ImageMetadataReq()
    {
        _image_id = INVALID_OID;
        _uid = INVALID_OID;
    };
    
    ImageMetadataReq(int64 iid, int64 uid)
    {
        _image_id = iid;
        _uid = uid;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageMetadataReq);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_uid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageMetadataReq);
        READ_VALUE(_image_id);
        READ_VALUE(_uid);
        DESERIALIZE_END();
    };
};

class ImageMetadataAck: public Serializable
{
public:
    ImageMetadata _image;
    STATUS        _result;

    ImageMetadataAck()
    {

    };
    
    ImageMetadataAck(const ImageMetadata& image, STATUS result):_image(image)
    {
        _result = result;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageMetadataAck);
        WRITE_OBJECT(_image);
        WRITE_VALUE(_result);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageMetadataAck);
        READ_OBJECT(_image);
        READ_VALUE(_result);
        DESERIALIZE_END();
    };
};

class ImageUploadAck: public Serializable
{
public:
    UploadUrl   _url;
    STATUS      _result;

    ImageUploadAck()
    {

    };
    
    ImageUploadAck(const UploadUrl& image, STATUS result):_url(image)
    {
        _result = result;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageMetadataAck);
        WRITE_OBJECT(_url);
        WRITE_VALUE(_result);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageMetadataAck);
        READ_OBJECT(_url);
        READ_VALUE(_result);
        DESERIALIZE_END();
    };
};

STATUS GetImageMetadata(int64 iid, int64 uid, ImageMetadata &data);
STATUS DeployImage(const vector<ImageDeployMsg> &msg);

}
#endif

