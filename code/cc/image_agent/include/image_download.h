/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_download.h
* 文件标识：
* 内容摘要：image下载管理头文件
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
#ifndef TECS_IMAGE_DOWNLOAD_H
#define TECS_IMAGE_DOWNLOAD_H
#include "sky.h"
#include "nfs.h"
#include "image_url.h"

namespace zte_tecs
{

/**
@brief 功能描述: 镜像下载请求结构\n
@li @b 其它说明: 镜像下载模块对外接口
*/
class ImageDownloadReq:public WorkReq
{
public:
    ImageDownloadReq()
    {
        _image_id = INVALID_FILEID;
        _size = 0;
    };

    ImageDownloadReq(const string& actionid, 
                            const string request_id, 
                            int64 image_id, 
                            int64 size, 
                            const FileUrl& source_url,
                            const FileUrl& target_url,
                            const ImageStoreOption& option)
    {
        action_id = actionid;
        _request_id = request_id;
        _image_id = image_id;
        _size = size;
        _source_url = source_url;
        _target_url = target_url;
        _option = option;
    };

    ~ImageDownloadReq(){};   

    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageDownloadReq);
        WRITE_VALUE(_request_id);  
        WRITE_VALUE(_image_id); 
        WRITE_VALUE(_size); 
        WRITE_OBJECT(_source_url);
        WRITE_OBJECT(_target_url);
        WRITE_OBJECT(_option);
        SERIALIZE_PARENT_END(WorkReq);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageDownloadReq);
        READ_VALUE(_request_id);
        READ_VALUE(_image_id);
        READ_VALUE(_size);
        READ_OBJECT(_source_url);
        READ_OBJECT(_target_url);
        READ_OBJECT(_option);
        DESERIALIZE_PARENT_END(WorkReq);
    };

    string _request_id;
    int64 _image_id;
    int64 _size;
    FileUrl _source_url;
    FileUrl _target_url;
    ImageStoreOption _option;
};

/**
@brief 功能描述: 镜像下载应答结构\n
@li @b 其它说明: 镜像下载模块对外接口
*/
class ImageDownloadAck: public WorkAck
{
public:
    ImageDownloadAck(){};

    ImageDownloadAck(const string& request_id):_request_id(request_id)
    {
    };
    
    ImageDownloadAck(const string& action_id,const string& request_id):
    WorkAck(action_id),_request_id(request_id)
    {
    };

    ~ImageDownloadAck(){};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageDownloadAck);
        WRITE_VALUE(_request_id); 
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageDownloadAck);
        READ_VALUE(_request_id);
        DESERIALIZE_PARENT_END(WorkAck);
    };

    string _request_id;
};

/**
@brief 功能描述:镜像下载状态枚举\n
@li @b 其它说明: 镜像下载模块内部使用
*/
enum ImageDownloadState
{
    IDS_INIT = 0,
    IDS_DOWNLOADING = 1,
    IDS_CHECKSUMING = 2,
    IDS_FINISH = 3, //下载或者checksum完成
    IDS_READY = 4, //准备好 发应答前的状态 应答消息可能发失败
    IDS_CANCELING = 5,
    IDS_ERROR = 6,
    IDS_UNKNOWN = 7
};

inline const char* ImageDownloadStateStr(int state)
{
#define ENUM2STR(value) case(value): return #value;
    switch((ImageDownloadState)state)
    {
        ENUM2STR(IDS_INIT);
        ENUM2STR(IDS_DOWNLOADING);
        ENUM2STR(IDS_CHECKSUMING);
        ENUM2STR(IDS_FINISH);
        ENUM2STR(IDS_READY);
        ENUM2STR(IDS_CANCELING);
        ENUM2STR(IDS_ERROR);
        ENUM2STR(IDS_UNKNOWN);
        default:return "IDS_UNKNOWN";
    }
};

/**
@brief 功能描述:镜像下载发送方地址信息\n
@li @b 其它说明: 镜像下载模块内部使用
*/
class ImageDownloadSender
{
public:
    ImageDownloadSender(const string& request_id,
                             const MID& sender):
    _request_id(request_id),_sender(sender)
    {
    };

