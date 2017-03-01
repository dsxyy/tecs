/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_manager_with_api_method.h
* 文件标识：见配置管理计划书
* 内容摘要：image_manager 与 api_method 之间的消息体结构定义
* 当前版本：1.0
* 作    者：陈文文
* 完成日期：2011年9月5日
*
* 修改记录1：
*     修改日期：2011/9/5
*     版 本 号：V1.0
*     修 改 人：陈文文
*     修改内容：创建
******************************************************************************/
#ifndef TC_IMAGE_MANAGER_WITH_API_METHOD_H
#define TC_IMAGE_MANAGER_WITH_API_METHOD_H

#include "sky.h"
#include "workflow.h"
//#include "tecs_pub.h"
#include "workflow.h"


using namespace std;
namespace zte_tecs 
{


/**
@brief 功能描述: 注册映像的消息类\n
@li @b 其它说明: 无
*/
class ImageRegisterMsg:public WorkReq
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    ImageRegisterMsg() {};    

    ImageRegisterMsg(int64 uid, 
                            int64 image_id, 
                            const string &name,
                            const string &type,
                            const string &arch,
                            bool  is_public,
                            bool  is_avaliable)
    { 
        _uid      = uid;
        _image_id = image_id;
        _name     = name;
        _type     = type;
        _arch     = arch;
        _is_public = is_public;
        _is_avaliable  = is_avaliable;
    };
    
    ~ImageRegisterMsg() {};
    

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageRegisterMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_image_id);
        WRITE_STRING(_name);
        WRITE_STRING(_type);
        WRITE_STRING(_arch);
        WRITE_STRING(_description);
        WRITE_DIGIT(_is_public);
        WRITE_DIGIT(_is_avaliable);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageRegisterMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_image_id);
        READ_STRING(_name);
        READ_STRING(_type);
        READ_STRING(_arch);
        READ_STRING(_description);
        READ_DIGIT(_is_public);
        READ_DIGIT(_is_avaliable);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageRegisterMsg : " <<
                     "_uid = "   << _uid << 
                     ", _image_id ="<< _image_id << 
                     ", _name ="<<  _name << 
                     ", _type ="<<  _type << 
                     ", _arch ="<<  _arch << 
                     ", _description ="<<  _description << 
                     ", _is_public ="<< _is_public << endl;         
    }; 

    int64     _uid;  
    int64      _image_id;
    string     _name;
    string     _type;
    string     _arch;
    string     _description;
    bool       _is_public;  
    bool       _is_avaliable; 
    
};



/**
@brief 功能描述: 注销映像的消息类\n
@li @b 其它说明: 无
*/
class ImageDeregisterMsg :public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    ImageDeregisterMsg():_uid(-1), _image_id(-1) {};
    
    ImageDeregisterMsg(const ImageDeregisterMsg  &msg)
    {
         _uid     = msg._uid;
        _image_id = msg._image_id;
    };
    
    ImageDeregisterMsg(int64 uid, int64 image_id)
    { 
        _uid      = uid;
        _image_id = image_id;
  
    };
    
    ~ImageDeregisterMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageDeregisterMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_image_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageDeregisterMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_image_id);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageDeregisterMsg : " <<
                     "_image_id = "   << _image_id << 
                     "_uid = "   << _uid <<endl;        
    }
    int64       _uid;
    int64       _image_id;

};


/**
@brief 功能描述: 删除映像的消息类\n
@li @b 其它说明: 无
*/
class ImageDeleteMsg :public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    ImageDeleteMsg():_uid(-1), _image_id(-1) {};

    ImageDeleteMsg(const ImageDeregisterMsg  &msg)
    {
         _uid     = msg._uid;
        _image_id = msg._image_id;
    };

    ImageDeleteMsg(int64 uid, int64 image_id)
    { 
        _uid      = uid;
        _image_id = image_id;
  
    };
    
    ~ImageDeleteMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageDeleteMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_image_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageDeleteMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_image_id);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageDeleteMsg : " <<
                     "_image_id = "   << _image_id << 
                     "_uid = "   << _uid <<endl;        
    }
    int64       _uid;
    int64       _image_id;
};

/**
@brief 功能描述: 修改映像属性的消息类\n
@li @b 其它说明: 无
*/
class ImageModifyMsg:public Serializable
{
public:
    ImageModifyMsg():_uid(-1), _image_id(-1){};
    
