/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_agent.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostAgent��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#include "host_agent.h"
#include "event.h"
#include "storage.h"
#include "tecs_config.h"
#include "registered_system.h"
#include "log_agent.h"
#include "host_storage.h"
#include "iscsi.h"
#include "pci_passthrought.h"
#include "hypervisor.h"
#include "dns_building.h"
#include "ntp_building.h"

string hostxml_full_path = "/var/lib/tecs/host.xml";
int32 statistics_interval = 5 * 60; // 300 seconds
string location;
string shelf_type;
string media_address;
STATUS EnableHostAgentOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("location", location, "Host location.");
    config->EnableCustom("shelf_type", shelf_type, "Type of the shelf where the host put on.");
    config->EnableCustom("media_address", media_address, "Local media ip address.");
    config->EnableCustom("statistics_interval", statistics_interval,
                         "Time interval between two statistics. 0 disable statistics! "
                         "Unit: second, default = 5 * 60 seconds.");
    return SUCCESS;
}
DEFINE_DEBUG_VAR(statistics_interval);
void DbgShowStatInterval()
{
    cout << "interval: " << statistics_interval << " s" << endl;
}
DEFINE_DEBUG_FUNC_ALIAS(statinterval, DbgShowStatInterval);
namespace zte_tecs
{

const int32 report_time_interval = 5; // 5 second

// ��ʱ����Ϣ��
#define TIMER_REGISTERED_SYSTEM     EV_TIMER_1  // �����鲥������cluster����
#define TIMER_HOST_DISCOVER         EV_TIMER_2  // ���ڷ�������������Ϣ
#define TIMER_HOST_INFO_REPORT      EV_TIMER_3  // ���ڲɼ���Ϣ���ϱ���CC

#define UNKNOWN_HYPERVISOR ((int32)0)
#define XEN_HYPERVISOR     ((int32)1)
#define KVM_HYPERVISOR     ((int32)2)

HostAgent *HostAgent::_instance = NULL;

/******************************************************************************/
HostAgent::HostAgent()
{
    _mu = NULL;
    _timer_id = INVALID_TIMER_ID;
    _hypervisor_type = UNKNOWN_HYPERVISOR;
    _report_count = 0;
    _board_type = 0;
    _pre_statistics_time = time(0);
}

/******************************************************************************/
STATUS HostAgent::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
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

/******************************************************************************/
void HostAgent::MessageEntry(const MessageFrame &message)
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
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch (message.option.id())
            {
                case EV_PUBLISH_SSH_PUBKEY:
                {
                    DoJoinFriendKey(message.data);
                    break;
                }
                case TIMER_HOST_DISCOVER:
                {
                    DoDiscover();
                    break;
                }
                case TIMER_HOST_INFO_REPORT:
                {
                    DoReport();
                    break;
                }
                case EV_HOST_REGISTER_REQ:
                {
                    DoRegister(message);
                    break;
                }
                case EV_HOST_UNREGISTER_REQ:
                {
                    DoUnregister(message);
                    break;
                }
                case TIMER_REGISTERED_SYSTEM:
                {
                    DoRegisteredSystem();
                    break;
                }
                case EV_REGISTERED_SYSTEM_QUERY:
                {
                    DoRegisteredSystem(message);
                    break;
                }
                case EV_REGISTERED_SYSTEM:
                {
                    break;
                }        
            
            
                default:
                {
                    cout << "HostAgent::MessageEntry: receive an unkown message!"<< endl;
                    OutPut(SYS_NOTICE, "Unknown message: %d.\n", message.option.id());
                    break;
                }
            }
            break;
        }

        default:
            break;
    }
}

/******************************************************************************/
void HostAgent::DoStartUp()
{
    // ���������Ϣ�鲥��
    STATUS ret = _mu->JoinMcGroup(string(ApplicationName()) + 
                                  MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("HostAgent failed to join group! ret = ");
        lastwords.append(to_string(ret, dec));
        SkyExit(-1,lastwords);
    }

    // ���ù�����Ⱥ��Ϣ�����鲥��ʱ��
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 50 * 1000;
    timeout.msgid = TIMER_REGISTERED_SYSTEM;
    if (SUCCESS != _mu->SetTimer(_mu->CreateTimer(), timeout))
    {
        OutPut(SYS_EMERGENCY, "Create timer for send registered cluster!\n");
        SkyAssert(0);
        SkyExit(-1, "HostAgent::DoStartUp: call _mu->SetTimer failed.");
    }

    // �����������ֻ���Ϣ�ϱ����õĶ�ʱ��
    if (INVALID_TIMER_ID == (_timer_id = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, 
               "Create timer failed for send host discover or report!\n");
        SkyAssert(0);
        SkyExit(-1, "HostAgent::DoStartUp: call _mu->SetTimer failed.");
    }

    // ��HC����������Ҳ��֪������ȺΪ��
    DoRegisteredSystem();

    //����kcs��ȡ�ӿ���Ϣʧ����ô����xml�ļ��ܲ��ܷ���д��
    //kcs����ʧ�ܵ�ԭ���п����ǿ��и������ӵ�����˳���Ȳ������쳣�쳣�����
    //�����쳣�������Ϊ��Ԥ�£���������


    //��æ����һ��PCIPthManager�ĳ�ʼ��
    PCIPthManager *pth_manager = PCIPthManager::GetInstance();
    if (pth_manager)
    {
        pth_manager->Startup();
    }

    // ������������
    GoToDiscover();
}

/******************************************************************************/
STATUS HostAgent::DoJoinFriendKey(const string &key)
{
    STATUS ret;

    vector<string>::iterator it = find(_ssh_keys.begin(),_ssh_keys.end(),key);
    if(it != _ssh_keys.end())
    {
        //OutPut(SYS_DEBUG, "key exist:%s!\n",key.c_str());
        return SUCCESS;
    }
    
    ret = SshTrust(key);
    if (SUCCESS == ret)
    {
        _ssh_keys.push_back(key);
        OutPut(SYS_INFORMATIONAL, "key join:%s!\n",key.c_str());
        return ret;
    }

    OutPut(SYS_EMERGENCY, "join ssh key fail %u!\n",ret);
    return ret;
}

/******************************************************************************/
void HostAgent::DoRegister(const MessageFrame &message)
{
    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());
    SkyAssert(sender._process == PROC_CC);
    SkyAssert(sender._unit == MU_HOST_MANAGER);

    MessageHostRegisterReq req;
    req.deserialize(message.data);

    // ����Ϣ���ݽ��кϷ��Լ��
    if (req._host_name != string(ApplicationName()))
    {
        OutPut(SYS_ALERT, "DoRegister: host name is err, %s != %s!\n",
               req._host_name.c_str(), ApplicationName().c_str());
        SkyAssert(req._host_name == string(ApplicationName()));
        return;
    }

    // ���û�й�����������Ϊ���ͷ�
    if (_cluster_name.empty())
    {
        SetCluster(sender._application);
        GoToReport();
    }

    string ip_connected;
    map<string, string>::iterator it;
    for (it  = req._cc_ifs_info.begin();
         it != req._cc_ifs_info.end();
         it++)
    {
        if (it->second.empty())
            continue;
        ip_connected.clear();
        if (SUCCESS != GetIpByConnect(it->second, ip_connected))
            continue;
        if (_hc_ip_address == ip_connected)
        {
            _cc_ip_address = it->second;
            break;
        }
    }
    if (!_cc_ip_address.empty())
    {
        SetNtpClientConfig("register", _cc_ip_address, true);
        SetDnsServerConfig("register", _cc_ip_address, DNS_IA_IMAGE_TECS);
        SetDnsClientConfig("register", _cc_ip_address, ApplicationName());
    }
}

/******************************************************************************/
void HostAgent::DoUnregister(const MessageFrame &message)
{
    MID sender = message.option.sender();

    SkyAssert(!sender._application.empty());
    SkyAssert(sender._process == PROC_CC);
    SkyAssert(sender._unit == MU_HOST_MANAGER);

    MessageHostUnregisterReq req;
    req.deserialize(message.data);

    // ����Ϣ���ݽ��кϷ��Լ��
    if (req._host_name != string(ApplicationName()))
    {
        OutPut(SYS_ALERT, "DoUnregister: host name is err, %s != %s!\n",
               req._host_name.c_str(), ApplicationName().c_str());
        SkyAssert(req._host_name == string(ApplicationName()));
        return;
    }

    // ������ǹ����ļ�Ⱥ����������Ϣ������˵�
    if (_cluster_name != sender._application)
    {
        return;
    }

    SetCluster("");

    if (!_cc_ip_address.empty())
    {
        SetNtpClientConfig("unregister", _cc_ip_address, true);
        SetDnsServerConfig("unregister", _cc_ip_address, DNS_IA_IMAGE_TECS);
        SetDnsClientConfig("unregister", _cc_ip_address, ApplicationName());
    }
    
    // HC�����˳������·����Ը���ϵͳ�Ĺ�����Ϣ
    SkyExit(-1, "Host is unregistered from the cluster, exit!\n");
}

