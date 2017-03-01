/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_tcu.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ConfigTcu��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_CONFIG_TCU_H
#define HM_CONFIG_TCU_H
#include "sky.h"
#include "sqldb.h"
#include "msg_host_manager_with_api_method.h"
#include "msg_cluster_agent_with_api_method.h" 

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

/**
@brief ��������: �����ඨ��
@li @b ����˵������
*/
class ConfigTcu: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigTcu *GetInstance()
    {
        return _instance;
    }

    static ConfigTcu* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigTcu(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigTcu() { _instance = NULL; }

    int32 Get(int64 cpu_info_id, TcuInfoType &tcu_info);
    int32 Set(int64 cpu_info_id, int32 tcu_num, const string &description);
    int32 Drop(int64 cpu_info_id);

    int32 Get(const map<string, string> &cpu_info, TcuInfoType &tcu_info);
    int32 Set(const map<string, string> &cpu_info, 
              int32 tcu_num, const string &description);
    int32 Drop(const map<string, string> &cpu_info);
    int64 Count();

    int32 Search(vector<TcuInfoType> &tcu_infos, const string &where);
    int32 GetSystemTcuNumByCpuId(int64 cpuid);

    void Print();

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
        CPU_INFO_ID     = 0,
        TCU_NUM         = 1,
        DESCRIPTION     = 2,
        LIMIT           = 3
    };

    static const char *_table;
    static const char *_db_names;
    static const char *_view_config_tcu_select_names;
    
    static const char *_view;
    static SqlDB      *_db;

    static ConfigTcu *_instance;
    ConfigTcu() {}
    ConfigTcu(SqlDB *pDb);

    int32 InsertReplace(int64 cpu_info_id, int32 tcu_num, 
                        const string &description, bool replace);
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 SearchCallback(void *nil, SqlColumn *columns);
    int32 CountCallback(void *nil, SqlColumn *columns);

    DISALLOW_COPY_AND_ASSIGN(ConfigTcu);
};

}  /* end namespace zte_tecs */

#endif /* end HM_CONFIG_TCU_H */

