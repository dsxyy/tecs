/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�cluster_manager.cpp
* �ļ���ʶ��
* ����ժҪ��cluster_manager��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���Т��
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ���Т��
*     �޸����ݣ�����
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
/* ��ʱ���¼����ǲ���Ҫ�����˿������¼�����ÿ��ģ����������Էŵ�ʹ�ÿռ��� */
#define EV_CLUSTER_KEEP_STATE_TIMER     EV_TIMER_1     /* ��Ⱥ״̬ˢ�¶�ʱ�� */
#define EV_CLUSTER_QUERY_TIMEOUT_TIMER  EV_TIMER_2     /* ��Ⱥ��Ϣͬ����ѯ��ʱ�� */

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

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("Cluste_Manager_Dbg_Agent"));
    temp_mu.Register();

    /* 3. ����Ϣ�� Cluster_Manager  */
    MessageClusterInfoReq ReqMsg(0);
    ReqMsg._StartIndex  = 0;
    ReqMsg._Query_count = 2000;
    MessageOption option(MU_CLUSTER_MANAGER, EV_CLUSTER_ACTION_INFO_REQ,0,0);
    temp_mu.Send(ReqMsg, option);

    /* 4. �ȴ�Cluster_MANAGER��Ӧ */
    MessageFrame message;
    if (SUCCESS == temp_mu.Wait(&message,2000))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
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
        /*  ��ʱ or �������� ? */
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
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS ClusterManager::StartMu(const string& name)
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
                    /* ���ü�Ⱥ�ϻ���ʱ�����ú����ڲ�����ƣ�ֻ�Ǵ���һ�ζ�ʱ�� */
                    SetClusterKeepTimer();
                    string lastwords;
                    /* �����鲥�飬ֻ����һ�� */
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
                    /* TC ��������������� CC �����ٷ� DISCOVER ��Ϣ
                     * Ҳ�Ͳ��Ὺ�� ntpd �� dnsmasq ����, �ʴ˴����Ӵ���
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
            /* CC��TC�����ϱ��Լ����ڵ���Ϣ */
            case EV_CLUSTER_DISCOVER:
            {
                doClusterDiscover(message);
                break;
            }

            /* CC��TC�ϱ���Ⱥ��Դ״̬��Ϣ */
            case EV_CLUSTER_INFO_REPORT:
            {
                doClusterInfoReport(message);
                break;
            }

            /* agent��manager��������������Ϣ��ȡ��ԭ���ֶ�ע�� */
            case EV_CLUSTER_ACTION_SET_REQ:
            {           
                doAgentSetCluster(message);            
                break;
            }

            /* agent��manager����ɾ��ע������ */
            case EV_CLUSTER_ACTION_FORGET_REQ:
            {
                doAgentForgetCluster(message);
                break;
            }

            /* agent��manager���Ͳ�ѯ���� */
            case EV_CLUSTER_ACTION_INFO_REQ:
            {
                doAgentQueryClusterInfo(message);
                break;
            }

            /* agent��manager���ͼ�ȺԤ�������� */
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
                /* ��ȡ��ǰ��ϵͳʱ�� */
                time(&now_time);
                /* �������ݳص�ˢ�½ӿڽ�����������ˢ�� */
                /* ���ϴμ�ص���ʱ���ļ���Ƿ񳬹������ʱ��,
                 * �����Ƿ����߱�־
                 */
                _cpool->RefreshCluster(now_time);
                break;
            }

            case EV_CLUSTER_QUERY_TIMEOUT_TIMER:
            {
                uint32 i;

                /* ֻҪ����һ����ʱ�����ˣ����Ե�ǰ��Դ�Ǹոձ����¹��ģ����Ը�ȫ���ȴ���
                   AGENT��Ӧ��*/
                for (i = 0; i < _req_cluster_mid_tab.size(); i++)
                {
                    ClusterResourceAckSend(_req_cluster_mid_tab.at(i));
                }

                /* ���MID�ļ�¼ */
                _req_cluster_mid_tab.clear();
                /* ���δӦ��ļ�Ⱥ���Ƽ�¼ */
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
@brief ��������: Ϊ��Ⱥ����ķ�������

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
void ClusterManager::doClusterDiscover(const MessageFrame& message)
{
    MID sender = message.option.sender();
    OutPut(SYS_DEBUG, "recv discover addr from %s\r\n", sender._application.c_str());

    /* ���ݸü�Ⱥ����������Ϣ�������Ҹü�Ⱥ�Ƿ��Ѿ�ע���� */
    Cluster *pCluster = _cpool->GetCluster(sender._application, false);
    if (NULL == pCluster)
    {
        if(ValidateLicense(LICOBJ_CLUSTERS, 1) != 0)
        {
            OutPut(SYS_ERROR, "license is invalid!");
            return;
        }
    
        /* û�в��ҵ������������ */
        int64 oidtemp = INVALID_OID;
        string err;    
        string cluster_append;        
        _cpool->Allocate(&oidtemp, sender._application, cluster_append, err);
        /* �ٴ�ͨ����Ⱥ�����ƻ�ȡ��Ⱥ��ĵ�ַ */
        pCluster = _cpool->GetCluster(sender._application, false);
        if (NULL == pCluster)
        {
            OutPut(SYS_ERROR, "Creat Cluster Fail name is %s\r\n", sender._application.c_str());
            return;
        }
        /* notice: ������ȡ��¼��δ����
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

    /* ���¼�Ⱥ��Ϣ */
    UpdateClusterInfo(sender._application, message);

    /* ����һ��REG�����CC��HostManger */
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

    SetNtpServerConfig("register", "127.127.1.0", false); // ע��˴����� 127.0.0.1
    DnsServiceRestart();

    /* ��CC��hostmanager��������ע����Ϣ*/
    dest._application = pCluster->_cluster_name;
    dest._process = PROC_CC;
    dest._unit    = CLUSTER_CC_CLUSTER_AGENT_UNIT_NAME;
    MessageOption option(dest,EV_CLUSTER_REGISTER_REQ,0,0);
    _mu->Send(MsgReq, option);
}

void ClusterManager::UpdateClusterInfo(const string &cluster_name, 
                                       const MessageFrame& message)
{
    /* �����������������
     */
    Cluster *pCluster = _cpool->GetCluster(cluster_name, true);
    if (NULL == pCluster)
    {
        return;
    }

    /* ���������״̬�ͣ��ı�״̬��������ֹͣ״̬�����޸��κ�״̬ */
    if (pCluster->get_is_online() == false)
    {
        pCluster->set_is_online(true);
        OutPut(SYS_NOTICE, "cluster %s is online\r\n", pCluster->_cluster_name.c_str());
        /* ���澯�ָ� */
        if (pCluster->offline_alarm_exist() == true)
        {
            pCluster->AlarmResume(ALM_TECS_CLUSTER_OFF_LINE);
            /* ����澯��־ */
            pCluster->clear_cc_alarm_flag(Cluster::CC_OFFLINE_ALARM);
        }
    }
    pCluster->update_last_moni_time();    

    MessageClusterInfoReport Msg;
    Msg.deserialize(message.data);
    
    /* ������Դ�ص���Դ���,��ʹ�ýṹ��ֱ�Ӹ�ֵ����Ϊ�˱�֤������ */
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

    //�����ϱ�hostpool��host��������Ϣ
    pCluster->_cluster_resource._host_count      = Msg._host_count;

    pCluster->_cluster_resource._img_usage = Msg._img_usage;
    pCluster->_cluster_resource._img_srctype = Msg._img_srctype;
    pCluster->_cluster_resource._img_spcexp = Msg._img_spcexp;
    pCluster->_cluster_resource._share_img_usage = Msg._share_img_usage;

    //ֻ��discover�н�CC�ϴ�����֤�����,report��Ϣ�����в�����
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

    /* д�����ݿ� */
    _cpool->Update(pCluster);

    pCluster->UnLock();
}

/*****************************************************************************/
/**
@brief ��������: �û����ü�Ⱥ����������

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵����ȡ���ֶ�ע�Ṧ�ܣ��޸�Ϊ���������ӿ�
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

    /* ���ݸü�Ⱥ����������Ϣ�������Ҹü�Ⱥ�Ƿ��Ѿ�ע���� */
    MsgAck._result = "Success";
    MsgAck._err_code = ERR_CM_SUCCESS;
    Cluster *pCluster = _cpool->GetCluster(Msg._name, true);
    if (NULL == pCluster)
    {
        /* û�в��ҵ���Ӧ�𲻴��� */
        MsgAck._result = "Cluster not exist";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_EXIST;
        _mu->Send(MsgAck, option);
        return;
    }

    /* �����±�ע��Ϣ */
    pCluster->_description = Msg._cluster_append;
    _cpool->Update(pCluster);
    pCluster->UnLock();

    ClusterManagerDbgPrint("send req ack\r\n");
    /* ����ֻ�跢Ӧ����Ϣ��Agent */
    _mu->Send(MsgAck, option);

    return;
}

/*****************************************************************************/
/**
@brief ��������: �û�������Ⱥ������

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
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

    /* ���ݸü�Ⱥ����������Ϣ�������Ҹü�Ⱥ�Ƿ��Ѿ�ע���� */
    Cluster *pCluster = _cpool->GetCluster(Msg._name, true);
    if (NULL == pCluster)
    {
        /* ��Ⱥɾ�����������ּ�Ⱥ�����ڣ����ز���ʧ�� */
        MsgAck._result = "cluster does not exist";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_EXIST;
        _mu->Send(MsgAck, option);
        return;
    }

    if (pCluster->is_free() == false)
    {
        /* ��Ⱥɾ�����������ּ�Ⱥ��Դû���ͷ���ϣ����ͷż�Ⱥ */
        MsgAck._result = "Cluster Resource is used";
        MsgAck._err_code = ERR_CM_CLUSTER_NOT_FREE;
    }
    else if (0 == _cpool->Drop(pCluster))
    {
        /* ��ɾ���ɹ���Ӧ����Ϣ��Agent */
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
@brief ��������: �û���ѯ��Ⱥ������

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
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

    /* �첽ģʽ */
    if (Msg._ReqMode == CLUSTER_QUERY_ASYN)
    {
        ClusterResourceAckSend(ReqInfo);
    }
    else
    {

        if (Msg._StartIndex == 0)
        {   /* ��һ����Ҫͬ���飬��2�����ݣ����첽����ѯ */
            /* �˴���ͬ������Ϣͬ��, ����ָ�������¸�����Ⱥ�����ϱ��Լ�����Ϣ
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
@brief ��������: ��Ⱥ��Ϣ�ϱ�����

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
void ClusterManager::doClusterInfoReport(const MessageFrame& message)
{
    string cluster_name = message.option.sender()._application;

    MessageClusterInfoReport Msg;
    Msg.deserialize(message.data);

    Cluster *pCluster = _cpool->GetCluster(cluster_name, false);
    /* ���ݸü�Ⱥ����������Ϣ�������Ҹü�Ⱥ�Ƿ��Ѿ�ע���� */
    if (NULL == pCluster)
    {
        /* û�в��ҵ���˵��CC�Ѿ���ɾ���ˣ����ܷ���CC��ע����Ϣ��ʧ
           �����ٴη���ע����Ϣ��CC*/
        MessageClusterUnRegisterReq MsgUnReg;
        MsgUnReg._name = cluster_name;
        MessageOption option1(message.option.sender(),EV_CLUSTER_UNREGISTER_REQ,0,0);
        _mu->Send(MsgUnReg, option1);
        /* ��¼һ��״̬��־ */
        OutPut(SYS_DEBUG, "recv EV_CLUSTER_INFO_REPORT from %s, Cluster does not exist!\r\n",
                          cluster_name.c_str());
    }
    else
    {
        //��Ϣ���������ж��¼�Ⱥ��֤���Ƿ���ȷ
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

        /* �˴��������²��ֵ�Ŀ��:
         * ִ������������, �� CC ֮ǰע���, �������� discover ��Ϣ
         * �����°汾д���� nfs_server(im.image.tecs) �� IP ��ӳ��
         * ��ϵд�뵽 /etc/hosts ��, �����²��������ʧ��
         */
        if (_ip_connect_lowstream.empty())
        {// �˴���������CC��IP����ӦTC��ͬһ��IP
            string cc_ip;
            map<int, string>::iterator node_iter;
            for (node_iter  = Msg._node.begin();
                 node_iter != Msg._node.end();
                 node_iter++)
            {
                cc_ip = node_iter->second; // Ŀǰ����һ��, ������չ��ע��!!!!
            }
            if ((!cc_ip.compare("127.0.0.1"))
                || (SUCCESS != GetIpByConnect(cc_ip, _ip_connect_lowstream)))
            {
                _ip_connect_lowstream.clear();
            }
            SetNtpServerConfig("register", "127.127.1.0", false); // ע��˴����� 127.0.0.1
            SetDnsServerConfig("register", _ip_connect_lowstream, DNS_IM_IMAGE_TECS);
        }
    }

    /*==============����CLUSTER_QUERY_SYNģʽ�²���Ч==============*/
    /* ��������б�cluster����Ϣ
     */
    _ClusterNameTab.erase(cluster_name);

    /* �����Ⱥ��Դ���ֱ�ɾ�����ˣ���ʾ���еļ�Ⱥ����Ӧ���� */
    if (_ClusterNameTab.empty())
    {
        uint32    i;    
        /* �����еȴ�ͬ����Agent��Ӧ�� */
        for (i = 0; i < _req_cluster_mid_tab.size(); i++)
        {
            ClusterResourceAckSend(_req_cluster_mid_tab.at(i));
            ClusterManagerDbgPrint("send cluster info to agent\r\n");
        }

        /* i������0 ˵����һ����Ч��Ӧ���� */
        if (i != 0)
        {
            /* ������е�MID��¼ */
            _req_cluster_mid_tab.clear();
            /* ɾ�������Ķ�ʱ�� */
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
@brief ��������: ��Ⱥ��Ϣ�ϱ�����

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
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

    //ָ�������ʱ�򣬲�ѯ�������Ӽ�Ⱥ��һ��
    if (!req._appointed_cluster.empty())
    {
        ClusterManagerDbgPrint(" cluster name got from req is %s \r\n",req._appointed_cluster.c_str());
        where<<" and "<<"name = '"<<req._appointed_cluster<<"'";
    }

    //������cluster_pool�ж���Ĳ�ѯ������ȡ��Ⱥ���б�
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

    /* ��ȡ��������Ϣ�ķ����ߵ�ַ */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_FILTER_CLUSTER_BY_COMPUTE_ACK,0,0);
    _mu->Send(ack, option);
    ClusterManagerDbgPrint("send action find ack to %s\r\n", sender._unit.c_str());
}

/*****************************************************************************/
/**
@brief ��������: �û�ֹͣ��Ⱥ������(����ά��ģʽ)

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
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
        /* ����յ��ü�Ⱥ����ά��̬����Ϣ�����û�н���ά��̬������� */
        if (pCluster->get_enabled() == true)
        {
            AckMsg._result = "Success";
            AckMsg._err_code = ERR_CM_SUCCESS;
            pCluster->set_enabled(false);
            _cpool->Update(pCluster);
            OutPut(SYS_NOTICE, "cluster %s in PRESERVE\r\n", pCluster->_cluster_name.c_str());
        }
        else
        {   /* �����Ӧ�����̨�������Ѿ�����ά��̬�� */
            AckMsg._result = "Cluster Current State is preserve!";
            AckMsg._err_code = ERR_CM_SUCCESS;
        }
        pCluster->UnLock();
    }
    /* ��ȡ��������Ϣ�ķ����ߵ�ַ */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_CLUSTER_ACTION_STOP_ACK,0,0);
    _mu->Send(AckMsg, option);
    ClusterManagerDbgPrint("send action PRESERVE ack to %s\r\n", sender._unit.c_str());
}

/*****************************************************************************/
/**
@brief ��������: �û�������Ⱥ������(�˳�ά��ģʽ)

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
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
        /* �����Ⱥ��ǰ״̬��ֹ̬ͣ�����޸�״̬Ϊ����̬ */
        if (pCluster->get_enabled() == false)
        {
            pCluster->set_enabled(true);
            AckMsg._result = "Success";
            AckMsg._err_code = ERR_CM_SUCCESS;
            _cpool->Update(pCluster);
            OutPut(SYS_NOTICE, "cluster %s is offline from PRESERVE\r\n", pCluster->_cluster_name.c_str());
        }
        else
        {   /* �������̨�ظ���ǰ��Ⱥ����ά��̬��Ӧ�� */
            AckMsg._result = "Cluster Current State is not preserve!";
            AckMsg._err_code = ERR_CM_SUCCESS;
        }
        pCluster->UnLock();
    }
    /* ��ȡ��������Ϣ�ķ����ߵ�ַ */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_CLUSTER_ACTION_START_ACK,0,0);
    _mu->Send(AckMsg, option);
    ClusterManagerDbgPrint("send action start ack to %s\r\n", sender._unit.c_str());
}

