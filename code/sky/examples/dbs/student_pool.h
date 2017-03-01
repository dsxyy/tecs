/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： student_pool.h
* 文件标识：
* 内容摘要： StudentPool 类定义文件
* 其它说明：
* 当前版本： 1.0
* 作    者： KIMI
* 完成日期： 2011-06-21
*
*    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
* 修 改 人：
* 修改日期：
* 修改内容：
**********************************************************************/
#ifndef DBS_STUDENT_POOL_SQL_H_
#define DBS_STUDENT_POOL_SQL_H_

#include "pool_sql.h"
#include "student.h"

using namespace std;

// 禁用拷贝构造函数
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

namespace zte_tecs
{

/**
 @brief StudentPool 类.资源池测试类
 @li @b 其它说明：无
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
    
    //数据库中生成一个新的student对象
    //但是缓存中没有
    int64 Allocate(int64 *oid,Student& stu,string& error_str);
    
    //获取一个student对象，如果缓存中没有，从数据库读取
    Student *Get(int64 oid, bool lock)
    {
        Student *stu = static_cast<Student *>(PoolSQL::Get(oid,lock));
        return stu;
    }        
    
    //根据名称获取student对象
    Student *Get(string name, bool lock)
    {
        map<string, int64>::iterator index;

        //首先根据名称获取OID，然后根据OID获取资源对象
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
            // 同时从名称和OID的map中删除
            _known_users.erase(stu->get_name());
        }
        return rc;
    };

    int Dump(ostringstream& oss, const string& where)
    {
        return 0;
    }

    STATUS Init();
    
    //创建表
    static int BootStrap(SqlDB *db)
    {
        return Student::BootStrap(db);
    };

    //创建视图
    static int BootStrap2(SqlDB *db)
    {
        return Student::BootStrap2(db);
    };
private:
    StudentPool(SqlDB *db);

    //初始化oid的回调函数
    int InitCallback(void *nil, SqlColumn *columns);

    //创建资源池对象
    PoolObjectSQL * Create()
    {
        return new Student;
    };

    static StudentPool* instance;

    DISALLOW_COPY_AND_ASSIGN(StudentPool);
    
    /* PoolSQL维护了OID和资源池对象的map关系,大部分情况下可以满足要求
     * 但是，有时候我们还需要其他的map关系，比如：用户登陆时需要根据用户名获取用户的相关信息
     * 下面的map维护了用户名和OID的映射关系
     */
    //name--oid mapping relation
    map<string, int64>	_known_users;
};

}  //namespace zte_tecs

#endif /*DBS_STUDENT_POOL_SQL_H_*/

