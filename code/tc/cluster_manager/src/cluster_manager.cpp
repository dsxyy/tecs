/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：cluster_manager.cpp
* 文件标识：
* 内容摘要：cluster_manager类的定义文件
* 当前版本：1.0
* 作    者：李孝成
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李孝成
*     修改内容：创建
******************************************************************************/
#include "cluster_manager.h"
#include "tecs_pub.h"
#include "sky.h"
#include "cluster.h"
#include "cluster_pool.h"
#include "message.h"
#include "event.h"
#include "mid.h"
#include "cluster_manager_with_host_manager.h"
#include "cluster_manager_with_vm_config.h"
#include "tecs_errcode.h"
#include "log_agent.h"
#include "pub_alarm.h"
#include "db_config.h"
#include "license_common.h"
#include "dns_building.h"
#include "ntp_building.h"

namespace zte_tecs
{
/* 定时器事件，是不需要给别人看到的事件，且每个模块独立，所以放到使用空间中 */
#define EV_CLUSTER_KEEP_STATE_TIMER     EV_TIMER_1     /* 集群状态刷新定时器 */
#define EV_CLUSTER_QUERY_TIMEOUT_TIMER  EV_TIMER_2     /* 集群信息同步查询定时器 */

int  cluster_manager_dbg_flag =0;


void DbgClusterManagerDisablePrint()
{
    cluster_manager_dbg_flag = 0;
}
DEFINE_DEBUG_FUNC(DbgClusterManagerDisablePrint);


void DbgClusterManagerEnablePrint()
{
    cluster_manager_dbg_flag = 1;
}
DEFINE_DEBUG_FUNC(DbgClusterManagerEnablePrint);

void DbgShowAllCluster(void)
{
    uint32 i;

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluste_Manager_Dbg_Agent"));
    temp_mu.Register();

    /* 3. 发消息给 Cluster_Manager  */
    MessageClusterInfoReq ReqMsg(0);
    ReqMsg._StartIndex  = 0;
    ReqMsg._Query_count = 2000;
    MessageOption option(MU_CLUSTER_MANAGER, EV_CLUSTER_ACTION_INFO_REQ,0,0);
    temp_mu.Send(ReqMsg, option);

    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if (SUCCESS == temp_mu.Wait(&message,2000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("Dbg Request Recv success!\n");
        MessageClusterInfoAck  ack_message;
        ack_message.deserialize(message.data);
        for (i = 0; i < ack_message._cluster_resource.size(); i++)
        {
            printf("************************************************\r\n");
            printf("cluster_name: %s\r\n", ack_message._cluster_resource.at(i)._cluster_name.c_str());
            printf("ip: %s\r\n", ack_message._cluster_resource.at(i)._ip.c_str());
            printf("description: %s\r\n", ack_message._cluster_resource.at(i)._cluster_append.c_str());
            printf("enable state: %s\r\n", Cluster::enable_state_str(ack_message._cluster_resource.at(i)._enabled));
            printf("online state: %s\r\n", Cluster::run_state_str(ack_message._cluster_resource.at(i)._is_online));

            printf("core_free_max: %d\r\n", ack_message._cluster_resource.at(i)._resource._core_free_max);
            printf("tcu_unit_free_max: %d\r\n", ack_message._cluster_resource.at(i)._resource._tcu_unit_free_max);

            printf("tcu_max_total: %d\r\n", ack_message._cluster_resource.at(i)._resource._tcu_max_total);
            printf("tcu_free_total: %d\r\n", ack_message._cluster_resource.at(i)._resource._tcu_free_total);
            printf("tcu_free_max: %d\r\n", ack_message._cluster_resource.at(i)._resource._tcu_free_max);

            printf("mem_max_total: %.0f(M)\r\n", BytesToMB(ack_message._cluster_resource.at(i)._resource._mem_max_total));
            printf("mem_free_total: %.0f(M)\r\n", BytesToMB(ack_message._cluster_resource.at(i)._resource._mem_free_total));
            printf("mem_free_max: %.0f(M)\r\n", BytesToMB(ack_message._cluster_resource.at(i)._resource._mem_free_max));

            printf("disk_max_total: %.2f(G)\r\n", BytesToGB(ack_message._cluster_resource.at(i)._resource._disk_max_total));
            printf("disk_free_total: %.2f(G)\r\n", BytesToGB(ack_message._cluster_resource.at(i)._resource._disk_free_total));
            printf("disk_free_max: %.2f(G)\r\n", BytesToGB(ack_message._cluster_resource.at(i)._resource._disk_free_max));

            printf("share_disk_max_total: %.2f(G)\r\n", BytesToGB(ack_message._cluster_resource.at(i)._resource._share_disk_max_total));
            printf("share_disk_free_total: %.2f(G)\r\n", BytesToGB(ack_message._cluster_resource.at(i)._resource._share_disk_free_total));

            printf("cpu_userate_average: %d\r\n", ack_message._cluster_resource.at(i)._resource._cpu_usage_average);
            printf("cpu_userate_max: %d\r\n", ack_message._cluster_resource.at(i)._resource._cpu_usage_max);
            printf("cpu_userate_min: %d\r\n", ack_message._cluster_resource.at(i)._resource._cpu_usage_min);
        }

        if (ack_message._cluster_resource.empty())
        {
            printf("no clusters!\r\n");
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("Dbg Request Recv Timeout!\n");
    }
}
DEFINE_DEBUG_FUNC(DbgShowAllCluster);
DEFINE_DEBUG_CMD(allcluster,DbgShowAllCluster);

void DbgSetIpConnect(char *ip)
{
    ClusterManager *cluster_manager = ClusterManager::GetInstance();
    if (!cluster_manager)
    {
        printf("GetInstance failed!\n");
        return;
    }

    if (!ip)
        printf("ip : %s\n", cluster_manager->GetIp().c_str());
    else
    {
        cluster_manager->SetIp(ip);
        printf("ip : %s\n", cluster_manager->GetIp().c_str());
    }
}
DEFINE_DEBUG_FUNC(DbgSetIpConnect);

void DbgShowAllIfsInfo()
{
    map<string, string> all_ifs_info;
    if (SUCCESS != GetAllIfsInfo(all_ifs_info))
    {
        printf("Get all interfaces info failed!\n");
        return;
    }
    map<string, string>::iterator it;
    for (it  = all_ifs_info.begin();
         it != all_ifs_info.end();
         it++)
    {
        printf("if %s ip : %s\n", it->first.c_str(), it->second.c_str());
    }
}
DEFINE_DEBUG_FUNC(DbgShowAllIfsInfo);

#define ClusterManagerDbgPrint(format...) \
{ \
    if (cluster_manager_dbg_flag == 1)   \
    {                       \
        printf(format);  \
    }                       \
}

ClusterManager  *ClusterManager::instance = NULL;

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
STATUS ClusterManager::StartMu(const string& name)
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

void ClusterManager::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
                case EV_POWER_ON:
                {
                    /* 设置集群老化定时器，该函数内部会控制，只是创建一次定时器 */
                    SetClusterKeepTimer();
                    string lastwords;
                    /* 设置组播组，只进入一次 */
                    STATUS ret = _mu->JoinMcGroup(MUTIGROUP_CLUSTER_DISCOVER);
                    if (SUCCESS != ret)
                    {
                        SkyAssert(false);
                        lastwords = "ClusterManager failed to join group! ret = ";
                        lastwords.append(to_string(ret,dec));
                        SkyExit(-1,lastwords);
                    }
                    ret = Transaction::Enable(GetDB());
                    if (SUCCESS != ret)
                    {
                        lastwords = "ClusterManager enable transaction failed! ret = ";
                        lastwords.append(to_string(ret,dec));
                        OutPut(SYS_ERROR, lastwords);
                        SkyExit(-1, lastwords);
                    }
                    /* TC 所在物理机重启后 CC 不会再发 DISCOVER 消息
                     * 也就不会开启 ntpd 和 dnsmasq 服务, 故此处增加处理
                     */
                    if (SUCCESS != NtpServiceRestart())
                    {
                        lastwords = "ClusterManager start ntpd service failed!";
                        OutPut(SYS_ERROR, lastwords);
                        SkyExit(-1, lastwords);
                    }
                    if (SUCCESS != DnsServiceRestart())
                    {
                        lastwords = "ClusterManager start dnsmasq service failed!";
                        OutPut(SYS_ERROR, lastwords);
                        SkyExit(-1, lastwords);
                    }
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
            /* CC向TC主动上报自己存在的消息 */
            case EV_CLUSTER_DISCOVER:
            {
                doClusterDiscover(message);
                break;
            }

            /* CC向TC上报集群资源状态消息 */
            case EV_CLUSTER_INFO_REPORT:
            {
                doClusterInfoReport(message);
                break;
            }

            /* agent向manager发送设置请求消息，取代原先手动注册 */
            case EV_CLUSTER_ACTION_SET_REQ:
            {           
                doAgentSetCluster(message);            
                break;
            }

            /* agent向manager发送删除注册请求 */
            case EV_CLUSTER_ACTION_FORGET_REQ:
            {
                doAgentForgetCluster(message);
                break;
            }

            /* agent向manager发送查询请求 */
            case EV_CLUSTER_ACTION_INFO_REQ:
            {
                doAgentQueryClusterInfo(message);
                break;
            }

            /* agent向manager发送集群预分配请求 */
            case EV_FILTER_CLUSTER_BY_COMPUTE_REQ:
            {
                doVmCfgFindCluster(message);
                break;
            }

            case EV_CLUSTER_ACTION_STOP_REQ:
            {
                doAgentStopCluster(message);
                break;
            }

            case EV_CLUSTER_ACTION_START_REQ:
            {
                doAgentStartCluster(message);
                break;
            }

            case EV_CLUSTER_ACTION_QUERY_CORE_TCU:
            {
                doAgentGetClusterCoreAndTcu(message);
                break;
            }

            case EV_CLUSTER_ACTION_QUERY_NETPLAN:
            {
                break;
            }

            case EV_CLUSTER_KEEP_STATE_TIMER:
            {
                time_t now_time;
                /* 获取当前的系统时间 */
                time(&now_time);
                /* 调用数据池的刷新接口进行数据内容刷新 */
                /* 距上次监控到的时间点的间隔是否超过保活的时间,
                 * 更新是否在线标志
                 */
                _cpool->RefreshCluster(now_time);
                break;
            }

            case EV_CLUSTER_QUERY_TIMEOUT_TIMER:
            {
                uint32 i;

                /* 只要其中一个定时器到了，所以当前资源是刚刚被更新过的，所以给全部等待的
                   AGENT回应答*/
                for (i = 0; i < _req_cluster_mid_tab.size(); i++)
                {
                    ClusterResourceAckSend(_req_cluster_mid_tab.at(i));
                }

                /* 清除MID的记录 */
                _req_cluster_mid_tab.clear();
                /* 清除未应答的集群名称记录 */
                _ClusterNameTab.clear();
                _req_cluster_timer = 0;
                break;
            }

            default:
            {
                OutPut(SYS_DEBUG, "ClusterManager receives a unkown message!");
                break;
            }
            }
            break;
        }

        default:
            break;
    }
}

