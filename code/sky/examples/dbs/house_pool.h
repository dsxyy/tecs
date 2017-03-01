/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�house.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��House��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2012��9��18��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/9/18
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#ifndef HOUSE_POOL_H        
#define HOUSE_POOL_H

#include "sky.h"
#include "pool_sql.h"
#include "house.h"

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: ��������ݿ������\n
@li @b ����˵��: ��
*/
class HousePool : public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static HousePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static HousePool *GetInstance(SqlDB *database = NULL)
    {
        if(NULL == _instance)
        {
            _instance = new HousePool(database);
        }
        
        return _instance;
    };
    virtual STATUS Init();
        
    int Allocate(House  &house);
    int Drop(const House  &house);
    int Update(House  &house);
    int Select(vector<House> &vec_house, const string &where);
	/* ����������ݿ���ʺ��� */
	
    int SelectCallback(void *nil, SqlColumn * columns);    
    /* ������ӻص����� */

/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/	
private:
    enum ColNames 
    {
        OID   = 0,
        NAME  = 1,
        UID   = 2,
        LIMIT = 3
    };
   
    HousePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        
    };    

    static int64         _lastOID;
    static HousePool       *_instance;  
    
    static const char   *_table_house;
    static const char   *_col_names_house;

    // ������ӱ� �� �ֶ���
    
	DISALLOW_COPY_AND_ASSIGN(HousePool);
};

}
#endif /* HOUSE_POOL_H */

