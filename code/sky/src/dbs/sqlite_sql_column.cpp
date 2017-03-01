/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sqlite_sql_column.cpp
* �ļ���ʶ��
* ����ժҪ�� SqliteSQLColumn��ʵ���ļ�
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
 * ������ֵ���ֶ����岻����Ϊ���ص����ַ���
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