/*****************************************************************************/
/**
@brief 功能描述: 为集群管理的发现流程

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doClusterDiscover(const MessageFrame& message)
{
    MID sender = message.option.sender();
    OutPut(SYS_DEBUG, "recv discover addr from %s\r\n", sender._application.c_str());

    /* 根据该集群发送来的信息，来查找该集群是否已经注册了 */
    Cluster *pCluster = _cpool->GetCluster(sender._application, false);
    if (NULL == pCluster)
    {
        if(ValidateLicense(LICOBJ_CLUSTERS, 1) != 0)
        {
            OutPut(SYS_ERROR, "license is invalid!");
            return;
        }
    
        /* 没有查找到，则进行申请 */
        int64 oidtemp = INVALID_OID;
        string err;    
        string cluster_append;        
        _cpool->Allocate(&oidtemp, sender._application, cluster_append, err);
        /* 再次通过集群的名称获取集群类的地址 */
        pCluster = _cpool->GetCluster(sender._application, false);
        if (NULL == pCluster)
        {
            OutPut(SYS_ERROR, "Creat Cluster Fail name is %s\r\n", sender._application.c_str());
            return;
        }
        /* notice: 上述获取记录均未上锁
         */
    }
    MessageClusterInfoReport Msg;
    Msg.deserialize(message.data);

    ClusterManagerDbgPrint("doClusterDiscover cluster verify ID: %s(tc db), %s(cc) \r\n ",
                           pCluster->_cluster_verfify_id.c_str(),
                           Msg._cluster_verify_id.c_str());

    if (!pCluster->_cluster_verfify_id.empty()
            && (pCluster->_cluster_verfify_id != Msg._cluster_verify_id))
    {
        ClusterManagerDbgPrint("doClusterDiscover different cluster verify ID detected!"
                               "Verify ID in db is %s,"
                               "but from cc is %s\r\n",
                               pCluster->_cluster_name.c_str(),
                               Msg._cluster_verify_id.c_str());
        if (true != pCluster->duplicate_name_exist())
        {
            pCluster->Alarm(ALM_TECS_CLUSTER_DUPLICATE_NAME);
            pCluster->set_cc_alarm_flag(Cluster::CC_DUPLICATE_NAME);
        }
        return;
    }
    if (true == pCluster->duplicate_name_exist())
    {
        pCluster->AlarmResume(ALM_TECS_CLUSTER_DUPLICATE_NAME);
        pCluster->clear_cc_alarm_flag(Cluster::CC_DUPLICATE_NAME);
    }

    /* 更新集群信息 */
    UpdateClusterInfo(sender._application, message);

    /* 发送一个REG请求给CC的HostManger */
    MessageClusterRegisterReq MsgReq;
    MID    dest;    
    MsgReq._name = pCluster->_cluster_name;

    if (_ifs_info.empty())
    {
        if (SUCCESS != GetAllIfsInfo(_ifs_info))
        {
            _ifs_info.clear();
        }
    }
    MsgReq._tc_ifs_info = _ifs_info;

    SetNtpServerConfig("register", "127.127.1.0", false); // 注意此处不是 127.0.0.1
    DnsServiceRestart();

    /* 向CC的hostmanager发送请求注册消息*/
    dest._application = pCluster->_cluster_name;
    dest._process = PROC_CC;
    dest._unit    = CLUSTER_CC_CLUSTER_AGENT_UNIT_NAME;
    MessageOption option(dest,EV_CLUSTER_REGISTER_REQ,0,0);
    _mu->Send(MsgReq, option);
}

