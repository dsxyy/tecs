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
	
    //���û�ָ���ĳ־û�������뵽���ݿ⣬���ҷ���һ��Ψһ��OID
    //Allocate�ķ���ֵ��OID,-1ʧ�ܣ������ɹ�
    Student stu1(320821001, "department_1","major_1");
    stu1.set_name("John");
    stu1.add_book("English","an english book");
    stu1.add_book("Math","a math book");

    Student stu2(320821002, "department_2","major_2");
    stu2.set_name("Tom");
    stu2.add_book("Chemistry","a chemistry book");

    //��ʾ������÷���ͬʱ��������ѧ����¼��Ҫ�ɹ��Ͷ��ɹ���Ҫʧ�ܾͶ�ʧ��
    Transaction::Begin();
    student_pool->Allocate(&oid1, stu1, errmsg);
    student_pool->Allocate(&oid2, stu2, errmsg);
    Transaction::Commit();
    
    //����Get����ʱ������ڶ�������olockΪtrue��ʾ���Զ���������
    //ʹ����ɺ���Ҫ����PoolSql��Put���������߶����UnLock����
    //����ʹ��Put����
    Student *pstu1 = student_pool->Get(oid1, true);
    Student *pstu2 = student_pool->Get(oid2, false);
	    
    if ( pstu1 )
    {
        cout << "oid:" << oid1 << ", name = " << pstu1->get_name() << endl;
        student_pool->Put(pstu1, true); // �˴����Ե���stu1->UnLock �������student_pool->Put
    }
    else
    {
	    cerr << "failed to get oid:" << oid1 << endl;
    }
    
    if ( pstu2 )
    {
        cout << "oid:" << oid2 << ", name = " << pstu2->get_name() << endl;
        student_pool->Put(pstu2, false); // �˴����ܵ���stu2->UnLock
    }
    else
    {
        cerr << "failed to get oid:" << oid2 << endl;
    }
}

void test_search(StudentPool *student_pool)
{
    //����ָ��������,��ѯһ��־û�����
    vector<int64>             oids;
    vector<int64>::iterator   it;
    Student *stu;
    //����Ҫ��where �ؼ���
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

            student_pool->Put(stu, true);    //�˴����Ե���stu->UnLock();,���ǲ�����
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
        // �˴�����Ҫ��Ҳ���ܵ���stu3->UnLock��student_pool->Put(stu3, true);
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
        // �˴�����Ҫ��Ҳ���ܵ���stu3->UnLock��student_pool->Put(stu3, true);
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
    //������,����Ҫ����CREATE DATABASE,�����open���Զ�������
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
            
    //������
    StudentPool::BootStrap(pDb);
    PersonPool::BootStrap(pDb);
    StudentPool::BootStrap2(pDb);
    
    //������Դ�ض���
    student_pool = StudentPool::CreateInstance(pDb);
    if(SUCCESS != student_pool->Init())
    {
        SkyAssert(false);
        return NULL;
    }
   
    //�������
    test_allocate(student_pool);
    //������ѯ����
    test_search(student_pool);
    //PoolObjectSQL����ָ��
    test_autoptr(student_pool,2);
    //���¶���
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


