/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_image.h
* 文件标识：
* 内容摘要：工程镜像管理头文件
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
#ifndef TECS_PROJECT_IMAGE_H
#define TECS_PROJECT_IMAGE_H
#include "sky.h"
#include "nfs.h"
#include "image_url.h"
#include "image.h"
#include "vmcfg.h"
#include "project_manager_with_image_manager.h"

namespace zte_tecs
{

/**
@brief 功能描述:工程镜像状态枚举\n
@li @b 其它说明: 工程镜像模块内部使用
*/
enum ProjectImageState
{
    PIS_INIT = 0,
    PIS_SAVE_PREPARE_TEMP = 1,
    PIS_SAVE_TARBALLING = 2,
    PIS_SAVE_REGISTRY = 3,
    PIS_SAVE_RELEASE_TEMP = 4,
    PIS_SAVE_FINISH = 5, 
    PIS_SAVE_READY = 6, 
    PIS_SAVE_ERROR = 7,
    PIS_IMPORT_PREPARE_TEMP = 8,
    PIS_IMPORT_TARBALLING = 9,
    PIS_IMPORT_REGISTRY = 10,
    PIS_IMPORT_RELEASE_TEMP = 11,
    PIS_IMPORT_FINISH = 12,
    PIS_IMPORT_READY = 13,
    PIS_IMPORT_ERROR = 14,
    PIS_UNKNOWN = 15
};

inline const char* ProjectImageStateStr(int state)
{
#define ENUM2STR(value) case(value): return #value;
    switch((ProjectImageState)state)
    {
        ENUM2STR(PIS_INIT);
        ENUM2STR(PIS_SAVE_PREPARE_TEMP);
        ENUM2STR(PIS_SAVE_TARBALLING);
        ENUM2STR(PIS_SAVE_REGISTRY);
        ENUM2STR(PIS_SAVE_RELEASE_TEMP);
        ENUM2STR(PIS_SAVE_FINISH);
        ENUM2STR(PIS_SAVE_READY);
        ENUM2STR(PIS_SAVE_ERROR);
        ENUM2STR(PIS_IMPORT_PREPARE_TEMP);
        ENUM2STR(PIS_IMPORT_TARBALLING);
        ENUM2STR(PIS_IMPORT_REGISTRY);
        ENUM2STR(PIS_IMPORT_RELEASE_TEMP);
        ENUM2STR(PIS_IMPORT_FINISH);
        ENUM2STR(PIS_IMPORT_READY);
        ENUM2STR(PIS_IMPORT_ERROR);
        ENUM2STR(PIS_UNKNOWN);
        default:return "PIS_UNKNOWN";
    }
};

class  TecsProjectImageConfig: public  Serializable
{     
public:
    Project ProjectInfo;
    vector<VmCfg> Instances;
    vector<VmRepel> Repels;
    vector<VmAffReg> AffRegs;
    vector<Image> Images;    
    
    TecsProjectImageConfig(){};
    TecsProjectImageConfig(const Project& project,const vector<VmCfg>& vms, const vector<VmRepel>& repels, const vector<VmAffReg>& affregs, const vector<Image>& images):
    ProjectInfo(project),Instances(vms),Repels(repels),AffRegs(affregs),Images(images)
    {
    };
    ~TecsProjectImageConfig(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(TecsProjectImageConfig);
        WRITE_OBJECT(ProjectInfo);
        WRITE_OBJECT_ARRAY(Instances);
        WRITE_OBJECT_ARRAY(Repels);
        WRITE_OBJECT_ARRAY(AffRegs);
        WRITE_OBJECT_ARRAY(Images);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(TecsProjectImageConfig);
        READ_OBJECT(ProjectInfo);
        READ_OBJECT_ARRAY(Instances);
        READ_OBJECT_ARRAY(Repels);
        READ_OBJECT_ARRAY(AffRegs);
        READ_OBJECT_ARRAY(Images);
        DESERIALIZE_END();
    };
};
class  TecsProjectImageFile: public Serializable
{     
public:
    string Name;
    string Checksum;
    
    TecsProjectImageFile(){};
    TecsProjectImageFile(const string &_name, const string &_checksum):
    Name(_name), Checksum(_checksum)
    {
    };
    ~TecsProjectImageFile(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(TecsProjectImageFile);
        WRITE_VALUE(Name);
        WRITE_VALUE(Checksum);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(TecsProjectImageFile);
        READ_VALUE(Name);
        READ_VALUE(Checksum);
        DESERIALIZE_END();
    };
};

class  TecsProjectImageFileList: public Serializable
{     
public:
    vector<TecsProjectImageFile> List;
    