void ClusterManager::UpdateClusterInfo(const string &cluster_name, 
                                       const MessageFrame& message)
{
    /* 下面检查的作用是上锁
     */
    Cluster *pCluster = _cpool->GetCluster(cluster_name, true);
    if (NULL == pCluster)
    {
        return;
    }

    /* 如果是离线状态就，改变状态，否则处于停止状态，则不修改任何状态 */
    if (pCluster->get_is_online() == false)
    {
        pCluster->set_is_online(true);
        OutPut(SYS_NOTICE, "cluster %s is online\r\n", pCluster->_cluster_name.c_str());
        /* 发告警恢复 */
        if (pCluster->offline_alarm_exist() == true)
        {
            pCluster->AlarmResume(ALM_TECS_CLUSTER_OFF_LINE);
            /* 清除告警标志 */
            pCluster->clear_cc_alarm_flag(Cluster::CC_OFFLINE_ALARM);
        }
    }
    pCluster->update_last_moni_time();    

    MessageClusterInfoReport Msg;
    Msg.deserialize(message.data);
    
    /* 更新资源池的资源情况,不使用结构体直接赋值，是为了保证兼容性 */
    pCluster->_cluster_resource._core_free_max = Msg._core_free_max;
    pCluster->_cluster_resource._tcu_unit_free_max = Msg._tcu_unit_free_max;

    pCluster->_cluster_resource._tcu_max_total = Msg._tcu_max_total;
    pCluster->_cluster_resource._tcu_free_total = Msg._tcu_free_total;
    pCluster->_cluster_resource._tcu_free_max = Msg._tcu_free_max;

    pCluster->_cluster_resource._disk_max_total = Msg._disk_max_total;
    pCluster->_cluster_resource._disk_free_total = Msg._disk_free_total;
    pCluster->_cluster_resource._disk_free_max = Msg._disk_free_max;


    pCluster->_cluster_resource._mem_max_total = Msg._mem_max_total;
    pCluster->_cluster_resource._mem_free_total = Msg._mem_free_total;
    pCluster->_cluster_resource._mem_free_max = Msg._mem_free_max;

    pCluster->_cluster_resource._cpu_usage_average = Msg._cpu_usage_average;
    pCluster->_cluster_resource._cpu_usage_max = Msg._cpu_usage_max;
    pCluster->_cluster_resource._cpu_usage_min = Msg._cpu_usage_min;
    //pCluster->_ip = Msg._ip_address;

    pCluster->_cluster_resource._db_name        = Msg._db_name;
    pCluster->_cluster_resource._db_server_url  = Msg._db_server_url;
    pCluster->_cluster_resource._db_server_port = Msg._db_server_port;
    pCluster->_cluster_resource._db_user        = Msg._db_user;
    pCluster->_cluster_resource._db_passwd      = Msg._db_passwd;

    //增加上报hostpool中host数量的信息
    pCluster->_cluster_resource._host_count      = Msg._host_count;

    pCluster->_cluster_resource._img_usage = Msg._img_usage;
    pCluster->_cluster_resource._img_srctype = Msg._img_srctype;
    pCluster->_cluster_resource._img_spcexp = Msg._img_spcexp;
    pCluster->_cluster_resource._share_img_usage = Msg._share_img_usage;

    //只在discover中将CC上传的验证码入库,report消息处理中不更新
    if(pCluster->_cluster_verfify_id.empty())
    {
        pCluster->_cluster_verfify_id      = Msg._cluster_verify_id;
    }


    pCluster->_cluster_node.clear();
    map<int, string>::iterator   node_iter;
    for (node_iter = Msg._node.begin();
         node_iter != Msg._node.end();
         node_iter++ )
    {
        ClusterNode cluster_node( pCluster->_cluster_name ,
                                    node_iter->second,
                                    node_iter->first);
        pCluster->_cluster_node.push_back(cluster_node);
    }

    /* 写入数据库 */
    _cpool->Update(pCluster);

    pCluster->UnLock();
}

