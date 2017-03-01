/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sqlite_sql_column.h
* 文件标识：
* 内容摘要： SqliteSQLColumn类定以文件
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
#ifndef DBS_SQLITE_SQLCOLUMN_H_
#define DBS_SQLITE_SQLCOLUMN_H_

#ifdef TECS_USE_SQLITE

#include "sql_column.h"

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief SqliteSQLColumn类. SQLite结果集处理类
 @li @b 其它说明：无
 */
class SqliteSQLColumn: public SqlColumn {
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    SqliteSQLColumn();
    virtual ~SqliteSQLColumn();
    
    /**************************************************************************/
    /**
    @brief 功能描述:获取字段值的长度

    @li @b 入参列表：
    @verbatim
       column     字段编号，从0开始
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       字段值长度
    @endverbatim

    @li @b 接口限制：目前仅对字符串类型的字段有意义
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int  GetValueLen( int column );

    /**************************************************************************/
    /**
    @brief 功能描述:获取字段的整型值

    @li @b 入参列表：
    @verbatim
       column     字段编号，从0开始
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       字段的整型值
    @endverbatim

    @li @b 接口限制：日期类型和二进制类型不能直接转换为整型
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual int    GetValueInt( int column );
    
    /**************************************************************************/
    /**
    @brief 功能描述:获取字段的64位整型值

    @li @b 入参列表：
    @verbatim
       column     字段编号，从0开始
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       字段的整型值
    @endverbatim

    @li @b 接口限制：日期类型和二进制类型不能直接转换为整型
    @li @b 其它说明：无
    */
    /**************************************************************************/       
    virtual int64  GetValueInt64( int column );

    /**************************************************************************/
    /**
    @brief 功能描述:获取字段的浮点数值

    @li @b 入参列表：
    @verbatim
       column     字段编号，从0开始
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       字段的浮点数值
    @endverbatim

    @li @b 接口限制：日期类型和二进制类型不能直接转换浮点数
    @li @b 其它说明：无
    */
    /**************************************************************************/    
    virtual double GetValueDouble( int column );
    
    /**************************************************************************/
    /**
    @brief 功能描述:获取字段的字符串值

    @li @b 入参列表：
    @verbatim
       column     字段编号，从0开始
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       字段的字符串值
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/    
    virtual char * GetValueText( int column );

    /**************************************************************************/
    /**
    @brief 功能描述:获取字段的二进制

    @li @b 入参列表：
    @verbatim
       column     字段编号，从0开始
    @endverbatim

    @li @b 出参列表：
    @verbatim
       无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
       字段的二进制值
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/     
    virtual void * GetValueBlob( int column );

    /**************************************************************************/
    /**
    @brief 功能描述:设置SQLite的查询结果集

    @li @b 入参列表：
    @verbatim
       col        字段数    
       values     字段值数组
       names      字段名数组
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
    void SetResult( int col, char ** values, char **names)
    {
        _values = values;
        _names  = names;
        set_column_num( col);
    }
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/       
private:

    DISALLOW_COPY_AND_ASSIGN(SqliteSQLColumn);
    
    //字段值数组
    char **_values;
    
    //字段名数组
    char **_names;
};

#endif /* TECS_USE_SQLITE */
#endif /*SQLITE_SQLCOLUMN_H_*/

