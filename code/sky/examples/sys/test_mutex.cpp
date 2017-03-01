#include "sky.h"

class Test
{
public:
    Test()
    {
        m.Init();
        //��������set�������Ǳ���ģ��������������ź�����������
        m.SetName("test");
        m.SetDebug(true);
        memset(a,0,sizeof(a));
    };

    void Read()
    {
        //�ڱ��������������ж�m����
        MutexLock lock(m);
        for(int i=0;i<8;i++)
            printf("a[%d] = %d\n",i,a[i]);
        //����������֮ǰ���ֲ�����lock�ᱻ�Զ����٣�
        //�������������л��Զ��ͷ���m
    }

    void Write()
    {
        MutexLock lock(m);
        for(int i=0;i<8;i++)
            a[i] = i;
    }
private:
    int32 a[8];
    Mutex m;   //�����ź���m�����ڱ������Ա����a
};

int main()
{
    Test t;
    t.Write();
    t.Read();

    return 0;
}




