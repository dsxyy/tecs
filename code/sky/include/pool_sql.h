/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� pool_sql.h
* �ļ���ʶ��
* ����ժҪ�� PoolSQL�ඨ���ļ�
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
#ifndef DBS_POOL_SQL_H_
#define DBS_POOL_SQL_H_

#include <stdint.h>
#include <map>
#include <string>
#include <queue>

#include "sqldb.h"
#include "pool_object_sql.h"
#include "sql_column.h"

using namespace std;

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

class SqlCallbackable: public Callbackable
{
public:
    SqlCallbackable() 
    {
    };

    SqlCallbackable(SqlDB *db)
    {
        _db = db;
    };

    int SelectColumn(const string& tablename,const string& columnname,const string& where, string& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectStringCallback),
                            (void *)&value);
    }
    
    int SelectColumn(const string& tablename,const string& columnname,const string& where, int& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectIntCallback),
                            (void *)&value);
    }
    
    int SelectColumn(const string& tablename,const string& columnname,const string& where, int64& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectInt64Callback),
                            (void *)&value);
    }

    int SelectColumn(const string& tablename,const string& columnname,const string& where, vector<string>& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectStringVecCallback),
                            (void *)&value);
    }

    int SelectColumn(const string& tablename,const string& columnname,const string& where, vector<int>& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectIntVecCallback),
                            (void *)&value);
    }

    int SelectColumn(const string& tablename,const string& columnname,const string& where, vector<int64>& value)
    {
        return SelectCommon(_db,
                            tablename,
                            columnname,
                            where,
                            static_cast<Callbackable::Callback>(&SqlCallbackable::SelectInt64VecCallback),
                            (void *)&value);
    }

protected:
    SqlDB *_db;
    
private:
    int SelectCommon(SqlDB *db,
                    const string& tablename,
                    const string& columnname,
                    const string& where,
                    Callback cb, 
                    void *arg = 0);
    int SelectStringCallback(void *nil, SqlColumn *columns);
    int SelectIntCallback(void *nil, SqlColumn *columns);
    int SelectInt64Callback(void *nil, SqlColumn *columns);
    int SelectStringVecCallback(void *nil, SqlColumn *columns);
    int SelectIntVecCallback(void *nil, SqlColumn *columns);
    int SelectInt64VecCallback(void *nil, SqlColumn *columns);
};

/**
 @brief PoolSQL�ࡣ�־û�������Դ���࣬�ṩ���̲߳������ʱ���
 @li @b ����˵������
 */
 
