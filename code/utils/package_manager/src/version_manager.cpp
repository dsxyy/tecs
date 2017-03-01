/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_manager.cpp
* 文件标识：
* 内容摘要：版本管理进程
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年9月26日
*
* 修改记录1：
*     修改日期：2012年9月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#include "version_manager.h"
#include "tecs_errcode.h"
#include "event.h"
#include "mid.h"
#include "log_agent.h"

namespace zte_tecs
{
static int version_manager_print_on = 0;
DEFINE_DEBUG_VAR(version_manager_print_on);
#define Debug(fmt,arg...) \
            do \
            { \
                if(version_manager_print_on) \
                { \
                        OutPut(SYS_DEBUG,fmt,##arg); \
                } \
            }while(0)

VersionManager *VersionManager::_instance = NULL;

STATUS VersionManager::Init()
{
    if(NULL != _qtpool)
    {
        return ERROR_BAD_ADDRESS;
    }

    _qtpool = new ThreadPool(5);
    if (!_qtpool)
    {
        OutPut(SYS_EMERGENCY,"Version failed to create thread pool!\n");
        return ERROR_NO_MEMORY;
    }
    STATUS ret = _qtpool->CreateThreads("Version Query Task");
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version failed to create threads:%u!\n",ret);
        delete _qtpool;
        _qtpool = NULL;
        return ret;
    }

    if(NULL != _utpool)
    {
        return ERROR_BAD_ADDRESS;
    }

    _utpool = new ThreadPool(5);
    if (!_utpool)
    {
        OutPut(SYS_EMERGENCY,"Version failed to create thread pool!\n");
        return ERROR_NO_MEMORY;
    }
    ret = _utpool->CreateThreads("Version Upgrade Task");
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version failed to create threads:%u!\n",ret);
        delete _utpool;
        _utpool = NULL;
        return ret;
    }
    VersionUpgradeImpl::StaticInit();
    return StartMu(MU_VERSION_MANAGER);
}

void VersionManager::DoSetRepoAddrReq(const MessageFrame &message)
{
    _repo_addr = message.data;

    MessageVersionSetRepoAddrAck msg(SUCCESS);
    MessageUnit temp_mu(TempUnitName("Ack set repo addr"));
    temp_mu.Register();

    MessageOption option(message.option.sender(), EV_VERSION_SET_REPOADDR_ACK, 0, 0);
    temp_mu.Send(msg,option);

    return;
}

void VersionManager::DoGetRepoAddrReq(const MessageFrame &message)
{
    MessageVersionGetRepoAddrAck msg(SUCCESS,_repo_addr);
    MessageUnit temp_mu(TempUnitName("ack get RepoAddr"));
    temp_mu.Register();

    MessageOption option(message.option.sender(), EV_VERSION_GET_REPOADDR_ACK, 0, 0);
    temp_mu.Send(msg,option);
    return;
}

void VersionManager::DoUpgradeReq(const MessageFrame &message)
{
    //todo:后面需要应答任务是否成功的消息
    MessageVersionUpdateReq msg;
    msg.deserialize(message.data);
    if (msg._node_type != "hc")
    {
       DoUpgradeAck(ERROR_INVALID_ARGUMENT,0,message.option.sender());
       return;
    }

    if (_utpool->FindWork(msg._cluster_name+"_"+
                        msg._host_name+"_"+
                        msg._package))
    {
        DoUpgradeAck(ERROR_DUPLICATED_OP,0,message.option.sender());
        return;
    }

    VersionUpgradeTask*  vt = new 
    VersionUpgradeTask(_repo_addr,
        msg._cluster_name,
        msg._cluster_ip,
        msg._host_name,
        msg._host_ip,
        msg._package);


    STATUS ret =_utpool->AssignWork(vt);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version failed to assign %s task:%u!\n",msg._host_name.c_str(),ret);
        DoUpgradeAck(ret,vt->GetOid(),message.option.sender());
        return;
    }
    vt->InsertData();

    DoUpgradeAck(ret,vt->GetOid(),message.option.sender());
    
    return;
}

