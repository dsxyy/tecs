/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� postgresql_column.cpp
* �ļ���ʶ��
* ����ժҪ�� PostgreSQLColumn��ʵ���ļ�
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� KIMI
* ������ڣ� 2011-06-21
*
*    �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
* �� �� �ˣ�
* �޸����ڣ�
* �޸����ݣ�
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
 * ������ֵ���ֶ����岻����Ϊ���ص����ַ���
 */
int PostgreSQLColumn::GetValueLen( int column )
{ 
    return PQgetlength(_res,_row,column);
}

//�������ͺ�PostgreSQL�����OID���Ͷ�Ӧ��ϵ
/* ��������        OID����
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
    
    /* ����ֶ�ֵΪNULL */
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
        /* ������������ͺͶ��������Ͳ���ֱ��ת��Ϊ���� */            
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
    
    /* ����ֶ�ֵΪNULL */
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
        /* ������������ͺͶ��������Ͳ���ֱ��ת��Ϊ���� */            
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
    
    /* ����ֶ�ֵΪNULL */
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
        /* ������������ͺͶ��������Ͳ���ֱ��ת��Ϊ���� */            
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
    
    /* ����ֶ�ֵΪNULL */
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
    
    /* ����ֶ�ֵΪNULL */
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
    /* ����ֶ�ֵΪNULL������������޸� */
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
    /* ����ֶ�ֵΪNULL������������޸� */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = GetValueInt(column);
}

void PostgreSQLColumn::GetValue(int column,int64& value)
{
    /* ����ֶ�ֵΪNULL������������޸� */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = GetValueInt64(column);
}

void PostgreSQLColumn::GetValue(int column,uint64& value)
{
    /* ����ֶ�ֵΪNULL������������޸� */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = (uint64)GetValueInt64(column);
}

void PostgreSQLColumn::GetValue(int column,string& value)
{
    /* ����ֶ�ֵΪNULL������������޸� */
    if ( PQgetisnull(_res, _row, column) )
    {
        return;
    }
    
    value = GetValueText(column);
}

bool PostgreSQLColumn::GetValue(int column,Serializable& value)
{
    /* ����ֶ�ֵΪNULL������������޸� */
    if ( PQgetisnull(_res, _row, column) )
    {
        return true;
    }
    
    string text = GetValueText(column);
    return value.deserialize(text);
}

