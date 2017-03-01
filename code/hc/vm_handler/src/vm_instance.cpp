/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_instance.cpp
* �ļ���ʶ��
* ����ժҪ��VmInstance���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/7/19
*     �� �� �ţ�V1.1
*     �� �� �ˣ�lixch
*     �޸����ݣ������ݽ��п������
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
*                                 �����ⲿ��������                                    *
****************************************************************************************/

/****************************************************************************************
*                                 ȫ�ֱ���                                              *
****************************************************************************************/
string   tecs_instance_path = "/var/lib/tecs/instances/";
string   ssh_key_file_path = "/root/.ssh/authorized_keys";

const int wait_op_duration  = 60;    /* �ȴ�����ִ�����ʱ������λ S */
const int op_check_period   = 1000;  /* ���ڼ����������ʱ������λ ms */
const int vm_refresh_period = 5000;  /* ���ڼ����������ʱ������λ ms */

const int load_refresh_period = 120000;  /* ���ڼ����������ʱ������λ ms */

extern int32 statistics_interval;

/****************************************************************************************
*                                 ���ߺ���                                              *
****************************************************************************************/
/****************************************************************************************
* �������ƣ�Is64BitSystem
* �����������Ƿ�Ϊ64λϵͳ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         ����
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
* �������ƣ�Init
* ������������ʼ��instance_x�̣߳���������MU
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
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

    // ����һ��״̬����õĶ�ʱ��
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
* �������ƣ�DoStartup
* ����������instance_x�յ�handler�������ϵ�֪ͨ������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2013/09/17    V1.0    johnir       create
****************************************************************************************/
STATUS VmInstance::DoStartup()
{
     OutPut(SYS_DEBUG, "%s receive EV_POWER_ON msg from handler.\n", _instance_name.c_str());
            
    // ���������ʵ����ѭ��ˢ�¶�ʱ��
    StartRefreshTimer();

    // �ϵ�״̬�£���Ҫˢ��һ��
    RefreshInstanceInfo();
    RefreshInstanceReportInfo(_out_info);
    
    OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
    
    return SUCCESS;
}

