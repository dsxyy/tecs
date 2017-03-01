/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�sqlite_db.h
* �ļ���ʶ��
* ����ժҪ�� SqliteDB�ඨ���ļ�
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
#ifndef DBS_SQLITE_DB_H_
#define DBS_SQLITE_DB_H_
#ifdef TECS_USE_SQLITE
#include <string>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sqlite3.h>

#include "sqldb.h"
#include "object_sql.h"

using namespace std;

 // ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

/**
 @brief SqliteDB��. SQLite���ݿ������
 @li @b ����˵������
 */
class SqliteDB : public SqlDB
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    SqliteDB(string& db_name);

    ~SqliteDB();

    /**************************************************************************/
    /**
    @brief ��������:���û�ָ����sqlite���ݿ⡣

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
       0       �ɹ�
       ����    ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    virtual int Open();
    
    /**************************************************************************/
    /**
    @brief ��������:�ر��û�ָ����sqlite���ݿ⡣

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
    virtual void Close();    

    /**************************************************************************/
    /**
    @brief ��������:SQLite���ݿ�ִ��SQL���

    @li @b ����б�
    @verbatim
       cmd      SQL����
       obj      �ص����� ��ѯʱʹ�ã�������е�ÿһ�����ݶ������
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

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    int Execute(ostringstream& cmd, Callbackable* obj=0);

    /**************************************************************************/
    /**
    @brief ��������:��Ӧ�ô�����ַ���ת��Ϊ�Ϸ���SQL����ַ���

    @li @b ����б�
    @verbatim
       str      Ӧ�ô�����ַ���
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       ��NULL       ��Ч��SQL�ַ����������׵�ַ
       NULL         ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���Ҫ����free_str�ͷŶ�̬�����SQL�ַ���
    @li @b ����˵������
    */
    /**************************************************************************/
    char * EscapeString(const string& str);

    /**************************************************************************/
    /**
    @brief ��������:�ͷŶ�̬������ַ���

    @li @b ����б�
    @verbatim
       str      ��̬������ַ������׵�ַ
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
    void FreeString(char * str);
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:
/*******************************************************************************
* 3. private section
*******************************************************************************/ 
private:

    DISALLOW_COPY_AND_ASSIGN(SqliteDB);

    //�Կ�����
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //�Կ����
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };
    
    //���������ź���
    pthread_mutex_t     _mutex;

    //SQLite���ݿ����Ӿ��
    sqlite3             *_db;
    
    //���ݿ�����
    string               _db_name;
    
    //���ݿ��Ƿ�����
    bool                 _bopen;
};
#endif /* TECS_USE_SQLITE */
#endif /*DBS_SQLITE_DB_H_*/