    ImageModifyMsg(int64 uid, 
                          int64   image_id,
                          string  name,
                          string  type,
                          string  arch,
                          string  bus,
                          string  os_type,
                          const string& container_format)
    { 
        _uid       = uid;
        _image_id  = image_id;
        _name      = name;
        _type      = type;
        _arch      = arch; 
        _bus       = bus;
        _os_type   = os_type;
        _container_format = container_format;
    };
    
    ~ImageModifyMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageModifyMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_image_id);
        WRITE_STRING(_name);
        WRITE_STRING(_type);
        WRITE_STRING(_arch);
        WRITE_STRING(_bus);
        WRITE_STRING(_description);
        WRITE_STRING(_os_type);
        WRITE_STRING(_container_format);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageModifyMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_image_id);
        READ_STRING(_name);
        READ_STRING(_type);
        READ_STRING(_arch);
        READ_STRING(_bus);
        READ_STRING(_description);
        READ_STRING(_os_type);
        READ_STRING(_container_format);
        DESERIALIZE_END();
    };

    void Print()
    {
       
        cout << "ImageModifyMsg : " <<
                     "_uid = "   << _uid << 
                     "_image_id" << _image_id<<
                     "_name" << _name << 
                     "_type" <<_type <<
                     "_description" <<_description <<
                     "_bus" <<_bus <<
                     "_arch" << _arch<<
                     "_os_type"<<_os_type<<
                     "_container_format"<<_container_format<<endl ;
 
    }   
    int64              _uid;
    int64              _image_id;
    string             _name;
    string             _type;
    string             _arch; 
    string             _bus;
    string             _description;
    string             _os_type;
    string             _container_format;
};


/**
@brief 功能描述: 设置映像使能属性的消息类\n
@li @b 其它说明: 无
*/
class ImageEnableMsg:public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

        
    ImageEnableMsg() {};
    
    ImageEnableMsg(const ImageEnableMsg  &msg)
    {
        ;
    };
    
    ImageEnableMsg(int64 uid,  
                          int64   image_id,
                          bool  enable_flag)
    { 
           _uid    = uid;
        _enable_flag = enable_flag;
        _image_id = image_id;
    };
    
    ~ImageEnableMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageEnableMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_image_id);
        WRITE_DIGIT(_enable_flag);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageEnableMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_image_id);
        READ_DIGIT(_enable_flag);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageEnableMsg : " <<
                     "_uid = "   << _uid << 
                     "_image_id = "   << _image_id <<
                     "_enable_flag = " << _enable_flag <<endl;        
    };
   
    int64                _uid;
    int64                _image_id;
    bool               _enable_flag;
    
};



/**
@brief 功能描述: 设置映像public属性的消息类\n
@li @b 其它说明: 无
*/
class ImagePublishMsg:public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

        
    ImagePublishMsg() {};
    
    ImagePublishMsg(const ImagePublishMsg  &msg)
    { 
        _uid    = msg._uid;
        _publish_flag = msg._publish_flag;
        _image_id = msg._image_id;

    };
    
    ImagePublishMsg(int64 uid, 
                          int64   image_id,
                          bool  publish_flag)
    {
        _uid    = uid;
        _publish_flag = publish_flag;
        _image_id = image_id;
    };
    
    ~ImagePublishMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImagePublishMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_image_id);
        WRITE_DIGIT(_publish_flag);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImagePublishMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_image_id);
        READ_DIGIT(_publish_flag);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImagePublishMsg : " <<
                     "_uid = "   << _uid << 
                     "_image_id = "   << _image_id <<
                     "_publish_flag = " << _publish_flag <<endl;

   };
   
    int64                _uid;
    int64                _image_id;
    bool               _publish_flag;
};



