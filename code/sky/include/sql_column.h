/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� sql_column.h
* �ļ���ʶ��
* ����ժҪ�� SqlColumn�ඨ���ļ�
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
#ifndef DBS_SQL_COLUMN_H_
#define DBS_SQL_COLUMN_H_
#include <stdint.h>
#include "pub.h"
#include "serialization.h"

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief SqlColumn��. ���ݿ��ѯ��������������
 @li @b ����˵������
 */
 
class SqlColumn 
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    SqlColumn() { _col_num = 0;}
    
    virtual ~SqlColumn() {};

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
    virtual int GetValueLen( int column ) = 0;
    
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
    virtual int GetValueInt( int column ) = 0;

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
    virtual int64  GetValueInt64( int column ) = 0;
    
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
    virtual double GetValueDouble( int column ) = 0;
    
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
    virtual const char *GetValueText( int column ) = 0;

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
    virtual void* GetValueBlob( int column ) = 0;
    virtual void GetValue(int column,bool& value) = 0;
    virtual void GetValue(int column,int& value) = 0;
    virtual void GetValue(int column,int64& value) = 0;
    virtual void GetValue(int column,uint64& value) = 0;
    virtual void GetValue(int column,string& value) = 0;
    virtual bool GetValue(int column,Serializable& value) = 0;

   /**************************************************************************/
    /**
    @brief ��������:��ȡ������е��ֶθ���

    @li @b ����б�
    @verbatim
       ��
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       �ֶθ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    int get_column_num()
    {
        return _col_num;
    }

   /**************************************************************************/
    /**
    @brief ��������:���ý�����е��ֶθ���

    @li @b ����б�
    @verbatim
       col_num     �ֶθ���
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
    void set_column_num( int col_num )
    {
        _col_num = col_num;
    }
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/    
private:
    int _col_num;
};
#endif /*DBS_SQL_COLUMN_H_*/