/*****************************************************************************/
/**
@brief ��������: �û���ȡ��Ⱥ�ĺ˺�TCU�����ֵ

@li @b ����б�
@verbatim
sender    ��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
void ClusterManager::doAgentGetClusterCoreAndTcu(const MessageFrame& message)
{
    int    core_max = 0;
    int    tcu_max = 0;

    core_max = _cpool->FindHcMaxCore();
    tcu_max = _cpool->FindHcMaxTcu();

    MessageClusterQueryCoreTcuAck AckInfo(core_max, tcu_max);

    /* ��ȡ��������Ϣ�ķ����ߵ�ַ */
    MID sender = message.option.sender();

    MessageOption option(sender,EV_CLUSTER_ACTION_QUERY_CORE_TCU_ACK,0,0);
    _mu->Send(AckInfo, option);
    ClusterManagerDbgPrint("send request core and tcu ack to %s\r\n", sender._unit.c_str());
}


/*****************************************************************************/
/**
@brief ��������: Ϊ��Ⱥ�����������һ��ѭ����ʱ����

@li @b ����б�
@verbatim
sender    ����������Ϣ�������ߵ�ַ
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵�����ú������ö�Σ�Ҳֻ������һ��ѭ����ʱ��
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
@brief ��������: ��Ⱥ��Դ��ѯӦ���ͺ���

@li @b ����б�
@verbatim
sender    ����������Ϣ�������ߵ�ַ
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
void ClusterManager::ClusterResourceAckSend(ClusterResourceSynReq &ReqInfo)
{
    vector<string> ClusterNameTab;
    MessageClusterInfoAck MsgAck;   /* ��Ӧ�����Ϣ */
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
        /* ʹ�õ�ǰ���ݿ�����ݻ� */
        pCluster = _cpool->GetCluster(ClusterNameTab.at(i),false);

        info.clear();
        /* base info: name/ip/description
         */
        pCluster->get_name(&info._cluster_name);
        pCluster->get_ip(info._ip);
        pCluster->get_description(&info._cluster_append);
        /* flags:
         *       online: 1, offline: 0
         *       enable: 1(��ά��), enable: 0(ά��)
         */
        info._is_online = pCluster->get_is_online();
        info._enabled = pCluster->get_enabled();
        /* resources: tcu/mem/disk��
         */
        pCluster->get_cluster_resource(info._resource);
        MsgAck._cluster_resource.push_back(info);
    }

    /* ֱ�Ӱ���Ϣ���ص�Agent��ȥ */
    MessageOption option(sender,EV_CLUSTER_ACTION_INFO_ACK,0,0);
    _mu->Send(MsgAck, option);
}

