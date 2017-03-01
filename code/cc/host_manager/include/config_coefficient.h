/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_coefficient.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��config_coefficient��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�2012��3��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��3��21��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_CONFIG_COEFFICIENT_H
#define HM_CONFIG_COEFFICIENT_H
#include "sky.h"
#include "sqldb.h"

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
class config_coefficient: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static config_coefficient *GetInstance()
    {
        return _instance;
    }

    static config_coefficient* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new config_coefficient(db); 
        }
        
        return _instance;
    };
    virtual ~config_coefficient() { _instance = NULL; }

    int32 Set(const string  &coefficient_type, 
               int32 coefficient_value, 
               const string &description);


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
        TECS_BENCH      = 0,
        DESCRIPTION     = 1,
        LIMIT           = 2
    };

    static const char *_table;
    static const char *_db_names;
    static const char *_view;
    static SqlDB      *_db;

    static config_coefficient *_instance;
    config_coefficient();
    config_coefficient(SqlDB *pDb);
    int32 InsertReplace(const string &coefficient_type, int32 coefficient_value, 
                        const string &description, bool replace);

    DISALLOW_COPY_AND_ASSIGN(config_coefficient);
};

}  /* end namespace zte_tecs */

#endif /* end CONFIG_COEFFIEIENT_H */

