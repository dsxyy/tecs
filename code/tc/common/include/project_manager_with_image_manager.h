/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_manager_with_image_manager.h
* 文件标识：
* 内容摘要：工程管理与镜像管理的相关头文件
* 当前版本：1.0
* 作    者：yw
* 完成日期：2013年4月10日
*
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
*******************************************************************************/
#ifndef TC_PROJECT_MANAGER_WITH_IMAGE_MANAGER_H
#define TC_PROJECT_MANAGER_WITH_IMAGE_MANAGER_H
#include "sky.h"
#include "workflow.h"
#include "vmcfg.h"
#include "project.h"

namespace zte_tecs
{

class  ExportProjectImageReq: public  WorkReq
{     
public:
    string _request_id;
    int64 _uid;
    string _user_name;
    Project _project;
    vector<VmCfg> _vms;
    vector<VmRepel> _repels;
    vector<VmAffReg> _affregs;
    string _description;
    
    ExportProjectImageReq()
    {
        _uid = INVALID_UID;
    };
    ExportProjectImageReq(const string& action_id,
                              const string& request_id, 
                              int64 uid, 
                              const string& user_name,
                              const Project& project,
                              const vector<VmCfg> vms, 
                              const vector<VmRepel> repels,
                              const vector<VmAffReg> affregs,
                              const string& description):
    WorkReq(action_id), _request_id(request_id), _uid(uid), _user_name(user_name), 
    _project(project), _vms(vms), _repels(repels), _affregs(affregs), _description(description)
    {
    };
    ~ExportProjectImageReq(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(ExportProjectImageReq);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_user_name);
        WRITE_OBJECT(_project);
        WRITE_OBJECT_ARRAY(_vms);
        WRITE_OBJECT_ARRAY(_repels);
        WRITE_OBJECT_ARRAY(_affregs);
        WRITE_VALUE(_description);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ExportProjectImageReq);
        READ_VALUE(_request_id);
        READ_VALUE(_uid);
        READ_VALUE(_user_name);
        READ_OBJECT(_project);
        READ_OBJECT_ARRAY(_vms);
        READ_OBJECT_ARRAY(_repels);
        READ_OBJECT_ARRAY(_affregs);
        READ_VALUE(_description);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};


class  ExportProjectImageAck: public  WorkAck
{     
public:
    string _request_id;
    int64  _image_id;
    
    ExportProjectImageAck()
    {
        _image_id = INVALID_IID;
    };
    ExportProjectImageAck(const string action_id, const string& request_id):
    WorkAck(action_id), _request_id(request_id)
    {
        _image_id = INVALID_IID;
    };
    ~ExportProjectImageAck(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(ExportProjectImageAck);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_image_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ExportProjectImageAck);
        READ_VALUE(_request_id);
        READ_VALUE(_image_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};

class  ImportProjectImageReq: public  WorkReq
{     
public:
    string _request_id;
    int64  _uid;
    string _user_name;
    int64  _image_id;
    
    ImportProjectImageReq()
    {
        _uid = INVALID_UID;
        _image_id = INVALID_IID;
    };
    ImportProjectImageReq(const string& action_id, const string& request_id, int64 uid, const string& user_name, int64 image_id):
    WorkReq(action_id), _request_id(request_id), _uid(uid), _user_name(user_name), _image_id(image_id)
    {
    };
    ~ImportProjectImageReq(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImportProjectImageReq);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_user_name);
        WRITE_VALUE(_image_id);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImportProjectImageReq);
        READ_VALUE(_request_id);
        READ_VALUE(_uid);
        READ_VALUE(_user_name);
        READ_VALUE(_image_id);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};


class  ImportProjectImageAck: public  WorkAck
{     
public:
    string _request_id;
    Project _project;
    vector<VmCfg> _vms;
    vector<VmRepel> _repels;
    vector<VmAffReg> _affregs;
    
    ImportProjectImageAck()
    {
    };
    ImportProjectImageAck(const string& action_id, const string& request_id):
    WorkAck(action_id),_request_id(request_id)
    {
    };
    ~ImportProjectImageAck(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImportProjectImageAck);
        WRITE_VALUE(_request_id);
        WRITE_OBJECT(_project);
        WRITE_OBJECT_ARRAY(_vms);
        WRITE_OBJECT_ARRAY(_repels);
        WRITE_OBJECT_ARRAY(_affregs);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImportProjectImageAck);
        READ_VALUE(_request_id);
        READ_OBJECT(_project);
        READ_OBJECT_ARRAY(_vms);
        READ_OBJECT_ARRAY(_repels);
        READ_OBJECT_ARRAY(_affregs);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};
//虚拟机操作应答，用于HC->CC，CC->TC
class ProOperationReq: public  WorkReq
{
public:
    int64   _pid;
    int     _operation;
    string  _workflow_id;  
    
    ProOperationReq()
    {
    };
    ~ProOperationReq(){};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ProOperationReq);  
        WRITE_VALUE(_pid);
        WRITE_VALUE(_operation); 
        WRITE_VALUE(_workflow_id);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ProOperationReq);
        READ_VALUE(_pid);
        READ_VALUE(_operation);   
        READ_VALUE(_workflow_id);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};
//虚拟机操作应答，用于HC->CC，CC->TC
class ProOperationAck: public WorkAck
{
public:
    int64   _pid;
    int     _operation;
    string  _workflow_id;  
    
    ProOperationAck()
    {
    };
    ~ProOperationAck(){};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ProOperationAck);  
        WRITE_VALUE(_pid);
        WRITE_VALUE(_operation); 
        WRITE_VALUE(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ProOperationAck);
        READ_VALUE(_pid);
        READ_VALUE(_operation);   
        READ_VALUE(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};

} //namespace zte_tecs


#endif // end TC_PROJECT_MANAGER_WITH_IMAGE_MANAGER_H