/******************************************************************************/
void HostAgent::DoDiscover()
{
    MessageHostBaseInfo msg;
    GetBaseInfoFromScript(msg);

    //����ڴ�Ϊ�գ��򷵻ؼ����ȴ�
    if(0 == msg._mem_max || 0 == msg._mem_total)
    {
        OutPut(SYS_ALERT, "DoDiscover: get memroy is 0!\n");
        return;
    }

    msg._cpus = _max_cpus;

    if(_max_cpus != _cpu_cores)
    {
        /* dom0 �� ��A �����, ubench ������ ��A �ļ�������,
         * ����ͬ�˵ļ���������ͬ, ����������û�����
         */
        msg._cpu_info._info["cpu_bench"]="0";        
    }
	
	// ������������ʱ��
    GetHostRunningTime(msg._running_time);
	
    // ����Ӳ����Ϣ
    GetDiskInfoForDiscover(msg._disk_total,msg._disk_max);

    // �������λ����Ϣ, ��tecs.conf�н�����ȡ
    GetBaseLocationInfo(msg._location);

    // ����ּܿ����Ϣ
    GetExtendLocationInfo(msg._phy_position);

    GetIscsiName(msg._iscsiname);  

    // ��������豸��Ϣ
    ConstSdeviceInfo(msg._sdevice_info);
    
    // ���ӻ���������Ϣ
    msg._shelf_type = shelf_type;

    // ���ӵ�������bomid��Ϣ
    int32 board_type = 0;
    if(SUCCESS == GetBoardTypeFromKcs(board_type))
    {
        msg._board_type = board_type;
    }
    string mask;
    if (0 != GetKernelPortInfo("dvs_kernel", _hc_ip_address, mask))
    {
        OutPut(SYS_ALERT, "Get hc ip address info failed!\n");
        _hc_ip_address.clear();
    }
    msg._ip_address = _hc_ip_address;
    //����ý�����ַ
    msg._media_address = media_address;
    
    // �����������鲥�鷢������������Ϣ
    MID receiver("group", MUTIGROUP_HOST_DISCOVER);
    MessageOption option(receiver, EV_HOST_DISCOVER, 0, 0);
    int32 ret = _mu->Send(msg, option);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ALERT, "DoDiscover: Send discover failed!\n");
        SkyAssert(SUCCESS == ret);
    }
}

/******************************************************************************/
void HostAgent::DoReport()
{
    MessageHostInfoReport msg;
    
    // ����Ӳ����Ϣ
    GetDiskInfoForReport(msg._vec_disk_info);

    //����ּܿ����Ϣ
    GetExtendLocationInfo(msg._phy_position);

    //���侧��״̬
    if (GetCrystalStatus())
    {
        msg._oscillator = 0; //0Ϊ�ã�1Ϊ��
    }
    else
    {
        msg._oscillator = 1;
    }

    msg._board_type = _board_type;

    // Ӳ����������Ϣ
    GetHostHwMonInfo(msg._vec_host_hwmon);

    // �����������Դʹ��ͳ����Ϣ
    GetHostStatistics(msg._host_statistics);

		// ������������ʱ��
    GetHostRunningTime(msg._running_time);
    
    // ���������IP��ַ
    string mask;
    if (0 != GetKernelPortInfo("dvs_kernel", msg._ip_address, mask))
    {
        OutPut(SYS_ALERT, "Get hc ip address info failed!\n");
        msg._ip_address.clear();
    }    
	
    _report_count++;

    // ������Ϣ
    MID receiver;
    receiver._application = _cluster_name;
    receiver._process = PROC_CC;
    receiver._unit = MU_HOST_MANAGER;
    MessageOption option(receiver, EV_HOST_INFO_REPORT, 0, 0);
    _mu->Send(msg, option);
}

