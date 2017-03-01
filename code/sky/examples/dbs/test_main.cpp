#include "sky.h"
#include "postgresql_db.h"
#include "sqlite_db.h"
#include "sample_type.h"
#include "student_pool.h"

using namespace zte_tecs;
static SqlDB *pDb = NULL;
static StudentPool *student_pool = NULL;

void test_allocate(StudentPool *student_pool)
{
    int64 oid1 = -1;
    int64 oid2 = -1;
    string errmsg;
	
    //将用户指定的持久化对象插入到数据库，并且分配一个唯一的OID
    //Allocate的返回值是OID,-1失败，其他成功
    Student stu1(320821001, "department_1","major_1");
    stu1.set_name("John");
    stu1.add_book("English","an english book");
    stu1.add_book("Math","a math book");

    Student stu2(320821002, "department_2","major_2");
    stu2.set_name("Tom");
    stu2.add_book("Chemistry","a chemistry book");

    //演示事务的用法，同时加入两个学生记录，要成功就都成功，要失败就都失败
    Transaction::Begin();
    student_pool->Allocate(&oid1, stu1, errmsg);
    student_pool->Allocate(&oid2, stu2, errmsg);
    Transaction::Commit();
    
    //调用Get方法时，如果第二个参数olock为true表示，对对象上锁，
    //使用完成后需要调用PoolSql的Put方法，或者对象的UnLock方法
    //建议使用Put方法
    Student *pstu1 = student_pool->Get(oid1, true);
    Student *pstu2 = student_pool->Get(oid2, false);
	    
    if ( pstu1 )
    {
        cout << "oid:" << oid1 << ", name = " << pstu1->get_name() << endl;
        student_pool->Put(pstu1, true); // 此处可以调用stu1->UnLock 建议调用student_pool->Put
    }
    else
    {
	    cerr << "failed to get oid:" << oid1 << endl;
    }
    
    if ( pstu2 )
    {
        cout << "oid:" << oid2 << ", name = " << pstu2->get_name() << endl;
        student_pool->Put(pstu2, false); // 此处不能调用stu2->UnLock
    }
    else
    {
        cerr << "failed to get oid:" << oid2 << endl;
    }
}

void test_search(StudentPool *student_pool)
{
    //根据指定的条件,查询一组持久化对象
    vector<int64>             oids;
    vector<int64>::iterator   it;
    Student *stu;
    //不需要加where 关键字
    string where = "department = 'department_1'"; 
    int rc = student_pool->Search(oids, "student", where);
	
    cout << "============================================================" << endl;
    if ( rc == 0 && !oids.empty() )
    {
        for ( it = oids.begin(); it != oids.end(); it++ )
        {
            stu = student_pool->Get(*it,true);

            if ( stu == 0 )
            {
                continue;
            }
		
            cout << "oid:" << *it << ", name = " << stu->get_name() << endl;
            
            if ( stu->GetOID() % 4 == 0)
            {
                student_pool->Drop(stu);
            }
            
            stu->set_age(stu->get_age() + 1);
            student_pool->Update(stu);

            student_pool->Put(stu, true);    //此处可以调用stu->UnLock();,但是不建议
        }	
    }
    cout << "============================================================" << endl;
}

#if 0
void test_type()
{
    ostringstream oss;
    string errmsg;
    //SqlDB *pDb = new PostgreSqlDB("",0,"postgresql","","test");
    string dbname = "test";
    SqlDB *pDb = new SqliteDB(dbname);
    
    pDb->Open();
    
    SampleType::BootStrap(pDb);
    
    SampleType sample;
	printf("select: \n");
    sample.Select(pDb);
    
    printf("drop: \n");
    sample.Drop(pDb);
    sample.Select(pDb);
    
    printf("Insert: \n");
    sample.Insert(pDb,errmsg);
    sample.Select(pDb);
   
    printf("update: \n");
    sample.Update(pDb);
    sample.Select(pDb);
   
    delete pDb;
}
#endif

void test_autoptr(StudentPool *student_pool,int64 oid)
{
    PoolObjectSQLPointer<Student>  stup(student_pool->Get(oid,true), true);
    Student *stu = stup.Object();
    if (stu)
    {
        cout << "oid:" << oid << ", name = " << stu->get_name() << endl;
        // 此处不需要，也不能调用stu3->UnLock和student_pool->Put(stu3, true);
    }
    else
    {
        cout << "failed to get oid:" << oid << endl;	
    }
}

void test_update(StudentPool *student_pool,int64 oid)
{
    PoolObjectSQLPointer<Student>  stup(student_pool->Get(oid,true), true);
    Student *stu = stup.Object();
    if (stu)
    {
        cout << "oid:" << oid << ", name = " << stu->get_name() << endl;
        // 此处不需要，也不能调用stu3->UnLock和student_pool->Put(stu3, true);
    }
    else
    {
        cout << "failed to get oid:" << oid << endl;	
    }
    stu->set_age(30);
    student_pool->Update(stu);
}

static void InsertStudent(const char* name,int64 person_id)
{
    int64 oid = -1;
    string errmsg;

    Student stu(person_id, "department_1","major_1");
    stu.add_book("English","an english book");
    stu.add_book("Math","a math book");
    stu.set_name(name);
    student_pool->Allocate(&oid, stu, errmsg);
    cout << "InsertStudent: " << name <<", oid = " << oid << endl;
}
DEFINE_DEBUG_FUNC(InsertStudent);

extern "C" void *StudentManagerEntry(void *arg)
{
#if 0
    test_type();
#endif
    //创建库,不需要调用CREATE DATABASE,下面的open会自动创建库
    pDb = new PostgreSqlDB("127.0.0.1",5432,"tecs","tecs","Students");
    int ret = pDb->Open();
    if(0 != ret)
    {
        SkyAssert(false);
        return NULL;
    }

    ret = CreateTransactionKey();
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return NULL;
    }
    
    ret = Transaction::Enable(pDb);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return NULL;
    }
            
    //创建表
    StudentPool::BootStrap(pDb);
    PersonPool::BootStrap(pDb);
    StudentPool::BootStrap2(pDb);
    
    //创建资源池对象
    student_pool = StudentPool::CreateInstance(pDb);
    if(SUCCESS != student_pool->Init())
    {
        SkyAssert(false);
        return NULL;
    }
   
    //分配对象
    test_allocate(student_pool);
    //批量查询对象
    test_search(student_pool);
    //PoolObjectSQL智能指针
    test_autoptr(student_pool,2);
    //更新对象
    test_update(student_pool,2);
    
    while(1)
    {
        sleep(1);
    }
    delete student_pool;
    delete pDb;
    return NULL;
}
    
int main( int argc , char *argv[])
{
    THREAD_ID tid = StartThread("StudentManager",StudentManagerEntry,NULL);
    if(INVALID_THREAD_ID == tid)
    {
        return -1;
    }
        
    Shell("test_dbs-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}


