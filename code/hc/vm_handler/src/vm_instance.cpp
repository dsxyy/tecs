/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_instance.cpp
* 文件标识：
* 内容摘要：VmInstance类的实现文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年7月26日
*
* 修改记录1：
*     修改日期：2012/7/19
*     版 本 号：V1.1
*     修 改 人：lixch
*     修改内容：对内容进行框架整改
*******************************************************************************/
#include <unistd.h>
#include <sys/wait.h>
#include "vm_instance.h"
#include "vm_handler.h"
#include "instance_pool.h"
#include "registered_system.h"
#include "vm_metadata.h"
#include "vm_messages.h"
#include "mid.h"
#include "timer.h"
#include "pci_passthrought.h"
#include "vnet_libnet.h"
#include "hypervisor.h"

/****************************************************************************************
*                                 引用外部函数声明                                    *
****************************************************************************************/

/****************************************************************************************
*                                 全局变量                                              *
****************************************************************************************/
string   tecs_instance_path = "/var/lib/tecs/instances/";
string   ssh_key_file_path = "/root/.ssh/authorized_keys";

const int wait_op_duration  = 60;    /* 等待操作执行完的时长，单位 S */
const int op_check_period   = 1000;  /* 周期检查操作结果的时长，单位 ms */
const int vm_refresh_period = 5000;  /* 周期检查操作结果的时长，单位 ms */

const int load_refresh_period = 120000;  /* 周期检查操作结果的时长，单位 ms */

extern int32 statistics_interval;

/****************************************************************************************
*                                 工具函数                                              *
****************************************************************************************/
/****************************************************************************************
* 函数名称：Is64BitSystem
* 功能描述：是否为64位系统
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
bool Is64BitSystem()
{
#if defined(__ia64__)
    return true;
#else
    if(sizeof(void *) == 8)
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
}

/****************************************************************************************
* 函数名称：Init
* 功能描述：初始化instance_x线程，包括启动MU
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
STATUS VmInstance::Init()
{
    _vm_driver      = VmDriver::GetInstance();
    if (NULL == _vm_driver)
    {
        OutPut(SYS_ALERT, "VmDriver GetInstance() failed in VmInstance:: init() !\n");
        return ERROR;
    }

    _storage_instance = StorageOperation::GetInstance();
    if (NULL == _storage_instance)
    {
        OutPut(SYS_ALERT, "StorageOperation GetInstance() failed in VmInstance:: init() !\n");
        return ERROR;
    }

    if (SUCCESS != StartMu())
    {
        OutPut(SYS_ALERT, "StartMu failed in VmInstance:: init() !\n");
        return ERROR;
    }

    // 创建一个状态检查用的定时器
    _op_check_timer     = _mu->CreateTimer();

    if (INVALID_TIMER_ID == _op_check_timer)
    {
        OutPut(SYS_ALERT, "CreateTimer _op_check_timer failed in VmInstance::init() !\n");
        return ERROR;
    }

    _refresh_timer     = _mu->CreateTimer();

    if (INVALID_TIMER_ID == _refresh_timer)
    {
        OutPut(SYS_ALERT, "CreateTimer _refresh_timer failed in VmInstance::init() !\n");
        return ERROR;
    }

    _load_timer         = _mu->CreateTimer();
    if (INVALID_TIMER_ID == _load_timer)
    {
        OutPut(SYS_ALERT, "CreateTimer _load_timer failed in VmInstance::init() !\n");
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************************
* 函数名称：DoStartup
* 功能描述：instance_x收到handler发来的上电通知处理函数
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2013/09/17    V1.0    johnir       create
****************************************************************************************/
STATUS VmInstance::DoStartup()
{
     OutPut(SYS_DEBUG, "%s receive EV_POWER_ON msg from handler.\n", _instance_name.c_str());
            
    // 启动虚拟机实例的循环刷新定时器
    StartRefreshTimer();

    // 上电状态下，就要刷新一次
    RefreshInstanceInfo();
    RefreshInstanceReportInfo(_out_info);
    
    OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
    
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：StartOpCheckTimer
* 功能描述：启动虚拟机操作后续检查的定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
void VmInstance::StartOpCheckTimer()
{
    TimeOut timeout;
    timeout.type     = RELATIVE_TIMER;
    timeout.duration = op_check_period;    // 定时1S
    timeout.msgid    = EV_TIMER_OP_RESULT;
    _mu->SetTimer(_op_check_timer, timeout);
}

/****************************************************************************************
* 函数名称：StartOpCheckTimer
* 功能描述：启动虚拟机操作后续检查的定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
void VmInstance::StartRefreshTimer()
{
    TimeOut timeout;
    timeout.type     = RELATIVE_TIMER;
    timeout.duration = vm_refresh_period;    // 默认定时10S

    // 如果处于运行中的话，则1S定时进行刷新
    if (IsOpRunning())
    {
        timeout.duration = 1000;    // 定时1S
    }

    timeout.msgid    = EV_TIMER_REFRESH;
    STATUS rc = _mu->SetTimer(_refresh_timer, timeout);
    if (SUCCESS != rc)
    {
        OutPut(SYS_ALERT, "%s StartRefreshTimer failed rc = %d!\n",
               _instance_name.c_str(),rc);
        SkyAssert(false);
    }
}

/****************************************************************************************
* 函数名称：GetRunVersion
* 功能描述：获取当前运行虚拟机的运行版本
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
void VmInstance::GetRunVersion()
{
    string version;

    // 如果运行版本已经获取到了，那么退出
    if (_run_version != -1)
    {
        return;
    }

    version = _vm_driver->GetDomainDescByName(_instance_name);

    // NULL的话，保持当前内存的运行版本不变
    if (version.empty())
    {
        return;
    }

    // 把描述信息中的版本信息保存到内存中
    from_string(_run_version, version, dec);
}

/****************************************************************************************
* 函数名称：DoOpResultFunction
* 功能描述：根据虚拟机操作结果做相应的动作
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
void VmInstance::DoOpResultFunction(STATUS rs)
{
    if (VMOP_RUNNING  == rs)
    {
        // 操作等待最长60S，主要用在RESET和REBOOT中等待虚拟机消失的时间
        _wait_time = wait_op_duration;
        // 设置个1S定时器，来检测操作是否完成
        StartOpCheckTimer();
        return;
    }

    if (VMOP_SUCC == rs)
    {
        SetProgress(100);
    }

    // 设置当前的执行结果
    SetOpResult(rs);
    // 回应答
    SendVmOprAckMsg(_op_sender, _op_action_id, _mu);
}


/****************************************************************************************
* 函数名称：StartWDG
* 功能描述：启动看门狗定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
bool  VmInstance::StartWDGTimer(const char *const op_fun)
{
    return OperateWatchDog(op_fun, EN_START_TIMER_WATCH_DOG);
}

/****************************************************************************************
* 函数名称：StopWDGTimer
* 功能描述：停止看门狗定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
bool  VmInstance::StopWDGTimer(const char *const op_fun)
{
    return OperateWatchDog(op_fun, EN_STOP_TIMER_WATCH_DOG);
}

/****************************************************************************************
* 函数名称：StartOpCheckWDG
* 功能描述：虚拟机操作检查启动看门狗定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
STATUS VmInstance::StartOpCheckWDG()
{
    if (!StartWDGTimer(__FUNCTION__))
    {
        return VMOP_RUNNING;
    }
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：CheckOpTimeOut
* 功能描述：检查虚拟机操作是否超时了，有些命令超时需要返回失败
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
STATUS VmInstance::CheckOpTimeOut()
{
    switch (_op_cmd)
    {
    case DESTROY:
    case RESET:
    case REBOOT:
    case RR:
        if (_wait_time != 0)
        {
            _wait_time--;
            return VMOP_RUNNING;
        }

        OutPut(SYS_WARNING, "### CheckOpTimeOut: %s wait cmd %s time out, finsh wait ###\r\n",
               _instance_name.c_str(),  Enum2Str::VmOpStr(_op_cmd) );
        return VMOP_TIME_OUT;
        break;

    default:
        return VMOP_RUNNING;
        break;
    }
}

/****************************************************************************************
* 函数名称：DoOpCheck
* 功能描述：虚拟机操作检查
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
STATUS VmInstance::DoOpCheck()
{
    // 如果不是操作等待态，则不进行下面判断
    if (VMOP_RUNNING != _op_result)
    {
        return SUCCESS;
    }

    switch (_op_cmd)
    {
    case CREATE:
    case START:
    case RESUME:
    {
        if (VM_RUNNING == _state)
        {
            return StartOpCheckWDG();
        }
        else
        {
            return CheckOpTimeOut();
        }
        break;
    }
    case STOP:
    case DESTROY:
    {
        if (VM_SHUTOFF == _state)
        {
            return SUCCESS;
        }
        else
        {
            return CheckOpTimeOut();
        }
        break;
    }
    case REBOOT:
    case RESET:
    {
        if (-1 == _domain_id)
        {
            return CheckOpTimeOut();
        }

        if (VM_RUNNING != _state)
        {
            return VMOP_RUNNING;
        }

        if (_op_domid != (uint32)_domain_id)
        {
            // 如果reboot 或者reset成功，则启动看门狗
            return StartOpCheckWDG();
        }
        else
        {
            return CheckOpTimeOut();
        }
        break;
    }
    case RR:
    {
        if (-1 == _domain_id)
        {
            if (_double_op_wait_time > 0)
            {
                _double_op_wait_time--;
            }
            return CheckOpTimeOut();
        }

        if (VM_RUNNING != _state)
        {
            OutPut(SYS_DEBUG, "DoOpCheck %s vm state is %s, op RR return VMOP_RUNNING\n",
                   _instance_name.c_str(),Enum2Str::InstStateStr(_state));
            return VMOP_RUNNING;
        }

        if (_op_domid != (uint32)_domain_id)
        {
            // 如果reboot 或者reset成功，则启动看门狗
            return StartOpCheckWDG();
        }
        else
        {
            if (_double_op_cmd == REBOOT)
            {
                if (_double_op_wait_time == -1)
                {
                    return VMOP_RUNNING;
                }

                if (_double_op_wait_time > 0)
                {
                    _double_op_wait_time--;
                }

                if (_double_op_wait_time == 0)
                {
                    SetProgress(75);
                    _double_op_cmd = RESET;
                    ResetDomain(false);
                }
            }

            return VMOP_RUNNING;
        }
        break;
    }
    case PAUSE:
    {
        if (VM_PAUSED == _state)
        {
            return SUCCESS;
        }
        else
        {
            return VMOP_RUNNING;
        }
        break;
    }
    default:
        OutPut(SYS_ALERT, "DoOpCheck %s find unknown op %d\n",_instance_name.c_str(),_op_cmd);
        return VMOP_ERROR_UNKNOWN;
    }

    return VMOP_ERROR_UNKNOWN;
}


/****************************************************************************************
* 函数名称：StartMu
* 功能描述：启动消息单元
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
STATUS VmInstance::StartMu()
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    _mu = new MessageUnit(_instance_name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",_instance_name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",_instance_name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",_instance_name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",_instance_name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    // 不必再发POWER_ON消息，直接在Init函数中初始化
    /*  MessageFrame message(SkyInt32(0), EV_POWER_ON);
      ret = _mu->Receive(message);
      if (SUCCESS != ret)
      {
          SkyAssert(0);
          return ret;
      } */
    _mu->Print();
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：MessageEntry
* 功能描述：消息单元执行体
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
void VmInstance::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            if(SUCCESS == DoStartup())
            {
                _mu->set_state(S_Work);
            }

            break;
        }
        default:
        {
            OutPut(SYS_DEBUG, "VmInstance receive unknown message on startup,id=%d, Ignorel.\n",
                   message.option.id());
            break;
        }
        }
    }
    
    case S_Work:
    {
        switch (message.option.id())
        {
            // 接收来自VmHandler的操作消息
        case EV_VM_OP:
        {
            // 执行虚拟机操作消息
            ExecVmOperation(message);
            break;
        }
        case EV_TIMER_OP_RESULT:
        {
            // 如果状态检查成功，则设置成功状态和进度100%
            STATUS rc = DoOpCheck();
            if (VMOP_RUNNING == rc)
            {
                // 设置当前进度的详细描述
                SetProgressDesc(WAIT_EXEC_SUCC);
                // 否则继续启动定时器，等待下个周期
                StartOpCheckTimer();
            }
            else
            {
                DoOpResultFunction(rc);
            }

            break;
        }
        case EV_TIMER_REFRESH:
        {
            RefreshInstanceInfo();
            RefreshInstanceReportInfo(_out_info);
            StartRefreshTimer();
            break;

        }
        case EV_TIMER_LOAD:
        {

            RefreshLoadBlocks();

            break;
        }
        default:
        {
            OutPut(SYS_DEBUG, "VmInstance %d receive unknown message,id=%d, Ignorel.\n",
                   _instance_id,
                   message.option.id());
            break;
        }
        }
        break;
    }
    default:
        OutPut(SYS_NOTICE, "%s in state %d, Ignore rcved message (%d)!\n", \
               _mu->name().c_str(),_mu->get_state(),message.option.id() );
        break;
    }
}

