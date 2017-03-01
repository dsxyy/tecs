/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�door_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��DoorPool��Ķ����ļ�
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

#ifndef DOOR_POOL_H        
#define DOOR_POOL_H

#include "pool_sql.h"
#include "door.h"

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: Door���ݿ������\n
@li @b ����˵��: ��
*/
class DoorPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static DoorPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static DoorPool *GetInstance(SqlDB *database = NULL)
    {
        if(NULL == _instance)
        {
            _instance = new DoorPool(database);
        }
        
        return _instance;
    };
     
    int Update(vector<Door>& vmbase_images, int64 id, bool is_vm);
    int Insert(const vector<Door>& vmbase_images, int64 id,bool is_vm);
	/* ����������ݿ���ʺ��� */
	
	/* ������ӻص����� */
	
/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/	
private:

    DoorPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        
    };
    static DoorPool *_instance;
};

}
#endif /* Door_POOL_H */

