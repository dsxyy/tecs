/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�image_db.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ�������ӳ�����ݿ����
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��03��09��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/03/09
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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
    int64   _image_id; //ӳ���ļ�ID
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