/****************************************************************************************
* 函数名称：ExecVmOperation
* 功能描述：执行虚拟机操作
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::ExecVmOperation(const MessageFrame& message)
{
    TranceMsg       TranceMessage;
    VmOperationReq  req;
    VmOperation     opr_id;
    string          vmcfg;                   /* 创建消息携带的配置信息 */

    // 先解析转发消息
    TranceMessage.deserialize(message.data);

    //再解析转发消息中真正消息
    req.deserialize(TranceMessage._data);
    opr_id        = (VmOperation)(req._operation);

    // 记录当前操作的API消息单元和当前操作的命令时的相关参数
    _op_action_id = req.action_id;
    _op_cmd       = opr_id;
    _op_progress  = 2;
    _op_result    = VMOP_RUNNING;
    _op_domid     = _domain_id;
    _op_sender    = TranceMessage._sender;
    _op_ack_end   = false;
    ClearDoubleCmdFlag();

    // 收到请求，应答一下进度
    SendVmOprAckMsg(_op_sender, req.action_id, _mu);

    // 收到操作消息，需要把查询定时器修改为1S查询一次
    //StartRefreshTimer();

    OutPut(SYS_DEBUG, "[%s] ExecVmOperation rcv req from %s, operation: %s\n",
           _instance_name.c_str(), \
           _op_sender.serialize().c_str(),\
           Enum2Str::VmOpStr(opr_id));

    // 如果收到CANCEL命令，则删除虚拟机操作检查定时器
    if (CANCEL == _op_cmd)
    {
        // 不管是否成功，直接执行下，没有启动定时器时，执行也没有影响
        STATUS rc = _mu->StopTimer(_op_check_timer);
        if (SUCCESS != rc)
        {
            OutPut(SYS_ALERT, "ExecVmOperation Stop check Timer Fail,rc = %d", rc);
        }
        rc = _mu->StopTimer(_refresh_timer);
        if (SUCCESS != rc)
        {
            OutPut(SYS_ALERT, "ExecVmOperation Stop refresh Timer Fail,rc = %d", rc);
        }
    }

    //根据命令执行相应的操作,注意喂狗相关设置
    switch (opr_id)
    {
    case CREATE:
    case START:
    {
        VmCreateReq  createreq;
        createreq.deserialize(TranceMessage._data);

        // 记录配置信息 /
        SetVmCfg(createreq._config);
        // 记录option信息 /
        SetVmOption(createreq._option);

        // 准备创建虚拟机的环境
        STATUS rc = PrepareResource(false);
        if (VMOP_SUCC != rc)
        {
            DoOpResultFunction(rc);
            return;
        }
        // 根据创建的结果返回值进行处理
        DoOpResultFunction(StartDomain());
        break;
    }
    case CANCEL:
    {
        // CANCEL的时候，先执行下取消上传的函数
        // 保证CANCEL把上传的流程能够结束掉，
        // 及时没有上传，也不要紧，因为该函数可重入
        _storage_instance->stop_upload_img(_instance_id,vm_cfg._machine);
        CancelDomain();
        break;
    }
    case STOP:
    {
        // 关闭虚拟机
        SetProgress(50);
        DoOpResultFunction(StopDomain());
        break;
    }
    case RESET:
    {
        //复位虚拟机
        SetProgress(50);
        DoOpResultFunction(ResetDomain());
        break;
    }
    case REBOOT:
    {
        // 1分钟内不允许多次启动
        if (!IsReboot())
        {
            DoOpResultFunction(VMOP_ERROR_REBOOT_FAST);
            return;
        }

        // 重启虚拟机
        SetProgress(50);
        DoOpResultFunction(RebootDomain());
        break;
    }
    case RR:
    {
        VmRRReq RRMsg;

        RRMsg.deserialize(TranceMessage._data);

        // 重启虚拟机
        SetProgress(50);
        _double_op_wait_time = RRMsg._delay_time;
        if (RRMsg._delay_time == 0)
        {
            _double_op_cmd = RESET;
            DoOpResultFunction(ResetDomain(false));
            break;
        }
        else
        {
            _double_op_cmd = REBOOT;
            DoOpResultFunction(RebootDomain(false));
            break;
        }
    }
    case PAUSE:
    {
        // 挂起虚拟机
        SetProgress(50);
        DoOpResultFunction(PauseDomain());
        break;
    }
    case RESUME:
    {
        // 恢复虚拟机
        SetProgress(50);
        DoOpResultFunction(ResumeDomain());
        break;
    }
    case PREP_M:
    {
        //目的端准备迁移
        SetProgress(50);
        DoOpResultFunction(PrepareVmMigration(TranceMessage._data));
        break;
    }
    case MIGRATE:
    {
        //源端执行迁移
        //SetProgress(50);
        ExecVmMigration(TranceMessage._data);
        break;
    }
    case POST_M:
    {
        //源端迁移成功
        DoOpResultFunction(HandleVmMigrateSucc(TranceMessage._data));
        break;
    }
    case DESTROY:
    {
        SetProgress(50);
        DoOpResultFunction(DestroyDomain());
        break;
    }
    case UPLOAD_IMG:
    {
        DoOpResultFunction(InstanceUpload(TranceMessage._data));
        break;
    }
    case CANCEL_UPLOAD_IMG:
    {
        DoOpResultFunction(InstanceCancelUpload());
        break;
    }
    case PLUG_IN_USB:
    case PLUG_OUT_USB:
    case OUT_IN_USB:
    {
        DoOpResultFunction(InstanceOpUsb(opr_id, TranceMessage._data));
        break;
    }
    case PLUG_IN_PDISK:
    {
        DoOpResultFunction(PlugInPdisk(TranceMessage._data));
        break;
    }

    case PLUG_OUT_PDISK:
    {
        DoOpResultFunction(PlugOutPdisk(TranceMessage._data));
        break;
    }

    case CREATE_IMAGE_BACKUP:
    {
        DoOpResultFunction(ImageBackupCreate(TranceMessage._data));
        break;
    }
    case DELETE_IMAGE_BACKUP:
    {
        DoOpResultFunction(ImageBackupDelete(TranceMessage._data));
        break;
    }
    case RECOVER_IMAGE:
    {
        DoOpResultFunction(RecoverDomain(TranceMessage._data));
        break;
    }
    default:
    {
        OutPut(SYS_ALERT,"ExecVmOperation rcv unknown cmd: %s\n", Enum2Str::VmOpStr(opr_id));
        break;
    }
    }
}


/****************************************************************************************
* 函数名称：IsUsbInvalid
* 功能描述：校验USB设备是否存在
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
bool VmInstance::IsUsbInvalid(int vendor_id, int product_id, const string &name)
{
    vector<VmQueryUsbInfo> usb_list;
    VmHandler::GetUsbList(usb_list);

    uint32 i;
    for (i = 0; i < usb_list.size(); i++)
    {
        if (usb_list.at(i).vendor_id == vendor_id &&
                usb_list.at(i).product_id == product_id &&
                usb_list.at(i).name == name)
        {
            return true;
        }
    }
    return false;
}

/****************************************************************************************
* 函数名称：InstanceAddUsb
* 功能描述：向虚拟机增加USB设备
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::InstanceOpUsb(VmOperation cmd, const string &message)
{
    VmUsbOp op_req;
    int ret;

    op_req.deserialize(message);
    if (!IsUsbInvalid(op_req._usb_info._vendor_id, op_req._usb_info._product_id, op_req._usb_info._name))
    {
        return VMOP_USB_NOTEXIST;
    }

    if (VM_SHUTOFF == _state)
    {
        // 虚拟机关机状态，直接返回成功
        return SUCCESS;
    }

    if (cmd == PLUG_IN_USB)
    {
        ret = _vm_driver->AddUsb(_instance_name, op_req._usb_info._vendor_id, op_req._usb_info._product_id);
        if (SUCCESS != ret)
        {
            OutPut(SYS_ERROR,"%s Add Usb Fail %d.",_instance_name.c_str(), ret);
            return VMOP_VM_DO_FAIL;
        }
        return SUCCESS;
    }
    else if (cmd == PLUG_OUT_USB)
    {
        ret = _vm_driver->DelUsb(_instance_name, op_req._usb_info._vendor_id, op_req._usb_info._product_id);
        if (SUCCESS != ret)
        {
            OutPut(SYS_ERROR,"%s Del Usb Fail %d.",_instance_name.c_str(), ret);
            return VMOP_VM_DO_FAIL;
        }
    }
    else
    {
        ret = _vm_driver->DelAndAddUsb(_instance_name, op_req._usb_info._vendor_id, op_req._usb_info._product_id);
        if (SUCCESS != ret)
        {
            OutPut(SYS_ERROR,"%s DelAndAdd Usb Fail %d.",_instance_name.c_str(), ret);
            return VMOP_VM_DO_FAIL;
        }
    }
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：InstanceAddUsb
* 功能描述：虚拟机加入USB设备，临时接口，libxl整改删除该接口
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::InstanceAddUsb()
{
    uint32 i;
    for (i = 0; i < vm_cfg._usb.size(); i++)
    {
        int ret = _vm_driver->AddUsb(_instance_name,
                                     vm_cfg._usb.at(i)._vendor_id,
                                     vm_cfg._usb.at(i)._product_id);
        if (SUCCESS != ret)
        {
            OutPut(SYS_ERROR,"%s Add Usb Fail %d.",_instance_name.c_str(), ret);
            return VMOP_VM_DO_FAIL;
        }
    }
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：RefrushInstanceInfo
* 功能描述：刷新虚拟机网口统计信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::RefreshInstanceNicInfo()
{
    VmDomainInterfaceStatsStruct nic_info;
    uint32        i;

    // 如果虚拟机正在操作中，减少LIBVIRT接口的调用
    // 不更新网卡的收发包统计
    if (IsOpRunning())
    {
        return;
    }

    // 获取网口的收发包情况，先清0
    _net_tx             = 0;
    _net_rx             = 0;
    uint32               nomal_nic_index=0;
    for (i = 0; i < vm_cfg._nics.size(); i++)
    {
        // 如果是soiv网卡，使用LIBVIRT接口获取不到网口流量，暂时忽略
        if (vm_cfg._nics.at(i)._sriov)
        {
            continue;
        }

        int res = _vm_driver->GetDomainNetInfoById(_domain_id, nomal_nic_index, nic_info);
        nomal_nic_index++;

        if (0 != res)
        {
            // 不应该查询不成功，记录日志，方便定位,最大可能是查询的过程中，虚拟机重启了
            OutPut(SYS_DEBUG,"refresh_instance_info:%s Get NicInfo failed. no %d,return %d \n",
                   _instance_name.c_str(), i, res);
            continue;
        }
        _net_tx += nic_info.tx_bytes;
        _net_rx += nic_info.rx_bytes;
    }
}


/****************************************************************************************
* 函数名称：RefrushInstanceCpuUseRateInfo
* 功能描述：刷新虚拟机网口统计信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::RefrushInstanceCpuUseRateInfo(VmDomainInfo &DomInfo)
{
    T_TimeValue now_temp;
    GetTimeFromPowerOn(&now_temp);
    int64 now_ms = now_temp.second*1000 + now_temp.milisecond;

    if (0 != _refresh_time)
    {
        // 由于domin单位是NS，刷新单位为ms,所以需要*1000000,求%比，所以*10000
        // 计算的CPU使用率是所有核的平均值
        if ((_vcpu_num != 0) && (now_ms != _refresh_time))
        {
            // 如果存在短时间的时间翻转，则该阶段不进行CPU使用率计算
            // 因为TECS对虚拟机的CPU使用率不敏感
            // 出现翻转的原因，很多时候是因为虚拟机重启了
            if ((DomInfo.cpuTime > _domain_run_time) &&\
                    (now_ms > _refresh_time))
            {
                int64 dom_run_time = DomInfo.cpuTime - _domain_run_time;
                int64 refresh_time = (now_ms -_refresh_time)*1000000;

                _cpu_usage = (dom_run_time*1000)/(refresh_time*_vcpu_num);

                if (_cpu_usage > 1000)
                {
                    _cpu_usage = 1000;
                    OutPut(SYS_ERROR,"_cpu_usage > 1000, dom_rt = %lld dom_lrt %lld" \
                           "now_ms %lld lref_ms\n", DomInfo.cpuTime, _domain_run_time, \
                           now_ms, _refresh_time);
                }
            }
        }
        else
        {
            OutPut(SYS_ERROR,"refresh_instance_info vcpu = %d now_ms %lld" \
                   "refresh time %lld\n", _vcpu_num, now_ms, _refresh_time);
            SkyAssert(false);
        }
    }

    // 记录上次刷新时间和DOMAIN运行时间
    _refresh_time = now_ms;
    _domain_run_time = DomInfo.cpuTime;
}


/****************************************************************************************
* 函数名称：NotifyNetDomIDChg
* 功能描述：通知网络部分，虚拟机ID发送变化
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
bool VmInstance::NotifyNetDomIDChg(const char *const  fun_name)
{
    if(!HasNetwork())
    {
        return true;
    }

    CVNetNotifyDomIDMsg   notify_req;
    uint32 i;
    notify_req.m_vm_id = _instance_id;
    notify_req.m_dom_id = _domain_id;
    //notify_req.action_id = GenerateUUID();
    for (i = 0; i < vm_cfg._nics.size(); i++)
    {
        CVNetVnicConfig temp;
        temp.m_nNicIndex = vm_cfg._nics.at(i)._nic_index;
        temp.m_nIsSriov   = vm_cfg._nics.at(i)._sriov;
        temp.m_nIsLoop   = vm_cfg._nics.at(i)._loop;
        temp.m_strLogicNetworkID = vm_cfg._nics.at(i)._logic_network_id;
        temp.m_strAdapterModel = vm_cfg._nics.at(i)._model;
        temp.m_strVSIProfileID = vm_cfg._nics.at(i)._vsi_profile_id;
        notify_req.m_VmNicCfgList.push_back(temp);
    }

    string ack_msg;
    notify_req.action_id = to_string<int64>(_domain_id,dec);
    bool rc = SendMsgToVnetWithTimeOut(notify_req, EV_VNETLIB_NOTIFY_DOMID,
                                       EV_VNETLIB_NOTIFY_DOMID_ACK,
                                       60, notify_req.action_id,ack_msg);


    if(!rc)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet not ack !!***\n",
               fun_name, _instance_id,  __FUNCTION__);
        return false;
    }

    CVNetNotifyDomIDMsgAck vnet_ack;
    vnet_ack.deserialize(ack_msg);
    if(SUCCESS != vnet_ack.state)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet ack exec fail, msg content: %s ***\n",
               fun_name, _instance_id,  __FUNCTION__, ack_msg.c_str());
        return false;
    }
    return true;
}

/****************************************************************************************
* 函数名称：ModifyCoredumpFileName
* 功能描述：当虚拟机ID变化的时候，该虚拟机的coredump名字为.core.ok
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::ModifyCoredumpFileName()
{
    uint32 i = 0;
    string coredump_save_local_dir;
    GetCoredumpFilePath(coredump_save_local_dir);
    // 查找是自己虚拟机的coredump文件名称
    string cmd = "ls " + coredump_save_local_dir + " | grep -v tar " + " | grep -w " +  _instance_name;
    vector<string> result;

    RunCmd(cmd, result);

    // 修改名称为core.ok
    for (i = 0; i < result.size(); i++)
    {
        string is_exit;
        string file_is_open_cmd="lsof  | grep -w " + coredump_save_local_dir;
        RunCmd(file_is_open_cmd, is_exit);
        if (is_exit.size() != 0)
        {
            continue;
        }
        // 文件没有打开，则可以修改名称
        string coredump_file= coredump_save_local_dir + "/" +result.at(i);
        string rename_file = coredump_save_local_dir + "/" +result.at(i) + ".ok";
        rename(coredump_file.c_str(), rename_file.c_str());
    }
}

/****************************************************************************************
* 函数名称：GetCoredumpFilePath
* 功能描述：获取虚拟机保存coredump文件的位置
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::GetCoredumpFilePath(string &path)
{
    if(_instance_name.empty())
    {
        OutPut(SYS_ALERT, "fatal! _instance_name is null. \n");
    }
    path =  "/var/lib/xen/dump/"+_instance_name;
}

/****************************************************************************************
* 函数名称：OperateWatchDog
* 功能描述：操作看门狗函数
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
bool VmInstance::OperateWatchDog(const char *const  fun_name, int32 op_id)
{
    if (vm_cfg._vm_wdtime == 0)// 未使能看门狗直接返回成功
    {
        return true;
    }

    CVNetWatchDogMsg  wd_req(_instance_id, op_id);
    //wd_req.action_id = GenerateUUID();

    string ack_msg;
    wd_req.action_id = to_string<int64>(_instance_id,dec);
    bool rc = SendMsgToVnetWithTimeOut(wd_req, EV_VNETLIB_WATCHDOG_OPER,
                                       EV_VNETLIB_WATCHDOG_OPER_ACK,
                                       60, wd_req.action_id,ack_msg);

    if(!rc)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet not ack !!***\n",
               fun_name, _instance_id,  __FUNCTION__);
        return false;
    }

    WorkAck vnet_ack;
    vnet_ack.deserialize(ack_msg);
    if(SUCCESS != vnet_ack.state)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet ack exec fail, msg content: %s ***\n",
               fun_name, _instance_id,  __FUNCTION__, ack_msg.c_str());
        return false;
    }

    return true;
}



/****************************************************************************************
* 函数名称：RefrushInstanceInfo
* 功能描述：刷新虚拟机状态信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::RefreshInstanceInfo()
{
    VmDomainInfo DomInfo;

    int domid = _vm_driver->GetDomainIDByName(_instance_name);
    if (-1 == domid)
    {
        _domain_id = -1;
        _state = VM_SHUTOFF;
        return;
    }

    // 检测dominid 是否变化,另外dominid变化只在这一个地方进行更改
    if (_domain_id != domid)
    {
        //StartNetWork(__FUNCTION__, VNET_VM_DEPLOY); domid变化之前 已经给vnet发了startnetwork

        // 更新domainid
        _domain_id = domid;

        // 通知网络那边，dominid变化了
        NotifyNetDomIDChg(__FUNCTION__);

        // 重新bind虚拟机CPU
        _vm_driver->VcpuAffinitySet(_instance_name, _vcpu_num);

        if(IsXenKernel())
        {
            // 检测该虚拟机是否产生了coredump文件了，如果产生了 ，那么修改名称
            ModifyCoredumpFileName();
        }

        InstanceAddUsb();
    }

    // 查虚拟机状态
    if (-1 == _vm_driver->GetDomainInfo(_instance_name, DomInfo))
    {
        // 查询失败置关机态
        _state = VM_SHUTOFF;
        return;
    }

    // 刷新CPU使用率的信息
    RefrushInstanceCpuUseRateInfo(DomInfo);

    // 刷新状态
    _state = DomInfo.state;

    // 记录运行版本号
    GetRunVersion();

    // 记录其他参数
    _mem_max            = DomInfo.maxMem*1024;
    _mem_free           = (DomInfo.maxMem - DomInfo.memory)*1024;

    // 刷新虚拟机网卡信息 - 611003891466kvm上未验证通过，先注掉，后继合入
    //RefreshInstanceNicInfo();

    // 增加虚拟机资源使用统计信息
    _vm_statistics._vec_nic_info.clear();
    DoVmStatistics(_vm_statistics, _instance_id);
    _report_count++;
}

/****************************************************************************************
* 函数名称：RefreshInstanceReportInfo
* 功能描述：获取当前实例的上报信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::RefreshInstanceReportInfo(VmInfo &info)
{
    // 刷新前先获取刷新锁
    MutexLock lock(_refresh_lock);

    info._vm_id                = _instance_id;
    info._vm_name              = _instance_name;
    info._vm_dom_id            = _domain_id;
    info._vm_vcpu_num          = _vcpu_num;
    info._vm_cpu_usage         = _cpu_usage;
    info._vm_mem_max           = _mem_max;
    info._vm_mem_free          = _mem_free;
    info._vm_net_tx            = _net_tx;
    info._vm_net_rx            = _net_rx;

    info._run_version          = _run_version;
    info._vm_state              = _state;
    info._vm_statistics        = _vm_statistics;
}

void VmInstance::RefreshLoadBlocks()
{
    vector<VmDiskConfig> disks;
    SetDiskInfo(disks);
    vector<VmDiskConfig>::iterator it;
    bool bRefreshok=true;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if(it->_position==STORAGE_POSITION_SHARE)
        {
            if(SUCCESS!=RefreshLoadShareDisk(it->_target,it->_share_url.access_option))
            {
                bRefreshok=false;
            }
        }
    }
    if(!bRefreshok)
    {
        TimeOut timeout;
        timeout.type     = RELATIVE_TIMER;
        timeout.duration = load_refresh_period;    // 默认定时120S
        timeout.msgid    = EV_TIMER_LOAD;
        STATUS rc = _mu->SetTimer(_load_timer, timeout);
        if (SUCCESS != rc)
        {
            OutPut(SYS_ALERT, "%s SetLoadTimer failed rc = %d!\n",
                   _instance_name.c_str(),rc);
            SkyAssert(false);
        }
    }

}

/****************************************************************************************
* 函数名称：GetInstanceReportInfo
* 功能描述：获取当前实例的上报信息
* 访问的表：监控专用
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/

bool VmInstance::GetInstanceReportInfo(VmInfo &info)
{
    // 刷新前先获取刷新锁
    MutexLock lock(_refresh_lock);

    // 如果输出ID为-1,表示监控的时候，取的数据还没有刷新，等待所有实例都
    // 刷新过一次后，才能上报
    if (_out_info._vm_id == -1)
    {
        return false;
    }

    info                = _out_info;

    return true;
}


/****************************************************************************************
* 函数名称：SetNetProcess
* 功能描述：设置网络的进度
* 访问的表：无
* 修改的表：无
* 输入参数：0-100
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::SetNetProgress(uint32 progress)
{
    if (progress > 100)
    {
        progress = 100;
    }

    _op_progress = VM_NET_PROCESS*progress/100;
}


/****************************************************************************************
* 函数名称：SetStorageProcess
* 功能描述：设置存贮的进度
* 访问的表：无
* 修改的表：无
* 输入参数：0-100
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::SetStorageProgress(uint32 progress)
{
    if (progress > 100)
    {
        progress = 100;
    }

    _op_progress = VM_NET_PROCESS + VM_STORAGE_PROCESS*progress/100;
}

/****************************************************************************************
* 函数名称：SetComputerProcess
* 功能描述：设置计算的进度
* 访问的表：无
* 修改的表：无
* 输入参数：0-100
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::SetComputerProgress(uint32 progress)
{
    if (progress > 100)
    {
        progress = 100;
    }

    _op_progress = VM_STORAGE_PROCESS + VM_NET_PROCESS + VM_COMPUTER_PROCESS*progress/100;
}

/****************************************************************************************
* 函数名称：SetProcess
* 功能描述：设置操作命令的进度
* 访问的表：无
* 修改的表：无
* 输入参数：0-100
* 输出参数：
* 返 回 值：
* 其它说明：一般用在其他命令的执行进度
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::SetProgress(uint32 progress)
{
    if (progress > 100)
    {
        progress = 100;
    }

    // 虚拟机所有操作默认从1开始
    if (progress == 0)
    {
        progress = 1;
    }

    _op_progress = progress;
}

/****************************************************************************************
* 函数名称：RefrushVmState
* 功能描述：刷新虚拟机状态
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::RefreshVmState()
{
    // 查找当前虚拟机的状态
    _state = _vm_driver->GetDomainState(_instance_name);
}

/****************************************************************************************
* 函数名称：InstanceSave
* 功能描述：保存虚拟机为映像
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::InstanceUpload(const string& message)
{
    // 实现参数的解析
    VmSaveReq req;
    req.deserialize(message);

    // 判断当前的虚拟机是否是关机态
    if (VM_SHUTOFF != _state)
    {
        return VMOP_ERROR_STATE;
    }

    if (0 == vm_cfg._machine._size)
    {
        return VMOMP_UPLOAD_ERROR;
    }

    // 调用存储接口，上传img到IMG服务器上去
    if (SUCCESS != _storage_instance->start_upload_img(_instance_id, req, vm_cfg._machine, vm_option._image_store_option))
    {
        return VMOMP_UPLOAD_ERROR;
    }

    int progress = 0;
    int64 finish = 0;
    int state = UPLOAD_STATE_INIT;
    while(1)
    {
        Delay(1000);
        _storage_instance->query_upload_img(_instance_id,vm_cfg._machine,finish,state);
        if (!InstanceIsUnSave())
        {
            switch(state)
            {
            case UPLOAD_STATE_FAILED:
                return VMOMP_UPLOAD_ERROR;
                break;
            case UPLOAD_STATE_UPLOAD:
                progress = (finish*100)/vm_cfg._machine._size;
                SetProgress(progress);
                break;
            case UPLOAD_STATE_CANCEL:
                return VMOP_SUCC;
                break;
            case UPLOAD_STATE_FINISH:
                SetProgress(100);
                return VMOP_SUCC;
                break;
            default:
                break;
            }
        }
        else
        {
            _storage_instance->cancel_upload_img(_instance_id,vm_cfg._machine);
            return VMOP_ERROR_CANCEL_ABORT;
            break;
        }
    };

    return VMOP_SUCC;
}

/****************************************************************************************
* 函数名称：InstanceUnsave
* 功能描述：取消保存虚拟机为映像
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::InstanceCancelUpload()
{
    // 不做任何事情，直接返回成功
    SetProgress(50);
    return _storage_instance->stop_upload_img(_instance_id,vm_cfg._machine);
}

/****************************************************************************************
* 函数名称：DomainIsExist
* 功能描述：判断虚拟机是否存在
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
* modify by wangtl: makesure dom is not exit
*                            0:  not exist
*                          -1: unkown
*                            1:  exist
****************************************************************************************/
int  VmInstance::DomainIsExist() const
{
    int      domid = _vm_driver->GetDomainIDByName(_instance_name);
    int      exist   = _vm_driver->IsDomExist(_instance_name, _domain_id);

    if (0 < domid || 1 == exist)
    {
        return DOM_EXIST;
    }

    if (-1 == domid && 0 == exist)
    {
        return DOM_NOT_EXIST;
    }

    return DOM_UNKNOWN;
}

