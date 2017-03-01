/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� pool_object_sql.h
* �ļ���ʶ��
* ����ժҪ�� PoolObjectSQL�ඨ���ļ�
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
#ifndef DBS_POOL_OBJECT_SQL_H_
#define DBS_POOL_OBJECT_SQL_H_
#include <stdint.h>
#include <pthread.h>
#include "pub.h"
#include "object_sql.h"

using namespace std;

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief PoolObjectSQL ��. ��Դ�س־û������ࡣÿһ��������һ��Ψһ��OID
 @li @b ����˵�����ṩ�˲�����������������ʱ�ٶ������Ǳ���ס�� 
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
    @brief ��������:��ȡ�����ʶ

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
       oid     �����ʶ
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    int64 GetOID() const
    {
        return _oid;
    };

    /**************************************************************************/
    /**
    @brief ��������:�������Ƿ���Ч

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
       true
       false
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/      
    const bool& IsValid() const
    {
       return _valid;
    };

    /**************************************************************************/
    /**
    @brief ��������:���ö������Ч���

    @li @b ����б�
    @verbatim
       valid    ��Ч���
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
    void set_valid(const bool valid)
    {
        _valid = valid;
    }

    //��ס����
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //��������
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:

    //����Ψһ��ʶ
    int64  _oid;

    //������Ч���
    bool _valid;
    
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

    //PoolSQL��Ԫ��
    friend class PoolSQL;
    
    DISALLOW_COPY_AND_ASSIGN(PoolObjectSQL);

    //���󲢷������ź��������ٶ���ʱ��������ס
    pthread_mutex_t _mutex;
};

/**
 @brief PoolObjectSQLPointer��. ����������ָ��,Get PoolObjectSQL�����,������ҪUnLock��Put����
 @li @b ����˵������
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
    @brief ��������:���캯��

    @li @b ����б�
    @verbatim
       obj    ��Դ�ض���
       lock   �Ƿ�����
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
    PoolObjectSQLPointer(Type *obj, bool lock):_obj(obj),_lock(lock) {};

    /**************************************************************************/
    /**
    @brief ��������:��������

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
       ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    @brief ��������:��ȡ��Դ�ض���

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
       ��Դ�ض���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
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
    //��Դ�ض���ָ��
    Type *_obj;

    //�Ƿ�����
    bool _lock;
};
#endif /*POOL_OBJECT_SQL_H_*/

