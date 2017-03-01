/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_project_manager_with_api_method.h
* 文件标识：见配置管理计划书
* 内容摘要：project_manager 与 api_method 之间的消息体结构定义
* 当前版本：1.0
* 作    者：yw
* 完成日期：2012年11月20日
*
* 修改记录1：
*     修改日期：2012/11/20
*     版 本 号：V1.0
*     修 改 人：yw
*     修改内容：创建
*******************************************************************************/
#ifndef MSG_PROJECT_MANAGER_WITH_API_METHOD_H
#define MSG_PROJECT_MANAGER_WITH_API_METHOD_H
#include "workflow.h"
using namespace std;
namespace zte_tecs {
class MessageProjectAllocateReq : public Serializable
{
public:
     
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageProjectAllocateReq);   
        WRITE_VALUE(_uid);
        WRITE_VALUE(_project_name);
        WRITE_VALUE(_description);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageProjectAllocateReq);
        READ_VALUE(_uid);
        READ_VALUE(_project_name);
        READ_VALUE(_description);
        DESERIALIZE_END();
    };   
    int64   _uid;
    string  _project_name;
    string  _description;
};

class MessageSaveAsImageReq : public WorkReq
{
public:
    MessageSaveAsImageReq()
    {
        _uid = INVALID_UID;
        _is_deployed_only = false;
        _is_image_save = true;
    };

    ~MessageSaveAsImageReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaveAsImageReq);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_user_name);
        WRITE_VALUE(_project_name);
        WRITE_VALUE(_is_deployed_only);
        WRITE_VALUE(_is_image_save);
        WRITE_VALUE(_description);
        SERIALIZE_PARENT_END(WorkReq);
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaveAsImageReq);
        READ_VALUE(_request_id);
        READ_VALUE(_uid);
        READ_VALUE(_user_name);
        READ_VALUE(_project_name);
        READ_VALUE(_is_deployed_only);
        READ_VALUE(_is_image_save);
        READ_VALUE(_description);
        DESERIALIZE_PARENT_END(WorkReq);
    };
    
    string  _request_id;
    int64 _uid;
    string _user_name;
    string  _project_name;
    bool _is_deployed_only;
    bool _is_image_save;
    string _description;
};
class MessageProjectOperateAck : public Serializable
{
public:    
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageProjectOperateAck);
        WRITE_VALUE(state);
        WRITE_VALUE(project_id);
        WRITE_VALUE(detail);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageProjectOperateAck); 
        READ_VALUE(state);
        READ_VALUE(project_id);
        READ_VALUE(detail); 
        DESERIALIZE_END();
    };   
    int state;
    int64  project_id;
    string  detail;
};

class MessageSaveAsImageAck : public WorkAck
{
public:    
    MessageSaveAsImageAck()
    {
    }

    MessageSaveAsImageAck(const string& action_id,
                                 const string& request_id):
    WorkAck(action_id),_request_id(request_id)
    {
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageSaveAsImageAck);        
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageSaveAsImageAck);       
        READ_VALUE(_request_id);
        READ_VALUE(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    string   _request_id;
    string   _workflow_id;
};

class MessageCreateByImageReq : public WorkReq
{
public:
    MessageCreateByImageReq()
    {
        _uid = INVALID_UID;
        _image_id = INVALID_IID;
    };
    ~MessageCreateByImageReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageCreateByImageReq);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_user_name);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_project_name);
        SERIALIZE_PARENT_END(WorkReq);
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageCreateByImageReq);
        READ_VALUE(_request_id);
        READ_VALUE(_uid);
        READ_VALUE(_user_name);
        READ_VALUE(_image_id);
        READ_VALUE(_project_name);
        DESERIALIZE_PARENT_END(WorkReq);
    };
    string  _request_id;
    int64 _uid;
    string _user_name;
    int64 _image_id;
    string _project_name;
};

class ProjectOperationReq: public WorkReq
{
public:
    int64   _proid;
    int     _operation;
    string  _project_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmOperationReq);  
        WRITE_DIGIT(_proid);
        WRITE_DIGIT(_operation);
        WRITE_VALUE(_project_name);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmOperationReq);
        READ_DIGIT(_proid);
        READ_DIGIT(_operation);
        READ_VALUE(_project_name);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};
class ProjectOperationAck: public WorkAck
{
public:
    int64   _pid;
    int     _operation;
    int     _state;        // 虚拟机状态
    string  _workflow_id;
    string  _workflow_engine;

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmOperationAck);  
        WRITE_VALUE(_pid);
        WRITE_VALUE(_operation);
        WRITE_VALUE(_state);
        WRITE_VALUE(_workflow_id);
        WRITE_VALUE(_workflow_engine);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmOperationAck);
        READ_VALUE(_pid);
        READ_VALUE(_operation);
        READ_VALUE(_state);
        READ_VALUE(_workflow_id);
        READ_VALUE(_workflow_engine);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};
