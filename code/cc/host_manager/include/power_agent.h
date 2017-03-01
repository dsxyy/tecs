/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：power_agent.h
* 文件标识：见配置管理计划书
* 内容摘要：PowerAgent类定义文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年5月9日
*
* 修改记录1：
*     修改日期：2013/5/9
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#ifndef HM_POWER_AGENT_H
#define HM_POWER_AGENT_H
#include "sky.h"
#include "mid.h"
#include "log_agent.h"
#include "host_pool.h" 
#include "config_cmm_pool.h"
#include "config_host_atca.h"
#include "event.h"
#include "msg_power_agent_with_high_available.h"
#include "msg_power_manager_with_api_method.h"

namespace zte_tecs 
{
    

#define M0_NotInstall            0   // 未安装
#define M1_Inactive              1   // 不活动, 就是没有加电
#define M2_ActivationRequest     2   // 激活请求, 已经收到上电请求了
#define M3_ActivationInProcess   3   // 激活中
#define M4_Active                4   // 活动， 这是正常的工作状态
#define M5_DeactivationRequest   5   // 去激活请求，关闭电源
#define M6_DeactivationInProcess 6   // 去激活中
#define M7_CommunicationLost     7   // 通信丢失，一般是单板被拔出或
#define State_Unknown            8   // 这是异常情况，例如：CMM不在位或IP地址错误等问题导致的状态无法查询

#define   FRUID             " 0x3e "  // 0x3e 表示前插 0x56 表示后插 , 目前不支持后插
#define   IPMITOOL_PARA     " ipmitool -I lan -H " 
#define   CMM_UESR_PASSWORD " -U zte -P zte -t "
#define   IPMITOOL_CMD      " raw " 
#define   HOST_QUERY_CMD    " 0x04 0x2d " 
//#define   HOST_RESET_CMD    " 0x34 0x33 " 
//#define   HOST_POWERON_CMD  " 0x2c 0xa 0x00 " 
//#define   HOST_POWEROFF_CMD " 0x2c 0x0c 0x00 " 
#define   HOST_RESET_CMD    " 0x34 0x33 0xff " 
#define   HOST_POWERON_CMD  " 0x2c 0xa 0 0 1 0 " 
#define   HOST_POWEROFF_CMD " 0x2c 0x0c 0 0 0 "

/* 单板功耗获取命令: get power reading
 * request:
 *       1 - netfund 0x2c
 *       2 - cmd     0x02
 *     3~6 - data    0xdc 0x0 0x0 0x0 后3个字节BMC暂不处理, 后续如有变动需同步修改
 * response: ipmitool里是把completion code屏掉的
 *       1 - DCMI Identifier 0xdc
 *     2~3 - Current Power in watts, 高字节在前, 比如3(0xAB), 4(0xCD), result: 0xABCD
 *   other - 无需关注
 */
#define HOST_POWER_DISSIPATION " 0x2c 0x02 0xdc 0x0 0x0 0x0 "

/* 风扇控速命令格式
 * ipmitool -I lan -H cmmip -U zte -P zte -t ipmb_addr raw 0x2c 0x15 0 0 N(1~13)
 */
#define FANTRAY_RPM_CONTROL " 0x2c 0x15 0 0 "

class HostPowerOperateTask:public Worktodo
{
public:
    HostPowerOperateTask(const string& name,const int64 &hid,const int32 &logic_slot,const string &cmm_ip,const int32 &opt):
            Worktodo(name),_hid(hid),_logic_slot(logic_slot),_opt(opt),_cmm_ip(cmm_ip)
    {};
    STATUS  Execute();
    void Print()
    {
        cout << "HostPowerQueryTask _hid: " << _hid << endl;
        cout << "HostPowerQueryTask _logic_slot: " << _logic_slot << endl;
        cout << "HostPowerQueryTask _opt: " << _opt << endl;
        cout << "HostPowerQueryTask _cmm_ip: " << _cmm_ip << endl;
    };
    
private:
    DISALLOW_COPY_AND_ASSIGN(HostPowerOperateTask);
    int64  _hid;         //host id
    int32  _logic_slot;  //逻辑槽位号 id
    int32  _opt;         //操作类型:0-上电，1-下电
    string _cmm_ip;      //cmm ip
};

class HostPowerQueryTask:public Worktodo
{
public:
    HostPowerQueryTask(const string& name,const int64 &hid,const int32 &logic_slot,const string &cmm_ip):
            Worktodo(name),_hid(hid),_logic_slot(logic_slot),_cmm_ip(cmm_ip)
    {};
    STATUS  Execute();
    void Print()
    {
        cout << "HostPowerQueryTask _hid: " << _hid << endl;
        cout << "HostPowerQueryTask _logic_slot: " << _logic_slot << endl;
        cout << "HostPowerQueryTask _cmm_ip: " << _cmm_ip << endl;
    };
    
private:
    int64  _hid;          //host id
    int32  _logic_slot;  //逻辑槽位号 id
    string _cmm_ip;     //cmm ip

DISALLOW_COPY_AND_ASSIGN(HostPowerQueryTask);
};

class HostPowerDissipationQueryTask:public Worktodo
{
public:
    HostPowerDissipationQueryTask(const string& name,const int64 &hid,const int32 &logic_slot,const string &cmm_ip):
            Worktodo(name),_hid(hid),_logic_slot(logic_slot),_cmm_ip(cmm_ip)
    {};
    STATUS  Execute();
    void Print()
    {
        cout << "HostPowerDissipationQueryTask _hid: " << _hid << endl;
        cout << "HostPowerDissipationQueryTask _logic_slot: " << _logic_slot << endl;
        cout << "HostPowerDissipationQueryTask _cmm_ip: " << _cmm_ip << endl;
    };
    
private:
    int64  _hid;          //host id
    int32  _logic_slot;   //逻辑槽位号 id
    string _cmm_ip;       //cmm ip

DISALLOW_COPY_AND_ASSIGN(HostPowerDissipationQueryTask);
};

class FantrayRPMCtlTask : public Worktodo
{
public:
    FantrayRPMCtlTask(const string& name,
                      const string &cmm_ip,
                      const int fan_index,
                      const int rpm_level) : 
                          Worktodo(name), _cmm_ip(cmm_ip),
                          _fan_index(fan_index), _rpm_level(rpm_level)
    {};
    STATUS  Execute();
    void Print()
    {
        cout << "FantrayRPMCtlTask _cmm_ip: " << _cmm_ip << endl;
        cout << "FantrayRPMCtlTask _fan_index: " << _fan_index << endl;
        cout << "FantrayRPMCtlTask _rpm_level: " << _rpm_level << endl;
    };
    
private:
    string _cmm_ip;       // cmm ip
    int    _fan_index;    // 风扇索引, 1/2
    int    _rpm_level;    // 调整的风扇转速等级, 1-13

DISALLOW_COPY_AND_ASSIGN(FantrayRPMCtlTask);
};


class PowerAgent : public MessageHandler
{
public:
    static PowerAgent *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new PowerAgent();
        }
        return _instance;

    }

    STATUS Init()
    {
        if (NULL == (_host_atca = HostAtca::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get HostAtca failed!\n");
            return ERROR_NOT_READY;
        }    

        if (NULL == (_cmm_config = ConfigCmmPool::GetInstance()))
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get ConfigCmmPool failed!\n");
            return ERROR_NOT_READY;
        }

        _thread_pool = new ThreadPool(4); // cmm有限制, 支持最多4个会话同时登录 
        if (_thread_pool == NULL) // 线程池创建失败
        {
            SkyAssert(0);
            OutPut(SYS_EMERGENCY, "Get ThreadPool failed!\n");
            return ERROR_NOT_READY;
        }
    
        if (SUCCESS != _thread_pool->CreateThreads("HostPowerQueryTask"))
         {
             OutPut(SYS_EMERGENCY,"failed to CreateThreads for HostPowerQueryTask!\n");
             SkyAssert(0);
             return ERROR_NOT_READY;
         }
        return StartMu(MU_POWER_AGENT);
    }
    
    virtual ~PowerAgent() 
    { 
        _instance = NULL; 
    }
    
private:
    MessageUnit         *_mu;               // 消息单元
    bool                _have_startup;      // 是否已启动的标志，避免重新初始化
    ConfigCmmPool       *_cmm_config;       // 本集群的cmm配置项
    ThreadPool          *_thread_pool; 
    HostAtca            *_host_atca;
    static PowerAgent   *_instance;

    PowerAgent();
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
    STATUS StartMu(const string& name);
    void DoQueryHostStateTimer();
    void DoCheckPowerStateTimer();
    int32 DoGetAssignWorkPara(const int64 &hid,string &cmm_ip,int32 &logic_slot);
    void DoHostResetProc(const MessageFrame &message);
    
    // 消息处理函数
    void DoStartUp();
    void DoQueryPowerDissipation();
    void DoFantrayRPMCtl(const MessageFrame &message);

    // 辅助函数
    void CheckCmmUserName();

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(PowerAgent);   
};

} // namespace zte_tecs
#endif // #ifndef 