/******************************************************************************/
void HostAgent::DoRegisteredSystem()
{
    MessageRegisteredSystem msg(_cluster_name);
    MID receiver("group", string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    MessageOption option(receiver, EV_REGISTERED_SYSTEM, 0, 0);
    _mu->Send(msg, option);
}


/******************************************************************************/
void HostAgent::DoRegisteredSystem(const MessageFrame &message)
{
    MessageRegisteredSystem msg(_cluster_name);
    MessageOption option(message.option.sender(), EV_REGISTERED_SYSTEM, 0, 0);
    _mu->Send(msg, option);
}

/******************************************************************************/
void HostAgent::SetHypervisorType()
{
    if (IsXenKernel())
        _hypervisor_type = XEN_HYPERVISOR;
    else if (IsKvmKernel())
        _hypervisor_type = KVM_HYPERVISOR;
    else
        _hypervisor_type = UNKNOWN_HYPERVISOR;
}

/******************************************************************************/
void HostAgent::RunCmdAndTrim(string& cmd, string& result)
{
    result.clear();
    RunCmd(cmd, result);
    boost::trim(result);
}

/******************************************************************************/
bool HostAgent::GetCrystalStatus()
{
// ���ڶ���V4, �Ǳ���, ������, ���ϱ�OK
#if 0
    // �ж� /proc/zte/crystal �ļ��Ƿ�ɶ�
    if (0 != access("/proc/zte/crystal", R_OK))
    {
        return true;
    }

    ifstream fin("/proc/zte/crystal");
    string str;
    while(fin >> str)
    {
        // ���ڶ������, һ�ж�Ӧһ��, ֻҪһ����, �������
        if(str == "ERROR")
        {
            return false;
        }
    }
#endif
    return true;
}

/******************************************************************************/
void HostAgent::GetBoardKCSInfo(vector<BoardKCSInfo> &infos)
{
    if (0 == access("/usr/local/bin/kcscmd", X_OK))
    {
        string cmd = "/usr/local/bin/kcscmd -i 2>/dev/null |grep -o \"[[:digit:]]*\"";
        string res;
        RunCmdAndTrim(cmd,res);
        stringstream iss(res);  
        BoardKCSInfo kcs;
        while (getline(iss, kcs._slot))
        {
            getline(iss, kcs._id);
            getline(iss, kcs._pcb);
            getline(iss, kcs._bom);

            infos.push_back(kcs);
        }
    }
}

/******************************************************************************/
int32 HostAgent::GetBoardTypeFromKcs(int32  &borad_type)
{// ��ȡĸ�� slot:0 �� board_type
    if (0 != access("/usr/local/bin/kcscmd", X_OK))
    {
        return ERROR;
    }
    /* ��������ӡ:
     * Slot:1, Id:65535, Pcb:255, Bom:255
     */
    string cmd = "/usr/local/bin/kcscmd -i 2>/dev/null "
                 "|grep Slot:0 "
                 "| awk -F ':' '{print $3}' "
                 "| awk -F ',' '{print $1}'";
    string res;
    int ret = RunCmd(cmd, res);
    if ((SUCCESS != ret) || res.empty())
    {
        return ERROR;
    }
    borad_type = atoi(res.c_str());

    return SUCCESS;
}

/* �������
 * 1) ���� CPU : �����Ͽ����������װ�Ĵ�����
 * 2) CPU ���� : ÿ������ CPU �ϴ������ݵ�оƬ��
 * 3) �߼� CPU : ���� 1 �� 2 �ĳ˻�, �ɿ����������������еĺ���, ����֮��
 */
/******************************************************************************/
int HostAgent::GetCpuCoreNumForXen(void)
{// ��Ϊ��ȡ�߼� CPU
    string result, cmd;
    cmd = "xl info | grep nr_cpus | awk -F': ' '{print $2}'";
    if (SUCCESS != RunCmd(cmd, result))
        return 0;
    else
        return atoi(result.c_str());
}

/******************************************************************************/
int HostAgent::GetCpuCoreNumForKvm(void)
{// ��Ϊ��ȡ�߼� CPU
    string result, cmd;
    cmd = "cat /proc/cpuinfo | grep processor | sort | uniq | wc -l";
    if (SUCCESS != RunCmd(cmd, result))
        return 0;
    else
        return atoi(result.c_str());
}

/******************************************************************************/
int HostAgent::GetDom0MaxVcpus(void)
{
    string result, cmd;
    cmd = " xl info | grep xen_commandline "
          "| awk -F'dom0_max_vcpus=' '{print $2}' "
          "| awk -F' ' '{print $1}' ";
    if(SUCCESS != RunCmd(cmd, result))
    {
       return 0;
    }
    return atoi(result.c_str());
}

/******************************************************************************/
int HostAgent::GetMaxCpusForXenVm(void)
{
    int32 cpus_for_vm = 0;
    int32 cpus_total = GetCpuCoreNumForXen();
    int32 cpus_for_dom0 = GetDom0MaxVcpus();
    if (cpus_total >= cpus_for_dom0)
    {
        /* dom0 �Ƿ��? 
         * 1) ����, �����Ƿ��һ�� cpus_for_dom0 == cpus_total?
         * 2) ��, cpus_total - cpus_for_dom0
         */
        cpus_for_vm = (cpus_for_dom0 == cpus_total) ?
                      cpus_total :
                      (cpus_total - cpus_for_dom0);
    }
    return cpus_for_vm;
}

/******************************************************************************/
int HostAgent::GetMaxCpusForKvmVm(void)
{
    return GetCpuCoreNumForKvm();
}

/******************************************************************************/
string HostAgent::GetOsInfo(void)
{
    string cmd = "uname -srvmpio";
    string res;
    RunCmdAndTrim(cmd, res);
    return res;
}

/******************************************************************************/
string HostAgent::GetVmmInfoForXen(void)
{
    if (0 != access("/sys/hypervisor/", X_OK))
        return "";
    string result, temp, cmd;
    cmd = "cat /sys/hypervisor/type";
    RunCmdAndTrim(cmd, temp);
    result += temp;

    cmd = "cat /sys/hypervisor/version/major";
    RunCmdAndTrim(cmd, temp);
    result += " " + temp;

    cmd = "cat /sys/hypervisor/version/minor";
    RunCmdAndTrim(cmd, temp);
    result += "." + temp;

    cmd = "cat /sys/hypervisor/version/extra";
    RunCmdAndTrim(cmd, temp);
    result += temp;
    return result;
}

/******************************************************************************/
string HostAgent::GetVmmInfoForKvm(void)
{
    return "kvm";
}

/******************************************************************************/
int64 HostAgent::GetTotalMemoryForXen(void)
{
    if (0 != access("/usr/sbin/xl", X_OK))
        return 0;
    string result, cmd;
    cmd = "xl info | grep total_memory | awk -F': ' '{print $2}'";
    RunCmdAndTrim(cmd, result);
    return (strtoul(result.c_str(),NULL,10) * 1024 * 1024); // MB to B
}

/******************************************************************************/
int64 HostAgent::GetTotalMemoryForKvm(void)
{
    string result, cmd;
    cmd = "cat /proc/meminfo | grep MemTotal | awk -F ' ' '{print $2}'";
    RunCmdAndTrim(cmd, result);
    return (strtoul(result.c_str(),NULL,10) * 1024); // KB to B
}

/******************************************************************************/
int64 HostAgent::GetDom0Memory(void)
{
    if (0 != access("/usr/sbin/xl", X_OK))
        return 0;
    string result, cmd;
    cmd = "xl list | grep 'Domain-0' | awk -F ' ' '{print $3}'";
    RunCmdAndTrim(cmd, result);
    return (strtoul(result.c_str(),NULL,10) * 1024 * 1024); // MB to B
}

/******************************************************************************/
int64 HostAgent::GetFreeMemoryForXen(void)
{
    int64 total_mem = GetTotalMemoryForXen();
    int64 dom0_mem = GetDom0Memory();
    /* �㷨:
     * temp = total-dom0-9-8*dom0/1024
     *  9: �������е������������ڴ�, �̶�
     *  8: Domain0 ÿG�������ĵ�ҳ���ڴ�
     * free = (temp-12*temp/1024-8*temp/1024)*1024*1024
     *  12: Domain0 ����ʣ���ڴ�ÿG����Ҫҳ���ڴ�ռ�
     *  8 : DomainU ʹ��ÿG�ڴ���Ҫ�Ķ���ҳ���ڴ�
     *  NOTE: �е���, 8 �ĸ����ͳһΪÿ������������Լ����ڴ�������ڴ�
     *        ���� 2 ���򻯺�Ϊ���µļ���
     */
    if(total_mem > dom0_mem)
        return ((total_mem - dom0_mem - 9*1024*1004 - dom0_mem*8/1024)*1004/1024);
    else
        return 0;
}

/******************************************************************************/
int64 HostAgent::GetFreeMemoryForKvm(void)
{
    int64 total_mem = GetTotalMemoryForKvm();
    if (total_mem > (1024*1024*1024))
        return (total_mem - 1024*1024*1024);
    else
        return 0;
}

/******************************************************************************/
string HostAgent::GetSshKey(void)
{
    string result, cmd;
    cmd = "cat ~/.ssh/id_dsa.pub";
    RunCmdAndTrim(cmd, result);
    return result;
}

/******************************************************************************/
bool HostAgent::GetIsSupportHvmForXen(void)
{
    string result, cmd;
    cmd = "xl info | grep xen_caps | grep hvm";
    RunCmdAndTrim(cmd, result);
    return (result.empty() ? false : true);
}

/******************************************************************************/
bool HostAgent::GetIsSupportHvmForKvm(void)
{
    string result, cmd;
    cmd = "cat /proc/cpuinfo | grep -E \"svm|vmx\"";
    RunCmdAndTrim(cmd, result);
    return (result.empty() ? false : true);
}

/******************************************************************************/
void HostAgent::GetCommonCpuInfo(CommonCpuInfo& common_cpuinfo)
{// �˲��ִ���ɹ̻�?
    string result, cmd;
    /* physical_id_num, cpu_cores ��ȡ���ɿ�
     *  1) ��� xen �� dom0 ���˺ˣ���ȡ���Ľ��� dom0 ��
     *  2) ��Щ��������֧���ṩ��Щ��Ϣ, ���� AMD
     * ps: ����Ҫ, ��ά��ԭ��
     */
    cmd = "cat /proc/cpuinfo | grep 'physical id' | sort | uniq | wc -l";
    RunCmdAndTrim(cmd, result);
    common_cpuinfo._physical_id_num = (!result.compare("0")) ? "1" : result;
    cmd = "cat /proc/cpuinfo | grep 'core id' | sort | uniq | wc -l";
    RunCmdAndTrim(cmd, result);
    common_cpuinfo._cpu_cores = (!result.compare("0")) ? "1" : result;
    cmd = "cat /proc/cpuinfo | grep -m 1 vendor_id | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._vendor_id);
    cmd = "cat /proc/cpuinfo | grep -m 1 family | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._cpu_family);
    cmd = "cat /proc/cpuinfo | grep -m 1 model | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._model);
    cmd = "cat /proc/cpuinfo | grep -m 1 'model name'| awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._model_name);
    cmd = "cat /proc/cpuinfo | grep -m 1 stepping | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._stepping);
    cmd = "cat /proc/cpuinfo | grep -m 1 cache "
          "| awk -F ': ' '{print $2}' | awk -F ' KB' '{print $1}'"; 
    RunCmdAndTrim(cmd, common_cpuinfo._cache_size);
    cmd = "cat /proc/cpuinfo | grep -m 1 fpu | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._fpu);
    cmd = "cat /proc/cpuinfo | grep -m 1 fpu_exception | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._fpu_exception);
    cmd = "cat /proc/cpuinfo | grep -m 1 'cpuid level' "
          "| awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._cpuid_level);
    cmd = "cat /proc/cpuinfo | grep -m 1 wp | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._wp);
    cmd = "cat /proc/cpuinfo | grep -m 1 flags | awk -F ': ' '{print $2}'";
    RunCmdAndTrim(cmd, common_cpuinfo._flags);
    cmd = "cat /opt/tecs/hc/etc/cpu_bench_result | grep CPU "
          "| awk -F ' ' '{print $3}' | sed -n '1p'";
    RunCmdAndTrim(cmd, result);
    common_cpuinfo._cpu_bench = (result.empty()) ? "0" : result;
}

/******************************************************************************/
void HostAgent::CreateHostResourceXml()
{// ���ļ���ʱ���ṩ�������ѯ�ӿ���Ϣʹ��
    string    filecontext("");
    filecontext = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";

    // ����xmlparse 
    XmlParser xml;
    xml.Create("content");
   
    xml.Write("host_name","");
    xml.Write("host_ip","");
    xml.Write("cluster_name","");

    xml.Write("cpu","");
    xml.Enter();

    xml.Write("vendor_id","");
    xml.Write("family","");
    xml.Write("model","");
    xml.Write("model_name","");
    xml.Write("cores","");

    xml.Exit();

    xml.Write("memory","");
    xml.Enter();

    xml.Write("size","");
    xml.Exit();

    xml.Write("network","");
    xml.Enter();
    xml.Write("dns","");
    xml.Write("route","");
    xml.Write("interfaces","");

    xml.Exit();

    //�ӿ�����������Ŀ¼
    xml.Write("blade_cards","");
    xml.Enter();

    //��ȡ���ӿ�����Ϣ����ѯд�뵽xml�ļ���
    vector<BoardKCSInfo> infos;
    GetBoardKCSInfo(infos);

    vector<BoardKCSInfo>::iterator  it;
    for (it = infos.begin(); it != infos.end(); it ++)
    {
        //��ѭ�����item�ı���ȴ������
        xml.Write("item","");
        xml.Enter();
        xml.Write("slot",it->_slot);
        xml.Write("pcb",it->_pcb);
        xml.Write("bom",it->_bom);
        xml.Write("id",it->_id);
    
        xml.Exit();
    }
    //�ӿ����ֽ���
    
    xml.Exit();

    xml.Write("special_devices","");
    xml.Write("location","");

    xml.Write("os","");
    xml.Enter();

    xml.Write("name","");
    xml.Write("version","");
    xml.Write("kernel_version","");

    xml.Exit();

    xml.Write("hypervisor","");
    xml.Enter();

    xml.Write("name","");
    xml.Write("version","");
    xml.Exit();
    
    /* �ȴ���Ŀ¼ */
    mkdir("/var/lib/tecs/", 0777);
    
    /* ��������ļ� */
    ofstream ofile(hostxml_full_path.c_str());

    if(ofile.fail() == true)
    {
        SkyAssert(0);
        SkyExit(-1, "HostAgent::DoStartUp: create host.xml failed.");
    }

    string s = filecontext + xml.DocToString();

    ofile << s << endl;

    ofile.close();
}

/******************************************************************************/
void HostAgent::ExitClusterGroup()
{
    STATUS ret = 0;
    // �ϵ��ʱ��Ϊ��, ��ʱ����Ҫ���в���
    if (_cluster_name.empty())
    {
        return;
    }
    ret = _mu->ExitMcGroup(_cluster_name + MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "HostAgent exit tecs group %s failed!\n",
            (_cluster_name+MUTIGROUP_SSH_TRUST).c_str());
    }
    OutPut(SYS_DEBUG, "HostAgent exit tecs group %s successfully!\n",
        (_cluster_name+MUTIGROUP_SSH_TRUST).c_str());
}