/****************************************************************************************
* 函数名称：StartDomain
* 功能描述：打开虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::StartDomain()
{
    bool     result = false;
    int      domid = 0;

    string config_file = tecs_instance_path+_instance_name+"/"+_instance_name+".xml";

    // 如果虚拟机不存在，走创建流程
    if (!DomainIsExist())
    {
        SetProgressDesc(START_EXEC_CREATE);
        result = _vm_driver->Create(config_file, &domid);

        // 创建虚拟机失败的时候，不再设置权重
        if ((result == false) || (domid == 0))
        {
            // 返回操作失败
            OutPut(SYS_ALERT,"Create %s Fail.\n",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }

        // 更新dominid
        _domain_id = domid;

        // 通知网络那边，dominid变化了
        if (!NotifyNetDomIDChg(__FUNCTION__))
        {
            return VMOP_ERROR_NETWORK_NOTIFY;
        }

        // 把虚拟机的核进行全核bind
        _vm_driver->VcpuAffinitySet(_instance_name, vm_cfg._vcpu);
        // 设置虚拟机的weight和cap
        _vm_driver->Sched_credit_weight_set(_instance_name, vm_cfg._cpu_percent);

        // 如果TCU共享，则不限值CAP值
        if (vm_option._is_tcu_share)
        {
            _vm_driver->Sched_credit_cap_set(_instance_name, vm_cfg._vcpu, 100);
        }
        else
        {
            _vm_driver->Sched_credit_cap_set(_instance_name, vm_cfg._vcpu, vm_cfg._cpu_percent);
        }


        // 设置系统启动时间,防止系统被过于频繁重启
        SetBootTime();

        // 加入USB设备到虚拟机里面
        STATUS ret = InstanceAddUsb();
        if ( ret != SUCCESS)
        {
            return ret;
        }

        return VMOP_SUCC;
    }

    // 刷新虚拟机状态
    RefreshVmState();

    if (VM_RUNNING == _state)
    {
        return VMOP_SUCC;
    }

    OutPut(SYS_ERROR,"%s state %d can't Start.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* 函数名称：StopDomain
* 功能描述：关闭虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::StopDomain()
{
    // 如果虚拟机不存在，直接返回成功
    if (!DomainIsExist())
    {
        return VMOP_SUCC;
    }

    // 刷新虚拟机状态
    RefreshVmState();

    // 如果是关机态，也直接返回成功
    if (VM_SHUTOFF == _state)
    {
        return VMOP_SUCC;
    }

    // 如果是运行态，则执行关机命令
    if (VM_RUNNING == _state)
    {
        SetProgressDesc(START_EXEC_SHUTDOWN);
        if (-1 == _vm_driver->ShutDown(_instance_name))
        {
            OutPut(SYS_ALERT,"ShutDown %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }

        if (!StopNetWork(__FUNCTION__, VNET_VM_CANCEL))
        {
            return VMOP_ERROR_NETWORK_STOP;
        }

        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Stop.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* 函数名称：StopDomain
* 功能描述：挂起虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::PauseDomain()
{
    // 如果虚拟机不存在，返回失败
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"Suspend %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // 刷新虚拟机状态
    RefreshVmState();

    // 如果是挂起态，也直接返回成功
    if (VM_PAUSED == _state)
    {
        return VMOP_SUCC;
    }

    // 如果是运行态，则执行挂起命令
    if (VM_RUNNING == _state)
    {
        SetProgressDesc(START_EXEC_PAUSE);
        if (-1 == _vm_driver->Suspend(_instance_name))
        {
            OutPut(SYS_ALERT,"Suspend %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }
        // 挂起命令执行完的时候，需要停止看门狗
        if (!StopWDGTimer(__FUNCTION__))
        {
            return VMOP_ERROR_WATCHDOG_STOP;
        }
        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Pause.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* 函数名称：ResumeDomain
* 功能描述：恢复虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::ResumeDomain()
{
    // 如果虚拟机不存在，返回失败
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"resume %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // 刷新虚拟机状态
    RefreshVmState();

    // 如果是运行态，也直接返回成功
    if (VM_RUNNING == _state)
    {
        return VMOP_SUCC;
    }

    // 如果是挂起态，则执行恢复命令
    if (VM_PAUSED == _state)
    {
        SetProgressDesc(START_EXEC_RESUME);
        if (-1 == _vm_driver->Resume(_instance_name))
        {
            OutPut(SYS_ALERT,"Resume %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }
        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Resume.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* 函数名称：RebootDomain
* 功能描述：重启虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::RebootDomain(bool need_stop_wgd )
{
    // 如果虚拟机不存在，返回失败
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"reboot %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // 刷新虚拟机状态
    RefreshVmState();

    // 如果是运行态，执行REBOOT命令
    if (VM_RUNNING == _state)
    {
        SetProgressDesc(START_EXEC_REBOOT);
        if (-1 == _vm_driver->Reboot(_instance_name))
        {
            OutPut(SYS_ALERT,"Reboot %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }
        // 重启后先停止看门狗
        if(need_stop_wgd)
        {
            if (!StopWDGTimer(__FUNCTION__))
            {
                return VMOP_ERROR_WATCHDOG_STOP;
            }
        }
        // 设置系统启动时间,防止系统被过于频繁重启
        SetBootTime();
        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Reboot.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* 函数名称：ResetDomain
* 功能描述：强制重启虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::ResetDomain(bool need_stop_wgd )
{
    // 如果虚拟机不存在，返回失败
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"Reset %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // 刷新虚拟机状态
    RefreshVmState();

    // 如果是运行态，执行reset命令
    if (VM_RUNNING == _state || VM_PAUSED == _state || VM_CRASHED == _state)
    {
        SetProgressDesc(START_EXEC_RESET);
        string config_file = tecs_instance_path+_instance_name+"/"+_instance_name+".xml";
        if (0 != _vm_driver->Reset(_instance_name, config_file))
        {
            OutPut(SYS_ALERT,"Reset %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }

        // 停止看门狗
        if(need_stop_wgd)
        {
            if (!StopWDGTimer(__FUNCTION__))
            {
                return VMOP_ERROR_WATCHDOG_STOP;
            }
        }
        // 设置系统启动时间,防止系统被过于频繁重启
        SetBootTime();
        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Reset.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* 函数名称：DestroyDomain
* 功能描述：强制关闭虚拟机
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::DestroyDomain()
{
    int max_delay = 10;
    int exist = DomainIsExist();
    int ret = 0;

    if (DOM_NOT_EXIST == exist)                        /*  not exit , then returen  */
    {
        return VMOP_SUCC;
    }

    if (DOM_UNKNOWN == exist)
    {
        SkyExit(-1, "main: call DomainIsExist(DestroyDomain) failed!");
    }

    SetProgressDesc(START_EXEC_DESTROY);
    ret = _vm_driver->DestroyDomain(_instance_name);     /*  destroy dom  */

    exist = DomainIsExist();
    while (DOM_EXIST == exist && !ret && max_delay)     /*  make sure the dom does not exist  */
    {
        exist = DomainIsExist();
        Delay(1000);
        max_delay--;
    }

    exist = DomainIsExist();                     /*  dom still exist,  so return fail  */
    if (DOM_EXIST == exist)
    {
        OutPut(SYS_ALERT,"Destroy %s Fail.",_instance_name.c_str());
        if (ret)
        {
            OutPut(SYS_ALERT,"_vm_driver->DestroyDomain  %s Fail.",_instance_name.c_str());
        }
        return VMOP_VM_DO_FAIL;
    }
    else if (DOM_UNKNOWN == exist)
    {
        SkyExit(-1, "main: call DomainIsExist(DestroyDomain) failed!");
    }

    if (!StopNetWork(__FUNCTION__, VNET_VM_CANCEL))
    {
        return VMOP_ERROR_NETWORK_STOP;
    }

    return VMOP_RUNNING;
}
/****************************************************************************************
* 函数名称：RecoverDomain
* 功能描述：虚拟机一键恢复镜像
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2013/05/26    V1.0   zhanghao        创建
****************************************************************************************/
STATUS VmInstance::RecoverDomain(const string& message)
{
    VmRecoverImageReq  recoverreq;
    recoverreq.deserialize(message);

    int ret = _storage_instance->recover_image(recoverreq._config._vid, recoverreq._config._machine);
    if(ERROR == ret)
    {
        return VMOP_RECOVER_ERROR;
    }
    else
    {
        return VMOP_SUCC;
    }
}