/**
@brief 功能描述: 查询映像属性的消息类\n
@li @b 其它说明: 无
*/
class ImageQueryMsg :public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    enum ImageQueryType
    {
        USER_REGISTER   = -4, /** < User's register Images  */
        USER_VISIBLE    = -3, /** < User's Images AND public images belonging to any user */
        USER_PUBLIC     = -2, /** < public images belonging to any user */
        APPOINTED       = 0 ,  /** < Image ID, user's Image */
        APPOINT_IMAGE_ID =1,
        QUERY_ALL_IMAGE = 2 ,   /** < 查询所有image，仅管理员有效 */
        APPOINT_IMAGE_NAME = 3 /** < 根据image name，查询image*/
    };
    
    ImageQueryMsg() {};
    
    ImageQueryMsg(int64  uid, int query_type)
    { 
        _uid    = uid;
        _query_type = query_type;
    };
    
    ~ImageQueryMsg() {};    

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageQueryMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_query_uid);
        WRITE_DIGIT(_start_index);
        WRITE_DIGIT(_query_count);
        WRITE_DIGIT(_query_type);
        WRITE_DIGIT(_query_image_id);
        WRITE_DIGIT(_query_filename);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageQueryMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_query_uid);
        READ_DIGIT(_start_index);
        READ_DIGIT(_query_count);
        READ_DIGIT(_query_type);
        READ_DIGIT(_query_image_id);
        READ_DIGIT(_query_filename);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageQueryMsg : " <<
                     "_uid = "   << _uid << 
                     "_query_uid = "   << _query_uid << 
                     "_start_index = "   << _start_index <<
                     "_query_count = "   << _query_count <<
                     "_query_image_id = "   << _query_image_id <<
                     "_query_type = "   << _query_type  <<
                     "_query_filename = "   << _query_filename  <<endl;
        
    };

    int64      _uid;
    int64      _start_index;
    int64      _query_count;
    int          _query_type;
    int64      _query_uid;
    int64      _query_image_id; 
    string     _query_filename;
};

class ImageFtpSvrInfoGetMsg :public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    
    ImageFtpSvrInfoGetMsg() {};
    
    ImageFtpSvrInfoGetMsg(int64  uid):_uid(uid)
    {
    };
    
    ~ImageFtpSvrInfoGetMsg() {};    

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageFtpSvrInfoGetMsg);
        WRITE_DIGIT(_uid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageFtpSvrInfoGetMsg);
        READ_DIGIT(_uid);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageFtpSvrInfoGetMsg : " <<
                "_uid = "<< _uid <<endl;
        
    };

    int64      _uid;
};


/*****************************************************************************/
/* 映像信息类，该类属于消息类，由 Image Manger向Agent转发过去的 */
/*****************************************************************************/
class ImageInfo : public Serializable
{
public:
    ImageInfo() 
    {
       _image_id = -1;
       _owner    = -1;
       _is_public = false;
       _is_enabled = false;
       _size     = 0;
       _running_vms = 0;
       _ref_count_by_private_vt = 0;
       _ref_count_by_public_vt = 0;
    };
    
    ~ImageInfo() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(ImageInfo);
        WRITE_DIGIT(_oid);
        WRITE_DIGIT(_image_id);
        WRITE_DIGIT(_owner);
        WRITE_STRING(_name);
        WRITE_STRING(_file);
        WRITE_STRING(_md5sum);
        WRITE_STRING(_type);
        WRITE_DIGIT(_is_public);
        WRITE_STRING(_register_time);        
        WRITE_DIGIT(_is_enabled);
        WRITE_DIGIT(_size);
        WRITE_DIGIT(_running_vms);
	WRITE_DIGIT(_ref_count_by_private_vt);
	WRITE_DIGIT(_ref_count_by_public_vt);
        WRITE_STRING(_os_type);
        WRITE_STRING(_os_version);
        WRITE_STRING(_os_arch);
        WRITE_STRING(_bus);
        WRITE_STRING(_description);
        WRITE_DIGIT(_disk_size);
        WRITE_STRING(_disk_format);
        WRITE_STRING(_container_format);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(ImageInfo);
        READ_DIGIT(_oid);
        READ_DIGIT(_image_id);
        READ_DIGIT(_owner);
        READ_STRING(_name);
        READ_STRING(_file);
        READ_STRING(_md5sum);
        READ_STRING(_type);
        READ_DIGIT(_is_public);
        READ_STRING(_register_time);        
        READ_DIGIT(_is_enabled);
        READ_DIGIT(_size);
        READ_DIGIT(_running_vms);
	READ_DIGIT(_ref_count_by_private_vt);
	READ_DIGIT(_ref_count_by_public_vt);
        READ_STRING(_os_type);
        READ_STRING(_os_version);
        READ_STRING(_os_arch);
        READ_STRING(_bus);
        READ_STRING(_description);
        READ_DIGIT(_disk_size);
        READ_STRING(_disk_format);
        READ_STRING(_container_format);
        DESERIALIZE_END();    
    };

    int64             _oid;
    int64             _image_id;  //映像文件ID
    int64             _owner;
    string            _name;
    //_file: 映像实际对应的文件名称，不带路径，由于目前映像采用ftp上传，测试用例中需要根据文件名查询映像id
    string            _file; 
    string            _md5sum; 
    string            _type;
    bool              _is_public;
    string            _register_time;
    bool              _is_enabled;
    int64             _size;
    int               _running_vms;
    int               _ref_count_by_private_vt;
    int               _ref_count_by_public_vt;
    string            _os_type;
    string            _os_version;
    string            _os_arch;
    string            _bus;
    string            _description;
    int64             _disk_size;
    string            _disk_format;
    string            _container_format;
};