    ~ImageDownloadSender()
    {
    };
    
    string _request_id;
    MID _sender;
};

/**
@brief 功能描述:镜像下载转换参数\n
@li @b 其它说明: 镜像下载模块内部使用
*/
class ImageDownloadPara
{
public:
    ImageDownloadPara()
    {
        _size = 0;
    };

    ImageDownloadPara(const string& source,
                             int64 size,
                             const string& checksum,
                             const string& target,
                             const ImageStoreOption& option):
    _source(source),_size(size),_checksum(checksum),_target(target),_option(option)
    {
    };

    ~ImageDownloadPara()
    {
    };
    
    void Print()
    {
        cout<<"source:"<<_source<<endl;
        cout<<"size:"<<_size<<endl;
        cout<<"checksum:"<<_checksum<<endl;
        cout<<"target:"<<_target<<endl;
        cout<<"space exp:"<<_option._img_spcexp<<endl;
        cout<<"source type:"<<_option._img_srctype<<endl;
        cout<<"image usage:"<<_option._img_usage<<endl;
        cout<<"image share usage:"<<_option._share_img_usage<<endl;

    };

    string _source;
    int64 _size;
    string _checksum;
    string _target;
    ImageStoreOption _option;
};

/**
@brief 功能描述:镜像下载控制结构\n
@li @b 其它说明: 镜像下载模块内部使用
*/
class ImageDownloadCtl
{
public:
    ImageDownloadCtl()
    {
        _state = IDS_INIT;
        _pid = -1;
        _begin_at.refresh();
        _end_at.refresh();
    };

    ImageDownloadCtl(const string& request_id, const ImageDownloadPara& para, const ImageDownloadReq& req):
    _request_id(request_id),_para(para),_req(req)
    {
        _state = IDS_INIT;
        _progress = 0;
        _pid = -1;
        _begin_at.refresh();
        _end_at.refresh();
    };
    
    ~ImageDownloadCtl()
    {
    };
    void Print()
    {
        cout<<"request_id:"<<_request_id<<endl;
        cout<<"state:"<<ImageDownloadStateStr(_state)<<endl;
        cout<<"progress:"<<_progress<<endl;
        cout<<"detail:"<<_detail<<endl;
        cout<<"begin:"<<_begin_at.tostr(false)<<endl;
        cout<<"end:"<<_end_at.tostr(false)<<endl;
        cout<<"pid:"<<_pid<<endl;
        _para.Print();
    };

    string _request_id;
    ImageDownloadState _state;
    int _progress;
    string _detail;
    Datetime   _begin_at;
    Datetime   _end_at;
    pid_t      _pid;
    ImageDownloadPara _para;
    ImageDownloadReq _req;
};

class ImageDownloadAgent;

/**
@brief 功能描述:镜像下载线程池任务结构\n
@li @b 其它说明: 镜像下载模块内部使用
*/
class ImageDownloadTask:public Worktodo
{
public:
    ImageDownloadTask(const string& name,
                             const ImageDownloadPara& para,
                             const ImageDownloadReq& req):
    Worktodo(name),_para(para),_req(req)
    {
    };

    ~ImageDownloadTask()
    {
    };
    STATUS Init();
    void Print();
    int do_system(const string& command);
    STATUS virtual Execute();

private:

    DISALLOW_COPY_AND_ASSIGN(ImageDownloadTask);
    ImageDownloadTask():Worktodo("null")
    {};
    THREAD_ID _work_thread_id;
    ImageDownloadPara _para;//准备好的输入
    ImageDownloadReq _req;//原始请求
    ImageDownloadAgent *_agent;
};

/**
@brief 功能描述:镜像下载代理\n
@li @b 其它说明: 镜像下载模块内部使用
*/
class ImageDownloadAgent:public MessageHandlerTpl<MessageUnit>
{
public:
    #define EV_TIMER_QUERY  EV_TIMER_1
    #define QUERY_STATIS_INTERVAL 5
    #define MAX_IMAGE_DOWNLOAD_TASKS 10
    #define SOURCE_MOUNT_DIR "/mnt/tecs/src/"
    #define TARGET_MOUNT_DIR "/mnt/tecs/tgt/"
    