/****************************************************************************************
* 函数名称：SetDiskInfo
* 功能描述：设置disk信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    yanwei         创建
****************************************************************************************/
void VmInstance::SetDiskInfo(vector<VmDiskConfig> &disks)
{
    string ctx_name = tecs_instance_path + _instance_name + "/tecs-context.iso";

    if(vm_cfg._kernel._id != INVALID_FILEID)
    {
        disks.push_back(vm_cfg._kernel);
    }

    if(vm_cfg._initrd._id != INVALID_FILEID)
    {
        disks.push_back(vm_cfg._initrd);
    }

    if(vm_cfg._machine._id != INVALID_FILEID)
    {
        disks.push_back(vm_cfg._machine);
    }

    vector<VmDiskConfig>::iterator it;
    for(it = vm_cfg._disks.begin(); it != vm_cfg._disks.end(); it++)
    {
        if (it->_source == ctx_name)
        {
            continue;
        }
        disks.push_back(*it);
    }
}

/****************************************************************************************
* 函数名称：GetDiskInfo
* 功能描述：获取该实例中disk的信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    yanwei         创建
****************************************************************************************/
void VmInstance::GetDiskInfo(vector<VmDiskConfig> &disks)
{
    vector<VmDiskConfig>::iterator it;
    vector<VmDiskConfig>::iterator itcfg = vm_cfg._disks.begin();
    string ctx_name = tecs_instance_path + _instance_name + "/tecs-context.iso";

    for (it = disks.begin(); it != disks.end(); it++)
    {
        if ((INVALID_FILEID != it->_id) && (it->_id == vm_cfg._kernel._id))
        {
            vm_cfg._kernel._source = it->_source;
            vm_cfg._kernel._source_type = it->_source_type;
        }
        else if ((INVALID_FILEID != it->_id) && (it->_id == vm_cfg._initrd._id))
        {
            vm_cfg._initrd._source = it->_source;
            vm_cfg._initrd._source_type = it->_source_type;
        }
        else if ((INVALID_FILEID != it->_id) && (it->_id == vm_cfg._machine._id))
        {
            vm_cfg._machine._source = it->_source;
            vm_cfg._machine._source_type = it->_source_type;
        }
        else
        {
            if (itcfg->_source == ctx_name)
            {
                itcfg++;
            }
            itcfg->_source = it->_source;
            itcfg->_size   = it->_size;
            itcfg->_source_type = it->_source_type;
            itcfg++;
        }
    }
}
/****************************************************************************************
* 函数名称：PredealStorage
* 功能描述：存储预处理
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：disks 配置的disk列表
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/19    V1.0    yanwei         创建
****************************************************************************************/
void VmInstance::PredealStorage(const vector<VmDiskConfig> &disks)
{
    vector<VmDiskConfig> exist_disks;

    _storage_instance->scan_storage(_instance_id,exist_disks,vm_option._image_store_option);

    if (0 == exist_disks.size())
    {
        return;
    }

    vector<VmDiskConfig>::iterator exist_it;
    vector<VmDiskConfig>::const_iterator it;
    for (exist_it = exist_disks.begin(); exist_it != exist_disks.end(); exist_it++)
    {
        for (it = disks.begin(); it != disks.end(); it++)
        {
#if 0
            if ((exist_it->_id == it->_id)
                    && (exist_it->_size == it->_size))
            {
                if (INVALID_FILEID != it->_id)
                {
                    if ((it->_type == exist_it->_type) && (IMAGE_TYPE_DATABLOCK == it->_type))
                    {
                        if (exist_it->_target != it->_target)
                        {
                            continue;
                        }
                    }
                }
                else
                {
                    if (exist_it->_target != it->_target)
                    {
                        continue;
                    }
                }
                break;
            }
#endif
            if (exist_it->_id == it->_id)
            {
                //重要:磁盘暂不支持 映像只用id来判断.如果需要支持硬盘的话，需考虑新的方案。由CC决定删除哪些存储项.
                break;
            }
        }
        if (it == disks.end())
        {
            _storage_instance->release_storage(_instance_id,*exist_it,vm_option._image_store_option);
        }
    }
}
/****************************************************************************************
* 函数名称：StartStorage
* 功能描述：存储初始化
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
*     存储环境准备，包括映像下载和虚拟存储的分配
*     也可能本地已就绪，由存储提供的接口负责屏蔽
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/19    V1.0    yanwei         创建
****************************************************************************************/
STATUS VmInstance::StartStorage()
{
    vector<VmDiskConfig> disks;

    SetDiskInfo(disks);
    SetStorageProgress(0);
    PredealStorage(disks);
    if (0 == disks.size())
    {
        SetStorageProgress(100);
        return SUCCESS;
    }

    int imgs = 0,blocks = 0;
    vector<VmDiskConfig>::iterator it;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        if (INVALID_FILEID != it->_id)
        {
            imgs++;
        }
        else
        {
            blocks++;
        }
    }
    //img下载占整个存储进度的80% 每个img占80/imgs;块设备占存储进度的20% 每个block占20/blocks
    if (0 == blocks)
    {
        imgs = 100/imgs;
    }
    else
    {
        imgs = 80/imgs;
        blocks = 20/blocks;
    }
    //imgs为每个映像所占存储的进度 blocks为每个块设备所占存储的进度
    int progress = 0,itemprogress = 0,checksums = 0,downloads = 0;
    bool is_first;
    for (it = disks.begin(); it != disks.end(); it++)
    {
        itemprogress = 0;
        is_first = true;
    storage_begin:
        int iRet=_storage_instance->get_storage(_instance_id,*it,vm_option._image_store_option);
        if (SUCCESS == iRet ||ERROR_LOAD_NEED_REFRESH==iRet)
        {
            if(ERROR_LOAD_NEED_REFRESH==iRet)
            {
                TimeOut timeout;
                timeout.type     = RELATIVE_TIMER;
                timeout.duration = load_refresh_period;    // 默认定时120S
                timeout.msgid    = EV_TIMER_LOAD;
                STATUS rc = _mu->SetTimer(_load_timer, timeout);
                if (SUCCESS != rc)
                {
                    OutPut(SYS_ALERT, "%s SetLoadTimer failed rc = %d!\n",
                           _instance_name.c_str(),rc);
                    SkyAssert(false);
                }

            }
            while (1)
            {
                /*等待完成*/
                int state;
                Delay(1000);
                if (!InstanceIsDestroy())
                {
                    int64 cursize;
                    _storage_instance->query_storage(_instance_id,
                                                     *it,
                                                     vm_option._image_store_option,
                                                     cursize,
                                                     state);
                    if (STORAGE_ITEM_FAILED == state)
                    {
                        OutPut(SYS_ERROR,"get_storage failed,vid:%lld,image:%lld\n",_instance_id,it->_id);
                        //611003915111 再get_storage一次                        
                        if(is_first)
                        {
                            is_first = false;
                            goto storage_begin;
                        } 
                        return VMOP_ERROR_STORAGE_START;
                    }
                    else if (STORAGE_ITEM_NOT_READY == state)
                    {
                        itemprogress = 0;
                    }
                    else if (STORAGE_ITEM_TEMP_READY == state)
                    {
                        //tmp起点为该映像的5% 整个下载占60%
                        itemprogress = 5 + (((cursize*100)/it->_size) * 6)/10;
                        if (cursize < it->_size)
                        {
                            downloads++;
                        }
                        else
                        {
                            //计算md5占15%
                            checksums++;
                            //大概每10秒进度加1 且进度不能超过80%
                            itemprogress += checksums/10;
                            if (80 < itemprogress)
                            {
                                itemprogress = 80;
                            }
                        }
                    }
                    else if (STORAGE_ITEM_BASE_READY == state)
                    {
                        //base准备好进度为85
                        itemprogress = 85;
                    }
                    else if (STORAGE_ITEM_SS_READY == state)
                    {
                        //快照准备好进度为95
                        itemprogress = 95;
                    }
                    else
                    {
                        itemprogress = 100;
                    }
                    //折算出该存储项的进度
                    if (INVALID_FILEID != it->_id)
                    {
                        itemprogress = (itemprogress*imgs)/100;
                    }
                    else
                    {
                        itemprogress = (itemprogress*blocks)/100;
                    }
                    SetStorageProgress(progress+itemprogress);
                }
                else
                {
                    /*收到cancel消息 直接退出*/
                    if (INVALID_FILEID != it->_id)
                    {
                        _storage_instance->cancel_storage(_instance_id,it->_id);
                    }
                    return VMOP_ERROR_CANCEL_ABORT;
                }

                if (STORAGE_ITEM_READY == state)
                {
                    break;
                }
            }
        }
        else
        {
            return VMOP_ERROR_STORAGE_START;
        }
        if (INVALID_FILEID != it->_id)
        {
            OutPut(SYS_DEBUG,"image:%lld,size:%lld,download:%d,checksum:%d\n",it->_id,it->_size,downloads,checksums);
            progress += imgs;
            downloads = 0;
            checksums = 0;
        }
        else
        {
            progress += blocks;
        }
    }

    GetDiskInfo(disks);

    SetStorageProgress(100);
    return VMOP_SUCC;
}
/****************************************************************************************
* 函数名称：ReleaseStorage
* 功能描述：释放存储资源
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
*     存储资源释放，包括映像删除和虚拟存储的回收
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/19    V1.0    yanwei         创建
****************************************************************************************/
STATUS VmInstance::ReleaseStorage()
{
    STATUS ret = SUCCESS;//某一项释放失败暂不返回,为了是每一次cancel尽量释放更多的存储资源
    STATUS tmp_ret = SUCCESS;
    vector<VmDiskConfig> disks;
    int retry = 0;

    SetDiskInfo(disks);
    SetStorageProgress(0);
    if (0 == disks.size())
    {
        retry = 0;
        while (retry < 3)
        {
            tmp_ret = _storage_instance->release_storage(_instance_id);
            if (SUCCESS == tmp_ret)
            {
                break;
            }
            Delay(1000);
            retry ++;
        }
        if (SUCCESS != tmp_ret)
        {//多次尝试不成功返回失败由cc继续跟催
            ret = tmp_ret;
        }
    }
    else
    {
        int progress = 0,itemprogress = 0;
        itemprogress = 100/disks.size();
        vector<VmDiskConfig>::iterator it;
        for (it = disks.begin(); it != disks.end(); it++)
        {
            retry = 0;
            while (retry < 3)
            {
                tmp_ret = _storage_instance->release_storage(_instance_id,*it,vm_option._image_store_option);
                if (SUCCESS == tmp_ret)
                {
                    break;
                }
                Delay(1000);
                retry ++;
            }
            if (SUCCESS != tmp_ret)
            {//多次尝试不成功返回失败由cc继续跟催
                ret = tmp_ret;
            }
            progress += itemprogress;
            SetStorageProgress(progress);
        }
    }
    SetStorageProgress(100);
    return ret;
}

/*********************************************************************
 *    配置文件相关                                                   *
 *    路径: /var/lib/tecs/instances/instancename/instancename.xml    *
 ********************************************************************/

/****************************************************************************************
* 函数名称：CreateConfigFile
* 功能描述：生成虚拟机实例的配置文件
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
* 2012/06/04    V1.0    GuoXiang    修改:网卡驱动设置成可配
****************************************************************************************/
int VmInstance::CreateConfigFile()
{
    // 构造xmlparse
    if (!CreateCfgParaIsValid())
    {
        return -1;
    }

    /* 构造输出文件 */
    string filename = tecs_instance_path+_instance_name+"/"+_instance_name+".xml";
    string s = _vm_driver->CreateConfig(_instance_name, vm_cfg);
    return CreateFile(filename, s);
}

/************************************************************************/
bool VmInstance::CreateCfgParaIsValid()
{
    if(0 == vm_cfg._vcpu)
    {
        OutPut(SYS_DEBUG,"vm_cfg._vcpu is 0!!\n");
        return false;
    }

    if(0 == vm_cfg._memory)
    {
        OutPut(SYS_DEBUG,"vm_cfg._memory is 0!!\n");
        return false;
    }

    if((!vm_cfg._machine._source.empty())
            &&(!vm_cfg._kernel._source.empty())
            &&(!vm_cfg._initrd._source.empty()))
    {
        OutPut(SYS_DEBUG,"In vm_cfg, machine&kernel&initrd is not empty!!\n");
        return false;
    }

    if(vm_cfg._virt_type.empty())
    {
        OutPut(SYS_DEBUG,"vm_cfg._virt_type is empty!!\n");
        return false;
    }

    if(vm_cfg._virt_type.compare("pvm") && vm_cfg._virt_type.compare("hvm"))
    {
        OutPut(SYS_DEBUG,"vm_cfg._virt_type is %s, error!!\n", vm_cfg._virt_type.c_str());
        return false;
    }

    return true;

}