    TecsProjectImageFileList(){};
    TecsProjectImageFileList(const vector<TecsProjectImageFile> _list):
    List(_list)
    {
    };
    ~TecsProjectImageFileList(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(TecsProjectImageFileList);
        WRITE_OBJECT_ARRAY(List);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(TecsProjectImageFileList);
        READ_OBJECT_ARRAY(List);
        DESERIALIZE_END();
    };
};

/**
@brief 功能描述:工程镜像转换转换参数\n
@li @b 其它说明: 工程镜像模块内部使用
*/
class ProjectImagePara
{
public:
    ProjectImagePara()
    {
    };

    ProjectImagePara(const string& dir, const string& file):
    _dir(dir),_file(file)
    {
    };

    ~ProjectImagePara()
    {
    };
    
    void Print()
    {
        cout<<"Tarball dir:"<<_dir<<endl;
        cout<<"Tarball file:"<<_file<<endl;
    };

    string _dir;
    string _file;
};

/**
@brief 功能描述:工程镜像发送方地址信息\n
@li @b 其它说明:工程镜像模块内部使用
*/
class ProjectImageSender
{
public:
    ProjectImageSender(const string& request_id,
                             const MID& sender):
    _request_id(request_id),_sender(sender)
    {
    };

    ~ProjectImageSender()
    {
    };
    
    string _request_id;
    MID _sender;
};

/**
@brief 功能描述:工程镜像控制结构\n
@li @b 其它说明: 工程镜像模块内部使用
*/
class ProjectImageCtl
{
public:
    ProjectImageCtl()
    {
        _state = PIS_INIT;
        _pid = -1;
        _fail_repeats = 0;
        _begin_at.refresh();
        _end_at.refresh();
    };

    ProjectImageCtl(const string& request_id, const ExportProjectImageReq& req, const ProjectImagePara& para):
    _request_id(request_id),_export_req(req),_para(para)
    {
        _state = PIS_INIT;
        _progress = 0;
        _pid = -1;
        _fail_repeats = 0;
        _begin_at.refresh();
        _end_at.refresh();
        _export_ack.action_id = req.action_id;
        _export_ack._request_id = req._request_id;
    };

    ProjectImageCtl(const string& request_id, const ImportProjectImageReq& req, const ProjectImagePara& para):
    _request_id(request_id),_import_req(req),_para(para)
    {
        _state = PIS_INIT;
        _progress = 0;
        _pid = -1;
        _fail_repeats = 0;
        _begin_at.refresh();
        _end_at.refresh();
        _import_ack.action_id = req.action_id;
        _import_ack._request_id = req._request_id;
    };

    
    ~ProjectImageCtl()
    {
    };
    void Print()
    {
        cout<<"request_id:"<<_request_id<<endl;
        cout<<"state:"<<ProjectImageStateStr(_state)<<endl;
        cout<<"progress:"<<_progress<<endl;
        cout<<"detail:"<<_detail<<endl;
        cout<<"begin:"<<_begin_at.tostr(false)<<endl;
        cout<<"end:"<<_end_at.tostr(false)<<endl;
        cout<<"pid:"<<_pid<<endl;
        _para.Print();
    };

    string _request_id;
    ProjectImageState _state;
    int _progress;
    string _detail;
    Datetime _begin_at;
    Datetime _end_at;
    pid_t _pid;
    int64 _fail_repeats;    
    ExportProjectImageReq _export_req;
    ExportProjectImageAck _export_ack;
    ImportProjectImageReq _import_req;
    ImportProjectImageAck _import_ack;
    ProjectImagePara _para;
};

class ProjectImageAgent;

/**
@brief 功能描述:工程镜像打包线程池任务结构\n
@li @b 其它说明: 工程镜像模块内部使用
*/
class TarballTask:public Worktodo
{
public:
    TarballTask(const string& name, const ExportProjectImageReq& req, const ProjectImagePara& para):
    Worktodo(name),_req(req), _para(para)
    {
    };

    ~TarballTask()
    {
    };
    STATUS Init();
    void Print();
    int do_system(const string& command);
    STATUS virtual Execute();

private:

    DISALLOW_COPY_AND_ASSIGN(TarballTask);
    TarballTask():Worktodo("null")
    {};
    THREAD_ID _work_thread_id;
    ExportProjectImageReq _req;//原始请求
    ProjectImagePara _para;
    ProjectImageAgent *_agent;
};

/**
@brief 功能描述:工程镜像打包线程池任务结构\n
@li @b 其它说明: 工程镜像模块内部使用
*/
class UnTarballTask:public Worktodo
{
public:
    UnTarballTask(const string& name, const ImportProjectImageReq& req, const ProjectImagePara& para):
    Worktodo(name),_req(req), _para(para)
    {
    };

    ~UnTarballTask()
    {
    };
    STATUS Init();
    void Print();
    int do_system(const string& command);
    STATUS virtual Execute();

private:

    DISALLOW_COPY_AND_ASSIGN(UnTarballTask);
    UnTarballTask():Worktodo("null")
    {};
    THREAD_ID _work_thread_id;
    ImportProjectImageReq _req;//原始请求
    ProjectImagePara _para;
    ProjectImageAgent *_agent;
};


/**
@brief 功能描述:工程镜像代理\n
@li @b 其它说明: 工程镜像模块内部使用
*/
class ProjectImageAgent:public MessageHandlerTpl<MessageUnit>
{
public:
    #define EV_TIMER_QUERY  EV_TIMER_1
    #define QUERY_STATIS_INTERVAL 5
    #define MAX_PROJECT_IMAGE_TASKS 10
    #define MAX_FAIL_REPEATS 5
    #define TECS_TEMP_DIR "/var/lib/tecs/temp"
    