    static ImageDownloadAgent* GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new ImageDownloadAgent(); 
        }
        return _instance;
    };
    
    ~ImageDownloadAgent(){};
    STATUS Init();
    void Lock()
    {
        _mutex.Lock();
    }
    void Unlock()
    {
        _mutex.Unlock();
    }
    void AddToList(const ImageDownloadCtl& ctl)
    {
        Lock();
        _ctl_list.push_back(ctl);
        Unlock();
    }
    void RemoveFromList(const string& request_id)
    {
        Lock();
        vector<ImageDownloadCtl>::iterator it;
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
    bool QueryList(const string& request_id,ImageDownloadCtl &ctl)
    {
        bool ret = false;
        Lock();
        vector<ImageDownloadCtl>::iterator it;
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
    bool QueryAndAddList(const string& request_id,const ImageDownloadCtl &ctl)
    {
        bool ret = false;
        Lock();
        vector<ImageDownloadCtl>::iterator it;
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

    void UpdateList(const string& request_id,const ImageDownloadState& state,const string& detail)
    {
        Lock();
        vector<ImageDownloadCtl>::iterator it;
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
        vector<ImageDownloadCtl>::iterator it;
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
        vector<ImageDownloadCtl>::iterator it;
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
        vector<ImageDownloadCtl>::iterator it;
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
            vector<ImageDownloadCtl>::iterator it;
            for (it = _ctl_list.begin(); it != _ctl_list.end(); it++)
            {
                cout << "----------------" << endl;
                it->Print();
            }
        }
        Unlock();
    }
    STATUS DeleteMountDir(const string& name, ostringstream& oss);
    string MakeSourceMountDir(const ImageDownloadReq& req, ostringstream& oss);
    string MakeTargetMountDir(const ImageDownloadReq& req, const string& device, ostringstream& oss);
    STATUS ReleaseNfsSource(const ImageDownloadReq& req, ostringstream& oss);
    STATUS PrepareNfsSource(const NfsAccessOption& option, 
                                  const string& path,
                                  string& mountdir, 
                                  string& source,
                                  ostringstream& oss);
    STATUS PrepareSource(const ImageDownloadReq& req, string& source, ostringstream& oss);
    STATUS ReleaseSource(const ImageDownloadReq& req, ostringstream& oss);
    STATUS MountTargetDisk(const ImageDownloadReq& req, const string& device, string &target, ostringstream& oss);
    STATUS UmountTargetDisk(const ImageDownloadReq& req, ostringstream& oss);
    STATUS PrepareTarget(const ImageDownloadReq& req, string &target, ostringstream& oss);
    STATUS ReleaseTarget(const ImageDownloadReq& req, ostringstream& oss);
    STATUS SetFinish(const ImageDownloadReq& req, const string& target, ostringstream& oss);
    bool CheckFinish(const ImageDownloadReq& req, const string& target, ostringstream& oss);
    STATUS StartDownload(const ImageDownloadReq& req, const string& source, const string& target, ostringstream& oss);
    STATUS KillProcess(const string& process,const string& tag, ostringstream& oss);
    STATUS StopDownload(const ImageDownloadReq& req, ostringstream& oss);
    bool CheckCtlList(const ImageDownloadReq& req, ImageDownloadAck& ack, ostringstream& oss);
    bool CheckReqValid(const ImageDownloadReq& req, ostringstream& oss);
    STATUS PrepareDownloadImage(const ImageDownloadReq& req);
    STATUS ReleaseDownloadImage(const ImageDownloadReq& req);
    STATUS CalcProgress(const ImageDownloadCtl& ctl, int& progress);
    void QueryDownloadImage(void);
    virtual void MessageEntry(const MessageFrame&);
    
private:
    ImageDownloadAgent(){};
    DISALLOW_COPY_AND_ASSIGN(ImageDownloadAgent);
    vector<ImageDownloadCtl> _ctl_list;
    Mutex _mutex;
    map<string,MID> _sender;
    ThreadPool *_tpool;
    TIMER_ID _timer;  
    int64 _last_query_times;
    int64 _current_query_times;
    static ImageDownloadAgent *_instance;
};

}
#endif