/************************************************************************
 * CONTEXT 相关                                                         *
 * 路径 /var/lib/tecs/instances/instancename/iso/tecs-context.xml       *
 *      /var/lib/tecs/instances/instancename/tecs-context.iso           *
 *                                                                      *
 ***********************************************************************/

/*******************************************************************************
* 函数名称：CreateFile
* 功能描述：将内容输出到某个文件中
* 输入参数：filename 文件名；filecontext 文件内容
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
* 2012/05/30    V1.0    钟春山       网口顺序代码
*******************************************************************************/
int VmInstance::CreateFile(const string &filename, const string &filecontext)
{
    ofstream file;

    if(filename.empty())
    {
        OutPut(SYS_ERROR,"CreateFile: filename or filecontext invalid!\n");
        return -1;
    }

    file.open(filename.c_str(), ios::out);
    if (file.fail() == true)
    {
        OutPut(SYS_ERROR,"CreateFile: Could not open context file:%s\n", filename.c_str());
        return -1;
    }
    file << filecontext << endl;
    file.close();

    return SUCCESS;
}

/*******************************************************************************
* 函数名称：CreateTecsContext
* 功能描述：将虚拟机信息收集，并生成virtual.xml，保存到iso目录下。
* 输入参数：无
* 输出参数：
* 返 回 值：CREATE_EMPTY 空内容，无需写文件
*           CREATE_ERROR 文件创建失败
*           CREATE_SUCC  文件创建成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------------
* 2012/05/30    V1.0    钟春山       创 建
*******************************************************************************/
int VmInstance::CreateTecsContext()
{
    string name;
    string context;

    //获取要写入iso的内容
    context = CreateVmMetadata(this);
    if(context.empty())
    {
        OutPut(SYS_DEBUG,"No need to create tecs context!\n");
        return CREATE_EMPTY;
    }

    //先创建存放文件的目录
    name = tecs_instance_path + _instance_name + "/iso/tecs";
    
    //string mkdir_cmd = " mkdir -p " + name;
    //if(0 != system(mkdir_cmd.c_str()))
    if(0 != CreateDir(name, 0))
    {
        OutPut(SYS_ERROR,"CreateTecsContext: mkdir %s failed!\n", name.c_str());
        return CREATE_ERROR;
    }
    //输出到文件中
    name.append("/vm.xml");
    if(0 != CreateFile(name, context))
    {
        OutPut(SYS_ERROR,"Could not create tecs context file!\n");
        return CREATE_ERROR;
    }

    extern string hostxml_full_path;

    string cp_host_xml = "cp -f " + hostxml_full_path + " "
                         + tecs_instance_path + _instance_name + "/iso/tecs";
    if(0 != system(cp_host_xml.c_str()))
    {
        OutPut(SYS_ERROR,"CreateTecsContext: cp host.xml failed!\n");
        return CREATE_ERROR;
    }

    return CREATE_SUCC;
}

/*******************************************************************************
* 函数名称：CreateUserContext
* 功能描述：根据调度下发的context字段，生成相应xml文件，保存到iso目录下。
* 输入参数：无
* 输出参数：
* 返 回 值：CREATE_EMPTY 空内容，无需写文件
*           CREATE_ERROR 文件创建失败
*           CREATE_SUCC  文件创建成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------------
* 2012/05/30    V1.0    钟春山       创 建
*******************************************************************************/
int VmInstance::CreateUserContext()
{
    string name;
    string context;

    if(vm_cfg._context.empty())
    {
        OutPut(SYS_DEBUG,"No need to create user context!\n");
        return CREATE_EMPTY;
    }

    //先创建存放的目录
    name = tecs_instance_path + _instance_name + "/iso/user";
    string mkdir_cmd = " mkdir -p " + name;
    if(0 != system(mkdir_cmd.c_str()))
    {
        OutPut(SYS_CRITICAL,"CreateUserContext: mkdir %s failed!\n", mkdir_cmd.c_str());
        return CREATE_ERROR;
    }

    map<string,string>::iterator it;
    for(it = vm_cfg._context.begin(); it != vm_cfg._context.end(); it++)
    {
        name = tecs_instance_path + _instance_name + "/iso/user/" + it->first;
        if(0 != CreateFile(name, it->second))
        {
            OutPut(SYS_CRITICAL,"Could not create user context file!\n");
            return CREATE_ERROR;
        }
    }

    return CREATE_SUCC;
}

/*******************************************************************************
* 函数名称：CreateContext
* 功能描述：创建用户数据文件和虚拟机元数据文件
* 输入参数：无
* 输出参数：
* 返 回 值：CREATE_EMPTY 空内容，无需写文件
*           CREATE_ERROR 文件创建失败
*           CREATE_SUCC  文件创建成功
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------------
* 2012/05/30    V1.0    钟春山       创 建
*******************************************************************************/
int VmInstance::CreateContext()
{
    int tecs_ret;
    int user_ret;

    tecs_ret = CreateTecsContext();
    if(CREATE_ERROR == tecs_ret)
    {
        return CREATE_ERROR;
    }

    user_ret = CreateUserContext();
    if(CREATE_ERROR == user_ret)
    {
        return CREATE_ERROR;
    }

    //都为空不需要创建返回1，失败返回-1，成功返回0
    if((tecs_ret == CREATE_EMPTY)&&(user_ret == CREATE_EMPTY))
    {
        return CREATE_EMPTY;
    }
    return CREATE_SUCC;
}
/****************************************************************************************
* 函数名称：CreateContextIso
* 功能描述：使用mkisofs工具，将./iso/tecs-context.txt生成tecs-context.iso
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         创建
****************************************************************************************/
/*
   DST_HASH=`echo -n $DST | $MD5SUM | $AWK '{print $1}'`

   TMP_DIR="/var/lib/one/$DST_HASH
   TMP_DIR="$ONE_LOCATION/var/$DST_HASH

   DST_PATH =`arg_path $DST`
   DST_DIR  =`dirname $DST_PATH`
   DST_FILE =`basename $DST_PATH`

   ISO_DIR="$TMP_DIR/isofiles"

   MKISOFS=/usr/bin/mkisofs;

*/
/* $MKISOFS -o $TMP_DIR/$DST_FILE -J -R $ISO_DIR
 * "mkisofs -o tt.iso -J -R ./iso"
 */
int VmInstance::CreateContextIso()
{
    int rc = 0;

    string cmd("mkisofs -o ");
    cmd = cmd + tecs_instance_path+_instance_name+"/tecs-context.iso -J -R ";
    cmd = cmd + tecs_instance_path+_instance_name+"/iso";

    rc = system(cmd.c_str());

    return rc;
}

/****************************************************************************************
* 函数名称：GetContextDiskTarget
* 功能描述：获取上下文磁盘的目标磁盘
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：新版本方案中，上下文只存放在hdd上!
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixiaocheng  创建
****************************************************************************************/
string VmInstance::GetContextDiskTarget()
{
    return "hdd";
}

/****************************************************************************************
* 函数名称：SendVmOprAckMsg
* 功能描述：回操作结果应答给CC
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixiaocheng  创建
****************************************************************************************/
void VmInstance::SendVmOprAckMsg(const MID &receiver, string &action_id, MessageUnit *mu)
{
    uint32 result   = _op_result;
    uint32 progress = _op_progress;

    // 避免应答WorkAck中 stat为VMOP_RUNNING，而进度progress为100
    if ((VMOP_RUNNING == _op_result)
            &&(100 == _op_progress))
    {
        progress = 99;
    }

    SendVmOprAckMsg(receiver,  result,
                    _op_cmd,   progress,
                    action_id, mu);
}

/****************************************************************************************
* 函数名称：SendVmOprAckMsg
* 功能描述：回操作结果应答给CC
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixiaocheng  创建
****************************************************************************************/
void VmInstance::SendVmOprAckMsg(const MID &receiver, uint32 result,
                                 int32 op_cmd, uint32 progress,
                                 string &action_id, MessageUnit *mu)
{
    if (NULL == mu)
    {
        OutPut(SYS_ALERT,
               "** SendVmOprAckMsg: send ack fail,mu is null, vm_id: %lld, name: %s\n",
               _instance_id, _instance_name.c_str());
        return;
    }

    // 如果当前是虚拟机正在进行，则记录当前的操作细节
    string detail;
    if(VMOP_RUNNING == result)
    {
        detail = _op_process_desc;
    }
    else
    {
        detail = Enum2Str::VmOpError(result);

        /* 成功失败，都需要设置，除了busy */
        if(result != VMOP_VM_AGT_BUSY)
        {
            SetOpCmd(UNKNOWN_OP);
            CleanAckEndFlag();
        }
        
    }

    VmOperationAck AckMsg(action_id,
                          result,
                          progress,
                          detail,
                          _instance_id,
                          op_cmd,
                          _state);

    MessageOption option(receiver, EV_VM_OP_ACK, 0, 0);

    STATUS ret = mu->Send(AckMsg, option);
    if(SUCCESS != ret)
    {
        OutPut(SYS_ALERT,
               "** SendVmOprAckMsg: send ack fail, return: %d --- vm_id: %lld, name: %s, msg content:\n %s\n",
               ret, _instance_id, _instance_name.c_str(), AckMsg.serialize().c_str());
    }
    else
    {
        OutPut(SYS_INFORMATIONAL,
               "** SendVmOprAckMsg: send ack success --- vm_id: %lld, name: %s, msg content:\n %s\n",
               _instance_id, _instance_name.c_str(), AckMsg.serialize().c_str());
    }

}

/*******************************************************************************
* 函数名称：PrepareResource
* 功能描述：虚拟机创建环境准备
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixiaocheng  创建
*******************************************************************************/
void VmInstance::SendVmOpAck2Wdg(const MID &receiver, CVNetNotifyHCrestatMsgAck &ack,
                                 int32 msg_id,      MessageUnit *mu)
{
    if (NULL == mu)
    {
        OutPut(SYS_ALERT,
               "** SendVmOpAck2Wdg: send ack fail,mu is null, vm_id: %lld, name: %s, msg content:\n %s\n",
               _instance_id, _instance_name.c_str(), ack.serialize().c_str());
        return;
    }

    ack.state    = _op_result;
    ack.progress = _op_progress;

    // 避免应答WorkAck中 stat为VMOP_RUNNING，而进度progress为100
    if ((VMOP_RUNNING == _op_result)
            &&(100 == _op_progress))
    {
        ack.progress = 99;
    }

    MessageOption option(receiver, msg_id, 0, 0);

    STATUS ret = mu->Send(ack, option);
    if(SUCCESS != ret)
    {
        OutPut(SYS_ALERT,
               "** SendVmOpAck2Wdg: send ack fail, return: %d --- vm_id: %lld, name: %s, msg content:\n %s\n",
               ret, _instance_id, _instance_name.c_str(), ack.serialize().c_str());
    }
    else
    {
        OutPut(SYS_INFORMATIONAL,
               "** SendVmOpAck2Wdg: send ack success --- vm_id: %lld, name: %s, msg content:\n %s\n",
               _instance_id, _instance_name.c_str(), ack.serialize().c_str());
    }
}

/*******************************************************************************
* 函数名称：SendDel2VmHandler
* 功能描述：通知VM_HANDLER删除本实例
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------------
* 2013/06/29    V1.0    chenww  创建
*******************************************************************************/
void VmInstance::SendDel2VmHandler( )
{
    //SkyAssert(VMOP_SUCC == _op_result);

    //SkyAssert(100 == _op_progress);

    // 记录当前的操作细节
    string    detail = Enum2Str::VmOpError(_op_result);
    VmOperationAck AckMsg(_op_action_id,
                          _op_result,
                          _op_progress,
                          detail,
                          _instance_id,
                          _op_cmd,
                          _state);

    MID           receiver(MU_VM_HANDLER);
    MessageOption option(receiver, EV_DEL_VM_INSTANCE_REQ, 0, 0);

    STATUS ret = _mu->Send(AckMsg, option);
    if(SUCCESS != ret)
    {
        OutPut(SYS_ALERT,
               "** SendDelReq2VmHandler: send ack fail, return: %d --- vm_id: %lld, name: %s, msg content:\n %s\n",
               ret, _instance_id, _instance_name.c_str(), AckMsg.serialize().c_str());
    }
    else
    {
        OutPut(SYS_INFORMATIONAL,
               "** SendDelReq2VmHandler: send ack success --- vm_id: %lld ***\n",
               _instance_id);

    }

}
/****************************************************************************************
* 函数名称：PrepareResource
* 功能描述：虚拟机创建环境准备
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixiaocheng  创建
****************************************************************************************/
STATUS VmInstance::PrepareResource(bool IsMigration)
{
    int    rc = VMOP_SUCC;
    BYTE   option = 0;


    option = IsMigration?  VNET_VM_PREPARE : VNET_VM_DEPLOY;

    OutPut(SYS_DEBUG, "--------begin StartupVmInstanceThread--------\n");
    OutPut(SYS_DEBUG, "kernel name: %s\n", vm_cfg._kernel._source.c_str());
    OutPut(SYS_DEBUG, "initrd name: %s\n", vm_cfg._initrd._source.c_str());
    OutPut(SYS_DEBUG, "machine name: %s\n", vm_cfg._machine._source.c_str());

    // 1、建目录,直接建到/var/lib/tecs/instances/instanceid/iso/
    string instance_iso_dir = tecs_instance_path + _instance_name + "/iso";
    string mkdir_cmd        = " mkdir "+instance_iso_dir+"  -p ";

    system(mkdir_cmd.c_str());

    if(IsXenKernel())
    {
        // 挂载 coredump url
        SetProgressDesc(START_MOUNT_COREDUMP_URL);
        string coredump_save_local_dir;
        GetCoredumpFilePath(coredump_save_local_dir);

        // 把CC传递过来的远程目录进行mount
        if (vm_cfg._core_dump_url.ip != "")
        {
            rc = MountNfs(vm_cfg._core_dump_url.ip, vm_cfg._core_dump_url.path,
                          coredump_save_local_dir, TO_FORCE_DIR);
            if (SUCCESS != rc)
            {
                OutPut(SYS_ALERT, "mount %s:/%s to %s failed, code:0x%x. \n",
                       vm_cfg._core_dump_url.ip.c_str(), vm_cfg._core_dump_url.path.c_str(),
                       coredump_save_local_dir.c_str(), rc);
                return VMOP_ERROR_MOUNT_COREDUMP_URL;
            }
        }
    }

    // 2、init network,记录返回值
    SetProgressDesc(START_PREP_NET);
    OutPut(SYS_DEBUG, "%s begin applying network resource,vid:%d,\n", _instance_name.c_str(), _domain_id);
    bool is_success = StartNetWork(__FUNCTION__, option);
    if(!is_success)
    {
        return CREATE_ERROR_STARTNETWORK;
    }
    OutPut(SYS_DEBUG, "%s  applying network resource succ,vid:%d,\n", _instance_name.c_str(), _domain_id);
    
    // 设置进度
    SetNetProgress(100);

    // 判断是否被Cancel
    if (true == InstanceIsDestroy())
    {
        return VMOP_ERROR_CANCEL_ABORT;
    }

    // 3、调用存贮接口，准备存储相关的资源
    SetProgressDesc(START_PREP_STORAGE);
    rc = StartStorage();
    if(0 != rc)
    {
        return rc;
    }
    // 设置进度
    SetStorageProgress(100);

    // 判断是否被Cancel
    if (true == InstanceIsDestroy())
    {
        return VMOP_ERROR_CANCEL_ABORT;
    }

    // 4、创建ISO
    SetProgressDesc(START_PREP_CONTEXT);
    rc = CreateContext();
    // 设置进度
    SetComputerProgress(20);
    if(rc == CREATE_SUCC)
    {
        rc = CreateContextIso();
        if(0 != rc)
        {
            return CREATE_ERROR_CREATEISO;
        }

        // 把该iso作为追加到当前最后一个盘符
        zte_tecs::VmDiskConfig contextdisk;
        contextdisk._type          = "ro_disk";    /*  只读磁盘 */
        contextdisk._source_type   = "file";
        contextdisk._source = tecs_instance_path + _instance_name + "/tecs-context.iso";
        contextdisk._target = GetContextDiskTarget();
        contextdisk._driver = "file";
        contextdisk._bus    = "ide";

        // 如果磁盘列表为NULL,则直接把context压入
        if (vm_cfg._disks.empty())
        {
            vm_cfg._disks.push_back(contextdisk);
        }
        else
        {
            // 如果最后一个磁盘的source不等于context的盘符内容，则把context
            // 插入到磁盘列表
            if (vm_cfg._disks.back()._source != contextdisk._source)
            {
                vm_cfg._disks.push_back(contextdisk);
            }
        }
    }
    else if(rc == CREATE_ERROR)
    {
        return CREATE_ERROR_CREATECONTEXT;
    }

    // 设置进度
    SetComputerProgress(40);

    // 判断是否被Cancel
    if (true == InstanceIsDestroy())
    {
        return VMOP_ERROR_CANCEL_ABORT;
    }

    // 获取特殊设备的PCI信息
    GetSdPciInfo(vm_cfg._pci_devices);

    // 5、create configure xml,要用到网络和存储接口的返回信息,注意pvm/hvm的区别
    SetProgressDesc(START_PREP_CONFIG);
    rc = CreateConfigFile();
    if(0 != rc)
    {
        return CREATE_ERROR_CREATEXML;
    }

    return VMOP_SUCC;
}