/*****************************************************************************/
/**
@brief 功能描述: 用户设置集群描述的命令

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：取消手动注册功能，修改为设置描述接口
*/
/*****************************************************************************/
void ClusterManager::doAgentSetCluster(const MessageFrame& message)
{
    MessageClusterSetReq Msg;
    Msg.deserialize(message.data);

    MessageClusterSetAck MsgAck;

    MID sender = message.option.sender();
    MessageOption option(sender,EV_CLUSTER_ACTION_SET_ACK,0,0);

    OutPut(SYS_DEBUG, "recv action set req from %s\r\n", sender._unit.c_str());

    /* 根据该集群发送来的信息，来查找该集群是否已经注册了 */
    MsgAck._result = "Success";
    MsgAck._err_code = ERR_CM_SUCCESS;
    Cluster *pCluster = _cpool->GetCluster(Msg._name, true);
    if (NULL == pCluster)
    {
        /* 没有查找到，应答不存在 */
        MsgAck._result = "Cluster not exist";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_EXIST;
        _mu->Send(MsgAck, option);
        return;
    }

    /* 更新下备注信息 */
    pCluster->_description = Msg._cluster_append;
    _cpool->Update(pCluster);
    pCluster->UnLock();

    ClusterManagerDbgPrint("send req ack\r\n");
    /* 设置只需发应答消息给Agent */
    _mu->Send(MsgAck, option);

    return;
}