class PoolSQL: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    /**************************************************************************/
    /**
    @brief ��������:��ʼ�������ʶ������������Ӧ�����ݿ���л�ȡ���Ķ����ʶ��

    @li @b ����б�
    @verbatim
       db       ���ݿ����
       table    �־û������Ӧ�ı�
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
    PoolSQL(SqlDB * db, const char * table);

    virtual ~PoolSQL();

    /**************************************************************************/
    /**
    @brief ��������:����һ���µĶ��󣬲����뵽���ݿ��У����ǻ�����û�С�

    @li @b ����б�
    @verbatim
       objsql       �Ѿ���ʼ���ĳ־û�����
    @endverbatim

    @li @b �����б�
    @verbatim
       error_str    ��ϸ�Ĵ�����Ϣ
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       -1       ʧ��
       ����     �ɹ�����һ���µĶ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int64 Allocate(
        PoolObjectSQL   *objsql,
        string&          error_str);

    /**************************************************************************/
    /**
    @brief ��������:����һ���µĶ��󣬲����뵽���ݿ��У����ǻ�����û�С�

    @li @b ����б�
    @verbatim
       objsql       �Ѿ���ʼ���ĳ־û�����
    @endverbatim

    @li @b �����б�
    @verbatim
       error_str    ��ϸ�Ĵ�����Ϣ
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       -1       ʧ��
       ����     �ɹ�����һ���µĶ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int64 Allocate(
        PoolObjectSQL   &objsql,
        string&          error_str ); 
                
    /**************************************************************************/
    /**
    @brief ��������:����Դ�ػ�ȡָ���Ķ�����Դ�ز����ڴ����ݿ��ȡ��

    @li @b ����б�
    @verbatim
       oid       �־û������ʶ
       lock      �Ƿ�Զ�������
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       NULL       ʧ��
       ����       �ɹ�
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    virtual PoolObjectSQL * Get(
        int64     oid,
        bool    lock);

    /**************************************************************************/
    /**
    @brief ��������:�����ȡ����ʱ�������ͷ��������󲻻ᱻɾ��������Դ�ض�����ʱ�Żᱻɾ����

    @li @b ����б�
    @verbatim
       objsql    �־û�����
       lock      �Ƿ�Զ�������
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
    virtual void Put(
        PoolObjectSQL *objsql,
        bool    lock);	
	
    /**************************************************************************/
    /**
    @brief ��������:��ѯ����������һ�����

    @li @b ����б�
    @verbatim
       table       �־û������Ӧ�Ĺ�ϵ��
       where       SQL����where��ѯ����
    @endverbatim

    @li @b �����б�
    @verbatim
       oids        �־û������ʶ����
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0          ʧ��
       ����       �ɹ�
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int Search(
        vector<int64>&    oids,
        const char *    table,
        const string&   where);

    /**************************************************************************/
    /**
    @brief ��������:�������ݿ��г־û������Ӧ�ļ�¼

    @li @b ����б�
    @verbatim
       objsql       �־û�����
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0          ʧ��
       ����       �ɹ�
    @endverbatim

    @li @b �ӿ����ƣ���������Ѿ�����
    @li @b ����˵������
    */
    /**************************************************************************/      
    virtual int Update(
        PoolObjectSQL * objsql)
    {
        int rc;

        rc = objsql->Update(_db);

        return rc;
    };

    /**************************************************************************/
    /**
    @brief ��������:ɾ�����ݿ��г־û������Ӧ�ļ�¼

    @li @b ����б�
    @verbatim
       objsql       �־û�����
    @endverbatim

    @li @b �����б�
    @verbatim
       ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
       0          ʧ��
       ����       �ɹ�
    @endverbatim

    @li @b �ӿ����ƣ���������Ѿ�����
    @li @b ����˵������
    */
    /**************************************************************************/     
    virtual int Drop(
        PoolObjectSQL * objsql)
    {
       return objsql->Drop(_db);
    };

    /**************************************************************************/
    /**
    @brief ��������:�����Դ�������еĳ־û�����

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
    void Clean();

    /**************************************************************************/
    /**
    @brief ��������:����Դ���еĳ־û�������XML��ʽ���

    @li @b ����б�
    @verbatim
       oss     �����
       where   SQL���where��ѯ����
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
    virtual int Dump(ostringstream& oss, const string& where) = 0;

    /**
    @brief ��������:��ȡ�������ʶ
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
      int64 �������ʶ
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/      
    int64 get_lastOID()
    {
        return _lastOID;
    };

    /**
    @brief ��������:�����������ʶ
    @li @b ����б�
    @verbatim
       int64  new_oid Ҫ���õ��������ʶ
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
    void set_lastOID(int64  new_oid)
    {
        _lastOID = new_oid;
    };
    
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    //��ס��Դ��
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    //������Դ��
    void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };

    //������󹤳�����������һ���־û�����
    virtual PoolObjectSQL * Create() = 0;
    
    DISALLOW_COPY_AND_ASSIGN(PoolSQL);
    
    /**************************************************************************/
    /**
    @brief ��������:ʹ������FIFO�Ĳ��ԡ���ɾ������֮ǰ��鿴�����Ƿ���ס��û����ס�ᱻɾ����������뵽����β

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

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/     
    void Replace();

    //���ó־û������ʶ�Ļص�����
    int  InitCallback(void *nil, SqlColumn * pColumns);

    //��ѯ����������һ�����Ļص�����
    int  SearchCallback(void *oids, SqlColumn * pColumns);
    
    //���Ʋ����Ļ����ź���
    pthread_mutex_t             _mutex;

    //��Դ�ػ������Ĵ�С�������Դ���ж��������������������replaceɾ������ͷ�Ķ���
    static const unsigned int   _max_pool_size;

    //�������ʶ
    int64                         _lastOID;

    //�����ʶ/����ָ���map����
    map<int64,PoolObjectSQL *>    _pool;

    //OID���У�����ʵ����Դ�ػ����FIFO�滻����
    queue<int64>                  _oid_queue;
};
#endif /*DBS_POOL_SQL_H_*/