    static ProjectImageAgent* GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new ProjectImageAgent(); 
        }
        return _instance;
    };
    
    ~ProjectImageAgent(){};
    STATUS Init();
    void Lock()
    {
        _mutex.Lock();
    }
    void Unlock()
    {
        _mutex.Unlock();
    }
    void AddToList(const ProjectImageCtl& ctl)
    {
        Lock();
        _ctl_list.push_back(ctl);
        Unlock();
    }
    void RemoveFromList(const string& request_id)
    {
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                _ctl_list.erase(it);
                break;
            }
        }
        Unlock();
    }
    bool QueryList(const string& request_id,ProjectImageCtl &ctl)
    {
        bool ret = false;
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                ctl = *it;
                ret = true;
                break;
            }
        }
        Unlock();
        return ret;
    }
    bool QueryAndAddList(const string& request_id,const ProjectImageCtl &ctl)
    {
        bool ret = false;
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                ret = true;
                break;
            }
        }
        if (!ret)
        {
            _ctl_list.push_back(ctl);
        }
        Unlock();
        return ret;
    }

    void UpdateList(const string& request_id,const ProjectImageState& state,const string& detail)
    {
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                it->_state = state;
                it->_detail = detail;
                break;
            }
        }
        Unlock();
    }
    void UpdateList(const string& request_id,const Datetime& dt)
    {
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                it->_end_at = dt;
                break;
            }
        }
        Unlock();
    }
    void UpdateList(const string& request_id,pid_t pid)
    {
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                it->_pid = pid;
                break;
            }
        }
        Unlock();
    }
    void UpdateProgress(const string& request_id,int progress)
    {
        Lock();
        vector<ProjectImageCtl>::iterator it;
        for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
        {
            if (it->_request_id == request_id)
            {
                it->_progress = progress;
                break;
            }
        }
        Unlock();
    }
    void AddSender(const string& request_id,const MID& sender)
    {
        _sender.insert(make_pair(request_id,sender));
    }
    void RemoveSender(const string& request_id)
    {
        _sender.erase(request_id);
    }
    bool GetSender(const string& request_id,MID& sender)
    {
        bool ret = false;
        map<string,MID>::iterator it = _sender.find(request_id);
        if (it != _sender.end())
        {
            sender = it->second;
            ret = true;
        }
        return ret;
    }
    string GetImageDir(void)
    {
        return _image_dir;
    }
    void Print()
    {
        Lock();
        cout<<"--------Agent Info--------"<<endl;
        cout<<"Timer:"<<_timer<<endl;
        cout<<"Last Querys:"<<_last_query_times<<endl;
        cout<<"Curr Querys:"<<_current_query_times<<endl;
        cout<<"--------Sender List--------"<<endl;
        if (_sender.empty())
        {
            cout<<"no senders"<<endl;
        }
        else
        {
            map<string,MID>::iterator it;
            for (it = _sender.begin(); it != _sender.end(); it++)
            {
                cout<<"----------------"<<endl;
                cout<<"RequestId:"<<it->first<<endl;
                cout<<"Application:"<<it->second._application<<endl;
                cout<<"Process:"<<it->second._process<<endl;
                cout<<"Unit:"<<it->second._unit<<endl;
            }
        }
        cout<<"--------Download List--------"<<endl;
        if (_ctl_list.empty())
        {
            cout << "No download tasks!" << endl;
        }
        else
        {
            vector<ProjectImageCtl>::iterator it;
            for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
            {
                cout << "----------------" << endl;
                it->Print();
            }
        }
        Unlock();
    }
    string MakeUserDir(const string& user_name);
    string File2Location(const string& user_name,const string& file_name);
    void RmSoftlinkInTempDir(const string& dir);
    STATUS MakeSoftlink2TempDir(const string& request_id, const string& dir);
    STATUS MakeTempDir(const string& dir);
    string MakeTempDir(const string& request_id, ostringstream& oss);
    STATUS ClearTempDir(const string &dir, ostringstream& oss);
    STATUS GenerateLstFile(const ExportProjectImageReq& req,const string &dir, ostringstream& oss);
    STATUS GenerateCfgFile(const ExportProjectImageReq& req, const vector<Image>& images, const string &dir, ostringstream& oss);
    STATUS GatherImagesAndWriteCfgFile(const ExportProjectImageReq& req, const string &dir, ostringstream& oss);
    STATUS GenerateTempFiles(const ExportProjectImageReq& req,const string &dir, ostringstream& oss);
    STATUS PrepareTempFiles(const ExportProjectImageReq& req, string &tmp_dir, ostringstream& oss);
    STATUS PrepareTpiFile(const ExportProjectImageReq& req, string &tpi_image, ostringstream& oss);
    STATUS ReleaseTempFiles(const string& request_id, ostringstream& oss);
    STATUS GetTpiImage(int64 image_id, Image& image, ostringstream& oss);
    bool CheckFinish(const string& image_name, ostringstream& oss);
    STATUS StartTarball(const ExportProjectImageReq& req, const string& tmp_dir, const string& tpi_file, ostringstream& oss);
    STATUS StartUnTarball(const ImportProjectImageReq& req, const string& tmp_dir, const string& tpi_file, ostringstream& oss);
    STATUS KillProcess(const string& process,const string& tag, ostringstream& oss);
    STATUS StopTarball(const string& request_id, ostringstream& oss);
    bool CheckCtlList(const string& request_id, WorkAck& ack, ostringstream& oss);
    bool CheckExportReqValid(const ExportProjectImageReq& req, ostringstream& oss);
    bool CheckImportReqValid(const ImportProjectImageReq& req, ostringstream& oss);
    STATUS ExportProjectImage(const ExportProjectImageReq& req);
    STATUS ImportProjectImage(const ImportProjectImageReq& req);
    STATUS CalcProgress(const ProjectImageCtl& ctl, int& progress);
    STATUS GetFileInfo(const string &file,int64& size, ostringstream& oss);
    STATUS RegisterImage(int64 uid, const string& description, const string& file, const string& container_format, int64& image_id, ostringstream& oss);
    STATUS RegisterImage(int64 uid, const string& file, const Image& image, int64& image_id, ostringstream& oss);
    STATUS RegisterImage(int64 uid, const string& src, const string& dst, const Image& image, int64& image_id, ostringstream& oss);
    bool MatchImage(const Image& src, const Image& dst, ostringstream& oss);
    int64 GetImageIdByChecksum(const string& checksum);
    int64 GetImageIdByName(const string& name);
    STATUS GetImageByLocation(const string& location, Image& image);
    int64 GetImageIdByLocation(const string& location);
    STATUS ParseTpiLstFile(const string& file, TecsProjectImageFileList& lst, ostringstream& oss);
    STATUS ParseTpiCfgFile(const string& file, TecsProjectImageConfig& cfg, ostringstream& oss);
    STATUS ParseTpiImage(const ImportProjectImageReq& req,const string& dir, Project& project, vector<VmCfg>& vms, vector<VmRepel>& repels, vector<VmAffReg>& affregs, ostringstream& oss);
    void QueryProjectImage(void);
    virtual void MessageEntry(const MessageFrame&);
    
private:
    ProjectImageAgent(){};
    DISALLOW_COPY_AND_ASSIGN(ProjectImageAgent);
    vector<ProjectImageCtl> _ctl_list;
    Mutex _mutex;
    map<string,MID> _sender;
    ThreadPool *_tpool;
    TIMER_ID _timer;  
    int64 _last_query_times;
    int64 _current_query_times;
    string _image_dir;
    static ProjectImageAgent *_instance;
};

}
#endif




