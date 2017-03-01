/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_cmm_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ConfigCmmPool��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2011��8��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_CONFIG_CMM_POOL_H
#define HM_CONFIG_CMM_POOL_H
#include "sky.h"
#include "sqldb.h"
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
class ConfigCmmPool: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigCmmPool *GetInstance()
    {
        return _instance;
    }

    static ConfigCmmPool* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigCmmPool(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigCmmPool() { _instance = NULL; }
    int32 SelectCallback(void *nil, SqlColumn * columns);
    int32 Set(const CmmConfig &cmm_config);
    int32 Add(const CmmConfig &cmm_config);
    int32 Delete(const int32  bureau,const int32  rack,const int32  shelf);
    int32 Search(vector<CmmConfig> &cmm_infos,const string  where);
    int32 Count();
    string GetCmmIP(const int32  bureau,const int32  rack,const int32  shelf);
    //string GetCmmStringIP(const int32  bureau,const int32  rack,const int32  shelf);
    string GetShelfType(const int32  bureau,const int32  rack,const int32  shelf);


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
        BUREAU         = 0,
        RACK           = 1,
        SHELF          = 2,
        TYPE           = 3,
        IP_ADDRESS     = 4,
        DESCRIPTION    = 5
    };

    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigCmmPool *_instance;
    ConfigCmmPool() {}
    ConfigCmmPool(SqlDB *pDb);

    DISALLOW_COPY_AND_ASSIGN(ConfigCmmPool);
};

}  /* end namespace zte_tecs */

#endif /* end HM_CONFIG_TCU_H */

