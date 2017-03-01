#include "sky.h"

class Test
{
public:
    Test()
    {
        m.Init();
        //下面两个set操作不是必须的，可以用来调试信号量死锁问题
        m.SetName("test");
        m.SetDebug(true);
        memset(a,0,sizeof(a));
    };

    void Read()
    {
        //在本函数的作用域中对m加锁
        MutexLock lock(m);
        for(int i=0;i<8;i++)
            printf("a[%d] = %d\n",i,a[i]);
        //本函数返回之前，局部变量lock会被自动销毁，
        //在其析构函数中会自动释放锁m
    }

    void Write()
    {
        MutexLock lock(m);
        for(int i=0;i<8;i++)
            a[i] = i;
    }
private:
    int32 a[8];
    Mutex m;   //互斥信号量m，用于保护类成员数组a
};

int main()
{
    Test t;
    t.Write();
    t.Read();

    return 0;
}