/******************************************************************************/
void HostAgent::JoinClusterGroup()
{
    STATUS ret = 0;
    // �ϵ��ʱ��Ϊ��, ��ʱ����Ҫ���в���
    if (_cluster_name.empty())
    {
        return;
    }
    ret = _mu->JoinMcGroup(_cluster_name + MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "HostAgent join cluster group %s failed!\n",
               (_cluster_name+MUTIGROUP_SSH_TRUST).c_str());
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "HostAgent join cluster group failed!.");
    }
    OutPut(SYS_INFORMATIONAL, "HostAgent join tecs group %s successfully!\n",
        (_cluster_name+MUTIGROUP_SSH_TRUST).c_str());
}

/******************************************************************************/
void HostAgent::GoToDiscover(void)
{
    // ���
    LightHost(LIGHT_NO_JOINED, true);

    // ������������ѭ����ʱ��
    _mu->StopTimer(_timer_id);
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;
    timeout.msgid = TIMER_HOST_DISCOVER;
    _mu->SetTimer(_timer_id, timeout);    

    // ����������������
    DoDiscover();
}

/******************************************************************************/
void HostAgent::GoToReport(void)
{
    // ���
    LightHost(LIGHT_NO_JOINED, false);

    // ����������Ϣ�ϱ�ѭ����ʱ��
    _mu->StopTimer(_timer_id);
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 5 * 1000;
    timeout.msgid = TIMER_HOST_INFO_REPORT;
    _mu->SetTimer(_timer_id, timeout);

    // ����������Ϣ�ϱ����̣���DoReport���޶�ʱ������
    DoReport();
}

/******************************************************************************/
void HostAgent::SetCluster(const string &cluster_name)
{
    if (_cluster_name != cluster_name)
    {
        //�������ļ�Ⱥ�仯ʱ����Ҫ�˳�ԭ���ļ�Ⱥ�������µļ�Ⱥ
        ExitClusterGroup();
        _cluster_name = cluster_name;
        JoinClusterGroup();
        OutPut(SYS_INFORMATIONAL, "Set regitered cluster %s.\n",
               _cluster_name.c_str());
    }

    DoRegisteredSystem();
}

/******************************************************************************/
void HostAgent::GetBaseInfoFromScript(MessageHostBaseInfo &message)
{
    ostringstream oss;
    // ���첿�ִ���
    switch (_hypervisor_type)
    {
        case XEN_HYPERVISOR:
        {
            _max_cpus = GetMaxCpusForXenVm();
            oss.str("");
            _cpu_cores = GetCpuCoreNumForXen();
            oss << _cpu_cores;
            message._cpu_info._info.insert(make_pair("processor_num", oss.str()));

            message._vmm_info  =  GetVmmInfoForXen();
            message._mem_total =  GetTotalMemoryForXen();
            message._mem_max   =  GetFreeMemoryForXen();
            message._is_support_hvm = GetIsSupportHvmForXen();
            break;
        }
        case KVM_HYPERVISOR:
        {
            _max_cpus = GetMaxCpusForKvmVm();
            oss.str("");
            _cpu_cores = GetCpuCoreNumForKvm();
            oss << _cpu_cores;
            message._cpu_info._info.insert(make_pair("processor_num", oss.str()));

            message._vmm_info  =  GetVmmInfoForKvm();
            message._mem_total =  GetTotalMemoryForKvm();
            message._mem_max   =  GetFreeMemoryForKvm();
            message._is_support_hvm = GetIsSupportHvmForKvm();
            break;
        }
        default:
            break;
    }

    // ���ò���
    message._ssh_key   =  GetSshKey();
    message._os_info   =  GetOsInfo();

    CommonCpuInfo common_cpuinfo;
    GetCommonCpuInfo(common_cpuinfo);
    message._cpu_info._info.insert(make_pair("physical_id_num", common_cpuinfo._physical_id_num));
    message._cpu_info._info.insert(make_pair("cpu_cores", common_cpuinfo._cpu_cores));
    message._cpu_info._info.insert(make_pair("vendor_id", common_cpuinfo._vendor_id));
    message._cpu_info._info.insert(make_pair("cpu_family", common_cpuinfo._cpu_family));
    message._cpu_info._info.insert(make_pair("model", common_cpuinfo._model));
    message._cpu_info._info.insert(make_pair("model_name", common_cpuinfo._model_name));
    message._cpu_info._info.insert(make_pair("stepping", common_cpuinfo._stepping));
    message._cpu_info._info.insert(make_pair("cache_size", common_cpuinfo._cache_size));
    message._cpu_info._info.insert(make_pair("fpu", common_cpuinfo._fpu));
    message._cpu_info._info.insert(make_pair("fpu_exception", common_cpuinfo._fpu_exception));
    message._cpu_info._info.insert(make_pair("cpuid_level", common_cpuinfo._cpuid_level));
    message._cpu_info._info.insert(make_pair("wp", common_cpuinfo._wp));
    message._cpu_info._info.insert(make_pair("flags", common_cpuinfo._flags));
    message._cpu_info._info.insert(make_pair("cpu_bench", common_cpuinfo._cpu_bench));
}

/******************************************************************************/
void HostAgent::GetDiskInfoForDiscover(int64 &disk_total,int64 &disk_max)
{
    StorageOperation *pInst=StorageOperation::GetInstance();
    TVGSpaceTotalInfo storage_info;
    if (pInst && SUCCESS == pInst->get_vginfo(storage_info))
    {
        disk_total = storage_info.uiTotalCapacity;
        disk_max = storage_info.uiTotalCapacity;
    }
    else
    {
        OutPut(SYS_ALERT, "Get VG space total info failed!\n");
        disk_total = 0;
        disk_max   = 0;
    }
}

/******************************************************************************/
int32 HostAgent::GetHostRunningTime(int64 &running_time)
{
    string  strsec;
    string cmd = "cat /proc/uptime |awk -F. '{print $1}'";

    int ret = RunCmd(cmd, strsec);
    if ((SUCCESS != ret) || strsec.empty())
    {
        return ERROR;
    }
    running_time = atol(strsec.c_str());
    return SUCCESS;
}

/******************************************************************************/
void HostAgent::GetDiskInfoForReport(vector<DiskInfo>& vec_disk_info)
{
    if ((0 == _report_count) 
         || (0 == _report_count % (12*60)))
    {
        /* �ݶ�1Сʱ�ϱ�һ��, ���ٽű��ĵ���
         */
        vector<T_DmPdSmartInfo> smartInfo;
        T_DmPdSmartInfo         tmpSmartInfo;
        vector<T_DmPdSmartInfo>::iterator it;
        DiskInfo                disk_info;
        /* ���ǰ��Ҫ��_vec_disk_info���,
         * ������ظ��ۼ�
         */
        _vec_disk_info.clear();
        DiskSmart::getSmartInfo(smartInfo);
        for (it = smartInfo.begin(); it != smartInfo.end(); it++)
        {
            tmpSmartInfo = *it;
            /* ��֧��SMART���, Ӳ�̽���״̬�����д?
             * ����дΪ0, Ϊ��ʼֵ
             */
            if ((PD_NOT_SUPPORT_SMART == tmpSmartInfo.ucIsSupportSmart)
                || (PD_SMART_OFF == tmpSmartInfo.ucIsSmartEnable))
            {
                tmpSmartInfo.ucSmartStatus = 0;
            }
            disk_info._name = tmpSmartInfo.aucPdName;
            disk_info._type = tmpSmartInfo.ucDiskType;
            disk_info._status = tmpSmartInfo.ucSmartStatus;
            _vec_disk_info.push_back(disk_info);
        }
    }
    vec_disk_info = _vec_disk_info;
}

