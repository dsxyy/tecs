/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sqlite_sql_column.h
* �ļ���ʶ��
* ����ժҪ�� SqliteSQLColumn�ඨ���ļ�
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
#ifndef DBS_SQLITE_SQLCOLUMN_H_
#define DBS_SQLITE_SQLCOLUMN_H_

#ifdef TECS_USE_SQLITE

#include "sql_column.h"

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief SqliteSQLColumn��. SQLite�����������
 @li @b ����˵������
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
    @brief ��������:��ȡ�ֶ�ֵ�ĳ���

    @li @b ����б�
    @verbatim
       column     �ֶα�ţ���0��ʼ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶ�ֵ����
    @endverbatim

    @li @b �ӿ����ƣ�Ŀǰ�����ַ������͵��ֶ�������
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int  GetValueLen( int column );

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�ֶε�����ֵ

    @li @b ����б�
    @verbatim
       column     �ֶα�ţ���0��ʼ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶε�����ֵ
    @endverbatim

    @li @b �ӿ����ƣ��������ͺͶ��������Ͳ���ֱ��ת��Ϊ����
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int    GetValueInt( int column );
    
    /**************************************************************************/
    /**
    @brief ��������:��ȡ�ֶε�64λ����ֵ

    @li @b ����б�
    @verbatim
       column     �ֶα�ţ���0��ʼ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶε�����ֵ
    @endverbatim

    @li @b �ӿ����ƣ��������ͺͶ��������Ͳ���ֱ��ת��Ϊ����
    @li @b ����˵������
    */
    /**************************************************************************/       
    virtual int64  GetValueInt64( int column );

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�ֶεĸ�����ֵ

    @li @b ����б�
    @verbatim
       column     �ֶα�ţ���0��ʼ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶεĸ�����ֵ
    @endverbatim

    @li @b �ӿ����ƣ��������ͺͶ��������Ͳ���ֱ��ת��������
    @li @b ����˵������
    */
    /**************************************************************************/    
    virtual double GetValueDouble( int column );
    
    /**************************************************************************/
    /**
    @brief ��������:��ȡ�ֶε��ַ���ֵ

    @li @b ����б�
    @verbatim
       column     �ֶα�ţ���0��ʼ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶε��ַ���ֵ
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    virtual char * GetValueText( int column );

    /**************************************************************************/
    /**
    @brief ��������:��ȡ�ֶεĶ�����

    @li @b ����б�
    @verbatim
       column     �ֶα�ţ���0��ʼ
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶεĶ�����ֵ
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual void * GetValueBlob( int column );

    /**************************************************************************/
    /**
    @brief ��������:����SQLite�Ĳ�ѯ�����

    @li @b ����б�
    @verbatim
       col        �ֶ���    
       values     �ֶ�ֵ����
       names      �ֶ�������
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    
    //�ֶ�ֵ����
    char **_values;
    
    //�ֶ�������
    char **_names;
};

#endif /* TECS_USE_SQLITE */
#endif /*SQLITE_SQLCOLUMN_H_*/

