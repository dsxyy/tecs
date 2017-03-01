/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_url.h
* 文件标识：
* 内容摘要：image url相关操作结构头文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2013年3月12日
*
* 修改记录1：
*     修改日期：2013/3/12
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
******************************************************************************/
#ifndef TECS_IMAGE_URL_H
#define TECS_IMAGE_URL_H
#include "image_common.h"

namespace zte_tecs
{
/**
@brief 功能描述:文件url结构\n
@li @b 其它说明: 无
*/
class FileUrl:public Serializable
{
public:
    string path;
    string checksum;
    string access_type;
    string access_option;
    
    FileUrl()    {};
    
    FileUrl(const string& _path,const string& _type,const string& _option)
    :path(_path),access_type(_type),access_option(_option)
    {};
    
    bool valid() const
    {
        return true;
    }

    void clear()
    {
        path.clear();
        checksum.clear();
        access_type.clear();
        access_option.clear();
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(FileUrl);
        WRITE_VALUE(path);
        WRITE_VALUE(access_type);
        WRITE_VALUE(access_option);
        WRITE_VALUE(checksum);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(FileUrl);
        READ_VALUE(path);
        READ_VALUE(access_type);
        READ_VALUE(access_option);
        READ_VALUE(checksum);
        DESERIALIZE_END();
    }
};
/**
@brief 功能描述: 镜像上传url结构\n
@li @b 其它说明: 无
*/
class UploadUrl:public WorkAck
{
public:
    string ip;
    int    port;
    string path;
    string user;
    string passwd;
    string access_type;
    string access_option;
    
    UploadUrl()    {};
    
    UploadUrl(const string& _path,const string& _type,const string& _option)
    :path(_path),access_type(_type),access_option(_option)
    {};
    
    bool valid() const
    {
        return true;
    }

    void clear()
    {
        ip.clear();
        port = 0;
        path.clear();
        user.clear();
        passwd.clear();
        access_type.clear();
        access_option.clear();
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(UploadUrl);
        WRITE_VALUE(ip);
        WRITE_VALUE(port);
        WRITE_VALUE(path);
        WRITE_VALUE(user);
        WRITE_VALUE(passwd);
        WRITE_VALUE(access_type);
        WRITE_VALUE(access_option);
        SERIALIZE_PARENT_END(WorkAck);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(UploadUrl);
        READ_VALUE(ip);
        READ_VALUE(port);
        READ_VALUE(path);
        READ_VALUE(user);
        READ_VALUE(passwd);
        READ_VALUE(access_type);
        READ_VALUE(access_option);
        DESERIALIZE_PARENT_END(WorkAck);
    }
};

/**
@brief 功能描述: 镜像部署请求结构\n
@li @b 其它说明: 无
*/
class DeployBaseImageReq:public WorkReq
{
public:
    DeployBaseImageReq()    
    {
        _base_id = INVALID_OID;
        _image_id = INVALID_FILEID;
        _size = 0;
        _disk_size = 0;
    };
 
    DeployBaseImageReq(const string& actionid,
                              const string& base_uuid, 
                              const string& cluster_name, 
                              int64 base_id, 
                              int64 image_id, 
                              int64 size, 
                              int64 disk_size,
                              const FileUrl& url,
                              const ImageStoreOption& option)
    {
        action_id = actionid;
        _base_uuid = base_uuid;
        _cluster_name = cluster_name;
        _base_id = base_id; //可作为dist_id用
        _image_id = image_id;
        _size = size;
        _disk_size = disk_size;
        _url = url;
        _option = option;
    };

    ~DeployBaseImageReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeployBaseImageReq);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_base_id);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_size);
        WRITE_VALUE(_disk_size);
        WRITE_OBJECT(_url);
        WRITE_OBJECT(_option);
        SERIALIZE_PARENT_END(WorkReq);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeployBaseImageReq);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_base_id);
        READ_VALUE(_image_id);
        READ_VALUE(_size);
        READ_VALUE(_disk_size);
        READ_OBJECT(_url);
        READ_OBJECT(_option);
        DESERIALIZE_PARENT_END(WorkReq);
    }

    string _base_uuid;
    string _cluster_name;
    int64  _base_id; //可作为dist_id用
    int64  _image_id;
    int64  _size;
    int64  _disk_size;
    FileUrl _url;
    ImageStoreOption _option;
};
/**
@brief 功能描述: 镜像部署应答结构\n
@li @b 其它说明: 无
*/
class DeployBaseImageAck:public WorkAck
{
public:
    DeployBaseImageAck(){};