/******************************************************************************/
void HostAgent::GetBaseLocationInfo(map<string,string> &hc_location)
{
    // ��������λ����Ϣ
    map<string,string> location_map;
    map<string,string>::iterator it;
    int i = 1;
    ostringstream oss;
    if (!location.empty())
    {
        string level;
        string::size_type position;

        //������ʽΪchina:jiangsu:nanjing��λ����Ϣ������map<string,string>��
        while (location.npos != location.find(":"))
        {
            position = location.find(":");
            level = location.substr(0,position);
            OutPut(SYS_DEBUG, "level=%s!\n", level.c_str());
            location = location.erase(0,position + 1);
            //��level����iת��Ϊ�ַ���
            oss << "_level" << i;
            hc_location.insert(make_pair(oss.str(),level));
            oss.str("");
            OutPut(SYS_DEBUG, "location after delete=%s!\n", level.c_str());
            i=i+1;
        }

        if (false == location.empty())
        {
            oss << "_level" << i;
            hc_location.insert(make_pair(oss.str(),location));
        }
    }

    for (it = hc_location.begin(); it != hc_location.end(); ++it)
    {
        cout << it->first << " : " << it->second << endl;
    }

}

/******************************************************************************/
void HostAgent::GetExtendLocationInfo(BoardPosition &phy_position)
{
    if (0 != access("/usr/local/bin/kcscmd", X_OK))
    {
        return;
    }
    string res,level,number;
    string cmd = "/usr/local/bin/kcscmd -l 2>/dev/null "
                     "| grep -E \"Bureau|Rack|Shelf|Slot\" "
                     "| awk -F \":\" '{print $2}'";
    RunCmd(cmd, res);
    stringstream iss(res);	
    int i = 0;
    while(getline(iss,number))
    {
        switch(i)
        {
            case 0:
                phy_position.bureau = atoi(number.c_str());
                break;
            case 1:
                phy_position.rack = atoi(number.c_str());
                break;
            case 2:
                phy_position.shelf = atoi(number.c_str());
                break;
            case 3:
                phy_position.slot = atoi(number.c_str());
                break;
            default:
                break;
        }
        i++;
    }
}

/******************************************************************************/
void HostAgent::ConstSdeviceInfo(vector<PciPthDevInfo> &data)
{
    //��������豸��Ϣ�ֶ�����
    PCIPthManager *pth_manager = PCIPthManager::GetInstance();
    if (pth_manager)
    {
        pth_manager->ReportInfo(data);
    }

}
    
