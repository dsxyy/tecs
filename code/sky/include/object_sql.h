/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： object_sql.h
* 文件标识：
* 内容摘要： ObjectSQL 类定义文件
* 其它说明：
* 当前版本： 1.0
* 作    者： KIMI
* 完成日期： 2011-06-21
*
*    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
* 修 改 人：
* 修改日期：
* 修改内容：
**********************************************************************/
#ifndef DBS_OBJECT_SQL_H_
#define DBS_OBJECT_SQL_H_

#include "callbackable.h"
#include "sqldb.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief ObjectSQL 类. 关系表和持久化对象映射抽象基类，封装了插入、删除、修改、删除操作,需要各个应用实现具体表和持久化对象映射的实体类
 @li @b 其它说明：无
 */ 
class ObjectSQL : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ObjectSQL(){};

    virtual ~ObjectSQL(){};
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:
    /**************************************************************************/
    /**
    @brief 功能描述:关系表和持久化对象查询映射操作

    @li @b 入参列表：
    @verbatim
       db    数据库对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0       成功
       其他    失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int Select(
        SqlDB * db) = 0;

    /**************************************************************************/
    /**
    @brief 功能描述:关系表和持久化对象插入映射操作

    @li @b 入参列表：
    @verbatim
       db    数据库对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0       成功
       其他    失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int Insert(
        SqlDB * db,
        string& error_str) = 0;

    /**************************************************************************/
    /**
    @brief 功能描述:关系表和持久化对象更新映射操作

    @li @b 入参列表：
    @verbatim
       db    数据库对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0       成功
       其他    失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int Update(
        SqlDB * db) = 0;

    /**************************************************************************/
    /**
    @brief 功能描述:关系表和持久化对象删除映射操作

    @li @b 入参列表：
    @verbatim
       db    数据库对象
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       0       成功
       其他    失败
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int Drop(
        SqlDB * db) = 0;
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:        
    DISALLOW_COPY_AND_ASSIGN(ObjectSQL);
};
#endif /*DBS_OBJECT_SQL_H_*/

