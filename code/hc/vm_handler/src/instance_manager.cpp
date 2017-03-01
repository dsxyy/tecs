/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�instance_manager.cpp
* �ļ���ʶ��
* ����ժҪ��InstanceManager���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/26
*     �� �� �ţ�V1.0
*     �� �� �ˣ�mhb
*     �޸����ݣ�����
* �޸ļ�¼2��
*     �޸����ڣ�2011/8/17
*     �� �� �ţ�V1.0
*     �� �� �ˣ�jixp
*     �޸����ݣ�ʵ�������ʵ��״̬��⼰��������Ź����
*******************************************************************************/
#include "instance_manager.h"
#include "vm_messages.h"
#include "sky.h"
#include "registered_system.h"
#include "vm_handler.h"

using namespace zte_tecs;

//�����״̬��ѯ5s ��ʱ��
const int k_vm_status_moni_timer          = 5000;


InstanceManager *InstanceManager::instance = NULL;
/******************************************************************************/
/* InstanceManagerer :: Constructor/Destructor                                */
/******************************************************************************/
InstanceManager::InstanceManager():_mu(NULL),_vm_driver(NULL),_instance_pool_ctrl(NULL),
        _vm_moni_tid(INVALID_TIMER_ID)
{
}

InstanceManager::~InstanceManager()
{
    if (NULL != _mu)
    {
        delete(_mu);
    }
}

/****************************************************************************************
* �������ƣ�Init
* ���������������ʵ�������ʼ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
STATUS InstanceManager::Init()
{
    // ��ȡVM�����������ʵ��
    _vm_driver      = VmDriver::GetInstance();

    if (NULL == _vm_driver)
    {
        OutPut(SYS_ALERT, "VmDriver::GetInstance()  failed in InstanceManager:: init() !\n");
        return ERROR;
    }

    // ��ȡ�����ʵ���صľ��
    _instance_pool_ctrl =InstancePool::GetSelf();
    if (NULL == _instance_pool_ctrl)
    {
        OutPut(SYS_ALERT, "InstancePool GetSelf()  failed in InstanceManager:: init() !\n");
        return ERROR;
    }

    // ����MU
    if (SUCCESS != StartMu())
    {
        OutPut(SYS_ALERT, "InstancePool StartMu  failed in InstanceManager:: init() !\n");
        return ERROR;
    }
    
    return SUCCESS;
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
STATUS InstanceManager::StartMu()
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    _mu = new MessageUnit(INSTANCE_MANAGER);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n","InstanceManager");
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n","InstanceManager",ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n","InstanceManager",ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n","InstanceManager",ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
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

/****************************************************************************************
* �������ƣ�MessageEntry
* ���������������ʵ����Ϣ���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        ����
****************************************************************************************/
void InstanceManager::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
                case EV_POWER_ON:
                {
                    VmHandler *pHandler = VmHandler::GetInstance();

                    if (NULL == pHandler)
                    {
                        SkyExit(-1,"InstanceManager: Get VmHandler Instance fail!\n");
                        return;
                    }
                    
                    // ���VmHandler�Ƿ������ɹ�
                    while(1)
                    {
                        Delay(2000); //�ӳ�2S����
                        if (S_Work == pHandler->GetVmHandlerWorkState())
                        {
                            // ���VmHandler�����ɹ��������̼߳���������
                            break;
                        }
                    }
                    
                    //����ʵ��״̬��ض�ʱ��
                    _vm_moni_tid = _mu->CreateTimer();
                    if (INVALID_TIMER_ID == _vm_moni_tid)
                    {
                        SkyExit(-1,"CreateTimer _vm_moni_tid failed in InstanceManager:: init() !\n");
                        return;
                    }
                    
                    StartMoniterTimer();
                    
                    _mu->set_state(S_Work);
                    OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
                    break;
                }
                default:
                    OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch (message.option.id())
            {
                case EV_TIMER_VM_MONI:
                {
                    //��������״̬�����ϱ�״̬
                    MonitorAllInstanceState();
                    StartMoniterTimer();
                    break;
                }

                default:
                {
                    OutPut(SYS_NOTICE, "unknown message %d !\n", message.option.id());
                    break;
                }
            }
            break;
        }

        default:
            break;
    }
}

/****************************************************************************************
* �������ƣ�StartMoniterTimer
* ����������������ض�ʱ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/12    V1.0    lixiaocheng  ����
****************************************************************************************/
void InstanceManager::StartMoniterTimer()
{
    TimeOut timeout;
    timeout.type     = RELATIVE_TIMER;
    timeout.duration = k_vm_status_moni_timer;
    timeout.msgid    = EV_TIMER_VM_MONI;
    STATUS rc = _mu->SetTimer(_vm_moni_tid, timeout);
    if (SUCCESS != rc)
    {
        OutPut(SYS_ALERT, "InstanceManager StartMoniterTimer failed rc = %d!\n", 
               rc);
        SkyAssert(false);
    }
}

/****************************************************************************************
* �������ƣ�SendMoniterDate
* �������������ͼ�صĽ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/12    V1.0    lixiaocheng  ����
****************************************************************************************/
void InstanceManager::SendMoniterDate(MessageVmInfoReport &info)
{
    MID receiver;
    STATUS ret = ERROR;
    ret = GetRegisteredSystem(receiver._application);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ALERT, "MonitorAllInstanceState: GetRegisteredSystem failed! ret=%d\n",ret);
    }
    
    receiver._process = PROC_CC;
    receiver._unit = MU_VM_LIFECYCLE_MANAGER;
    MessageOption option(receiver, EV_VM_INFO_REPORT, 0, 0);
    
    STATUS sdresult=ERROR;
    sdresult=_mu->Send(info, option);
    if (SUCCESS != sdresult)
    {
        OutPut(SYS_ALERT, "SendMoniterDate: send to cc failed! ret=%d\n",sdresult);
    }
}

