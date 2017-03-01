#include "host_ha.h"
#include "event.h"
#include "mid.h"
#include "msg_host_manager_with_high_available.h"
#include "vm_messages.h"
#include "vm_pool.h"
#include "host_pool.h"

namespace zte_tecs
{
static int ha_print_on = 0;
DEFINE_DEBUG_VAR(ha_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(ha_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

static int ha_enabled = 1; //是否启用HA机制
DEFINE_DEBUG_VAR(ha_enabled);

HostHA *HostHA::_instance = NULL;
HostHA::HostHA()
{
    _mu = NULL;
}

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS HostHA::StartMu(const string& name)
{
    if (_mu)
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

void HostHA::RecordFaultVm (HostHaRec *pRec)
{
    _vm_process_list.push_back(pRec);
}

void HostHA::PrintAllFaultVm ()
{
    list<HostHaRec*> ::iterator iter;
    cout <<"==========HOST HA Information==========="<< endl;
    for (iter=_vm_process_list.begin();iter!=_vm_process_list.end();iter++)
    {
        cout<<"  host name= "<<(*iter)->_host_name<<"  host id=  "<<(*iter)->_host_id;
        cout<<"  vm name= "<<(*iter)->_vm_name<<"  vm id= "<<(*iter)->_vm_id;
        cout<<"  port name= "<<(*iter)->_port_name<<"  type=  "<<(*iter)->_type<<endl;
    }
}

void HostHA::PolicyToOperation (const string &policy,int &operation)
{
    operation=UNKNOWN_OP;
    if (policy==POLICY_MIGRATE)
        operation=RESCHEDULE;
    else if (policy==POLICY_STOP)
        operation=STOP;
    else if (policy==POLICY_REBOOT)
        operation=REBOOT;
    else if (policy==POLICY_PAUSE)
        operation=PAUSE;
}

void HostHA::HostFaultProcess(const MessageFrame &message)
{
    MessageHostFault req;
    HostHaRec *pRec= new HostHaRec;
    req.deserialize(message.data);
    pRec->_host_name=req._host_name;
    pRec->_host_id=req._host_id;
    pRec->_type=EV_HOST_FAULT;
    RecordFaultVm(pRec);
    Debug("HostFaultProcess:hostname %s host id %d\n",req._host_name.c_str(),req._host_id);
    VirtualMachinePool *pool = VirtualMachinePool::GetInstance();
    if (!pool)
    {
        cout<<"HostFaultProcess:get VirtualMachinePool err"<<endl;
        return ;
    }
    VmHAPolicy policy;
    vector<int64> vids;
    int  vmope;
    pool->GetVmsByHost(vids,req._host_id,100);
    vector<int64>::iterator it;
    for (it = vids.begin(); ((vids.size()!= 0)&&(it!= vids.end())); ++it)
    {
        pool->GetHAPolicy(*it,policy);
        PolicyToOperation(policy.host_down,vmope);
        if (vmope<0)
            vmope=RESCHEDULE;

        SendVmOp(*it,vmope);
    }
}

void HostHA::HostEnetFaultProcess(const MessageFrame &message)
{
#if 0    
    // added by vnet
    MessageHostEnetError req;

    MID receiver(VNET_MANAGER_PROCESS, MU_VNET_MANAGER); //是否在同一个application？？？
    CNetPlaneMsg cReqMsg;
    int      ret = 0;
    HostHaRec *pRec= new HostHaRec;
    req.deserialize(message.data);
    pRec->_host_name=req._host_name;
    pRec->_host_id=req._host_id;
    pRec->_port_name=req._port_name;
    pRec->_type=EV_HOST_ENET_ERROR;
    RecordFaultVm(pRec);
    Debug("HostEnetFaultProcess:hostname %s host id %d\n",req._host_name.c_str(),req._host_id);
    CVNetManager *pVNetHandle = CVNetManager::GetInstance();
    if (pVNetHandle)
    {
        CNetPlaneData data;
        int           found=0;
        HostPool *host_pool = HostPool::GetInstance();
        if (!host_pool)
        {
            cout<<"HostEnetFaultProcess: get HostPool instence err"<<endl;
            return;
        }
        data.m_strPhyNicName = req._port_name;
        //lvec deleted
#if 0
        ret=pVNetHandle->GetNetPlaneData(req._host_name, &data);
#endif
        if (ret!=0)
        {
            cout<<"HostEnetFaultProcess: get GetNetPlaneData err="<<ret<<endl;
            return;
        }
#if 0
        vector<CNetPlaneData> net;
        host_pool->GetNetPlane(req._host_id, net);

        vector<CNetPlaneData>::iterator it;
        for (it = net.begin(); it != net.end(); ++it)
        {
            if (it->m_strNetPlane == data.m_strNetPlane)
            {
                found=1;
                break;
            }
        }
#endif
        if (0==found)
        {
            VirtualMachinePool *pool = VirtualMachinePool::GetInstance();
            int                 vmope;
            if (!pool)
            {
                cout<<"HostEnetFaultProcess: get VirtualMachinePool instence err"<<endl;
                return ;
            }
            VmHAPolicy policy;
            vector<int64> vids;
            pool->GetVmsByHost(vids,pRec->_host_id,100);
            vector<int64>::iterator it;
            for (it = vids.begin(); ((vids.size()!= 0)&&(it!= vids.end())); ++it)
            {
                pool->GetHAPolicy(*it,policy);
                PolicyToOperation(policy.host_eth_down,vmope);
                if (vmope==UNKNOWN_OP)
                    vmope=RESCHEDULE;
                SendVmOp(*it,vmope);
            }
        }
    }
    else
    {
        cout<<"HostEnetFaultProcess:get CVNetManager instence err"<<endl;
    }
#endif
}

void HostHA::HostStorageFaultProcess(const MessageFrame &message)
{
    MessageHostStorageFault req;
    HostHaRec *pRec= new HostHaRec;
    req.deserialize(message.data);
    pRec->_host_name=req._host_name;
    pRec->_host_id=req._host_id;
    pRec->_type=EV_HOST_STORAGE_ERROR;
    RecordFaultVm(pRec);
    Debug("HostStorageFaultProcess:hostname %s host id %d\n",req._host_name.c_str(),req._host_id);
    VirtualMachinePool *pool = VirtualMachinePool::GetInstance();
    if (!pool)
    {
        cout<<"HostStorageFaultProcess:get VirtualMachinePool err"<<endl;
        return ;
    }
    VmHAPolicy policy;
    vector<int64> vids;
    int    vmope;
    pool->GetVmsByHost(vids,req._host_id,100);
    vector<int64>::iterator it;
    for (it = vids.begin(); ((vids.size()!= 0)&&(it!= vids.end())); ++it)
    {
        pool->GetHAPolicy(*it,policy);
        PolicyToOperation(policy.host_storage_fault,vmope);
        if (vmope==UNKNOWN_OP)
            vmope=RESCHEDULE;
        SendVmOp(*it,vmope);
    }
}

void HostHA::HostVmFaultProcess(const MessageFrame &message)
{
    MessageVmFault req;
    int  vmope;
    HostHaRec *pRec= new HostHaRec;
    req.deserialize(message.data);
    pRec->_host_name=req._host_name;
    pRec->_host_id=req._host_id;
    pRec->_vm_name=req._vm_name;
    pRec->_vm_id=req._vm_id;
    pRec->_type=EV_HOST_VM_FAULT;
    RecordFaultVm(pRec);
    Debug("HostVmFaultProcess:hostname %s host id %d vm_name %s\n",req._host_name.c_str(),req._host_id,req._vm_name.c_str());

    if (req._exist == false)
    {
        //虚拟机已经不存在于物理机上了，需要重新调度
        vmope=RESCHEDULE;
    }
    else
    {
        VirtualMachinePool *pool = VirtualMachinePool::GetInstance();
        if (!pool)
        {
            cout<<"HostVmFaultProcess:get VirtualMachinePool err"<<endl;
            return ;
        }
        VmHAPolicy policy;
        pool->GetHAPolicy(req._vm_id,policy);
        PolicyToOperation(policy.vm_crash,vmope);
        if (vmope==UNKNOWN_OP)
            vmope=REBOOT;
    }
    SendVmOp(req._vm_id,vmope);
}

void HostHA::HostCpuBusyProcess(const MessageFrame &message)
{
    MessageHostCpuBusy req;
    HostHaRec *pRec= new HostHaRec;
    req.deserialize(message.data);
    pRec->_host_name=req._host_name;
    pRec->_host_id=req._host_id;
    pRec->_type=EV_HOST_CPU_BYSY;
    RecordFaultVm(pRec);
    Debug("HostCpuBusyProcess:hostname %s host id %d\n",req._host_name.c_str(),req._host_id);
    VirtualMachinePool *pool = VirtualMachinePool::GetInstance();
    if (!pool)
    {
        cout<<"HostCpuBusyProcess:get VirtualMachinePool err"<<endl;
        return ;
    }
    VmHAPolicy policy;
    vector<int64> vids;
    int  vmope;
    pool->GetVmsByHost(vids,req._host_id,100);
    vector<int64>::iterator it;
    for (it = vids.begin(); ((vids.size()!= 0)&&(it!= vids.end())); ++it)
    {/*目前直接返回找到的第一个*/
        pool->GetHAPolicy(*it,policy);
        vmope=RESCHEDULE;
        SendVmOp(*it,vmope);
        break;
    }
}

STATUS HostHA::SendVmOp(int64 vid,int operation)
{
    if (ha_enabled != 0)
    {
        VmOperationReq req(vid,operation);
        MessageOption  option(MU_VM_LIFECYCLE_MANAGER, EV_VM_OP, 0, 0);
        OutPut(SYS_NOTICE,"vm %lld need HA operation %s!\n",vid,Enum2Str::VmOpStr(operation));
        return _mu->Send(req, option);
    }
    else
    {
        OutPut(SYS_WARNING,"vm %lld HA operation %s is ignored!\n",vid,Enum2Str::VmOpStr(operation));
        return SUCCESS;
    }
}

void  HostHA::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
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
        case EV_HOST_FAULT: //单板晶振故障、内存故障、离线；消息内容是物理机ｎａｍｅ
        {
            HostFaultProcess(message);
            break;
        }

        case EV_HOST_ENET_ERROR://物理机网口故障：物理机名称、物理网口索引
        {
            HostEnetFaultProcess(message);
            break;
        }

        case EV_HOST_STORAGE_ERROR://物理机本地存储故障：物理机名称
        {
            HostStorageFaultProcess(message);
            break;
        }

        case EV_HOST_VM_FAULT://虚拟机故障
        {
            HostVmFaultProcess(message);
            break;
        }

        case EV_HOST_CPU_BYSY:
        {
            HostCpuBusyProcess(message);
            break;
        }

        default:
            cout << "HostHA receives an unknown message!" << endl;
            break;
        }
        break;
    }

    default:
        break;
    }
};

void DbgVmFault()
{
    MessageVmFault req;
    MessageUnit tempmu(TempUnitName());
    req._host_id=100;
    req._host_name="name_host";
    req._vm_id=1;
    req._vm_name="name_vm";
    MessageOption  option(MU_HOST_HIGHAVAILABLE, EV_HOST_VM_FAULT, 0, 0);
    tempmu.Send(req, option);
}

void DbgPrintAllFaultVm()
{
    HostHA* pHa = HostHA::GetInstance();
    pHa->PrintAllFaultVm();
}

DEFINE_DEBUG_FUNC(DbgVmFault);
DEFINE_DEBUG_FUNC(DbgPrintAllFaultVm);
}// namespace zte_tecs




