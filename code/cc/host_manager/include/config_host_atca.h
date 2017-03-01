/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_host_atca.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ConfigHostAtca��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2013��4��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/4/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_CONFIG_HOST_ATCA_H
#define HM_CONFIG_HOST_ATCA_H
#include "sky.h"
#include "sqldb.h"
#include "msg_cluster_agent_with_api_method.h" 
#include "msg_host_manager_with_host_agent.h" 
#include "tecs_pub.h"
#include "host.h"


// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

class host_atca_table_info
{
    public :
        host_atca_table_info():hid(INVALID_HID),board_type(INVALID_BOARD_TYPE),
         power_state(0),power_operate(1),rate_power(0),poweroff_time(0),total_poweroff_time(0){};
         
        int64 hid;
        BoardPosition host_position;
        int32 board_type;
        int32 power_state;
        int32 power_operate;
        int32 rate_power;
        int64 poweroff_time;
        int64 total_poweroff_time;
        string decription;   
};

class HostAtca: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    /* cmm��ѯ״̬ */
    enum Power_State
    {
        UNKOUWN          = 0,    // ��ʼ״̬
        BOARD_POWER_ON   = 1,    // ������λ�ϵ�
        BOARD_POWER_OFF  = 2,    // ������λ�µ�
        BOARD_ABSENT     = 3     // ���岻��λ
    };


    /* ������������״̬ */
    enum Power_Operate
    {
        OPERATE_POWER_ON         = 1,   // �ϵ����
        OPERATE_POWER_OFF        = 2,    // �µ����
        OPERATE_RESET            = 3   // ��λ����

    };

    static HostAtca *GetInstance()
    {
        return _instance;
    }

    static HostAtca* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new HostAtca(db); 
        }
        
        return _instance;
    };
    virtual ~HostAtca() { _instance = NULL; }
    int32 Add(const host_atca_table_info &info);
    int32 Delete(const int64  &hid);
    int32 Count();

    int32 Update(const host_atca_table_info &info);    
    int32 UpdatePowerOperate(int64 &hid,const int32 &power_operate);
    int32 UpdatePowerState(int64 &hid,const int32 &power_state);  
    int32 UpdatePowerOffTime(int64 &hid,int64 &time);
    int32 UpdateTotalPowerOffTime(int64 &hid,int64 &time);
    int32 UpdateRatePower(int64 &hid, int32& rate_power);
    
    int32 Search(vector<host_atca_table_info> &infos,const string  where);
    int32 SearchByHostId(host_atca_table_info &info,const int64 &hid);
    
    int32 GetRunningVmsHosts();
    int32 GetPowerState(int64 &hid);
    int32 GetPowerOffTime(int64 &hid,int64 &time);
    int32 GetTotalPowerOffTime(int64 &hid,int64 &time); 
    int32 GetPowerOperateByHostId(int64 &hid,int32 &state);
    int32 GetPoweStateByHostId(int64 &hid,int32 &state);
    int32 GetPowerStateNotMatchHost(vector<int64> &hids);
    int32 GetAllPoweroffOperateHosts(vector<int64> &hids);
    void  GetAllQueryHosts(vector<int64> &hids);
    int32 GetOnlineAndNoVmsHostNum();
    int32 GetOffLineAndPoweroffOperateHosts(vector<int64> &hids);
    int32 GetOnLineAndPoweronOperateHosts(vector<int64> &hids);
    int32 GetRatePower(int64& hid, int32& rate_power);

    bool CheckPositionIsRepeat(int64 &hid);
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    /* ����Ա�����ݿ���е���λ�� */
    enum ColNames
    {
        HID                 = 0,
        BUREAU              = 1,
        RACK                = 2,
        SHELF               = 3,
        SLOT                = 4,
        BOARD_TYPE          = 5,
        POWER_STATE         = 6,
        POWER_OPERATE       = 7,
        RATE_POWER          = 8,
        POWEROFF_TIME       = 9,
        TOTAL_POWEROFF_TIME = 10,
        DESCRIPTION         = 11,
        LIMIT               = 12
        
    };

    static const char *_table;
    static const char *_host_atca_with_host_pool_view;
    static const char *_host_position_delete_repeat_view;
    static const char *_host_position_cmmip_valid_view;
    
    static const char *_db_names;
    static SqlDB      *_db;

    static HostAtca *_instance;
    HostAtca() {}
    HostAtca(SqlDB *pDb);
    int32 SelectCallback(void *nil, SqlColumn * columns);
    int32 SelectByHostIdCallback(void *nil, SqlColumn * columns);

    DISALLOW_COPY_AND_ASSIGN(HostAtca);
};


}  /* end namespace zte_tecs */

#endif /* end HM_CONFIG_TCU_H */