/*****************************************************************************/
/**
@brief ��������: ��Ⱥ������̸�����CC���ͼ�Ⱥ��Դ�ϱ�����

@li @b ����б�
@verbatim
sender    ����������Ϣ�������ߵ�ַ
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
void ClusterManager::ResourceReqSendToAllCluster(ClusterResourceSynReq &ReqInfo)
{
    MessageClusterInfoReq Msg;         /* �յ���������Ϣ */
    Cluster *pCluster = NULL;
    vector<string> ClusterNameTab;
    uint32          i;
    MID            dest;
    MessageOption  option(dest,EV_CLUSTER_INFO_REQ,0,0);
    TimeOut        timeout;

    dest._unit    = CLUSTER_CC_CLUSTER_AGENT_UNIT_NAME;

    /* ��¼��ǰ��Sender������ṹ��ȥ */
    _req_cluster_mid_tab.push_back(ReqInfo);

    /* �����ǰ��Դ�鱻�����ˣ��򲻽������淢�ͺ����ö�ʱ�� */
    if (!_ClusterNameTab.empty())
    {
        return;
    }

    /* �������еļ�Ⱥ���� */
    _cpool->GetAllClusterName(ClusterNameTab);

    /* ��������Դˢ�µ���Ϣȫ�����͵�������Ⱥ��ȥ */
    for (i = 0; i < ClusterNameTab.size(); i++)
    {
        /* ����Ϣ���͵����м�Ⱥ��ȥ */
        pCluster = _cpool->GetCluster(ClusterNameTab.at(i),false);
        dest._application = pCluster->_cluster_name;
        dest._process = PROC_CC;

        option.setReceiver(dest);
        _mu->Send(Msg, option);
    }
    /* ����һ����ʱ�����ȴ�����CC��ʱ�䵽��ǰ��cluster_agent�ϱ���Դ */
    _req_cluster_timer = _mu->CreateTimer();
    timeout.duration = 2000;
    timeout.msgid = EV_CLUSTER_QUERY_TIMEOUT_TIMER;
    if (SUCCESS != _mu->SetTimer(_req_cluster_timer,timeout))
    {
        OutPut(SYS_EMERGENCY, "ResourceReqSendToAllCluster Create timer fail!\n");
        SkyAssert(0);
        SkyExit(-1, "ClusterManager::ResourceReqSendToAllCluster: call mu->SetTimer failed.");
    }

    /* �Ѽ�Ⱥ�����Ƴ��μ��뵽ӳ����� */
    if (_ClusterNameTab.empty())
    {
        for (i = 0; i < ClusterNameTab.size(); i++)
        {
            _ClusterNameTab.insert(make_pair(ClusterNameTab.at(i),i));
        }
    }
}

} //end namespace zte_tecs