void VersionManager::DoUpgradeAck(STATUS result,int64 taskid,const MID&receiver)
{
    MessageVersionUpdateAck msg(result,taskid);
    MessageUnit temp_mu(TempUnitName("ack Upgrade"));
    temp_mu.Register();

    MessageOption option(receiver, EV_VERSION_QUERY_ACK, 0, 0);
    temp_mu.Send(msg,option);

    return;
}

void VersionManager::DoQueryReq(const MessageFrame &message)
{
    //todo:后面需要应答任务是否成功的消息
    VersionQueryTask    *vt=NULL;
    MessageVersionQueryReq msg;
    msg.deserialize(message.data);

    if (msg._node_type == "tc")
    {
       vt = new VersionQueryTask(60,
            _repo_addr,
            message.option.sender(),
            msg._cloud_ip);
    }
    else if (msg._node_type == "cc")
    {
        vt = new  VersionQueryTask(60,
            _repo_addr,
            message.option.sender(),
            msg._node_type,
            msg._cluster_name,
            msg._cluster_ip);    
    }
    else if (msg._node_type == "sa")
    {
        vt = new  VersionQueryTask(60,
            _repo_addr,
            message.option.sender(),
            msg._node_type,
            msg._storage_adapter_name,
            msg._storage_adapter_ip);    
    }
    else if  (msg._node_type == "hc")
    {
        vt = new VersionQueryTask(60,
            _repo_addr,
            message.option.sender(),
            msg._cluster_name,
            msg._cluster_ip,
            msg._host_name,
            msg._host_ip);
    }
    else
    {
        DoQueryAck(ERROR_INVALID_ARGUMENT,message.option.sender());
        return;
    }
    //DoQueryAck(ERROR_INVALID_ARGUMENT,message.option.sender());
    //printf("Version failed to assign %s task!\n",msg._host_name.c_str());

    STATUS ret =_qtpool->AssignWork(vt);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version failed to assign %s task:%u!\n",msg._host_name.c_str(),ret);
        DoQueryAck(ret,message.option.sender());
    }

    return;
}

void VersionManager::DoQueryAck(STATUS result,const MID&receiver)
{
    MessageVersionQueryAck msg(result);
    MessageUnit temp_mu(TempUnitName("ack query"));
    temp_mu.Register();

    MessageOption option(receiver, EV_VERSION_QUERY_ACK, 0, 0);
    temp_mu.Send(msg,option);

    return;
}

void VersionManager::DoCancelReq(const MessageFrame &message)
{
    //todo:后面需要应答任务是否成功的消息
    if(NULL == _utpool)
    {
        OutPut(SYS_EMERGENCY,"Version fail to cancel task!\n");
        DoCancelAck(ERROR,message.option.sender()); 
    }

    MessageVersionCancelReq msg;
    msg.deserialize(message.data);
    string name=msg._cluster_name+"_"+msg._host_name+"_"+msg._package;
    STATUS ret = _utpool->CancelWork(name);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version fail to cancel %s task!\n",name.c_str());
    }

    DoCancelAck(ret,message.option.sender()); 

    return;
}

void VersionManager::DoCancelAck(STATUS result,const MID&receiver)
{
    MessageVersionCancelAck msg(result);
    MessageUnit temp_mu(TempUnitName("ack Cancel"));
    temp_mu.Register();

    MessageOption option(receiver, EV_VERSION_CANCEL_ACK, 0, 0);
    temp_mu.Send(msg,option);

    return;
}

/******************************************************************************/
void VersionManager::DoStartUp()
{
    return;
}