/****************************************************************************************
* �������ƣ�MonitorAllInstanceState
* ����������ʵ��״̬���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/12    V1.0    lixiaocheng  ����
****************************************************************************************/
STATUS InstanceManager::MonitorAllInstanceState(void)
{
    vector<VmInstance*> *instance_pool;
    MessageVmInfoReport report;
    bool   exist_vm_op_running = false;

    instance_pool = _instance_pool_ctrl->GetInstancePool();
    if (NULL == instance_pool)
    {
        return SUCCESS;
    }
    
    // ��ȡ������ص���
    _instance_pool_ctrl->LockInstance_pool();

    // ��ȡ��Ӧ�������ʵ��
    vector<VmInstance*>::iterator it;
    for (it = instance_pool->begin(); it != instance_pool->end(); it++)
    {
        VmInfo   tempinfo;
        //��ȡ��ǰҪ�ϱ�����Ϣ,�����ȡʧ�ܣ����ϱ�
        // ʧ�ܵ�ԭ���ǣ���ʵ����������ݻ�û��ˢ�µ�
        if (!(*it)->GetInstanceReportInfo(tempinfo))
        {
            _instance_pool_ctrl->UnlockInstance_pool();
            return ERROR;
        }

        // �������������ڲ�����,������ǣ����浱��������������ڲ���
        // ������������Ƿ�����Լ���ɨ�裬��ֹ��ϵͳ��æ��ʱ������
        // ��������xen�ӿڣ�����������������
        if ((*it)->IsOpRunning())
        {            
            OutPut(SYS_DEBUG, "MonitorAllInstanceState vm:%s, _op_ack_end:%d, _op_result:%d.\n", 
                    (*it)->GetInstanceName().c_str(), (*it)->GetOpAckEnd(), (*it)->GetOpResult());
            exist_vm_op_running = true;
        }
        // ��Ҫ�ϱ�����Ϣ�ŵ�vector��ȥ
        report._vm_info.push_back(tempinfo);
    }
    
    _instance_pool_ctrl->UnlockInstance_pool();

    // ������������ڲ����������ж���������ɨ��
    if (exist_vm_op_running)
    {
        SendMoniterDate(report);
        return SUCCESS;
    }
    
    // Ȼ�����libvirt�ӿڻ�ȡ��ǰ����������������
    int domid[MAX_DOM_NUM];
    uint32 i;
    int dom_num = _vm_driver->ListDomains(domid);
    if(dom_num < 0)
    {
        OutPut(SYS_WARNING, "failed to list domains.\n");
    }
    else
    {
        for (i = 0; i < static_cast<uint32>(dom_num); i++)
        {
            VmInfo   tempinfo;
            // ���domid=0��ʾ��ǰ��DOMAIN0������
            if (0 == domid[i])
            {
                continue;
            }

            // ���domin���Ѵ���ʵ���������������һ��,���ٶ�vm_driver�ĵ���
            // ���ID���Ҳ�����������ID�仯�ˣ���ô�ٵ�����������ƽӿڣ�����ѯ
            // ������Ӧ������������Ƿ���ʵ��������
            if (NULL != _instance_pool_ctrl->GetInstanceByDomID(domid[i]))
            {
                continue;
            }

            string dom_name = _vm_driver->GetDomainNameByDomid(domid[i]);
            // ������ҳ���������������ǿգ��ܴ�������������������
            if (dom_name.empty())
            {
                continue;
            }
        
            // ���domin���Ѵ���ʵ���������������һ��
            if (NULL != _instance_pool_ctrl->GetInstanceByDomName(dom_name))
            {
                continue;
            }

            // ������ҵ��������������Domain-Unnamed�ģ���ǿ�ƹػ�
            if (0 == dom_name.compare(0,strlen("Domain-Unnamed"), "Domain-Unnamed"))
            {
                _vm_driver->DestroyDomain(dom_name);
                OutPut(SYS_WARNING, "find the vm(%s) does not owener tecs, will destroy\n",dom_name.c_str());
                continue;
            }        
    
            // ������ҵ�����������Ʋ���instances_x���͵ģ���ֱ������HC
            if (0 != dom_name.compare(0,strlen("instance_"), "instance_"))
            {
                // ���ַǷ����������HCֱ���˳���������
                SkyExit(-1, "MonitorAllInstanceState: find a domin without create by Tecs");
                SkyAssert(false);
                continue;
            }

            // �Ѳ�������ǰϵͳ�����������ϱ���CC
            from_string(tempinfo._vm_id, dom_name.substr(strlen("instance_")), dec);
            tempinfo._vm_dom_id = domid[i];
            tempinfo._vm_name = dom_name;
            
            OutPut(SYS_DEBUG, "Caution! % need disappear but exist.\n", dom_name.c_str());
    
            // �����õ���Ϣѹ����Ϣ��ȥ
            report._vm_info.push_back(tempinfo);
        }
    }
    SendMoniterDate(report);

    return SUCCESS;
}



