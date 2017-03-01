/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� student_pool.h
* �ļ���ʶ��
* ����ժҪ�� StudentPool �ඨ���ļ�
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
#ifndef DBS_STUDENT_POOL_SQL_H_
#define DBS_STUDENT_POOL_SQL_H_

#include "pool_sql.h"
#include "student.h"

using namespace std;

// ���ÿ������캯��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

/**
 @brief StudentPool ��.��Դ�ز�����
 @li @b ����˵������
 */ 
class StudentPool: public PoolSQL
{
public:
    static StudentPool* GetInstance()
    {
        return instance;
    };

    static StudentPool* CreateInstance(SqlDB *db)
    {
        if (NULL == instance)
        {
            instance = new StudentPool(db);
        }

        return instance;
    };

    virtual ~StudentPool();
    
    //���ݿ�������һ���µ�student����
    //���ǻ�����û��
    int64 Allocate(int64 *oid,Student& stu,string& error_str);
    
    //��ȡһ��student�������������û�У������ݿ��ȡ
    Student *Get(int64 oid, bool lock)
    {
        Student *stu = static_cast<Student *>(PoolSQL::Get(oid,lock));
        return stu;
    }        
    
    //�������ƻ�ȡstudent����
    Student *Get(string name, bool lock)
    {
        map<string, int64>::iterator index;

        //���ȸ������ƻ�ȡOID��Ȼ�����OID��ȡ��Դ����
        index = _known_users.find(name);
        if ( index != _known_users.end() )
        {
            return Get((int64)index->second,lock);
        }
        return 0;
    }

    int Update(Student *stu)
    {
        return stu->Update(_db);
    };
    
    int Drop(Student *stu)
    {
        int rc = PoolSQL::Drop(stu);
        if ( rc == 0)
        {
            // ͬʱ�����ƺ�OID��map��ɾ��
            _known_users.erase(stu->get_name());
        }
        return rc;
    };

    int Dump(ostringstream& oss, const string& where)
    {
        return 0;
    }

    STATUS Init();
    
    //������
    static int BootStrap(SqlDB *db)
    {
        return Student::BootStrap(db);
    };

    //������ͼ
    static int BootStrap2(SqlDB *db)
    {
        return Student::BootStrap2(db);
    };
private:
    StudentPool(SqlDB *db);

    //��ʼ��oid�Ļص�����
    int InitCallback(void *nil, SqlColumn *columns);

    //������Դ�ض���
    PoolObjectSQL * Create()
    {
        return new Student;
    };

    static StudentPool* instance;

    DISALLOW_COPY_AND_ASSIGN(StudentPool);
    
    /* PoolSQLά����OID����Դ�ض����map��ϵ,�󲿷�����¿�������Ҫ��
     * ���ǣ���ʱ�����ǻ���Ҫ������map��ϵ�����磺�û���½ʱ��Ҫ�����û�����ȡ�û��������Ϣ
     * �����mapά�����û�����OID��ӳ���ϵ
     */
    //name--oid mapping relation
    map<string, int64>	_known_users;
};

}  //namespace zte_tecs

#endif /*DBS_STUDENT_POOL_SQL_H_*/

