/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： postgresql_column.cpp
* 文件标识：
* 内容摘要： PostgreSQLColumn类实现文件
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
/* for ntohl/htonl */
#include <netinet/in.h>
#include <stdlib.h>
#include <iostream>

#include "pg_type.h"
#include "postgresql_column.h"


/* ************************************************************************** */
/* PostgreSQLColumn constructor/destructor                                             */
/* ************************************************************************** */

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
PostgreSQLColumn::PostgreSQLColumn()
{
    _db  = NULL;
    _res = NULL;
    _row = 0;
}
PostgreSQLColumn::~PostgreSQLColumn()
{
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/**
 * 对于数值型字段意义不大，因为返回的是字符串
 */
int PostgreSQLColumn::GetValueLen( int column )
{ 
    return PQgetlength(_res,_row,column);
}

//数据类型和PostgreSQL定义的OID类型对应关系
/* 数据类型        OID类型
smallint          INT2OID
integer           INT4OID
bigint            INT8OID
decimal           NUMERICOID
numeric           NUMERICOID
real              FLOAT4OID
double precision  FLOAT8OID
float             FLOAT8OID
char              BPCHAROID
varchar           VARCHAROID
text              TEXTOID
bytea             BYTEAOID
timestamp         TIMESTAMPOID
interval          INTERVALOID
date              DATEOID
time              TIMEOID
boolean           BOOLOID
*/

/* -------------------------------------------------------------------------- */
int PostgreSQLColumn::GetValueInt( int column )
{
    Oid type;
    char *vptr;
    
    /* 如果字段值为NULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        return 0;
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        return 0;
    }
    
    type = PQftype(_res, column );
    
	switch (type)
	{
		case BOOLOID:
            if( 't' == vptr[0]  ||  'T' == vptr[0] )
                return 1;
            return 0;
            break;
		case INT2OID:
		case INT4OID:
            return strtol(vptr, NULL, 10);
            break;
		case INT8OID:
            return strtoll(vptr, NULL, 10);
            break;
		case FLOAT4OID:
		case FLOAT8OID:
		case NUMERICOID:     
            return (int)strtold(vptr, NULL);        
            break;
		case TEXTOID:            
		case BPCHAROID:
        case VARCHAROID:
            return (int)strtoll(vptr, NULL, 10);
            break;      
        /* 下面的日期类型和二进制类型不能直接转换为整型 */            
		case DATEOID:
		case TIMEOID:
		case TIMESTAMPOID:
        case BYTEAOID:
        case INTERVALOID:
		default:
            return 0;
			break;
	}    
    return 0;
}

/* -------------------------------------------------------------------------- */
int64 PostgreSQLColumn::GetValueInt64( int column )
{
    Oid type;
    char *vptr;
    
    /* 如果字段值为NULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        return 0;
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        return 0;
    }
    
    type = PQftype(_res, column );
    
	switch (type)
	{
		case BOOLOID:
            if( 't' == vptr[0]  ||  'T' == vptr[0] )
                return 1;
            return 0;
            break;
		case INT2OID:
		case INT4OID:
            return strtol(vptr, NULL, 10);
            break;
		case INT8OID:
            return strtoll(vptr, NULL, 10);
            break;
		case FLOAT4OID:
		case FLOAT8OID:
		case NUMERICOID:     
            return (long long)strtold(vptr, NULL);        
            break;
		case TEXTOID:            
		case BPCHAROID:
        case VARCHAROID:
            return strtoll(vptr, NULL, 10);
            break;      
        /* 下面的日期类型和二进制类型不能直接转换为整型 */            
		case DATEOID:
		case TIMEOID:
		case TIMESTAMPOID:
        case BYTEAOID:
        case INTERVALOID:
		default:
            return 0;
			break;
	}    
    return 0;
}

/* -------------------------------------------------------------------------- */
double PostgreSQLColumn::GetValueDouble( int column )
{
    Oid type;
    char *vptr;
    
    /* 如果字段值为NULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        return 0.0;
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        return 0.0;
    }
    
    type = PQftype(_res, column );
    
	switch (type)
	{
		case BOOLOID:
            if( 't' == vptr[0]  ||  'T' == vptr[0] )
                return 1.0;
            return 0.0;
            break;
		case INT2OID:
		case INT4OID:
            return (double)strtol(vptr, NULL, 10);
            break;
		case INT8OID:
            return (double)strtoll(vptr, NULL, 10);
            break;
		case FLOAT4OID:
		case FLOAT8OID:
		case NUMERICOID:     
            return strtold(vptr, NULL);        
            break;
		case TEXTOID:            
		case BPCHAROID:
        case VARCHAROID:
            return strtold(vptr, NULL);
            break;      
        /* 下面的日期类型和二进制类型不能直接转换为整型 */            
		case DATEOID:
		case TIMEOID:
		case TIMESTAMPOID:
        case BYTEAOID:
        case INTERVALOID:
		default:
            return 0.0;
			break;
	}    
    return 0.0;
}

/* -------------------------------------------------------------------------- */
const char * PostgreSQLColumn::GetValueText( int column )
{
    Oid type;
    char *vptr;
    
    /* 如果字段值为NULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        return "";
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        return "";
    }
    
    type = PQftype(_res, column );
    
	switch (type)
	{
		case BOOLOID:
            if( 't' == vptr[0]  ||  'T' == vptr[0] )
                return "true";
            return "false";
            break;
		default:
            return vptr;
			break;
	}    
    return "";
}

/* -------------------------------------------------------------------------- */
void * PostgreSQLColumn::GetValueBlob( int column )
{
    Oid type;
    char *vptr;
    
    /* 如果字段值为NULL */
    if ( PQgetisnull(_res, _row, column) )
    {
        return (void *)"";
    }
    if ( (vptr = PQgetvalue(_res, _row, column) ) == NULL )
    {
        return (void *)"";
    }
    
    type = PQftype(_res, column );
    
	switch (type)
	{
		case BOOLOID:
            if( 't' == vptr[0]  ||  'T' == vptr[0] )
                return (void *)"true";
            return (void *)"false";
            break;
		default:
            return (void *)vptr;
			break;
	}    
    return (void *)"";
}

void PostgreSQLColumn::GetValue(int column,bool& value)
{
    /* 如果字段值为NULL，不对入参做修改 */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    int tmp_value = GetValueInt(column);
    if(0 == tmp_value)
        value = false;
    else
        value = true;
}

void PostgreSQLColumn::GetValue(int column,int& value)
{
    /* 如果字段值为NULL，不对入参做修改 */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = GetValueInt(column);
}

void PostgreSQLColumn::GetValue(int column,int64& value)
{
    /* 如果字段值为NULL，不对入参做修改 */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = GetValueInt64(column);
}

void PostgreSQLColumn::GetValue(int column,uint64& value)
{
    /* 如果字段值为NULL，不对入参做修改 */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = (uint64)GetValueInt64(column);
}

void PostgreSQLColumn::GetValue(int column,string& value)
{
    /* 如果字段值为NULL，不对入参做修改 */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = GetValueText(column);
}

bool PostgreSQLColumn::GetValue(int column,Serializable& value)
{
    /* 如果字段值为NULL，不对入参做修改 */
    if ( PQgetisnull(_res, _row, column) )
    {
        return true;
    }
    
    string text = GetValueText(column);
    return value.deserialize(text);
}