    DeployBaseImageAck(const string& action_id, const string& base_uuid, const string& cluster_name):
    WorkAck(action_id), _base_uuid(base_uuid), _cluster_name(cluster_name)
    {
    };


    ~DeployBaseImageAck(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(DeployBaseImageAck);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(DeployBaseImageAck);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    }

    string _base_uuid;
    string _cluster_name;
    string _workflow_id;
};
/**
@brief 功能描述: 镜像撤销请求结构\n
@li @b 其它说明: 无
*/
class CancelBaseImageReq : public WorkReq
{
public:
    CancelBaseImageReq()    
    {
        _base_id = INVALID_OID;
        _image_id = INVALID_FILEID;
        _size = 0;
        _disk_size = 0;
    };
    
    CancelBaseImageReq(const string& actionid,
                              const string& base_uuid, 
                              const string& cluster_name, 
                              int64 base_id, 
                              int64 image_id, 
                              int64 size, 
                              int64 disk_size, 
                              const FileUrl& url,
                              const ImageStoreOption& option)

    {
        action_id = actionid;
        _base_uuid = base_uuid;
        _cluster_name = cluster_name;
        _base_id = base_id; //可作为dist_id用
        _image_id = image_id;
        _size = size;
        _disk_size = disk_size;
        _url = url;
        _option = option;

    };

    ~CancelBaseImageReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(CancelBaseImageReq);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_base_id);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_size);
        WRITE_VALUE(_disk_size);
        WRITE_OBJECT(_url);
        WRITE_OBJECT(_option);
        SERIALIZE_PARENT_END(WorkReq);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CancelBaseImageReq);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_base_id);
        READ_VALUE(_image_id);
        READ_VALUE(_size);
        READ_VALUE(_disk_size);
        READ_OBJECT(_url);
        READ_OBJECT(_option);
        DESERIALIZE_PARENT_END(WorkReq);
    }
    
    string _base_uuid;
    string _cluster_name;
    int64  _base_id;
    int64  _image_id;
    int64  _size;
    int64  _disk_size;
    FileUrl _url;
    ImageStoreOption _option;
};
/**
@brief 功能描述: 镜像撤销应答结构\n
@li @b 其它说明: 无
*/
class CancelBaseImageAck:public WorkAck
{
public:
    CancelBaseImageAck(){};
    
    CancelBaseImageAck(const string& action_id, const string& base_uuid, const string& cluster_name):
    WorkAck(action_id), _base_uuid(base_uuid), _cluster_name(cluster_name)
    {
    };

    ~CancelBaseImageAck(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(CancelBaseImageAck);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CancelBaseImageAck);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    }

    string _base_uuid;
    string _cluster_name;
    string _workflow_id;
};
#if 0
class ImageUrlPrepareReq:public WorkReq
{
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageUrlPrepareReq);
        WRITE_VALUE(_vid); 
        WRITE_VALUE(_image_id); 
        WRITE_VALUE(_position); 
        WRITE_VALUE(_request_id); 
        WRITE_VALUE(_cluster_name); 
        WRITE_VALUE(_iscsiname); 
        WRITE_VALUE(_userip); 
        WRITE_OBJECT(_url);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageUrlPrepareReq);
        READ_VALUE(_vid); 
        READ_VALUE(_image_id); 
        READ_VALUE(_position); 
        READ_VALUE(_request_id); 
        READ_VALUE(_cluster_name); 
        READ_VALUE(_iscsiname); 
        READ_VALUE(_userip); 
        READ_OBJECT(_url);
        DESERIALIZE_PARENT_END(WorkReq);
    };
#if 0
    ImageUrlPrepareReq(int64 vid, const VmDiskConfig &disk, const string &cluster_name, const string &iscsiname, const string &userip)
                        : _request_id(disk._request_id), _cluster_name(cluster_name), _iscsiname(iscsiname), _userip(userip)
    {
        _vid = vid;
        _image_id = disk._id;
    }
