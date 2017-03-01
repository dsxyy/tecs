/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�power_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��PowerAgent�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2013��5��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/5/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
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
    

#define M0_NotInstall            0   // δ��װ
#define M1_Inactive              1   // ���, ����û�мӵ�
#define M2_ActivationRequest     2   // ��������, �Ѿ��յ��ϵ�������
#define M3_ActivationInProcess   3   // ������
#define M4_Active                4   // ��� ���������Ĺ���״̬
#define M5_DeactivationRequest   5   // ȥ�������󣬹رյ�Դ
#define M6_DeactivationInProcess 6   // ȥ������
#define M7_CommunicationLost     7   // ͨ�Ŷ�ʧ��һ���ǵ��屻�γ���
#define State_Unknown            8   // �����쳣��������磺CMM����λ��IP��ַ��������⵼�µ�״̬�޷���ѯ

#define   FRUID             " 0x3e "  // 0x3e ��ʾǰ�� 0x56 ��ʾ��� , Ŀǰ��֧�ֺ��
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

/* ���幦�Ļ�ȡ����: get power reading
 * request:
 *       1 - netfund 0x2c
 *       2 - cmd     0x02
 *     3~6 - data    0xdc 0x0 0x0 0x0 ��3���ֽ�BMC�ݲ�����, �������б䶯��ͬ���޸�
 * response: ipmitool���ǰ�completion code������
 *       1 - DCMI Identifier 0xdc
 *     2~3 - Current Power in watts, ���ֽ���ǰ, ����3(0xAB), 4(0xCD), result: 0xABCD
 *   other - �����ע
 */
#define HOST_POWER_DISSIPATION " 0x2c 0x02 0xdc 0x0 0x0 0x0 "

/* ���ȿ��������ʽ
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
    int32  _logic_slot;  //�߼���λ�� id
    int32  _opt;         //��������:0-�ϵ磬1-�µ�
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
    int32  _logic_slot;  //�߼���λ�� id
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
    int32  _logic_slot;   //�߼���λ�� id
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
    int    _fan_index;    // ��������, 1/2
    int    _rpm_level;    // �����ķ���ת�ٵȼ�, 1-13

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

        _thread_pool = new ThreadPool(4); // cmm������, ֧�����4���Ựͬʱ��¼ 
        if (_thread_pool == NULL) // �̳߳ش���ʧ��
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
    MessageUnit         *_mu;               // ��Ϣ��Ԫ
    bool                _have_startup;      // �Ƿ��������ı�־���������³�ʼ��
    ConfigCmmPool       *_cmm_config;       // ����Ⱥ��cmm������
    ThreadPool          *_thread_pool; 
    HostAtca            *_host_atca;
    static PowerAgent   *_instance;

    PowerAgent();
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
    STATUS StartMu(const string& name);
    void DoQueryHostStateTimer();
    void DoCheckPowerStateTimer();
    int32 DoGetAssignWorkPara(const int64 &hid,string &cmm_ip,int32 &logic_slot);
    void DoHostResetProc(const MessageFrame &message);
    
    // ��Ϣ������
    void DoStartUp();
    void DoQueryPowerDissipation();
    void DoFantrayRPMCtl(const MessageFrame &message);

    // ��������
    void CheckCmmUserName();

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(PowerAgent);   
};

} // namespace zte_tecs
#endif // #ifndef 