/**
@brief 功能描述: 查看映像属性的消息类\n
@li @b 其它说明: 无
*/
class ImageQueryAckMsg:public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ImageQueryAckMsg(){};
    
    ImageQueryAckMsg(int64 uid, int ret_code, string err_str) 
    {
        _uid        = uid;
        _ret_code   = ret_code;
        _err_str    = err_str;
        _next_index = -1;
        _max_count = 0;
    };
    

    ~ImageQueryAckMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageQueryAckMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_next_index);
        WRITE_DIGIT(_max_count);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_err_str);
        WRITE_OBJECT_VECTOR(_image_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageQueryAckMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_next_index);
        READ_DIGIT(_max_count);
        READ_DIGIT(_ret_code);
        READ_STRING(_err_str);
        READ_OBJECT_VECTOR(_image_info);
        DESERIALIZE_END();
    };

    int64     _uid;
    int64     _next_index;
    int64     _max_count;
    int       _ret_code;
    string    _err_str;
    vector<ImageInfo>  _image_info;     
};



class ImageFtpSvrInfo : public Serializable
{
public:
    ImageFtpSvrInfo(int port,string ip):_ftp_svr_port(port),_ftp_svr_ip(ip)
    {
    };

    ImageFtpSvrInfo(){};
	
    ~ImageFtpSvrInfo() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(ImageFtpSvrInfo);
        WRITE_DIGIT(_ftp_svr_port);
        WRITE_DIGIT(_ftp_svr_ip);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(ImageFtpSvrInfo);
        READ_DIGIT(_ftp_svr_port);
        READ_DIGIT(_ftp_svr_ip);
        DESERIALIZE_END();    
    };

    int      _ftp_svr_port;
    string   _ftp_svr_ip;

};


class ImageFtpSvrInfoAckMsg:public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    ImageFtpSvrInfoAckMsg(){};
    
    ImageFtpSvrInfoAckMsg(int64 uid, int ret_code = 0, string err_str = ""):_image_ftp_svr_info(0,"")
    {
        _uid        = uid;
        _ret_code   = ret_code;
        _err_str    = err_str;
    };   

    ~ImageFtpSvrInfoAckMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageQueryAckMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_err_str);
        WRITE_OBJECT(_image_ftp_svr_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageQueryAckMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_ret_code);
        READ_STRING(_err_str);
        READ_OBJECT(_image_ftp_svr_info);
        DESERIALIZE_END();
    };

    int64     _uid;
    int       _ret_code;
    string    _err_str;
    ImageFtpSvrInfo  _image_ftp_svr_info;     
};


/**
@brief 功能描述: 修改映像属性的消息类\n
@li @b 其它说明: 无
*/
class ImageOperateAckMsg:public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    ImageOperateAckMsg() {};
    
    ImageOperateAckMsg(int64          uid, 
                                // const string &stamp,
                                 int          ret_code, 
                                 const string &result)
    {
        _uid     = uid;
        _ret_code = ret_code;
        _result  = result;   
       // _stamp  = stamp;
    };

    
    ~ImageOperateAckMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageOperateAckMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_ret_code);
        WRITE_STRING(_result);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageOperateAckMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_ret_code);
        READ_STRING(_result);
        DESERIALIZE_END();
    };
    void Print()
    {
        cout << "ImageOperateAckMsg : " <<
                     "_uid = "   << _uid << 
                 //    "_stamp = "   << _stamp << 
                     "_ret_code = "   << _ret_code  <<
                     "_result = "  << _result << endl;       
    };

    int64      _uid;
//    string   _stamp;
    int      _ret_code;
    string   _result;
     
};

/**
@brief 功能描述: 部署镜像的消息类\n
@li @b 其它说明: 无
*/
class ImageDeployMsg:public WorkReq
{
public:
    ImageDeployMsg():_uid(INVALID_UID), _image_id(INVALID_FILEID){};
    
    ImageDeployMsg(int64 uid, 
                         int64 image_id, 
                         const string &cluster_name, 
                         const string &base_uuid,
                         const string &user_name ):
                         _uid(uid), _image_id(image_id), _cluster_name(cluster_name), _base_uuid(base_uuid), _user_name(user_name)
    { 
    };
    