/*****************************************************************************/
/**
@brief 功能描述: 用户遗忘集群的命令

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doAgentForgetCluster(const MessageFrame& message)
{
    MessageClusterForgetReq Msg;
    Msg.deserialize(message.data);

    MessageClusterForgetAck MsgAck;
    
    MID     sender = message.option.sender();
    MessageOption option(sender,EV_CLUSTER_ACTION_FORGET_ACK,0,0);    

    OutPut(SYS_DEBUG, "recv action unreg req from %s\r\n", sender._application.c_str());

    /* 根据该集群发送来的信息，来查找该集群是否已经注册了 */
    Cluster *pCluster = _cpool->GetCluster(Msg._name, true);
    if (NULL == pCluster)
    {
        /* 集群删除命令，如果发现集群不存在，返回操作失败 */
        MsgAck._result = "cluster does not exist";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_EXIST;
        _mu->Send(MsgAck, option);
        return;
    }

    if (pCluster->is_free() == false)
    {
        /* 集群删除命令，如果发现集群资源没有释放完毕，则不释放集群 */
        MsgAck._result = "Cluster Resource is used";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_FREE;
    }
    else if (0 == _cpool->Drop(pCluster))
    {
        /* 回删除成功的应答消息给Agent */
        MsgAck._result = "Success";
        MsgAck._err_code = ERR_CM_SUCCESS;
    }
    else
    {
        MsgAck._result = "Cluster Resource is used";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_FREE;
    }

    pCluster->UnLock();
    _mu->Send(MsgAck, option);    
}

/*****************************************************************************/
/**
@brief 功能描述: 用户查询集群的命令

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doAgentQueryClusterInfo(const MessageFrame& message)
{
    MessageClusterInfoReq Msg;
    Msg.deserialize(message.data);
    MID sender = message.option.sender();

    ClusterResourceSynReq ReqInfo;
    ReqInfo._sender      = sender;
    ReqInfo._start_index = Msg._StartIndex;
    ReqInfo._query_count = Msg._Query_count;

    /* 异步模式 */
    if (Msg._ReqMode == CLUSTER_QUERY_ASYN)
    {
        ClusterResourceAckSend(ReqInfo);
    }
    else
    {

        if (Msg._StartIndex == 0)
        {   /* 第一包需要同步查，第2包数据，按异步来查询 */
            /* 此处的同步非消息同步, 而是指触发旗下各个集群重新上报自己的信息
             */
            ResourceReqSendToAllCluster(ReqInfo);
        }
        else
        {
            ClusterResourceAckSend(ReqInfo);
        }

    }
}