/****************************************************************************************/
void VmInstance::GetSdPciInfo(vector<PciBus> &pci_info)
{
    if (vm_cfg._devices.empty())
    {
        return;
    }

    vector<PCIPassthrough> pci_devices;
    PCIPthManager *pth_manager = PCIPthManager::GetInstance();
    if (pth_manager)
    {
        pth_manager->Request(vm_cfg._vid, vm_cfg._devices, pci_devices);
    }

    vector<PciBus>::iterator it_bus;
    vector<PCIPassthrough>::iterator it_dev;
    for (it_dev = pci_devices.begin(); it_dev != pci_devices.end(); it_dev ++)
    {
        for (it_bus = it_dev->_pci_buses.begin(); it_bus != it_dev->_pci_buses.end(); it_bus ++)
        {
            pci_info.push_back(*it_bus);
        }
    }
}

/****************************************************************************************
* 函数名称：ExitDestroy
* 功能描述：退出销毁流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::ExitDestroy(const STATUS rs)
{
    DoOpResultFunction(rs);
    VmClearDestroyFlag();
}

/****************************************************************************************
* 函数名称：EnterDestroy
* 功能描述：进入销毁流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::EnterDestroy()
{
    // cancel或者迁移成功进入销毁流程，后续将销毁实例
    //SetProgress(100);
    //SetOpResult(VMOP_SUCC);

    // 保证不再收消息，只影响消息的接收，对下面的发送没有影响
    _mu->SetHandler(NULL);

    // 使用该接口，发现消息到VMHANDLER,VmHandler收到后，开始释放资源
    SendDel2VmHandler();
}

/****************************************************************************************
* 函数名称：CancelDomain
* 功能描述：Cancel流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::CancelDomain()
{
    // 设置当前线程处于Cancel状态
    VmSetDestroyFlag();

    if(!StopWDGTimer(__FUNCTION__))
    {
        OutPut(SYS_DEBUG, "%s stopWdTimer failed.\n", _instance_name.c_str());
    }
        
    // 停止虚拟机,如果虚拟机不存在，该函数也会返回成功
    if (VMOP_VM_DO_FAIL == DestroyDomain())
    {
        ExitDestroy(VMOP_VM_DO_FAIL);
        return;
    }

    uint32 wait_time = 0;
    // 检测看虚拟机是否消亡了
    do
    {
        // 如果消亡则跳出
        if (-1 == _vm_driver->GetDomainIDByName(_instance_name))
        {
            break;
        }
        Delay(1000);
        //  如果60S虚拟机都没有消亡，则cancel失败
        wait_time++;
        if (wait_time >= 60)
        {
            ExitDestroy(VMOP_VM_DO_FAIL);
            return;
        }
    }
    while(1);

    SetProgress(10);

    // 删除了虚拟机，延时1S，进行资源释放，尽量让xend来把loop设备释放
    Delay(1000);

    OutPut(SYS_DEBUG, "%s start ReleaseResource\n", _instance_name.c_str());
    STATUS rc = ReleaseResource();
    if (SUCCESS != rc)
    {
        ExitDestroy(rc);
        return;
    }

    EnterDestroy();
}

/****************************************************************************************
* 函数名称：ReleaseResource
* 功能描述：释放资源
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::ReleaseResource()
{
    STATUS   rc;

    // 释放网络资源
    SetProgressDesc(START_REL_NET);
    bool is_success = StopNetWork(__FUNCTION__, VNET_VM_CANCEL);
    if (!is_success)
    {
        OutPut(SYS_ALERT, "%s StopNetWork fail\n", _instance_name.c_str());
        return VMOP_ERROR_NETWORK_STOP;
    }
    SetNetProgress(100);

    // 释放存贮资源
    SetProgressDesc(START_REL_STORAGE);
    OutPut(SYS_DEBUG, "%s start ReleaseStorage", _instance_name.c_str());
    rc = ReleaseStorage();
    if (SUCCESS != rc)
    {
        OutPut(SYS_ALERT, "%s ReleaseStorage fail: rc = %d\n", _instance_name.c_str(), rc);
        return VMOP_ERROR_STORAGE_RELEASE;
    }
    SetStorageProgress(100);

    PCIPthManager *pth_manager = PCIPthManager::GetInstance();
    if (pth_manager)
    {
        pth_manager->Release(vm_cfg._vid);
    }

    // 删除目录
    SetProgressDesc(START_DEL_DIR);
    string cmd("rm -rf ");
    cmd = cmd + tecs_instance_path+_instance_name;
    system(cmd.c_str());

    if(IsXenKernel())
    {
        string coredump_save_local_dir;
        GetCoredumpFilePath(coredump_save_local_dir);
        // 直接umount coredump目录
        if(access(coredump_save_local_dir.c_str(), F_OK) != 0)
        {
            OutPut(SYS_INFORMATIONAL, "dir %s not exist, neednot umount, return OK.\n", coredump_save_local_dir.c_str());
            return SUCCESS;
        }
        else
        {
            if(SUCCESS != (rc = UnMountNfs(coredump_save_local_dir)))
            {
                OutPut(SYS_ALERT, "UnMountNfs %s failed: rc = %d\n", coredump_save_local_dir.c_str(),rc);
                return VMOP_ERROR_UMOUNT_COREDUMP_URL;
            }

            // 删除存在的目录
            cmd = "rm -fr " + coredump_save_local_dir;
            system(cmd.c_str());
        }
    }
    return SUCCESS;
}


/****************************************************************************************
* 函数名称：SendMsgToVna
* 功能描述：发送消息到VNA上去
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：out_msg 发送消息成功，收到的应答消息内容
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2013/04/07    V1.0    lixch         创建
****************************************************************************************/
bool VmInstance::SendMsgToVnetWithTimeOut(const Serializable& message,
        uint32 msg_id,
        uint32 ack_id,
        uint32 time_out,
        const string &request_stamp,
        string &out_msg)
{
    MID receiver(MU_VNET_LIBNET);
    MessageOption option(receiver, msg_id, 0, 0);


    /* 创建临时消息单元 */
    string tmpmu = "vm" + to_string<int64>(_instance_id,dec) + "_tmpmu";
    MessageUnit temp_mu(tmpmu + "_" + request_stamp);
    temp_mu.Register();
    
    STATUS rc = temp_mu.Send(message, option);
    if (SUCCESS != rc)
    {
        OutPut(SYS_ALERT, "*** Instances %lld Send Msg To Vna Fail, rc = %d ***\n", _instance_id, rc);
        return false;
    }

    MessageFrame  msg;
    T_TimeValue     start_time;
    T_TimeValue     now_time;
    T_TimeValue     resend_time;
    GetTimeFromPowerOn(&start_time);
    resend_time = start_time;
    while (1)
    {
        GetTimeFromPowerOn(&now_time);
        // 上电秒，不考虑越界，要越界需要136年
        if ((now_time.second - start_time.second) > time_out)
        {
            OutPut(SYS_ALERT, "*** Instances %lld Send Msg To Vna TimeOut, time = %d ***\n",
                   _instance_id, time_out);
            return false;
        }
        // 如果2秒收不到请求的应答，则重发消息给网络模块
        if ((now_time.second - resend_time.second) > 2)
        {
            resend_time = now_time;
            rc = temp_mu.Send(message, option);
            if (SUCCESS != rc)
            {
                OutPut(SYS_ALERT, "*** Instances %lld Send Msg To Vna Fail, rc = %d ***\n", _instance_id, rc);
                return false;
            }
        }

        if (SUCCESS != temp_mu.Wait(&msg, 5))
        {
            continue;
        }

        if (msg.option.id() != ack_id)
        {
            OutPut(SYS_DEBUG, "*** In SendMsgToVnetWithTimeOut: Instances %lld rcv ignore msgId %d ***\n",
                   _instance_id, msg.option.id());
            continue;
        }

        /* OutPut(SYS_DEBUG, "*** Instances %lld rcv Msg from vnet, content is:\n%s ++\n", \
                _instance_id, msg.data.c_str()); */
        WorkAck work_ack;
        work_ack.deserialize(msg.data);
        if (work_ack.action_id != request_stamp)
        {
            OutPut(SYS_DEBUG, "*** Instances %lld Recver Msg's  request_stamp Is Invalid ***\n",
                   _instance_id);
            continue;
        }

        // 如果网络返回的是中间数据，需要继续等待应答操作结果
        if(work_ack.state == ERROR_ACTION_RUNNING)
        {
            OutPut(SYS_DEBUG, "*** Instances %lld Recve vnet ack state:%d, not expect result***\n",
                   _instance_id, work_ack.state);
            continue;
        }

        out_msg = msg.data;
        return true;
    }
    return false;
}

/****************************************************************************************
* 函数名称：StartNetWork
* 功能描述：申请本实例相关网络资源
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
bool VmInstance::StartNetWork(const char *const  fun_name, BYTE option)
{
    CVNetStartNetworkMsg  vnet_req;
    uint32 i;

    if(!HasNetwork())
    {
        return true;
    }

    vnet_req.m_option = option;
    vnet_req.m_vm_id = _instance_id;
    //vnet_req.action_id = GenerateUUID();
    for (i = 0; i < vm_cfg._nics.size(); i++)
    {
        CVNetVnicConfig temp;
        temp.m_nNicIndex = vm_cfg._nics.at(i)._nic_index;
        temp.m_nIsSriov   = vm_cfg._nics.at(i)._sriov;
        temp.m_nIsLoop   = vm_cfg._nics.at(i)._loop;
        temp.m_strLogicNetworkID = vm_cfg._nics.at(i)._logic_network_id;
        temp.m_strAdapterModel = vm_cfg._nics.at(i)._model;
        temp.m_strVSIProfileID = vm_cfg._nics.at(i)._vsi_profile_id;
        temp.m_strIP     = vm_cfg._nics.at(i)._ip;
        temp.m_strNetmask = vm_cfg._nics.at(i)._netmask;
        temp.m_strGateway = vm_cfg._nics.at(i)._gateway;
        vnet_req.m_VmNicCfgList.push_back(temp);
    }

    if (vm_cfg._vm_wdtime != 0)
    {
        vnet_req.m_WatchDogInfo.m_nIsWDGEnable = true;
        vnet_req.m_WatchDogInfo.m_nTimeOut = vm_cfg._vm_wdtime;
    }
    else
    {
        vnet_req.m_WatchDogInfo.m_nIsWDGEnable = false;
        vnet_req.m_WatchDogInfo.m_nTimeOut = 0;
    }

    string ack_msg;
    //携带一下本次消息的md5作为action_id，目的是为了鉴定所收应答消息的合法性，即该应答究竟是否针对本次请求
    vnet_req.action_id = GetStrMd5sum(vnet_req.serialize());
    bool rc = SendMsgToVnetWithTimeOut(vnet_req, EV_VNETLIB_STARTNETWORK,
                                       EV_VNETLIB_STARTNETWORK_ACK,
                                       60, vnet_req.action_id,ack_msg);

    if(!rc)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet not ack !!***\n",
               fun_name, _instance_id,  __FUNCTION__);
        return false;
    }
    // 处理获取到的配置
    CVNetStartNetworkMsgAck vnet_ack;
    vnet_ack.deserialize(ack_msg);
    if(SUCCESS != vnet_ack.state)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet ack exec fail, msg content: %s ***\n",
               fun_name, _instance_id,  __FUNCTION__, ack_msg.c_str());
        return false;
    }

    vm_cfg._nics.clear();
    for (i = 0; i < vnet_ack.m_vsiinfo.size(); i++)
    {
        VmNicConfig nic(vnet_ack.m_vsiinfo.at(i).m_cVnicConfig.m_nNicIndex,
                        vnet_ack.m_vsiinfo.at(i).m_cVnicConfig.m_nIsSriov,
                        vnet_ack.m_vsiinfo.at(i).m_cVnicConfig.m_nIsLoop,
                        vnet_ack.m_vsiinfo.at(i).m_cVnicConfig.m_strLogicNetworkID,
                        vnet_ack.m_vsiinfo.at(i).m_cVnicConfig.m_strAdapterModel,
                        vnet_ack.m_vsiinfo.at(i).m_cVnicConfig.m_strVSIProfileID,
                        vnet_ack.m_vsiinfo.at(i).m_strPCIOrder,
                        vnet_ack.m_vsiinfo.at(i).m_strBridgeName,
                        vnet_ack.m_vsiinfo.at(i).m_strIP,
                        vnet_ack.m_vsiinfo.at(i).m_strNetmask,
                        vnet_ack.m_vsiinfo.at(i).m_strGateway,
                        vnet_ack.m_vsiinfo.at(i).m_strMac,
                        vnet_ack.m_vsiinfo.at(i).m_nCvlan
                       );

        vm_cfg._nics.push_back(nic);
    }

    // 保存看门狗信息
    if (vm_cfg._vm_wdtime != 0)
    {
        VmNicConfig nic(vnet_ack.m_wdg_info.m_cVnicConfig.m_nNicIndex,
                        vnet_ack.m_wdg_info.m_cVnicConfig.m_nIsSriov,
                        vnet_ack.m_wdg_info.m_cVnicConfig.m_nIsLoop,
                        vnet_ack.m_wdg_info.m_cVnicConfig.m_strLogicNetworkID,
                        vnet_ack.m_wdg_info.m_cVnicConfig.m_strAdapterModel,
                        vnet_ack.m_wdg_info.m_cVnicConfig.m_strVSIProfileID,
                        vnet_ack.m_wdg_info.m_strPCIOrder,
                        vnet_ack.m_wdg_info.m_strBridgeName,
                        vnet_ack.m_wdg_info.m_strIP,
                        vnet_ack.m_wdg_info.m_strNetmask,
                        vnet_ack.m_wdg_info.m_strGateway,
                        vnet_ack.m_wdg_info.m_strMac,
                        vnet_ack.m_wdg_info.m_nCvlan
                       );
        vm_cfg._wdg_nics = nic;
    }

    return true;
}

/****************************************************************************************
* 函数名称：StopNetWork
* 功能描述：申请本实例相关网络资源
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         创建
****************************************************************************************/
bool VmInstance::StopNetWork(const char *const  fun_name, BYTE option)
{
    CVNetStopNetworkMsg  stop_req;
    uint32 i;

    if(!HasNetwork())
    {
        return true;
    }

    stop_req.m_option = option;
    stop_req.m_vm_id = _instance_id;
    //stop_req.action_id = GenerateUUID();
    for (i = 0; i < vm_cfg._nics.size(); i++)
    {
        CVNetVnicConfig temp;
        temp.m_nNicIndex = vm_cfg._nics.at(i)._nic_index;
        temp.m_nIsSriov   = vm_cfg._nics.at(i)._sriov;
        temp.m_nIsLoop   = vm_cfg._nics.at(i)._loop;
        temp.m_strLogicNetworkID = vm_cfg._nics.at(i)._logic_network_id;
        temp.m_strAdapterModel = vm_cfg._nics.at(i)._model;
        temp.m_strVSIProfileID = vm_cfg._nics.at(i)._vsi_profile_id;
        stop_req.m_VmNicCfgList.push_back(temp);
    }

    if (vm_cfg._vm_wdtime != 0)
    {
        stop_req.m_WatchDogInfo.m_nIsWDGEnable = true;
        stop_req.m_WatchDogInfo.m_nTimeOut = vm_cfg._vm_wdtime;
    }
    else
    {
        stop_req.m_WatchDogInfo.m_nIsWDGEnable = false;
        stop_req.m_WatchDogInfo.m_nTimeOut = 0;
    }

    string ack_msg;
    stop_req.action_id = GetStrMd5sum(stop_req.serialize());
    bool rc = SendMsgToVnetWithTimeOut(stop_req, EV_VNETLIB_STOPNETWORK,
                                       EV_VNETLIB_STOPNETWORK_ACK,
                                       60, stop_req.action_id,ack_msg);

    if(!rc)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet not ack !!***\n",
               fun_name, _instance_id,  __FUNCTION__);
        return false;
    }

    CVNetStopNetworkMsgAck vnet_ack;
    vnet_ack.deserialize(ack_msg);
    if(SUCCESS != vnet_ack.state)
    {
        OutPut(SYS_ALERT, "*** %s: Instances %lld %s fail, vnet ack exec fail!!! msg content: %s ***\n",
               fun_name, _instance_id,  __FUNCTION__, ack_msg.c_str());
        return false;
    }
    return true;
}