/******************************************************************************/
void HostAgent::LightHost(LightState light_state, bool is_err)
{

    static bool registered_err = false;
    static bool port_err = false;
    static bool disk_err = false;
    static bool memory_err = false;
    static bool oscillator_err = false;

    static LightInfo light_info;
    static LightAgent *light_agent = NULL;
    string setter;

    if (NULL == light_agent)
    {
        light_agent = LightAgent::GetInstance();
        light_info._type = LIGHT_TYPE_HOST;
        setter = "host_agent_of_" + ApplicationName() + "_" + ProcessName();
        strncpy(light_info._setter,setter.c_str(),LI_MAX_SETTER_LEN-1);
    }

    switch (light_state)
    {
        case LIGHT_NO_JOINED:
        {
            if (registered_err != is_err)
            {
                registered_err = is_err;
                light_info._state = light_state;
                light_info._priority = 2;
                if (is_err)
                {
                    strncpy(light_info._remark,"Not registered into cluster.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOn(light_info);
                }
                else
                {
                    strncpy(light_info._remark,"Have registered into cluster.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOff(light_info);
                }
            }
            break;
        }
        case LIGHT_NIC_ERR:
        {
            if (port_err != is_err)
            {
                port_err = is_err;
                light_info._state = light_state;
                light_info._priority = 3;
                if (is_err)
                {
                    strncpy(light_info._remark,"Port error.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOn(light_info);
                }
                else
                {
                    strncpy(light_info._remark,"Port normal.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOff(light_info);
                }
            }
            break;
        }
        case LIGHT_DISK_ERR:
        {
            if (disk_err != is_err)
            {
                disk_err = is_err;
                light_info._state = light_state;
                light_info._priority = 4;
                if (is_err)
                {
                    strncpy(light_info._remark,"Disk error.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOn(light_info);
                }
                else
                {
                    strncpy(light_info._remark,"Disk normal.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOff(light_info);
                }
            }
            break;
        }
        case LIGHT_RAM_ERR:
        {
            if (memory_err != is_err)
            {
                memory_err = is_err;
                light_info._state = light_state;
                light_info._priority = 5;
                if (is_err)
                {
                    strncpy(light_info._remark,"Memory error.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOn(light_info);
                }
                else
                {
                    strncpy(light_info._remark,"Memory normal.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOff(light_info);
                }
            }
            break;
        }
        case LIGHT_CRITICAL_ERR:
        {
            if (oscillator_err != is_err)
            {
                oscillator_err = is_err;
                light_info._state = light_state;
                light_info._priority = 6;
                if (is_err)
                {
                    strncpy(light_info._remark,"Oscillator error.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOn(light_info);
                }
                else
                {
                    strncpy(light_info._remark,"Oscillator normal.",LI_MAX_REMARK_LEN-1);
                    light_agent->TurnOff(light_info);
                }
            }
            break;
        }
        default:
            break;
    }
}

/******************************************************************************/
void HostAgent::GetHostHwMonInfo(vector<HostHwmon>& vec_host_hwmon)
{
    vec_host_hwmon.clear();
    /* ���sensors�Ƿ��Ѿ���װ��������û��װ���������޷�ִ��
     */
    string result;
    string cmd = "/usr/bin/sensors 2>/dev/null";
    int ret = RunCmd(cmd, result);
    if ((SUCCESS != ret) || (result.empty()))
    {
        return;
    }
    HostHwmon host_hwmon;
    result.clear();
    cmd = "/usr/bin/sensors "
          "| grep -E \":\\s+[+-][0-9]+\\.[0-9]+.C\"";
    RunCmd(cmd, result);
    stringstream isstemp(result);
    string line;
    string::size_type pos = 0;
    while (getline(isstemp, line))
    {
        pos = line.find(':');
        if (line.npos == pos)
            continue;
        host_hwmon._name = line.substr(0, pos);
        cmd = "echo \"" + line.substr(pos+1, line.size())
              + "\" | awk -F ' ' '{print $1}'";
        result.clear();
        RunCmdAndTrim(cmd, result);
        host_hwmon._value = result;
        host_hwmon._type = SENSOR_TYPE_TEMP;
        vec_host_hwmon.push_back(host_hwmon);
    }

    cmd = "/usr/bin/sensors "
          "| grep -E \":\\s+[0-9]+\\sRPM\"";
    result.clear();
    RunCmd(cmd, result);
    stringstream issfan(result);
    while (getline(issfan, line))
    {
        pos = line.find(':');
        if (line.npos == pos)
            continue;
        host_hwmon._name = line.substr(0, pos);
        cmd = "echo \"" + line.substr(pos+1, line.size())
              + "\" | awk -F ' ' '{print $1}'";
        result.clear();
        RunCmdAndTrim(cmd, result);
        host_hwmon._value = result + " RPM";
        host_hwmon._type = SENSOR_TYPE_FANRPM;
        vec_host_hwmon.push_back(host_hwmon);
    }

    cmd = "/usr/bin/sensors "
          "| grep -E \"vid:\\s+[+-][0-9]+\\.[0-9]+\\sV\"";
    result.clear();
    RunCmd(cmd, result);
    stringstream isscpuvid(result);
    while (getline(isscpuvid, line))
    {
        pos = line.find(':');
        if (line.npos == pos)
            continue;
        host_hwmon._name = line.substr(0, pos);
        cmd = "echo \"" + line.substr(pos+1, line.size())
              + "\" | awk -F ' ' '{print $1}'";
        result.clear();
        RunCmdAndTrim(cmd, result);
        host_hwmon._value = result + " V";
        host_hwmon._type = SENSOR_TYPE_CPUVID;
        vec_host_hwmon.push_back(host_hwmon);
    }

    cmd = "/usr/bin/sensors "
          "| grep -E \":\\s+[+-][0-9]+\\.[0-9]+\\sV\" "
          "| grep -v \"vid\"";
    result.clear();
    RunCmd(cmd, result);
    stringstream issin(result);
    while (getline(issin, line))
    {
        pos = line.find(':');
        if (line.npos == pos)
            continue;
        host_hwmon._name = line.substr(0, pos);
        cmd = "echo \"" + line.substr(pos+1, line.size())
              + "\" | awk -F ' ' '{print $1}'";
        result.clear();
        RunCmdAndTrim(cmd, result);
        host_hwmon._value = result + " V";
        host_hwmon._type = SENSOR_TYPE_IN;
        vec_host_hwmon.push_back(host_hwmon);
    }
}

/******************************************************************************/
void HostAgent::GetHostStatistics(ResourceStatistics& host_statistics)
{
    if (0 == statistics_interval)
    {
        host_statistics = _pre_host_statistics;
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
            XenHostStatistics(host_statistics);
        }
        else if (true == IsKvmKernel())
        {
            KvmHostStatistics(host_statistics);
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
            GetNicUsedRate(host_statistics._vec_nic_info);
        else
            _pre_host_statistics = host_statistics;
    }
    else
        host_statistics = _pre_host_statistics;
}

/******************************************************************************/
void HostAgent::GetNicUsedRate(vector<NicInfo>& vec_nic_info)
{
    int time_interval = 5; // 5s, ��λ����
    vector<NicInfo>::iterator it_now;
    vector<NicInfo>::iterator it_pre;
    for (it_now  = vec_nic_info.begin();
         it_now != vec_nic_info.end();
         it_now++)
    {
        it_pre = find_if(_pre_host_statistics._vec_nic_info.begin(),
                         _pre_host_statistics._vec_nic_info.end(),
                         NicInfo(it_now->_name));
        if ((it_pre != _pre_host_statistics._vec_nic_info.end())
            && (0 < it_now->_speed))
        {// Ѱ�ҵ���Ӧ������, ������Ϊ pre
            it_now->_used_rate = 1000 * (it_now->_net_flow - it_pre->_net_flow) / (it_now->_speed * time_interval);
            *it_pre = *it_now;
        }
    }
    _pre_host_statistics._vec_nic_info.clear();
    _pre_host_statistics._vec_nic_info = vec_nic_info;
}

/******************************************************************************/
/* brief: xen���⻯�µ��������Դʹ��ͳ��
 * in   : 
 * out  : host_statistics - �����ͳ�Ƶ���Դ��Ϣ
 * return: SUCCESS/ERROR
 * note : �������, ��������ʾ, ���� 56.2% ��¼Ϊ 562, ���ʱΪ 0.562
 */
int32 HostAgent::XenHostStatistics(ResourceStatistics& host_statistics)
{
    // ͳ��ʱ��
    Datetime now;
    host_statistics._statistics_time = now.tostr();
    string strtmp;
    string::size_type pos = 0;
    /* CPUʹ����
     * �漰����: 
     *      xl info - ��ȡ�ܺ���
     *      xentop  - ��ȡ����dom��cpuʹ����
     * ��ӡ:
     *      nr_cpus                : 16
     *      NAME          STATE       CPU(sec) CPU(%)     MEM(k) MEM(%)
     *      instance_1020 --b---      14939     13.2    1052644    4.2
     * ����:
     *      sum(all dom's cpu_used_rate)/cpus
     */
    string cmd = "xl info | grep -E 'nr_cpus'"
                 "| awk -F ' ' '{print $3}'";
    int ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    pos = strtmp.find('\n', 0);
    if (strtmp.npos != pos)
        strtmp.erase(pos);
    int cpu_nr = atoi(strtmp.c_str());

    /* -b batchģʽ, ����ɼ���Ϣ, ���ڹܵ�����
     * -f ��ʾȫ��, �˴�Ҳ�ɲ���Ҫ
     * -i ��������, ��һ��Ϊ��ʼֵ, ��ͳ�Ƶڶ���
     * -d ÿ�ε�����ʱ0.3��
     */
    strtmp.clear();
    cmd.clear();
    pos = 0;
    cmd = "xentop -b -f -i 2 -d 0.3|grep -E \"Domain-0|instance_\"";
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    // 10 offset �ܱܿ���һ�ε�ͳ��
    pos = strtmp.find("Domain-0", 10);
    if (strtmp.npos == pos)
    {
        return ERROR;
    }
    string strsec = strtmp.substr(pos);

    stringstream iss(strsec);
    string line;
    int sum = 0;
    int ivalue = 0;
    while (getline(iss, line))
    {
        strtmp.clear();
        cmd.clear();
        pos = 0;
        ivalue = 0;
        cmd = "echo \"" + line + "\" |awk -F ' ' '{print $4}'";
        ret = RunCmd(cmd, strtmp);
        if ((SUCCESS != ret) || strtmp.empty())
        {
            continue;
        }
        pos = strtmp.find('\n', 0);
        if (strtmp.npos != pos)
            strtmp.erase(pos);
        ivalue = atof(strtmp.c_str()) * 10; // xentop��ȡ�����Ѿ�����100
        sum += ivalue;
    }
    host_statistics._cpu_used_rate = sum/cpu_nr;

    /* �ڴ�ʹ����
     * ����: xl info
     * ��ӡ: 
     *      total_memory           : 49103
     *      free_memory            : 10192
     * ����:
     *     1000*(total-free)/total
     */
    strtmp.clear();
    cmd.clear();
    pos = 0;
    cmd = "xl info | grep -E 'total_memory'"
                 "| awk -F ' ' '{print $3}'";
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    pos = strtmp.find('\n', 0);
    if (strtmp.npos != pos)
        strtmp.erase(pos);
    int mem_total = atoi(strtmp.c_str());

    strtmp.clear();
    cmd.clear();
    pos = 0;
    cmd = "xl info | grep -E 'free_memory'"
                 "| awk -F ' ' '{print $3}'";
    ret = RunCmd(cmd, strtmp);
    if ((SUCCESS != ret) || strtmp.empty())
    {
        return ERROR;
    }
    pos = strtmp.find('\n', 0);
    if (strtmp.npos != pos)
        strtmp.erase(pos);
    int mem_free = atoi(strtmp.c_str());
    host_statistics._mem_used_rate = 1000*(mem_total-mem_free)/mem_total;

    /* ����ʹ���� */
    /* �������:
     *  ����ʱ��� t(s), �������� 100Mb/s
     *  ����ʹ����: t�ڵ��շ����Ĵ�С(Mb)/(100 * t)
     * �������3:
     *  ls -l /sys/class/net | grep pci
     *      ��ȡ����������Ϣ, ���˵���������
     *      ��ӡ: lrwxrwxrwx 1 root root 0 Aug  7 16:55 eth0 ->
     *      ../../devices/pci0000:00/0000:00:1c.1/0000:02:00.0/net/eth0
     *  ifconfig ethx | grep -E 'RX bytes'(�� TX bytes)
     *  | awk -F '(' '{print $1}' | awk -F ':' '{print $2}'
     *      ifconfig ethx: ��ȡ ethx ����Ϣ, ���а��� rx, tx
     *      grep: ����ȡ���� rx tx ��һ��
     *      awk: ��ȡ�� rx, tx ���ֽ���
     *      ��ӡ: RX bytes:1115425514 (1.0 GiB)  TX bytes:424983408 (405.2 MiB)
     *  ethtool ethx | grep Speed | awk -F 'M' '{print $1}' | awk -F ':' '{print $2}'
     *      ethtool: ��ȡ������Ϣ
     *      grep: ����ȡ���� Speed ��һ��, ע�� S Ϊ��д 
     *      awk: ��ȡ speed ��ֵ
     *      ��ӡ: Speed: 100Mb/s
     * NOTE: �������� MB Ϊ��λ
     */
    string strall;
    cmd = "ls -l /sys/class/net | grep pci";
    ret = RunCmd(cmd, strall);
    if ((SUCCESS != ret) || strall.empty())
    {
        return ERROR;
    }
    stringstream iss2(strall);
    string nic_name, strrx, strtx, strspeed;
    int index = 0;
    long long int irx = 0, itx = 0;
    NicInfo nic_info;
    line.clear();
    while (getline(iss2, line))
    {
        pos = 0;
        pos = line.find_last_of('/', line.size());
        if (line.npos == pos)
            continue;
        nic_name.clear();
        index++;
        nic_name = line.substr(pos+1);
        nic_info._name = nic_name;

        cmd.clear();
        strrx.clear();
        irx = 0;
        cmd = "ifconfig " + nic_name + "| grep -E 'RX bytes' | awk -F '(' '{print $1}' | awk -F ':' '{print $2}'";
        ret = RunCmd(cmd, strrx);
        if ((SUCCESS != ret) || strrx.empty())
        {
            continue;
        }
        pos = strrx.find('\n', 0);
        if (strrx.npos != pos)
            strrx.erase(pos);
        irx = atoi(strrx.c_str());

        cmd.clear();
        strtx.clear();
        itx = 0;
        cmd = "ifconfig " + nic_name + "| grep -E 'TX bytes' | awk -F ':' '{print $3}' | awk -F '(' '{print $1}'";
        ret = RunCmd(cmd, strtx);
        if ((SUCCESS != ret) || strtx.empty())
        {
            continue;
        }
        pos = strtx.find('\n', 0);
        if (strtx.npos != pos)
            strtx.erase(pos);
        itx = atoi(strtx.c_str());

        nic_info._net_flow = irx + itx;

        cmd.clear();
        strspeed.clear();
        cmd = "ethtool " + nic_name + "| grep Speed | awk -F 'M' '{print $1}' | awk -F ':' '{print $2}'";
        ret = RunCmd(cmd, strspeed);
        if ((SUCCESS != ret) || strspeed.empty())
        {
            continue;
        }
        pos = strspeed.find('\n', 0);
        if (strspeed.npos != pos)
            strspeed.erase(pos);
        nic_info._speed = atoi(strspeed.c_str()) * 1024 *1024; // �����ȡ���ٶȵ�λ�� MB/s
        host_statistics._vec_nic_info.push_back(nic_info);
    }

    return SUCCESS;
}

/******************************************************************************/
int32 HostAgent::KvmHostStatistics(ResourceStatistics& host_statistics)
{
    Datetime now;
    host_statistics._statistics_time = now.tostr();
    /* CPUʹ����/MEMʹ����
     * �漰����: 
     *      top -bn 2 -d 0.5 | grep \"Cpu|Mem\" | awk -F ':' '{print $2}' 
     *      | awk -F ',' '{print $4}' | awk -F '%' '{print $1}'
     * ��ӡ: (ע�⿪ʼ�����ܻ���ڿո�)
     *         Cpu(s):  0.3%us,  0.1%sy,  0.0%ni, 99.5%id,  0.1%wa,  0.0%hi,  0.0%si,  0.0%st
     *         Mem:   1887136k total,  1465440k used,   421696k free,    63744k buffers
     * ����:
     *      ��ȡ����ʹ�����Ѿ�*100, then (100-idle)*10
     * note: Top ��һ�λ�ȡ����ʹ����ʱ��ϵͳ����������ƽ��ֵ
     *       ȡ�ڶ��ε�Ϊ׼
     */
    string strall, strsec;
    string::size_type pos;
    /* cpu_used_rate
     * */
    /* �������1:
     *  top -bn 2 -d 0.5
     *      -b batch ģʽ, ����ˢ��, �����ڹܵ�
     *      -n ͳ�ƴ���, 2 ��
     *      -d ͳ�Ƽ��, 0.5 s
     *  grep Cpu | grep id
     *      ɸ�� cpu ��ص���Ϣ
     *  awk -F ':' '{print $2}'| awk -F ',' '{print $4}' | awk -F '%' '{print $1}'
     *      1st awk ȥ�� Cpu(s):
     *      2nd awk ��ȡ�� 99.5%id
     *      3rd awk ��ȡ�� 99.5
     */
    string cmd = "top -bn 2 -d 0.5"
                 "| grep Cpu | grep id";
    int ret = RunCmd(cmd, strall);
    if ((SUCCESS != ret) || strall.empty())
    {
        return ERROR;
    }
    pos = strall.find("Cpu", 10);
    if (pos == strall.npos)
    {
        return ERROR;
    }
    strsec = strall.substr(pos);

    string strcpu;
    cmd = "echo \"" + strsec + "\" | awk -F ':' '{print $2}'"
          "| awk -F ',' '{print $4}'"
          "| awk -F '%' '{print $1}'";
    ret = RunCmd(cmd, strcpu);
    if ((SUCCESS != ret) || strcpu.empty())
    {
        return ERROR;
    }
    pos = strcpu.find('\n', 0);
    if (strcpu.npos != pos)
        strcpu.erase(pos);
    host_statistics._cpu_used_rate = (100-atof(strcpu.c_str()))*10;

    /* mem_used_rate
     * */
    /* �������2:
     *  top -bn 2 -d 0.5
     *      �μ����1
     *  grep Mem | grep free
     *      ɸ�� mem ��ص���Ϣ
     *  awk -F ':' '{print $2}'| awk -F 'k' '{print $1}'
     *      1st awk ȥ�� Mem:
     *      2nd awk ��ȡ�� total ��ֵ
     *  awk -F ':' '{print $2}'| awk -F ',' '{print $3}'| awk -F 'k' '{print $1}'
     *      1st awk ȥ�� Mem:
     *      2nd awk ��ȡ�� 421696k free
     *      3rd awk ��ȡ�� 421696
     * note: 
     *      �㷨: 1000 * (total - free) / total
     *      int ���ͱ�ʾ�ڴ�, ֧������ڴ�Ϊ 4095 GB
     */
    strall.clear();
    strsec.clear();
    cmd = "top -bn 2 -d 0.5"
                 "| grep Mem | grep free";
    ret = RunCmd(cmd, strall);
    if ((SUCCESS != ret) || strall.empty())
    {
        return ERROR;
    }
    pos = strall.find("Mem", 10);
    if (pos == strall.npos)
    {
        return ERROR;
    }
    strsec = strall.substr(pos);

    string strtotal;
    cmd = "echo \"" + strsec + "\" | awk -F ':' '{print $2}'"
          "| awk -F 'k' '{print $1}'";
    ret = RunCmd(cmd, strtotal);
    if ((SUCCESS != ret) || strtotal.empty())
    {
        return ERROR;
    }
    pos = strtotal.find('\n', 0);
    if (strtotal.npos != pos)
        strtotal.erase(pos);
    int itotal = atoi(strtotal.c_str());

    string strfree;
    cmd = "echo \"" + strsec + "\" | awk -F ':' '{print $2}'"
          "| awk -F ',' '{print $3}'"
          "| awk -F 'k' '{print $1}'";
    ret = RunCmd(cmd, strfree);
    if ((SUCCESS != ret) || strfree.empty())
    {
        return ERROR;
    }
    pos = strfree.find('\n', 0);
    if (strfree.npos != pos)
        strfree.erase(pos);
    int ifree = atoi(strfree.c_str());

    host_statistics._mem_used_rate = 1000 * (itotal - ifree) / itotal;

    // ����ʹ����
    /* �������:
     *  ����ʱ��� t(s), �������� 100Mb/s
     *  ����ʹ����: t�ڵ��շ����Ĵ�С(Mb)/(100 * t)
     * �������3:
     *  ls -l /sys/class/net | grep pci
     *      ��ȡ����������Ϣ, ���˵���������
     *      ��ӡ: lrwxrwxrwx 1 root root 0 Aug  7 16:55 eth0 ->
     *      ../../devices/pci0000:00/0000:00:1c.1/0000:02:00.0/net/eth0
     *  ifconfig ethx | grep -E 'RX bytes'(�� TX bytes)
     *  | awk -F '(' '{print $1}' | awk -F ':' '{print $2}'
     *      ifconfig ethx: ��ȡ ethx ����Ϣ, ���а��� rx, tx
     *      grep: ����ȡ���� rx tx ��һ��
     *      awk: ��ȡ�� rx, tx ���ֽ���
     *      ��ӡ: RX bytes:1115425514 (1.0 GiB)  TX bytes:424983408 (405.2 MiB)
     *  ethtool ethx | grep Speed | awk -F 'M' '{print $1}' | awk -F ':' '{print $2}'
     *      ethtool: ��ȡ������Ϣ
     *      grep: ����ȡ���� Speed ��һ��, ע�� S Ϊ��д 
     *      awk: ��ȡ speed ��ֵ
     *      ��ӡ: Speed: 100Mb/s
     * NOTE: �������� B Ϊ��λ
     */
    strall.clear();
    cmd = "ls -l /sys/class/net | grep pci";
    ret = RunCmd(cmd, strall);
    if ((SUCCESS != ret) || strall.empty())
    {
        return ERROR;
    }
    stringstream iss(strall);
    string line, nic_name, strrx, strtx, strspeed;
    int index = 0;
    int64 irx = 0, itx = 0;
    NicInfo nic_info;
    while (getline(iss, line))
    {
        pos = 0;
        pos = line.find_last_of('/', line.size());
        if (line.npos == pos)
            continue;
        nic_name.clear();
        index++;
        nic_name = line.substr(pos+1);
        nic_info._name = nic_name;

        cmd.clear();
        strrx.clear();
        irx = 0;
        cmd = "ifconfig " + nic_name + "| grep -E 'RX bytes' | awk -F '(' '{print $1}' | awk -F ':' '{print $2}'";
        ret = RunCmd(cmd, strrx);
        if ((SUCCESS != ret) || strrx.empty())
        {
            continue;
        }
        pos = strrx.find('\n', 0);
        if (strrx.npos != pos)
            strrx.erase(pos);
        irx = atoi(strrx.c_str());

        cmd.clear();
        strtx.clear();
        itx = 0;
        cmd = "ifconfig " + nic_name + "| grep -E 'TX bytes' | awk -F ':' '{print $3}' | awk -F '(' '{print $1}'";
        ret = RunCmd(cmd, strtx);
        if ((SUCCESS != ret) || strtx.empty())
        {
            continue;
        }
        pos = strtx.find('\n', 0);
        if (strtx.npos != pos)
            strtx.erase(pos);
        itx = atoi(strtx.c_str());

        nic_info._net_flow = irx + itx;

        cmd.clear();
        strspeed.clear();
        cmd = "ethtool " + nic_name + "| grep Speed | awk -F 'M' '{print $1}' | awk -F ':' '{print $2}'";
        ret = RunCmd(cmd, strspeed);
        if ((SUCCESS != ret) || strspeed.empty())
        {
            continue;
        }
        pos = strspeed.find('\n', 0);
        if (strspeed.npos != pos)
            strspeed.erase(pos);
        nic_info._speed = atoi(strspeed.c_str()) * 1024 * 1024;
        
        host_statistics._vec_nic_info.push_back(nic_info);
    }

    return SUCCESS;
    
}


/******************************************************************************/
/* brief: Ӳ�̽���״̬���
 * in   : 
 *      disk_name: Ӳ������
 * out  : 
 *      disk_info: Ӳ����Ϣ
 * return: SUCCESS/ERROR
 * note :
 */
int HostAgent::SetDiskInfo(string disk_name, int status)
{
    if (disk_name.empty()
        || (1 > status)
        || (3 < status))
    {
        return ERROR;
    }
    vector<DiskInfo>::iterator it;
    for (it  = _vec_disk_info.begin();
         it != _vec_disk_info.end();
         it++)
    {
        if (!it->_name.compare(disk_name))
        {
            it->_status = status;
            break; // �������Ӳ����һ�������
        }
    }
    return SUCCESS;
}

/******************************************************************************/
/* brief: ģ��hostӲ�̵Ĳ�ͬ����״̬
 * in   : 
 *        status      - Ӳ�̵Ľ���״̬, 1(normal), 2(warn), 3(fault)
 *        disk_name   - Ӳ�̵�����, ��/dev/sda, ��ͨ��������ȡ��
 * out  :
 * return:
 * note :
 *      ��1,2,3��ʾ����, ������εĽ��ͺͱ�host���е�Ӳ��
 */
void DbgSimuDiskHealth(int status, char *disk_name)
{
    // ��ȡhost_agentʵ��
    HostAgent *host_agent = HostAgent::GetInstance();
    if (NULL == host_agent)
    {
        cout << "Please check host_agent if work normally!" << endl;
        return;
    }

    /* �����>=1�����, 0Ϊ���Ժ������� */
    if ((1 > status) || (3 < status))
    {
        cout << "-help" << endl;
        cout << "--input params:" << endl;
        cout << "---status" << endl;
        cout << "      1 - disk status: normal" << endl;
        cout << "      2 - disk status: warn" << endl;
        cout << "      3 - disk status: fault" << endl;
        cout << "---disk_name" << endl;
        cout << "Disks that this host has:" << endl;
        vector<DiskInfo> vec_disk_info;
        host_agent->GetAllDiskInfo(vec_disk_info);
        vector<DiskInfo>::iterator it;
        string disk_type;
        string disk_status;
        int index = 0;
        for (it  = vec_disk_info.begin();
             it != vec_disk_info.end();
             it++)
        {
            index++;
            if (DISK_TYPE_ATA == it->_type)
            {
                disk_type = "ATA";
            }
            else if (DISK_TYPE_SCSI == it->_type)
            {
                disk_type = "SCSI";
            }
            else
            {
                cout << it->_name << ", Unknown disk type!" << endl;
                continue;
            }
            
            cout << index << ". " << it->_name
                 << ", type: " << disk_type
                 << ", status: " << it->_status << endl;
        }
        return;
    }

    // ��������
    host_agent->SetDiskInfo(disk_name, status);
    
    return;
}
DEFINE_DEBUG_FUNC(DbgSimuDiskHealth);
DEFINE_DEBUG_FUNC_ALIAS(disksense, DbgSimuDiskHealth);
/******************************************************************************/
void DbgGetIpByConnect(const char* borker_ip)
{
    string ip_address;
    if(SUCCESS != GetIpByConnectBroker(borker_ip,ip_address))
    {
        return; 
    }
    printf("%s ip = %s \n",borker_ip ,ip_address.c_str());
}
DEFINE_DEBUG_FUNC(DbgGetIpByConnect);

/******************************************************************************/
void DbgPrintAllHostCpus()
{
    HostAgent *host_agent = HostAgent::GetInstance();
    if (NULL == host_agent)
    {
        cout << "host_agent is not created." << endl;
        return;
    }
    cout << "all cpu core num = " << host_agent->GetPcpuNum() << endl;
    cout << "cpu cores for vm = " << host_agent->GetCpusForVm() << endl;
    cout << "dom0 band cpu num = " << host_agent->GetDom0Cpus() << endl;

    return;
}
DEFINE_DEBUG_CMD(allcpus, DbgPrintAllHostCpus);

/******************************************************************************/
void DbgPrintAllDiscoverInfo()
{
    HostAgent *host_agent = HostAgent::GetInstance();
    if (NULL == host_agent)
    {
        cout << "host_agent is not created." << endl;
        return;
    }
    switch (host_agent->GetHypervisorType())
    {
        case XEN_HYPERVISOR:
        {
            cout << "mem_total       = " << host_agent->GetTotalMemoryForXen() << endl;
            cout << "mem_max         = " << host_agent->GetFreeMemoryForXen() << endl;
            cout << "vmm_info        = " << host_agent->GetVmmInfoForXen() << endl;
            cout << "processor_num   = " << host_agent->GetCpuCoreNumForXen() << endl;    
            cout << "is_support_hvm  = " << host_agent->GetIsSupportHvmForXen() << endl;
            break;
        }
        case KVM_HYPERVISOR:
        {
            cout << "mem_total       = " << host_agent->GetTotalMemoryForKvm() << endl;
            cout << "mem_max         = " << host_agent->GetFreeMemoryForKvm() << endl;
            cout << "vmm_info        = " << host_agent->GetVmmInfoForKvm() << endl;
            cout << "processor_num   = " << host_agent->GetCpuCoreNumForKvm() << endl;    
            cout << "is_support_hvm  = " << host_agent->GetIsSupportHvmForKvm() << endl;
            break;
        }
        default:
            break;
    }

    cout << "ssh_key         = " << host_agent->GetSshKey() << endl;
    cout << "os_info         = " << host_agent->GetOsInfo() << endl;

    CommonCpuInfo common_cpuinfo;
    host_agent->GetCommonCpuInfo(common_cpuinfo);
    cout << "physical_id_num = " << common_cpuinfo._physical_id_num << endl;
    cout << "cpu_cores       = " << common_cpuinfo._cpu_cores << endl;
    cout << "vendor_id       = " << common_cpuinfo._vendor_id << endl;
    cout << "cpu_family      = " << common_cpuinfo._cpu_family << endl;
    cout << "model           = " << common_cpuinfo._model << endl;
    cout << "model_name      = " << common_cpuinfo._model_name << endl;
    cout << "stepping        = " << common_cpuinfo._stepping << endl;
    cout << "cache_size      = " << common_cpuinfo._cache_size << endl;
    cout << "fpu             = " << common_cpuinfo._fpu << endl;
    cout << "fpu_exception   = " << common_cpuinfo._fpu_exception << endl;
    cout << "cpuid_level     = " << common_cpuinfo._cpuid_level << endl;
    cout << "wp              = " << common_cpuinfo._wp << endl;
    cout << "flags           = " << common_cpuinfo._flags << endl;
    cout << "cpu_bench       = " << common_cpuinfo._cpu_bench << endl;

    string ip, mask;
    if (0 != GetKernelPortInfo("dvs_kernel", ip, mask))
    {
        cout << "Call GetKernelPortInfo failed!" << endl;
    }
    cout << "ip_address      = " << ip << endl;

    return;
}
DEFINE_DEBUG_CMD(discoverinfo, DbgPrintAllDiscoverInfo);

/******************************************************************************/
void DbgHostStatistics()
{
    // ��ȡhost_agentʵ��
    HostAgent *host_agent = HostAgent::GetInstance();
    if (NULL == host_agent)
    {
        cout << "Please check host_agent if work normally!" << endl;
        return;
    }
    ResourceStatistics host_statistics;
    host_agent->GetHostStatistics(host_statistics);
    cout << "------ host statistics info ------" << endl;
    cout << "statistics_time: " << host_statistics._statistics_time << endl;
    cout << "cpu_used_rate  : " << host_statistics._cpu_used_rate << endl;
    cout << "mem_used_rate  : " << host_statistics._mem_used_rate << endl;
    host_agent->GetNicUsedRate(host_statistics._vec_nic_info);
    vector<NicInfo>::iterator it;
    int32 index = 0;
    for (it  = host_statistics._vec_nic_info.begin();
         it != host_statistics._vec_nic_info.end();
         it++)
    {
        index++;
        cout << "---nic " << index << " ---" << endl;
        cout << "name     : " << it->_name << endl;
        cout << "used_rate: " << it->_used_rate << endl;
        cout << "net_flow : " << it->_net_flow << " B" << endl;
        cout << "speed    : " << it->_speed << " B" << endl;
    }
}
DEFINE_DEBUG_FUNC_ALIAS(hoststatistics, DbgHostStatistics);

/******************************************************************************/
void DbgHostHwmon()
{
    // ��ȡhost_agentʵ��
    HostAgent *host_agent = HostAgent::GetInstance();
    if (NULL == host_agent)
    {
        cout << "Please check host_agent if work normally!" << endl;
        return;
    }
    vector<HostHwmon> vec_host_hwmon;
    host_agent->GetHostHwMonInfo(vec_host_hwmon);
    vector<HostHwmon>::iterator it;
    string type;
    for (it  = vec_host_hwmon.begin();
         it != vec_host_hwmon.end();
         it++)
    {
        switch (it->_type)
        {
            case SENSOR_TYPE_TEMP:
                type = "temp";
                break;
            case SENSOR_TYPE_FANRPM:
                type = "fan";
                break;
            case SENSOR_TYPE_CPUVID:
                type = "cpuvid";
                break;
            case SENSOR_TYPE_IN:
                type = "in";
                break;
            default: break;
        }
        cout << "name : " << it->_name
             << ", type : " << type
             << ", value: " << it->_value << endl;
    }
}
DEFINE_DEBUG_FUNC_ALIAS(hwmon, DbgHostHwmon);

} // namespace zte_tecs

