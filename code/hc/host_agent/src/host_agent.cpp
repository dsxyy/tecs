/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：HostAgent类实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月9日
*
* 修改记录1：
*     修改日期：2011/8/9
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
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

// 定时器消息号
#define TIMER_REGISTERED_SYSTEM     EV_TIMER_1  // 周期组播归属的cluster名称
#define TIMER_HOST_DISCOVER         EV_TIMER_2  // 周期发送主机发现消息
#define TIMER_HOST_INFO_REPORT      EV_TIMER_3  // 周期采集信息并上报给CC

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
    // 加入归属信息组播组
    STATUS ret = _mu->JoinMcGroup(string(ApplicationName()) + 
                                  MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("HostAgent failed to join group! ret = ");
        lastwords.append(to_string(ret, dec));
        SkyExit(-1,lastwords);
    }

    // 设置归属集群信息周期组播定时器
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

    // 创建主机发现或信息上报共用的定时器
    if (INVALID_TIMER_ID == (_timer_id = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, 
               "Create timer failed for send host discover or report!\n");
        SkyAssert(0);
        SkyExit(-1, "HostAgent::DoStartUp: call _mu->SetTimer failed.");
    }

    // 给HC上其他进程也告知归属集群为空
    DoRegisteredSystem();

    //假如kcs读取子卡信息失败怎么处理，xml文件能不能分批写入
    //kcs命令失败的原因有可能是框中各个板子的启动顺序，先不考虑异常异常情况，
    //如有异常请况，人为干预下，重启单板


    //帮忙调用一下PCIPthManager的初始化
    PCIPthManager *pth_manager = PCIPthManager::GetInstance();
    if (pth_manager)
    {
        pth_manager->Startup();
    }

    // 进行主机发现
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

    // 对消息内容进行合法性检查
    if (req._host_name != string(ApplicationName()))
    {
        OutPut(SYS_ALERT, "DoRegister: host name is err, %s != %s!\n",
               req._host_name.c_str(), ApplicationName().c_str());
        SkyAssert(req._host_name == string(ApplicationName()));
        return;
    }

    // 如果没有归属，则设置为发送方
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

    // 对消息内容进行合法性检查
    if (req._host_name != string(ApplicationName()))
    {
        OutPut(SYS_ALERT, "DoUnregister: host name is err, %s != %s!\n",
               req._host_name.c_str(), ApplicationName().c_str());
        SkyAssert(req._host_name == string(ApplicationName()));
        return;
    }

    // 如果不是归属的集群发过来的消息，则过滤掉
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
    
    // HC进程退出，重新发现以更新系统的归属信息
    SkyExit(-1, "Host is unregistered from the cluster, exit!\n");
}