/*****************************************************************************/
/**
@brief 功能描述: 集群信息上报流程

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doClusterInfoReport(const MessageFrame& message)
{
    string cluster_name = message.option.sender()._application;

    MessageClusterInfoReport Msg;
    Msg.deserialize(message.data);

    Cluster *pCluster = _cpool->GetCluster(cluster_name, false);
    /* 根据该集群发送来的信息，来查找该集群是否已经注册了 */
    if (NULL == pCluster)
    {
        /* 没有查找到，说明CC已经被删除了，可能发给CC的注销消息丢失
           所以再次发送注销消息给CC*/
        MessageClusterUnRegisterReq MsgUnReg;
        MsgUnReg._name = cluster_name;
        MessageOption option1(message.option.sender(),EV_CLUSTER_UNREGISTER_REQ,0,0);
        _mu->Send(MsgUnReg, option1);
        /* 记录一条状态日志 */
        OutPut(SYS_DEBUG, "recv EV_CLUSTER_INFO_REPORT from %s, Cluster does not exist!\r\n",
                          cluster_name.c_str());
    }
    else
    {
        //消息处理中先判断下集群验证码是否正确
        ClusterManagerDbgPrint("doClusterDiscover cluster verify ID: %s(tc db), %s(cc) \r\n ",
                           pCluster->_cluster_verfify_id.c_str(),
                           Msg._cluster_verify_id.c_str());

        if (!pCluster->_cluster_verfify_id.empty()
              && (pCluster->_cluster_verfify_id != Msg._cluster_verify_id))
        {
            ClusterManagerDbgPrint("doClusterDiscover different cluster verify ID detected!"
                                   "Verify ID in db is %s,"
                                   "but from cc is %s\r\n",
                                   pCluster->_cluster_name.c_str(),
                                   Msg._cluster_verify_id.c_str());
            if (true != pCluster->duplicate_name_exist())
            {
                pCluster->Alarm(ALM_TECS_CLUSTER_DUPLICATE_NAME);                
                pCluster->set_cc_alarm_flag(Cluster::CC_DUPLICATE_NAME);
            }
            return;
        }
        if (true == pCluster->duplicate_name_exist())
        {        
            pCluster->AlarmResume(ALM_TECS_CLUSTER_DUPLICATE_NAME);
            pCluster->clear_cc_alarm_flag(Cluster::CC_DUPLICATE_NAME);
        }
        UpdateClusterInfo(cluster_name, message);
        ClusterManagerDbgPrint("recv EV_CLUSTER_INFO_REPORT from %s\r\n", cluster_name.c_str());

        /* 此处增加如下部分的目的:
         * 执行升级操作后, 因 CC 之前注册过, 不会再有 discover 消息
         * 导致新版本写死的 nfs_server(im.image.tecs) 和 IP 的映射
         * 关系写入到 /etc/hosts 中, 导致新部署虚拟机失败
         */
        if (_ip_connect_lowstream.empty())
        {// 此处限制所有CC的IP都对应TC的同一个IP
            string cc_ip;
            map<int, string>::iterator node_iter;
            for (node_iter  = Msg._node.begin();
                 node_iter != Msg._node.end();
                 node_iter++)
            {
                cc_ip = node_iter->second; // 目前仅有一个, 后续扩展需注意!!!!
            }
            if ((!cc_ip.compare("127.0.0.1"))
                || (SUCCESS != GetIpByConnect(cc_ip, _ip_connect_lowstream)))
            {
                _ip_connect_lowstream.clear();
            }
            SetNtpServerConfig("register", "127.127.1.0", false); // 注意此处不是 127.0.0.1
            SetDnsServerConfig("register", _ip_connect_lowstream, DNS_IM_IMAGE_TECS);
        }
    }

    /*==============如下CLUSTER_QUERY_SYN模式下才有效==============*/
    /* 清除缓存中本cluster的信息
     */
    _ClusterNameTab.erase(cluster_name);

    /* 如果集群资源名字被删除完了，表示所有的集群都回应答了 */
    if (_ClusterNameTab.empty())
    {
        uint32    i;    
        /* 给所有等待同步的Agent回应答 */
        for (i = 0; i < _req_cluster_mid_tab.size(); i++)
        {
            ClusterResourceAckSend(_req_cluster_mid_tab.at(i));
            ClusterManagerDbgPrint("send cluster info to agent\r\n");
        }

        /* i不等于0 说明是一次有效的应答发送 */
        if (i != 0)
        {
            /* 清除所有的MID记录 */
            _req_cluster_mid_tab.clear();
            /* 删除创建的定时器 */
            if (_req_cluster_timer != 0)
            {
                _mu->KillTimer(_req_cluster_timer);
                _req_cluster_timer = 0;
            }
        }
    }
    /*=============================================================*/
}

/*****************************************************************************/
/**
@brief 功能描述: 集群信息上报流程

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doVmCfgFindCluster(const MessageFrame& message)
{
    FilterClusterByComputeReq req;
    FilterClusterByComputeAck ack;

    bool ret;
	
    ret = req.deserialize(message.data);
    if(!ret)
    {
        SkyAssert(false);
    }
        
    ack.action_id= req.action_id;

    ostringstream   where;
    where << "core_free_max >= " << req._cpunum \
        <<" and " << "tcu_unit_free_max >= " << req._tcu_num \
        <<" and " << "tcu_free_max >= " << req._cpunum*req._tcu_num \
        <<" and "<<" mem_free_max >= "<< req._memsize \
        <<" and "<<" disk_free_max >= " <<req._disksize \
        <<" and "<<" is_online = " << true \
        <<" and "<<" enabled = " << true;

    //指定部署的时候，查询条件增加集群名一项
    if (!req._appointed_cluster.empty())
    {
        ClusterManagerDbgPrint(" cluster name got from req is %s \r\n",req._appointed_cluster.c_str());
        where<<" and "<<"name = '"<<req._appointed_cluster<<"'";
    }

    //调用在cluster_pool中定义的查询函数获取集群名列表
    _cpool->GetClusterNamesWithEnoughResource(where.str(), ack._cluster_list);

    if(ack._cluster_list.empty())
    {
        ClusterManagerDbgPrint(" cluster find fail,without enough resource \r\n");	
        ClusterManagerDbgPrint("query cpunum = %d, tcu = %d, mem = %ld," 
                      "disk = %ld, online = %d, enabled = %d, name = %s\r\n",
                      req._cpunum,
                      req._tcu_num,
                      req._memsize,
                      req._disksize,
                      true,
                      true,
                      req._appointed_cluster.c_str());

        ack.state = ERROR;
        ack.progress = 0;
        ack.detail = "find cluster failed, without enough resource";

    }
    else
    {
        ack.state = SUCCESS;
        ack.progress = 100;
    }

    /* 获取该请求消息的发送者地址 */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_FILTER_CLUSTER_BY_COMPUTE_ACK,0,0);
    _mu->Send(ack, option);
    ClusterManagerDbgPrint("send action find ack to %s\r\n", sender._unit.c_str());
}