/****************************************************************************************
* 函数名称：PrepareVmMigration
* 功能描述：目的端准备虚拟机迁移
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/12/30    V1.0    mhb         创建
****************************************************************************************/
STATUS VmInstance::PrepareVmMigration(const string& message)
{
    string        sshkey;
    VmMigrateData prepare;
    string        vmcfg;                  /* 创建消息携带的配置信息 */

    prepare.deserialize(message);
    sshkey        = prepare._src_ssh_public_key;

    OutPut(SYS_DEBUG, "VmInstance::PrepareVmMigration, recv req name: %s\n", _instance_name.c_str());

    /* 记录CREATE的时间点 */
    SetLaunchTime();

    /* 记录配置信息 */
    SetVmCfg(prepare._config);

    // 准备创建虚拟机的环境
    STATUS rc = PrepareResource(true);
    if (VMOP_SUCC != rc)
    {
        return rc;
    }

    // 记录sshkey
    SshTrust(sshkey);

    return VMOP_SUCC;
}

/****************************************************************************************
* 函数名称：ExecVmMigration
* 功能描述：源端执行虚拟机迁移
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/17    V1.0    lixch         创建
****************************************************************************************/
void VmInstance::ExecVmMigration(const string& message)
{
    VmMigrateData  migrate;
    STATUS        rc = 0;

    migrate.deserialize(message);
    string host_ip           = migrate._targe_ip;

    OutPut(SYS_DEBUG, "VmInstance::ExecVmMigration, recv Migrate name: %s\n", _instance_name.c_str());

    // 设置撤销标志
    VmSetDestroyFlag();

    // 停止看门狗
    if (!StopWDGTimer(__FUNCTION__))
    {
        ExitDestroy(VMOP_ERROR_WATCHDOG_STOP);
        return;
    }

    SetProgressDesc(START_EXEC_MIGRATE);
    rc = _vm_driver->Migrate(_instance_name, host_ip);
    if (SUCCESS != rc)
    {
        ExitDestroy(VMOP_VM_DO_FAIL);
        return;
    }

    string finish_str = "Migrate "+_instance_name+" to "+host_ip+" finish!\n";
    write(STDOUT_FILENO,finish_str.c_str(),finish_str.length());

    if (DomainIsExist())
    {
        finish_str = "Migrate "+_instance_name+" to "+host_ip+" failed for still exist!\n";
        write(STDOUT_FILENO,finish_str.c_str(),finish_str.length());
        ExitDestroy(VMOP_VM_DO_FAIL);
        return;
    }
    
    SetProgress(10);

    // 释放资源, 失败不执行
    rc = ReleaseResource();
    if (SUCCESS != rc)
    {
        ExitDestroy(rc);
        return;
    }

    EnterDestroy();
}

/****************************************************************************************
* 函数名称：HandleVmMigrateSucc
* 功能描述：源端迁移成功，通知目的端网络进行切换
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::HandleVmMigrateSucc(const string& message)
{
    VmMigrateData  inform;

    inform.deserialize(message);

    OutPut(SYS_DEBUG, "VmInstance::HandleVmMigrateSucc, recv MigrateSucc name: %s\n", _instance_name.c_str());

    // 解除sshkey权限
    //SshUntrust(inform._src_ssh_public_key);

    // 刷新虚拟机状态
    RefreshVmState();

    // 设置启动时间
    SetBootTime();

    // 刷新最新的domid
    _domain_id = _vm_driver->GetDomainIDByName(_instance_name);

    // 启动网络:网卡和看门狗
    bool is_success = StartNetWork(__FUNCTION__, VNET_VM_MIGRATE);
    if(!is_success)
    {
        return VMOP_ERROR_NETWORK_START;
    }
    
    // 通知网络那边，dominid变化了
    if (!NotifyNetDomIDChg(__FUNCTION__))
    {
        return VMOP_ERROR_NETWORK_NOTIFY;
    }


    return VMOP_SUCC;
}

/****************************************************************************************
* 函数名称：AttachDisk
* 功能描述：动态增加一个盘的函数实现
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::PlugInPdisk(const string& message)
{
    VmAttachReq req;
    req.deserialize(message);

    // 当虚拟机是关机状态的时候，直接返回成功
    if (_state == VM_SHUTOFF)
    {
        return VMOP_SUCC;
    }
    int ret=LoadShareDisk(req._vid, req._diskcfg._target, req._diskcfg._share_url.access_option, "", req._diskcfg._source);
    if(SUCCESS !=ret && ERROR_LOAD_NEED_REFRESH!=ret)
    {
        OutPut(SYS_ERROR, "VmInstance::%s LoadShareDisk Fail  \n", _instance_name.c_str());
        return VMOP_ERROR_STORAGE;
    }
    vm_cfg._disks.push_back(req._diskcfg);
    if(ret==ERROR_LOAD_NEED_REFRESH)
    {
        TimeOut timeout;
        timeout.type     = RELATIVE_TIMER;
        timeout.duration = load_refresh_period;    // 默认定时120S
        timeout.msgid    = EV_TIMER_LOAD;
        STATUS rc = _mu->SetTimer(_load_timer, timeout);
        if (SUCCESS != rc)
        {
            OutPut(SYS_ALERT, "%s SetLoadTimer failed rc = %d!\n",
                   _instance_name.c_str(),rc);
            SkyAssert(false);
        }

    }
    req._diskcfg._source_type=STORAGE_SOURCE_TYPE_BLOCK;

    if ( _vm_driver->AddRemovableDisk(_instance_name, req._diskcfg) != 0)
    {
        OutPut(SYS_ERROR, "VmInstance::%s AttachDisk Fail\n", _instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    return VMOP_SUCC;
}

/****************************************************************************************
* 函数名称：DeAttachDisk
* 功能描述：动态删除一个盘的实现
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
****************************************************************************************/
STATUS VmInstance::PlugOutPdisk(const string& message)
{
    // 实现参数的解析
    VmAttachReq req;
    req.deserialize(message);

    if (_state != VM_SHUTOFF)
    {
        if(SUCCESS != LoadShareDisk(req._vid, req._diskcfg._target, req._diskcfg._share_url.access_option, "", req._diskcfg._source))
        {
            OutPut(SYS_ERROR, "VmInstance::%s LoadShareDisk Fail  \n", _instance_name.c_str());
            return VMOP_ERROR_STORAGE;
        }
        req._diskcfg._source_type=STORAGE_SOURCE_TYPE_BLOCK;

        if (_vm_driver->DelRemovableDisk(_instance_name, req._diskcfg) != 0)
        {
            OutPut(SYS_ERROR, "VmInstance::%s DeAttachDisk Fail\n", _instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }
    }

    if(SUCCESS!=UnLoadShareDisk(req._vid,req._diskcfg._target, req._diskcfg._share_url.access_option))
    {
        OutPut(SYS_ERROR, "VmInstance::%s UnLoadShareDisk Fail  \n", _instance_name.c_str());
        return VMOP_ERROR_STORAGE;
    }
    vector<VmDiskConfig>::iterator it = vm_cfg._disks.begin();
    for(; it != vm_cfg._disks.end(); it++)
    {
        if  (it->_request_id == req._diskcfg._request_id)
        {
            vm_cfg._disks.erase(it);
            break;
        }
    }
    return VMOP_SUCC;
}

/****************************************************************************************
* 函数名称：ImageBackupCreate
* 功能描述：创建镜像备份
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    fj         创建
****************************************************************************************/
STATUS VmInstance::ImageBackupCreate(const string& message)
{
    // 实现参数的解析
    VmImageBackupReq req;
    req.deserialize(message);

    if (_state != VM_SHUTOFF)
    {
        OutPut(SYS_ERROR, "VmInstance::%s CreateBackupImg Fail ,pls shutoff the vm first \n", _instance_name.c_str());
        return VMOP_ERROR_STORAGE;
    }
    return  _storage_instance->CreateImgBackup(req);


}

/****************************************************************************************
* 函数名称：ImageBackupDelete
* 功能描述：删除镜像备份
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2013/7/14     V1.0    zhanghao         创建
****************************************************************************************/
STATUS VmInstance::ImageBackupDelete(const string& message)
{
    // 实现参数的解析
    VmImageBackupReq req;
    req.deserialize(message);

    if (_state != VM_SHUTOFF)
    {
        OutPut(SYS_ERROR, "VmInstance::%s DeleteBackupImg Fail ,pls shutoff the vm first \n", _instance_name.c_str());
        return VMOP_ERROR_STORAGE;
    }
    return  _storage_instance->DeleteImgBackup(req);

}


/****************************************************************************************/
void VmInstance::DoVmStatistics(ResourceStatistics& vm_statistics,
                                int64& vid)
{
    if (0 == statistics_interval)
    {
        vm_statistics = _pre_vm_statistics;
        return;
    }

    time_t now = time(0);
    if (0 > (now-_pre_statistics_time))
        return;
    int32 diff = now-_pre_statistics_time-statistics_interval;
    if ((0 == _report_count)
        || (0 <= diff))
    {
        _pre_statistics_time = now;
        if (true == IsXenKernel())
        {
            XenVmStatistics(vm_statistics, vid);
        }
        else if (true == IsKvmKernel())
        {
            KvmVmStatistics(vm_statistics, vid);
        }
        else
        {
            return;
        }
        /* 考虑到安装 tecs 版本之前系统已经进行了通信
         * 即存在了多个统计间隔时间的收发包大小, 
         * 再和 0 做差值, 得到的使用率是很大, 不合适
         * 故第一次仅记录到内存中, 数据库中为0, 后续正常计算
         */
        if (0 < _report_count)
            GetNicUsedRate(vm_statistics._vec_nic_info);
        else
            _pre_vm_statistics = vm_statistics;
    }
    else
        vm_statistics = _pre_vm_statistics;
}

/****************************************************************************************/
void VmInstance::GetNicUsedRate(vector<NicInfo>& vec_nic_info)
{
    int time_interval = 5; // 5s, 单位是秒
    vector<NicInfo>::iterator it_now;
    vector<NicInfo>::iterator it_pre;
    for (it_now  = vec_nic_info.begin();
         it_now != vec_nic_info.end();
         it_now++)
    {
        it_pre = find_if(_pre_vm_statistics._vec_nic_info.begin(),
                         _pre_vm_statistics._vec_nic_info.end(),
                         NicInfo(it_now->_name));
        if ((it_pre != _pre_vm_statistics._vec_nic_info.end())
            && (0 < it_now->_speed))
        {
            // 寻找到对应的网卡, 并更新为 pre
            it_now->_used_rate = 1000 * (it_now->_net_flow - it_pre->_net_flow) / (it_now->_speed * time_interval);
            *it_pre = *it_now;
        }
    }
    _pre_vm_statistics._vec_nic_info.clear();
    _pre_vm_statistics._vec_nic_info = vec_nic_info;
}

/******************************************************************************/
/* brief: vNIC 的速率
 * in   : strallports - vNIC 对应网桥上的所有端口
 * out  : ispeed - vNIC 对应的物理端口的速率
 * return: SUCCESS/ERROR
 * note : 
 *  1) 单位为 B/s
 *  2) 获取的实际上 vNIC 对应的物理端口的速率, 且仅存在一个物理端口
 */
int32 VmInstance::GetVnicSpeed(string strallports, int64& ispeed)
{
    if (strallports.empty())
    {
        return ERROR;
    }
    int ret = -1;
    string line, cmd, strtmp;
    string::size_type pos = 0;
    stringstream iss(strallports);
    while (getline(iss, line))
    {
        cmd.clear();
        strtmp.clear();
        cmd = "ls -l /sys/class/net"
              "| grep pci"
              "| grep " + line;
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        cmd.clear();
        strtmp.clear();
        ispeed = 0;
        pos = 0;
        cmd = "ethtool " + line + "| grep Speed"
              "| awk -F ':' '{print $2}'| awk -F 'M' '{print $1}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        ispeed = atoi(strtmp.c_str()) * 1024 *1024;
        break; // 仅存在一个物理端口
    }
    return 0;
}

/****************************************************************************************/
int32 VmInstance::XenVmStatistics(ResourceStatistics& vm_statistics,
                                  const int64& vid)
{
    // 统计时间
    Datetime now;
    vm_statistics._statistics_time = now.tostr();
    string strtmp;
    /* CPU使用率/MEM使用率
     * 涉及命令: 
     *      xentop  - 获取各个dom的cpu使用率
     * 打印:
     *      NAME          STATE       CPU(sec) CPU(%)     MEM(k) MEM(%)
     *      instance_1020 --b---      14939     13.2    1052644    4.2
     * 计算:
     *      获取到的使用率已经*100, then rate_from_xentop*10
     * note: 此处的内存使用率指的是分配给VM的内存占物理机的总的内存的比例
     */
    string cmd = "xentop -b -f -i 2 -d 0.5|grep -E \"Domain-0|instance_\"";
    int ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }

    // 10 offset 能避开第一次的统计
    string::size_type pos = strtmp.find("Domain-0", 10);
    if (strtmp.npos == pos)
    {
        return ERROR;
    }
    string strsec = strtmp.substr(pos);

    stringstream iss(strsec);
    string line, strvid;
    int64 ivid = 0;
    while (getline(iss, line))
    {
        // vid
        strtmp.clear();
        strvid.clear();
        ivid = 0;
        pos = 0;
        cmd = "echo \"" + line + "\" |awk -F ' ' '{print $1}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        if (!strtmp.compare("Domain-0"))
        {
            //Domain-0 不统计
            continue;
        }
        pos = strtmp.find('_', 0);
        if (strtmp.npos == pos)
        {
            continue;
        }
        strvid = strtmp.substr(pos+1);
        ivid = atoi(strvid.c_str());
        if (vid != ivid)
            continue;
        else
        {
            /* cpu_used_rate
             * 空格为间隔符, 第 4 个参数
             */
            strtmp.clear();
            cmd.clear();
            pos = 0;
            cmd = "echo \"" + line + "\" |awk -F ' ' '{print $4}'";
            ret = RunCmd(cmd, strtmp);
            if ((SUCCESS != ret) || strtmp.empty())
            {
                break;
            }
            pos = strtmp.find('\n', 0);
            if (strtmp.npos != pos)
                strtmp.erase(pos);
            vm_statistics._cpu_used_rate = atof(strtmp.c_str()) * 10;

            /* mem_used_rate
             * 空格为间隔符, 第 6 个参数
             */
            strtmp.clear();
            cmd.clear();
            pos = 0;
            cmd = "echo \"" + line + "\" |awk -F ' ' '{print $6}'";
            ret = RunCmd(cmd, strtmp);
            if ((SUCCESS != ret) || strtmp.empty())
            {
                break;
            }
            pos = strtmp.find('\n', 0);
            if (strtmp.npos != pos)
                strtmp.erase(pos);
            vm_statistics._mem_used_rate = atof(strtmp.c_str()) * 10;

            break;
        }
    }

    /* 网卡使用率
     */
    /* vid 需要转成字符串, 便于命令中使用
     */
    strvid.clear();
    char actmp[20];
    memset(actmp, 0, 20);
    sprintf(actmp, "%lld", vid);
    strvid = actmp;
    /* 获取 vid 对应虚拟机的 domid
     * xl list | grep instance_ivid | awk -F ' ' '{print $2}'
     */
    strtmp.clear();
    cmd.clear();
    pos = 0;
    cmd = "xl list | grep -w instance_" + strvid + "| awk -F ' ' '{print $2}'";
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    pos = strtmp.find('\n', 0);
    if (strtmp.npos != pos)
        strtmp.erase(pos);
    int idomid = atoi(strtmp.c_str());
    /* 检查该虚拟机配置了多少了 vNIC
     * cat /var/lib/tecs/instances/instance_x/instance_x.xml | grep -E "<interface"
     */
    strtmp.clear();
    cmd.clear();
    iss.str("");
    int inum = 0;
    cmd = "cat /var/lib/tecs/instances/instance_" + strvid
          + "/instance_" + strvid + ".xml"
          "| grep -E \"<interface\"";
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    iss.str(strtmp);
    line.clear();
    while (getline(iss, line))
        inum++;
    int i = 0;
    long long irx = 0, itx = 0;
    ostringstream oss;
    string strbr, strport;
    NicInfo nic_info;
    for (i = 0; i < inum; i++)
    {
        /* 判定端口为 tap 还是 vif, tap 优先级高
         * ovs-vsctl show | grep tap 或者 vif
         * */
        oss.str("");
        cmd.clear();
        strtmp.clear();
        oss << "tap" << idomid << "." << i;
        cmd = "ovs-vsctl show | grep " + oss.str();
        ret = RunCmd(cmd, strtmp);
        if (SUCCESS != ret)
            continue;
        else if (strtmp.empty())
        {
            oss.str("");
            cmd.clear();
            strtmp.clear();
            oss << "vif" << idomid << "." << i;
            cmd = "ovs-vsctl show | grep " + oss.str();
            ret = RunCmd(cmd, strtmp);
            if ((SUCCESS != ret) || strtmp.empty())
            {
                continue;
            }
        }
        else
        {
        }
        /* 获取网桥
         * ovs-vsctl port-to-br tapx.x
         * */
        strport.clear();
        strbr.clear();
        cmd.clear();
        pos = 0;
        strport = oss.str();
        cmd = "ovs-vsctl port-to-br " + strport;
        ret = RunCmd(cmd, strbr);
        if ((SUCCESS != ret) || strbr.empty())
        {
            continue;
        }
        pos = strbr.find('\n', 0);
        if (strbr.npos != pos)
            strbr.erase(pos);
        nic_info._name = strport;
        /* 获取 rx, tx 的值, 单位为字节
         * ovs-ofctl dump-ports br port | grep rx or tx | awk 过滤
         * */
        irx = 0;
        pos = 0;
        cmd.clear();
        strtmp.clear();
        cmd = "ovs-ofctl dump-ports " + strbr + " " + strport
              + "| grep rx"
              "| awk -F '=' '{print $3}'"
              "| awk -F ',' '{print $1}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        irx = atoi(strtmp.c_str());
        itx = 0;
        pos = 0;
        cmd.clear();
        strtmp.clear();
        cmd = "ovs-ofctl dump-ports " + strbr + " " + strport
              + "| grep tx"
              "| awk -F '=' '{print $3}'"
              "| awk -F ',' '{print $1}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        itx = atoi(strtmp.c_str());
        nic_info._net_flow = irx + itx;
        /* vNIC 的速率取对应物理端口的速率
         * ovs-vsctl list-ports strbr 获取到所有的端口
         * 判断端口是否是物理端口
         *  ls -l /sys/class/net | grep pci | grep strport
         *  note: 仅会有一个物理端口
         */
        strtmp.clear();
        cmd.clear();
        cmd = "ovs-vsctl list-ports " + strbr;
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        GetVnicSpeed(strtmp, nic_info._speed);
        vm_statistics._vec_nic_info.push_back(nic_info);
    }
    return SUCCESS;
}

