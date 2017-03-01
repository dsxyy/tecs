/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� postgresql_column.h
* �ļ���ʶ��
* ����ժҪ�� PostgreSQLColumn�ඨ���ļ�
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
#ifndef DBS_POSTGRESQL_COLUMN_H_
#define DBS_POSTGRESQL_COLUMN_H_

#include "libpq-fe.h"
#include "sql_column.h"

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief PostgreSQLColumn��. PostgreSQL�����������
 @li @b ����˵������
 */
class PostgreSQLColumn: public SqlColumn
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    PostgreSQLColumn();
    virtual ~PostgreSQLColumn();
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
    virtual int GetValueLen( int column );

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
    virtual int GetValueInt( int column );

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
    virtual const char * GetValueText( int column );
    
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
    void GetValue(int column,bool& value);
    void GetValue(int column,int& value);
    void GetValue(int column,int64& value);
    void GetValue(int column,uint64& value);
    void GetValue(int column,string& value);
    bool GetValue(int column,Serializable& value);
    
    /**************************************************************************/
    /**
    @brief ��������:����PostgreSQL�Ĳ�ѯ�����

    @li @b ����б�
    @verbatim
       db         ���ݿ����
       res        �����
       row        ��¼��
       col_num    �ֶ���
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
    void SetPGresult(PGconn *db, PGresult  *res, int row, int col_num )
    {
        _db  = db;
        _res = res;
        _row = row;
        set_column_num( col_num);
    }
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/    
private:

    DISALLOW_COPY_AND_ASSIGN(PostgreSQLColumn);
    
    //PostgreSQL���ݿ����Ӿ��
    PGconn     *_db;
    
    //PostgreSQL�����
    PGresult   *_res;
    
    //������еļ�¼��
    int         _row;
};
#endif /*DBS_POSTGRESQL_COLUMN_H_*/