/*****************************************************************************/
/**
@brief 功能描述: 用户停止集群的命令(进入维护模式)

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doAgentStopCluster(const MessageFrame& message)
{
    MessageClusterRunStateModifyReq Msg;
    MessageClusterRunStateModifyAck AckMsg;
    Cluster *pCluster = NULL;

    Msg.deserialize(message.data);

    pCluster = _cpool->GetCluster(Msg._name, true);
    if (NULL == pCluster)
    {
        AckMsg._result = "Cluster does not exist";
        AckMsg._err_code = ERR_CM_CLUSTER_NOT_EXIST;
    }
    else
    {
        /* 如果收到让集群进入维护态的消息，如果没有进入维护态，则进入 */
        if (pCluster->get_enabled() == true)
        {
            AckMsg._result = "Success";
            AckMsg._err_code = ERR_CM_SUCCESS;
            pCluster->set_enabled(false);
            _cpool->Update(pCluster);
            OutPut(SYS_NOTICE, "cluster %s in PRESERVE\r\n", pCluster->_cluster_name.c_str());
        }
        else
        {   /* 否则回应答给后台，告诉已经进入维护态了 */
            AckMsg._result = "Cluster Current State is preserve!";
            AckMsg._err_code = ERR_CM_SUCCESS;
        }
        pCluster->UnLock();
    }
    /* 获取该请求消息的发送者地址 */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_CLUSTER_ACTION_STOP_ACK,0,0);
    _mu->Send(AckMsg, option);
    ClusterManagerDbgPrint("send action PRESERVE ack to %s\r\n", sender._unit.c_str());
}

/*****************************************************************************/
/**
@brief 功能描述: 用户启动集群的命令(退出维护模式)

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doAgentStartCluster(const MessageFrame& message)
{
    MessageClusterRunStateModifyReq Msg;
    MessageClusterRunStateModifyAck AckMsg;
    Cluster *pCluster = NULL;

    Msg.deserialize(message.data);

    pCluster = _cpool->GetCluster(Msg._name, true);
    if (NULL == pCluster)
    {
        AckMsg._result = "Cluster does not exist";
        AckMsg._err_code = ERR_CM_CLUSTER_NOT_EXIST;
    }
    else
    {
        /* 如果集群当前状态是停止态，则修改状态为离线态 */
        if (pCluster->get_enabled() == false)
        {
            pCluster->set_enabled(true);
            AckMsg._result = "Success";
            AckMsg._err_code = ERR_CM_SUCCESS;
            _cpool->Update(pCluster);
            OutPut(SYS_NOTICE, "cluster %s is offline from PRESERVE\r\n", pCluster->_cluster_name.c_str());
        }
        else
        {   /* 否则给后台回个当前集群不在维护态的应答 */
            AckMsg._result = "Cluster Current State is not preserve!";
            AckMsg._err_code = ERR_CM_SUCCESS;
        }
        pCluster->UnLock();
    }
    /* 获取该请求消息的发送者地址 */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_CLUSTER_ACTION_START_ACK,0,0);
    _mu->Send(AckMsg, option);
    ClusterManagerDbgPrint("send action start ack to %s\r\n", sender._unit.c_str());
}

/*****************************************************************************/
/**
@brief 功能描述: 用户获取集群的核和TCU的最大值

@li @b 入参列表：
@verbatim
sender    无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::doAgentGetClusterCoreAndTcu(const MessageFrame& message)
{
    int    core_max = 0;
    int    tcu_max = 0;

    core_max = _cpool->FindHcMaxCore();
    tcu_max = _cpool->FindHcMaxTcu();

    MessageClusterQueryCoreTcuAck AckInfo(core_max, tcu_max);

    /* 获取该请求消息的发送者地址 */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_CLUSTER_ACTION_QUERY_CORE_TCU_ACK,0,0);
    _mu->Send(AckInfo, option);
    ClusterManagerDbgPrint("send request core and tcu ack to %s\r\n", sender._unit.c_str());
}


/*****************************************************************************/
/**
@brief 功能描述: 为集群管理进程设置一个循环定时器，

@li @b 入参列表：
@verbatim
sender    发送请求消息的请求者地址
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：该函数调用多次，也只会设置一次循环定时器
*/
/*****************************************************************************/
void ClusterManager::SetClusterKeepTimer()
{
    TimeOut timeout;

    if (_manager_period_timer != INVALID_TIMER_ID)
    {
        return;
    }

    _manager_period_timer = _mu->CreateTimer();
    timeout.type = LOOP_TIMER;
    timeout.duration = CLUSTER_PERIOD_TIME_LEN;
    timeout.msgid = EV_CLUSTER_KEEP_STATE_TIMER;
    if (SUCCESS != _mu->SetTimer(_manager_period_timer,timeout))
    {
        OutPut(SYS_EMERGENCY, "ClusterManager SetLoopTime Create timer fail!\n");
        SkyAssert(0);
        SkyExit(-1, "ClusterManager::SetLoopTime: call mu->SetTimer(_manager_period_timer,timeout) failed.");
    }
}