    ~ImageDeployMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageDeployMsg);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_user_name);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageDeployMsg);
        READ_VALUE(_uid);
        READ_VALUE(_image_id);
        READ_VALUE(_cluster_name);
        READ_VALUE(_base_uuid);
        READ_VALUE(_user_name);
        DESERIALIZE_PARENT_END(WorkReq);
    };

    void Print()
    {
        cout << "ImageDeployMsg : " 
             << "action_id = " << action_id 
             << ",_uid = "   << _uid 
             << ",_image_id = " << _image_id
             << ",_cluster_name = " << _cluster_name 
             << ",_base_uuid = " << _base_uuid <<  endl ;
    }   
       
    int64  _uid;
    int64  _image_id;
    string _cluster_name;
    string _base_uuid;
    string _user_name;
};

/**
@brief 功能描述: 部署镜像的应答消息类\n
@li @b 其它说明: 无
*/
class ImageDeployAckMsg:public WorkAck
{
public:
    ImageDeployAckMsg():_uid(INVALID_UID){};

    ImageDeployAckMsg(const string& action_id, int64 uid, const string& base_uuid, const string& cluster_name):
    WorkAck(action_id), _uid(uid), _base_uuid(base_uuid), _cluster_name(cluster_name)        
    { 
    };
    
    ~ImageDeployAckMsg(){};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageDeployAckMsg);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_workflow_id);
        WRITE_VALUE(_workflow_engine);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageDeployAckMsg);
        READ_VALUE(_uid);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_workflow_id);
        READ_VALUE(_workflow_engine);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    void Print()
    {
        cout << "ImageDeployAckMsg : " 
             << "action_id = " << action_id 
             << ",state = " << state 
             << ",progress = " << progress
             << ",detail = " << detail 
             << ",_uid = " << _uid 
             << ",_base_uuid = " << _base_uuid 
             << ",_cluster_name = " << _cluster_name 
             << ",_workflow_id = " << _workflow_id  
             << ",_workflow_engine = " << _workflow_engine << endl ;
    }

    int64 _uid;
    string _base_uuid;
    string _cluster_name;
    string _workflow_id;
    string _workflow_engine;
};

/**
@brief 功能描述: 准备镜像的消息类\n
@li @b 其它说明: 无
*/
class ImagePrepareMsg:public WorkReq
{
public:
    ImagePrepareMsg():_uid(INVALID_UID), _image_id(INVALID_FILEID){};

    ImagePrepareMsg(const string id_of_action ,
                        int64 uid, 
                        int64 image_id, 
                        const string& cluster_name):
                            WorkReq(id_of_action), 
                            _uid(uid),
                            _image_id(image_id),
                            _cluster_name(cluster_name)    
    { 
    };
    ImagePrepareMsg(int64 uid, int64 image_id, const string& cluster_name):
    _uid(uid), _image_id(image_id), _cluster_name(cluster_name)    
    { 
    };
    
    ~ImagePrepareMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImagePrepareMsg);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_cluster_name);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImagePrepareMsg);
        READ_VALUE(_uid);
        READ_VALUE(_image_id);
        READ_VALUE(_cluster_name);
        DESERIALIZE_PARENT_END(WorkReq);
    };

    void Print()
    {
        cout << "ImagePrepareMsg: " 
             << "action_id = " << action_id 
             << ",_uid = "   << _uid 
             << ",_image_id = " << _image_id
             << ",_cluster_name = " << _cluster_name <<  endl ;
    }   
    
    int64  _uid;
    int64  _image_id;
    string _cluster_name;
};

/**
@brief 功能描述: 准备镜像的应答消息类\n
@li @b 其它说明: 无
*/
class ImagePrepareAckMsg:public WorkAck
{
public:
    ImagePrepareAckMsg():_uid(INVALID_UID), _image_id(INVALID_FILEID){};
    
    ImagePrepareAckMsg(const string& action_id, int64 uid, int64 image_id, const string& cluster_name):
    WorkAck(action_id), _uid(uid), _image_id(image_id), _cluster_name(cluster_name)    
    { 
    };
    
    ~ImagePrepareAckMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImagePrepareAckMsg);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_cluster_name);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImagePrepareAckMsg);
        READ_VALUE(_uid);
        READ_VALUE(_image_id);
        READ_VALUE(_cluster_name);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    void Print()
    {
       
        cout << "ImagePrepareAckMsg: " 
             << "action_id = " << action_id 
             << ",state = " << state 
             << ",progress = " << progress
             << ",detail = " << detail 
             << ",_uid = "   << _uid 
             << ",_image_id = " << _image_id
             << ",_cluster_name = " << _cluster_name <<  endl ;
 
    }   
    int64  _uid;
    int64  _image_id;
    string _cluster_name;
};