/****************************************************************************************
* �������ƣ�StartOpCheckTimer
* ������������������������������Ķ�ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
void VmInstance::StartOpCheckTimer()
{
    TimeOut timeout;
    timeout.type     = RELATIVE_TIMER;
    timeout.duration = op_check_period;    // ��ʱ1S
    timeout.msgid    = EV_TIMER_OP_RESULT;
    _mu->SetTimer(_op_check_timer, timeout);
}

/****************************************************************************************
* �������ƣ�StartOpCheckTimer
* ������������������������������Ķ�ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
void VmInstance::StartRefreshTimer()
{
    TimeOut timeout;
    timeout.type     = RELATIVE_TIMER;
    timeout.duration = vm_refresh_period;    // Ĭ�϶�ʱ10S

    // ������������еĻ�����1S��ʱ����ˢ��
    if (IsOpRunning())
    {
        timeout.duration = 1000;    // ��ʱ1S
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
* �������ƣ�GetRunVersion
* ������������ȡ��ǰ��������������а汾
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
void VmInstance::GetRunVersion()
{
    string version;

    // ������а汾�Ѿ���ȡ���ˣ���ô�˳�
    if (_run_version != -1)
    {
        return;
    }

    version = _vm_driver->GetDomainDescByName(_instance_name);

    // NULL�Ļ������ֵ�ǰ�ڴ�����а汾����
    if (version.empty())
    {
        return;
    }

    // ��������Ϣ�еİ汾��Ϣ���浽�ڴ���
    from_string(_run_version, version, dec);
}

/****************************************************************************************
* �������ƣ�DoOpResultFunction
* ������������������������������Ӧ�Ķ���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
void VmInstance::DoOpResultFunction(STATUS rs)
{
    if (VMOP_RUNNING  == rs)
    {
        // �����ȴ��60S����Ҫ����RESET��REBOOT�еȴ��������ʧ��ʱ��
        _wait_time = wait_op_duration;
        // ���ø�1S��ʱ�������������Ƿ����
        StartOpCheckTimer();
        return;
    }

    if (VMOP_SUCC == rs)
    {
        SetProgress(100);
    }

    // ���õ�ǰ��ִ�н��
    SetOpResult(rs);
    // ��Ӧ��
    SendVmOprAckMsg(_op_sender, _op_action_id, _mu);
}


/****************************************************************************************
* �������ƣ�StartWDG
* �����������������Ź���ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
bool  VmInstance::StartWDGTimer(const char *const op_fun)
{
    return OperateWatchDog(op_fun, EN_START_TIMER_WATCH_DOG);
}

/****************************************************************************************
* �������ƣ�StopWDGTimer
* ����������ֹͣ���Ź���ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
bool  VmInstance::StopWDGTimer(const char *const op_fun)
{
    return OperateWatchDog(op_fun, EN_STOP_TIMER_WATCH_DOG);
}

/****************************************************************************************
* �������ƣ�StartOpCheckWDG
* �����������������������������Ź���ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
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
* �������ƣ�CheckOpTimeOut
* �����������������������Ƿ�ʱ�ˣ���Щ���ʱ��Ҫ����ʧ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
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
* �������ƣ�DoOpCheck
* ����������������������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
STATUS VmInstance::DoOpCheck()
{
    // ������ǲ����ȴ�̬���򲻽��������ж�
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
            // ���reboot ����reset�ɹ������������Ź�
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
            // ���reboot ����reset�ɹ������������Ź�
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
* �������ƣ�StartMu
* ����������������Ϣ��Ԫ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
STATUS VmInstance::StartMu()
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
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

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    // �����ٷ�POWER_ON��Ϣ��ֱ����Init�����г�ʼ��
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
* �������ƣ�MessageEntry
* ������������Ϣ��Ԫִ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
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
            // ��������VmHandler�Ĳ�����Ϣ
        case EV_VM_OP:
        {
            // ִ�������������Ϣ
            ExecVmOperation(message);
            break;
        }
        case EV_TIMER_OP_RESULT:
        {
            // ���״̬���ɹ��������óɹ�״̬�ͽ���100%
            STATUS rc = DoOpCheck();
            if (VMOP_RUNNING == rc)
            {
                // ���õ�ǰ���ȵ���ϸ����
                SetProgressDesc(WAIT_EXEC_SUCC);
                // �������������ʱ�����ȴ��¸�����
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
* �������ƣ�ExecVmOperation
* ����������ִ�����������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::ExecVmOperation(const MessageFrame& message)
{
    TranceMsg       TranceMessage;
    VmOperationReq  req;
    VmOperation     opr_id;
    string          vmcfg;                   /* ������ϢЯ����������Ϣ */

    // �Ƚ���ת����Ϣ
    TranceMessage.deserialize(message.data);

    //�ٽ���ת����Ϣ��������Ϣ
    req.deserialize(TranceMessage._data);
    opr_id        = (VmOperation)(req._operation);

    // ��¼��ǰ������API��Ϣ��Ԫ�͵�ǰ����������ʱ����ز���
    _op_action_id = req.action_id;
    _op_cmd       = opr_id;
    _op_progress  = 2;
    _op_result    = VMOP_RUNNING;
    _op_domid     = _domain_id;
    _op_sender    = TranceMessage._sender;
    _op_ack_end   = false;
    ClearDoubleCmdFlag();

    // �յ�����Ӧ��һ�½���
    SendVmOprAckMsg(_op_sender, req.action_id, _mu);

    // �յ�������Ϣ����Ҫ�Ѳ�ѯ��ʱ���޸�Ϊ1S��ѯһ��
    //StartRefreshTimer();

    OutPut(SYS_DEBUG, "[%s] ExecVmOperation rcv req from %s, operation: %s\n",
           _instance_name.c_str(), \
           _op_sender.serialize().c_str(),\
           Enum2Str::VmOpStr(opr_id));

    // ����յ�CANCEL�����ɾ�������������鶨ʱ��
    if (CANCEL == _op_cmd)
    {
        // �����Ƿ�ɹ���ֱ��ִ���£�û��������ʱ��ʱ��ִ��Ҳû��Ӱ��
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

    //��������ִ����Ӧ�Ĳ���,ע��ι���������
    switch (opr_id)
    {
    case CREATE:
    case START:
    {
        VmCreateReq  createreq;
        createreq.deserialize(TranceMessage._data);

        // ��¼������Ϣ /
        SetVmCfg(createreq._config);
        // ��¼option��Ϣ /
        SetVmOption(createreq._option);

        // ׼������������Ļ���
        STATUS rc = PrepareResource(false);
        if (VMOP_SUCC != rc)
        {
            DoOpResultFunction(rc);
            return;
        }
        // ���ݴ����Ľ������ֵ���д���
        DoOpResultFunction(StartDomain());
        break;
    }
    case CANCEL:
    {
        // CANCEL��ʱ����ִ����ȡ���ϴ��ĺ���
        // ��֤CANCEL���ϴ��������ܹ���������
        // ��ʱû���ϴ���Ҳ��Ҫ������Ϊ�ú���������
        _storage_instance->stop_upload_img(_instance_id,vm_cfg._machine);
        CancelDomain();
        break;
    }
    case STOP:
    {
        // �ر������
        SetProgress(50);
        DoOpResultFunction(StopDomain());
        break;
    }
    case RESET:
    {
        //��λ�����
        SetProgress(50);
        DoOpResultFunction(ResetDomain());
        break;
    }
    case REBOOT:
    {
        // 1�����ڲ�����������
        if (!IsReboot())
        {
            DoOpResultFunction(VMOP_ERROR_REBOOT_FAST);
            return;
        }

        // ���������
        SetProgress(50);
        DoOpResultFunction(RebootDomain());
        break;
    }
    case RR:
    {
        VmRRReq RRMsg;

        RRMsg.deserialize(TranceMessage._data);

        // ���������
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
        // ���������
        SetProgress(50);
        DoOpResultFunction(PauseDomain());
        break;
    }
    case RESUME:
    {
        // �ָ������
        SetProgress(50);
        DoOpResultFunction(ResumeDomain());
        break;
    }
    case PREP_M:
    {
        //Ŀ�Ķ�׼��Ǩ��
        SetProgress(50);
        DoOpResultFunction(PrepareVmMigration(TranceMessage._data));
        break;
    }
    case MIGRATE:
    {
        //Դ��ִ��Ǩ��
        //SetProgress(50);
        ExecVmMigration(TranceMessage._data);
        break;
    }
    case POST_M:
    {
        //Դ��Ǩ�Ƴɹ�
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
* �������ƣ�IsUsbInvalid
* ����������У��USB�豸�Ƿ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�InstanceAddUsb
* ���������������������USB�豸
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
        // ������ػ�״̬��ֱ�ӷ��سɹ�
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
* �������ƣ�InstanceAddUsb
* �������������������USB�豸����ʱ�ӿڣ�libxl����ɾ���ýӿ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�RefrushInstanceInfo
* ����������ˢ�����������ͳ����Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::RefreshInstanceNicInfo()
{
    VmDomainInterfaceStatsStruct nic_info;
    uint32        i;

    // �����������ڲ����У�����LIBVIRT�ӿڵĵ���
    // �������������շ���ͳ��
    if (IsOpRunning())
    {
        return;
    }

    // ��ȡ���ڵ��շ������������0
    _net_tx             = 0;
    _net_rx             = 0;
    uint32               nomal_nic_index=0;
    for (i = 0; i < vm_cfg._nics.size(); i++)
    {
        // �����soiv������ʹ��LIBVIRT�ӿڻ�ȡ����������������ʱ����
        if (vm_cfg._nics.at(i)._sriov)
        {
            continue;
        }

        int res = _vm_driver->GetDomainNetInfoById(_domain_id, nomal_nic_index, nic_info);
        nomal_nic_index++;

        if (0 != res)
        {
            // ��Ӧ�ò�ѯ���ɹ�����¼��־�����㶨λ,�������ǲ�ѯ�Ĺ����У������������
            OutPut(SYS_DEBUG,"refresh_instance_info:%s Get NicInfo failed. no %d,return %d \n",
                   _instance_name.c_str(), i, res);
            continue;
        }
        _net_tx += nic_info.tx_bytes;
        _net_rx += nic_info.rx_bytes;
    }
}


/****************************************************************************************
* �������ƣ�RefrushInstanceCpuUseRateInfo
* ����������ˢ�����������ͳ����Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::RefrushInstanceCpuUseRateInfo(VmDomainInfo &DomInfo)
{
    T_TimeValue now_temp;
    GetTimeFromPowerOn(&now_temp);
    int64 now_ms = now_temp.second*1000 + now_temp.milisecond;

    if (0 != _refresh_time)
    {
        // ����domin��λ��NS��ˢ�µ�λΪms,������Ҫ*1000000,��%�ȣ�����*10000
        // �����CPUʹ���������к˵�ƽ��ֵ
        if ((_vcpu_num != 0) && (now_ms != _refresh_time))
        {
            // ������ڶ�ʱ���ʱ�䷭ת����ý׶β�����CPUʹ���ʼ���
            // ��ΪTECS���������CPUʹ���ʲ�����
            // ���ַ�ת��ԭ�򣬺ܶ�ʱ������Ϊ�����������
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

    // ��¼�ϴ�ˢ��ʱ���DOMAIN����ʱ��
    _refresh_time = now_ms;
    _domain_run_time = DomInfo.cpuTime;
}


/****************************************************************************************
* �������ƣ�NotifyNetDomIDChg
* ����������֪ͨ���粿�֣������ID���ͱ仯
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�ModifyCoredumpFileName
* �����������������ID�仯��ʱ�򣬸��������coredump����Ϊ.core.ok
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::ModifyCoredumpFileName()
{
    uint32 i = 0;
    string coredump_save_local_dir;
    GetCoredumpFilePath(coredump_save_local_dir);
    // �������Լ��������coredump�ļ�����
    string cmd = "ls " + coredump_save_local_dir + " | grep -v tar " + " | grep -w " +  _instance_name;
    vector<string> result;

    RunCmd(cmd, result);

    // �޸�����Ϊcore.ok
    for (i = 0; i < result.size(); i++)
    {
        string is_exit;
        string file_is_open_cmd="lsof  | grep -w " + coredump_save_local_dir;
        RunCmd(file_is_open_cmd, is_exit);
        if (is_exit.size() != 0)
        {
            continue;
        }
        // �ļ�û�д򿪣�������޸�����
        string coredump_file= coredump_save_local_dir + "/" +result.at(i);
        string rename_file = coredump_save_local_dir + "/" +result.at(i) + ".ok";
        rename(coredump_file.c_str(), rename_file.c_str());
    }
}

/****************************************************************************************
* �������ƣ�GetCoredumpFilePath
* ������������ȡ���������coredump�ļ���λ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�OperateWatchDog
* �����������������Ź�����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
bool VmInstance::OperateWatchDog(const char *const  fun_name, int32 op_id)
{
    if (vm_cfg._vm_wdtime == 0)// δʹ�ܿ��Ź�ֱ�ӷ��سɹ�
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
* �������ƣ�RefrushInstanceInfo
* ����������ˢ�������״̬��Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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

    // ���dominid �Ƿ�仯,����dominid�仯ֻ����һ���ط����и���
    if (_domain_id != domid)
    {
        //StartNetWork(__FUNCTION__, VNET_VM_DEPLOY); domid�仯֮ǰ �Ѿ���vnet����startnetwork

        // ����domainid
        _domain_id = domid;

        // ֪ͨ�����Ǳߣ�dominid�仯��
        NotifyNetDomIDChg(__FUNCTION__);

        // ����bind�����CPU
        _vm_driver->VcpuAffinitySet(_instance_name, _vcpu_num);

        if(IsXenKernel())
        {
            // ����������Ƿ������coredump�ļ��ˣ���������� ����ô�޸�����
            ModifyCoredumpFileName();
        }

        InstanceAddUsb();
    }

    // �������״̬
    if (-1 == _vm_driver->GetDomainInfo(_instance_name, DomInfo))
    {
        // ��ѯʧ���ùػ�̬
        _state = VM_SHUTOFF;
        return;
    }

    // ˢ��CPUʹ���ʵ���Ϣ
    RefrushInstanceCpuUseRateInfo(DomInfo);

    // ˢ��״̬
    _state = DomInfo.state;

    // ��¼���а汾��
    GetRunVersion();

    // ��¼��������
    _mem_max            = DomInfo.maxMem*1024;
    _mem_free           = (DomInfo.maxMem - DomInfo.memory)*1024;

    // ˢ�������������Ϣ - 611003891466kvm��δ��֤ͨ������ע������̺���
    //RefreshInstanceNicInfo();

    // �����������Դʹ��ͳ����Ϣ
    _vm_statistics._vec_nic_info.clear();
    DoVmStatistics(_vm_statistics, _instance_id);
    _report_count++;
}

/****************************************************************************************
* �������ƣ�RefreshInstanceReportInfo
* ������������ȡ��ǰʵ�����ϱ���Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::RefreshInstanceReportInfo(VmInfo &info)
{
    // ˢ��ǰ�Ȼ�ȡˢ����
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
        timeout.duration = load_refresh_period;    // Ĭ�϶�ʱ120S
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
* �������ƣ�GetInstanceReportInfo
* ������������ȡ��ǰʵ�����ϱ���Ϣ
* ���ʵı����ר��
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/

bool VmInstance::GetInstanceReportInfo(VmInfo &info)
{
    // ˢ��ǰ�Ȼ�ȡˢ����
    MutexLock lock(_refresh_lock);

    // ������IDΪ-1,��ʾ��ص�ʱ��ȡ�����ݻ�û��ˢ�£��ȴ�����ʵ����
    // ˢ�¹�һ�κ󣬲����ϱ�
    if (_out_info._vm_id == -1)
    {
        return false;
    }

    info                = _out_info;

    return true;
}


/****************************************************************************************
* �������ƣ�SetNetProcess
* ������������������Ľ���
* ���ʵı���
* �޸ĵı���
* ���������0-100
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�SetStorageProcess
* �������������ô����Ľ���
* ���ʵı���
* �޸ĵı���
* ���������0-100
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�SetComputerProcess
* �������������ü���Ľ���
* ���ʵı���
* �޸ĵı���
* ���������0-100
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�SetProcess
* �������������ò�������Ľ���
* ���ʵı���
* �޸ĵı���
* ���������0-100
* ���������
* �� �� ֵ��
* ����˵����һ���������������ִ�н���
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::SetProgress(uint32 progress)
{
    if (progress > 100)
    {
        progress = 100;
    }

    // ��������в���Ĭ�ϴ�1��ʼ
    if (progress == 0)
    {
        progress = 1;
    }

    _op_progress = progress;
}

/****************************************************************************************
* �������ƣ�RefrushVmState
* ����������ˢ�������״̬
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::RefreshVmState()
{
    // ���ҵ�ǰ�������״̬
    _state = _vm_driver->GetDomainState(_instance_name);
}

/****************************************************************************************
* �������ƣ�InstanceSave
* �������������������Ϊӳ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::InstanceUpload(const string& message)
{
    // ʵ�ֲ����Ľ���
    VmSaveReq req;
    req.deserialize(message);

    // �жϵ�ǰ��������Ƿ��ǹػ�̬
    if (VM_SHUTOFF != _state)
    {
        return VMOP_ERROR_STATE;
    }

    if (0 == vm_cfg._machine._size)
    {
        return VMOMP_UPLOAD_ERROR;
    }

    // ���ô洢�ӿڣ��ϴ�img��IMG��������ȥ
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
* �������ƣ�InstanceUnsave
* ����������ȡ�����������Ϊӳ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::InstanceCancelUpload()
{
    // �����κ����飬ֱ�ӷ��سɹ�
    SetProgress(50);
    return _storage_instance->stop_upload_img(_instance_id,vm_cfg._machine);
}

/****************************************************************************************
* �������ƣ�DomainIsExist
* �����������ж�������Ƿ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�StartDomain
* �����������������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::StartDomain()
{
    bool     result = false;
    int      domid = 0;

    string config_file = tecs_instance_path+_instance_name+"/"+_instance_name+".xml";

    // �������������ڣ��ߴ�������
    if (!DomainIsExist())
    {
        SetProgressDesc(START_EXEC_CREATE);
        result = _vm_driver->Create(config_file, &domid);

        // ���������ʧ�ܵ�ʱ�򣬲�������Ȩ��
        if ((result == false) || (domid == 0))
        {
            // ���ز���ʧ��
            OutPut(SYS_ALERT,"Create %s Fail.\n",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }

        // ����dominid
        _domain_id = domid;

        // ֪ͨ�����Ǳߣ�dominid�仯��
        if (!NotifyNetDomIDChg(__FUNCTION__))
        {
            return VMOP_ERROR_NETWORK_NOTIFY;
        }

        // ��������ĺ˽���ȫ��bind
        _vm_driver->VcpuAffinitySet(_instance_name, vm_cfg._vcpu);
        // �����������weight��cap
        _vm_driver->Sched_credit_weight_set(_instance_name, vm_cfg._cpu_percent);

        // ���TCU��������ֵCAPֵ
        if (vm_option._is_tcu_share)
        {
            _vm_driver->Sched_credit_cap_set(_instance_name, vm_cfg._vcpu, 100);
        }
        else
        {
            _vm_driver->Sched_credit_cap_set(_instance_name, vm_cfg._vcpu, vm_cfg._cpu_percent);
        }


        // ����ϵͳ����ʱ��,��ֹϵͳ������Ƶ������
        SetBootTime();

        // ����USB�豸�����������
        STATUS ret = InstanceAddUsb();
        if ( ret != SUCCESS)
        {
            return ret;
        }

        return VMOP_SUCC;
    }

    // ˢ�������״̬
    RefreshVmState();

    if (VM_RUNNING == _state)
    {
        return VMOP_SUCC;
    }

    OutPut(SYS_ERROR,"%s state %d can't Start.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* �������ƣ�StopDomain
* �����������ر������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::StopDomain()
{
    // �������������ڣ�ֱ�ӷ��سɹ�
    if (!DomainIsExist())
    {
        return VMOP_SUCC;
    }

    // ˢ�������״̬
    RefreshVmState();

    // ����ǹػ�̬��Ҳֱ�ӷ��سɹ�
    if (VM_SHUTOFF == _state)
    {
        return VMOP_SUCC;
    }

    // ���������̬����ִ�йػ�����
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
* �������ƣ�StopDomain
* �������������������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::PauseDomain()
{
    // �������������ڣ�����ʧ��
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"Suspend %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // ˢ�������״̬
    RefreshVmState();

    // ����ǹ���̬��Ҳֱ�ӷ��سɹ�
    if (VM_PAUSED == _state)
    {
        return VMOP_SUCC;
    }

    // ���������̬����ִ�й�������
    if (VM_RUNNING == _state)
    {
        SetProgressDesc(START_EXEC_PAUSE);
        if (-1 == _vm_driver->Suspend(_instance_name))
        {
            OutPut(SYS_ALERT,"Suspend %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }
        // ��������ִ�����ʱ����Ҫֹͣ���Ź�
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
* �������ƣ�ResumeDomain
* �����������ָ������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::ResumeDomain()
{
    // �������������ڣ�����ʧ��
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"resume %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // ˢ�������״̬
    RefreshVmState();

    // ���������̬��Ҳֱ�ӷ��سɹ�
    if (VM_RUNNING == _state)
    {
        return VMOP_SUCC;
    }

    // ����ǹ���̬����ִ�лָ�����
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
* �������ƣ�RebootDomain
* �������������������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::RebootDomain(bool need_stop_wgd )
{
    // �������������ڣ�����ʧ��
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"reboot %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // ˢ�������״̬
    RefreshVmState();

    // ���������̬��ִ��REBOOT����
    if (VM_RUNNING == _state)
    {
        SetProgressDesc(START_EXEC_REBOOT);
        if (-1 == _vm_driver->Reboot(_instance_name))
        {
            OutPut(SYS_ALERT,"Reboot %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }
        // ��������ֹͣ���Ź�
        if(need_stop_wgd)
        {
            if (!StopWDGTimer(__FUNCTION__))
            {
                return VMOP_ERROR_WATCHDOG_STOP;
            }
        }
        // ����ϵͳ����ʱ��,��ֹϵͳ������Ƶ������
        SetBootTime();
        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Reboot.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* �������ƣ�ResetDomain
* ����������ǿ�����������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::ResetDomain(bool need_stop_wgd )
{
    // �������������ڣ�����ʧ��
    if (!DomainIsExist())
    {
        OutPut(SYS_ALERT,"Reset %s Fail, vm does not exist.",_instance_name.c_str());
        return VMOP_VM_DO_FAIL;
    }

    // ˢ�������״̬
    RefreshVmState();

    // ���������̬��ִ��reset����
    if (VM_RUNNING == _state || VM_PAUSED == _state || VM_CRASHED == _state)
    {
        SetProgressDesc(START_EXEC_RESET);
        string config_file = tecs_instance_path+_instance_name+"/"+_instance_name+".xml";
        if (0 != _vm_driver->Reset(_instance_name, config_file))
        {
            OutPut(SYS_ALERT,"Reset %s Fail.",_instance_name.c_str());
            return VMOP_VM_DO_FAIL;
        }

        // ֹͣ���Ź�
        if(need_stop_wgd)
        {
            if (!StopWDGTimer(__FUNCTION__))
            {
                return VMOP_ERROR_WATCHDOG_STOP;
            }
        }
        // ����ϵͳ����ʱ��,��ֹϵͳ������Ƶ������
        SetBootTime();
        return VMOP_RUNNING;
    }

    OutPut(SYS_ALERT,"%s state %d can't Reset.",_instance_name.c_str(), _state);
    return VMOP_VM_DO_FAIL;
}

/****************************************************************************************
* �������ƣ�DestroyDomain
* ����������ǿ�ƹر������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�RecoverDomain
* ���������������һ���ָ�����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2013/05/26    V1.0   zhanghao        ����
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
* �������ƣ�SetDiskInfo
* ��������������disk��Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    yanwei         ����
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
* �������ƣ�GetDiskInfo
* ������������ȡ��ʵ����disk����Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    yanwei         ����
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
* �������ƣ�PredealStorage
* �����������洢Ԥ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������disks ���õ�disk�б�
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/19    V1.0    yanwei         ����
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
                //��Ҫ:�����ݲ�֧�� ӳ��ֻ��id���ж�.�����Ҫ֧��Ӳ�̵Ļ����迼���µķ�������CC����ɾ����Щ�洢��.
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
* �������ƣ�StartStorage
* �����������洢��ʼ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
*     �洢����׼��������ӳ�����غ�����洢�ķ���
*     Ҳ���ܱ����Ѿ������ɴ洢�ṩ�Ľӿڸ�������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/19    V1.0    yanwei         ����
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
    //img����ռ�����洢���ȵ�80% ÿ��imgռ80/imgs;���豸ռ�洢���ȵ�20% ÿ��blockռ20/blocks
    if (0 == blocks)
    {
        imgs = 100/imgs;
    }
    else
    {
        imgs = 80/imgs;
        blocks = 20/blocks;
    }
    //imgsΪÿ��ӳ����ռ�洢�Ľ��� blocksΪÿ�����豸��ռ�洢�Ľ���
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
                timeout.duration = load_refresh_period;    // Ĭ�϶�ʱ120S
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
                /*�ȴ����*/
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
                        //611003915111 ��get_storageһ��                        
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
                        //tmp���Ϊ��ӳ���5% ��������ռ60%
                        itemprogress = 5 + (((cursize*100)/it->_size) * 6)/10;
                        if (cursize < it->_size)
                        {
                            downloads++;
                        }
                        else
                        {
                            //����md5ռ15%
                            checksums++;
                            //���ÿ10����ȼ�1 �ҽ��Ȳ��ܳ���80%
                            itemprogress += checksums/10;
                            if (80 < itemprogress)
                            {
                                itemprogress = 80;
                            }
                        }
                    }
                    else if (STORAGE_ITEM_BASE_READY == state)
                    {
                        //base׼���ý���Ϊ85
                        itemprogress = 85;
                    }
                    else if (STORAGE_ITEM_SS_READY == state)
                    {
                        //����׼���ý���Ϊ95
                        itemprogress = 95;
                    }
                    else
                    {
                        itemprogress = 100;
                    }
                    //������ô洢��Ľ���
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
                    /*�յ�cancel��Ϣ ֱ���˳�*/
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
* �������ƣ�ReleaseStorage
* �����������ͷŴ洢��Դ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
*     �洢��Դ�ͷţ�����ӳ��ɾ��������洢�Ļ���
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/19    V1.0    yanwei         ����
****************************************************************************************/
STATUS VmInstance::ReleaseStorage()
{
    STATUS ret = SUCCESS;//ĳһ���ͷ�ʧ���ݲ�����,Ϊ����ÿһ��cancel�����ͷŸ���Ĵ洢��Դ
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
        {//��γ��Բ��ɹ�����ʧ����cc��������
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
            {//��γ��Բ��ɹ�����ʧ����cc��������
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
 *    �����ļ����                                                   *
 *    ·��: /var/lib/tecs/instances/instancename/instancename.xml    *
 ********************************************************************/

/****************************************************************************************
* �������ƣ�CreateConfigFile
* �������������������ʵ���������ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         ����
* 2012/06/04    V1.0    GuoXiang    �޸�:�����������óɿ���
****************************************************************************************/
int VmInstance::CreateConfigFile()
{
    // ����xmlparse
    if (!CreateCfgParaIsValid())
    {
        return -1;
    }

    /* ��������ļ� */
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
 * CONTEXT ���                                                         *
 * ·�� /var/lib/tecs/instances/instancename/iso/tecs-context.xml       *
 *      /var/lib/tecs/instances/instancename/tecs-context.iso           *
 *                                                                      *
 ***********************************************************************/

/*******************************************************************************
* �������ƣ�CreateFile
* ���������������������ĳ���ļ���
* ���������filename �ļ�����filecontext �ļ�����
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         ����
* 2012/05/30    V1.0    �Ӵ�ɽ       ����˳�����
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
* �������ƣ�CreateTecsContext
* �������������������Ϣ�ռ���������virtual.xml�����浽isoĿ¼�¡�
* �����������
* ���������
* �� �� ֵ��CREATE_EMPTY �����ݣ�����д�ļ�
*           CREATE_ERROR �ļ�����ʧ��
*           CREATE_SUCC  �ļ������ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------------
* 2012/05/30    V1.0    �Ӵ�ɽ       �� ��
*******************************************************************************/
int VmInstance::CreateTecsContext()
{
    string name;
    string context;

    //��ȡҪд��iso������
    context = CreateVmMetadata(this);
    if(context.empty())
    {
        OutPut(SYS_DEBUG,"No need to create tecs context!\n");
        return CREATE_EMPTY;
    }

    //�ȴ�������ļ���Ŀ¼
    name = tecs_instance_path + _instance_name + "/iso/tecs";
    
    //string mkdir_cmd = " mkdir -p " + name;
    //if(0 != system(mkdir_cmd.c_str()))
    if(0 != CreateDir(name, 0))
    {
        OutPut(SYS_ERROR,"CreateTecsContext: mkdir %s failed!\n", name.c_str());
        return CREATE_ERROR;
    }
    //������ļ���
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
* �������ƣ�CreateUserContext
* �������������ݵ����·���context�ֶΣ�������Ӧxml�ļ������浽isoĿ¼�¡�
* �����������
* ���������
* �� �� ֵ��CREATE_EMPTY �����ݣ�����д�ļ�
*           CREATE_ERROR �ļ�����ʧ��
*           CREATE_SUCC  �ļ������ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------------
* 2012/05/30    V1.0    �Ӵ�ɽ       �� ��
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

    //�ȴ�����ŵ�Ŀ¼
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
* �������ƣ�CreateContext
* ���������������û������ļ��������Ԫ�����ļ�
* �����������
* ���������
* �� �� ֵ��CREATE_EMPTY �����ݣ�����д�ļ�
*           CREATE_ERROR �ļ�����ʧ��
*           CREATE_SUCC  �ļ������ɹ�
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------------
* 2012/05/30    V1.0    �Ӵ�ɽ       �� ��
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

    //��Ϊ�ղ���Ҫ��������1��ʧ�ܷ���-1���ɹ�����0
    if((tecs_ret == CREATE_EMPTY)&&(user_ret == CREATE_EMPTY))
    {
        return CREATE_EMPTY;
    }
    return CREATE_SUCC;
}
/****************************************************************************************
* �������ƣ�CreateContextIso
* ����������ʹ��mkisofs���ߣ���./iso/tecs-context.txt����tecs-context.iso
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    mhb         ����
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
* �������ƣ�GetContextDiskTarget
* ������������ȡ�����Ĵ��̵�Ŀ�����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵�����°汾�����У�������ֻ�����hdd��!
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixiaocheng  ����
****************************************************************************************/
string VmInstance::GetContextDiskTarget()
{
    return "hdd";
}

/****************************************************************************************
* �������ƣ�SendVmOprAckMsg
* �����������ز������Ӧ���CC
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixiaocheng  ����
****************************************************************************************/
void VmInstance::SendVmOprAckMsg(const MID &receiver, string &action_id, MessageUnit *mu)
{
    uint32 result   = _op_result;
    uint32 progress = _op_progress;

    // ����Ӧ��WorkAck�� statΪVMOP_RUNNING��������progressΪ100
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
* �������ƣ�SendVmOprAckMsg
* �����������ز������Ӧ���CC
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixiaocheng  ����
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

    // �����ǰ����������ڽ��У����¼��ǰ�Ĳ���ϸ��
    string detail;
    if(VMOP_RUNNING == result)
    {
        detail = _op_process_desc;
    }
    else
    {
        detail = Enum2Str::VmOpError(result);

        /* �ɹ�ʧ�ܣ�����Ҫ���ã�����busy */
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
* �������ƣ�PrepareResource
* �����������������������׼��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixiaocheng  ����
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

    // ����Ӧ��WorkAck�� statΪVMOP_RUNNING��������progressΪ100
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
* �������ƣ�SendDel2VmHandler
* ����������֪ͨVM_HANDLERɾ����ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------------
* 2013/06/29    V1.0    chenww  ����
*******************************************************************************/
void VmInstance::SendDel2VmHandler( )
{
    //SkyAssert(VMOP_SUCC == _op_result);

    //SkyAssert(100 == _op_progress);

    // ��¼��ǰ�Ĳ���ϸ��
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
* �������ƣ�PrepareResource
* �����������������������׼��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixiaocheng  ����
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

    // 1����Ŀ¼,ֱ�ӽ���/var/lib/tecs/instances/instanceid/iso/
    string instance_iso_dir = tecs_instance_path + _instance_name + "/iso";
    string mkdir_cmd        = " mkdir "+instance_iso_dir+"  -p ";

    system(mkdir_cmd.c_str());

    if(IsXenKernel())
    {
        // ���� coredump url
        SetProgressDesc(START_MOUNT_COREDUMP_URL);
        string coredump_save_local_dir;
        GetCoredumpFilePath(coredump_save_local_dir);

        // ��CC���ݹ�����Զ��Ŀ¼����mount
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

    // 2��init network,��¼����ֵ
    SetProgressDesc(START_PREP_NET);
    OutPut(SYS_DEBUG, "%s begin applying network resource,vid:%d,\n", _instance_name.c_str(), _domain_id);
    bool is_success = StartNetWork(__FUNCTION__, option);
    if(!is_success)
    {
        return CREATE_ERROR_STARTNETWORK;
    }
    OutPut(SYS_DEBUG, "%s  applying network resource succ,vid:%d,\n", _instance_name.c_str(), _domain_id);
    
    // ���ý���
    SetNetProgress(100);

    // �ж��Ƿ�Cancel
    if (true == InstanceIsDestroy())
    {
        return VMOP_ERROR_CANCEL_ABORT;
    }

    // 3�����ô����ӿڣ�׼���洢��ص���Դ
    SetProgressDesc(START_PREP_STORAGE);
    rc = StartStorage();
    if(0 != rc)
    {
        return rc;
    }
    // ���ý���
    SetStorageProgress(100);

    // �ж��Ƿ�Cancel
    if (true == InstanceIsDestroy())
    {
        return VMOP_ERROR_CANCEL_ABORT;
    }

    // 4������ISO
    SetProgressDesc(START_PREP_CONTEXT);
    rc = CreateContext();
    // ���ý���
    SetComputerProgress(20);
    if(rc == CREATE_SUCC)
    {
        rc = CreateContextIso();
        if(0 != rc)
        {
            return CREATE_ERROR_CREATEISO;
        }

        // �Ѹ�iso��Ϊ׷�ӵ���ǰ���һ���̷�
        zte_tecs::VmDiskConfig contextdisk;
        contextdisk._type          = "ro_disk";    /*  ֻ������ */
        contextdisk._source_type   = "file";
        contextdisk._source = tecs_instance_path + _instance_name + "/tecs-context.iso";
        contextdisk._target = GetContextDiskTarget();
        contextdisk._driver = "file";
        contextdisk._bus    = "ide";

        // ��������б�ΪNULL,��ֱ�Ӱ�contextѹ��
        if (vm_cfg._disks.empty())
        {
            vm_cfg._disks.push_back(contextdisk);
        }
        else
        {
            // ������һ�����̵�source������context���̷����ݣ����context
            // ���뵽�����б�
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

    // ���ý���
    SetComputerProgress(40);

    // �ж��Ƿ�Cancel
    if (true == InstanceIsDestroy())
    {
        return VMOP_ERROR_CANCEL_ABORT;
    }

    // ��ȡ�����豸��PCI��Ϣ
    GetSdPciInfo(vm_cfg._pci_devices);

    // 5��create configure xml,Ҫ�õ�����ʹ洢�ӿڵķ�����Ϣ,ע��pvm/hvm������
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
* �������ƣ�ExitDestroy
* �����������˳���������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::ExitDestroy(const STATUS rs)
{
    DoOpResultFunction(rs);
    VmClearDestroyFlag();
}

/****************************************************************************************
* �������ƣ�EnterDestroy
* ����������������������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::EnterDestroy()
{
    // cancel����Ǩ�Ƴɹ������������̣�����������ʵ��
    //SetProgress(100);
    //SetOpResult(VMOP_SUCC);

    // ��֤��������Ϣ��ֻӰ����Ϣ�Ľ��գ�������ķ���û��Ӱ��
    _mu->SetHandler(NULL);

    // ʹ�øýӿڣ�������Ϣ��VMHANDLER,VmHandler�յ��󣬿�ʼ�ͷ���Դ
    SendDel2VmHandler();
}

/****************************************************************************************
* �������ƣ�CancelDomain
* ����������Cancel����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::CancelDomain()
{
    // ���õ�ǰ�̴߳���Cancel״̬
    VmSetDestroyFlag();

    if(!StopWDGTimer(__FUNCTION__))
    {
        OutPut(SYS_DEBUG, "%s stopWdTimer failed.\n", _instance_name.c_str());
    }
        
    // ֹͣ�����,�������������ڣ��ú���Ҳ�᷵�سɹ�
    if (VMOP_VM_DO_FAIL == DestroyDomain())
    {
        ExitDestroy(VMOP_VM_DO_FAIL);
        return;
    }

    uint32 wait_time = 0;
    // ��⿴������Ƿ�������
    do
    {
        // �������������
        if (-1 == _vm_driver->GetDomainIDByName(_instance_name))
        {
            break;
        }
        Delay(1000);
        //  ���60S�������û����������cancelʧ��
        wait_time++;
        if (wait_time >= 60)
        {
            ExitDestroy(VMOP_VM_DO_FAIL);
            return;
        }
    }
    while(1);

    SetProgress(10);

    // ɾ�������������ʱ1S��������Դ�ͷţ�������xend����loop�豸�ͷ�
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
* �������ƣ�ReleaseResource
* �����������ͷ���Դ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::ReleaseResource()
{
    STATUS   rc;

    // �ͷ�������Դ
    SetProgressDesc(START_REL_NET);
    bool is_success = StopNetWork(__FUNCTION__, VNET_VM_CANCEL);
    if (!is_success)
    {
        OutPut(SYS_ALERT, "%s StopNetWork fail\n", _instance_name.c_str());
        return VMOP_ERROR_NETWORK_STOP;
    }
    SetNetProgress(100);

    // �ͷŴ�����Դ
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

    // ɾ��Ŀ¼
    SetProgressDesc(START_DEL_DIR);
    string cmd("rm -rf ");
    cmd = cmd + tecs_instance_path+_instance_name;
    system(cmd.c_str());

    if(IsXenKernel())
    {
        string coredump_save_local_dir;
        GetCoredumpFilePath(coredump_save_local_dir);
        // ֱ��umount coredumpĿ¼
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

            // ɾ�����ڵ�Ŀ¼
            cmd = "rm -fr " + coredump_save_local_dir;
            system(cmd.c_str());
        }
    }
    return SUCCESS;
}


/****************************************************************************************
* �������ƣ�SendMsgToVna
* ����������������Ϣ��VNA��ȥ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������out_msg ������Ϣ�ɹ����յ���Ӧ����Ϣ����
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2013/04/07    V1.0    lixch         ����
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


    /* ������ʱ��Ϣ��Ԫ */
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
        // �ϵ��룬������Խ�磬ҪԽ����Ҫ136��
        if ((now_time.second - start_time.second) > time_out)
        {
            OutPut(SYS_ALERT, "*** Instances %lld Send Msg To Vna TimeOut, time = %d ***\n",
                   _instance_id, time_out);
            return false;
        }
        // ���2���ղ��������Ӧ�����ط���Ϣ������ģ��
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

        // ������緵�ص����м����ݣ���Ҫ�����ȴ�Ӧ��������
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
* �������ƣ�StartNetWork
* �������������뱾ʵ�����������Դ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
    //Я��һ�±�����Ϣ��md5��Ϊaction_id��Ŀ����Ϊ�˼�������Ӧ����Ϣ�ĺϷ��ԣ�����Ӧ�𾿾��Ƿ���Ա�������
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
    // �����ȡ��������
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

    // ���濴�Ź���Ϣ
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
* �������ƣ�StopNetWork
* �������������뱾ʵ�����������Դ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch         ����
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
* �������ƣ�PrepareVmMigration
* ����������Ŀ�Ķ�׼�������Ǩ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/12/30    V1.0    mhb         ����
****************************************************************************************/
STATUS VmInstance::PrepareVmMigration(const string& message)
{
    string        sshkey;
    VmMigrateData prepare;
    string        vmcfg;                  /* ������ϢЯ����������Ϣ */

    prepare.deserialize(message);
    sshkey        = prepare._src_ssh_public_key;

    OutPut(SYS_DEBUG, "VmInstance::PrepareVmMigration, recv req name: %s\n", _instance_name.c_str());

    /* ��¼CREATE��ʱ��� */
    SetLaunchTime();

    /* ��¼������Ϣ */
    SetVmCfg(prepare._config);

    // ׼������������Ļ���
    STATUS rc = PrepareResource(true);
    if (VMOP_SUCC != rc)
    {
        return rc;
    }

    // ��¼sshkey
    SshTrust(sshkey);

    return VMOP_SUCC;
}

/****************************************************************************************
* �������ƣ�ExecVmMigration
* ����������Դ��ִ�������Ǩ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/17    V1.0    lixch         ����
****************************************************************************************/
void VmInstance::ExecVmMigration(const string& message)
{
    VmMigrateData  migrate;
    STATUS        rc = 0;

    migrate.deserialize(message);
    string host_ip           = migrate._targe_ip;

    OutPut(SYS_DEBUG, "VmInstance::ExecVmMigration, recv Migrate name: %s\n", _instance_name.c_str());

    // ���ó�����־
    VmSetDestroyFlag();

    // ֹͣ���Ź�
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

    // �ͷ���Դ, ʧ�ܲ�ִ��
    rc = ReleaseResource();
    if (SUCCESS != rc)
    {
        ExitDestroy(rc);
        return;
    }

    EnterDestroy();
}

/****************************************************************************************
* �������ƣ�HandleVmMigrateSucc
* ����������Դ��Ǩ�Ƴɹ���֪ͨĿ�Ķ���������л�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::HandleVmMigrateSucc(const string& message)
{
    VmMigrateData  inform;

    inform.deserialize(message);

    OutPut(SYS_DEBUG, "VmInstance::HandleVmMigrateSucc, recv MigrateSucc name: %s\n", _instance_name.c_str());

    // ���sshkeyȨ��
    //SshUntrust(inform._src_ssh_public_key);

    // ˢ�������״̬
    RefreshVmState();

    // ��������ʱ��
    SetBootTime();

    // ˢ�����µ�domid
    _domain_id = _vm_driver->GetDomainIDByName(_instance_name);

    // ��������:�����Ϳ��Ź�
    bool is_success = StartNetWork(__FUNCTION__, VNET_VM_MIGRATE);
    if(!is_success)
    {
        return VMOP_ERROR_NETWORK_START;
    }
    
    // ֪ͨ�����Ǳߣ�dominid�仯��
    if (!NotifyNetDomIDChg(__FUNCTION__))
    {
        return VMOP_ERROR_NETWORK_NOTIFY;
    }


    return VMOP_SUCC;
}

/****************************************************************************************
* �������ƣ�AttachDisk
* ������������̬����һ���̵ĺ���ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::PlugInPdisk(const string& message)
{
    VmAttachReq req;
    req.deserialize(message);

    // ��������ǹػ�״̬��ʱ��ֱ�ӷ��سɹ�
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
        timeout.duration = load_refresh_period;    // Ĭ�϶�ʱ120S
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
* �������ƣ�DeAttachDisk
* ������������̬ɾ��һ���̵�ʵ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
STATUS VmInstance::PlugOutPdisk(const string& message)
{
    // ʵ�ֲ����Ľ���
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
* �������ƣ�ImageBackupCreate
* �����������������񱸷�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    fj         ����
****************************************************************************************/
STATUS VmInstance::ImageBackupCreate(const string& message)
{
    // ʵ�ֲ����Ľ���
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
* �������ƣ�ImageBackupDelete
* ����������ɾ�����񱸷�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2013/7/14     V1.0    zhanghao         ����
****************************************************************************************/
STATUS VmInstance::ImageBackupDelete(const string& message)
{
    // ʵ�ֲ����Ľ���
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
        /* ���ǵ���װ tecs �汾֮ǰϵͳ�Ѿ�������ͨ��
         * �������˶��ͳ�Ƽ��ʱ����շ�����С, 
         * �ٺ� 0 ����ֵ, �õ���ʹ�����Ǻܴ�, ������
         * �ʵ�һ�ν���¼���ڴ���, ���ݿ���Ϊ0, ������������
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
    int time_interval = 5; // 5s, ��λ����
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
            // Ѱ�ҵ���Ӧ������, ������Ϊ pre
            it_now->_used_rate = 1000 * (it_now->_net_flow - it_pre->_net_flow) / (it_now->_speed * time_interval);
            *it_pre = *it_now;
        }
    }
    _pre_vm_statistics._vec_nic_info.clear();
    _pre_vm_statistics._vec_nic_info = vec_nic_info;
}

/******************************************************************************/
/* brief: vNIC ������
 * in   : strallports - vNIC ��Ӧ�����ϵ����ж˿�
 * out  : ispeed - vNIC ��Ӧ������˿ڵ�����
 * return: SUCCESS/ERROR
 * note : 
 *  1) ��λΪ B/s
 *  2) ��ȡ��ʵ���� vNIC ��Ӧ������˿ڵ�����, �ҽ�����һ������˿�
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
        break; // ������һ������˿�
    }
    return 0;
}

/****************************************************************************************/
int32 VmInstance::XenVmStatistics(ResourceStatistics& vm_statistics,
                                  const int64& vid)
{
    // ͳ��ʱ��
    Datetime now;
    vm_statistics._statistics_time = now.tostr();
    string strtmp;
    /* CPUʹ����/MEMʹ����
     * �漰����: 
     *      xentop  - ��ȡ����dom��cpuʹ����
     * ��ӡ:
     *      NAME          STATE       CPU(sec) CPU(%)     MEM(k) MEM(%)
     *      instance_1020 --b---      14939     13.2    1052644    4.2
     * ����:
     *      ��ȡ����ʹ�����Ѿ�*100, then rate_from_xentop*10
     * note: �˴����ڴ�ʹ����ָ���Ƿ����VM���ڴ�ռ��������ܵ��ڴ�ı���
     */
    string cmd = "xentop -b -f -i 2 -d 0.5|grep -E \"Domain-0|instance_\"";
    int ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }

    // 10 offset �ܱܿ���һ�ε�ͳ��
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
            //Domain-0 ��ͳ��
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
             * �ո�Ϊ�����, �� 4 ������
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
             * �ո�Ϊ�����, �� 6 ������
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

    /* ����ʹ����
     */
    /* vid ��Ҫת���ַ���, ����������ʹ��
     */
    strvid.clear();
    char actmp[20];
    memset(actmp, 0, 20);
    sprintf(actmp, "%lld", vid);
    strvid = actmp;
    /* ��ȡ vid ��Ӧ������� domid
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
    /* ��������������˶����� vNIC
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
        /* �ж��˿�Ϊ tap ���� vif, tap ���ȼ���
         * ovs-vsctl show | grep tap ���� vif
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
        /* ��ȡ����
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
        /* ��ȡ rx, tx ��ֵ, ��λΪ�ֽ�
         * ovs-ofctl dump-ports br port | grep rx or tx | awk ����
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
        /* vNIC ������ȡ��Ӧ����˿ڵ�����
         * ovs-vsctl list-ports strbr ��ȡ�����еĶ˿�
         * �ж϶˿��Ƿ�������˿�
         *  ls -l /sys/class/net | grep pci | grep strport
         *  note: ������һ������˿�
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
    // ���� vid ���������������
    ostringstream oss;
    oss << "instance_" << vid;
    /* ps -eo pid,args
     *  ps ��ʾ pid �� ���̵Ĳ���, �����а��������������
     * grep instance_x
     *  grep �ҳ���������ڵĽ���
     * awk -F ' ' '{if($4 == \"instance_x\") print $1}'
     *  �������� grep ɸ���Ķ�����Ϣ
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
    /* ���� pid ��ȡ��Ӧ�� CPU �� MEM ʹ����
     * top ������ʾ���� example:
     *  7169 qemu      20   0  381m  34m 4456 R  0.0  1.9   7:30.24 qemu-kvm
     *  -b: batch ģʽ, �����ڹܵ�
     *  -n: ��������
     *  -d: �������ʱ��(Ĭ���� 3s , ��Ϊ 1s)
     * NOTE: ��һ��Ϊ��ϵͳ���е�ƽ��ֵ, ���ȡ�ڶ��ε�
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
        // , ��Ϊ CPU , ��Ϊ MEM
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
    
    /* ����ʹ����ʵ�ַ���
     * �������:
     *  ����ʱ��� t(s), �������� 100Mb/s
     *  ����ʹ����: t�ڵ��շ����Ĵ�С(Mb)/(100 * t)
     * ����ʵ��:
     *  KVM �������������Ʋ�����vif��tap��ͷ����������vnet0��vnet1�ĸ�ʽ
     * �������3:
     *  �μ������С����ʵ��
     */
    /* ��ȡ�������Ӧ����������
     * virsh domiflist instance_x
     * NOTE: ���ܴ��ڶ��
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
        /* ɾ�����Դ�ӡ, ǰ 2 �д��͵�
         * ��ʽ����
         *  Interface  Type       Source     Model       MAC
         * -------------------------------------------------------
         *  vnet0      bridge     sdvs_demo  -           00:d0:d0:00:00:03
         */
        index++;
        if (3 > index)
            continue;
        /* ժ���˿�����������, ������ vnet0, sdvs_demo
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
        /* ��ȡ rx, tx ��ֵ, ��λΪ�ֽ�
         * ovs-ofctl dump-ports br port | grep rx or tx | awk ����
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
        /* vNIC ������ȡ��Ӧ����˿ڵ�����
         * ovs-vsctl list-ports strbr ��ȡ�����еĶ˿�
         * �ж϶˿��Ƿ�������˿�
         *  ls -l /sys/class/net | grep pci | grep strport
         *  note: ������һ������˿�
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
/* lixiaocheng���Խӿڳ���,start */

void testsetallvcpu(const string &name, int vcpu)
{
    VmDriver *pDrv  = NULL;

    pDrv = VmDriver::GetInstance();

    /* ��������ĺ˽���ȫ��bind */
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
/* lixiaocheng���Խӿڳ���,end */