/****************************************************************************************/
int32 VmInstance::KvmVmStatistics(ResourceStatistics& vm_statistics,
                                  const int64& vid)
{
    // statistics time
    Datetime now;
    vm_statistics._statistics_time = now.tostr();

    string strtmp;
    // 根据 vid 构造虚拟机的名字
    ostringstream oss;
    oss << "instance_" << vid;
    /* ps -eo pid,args
     *  ps 显示 pid 和 进程的参数, 参数中包括虚拟机的名字
     * grep instance_x
     *  grep 找出虚拟机所在的进程
     * awk -F ' ' '{if($4 == \"instance_x\") print $1}'
     *  过滤满足 grep 筛出的多余信息
     */
    string strpid;
    string::size_type pos = 0;
    string cmd = "ps -eo pid,args"
                 "| grep " + oss.str() +
                 "| awk -F ' ' '{if($4 == \""
                 + oss.str() + "\") print $1}'";
    int ret = RunCmd(cmd, strpid);
    if ((SUCCESS != ret) || strpid.empty())
    {
        return ERROR;
    }
    pos = strpid.find('\n', 0);
    if (strpid.npos != pos)
        strpid.erase(pos);
    /* 根据 pid 获取对应的 CPU 和 MEM 使用率
     * top 命令显示进程 example:
     *  7169 qemu      20   0  381m  34m 4456 R  0.0  1.9   7:30.24 qemu-kvm
     *  -b: batch 模式, 可用于管道
     *  -n: 迭代次数
     *  -d: 迭代间隔时间(默认是 3s , 改为 1s)
     * NOTE: 第一次为自系统运行的平均值, 需获取第二次的
     * */
    strtmp.clear();
    cmd.clear();
    cmd = "top -bn 2 -d 1 | awk -F ' ' '{if($1 == \"" + strpid + "\") print $9,$10}'";
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    stringstream iss(strtmp);
    int index = 0;
    string line, strcpu, strmem;
    while (getline(iss, line))
    {
        if (0 == index)
        {
            // skip first time
            index++;
            continue;
        }
        // , 左为 CPU , 右为 MEM
        pos = line.find(' ');
        if (line.npos == pos)
            continue;
        strcpu.clear();
        strmem.clear();
        strcpu = line.substr(0, pos);
        strmem = line.substr(pos+1);
        vm_statistics._cpu_used_rate = atof(strcpu.c_str()) * 10;
        vm_statistics._mem_used_rate = atof(strmem.c_str()) * 10;
        break;
    }
    
    /* 网卡使用率实现方案
     * 概念解释:
     *  设置时间段 t(s), 网卡速率 100Mb/s
     *  网卡使用率: t内的收发包的大小(Mb)/(100 * t)
     * 网络实现:
     *  KVM 虚拟网卡的名称不再是vif、tap开头，而是类似vnet0、vnet1的格式
     * 命令详解3:
     *  参见下面各小步的实现
     */
    /* 获取虚拟机对应的虚拟网卡
     * virsh domiflist instance_x
     * NOTE: 可能存在多个
     */
    strtmp.clear();
    cmd.clear();
    cmd = "virsh domiflist " + oss.str();
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    iss.str("");
    iss.str(strtmp);
    string strport, strbr;
    index = 0;
    long long irx = 0, itx = 0;
    line.clear();
    NicInfo nic_info;
    while (getline(iss, line))
    {
        /* 删除多以打印, 前 2 行打酱油的
         * 格式如下
         *  Interface  Type       Source     Model       MAC
         * -------------------------------------------------------
         *  vnet0      bridge     sdvs_demo  -           00:d0:d0:00:00:03
         */
        index++;
        if (3 > index)
            continue;
        /* 摘出端口名和网桥名, 上述的 vnet0, sdvs_demo
         */
        strtmp.clear();
        cmd.clear();
        strport.clear();
        strbr.clear();
        pos = 0;
        cmd = "echo \"" + line + "\" | awk -F ' ' '{print $1, $3}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        pos = strtmp.find(' ');
        strport = strtmp.substr(0, pos);
        strbr = strtmp.substr(pos+1);
        nic_info._name = strport;
        /* 获取 rx, tx 的值, 单位为字节
         * ovs-ofctl dump-ports br port | grep rx or tx | awk 过滤
         * */
        irx = 0;
        pos = 0;
        cmd.clear();
        strtmp.clear();
        cmd = "ovs-ofctl dump-ports " + strbr + " " + strport
              + "| grep rx"
              "| awk -F '=' '{print $3}'"
              "| awk -F ',' '{print $1}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        irx = atoi(strtmp.c_str());
        itx = 0;
        pos = 0;
        cmd.clear();
        strtmp.clear();
        cmd = "ovs-ofctl dump-ports " + strbr + " " + strport
              + "| grep tx"
              "| awk -F '=' '{print $3}'"
              "| awk -F ',' '{print $1}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        itx = atoi(strtmp.c_str());
        nic_info._net_flow = itx + irx;
        /* vNIC 的速率取对应物理端口的速率
         * ovs-vsctl list-ports strbr 获取到所有的端口
         * 判断端口是否是物理端口
         *  ls -l /sys/class/net | grep pci | grep strport
         *  note: 仅会有一个物理端口
         */
        strtmp.clear();
        cmd.clear();
        cmd = "ovs-vsctl list-ports " + strbr;
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        GetVnicSpeed(strtmp, nic_info._speed);
        vm_statistics._vec_nic_info.push_back(nic_info);
    }

    return SUCCESS;    
}

void test_get_sigaction()
{
    struct sigaction sigchld_saved_action;

    sigaction(SIGCHLD, NULL, &sigchld_saved_action);

}
DEFINE_DEBUG_FUNC(test_get_sigaction);
/* lixiaocheng测试接口程序,start */

void testsetallvcpu(const string &name, int vcpu)
{
    VmDriver *pDrv  = NULL;

    pDrv = VmDriver::GetInstance();

    /* 把虚拟机的核进行全核bind */
    pDrv->VcpuAffinitySet(name, vcpu);
}
DEFINE_DEBUG_FUNC(testsetallvcpu);

void testsetweight(const string &name, int percent)
{
    VmDriver *pDrv  = NULL;
    pDrv = VmDriver::GetInstance();

    pDrv->Sched_credit_weight_set(name, percent);
}
DEFINE_DEBUG_FUNC(testsetweight);

void testsetcap(const string &name, int vcpu, int percent)
{
    VmDriver *pDrv  = NULL;
    pDrv = VmDriver::GetInstance();

    pDrv->Sched_credit_cap_set(name, vcpu, percent);
}
DEFINE_DEBUG_FUNC(testsetcap);

void testgetnic(int domid, int nic_no)
{
    VmDriver *pDrv  = NULL;
    pDrv = VmDriver::GetInstance();
    VmDomainInterfaceStatsStruct  nic_info;

    pDrv->GetDomainNetInfoById(domid, nic_no, nic_info);

    printf("rx = %d, tx = %d\r\n", nic_info.rx_bytes, nic_info.tx_bytes);
}
DEFINE_DEBUG_FUNC(testgetnic);

void testgetdesc(const char *domname)
{
    VmDriver *pDrv  = NULL;
    pDrv = VmDriver::GetInstance();

    //string name = *domname;
    string desc = pDrv->GetDomainDescByName(domname);

    cout << desc << endl;

}
DEFINE_DEBUG_FUNC(testgetdesc);

void testsetdesc(const char *domname, const char *desc)
{
    VmDriver *pDrv  = NULL;
    pDrv = VmDriver::GetInstance();

    //string name = *domname;
    //string desc_s = *desc;
    pDrv->SetDomainDescByName(domname, desc);
}
DEFINE_DEBUG_FUNC(testsetdesc);


void testgetdomid(const char *name)
{
    VmDriver *pDrv  = NULL;
    pDrv = VmDriver::GetInstance();

    int id;

    id = pDrv->GetDomainIDByName(name);

    cout << id << endl;
}
DEFINE_DEBUG_FUNC(testgetdomid);
/* lixiaocheng测试接口程序,end */