#endif    
    int64 _vid;
    int64 _image_id;
    int   _position;
    string _request_id;
    string _cluster_name;
    string _iscsiname;
    string _userip;
    FileUrl _url;
};

class ImageUrlPrepareAck:public WorkAck
{
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageUrlPrepareAck);
        WRITE_VALUE(_request_id); 
		WRITE_OBJECT(_url);
		WRITE_OBJECT(_share_url);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageUrlPrepareAck);
        READ_VALUE(_request_id);
		READ_OBJECT(_url);
		READ_OBJECT(_share_url);
        DESERIALIZE_PARENT_END(WorkAck);
    };
	
    string _request_id;
    FileUrl _url;
    FileUrl _share_url;
};
    
class ImageUrlReleaseReq:public WorkReq
{
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageUrlReleaseReq);
        WRITE_VALUE(_vid); 
        WRITE_VALUE(_image_id); 
        WRITE_VALUE(_position); 
        WRITE_VALUE(_request_id); 
        WRITE_VALUE(_cluster_name); 
        WRITE_VALUE(_iscsiname); 
        WRITE_VALUE(_userip); 
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageUrlReleaseReq);
        READ_VALUE(_vid); 
        READ_VALUE(_image_id); 
        READ_VALUE(_position); 
        READ_VALUE(_request_id); 
        READ_VALUE(_cluster_name); 
        READ_VALUE(_iscsiname); 
        READ_VALUE(_userip); 
        DESERIALIZE_PARENT_END(WorkReq);
    };
    int64 _vid;
    int64 _image_id;
    int   _position;
    string _request_id;
    string _cluster_name;
    string _iscsiname;
    string _userip;
};

class ImageUrlReleaseAck:public WorkAck
{
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageUrlReleaseAck);
        WRITE_VALUE(_request_id); 
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageUrlReleaseAck);
        READ_VALUE(_request_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
    string _request_id;
};
#endif
class ImageUrlPrepareReq : public WorkReq
{
public:
    ImageUrlPrepareReq()    
    {
        _image_id = INVALID_FILEID;
        _position = IMAGE_POSITION_LOCAL;
    };
    
    ImageUrlPrepareReq(const string& actionid,
                              const string& request_id, 
                              const string& cluster_name, 
                              int64 image_id, 
                              int position, 
                              const FileUrl& url,
                              const ImageStoreOption& option)

    {
        action_id = actionid;
        _request_id = request_id;
        _cluster_name = cluster_name;
        _image_id = image_id;
        _position = position;
        _url = url;
        _option = option;

    };

    ~ImageUrlPrepareReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageUrlPrepareReq);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_position);
        WRITE_OBJECT(_url);
        WRITE_OBJECT(_option);
        SERIALIZE_PARENT_END(WorkReq);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageUrlPrepareReq);
        READ_VALUE(_request_id);
        READ_VALUE(_cluster_name);
        READ_VALUE(_image_id);
        READ_VALUE(_position);
        READ_OBJECT(_url);
        READ_OBJECT(_option);
        DESERIALIZE_PARENT_END(WorkReq);
    }
    
    string _request_id;
    string _cluster_name;
    int64 _image_id;
    int _position;
    FileUrl _url;
    ImageStoreOption _option;
};

class ImageUrlPrepareAck : public WorkAck
{
public:
    ImageUrlPrepareAck()    
    {
        _size = 0;
    };

    ImageUrlPrepareAck(const string& actionid,
                            const string& request_id, 
                            const FileUrl& url)

    {
        action_id = actionid;
        _request_id = request_id;
        _url = url;
        _size = 0;
    };

    ~ImageUrlPrepareAck(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageUrlPrepareAck);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_base_uuid_vec);
        WRITE_OBJECT(_url);
        WRITE_VALUE(_size);
        SERIALIZE_PARENT_END(WorkAck);
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageUrlPrepareAck);
        READ_VALUE(_request_id);
        READ_VALUE(_base_uuid_vec);
        READ_OBJECT(_url);
        READ_VALUE(_size);
        DESERIALIZE_PARENT_END(WorkAck);
    }
    
    string _request_id;
    vector<string> _base_uuid_vec;  
    FileUrl _url;
    int64 _size;
};

}
#endif