STATUS VersionManager::StartMu(const string& name)
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
void VersionManager::MessageEntry(const MessageFrame &message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            DoStartUp();
            _mu->set_state(S_Work);            
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            OutPut(SYS_WARNING,"Version recve unkown msg %d powen on \n", message.option.id());
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
            case EV_VERSION_GET_REPOADDR_REQ:
            {
                Debug("recve req msg %d\n", message.option.id());
                DoGetRepoAddrReq(message);
                break;
            }

            case EV_VERSION_SET_REPOADDR_REQ:
            {
                Debug("recve req msg %d\n", message.option.id());
                DoSetRepoAddrReq(message);
                break;
            }

            case EV_VERSION_QUERY_REQ:
            {
                Debug("recve req msg %d\n", message.option.id());
                DoQueryReq(message);
                break;
            }

            case EV_VERSION_UPDATE_REQ:
            {
                Debug("recve req msg %d\n", message.option.id());
                DoUpgradeReq(message);
                break;
            }
            case EV_VERSION_CANCEL_REQ:
            {
                Debug("recve ack msg %d\n", message.option.id());
                DoCancelReq(message);
                break;
            }
            default:
            {
                OutPut(SYS_WARNING,"Version recve unkown msg %d\n", message.option.id());
                break;
            }
        }
        break;
    }

    default:
        break;
    }
}

void DbgVmmSet(const char *repo_addr)
{
    MessageUnit temp_mu(TempUnitName("VmmReqUpgrade"));
    temp_mu.Register();
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_SET_REPOADDR_REQ,0,0);

    temp_mu.Send(repo_addr, option);
}

void DbgVmmGet()
{
    MessageUnit temp_mu(TempUnitName("VmmReqUpgrade"));
    temp_mu.Register();
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_GET_REPOADDR_REQ,0,0);
    temp_mu.Send("no data", option);
}

void DbgVmmReqUpgrade(const char *repo_addr,
                const char *cluster_name,
                const char *cluster_ip,
                const char *host_name,
                const char *host_ip,
                const char *package)
{
    MessageUnit temp_mu(TempUnitName("VmmReqUpgrade"));
    temp_mu.Register();
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_UPDATE_REQ,0,0);

    MessageVersionUpdateReq msg;
    //msg._repo_addr = repo_addr;
    msg._cluster_name = cluster_name;
    msg._cluster_ip = cluster_ip;
    msg._host_name = host_name;
    msg._host_ip = host_ip;
    msg._package= package;
    temp_mu.Send(msg, option);
}

void DbgVmmReqQuery1(char *repo_addr,char *ip,const char *package)
{
    //DbgVmmReq("tc",EV_VERSION_QUERY_REQ,repo_addr,ip,"","","","",package);
}

void DbgVmmReqQuery2(char *repo_addr,char *name,char *ip,char *package)
{
    //DbgVmmReq("cc",EV_VERSION_QUERY_REQ,repo_addr,"",name,ip,"","",package);
    MessageUnit temp_mu(TempUnitName("VmmReqUpgrade"));
    temp_mu.Register();
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_QUERY_REQ,0,0);

    MessageVersionQueryReq msg;
    msg._node_type = "cc";
    //msg._repo_addr = repo_addr;
    msg._cluster_name = name;
    msg._cluster_ip = ip;
    temp_mu.Send(msg, option);
}

void DbgVmmReqQuery3(char *repo_addr,char *cluster_name,char *cluster_ip,char *host_name,char *host_ip,char *package)
{
    //DbgVmmReq("hc",EV_VERSION_QUERY_REQ,repo_addr,"",cluster_name,cluster_ip,host_name,host_ip,package);
}

void DbgVmmReqCancel1(char* cluster_name,char*host_name)
{
    MessageUnit temp_mu(TempUnitName("VmmReqCancel"));
    temp_mu.Register();
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_CANCEL_REQ,0,0);
    MessageVersionCancelReq msg;
    msg._cluster_name= cluster_name;
    msg._host_name= host_name;
    temp_mu.Send(msg, option);
}

DEFINE_DEBUG_FUNC(DbgVmmSet);
DEFINE_DEBUG_FUNC(DbgVmmGet);


DEFINE_DEBUG_FUNC(DbgVmmReqUpgrade);
DEFINE_DEBUG_FUNC(DbgVmmReqQuery1);
DEFINE_DEBUG_FUNC(DbgVmmReqQuery2);
DEFINE_DEBUG_FUNC(DbgVmmReqQuery3);
DEFINE_DEBUG_FUNC(DbgVmmReqCancel1);
} // namespace zte_tecs

