/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：house.h
* 文件标识：见配置管理计划书
* 内容摘要：House类的定义文件
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

#ifndef HOUSE_POOL_H        
#define HOUSE_POOL_H

#include "sky.h"
#include "pool_sql.h"
#include "house.h"

using namespace std;

namespace zte_tecs
{

/**
@brief 功能描述: 虚拟机数据库操作类\n
@li @b 其它说明: 无
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
	/* 继续添加数据库访问函数 */
	
    int SelectCallback(void *nil, SqlColumn * columns);    
    /* 继续添加回调函数 */

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

    // 这里添加表 和 字段名
    
	DISALLOW_COPY_AND_ASSIGN(HousePool);
};

}
#endif /* HOUSE_POOL_H */

