/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：door_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：DoorPool类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2012年9月18日
*
* 修改记录1：
*    修改日期：2012/9/18
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef DOOR_POOL_H        
#define DOOR_POOL_H

#include "pool_sql.h"
#include "door.h"

using namespace std;

namespace zte_tecs
{

/**
@brief 功能描述: Door数据库操作类\n
@li @b 其它说明: 无
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
	/* 继续添加数据库访问函数 */
	
	/* 继续添加回调函数 */
	
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

