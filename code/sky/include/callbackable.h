/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� callbackable.h
* �ļ���ʶ��
* ����ժҪ�� ���ݿ���ʻص���������Callbackable�ඨ���ļ�
* ����˵����
* ��ǰ�汾��
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
#ifndef DBS_CALLBACKABLE_H_
#define DBS_CALLBACKABLE_H_

#include <pthread.h>
#include "sql_column.h"

using namespace std;

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief Callbackable ��. ���ݿ���ʻص��������࣬�־û�������Ҫ����������ʵ�ֻص�����
 @li @b ����˵������
 */ 
class Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    //�ص��������Ͷ���
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
    @brief ��������:������һ��SQL���ִ��ʱ���õĻص��������Զ������

    @li @b ����б�
    @verbatim
       cb       �ص�����ָ��
       arg      Ӧ���Զ������
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       ��
    @endverbatim

    @li @b �ӿ����ƣ�����ס_mutexֱ������unset_callback
    @li @b ����˵������
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
    @brief ��������:�жϻص������Ƿ�����

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
       true       �����ûص�����
       false      û�����ûص�����
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */   
    /**************************************************************************/    
    bool IsCallBackSet()
    {
        return (_cb != 0);
    };

    /**************************************************************************/
    /**
    @brief ��������:�����Ѿ��ɹ����õĻص�����

    @li @b ����б�
    @verbatim
       pColumn    ��ѯ�����
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0       �ɹ�
       ����    ʧ��
    @endverbatim

    @li @b �ӿ����ƣ�isCallBackSet���뷵��true
    @li @b ����˵������
    */      
    /**************************************************************************/
    int DoCallback(SqlColumn * pColumn)
    {
        return (this->*_cb)(_arg, pColumn);
    };

    /**************************************************************************/
    /**
    @brief ��������:ȡ���Ѿ��ɹ����õĻص�����

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
    
    //��ѯʱ���������ÿһ�����ݵ��õĻص�����
    Callback _cb;

    //Ӧ���Զ���Ļص������Ĳ���
    void *   _arg;

    //���ƻص������������ź���.
    pthread_mutex_t    _mutex;
};
#endif /*CALLBACKABLE_H_*/

