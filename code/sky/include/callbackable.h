/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： callbackable.h
* 文件标识：
* 内容摘要： 数据库访问回调函数基类Callbackable类定义文件
* 其它说明：
* 当前版本：
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
#ifndef DBS_CALLBACKABLE_H_
#define DBS_CALLBACKABLE_H_

#include <pthread.h>
#include "sql_column.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief Callbackable 类. 数据库访问回调函数基类，持久化对象需要从它派生并实现回调函数
 @li @b 其它说明：无
 */ 
class Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    //回调函数类型定义
    typedef int (Callbackable::*Callback)(void *, SqlColumn * );
    
    Callbackable():_cb(0),_arg(0)
    {
        pthread_mutex_init(&_mutex,0);
    };

    virtual ~Callbackable()
    {
        pthread_mutex_destroy(&_mutex);
    };

    /**************************************************************************/
    /**
    @brief 功能描述:设置下一条SQL语句执行时调用的回调函数和自定义参数

    @li @b 入参列表：
    @verbatim
       cb       回调函数指针
       arg      应用自定义参数
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       无
    @endverbatim

    @li @b 接口限制：会锁住_mutex直到调用unset_callback
    @li @b 其它说明：无
    */
    /**************************************************************************/       
    void SetCallback(Callback cb, void * arg = 0)
    {
        pthread_mutex_lock(&_mutex);

        _cb  = cb;
        _arg = arg;
    };

    /**************************************************************************/
    /**
    @brief 功能描述:判断回调函数是否设置

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
       true       已设置回调函数
       false      没有设置回调函数
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */   
    /**************************************************************************/    
    bool IsCallBackSet()
    {
        return (_cb != 0);
    };

    /**************************************************************************/
    /**
    @brief 功能描述:调用已经成功设置的回调函数

    @li @b 入参列表：
    @verbatim
       pColumn    查询结果集
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

    @li @b 接口限制：isCallBackSet必须返回true
    @li @b 其它说明：无
    */      
    /**************************************************************************/
    int DoCallback(SqlColumn * pColumn)
    {
        return (this->*_cb)(_arg, pColumn);
    };

    /**************************************************************************/
    /**
    @brief 功能描述:取消已经成功设置的回调函数

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
    void UnsetCallback()
    {
        _cb  = 0;
        _arg = 0;

        pthread_mutex_unlock(&_mutex);
    }

/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    DISALLOW_COPY_AND_ASSIGN(Callbackable);
    
    //查询时，结果集中每一行数据调用的回调函数
    Callback _cb;

    //应用自定义的回调函数的参数
    void *   _arg;

    //控制回调函数并发的信号量.
    pthread_mutex_t    _mutex;
};
#endif /*CALLBACKABLE_H_*/