/******************************************************************************/
void HostAgent::DoDiscover()
{
    MessageHostBaseInfo msg;
    GetBaseInfoFromScript(msg);

    //如果内存为空，则返回继续等待
    if(0 == msg._mem_max || 0 == msg._mem_total)
    {
        OutPut(SYS_ALERT, "DoDiscover: get memroy is 0!\n");
        return;
    }

    msg._cpus = _max_cpus;

    if(_max_cpus != _cpu_cores)
    {
        /* dom0 绑定 核A 的情况, ubench 仅看到 核A 的计算能力,
         * 而不同核的计算能力不同, 此情况下需用户自配
         */
        msg._cpu_info._info["cpu_bench"]="0";        
    }
	
	// 补充主机在线时间
    GetHostRunningTime(msg._running_time);
	
    // 补充硬盘信息
    GetDiskInfoForDiscover(msg._disk_total,msg._disk_max);

    // 补充基本位置信息, 从tecs.conf中解析获取
    GetBaseLocationInfo(msg._location);

    // 补充局架框槽信息
    GetExtendLocationInfo(msg._phy_position);

    GetIscsiName(msg._iscsiname);  

    // 填充特殊设备信息
    ConstSdeviceInfo(msg._sdevice_info);
    
    // 增加机框类型信息
    msg._shelf_type = shelf_type;

    // 增加单板类型bomid信息
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
    //增加媒体面地址
    msg._media_address = media_address;
    
    // 给主机发现组播组发送主机发现消息
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
    
    // 补充硬盘信息
    GetDiskInfoForReport(msg._vec_disk_info);

    //补充局架框槽信息
    GetExtendLocationInfo(msg._phy_position);

    //补充晶振状态
    if (GetCrystalStatus())
    {
        msg._oscillator = 0; //0为好，1为坏
    }
    else
    {
        msg._oscillator = 1;
    }

    msg._board_type = _board_type;

    // 硬件传感器信息
    GetHostHwMonInfo(msg._vec_host_hwmon);

    // 增加物理机资源使用统计信息
    GetHostStatistics(msg._host_statistics);

		// 补充主机在线时间
    GetHostRunningTime(msg._running_time);
    
    // 增加物理机IP地址
    string mask;
    if (0 != GetKernelPortInfo("dvs_kernel", msg._ip_address, mask))
    {
        OutPut(SYS_ALERT, "Get hc ip address info failed!\n");
        msg._ip_address.clear();
    }    
	
    _report_count++;

    // 发送消息
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
// 早期对齐V4, 非必须, 暂屏蔽, 均上报OK
#if 0
    // 判断 /proc/zte/crystal 文件是否可读
    if (0 != access("/proc/zte/crystal", R_OK))
    {
        return true;
    }

    ifstream fin("/proc/zte/crystal");
    string str;
    while(fin >> str)
    {
        // 存在多个晶振, 一行对应一个, 只要一个坏, 则输出坏
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
{// 获取母板 slot:0 的 board_type
    if (0 != access("/usr/local/bin/kcscmd", X_OK))
    {
        return ERROR;
    }
    /* 命令结果打印:
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

/* 概念解释
 * 1) 物理 CPU : 主板上看到的物理封装的处理器
 * 2) CPU 核数 : 每个物理 CPU 上处理数据的芯片组
 * 3) 逻辑 CPU : 上述 1 和 2 的乘积, 可看做整个主板上所有的核数, 重中之重
 */
/******************************************************************************/
int HostAgent::GetCpuCoreNumForXen(void)
{// 此为获取逻辑 CPU
    string result, cmd;
    cmd = "xl info | grep nr_cpus | awk -F': ' '{print $2}'";
    if (SUCCESS != RunCmd(cmd, result))
        return 0;
    else
        return atoi(result.c_str());
}

/******************************************************************************/
int HostAgent::GetCpuCoreNumForKvm(void)
{// 此为获取逻辑 CPU
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
        /* dom0 是否绑定? 
         * 1) 不绑定, 不绑定是否就一定 cpus_for_dom0 == cpus_total?
         * 2) 绑定, cpus_total - cpus_for_dom0
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
    /* 算法:
     * temp = total-dom0-9-8*dom0/1024
     *  9: 管理所有的虚拟机所需的内存, 固定
     *  8: Domain0 每G所需消耗的页表内存
     * free = (temp-12*temp/1024-8*temp/1024)*1024*1024
     *  12: Domain0 管理剩余内存每G所需要页表内存空间
     *  8 : DomainU 使用每G内存需要的额外页表内存
     *  NOTE: 有点绕, 8 的概念可统一为每个虚拟机管理自己的内存所需的内存
     *        上述 2 步简化后为如下的计算
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
{// 此部分代码可固化?
    string result, cmd;
    /* physical_id_num, cpu_cores 获取不可靠
     *  1) 如果 xen 的 dom0 绑定了核，获取到的仅是 dom0 的
     *  2) 有些处理器不支持提供这些信息, 比如 AMD
     * ps: 因不重要, 暂维持原判
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
{// 此文件暂时仅提供虚拟机查询子卡信息使用
    string    filecontext("");
    filecontext = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";

    // 构造xmlparse 
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

    //子卡部分有两层目录
    xml.Write("blade_cards","");
    xml.Enter();

    //获取出子卡的信息，轮询写入到xml文件中
    vector<BoardKCSInfo> infos;
    GetBoardKCSInfo(infos);

    vector<BoardKCSInfo>::iterator  it;
    for (it = infos.begin(); it != infos.end(); it ++)
    {
        //用循环添加item的表项，先搭个架子
        xml.Write("item","");
        xml.Enter();
        xml.Write("slot",it->_slot);
        xml.Write("pcb",it->_pcb);
        xml.Write("bom",it->_bom);
        xml.Write("id",it->_id);
    
        xml.Exit();
    }
    //子卡部分结束
    
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
    
    /* 先创建目录 */
    mkdir("/var/lib/tecs/", 0777);
    
    /* 构造输出文件 */
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
    // 上电的时候为空, 此时不需要进行操作
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
    // 上电的时候为空, 此时不需要进行操作
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
    // 点灯
    LightHost(LIGHT_NO_JOINED, true);

    // 设置主机发现循环定时器
    _mu->StopTimer(_timer_id);
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;
    timeout.msgid = TIMER_HOST_DISCOVER;
    _mu->SetTimer(_timer_id, timeout);    

    // 处理主机发现流程
    DoDiscover();
}

/******************************************************************************/
void HostAgent::GoToReport(void)
{
    // 点灯
    LightHost(LIGHT_NO_JOINED, false);

    // 设置主机信息上报循环定时器
    _mu->StopTimer(_timer_id);
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 5 * 1000;
    timeout.msgid = TIMER_HOST_INFO_REPORT;
    _mu->SetTimer(_timer_id, timeout);

    // 处理主机信息上报流程，在DoReport中无定时器处理
    DoReport();
}

/******************************************************************************/
void HostAgent::SetCluster(const string &cluster_name)
{
    if (_cluster_name != cluster_name)
    {
        //但归属的集群变化时，需要退出原来的集群，加入新的集群
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
    // 差异部分处理
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

    // 公用部分
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
        /* 暂定1小时上报一次, 减少脚本的调用
         */
        vector<T_DmPdSmartInfo> smartInfo;
        T_DmPdSmartInfo         tmpSmartInfo;
        vector<T_DmPdSmartInfo>::iterator it;
        DiskInfo                disk_info;
        /* 检测前需要将_vec_disk_info清空,
         * 否则会重复累加
         */
        _vec_disk_info.clear();
        DiskSmart::getSmartInfo(smartInfo);
        for (it = smartInfo.begin(); it != smartInfo.end(); it++)
        {
            tmpSmartInfo = *it;
            /* 不支持SMART检测, 硬盘健康状态如何填写?
             * 暂填写为0, 为初始值
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
    // 补充主机位置信息
    map<string,string> location_map;
    map<string,string>::iterator it;
    int i = 1;
    ostringstream oss;
    if (!location.empty())
    {
        string level;
        string::size_type position;

        //解析格式为china:jiangsu:nanjing的位置信息并存入map<string,string>中
        while (location.npos != location.find(":"))
        {
            position = location.find(":");
            level = location.substr(0,position);
            OutPut(SYS_DEBUG, "level=%s!\n", level.c_str());
            location = location.erase(0,position + 1);
            //将level级别i转换为字符串
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
    //填充特殊设备信息字段内容
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
    /* 检测sensors是否已经安装或因驱动没安装导致命令无法执行
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
        /* 考虑到安装 tecs 版本之前系统已经进行了通信
         * 即存在了多个统计间隔时间的收发包大小, 
         * 再和 0 做差值, 得到的使用率是很大, 不合适
         * 故第一次仅记录到内存中, 数据库中为0, 后续正常计算
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
    int time_interval = 5; // 5s, 单位是秒
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
        {// 寻找到对应的网卡, 并更新为 pre
            it_now->_used_rate = 1000 * (it_now->_net_flow - it_pre->_net_flow) / (it_now->_speed * time_interval);
            *it_pre = *it_now;
        }
    }
    _pre_host_statistics._vec_nic_info.clear();
    _pre_host_statistics._vec_nic_info = vec_nic_info;
}

/******************************************************************************/
/* brief: xen虚拟化下的物理机资源使用统计
 * in   : 
 * out  : host_statistics - 物理机统计的资源信息
 * return: SUCCESS/ERROR
 * note : 方便起见, 以整数表示, 比如 56.2% 记录为 562, 入库时为 0.562
 */
int32 HostAgent::XenHostStatistics(ResourceStatistics& host_statistics)
{
    // 统计时间
    Datetime now;
    host_statistics._statistics_time = now.tostr();
    string strtmp;
    string::size_type pos = 0;
    /* CPU使用率
     * 涉及命令: 
     *      xl info - 获取总核数
     *      xentop  - 获取各个dom的cpu使用率
     * 打印:
     *      nr_cpus                : 16
     *      NAME          STATE       CPU(sec) CPU(%)     MEM(k) MEM(%)
     *      instance_1020 --b---      14939     13.2    1052644    4.2
     * 计算:
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

    /* -b batch模式, 方便采集信息, 用于管道命令
     * -f 显示全名, 此处也可不需要
     * -i 迭代次数, 第一次为初始值, 需统计第二次
     * -d 每次迭代耗时0.3秒
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
    // 10 offset 能避开第一次的统计
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
        ivalue = atof(strtmp.c_str()) * 10; // xentop获取到的已经乘以100
        sum += ivalue;
    }
    host_statistics._cpu_used_rate = sum/cpu_nr;

    /* 内存使用率
     * 命令: xl info
     * 打印: 
     *      total_memory           : 49103
     *      free_memory            : 10192
     * 计算:
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

    /* 网卡使用率 */
    /* 概念解释:
     *  设置时间段 t(s), 网卡速率 100Mb/s
     *  网卡使用率: t内的收发包的大小(Mb)/(100 * t)
     * 命令详解3:
     *  ls -l /sys/class/net | grep pci
     *      获取物理网卡信息, 过滤掉虚拟网卡
     *      打印: lrwxrwxrwx 1 root root 0 Aug  7 16:55 eth0 ->
     *      ../../devices/pci0000:00/0000:00:1c.1/0000:02:00.0/net/eth0
     *  ifconfig ethx | grep -E 'RX bytes'(或 TX bytes)
     *  | awk -F '(' '{print $1}' | awk -F ':' '{print $2}'
     *      ifconfig ethx: 获取 ethx 的信息, 其中包括 rx, tx
     *      grep: 仅获取包含 rx tx 的一句
     *      awk: 获取到 rx, tx 的字节数
     *      打印: RX bytes:1115425514 (1.0 GiB)  TX bytes:424983408 (405.2 MiB)
     *  ethtool ethx | grep Speed | awk -F 'M' '{print $1}' | awk -F ':' '{print $2}'
     *      ethtool: 获取网卡信息
     *      grep: 仅获取包含 Speed 的一句, 注意 S 为大写 
     *      awk: 获取 speed 的值
     *      打印: Speed: 100Mb/s
     * NOTE: 出参中以 MB 为单位
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
        nic_info._speed = atoi(strspeed.c_str()) * 1024 *1024; // 命令获取到速度单位是 MB/s
        host_statistics._vec_nic_info.push_back(nic_info);
    }

    return SUCCESS;
}

/******************************************************************************/
int32 HostAgent::KvmHostStatistics(ResourceStatistics& host_statistics)
{
    Datetime now;
    host_statistics._statistics_time = now.tostr();
    /* CPU使用率/MEM使用率
     * 涉及命令: 
     *      top -bn 2 -d 0.5 | grep \"Cpu|Mem\" | awk -F ':' '{print $2}' 
     *      | awk -F ',' '{print $4}' | awk -F '%' '{print $1}'
     * 打印: (注意开始出可能会存在空格)
     *         Cpu(s):  0.3%us,  0.1%sy,  0.0%ni, 99.5%id,  0.1%wa,  0.0%hi,  0.0%si,  0.0%st
     *         Mem:   1887136k total,  1465440k used,   421696k free,    63744k buffers
     * 计算:
     *      获取到的使用率已经*100, then (100-idle)*10
     * note: Top 第一次获取到的使用率时子系统运行以来的平均值
     *       取第二次的为准
     */
    string strall, strsec;
    string::size_type pos;
    /* cpu_used_rate
     * */
    /* 命令详解1:
     *  top -bn 2 -d 0.5
     *      -b batch 模式, 不再刷屏, 可用于管道
     *      -n 统计次数, 2 次
     *      -d 统计间隔, 0.5 s
     *  grep Cpu | grep id
     *      筛出 cpu 相关的信息
     *  awk -F ':' '{print $2}'| awk -F ',' '{print $4}' | awk -F '%' '{print $1}'
     *      1st awk 去掉 Cpu(s):
     *      2nd awk 获取到 99.5%id
     *      3rd awk 获取到 99.5
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
    /* 命令详解2:
     *  top -bn 2 -d 0.5
     *      参见详解1
     *  grep Mem | grep free
     *      筛出 mem 相关的信息
     *  awk -F ':' '{print $2}'| awk -F 'k' '{print $1}'
     *      1st awk 去掉 Mem:
     *      2nd awk 获取到 total 的值
     *  awk -F ':' '{print $2}'| awk -F ',' '{print $3}'| awk -F 'k' '{print $1}'
     *      1st awk 去掉 Mem:
     *      2nd awk 获取到 421696k free
     *      3rd awk 获取到 421696
     * note: 
     *      算法: 1000 * (total - free) / total
     *      int 类型表示内存, 支持最大内存为 4095 GB
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

    // 网卡使用率
    /* 概念解释:
     *  设置时间段 t(s), 网卡速率 100Mb/s
     *  网卡使用率: t内的收发包的大小(Mb)/(100 * t)
     * 命令详解3:
     *  ls -l /sys/class/net | grep pci
     *      获取物理网卡信息, 过滤掉虚拟网卡
     *      打印: lrwxrwxrwx 1 root root 0 Aug  7 16:55 eth0 ->
     *      ../../devices/pci0000:00/0000:00:1c.1/0000:02:00.0/net/eth0
     *  ifconfig ethx | grep -E 'RX bytes'(或 TX bytes)
     *  | awk -F '(' '{print $1}' | awk -F ':' '{print $2}'
     *      ifconfig ethx: 获取 ethx 的信息, 其中包括 rx, tx
     *      grep: 仅获取包含 rx tx 的一句
     *      awk: 获取到 rx, tx 的字节数
     *      打印: RX bytes:1115425514 (1.0 GiB)  TX bytes:424983408 (405.2 MiB)
     *  ethtool ethx | grep Speed | awk -F 'M' '{print $1}' | awk -F ':' '{print $2}'
     *      ethtool: 获取网卡信息
     *      grep: 仅获取包含 Speed 的一句, 注意 S 为大写 
     *      awk: 获取 speed 的值
     *      打印: Speed: 100Mb/s
     * NOTE: 出参中以 B 为单位
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
/* brief: 硬盘健康状态监测
 * in   : 
 *      disk_name: 硬盘名字
 * out  : 
 *      disk_info: 硬盘信息
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
            break; // 不会存在硬盘名一样的情况
        }
    }
    return SUCCESS;
}

/******************************************************************************/
/* brief: 模拟host硬盘的不同三种状态
 * in   : 
 *        status      - 硬盘的健康状态, 1(normal), 2(warn), 3(fault)
 *        disk_name   - 硬盘的名字, 如/dev/sda, 可通过帮助获取到
 * out  :
 * return:
 * note :
 *      非1,2,3显示帮助, 包含入参的解释和本host持有的硬盘
 */
void DbgSimuDiskHealth(int status, char *disk_name)
{
    // 获取host_agent实例
    HostAgent *host_agent = HostAgent::GetInstance();
    if (NULL == host_agent)
    {
        cout << "Please check host_agent if work normally!" << endl;
        return;
    }

    /* 仅检测>=1的情况, 0为调试函数帮助 */
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

    // 更改数据
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
    // 获取host_agent实例
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
    // 获取host_agent实例
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

