/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： sqlite_sql_column.cpp
* 文件标识：
* 内容摘要： SqliteSQLColumn类实现文件
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
#ifdef TECS_USE_SQLITE

/* for ntohl/htonl */
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "sqlite_sql_column.h"

/* ************************************************************************** */
/* SqliteSQLColumn constructor/destructor                                             */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
SqliteSQLColumn::SqliteSQLColumn()
{
    _values = NULL;
    _names  = NULL;
}
SqliteSQLColumn::~SqliteSQLColumn()
{
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/**
 * 对于数值型字段意义不大，因为返回的是字符串
 */
int SqliteSQLColumn::GetValueLen( int column )
{ 
    if ( column >= get_column_num() || NULL == _values || NULL == _values[column] )
        return -1;
    return strlen(_values[column]);
}

/* -------------------------------------------------------------------------- */
int SqliteSQLColumn::GetValueInt( int column )
{
    if ( column >= get_column_num() || NULL == _values || NULL == _values[column] )
        return 0;

    return strtol(_values[column], NULL, 10);

}

/* -------------------------------------------------------------------------- */
int64 SqliteSQLColumn::GetValueInt64( int column )
{
    if ( column >= get_column_num() || NULL == _values || NULL == _values[column] )
        return 0;
    
    return strtoll(_values[column], NULL, 10);
}

/* -------------------------------------------------------------------------- */
double SqliteSQLColumn::GetValueDouble( int column )
{
    if ( column >= get_column_num() || NULL == _values || NULL == _values[column] )
        return 0.0;
    
    return strtod(_values[column], NULL);
}

/* -------------------------------------------------------------------------- */
char * SqliteSQLColumn::GetValueText( int column )
{
    if ( column >= get_column_num() || NULL == _values || NULL == _values[column] )
        return "";
    
    return _values[column];
}

/* -------------------------------------------------------------------------- */
void * SqliteSQLColumn::GetValueBlob( int column )
{
    if ( column >= get_column_num() || NULL == _values || NULL == _values[column] )
        return (void *)"";
    
    return (void *)_values[column];
}
#endif /* TECS_USE_SQLITE */