/*****************************************************************************/
/**
@brief 功能描述: 集群资源查询应答发送函数

@li @b 入参列表：
@verbatim
sender    发送请求消息的请求者地址
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::ClusterResourceAckSend(ClusterResourceSynReq &ReqInfo)
{
    vector<string> ClusterNameTab;
    MessageClusterInfoAck MsgAck;   /* 回应答的消息 */
    uint32          i;
    ClusterInfo    info;
    Cluster        *pCluster = NULL;
    int64          out_count;
    int64          start_index;
    int64          query_count;
    MID            sender;

    sender      = ReqInfo._sender;
    start_index = ReqInfo._start_index;
    query_count = ReqInfo._query_count;

    _cpool->GetAllClusterName(ClusterNameTab);

    MsgAck._max_count = ClusterNameTab.size();
    if ((uint64)(start_index + query_count) >= ClusterNameTab.size())
    {
        out_count = ClusterNameTab.size() - start_index;
        MsgAck._next_index = -1;
    }
    else
    {
        out_count = query_count;
        MsgAck._next_index = start_index+out_count;
    }

    for (i = start_index; i < (start_index + out_count); i++)
    {
        /* 使用当前数据库的内容回 */
        pCluster = _cpool->GetCluster(ClusterNameTab.at(i),false);

        info.clear();
        /* base info: name/ip/description
         */
        pCluster->get_name(&info._cluster_name);
        pCluster->get_ip(info._ip);
        pCluster->get_description(&info._cluster_append);
        /* flags:
         *       online: 1, offline: 0
         *       enable: 1(非维护), enable: 0(维护)
         */
        info._is_online = pCluster->get_is_online();
        info._enabled = pCluster->get_enabled();
        /* resources: tcu/mem/disk等
         */
        pCluster->get_cluster_resource(info._resource);
        MsgAck._cluster_resource.push_back(info);
    }

    /* 直接把消息返回到Agent上去 */
    MessageOption option(sender,EV_CLUSTER_ACTION_INFO_ACK,0,0);
    _mu->Send(MsgAck, option);
}

/*****************************************************************************/
/**
@brief 功能描述: 集群管理进程给所有CC发送集群资源上报请求

@li @b 入参列表：
@verbatim
sender    发送请求消息的请求者地址
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterManager::ResourceReqSendToAllCluster(ClusterResourceSynReq &ReqInfo)
{
    MessageClusterInfoReq Msg;         /* 收到的请求消息 */
    Cluster *pCluster = NULL;
    vector<string> ClusterNameTab;
    uint32          i;
    MID            dest;
    MessageOption  option(dest,EV_CLUSTER_INFO_REQ,0,0);
    TimeOut        timeout;

    dest._unit    = CLUSTER_CC_CLUSTER_AGENT_UNIT_NAME;

    /* 记录当前的Sender到管理结构中去 */
    _req_cluster_mid_tab.push_back(ReqInfo);

    /* 如果当前资源组被加入了，则不进行下面发送和设置定时器 */
    if (!_ClusterNameTab.empty())
    {
        return;
    }

    /* 查找所有的集群名称 */
    _cpool->GetAllClusterName(ClusterNameTab);

    /* 把请求资源刷新的信息全部发送到各个集群上去 */
    for (i = 0; i < ClusterNameTab.size(); i++)
    {
        /* 把消息发送到所有集群中去 */
        pCluster = _cpool->GetCluster(ClusterNameTab.at(i),false);
        dest._application = pCluster->_cluster_name;
        dest._process = PROC_CC;

        option.setReceiver(dest);
        _mu->Send(Msg, option);
    }
    /* 设置一个定时器，等待各个CC在时间到点前给cluster_agent上报资源 */
    _req_cluster_timer = _mu->CreateTimer();
    timeout.duration = 2000;
    timeout.msgid = EV_CLUSTER_QUERY_TIMEOUT_TIMER;
    if (SUCCESS != _mu->SetTimer(_req_cluster_timer,timeout))
    {
        OutPut(SYS_EMERGENCY, "ResourceReqSendToAllCluster Create timer fail!\n");
        SkyAssert(0);
        SkyExit(-1, "ClusterManager::ResourceReqSendToAllCluster: call mu->SetTimer failed.");
    }

    /* 把集群的名称初次加入到映射表中 */
    if (_ClusterNameTab.empty())
    {
        for (i = 0; i < ClusterNameTab.size(); i++)
        {
            _ClusterNameTab.insert(make_pair(ClusterNameTab.at(i),i));
        }
    }
}

} //end namespace zte_tecs