class ProjectCloneReq: public Serializable
{
public:
    string  old_name;
    string  new_name;
    string  des;
    int64 _uid;

    SERIALIZE
    {
        SERIALIZE_BEGIN(ProjectCloneReq);  
        WRITE_VALUE(old_name);
        WRITE_VALUE(new_name);
        WRITE_VALUE(des);
        WRITE_VALUE(_uid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ProjectCloneReq);
        READ_VALUE(old_name);
        READ_VALUE(new_name);
        READ_VALUE(des);
        READ_VALUE(_uid);
        DESERIALIZE_END();
    };
};
class ProjectCloneAck: public Serializable
{
public:
    int64   _pid;    
    int     _state;        // 虚拟机状态
    string  _detail;

    SERIALIZE
    {
        SERIALIZE_BEGIN(ProjectCloneAck);  
        WRITE_VALUE(_pid);
        WRITE_VALUE(_state);
        WRITE_VALUE(_detail);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ProjectCloneAck);
        READ_VALUE(_pid);  
        READ_VALUE(_state);
        READ_VALUE(_detail);
        DESERIALIZE_END();
    };
};
class MessageCreateByImageAck : public WorkAck
{
public:    
    MessageCreateByImageAck()
    {
    }

    MessageCreateByImageAck(const string& action_id,
                                 const string& request_id):
    WorkAck(action_id),_request_id(request_id)
    {
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageCreateByImageAck);        
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_workflow_id);
        SERIALIZE_PARENT_END(WorkAck);
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageCreateByImageAck);       
        READ_VALUE(_request_id);
        READ_VALUE(_workflow_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    string   _request_id;
    string   _workflow_id;
};

class MessageProjectDataReq : public WorkReq
{
public:
    MessageProjectDataReq()
    {
        _start_index=0;
        _count = 0;
    };

    ~MessageProjectDataReq(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageProjectDataReq);
        WRITE_VALUE(_start_index);
        WRITE_VALUE(_count);
        WRITE_VALUE(_project_name);
        SERIALIZE_PARENT_END(WorkReq);
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageProjectDataReq);
        READ_VALUE(_start_index);
        READ_VALUE(_count);
        READ_VALUE(_project_name);
        DESERIALIZE_PARENT_END(WorkReq);
    };
    
    int64   _start_index;
    int64   _count;
    string  _project_name;
};


class ProjectDynamicData: public Serializable
{
public:
    long long  _vm_id;   //虚拟机id
//    string     _vm_name; //虚拟机独有的字段
    long long  _nic_used_rate;//网卡平均利用率
    long long  _cpu_used_rate;// cpu利用率
    long long  _mem_used_rate;//内存利用率

    SERIALIZE
    {
        SERIALIZE_BEGIN(ProjectDynamicData);
        WRITE_DIGIT(_vm_id);
//        WRITE_STRING(_vm_name);
        WRITE_DIGIT(_nic_used_rate);
        WRITE_DIGIT(_cpu_used_rate);
        WRITE_DIGIT(_mem_used_rate);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ProjectDynamicData);
        READ_DIGIT(_vm_id);
//        READ_STRING(_vm_name);
        READ_DIGIT(_nic_used_rate);
        READ_DIGIT(_cpu_used_rate);
        READ_DIGIT(_mem_used_rate);   
        DESERIALIZE_END();
    };
};


class MessageProjectDataAck : public WorkAck
{
public:    
    MessageProjectDataAck()
    {
        _ret_code = 0;
        _next_index = -1;
        _max_count = 0;
    }

    ~MessageProjectDataAck()
    {
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageProjectDataAck);  
        WRITE_VALUE(_ret_code);
        WRITE_VALUE(_project_name);		
        WRITE_VALUE(_next_index);
        WRITE_VALUE(_max_count);
        WRITE_OBJECT_ARRAY(_project_data);
        SERIALIZE_PARENT_END(WorkAck);
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageProjectDataAck);   
        READ_VALUE(_ret_code);
        READ_VALUE(_project_name);
        READ_VALUE(_next_index);
        READ_VALUE(_max_count);		
        READ_OBJECT_ARRAY(_project_data);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    int32   _ret_code;	
    string  _project_name;
    int64   _next_index;
    uint64  _max_count;
    vector<ProjectDynamicData> _project_data;

};


}

#endif