/**
@brief 功能描述: 撤销镜像的消息类\n
@li @b 其它说明: 无
*/
class ImageCancelMsg:public WorkReq
{
public:
    ImageCancelMsg():_uid(INVALID_UID){};
    
    ImageCancelMsg(int64 uid, const string &base_uuid, const string &cluster_name, const string &user_name):
    _uid(uid), _base_uuid(base_uuid), _cluster_name(cluster_name), _user_name(user_name)    
    { 
    };
    
    ~ImageCancelMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageCancelMsg);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_user_name);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageCancelMsg);
        READ_VALUE(_uid);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_user_name);
        DESERIALIZE_PARENT_END(WorkReq);
    };

    void Print()
    {
        cout << "ImageCancelMsg: " 
             << "action_id = " << action_id 
             << ",_uid = "   << _uid 
             << ",_base_uuid = " << _base_uuid 
             << ",_cluster_name = " << _cluster_name << endl ;
    }   
       
    int64  _uid;
    string _base_uuid;
    string _cluster_name;
    string _user_name;
};

/**
@brief 功能描述: 部署镜像的应答消息类\n
@li @b 其它说明: 无
*/
class ImageCancelAckMsg:public WorkAck
{
public:
    ImageCancelAckMsg():_uid(INVALID_UID){};
 
    ImageCancelAckMsg(const string& action_id, int64 uid, const string& base_uuid, const string& cluster_name):
    WorkAck(action_id), _uid(uid), _base_uuid(base_uuid), _cluster_name(cluster_name)    
    { 
    };
    
    ~ImageCancelAckMsg(){};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageCancelAckMsg);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_workflow_id);
        WRITE_VALUE(_workflow_engine);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageCancelAckMsg);
        READ_VALUE(_uid);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_workflow_id);
        READ_VALUE(_workflow_engine);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    void Print()
    {
        cout << "ImageCancelAckMsg : " 
             << "action_id = " << action_id 
             << ",state = " << state 
             << ",progress = " << progress
             << ",detail = " << detail 
             << ",_uid = " << _uid 
             << ",_base_uuid = " << _base_uuid 
             << ",_cluster_name = " << _cluster_name 
             << ",_workflow_id = " << _workflow_id 
             << ",_workflow_engine = " << _workflow_engine <<  endl ;
    }   

    int64  _uid;
    string _base_uuid;
    string _cluster_name;
    string _workflow_id;
    string _workflow_engine;

};

/**
@brief 功能描述: 查询映像服务器剩余空间的消息类\n
@li @b 其它说明: 无
*/
class ImageGetSpaceMsg :public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ImageGetSpaceMsg() {};
    
    ImageGetSpaceMsg(int64  uid)
    { 
        _uid    = uid;
    };
    
    ~ImageGetSpaceMsg() {};    

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageGetSpaceMsg);
        WRITE_DIGIT(_uid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageGetSpaceMsg);
        READ_DIGIT(_uid);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "ImageGetSpaceMsg : " <<
                "_uid = "   << _uid <<endl;    
    };

    int64      _uid;
};


/**
@brief 功能描述: 查询映像服务器剩余空间的应答消息类\n
@li @b 其它说明: 无
*/
class ImageGetSpaceAckMsg:public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ImageGetSpaceAckMsg(){};
    
    ImageGetSpaceAckMsg(int64 uid, int ret_code, int64 freespace, int64 totalspace) 
    {
        _uid        = uid;
        _ret_code   = ret_code;
        _freespace  = freespace;
        _totalspace = totalspace;
    };
    

    ~ImageGetSpaceAckMsg() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageGetSpaceAckMsg);
        WRITE_DIGIT(_uid);
        WRITE_DIGIT(_ret_code);
        WRITE_DIGIT(_freespace);
        WRITE_DIGIT(_totalspace);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageGetSpaceAckMsg);
        READ_DIGIT(_uid);
        READ_DIGIT(_ret_code);
        READ_DIGIT(_freespace);
        READ_DIGIT(_totalspace);
        DESERIALIZE_END();
    };

    int64     _uid;
    int       _ret_code;
    int64     _freespace;
    int64     _totalspace;
};

}  /* end namespace zte_tecs */

#endif /* end TC_IMAGE_MANAGER_WITH_API_METHOD_H */

