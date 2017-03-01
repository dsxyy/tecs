/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： pool_object_sql.h
* 文件标识：
* 内容摘要： PoolObjectSQL类定义文件
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
#ifndef DBS_POOL_OBJECT_SQL_H_
#define DBS_POOL_OBJECT_SQL_H_
#include <stdint.h>
#include <pthread.h>
#include "pub.h"
#include "object_sql.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief PoolObjectSQL 类. 资源池持久化对象类。每一个对象都有一个唯一的OID
 @li @b 其它说明：提供了并发保护，对象销毁时假定对象是被锁住的 
 */ 
class PoolObjectSQL : public ObjectSQL
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    PoolObjectSQL(int64 id=-1):_oid(id),_valid(true)
    {
        pthread_mutex_init(&_mutex,0);
    };

    virtual ~PoolObjectSQL()
    {
        pthread_mutex_unlock(&_mutex);

        pthread_mutex_destroy(&_mutex);
    };

    /**************************************************************************/
    /**
    @brief 功能描述:获取对象标识

    @li @b 入参列表：
    @verbatim
       无
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       oid     对象标识
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/    
    int64 GetOID() const
    {
        return _oid;
    };

    /**************************************************************************/
    /**
    @brief 功能描述:检查对象是否有效

    @li @b 入参列表：
    @verbatim
       无
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       true
       false
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/      
    const bool& IsValid() const
    {
       return _valid;
    };

    /**************************************************************************/
    /**
    @brief 功能描述:设置对象的有效标记

    @li @b 入参列表：
    @verbatim
       valid    有效标记
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/       
    void set_valid(const bool valid)
    {
        _valid = valid;
    }

    //锁住对象
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //解锁对象
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:

    //对象唯一标识
    int64  _oid;

    //对象有效标记
    bool _valid;
    
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

    //PoolSQL友元类
    friend class PoolSQL;
    
    DISALLOW_COPY_AND_ASSIGN(PoolObjectSQL);

    //对象并发控制信号量，销毁对象时，必须锁住
    pthread_mutex_t _mutex;
};

/**
 @brief PoolObjectSQLPointer类. 类似于智能指针,Get PoolObjectSQL对象后,不再需要UnLock或Put操作
 @li @b 其它说明：无
 */
template <class Type>
class PoolObjectSQLPointer
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    /**************************************************************************/
    /**
    @brief 功能描述:构造函数

    @li @b 入参列表：
    @verbatim
       obj    资源池对象
       lock   是否上锁
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/ 
    PoolObjectSQLPointer(Type *obj, bool lock):_obj(obj),_lock(lock) {};

    /**************************************************************************/
    /**
    @brief 功能描述:析构函数

    @li @b 入参列表：
    @verbatim
       无
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    ~PoolObjectSQLPointer()
    {
        if ( NULL != _obj && _lock )
        {
            _obj->UnLock();
        }
    }

    /**************************************************************************/
    /**
    @brief 功能描述:获取资源池对象

    @li @b 入参列表：
    @verbatim
       无
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       资源池对象
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    Type *Object()
    {
        return _obj; 
    }
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/  
private:
    //资源池对象指针
    Type *_obj;

    //是否上锁
    bool _lock;
};
#endif /*POOL_OBJECT_SQL_H_*/

