#include "sky.h"
static void test1(int a)
{
    printf("you input a number = %d\n",a);
}
DEFINE_DEBUG_FUNC(test1);
//DEFINE_DEBUG_FUNC������һ�����Ժ�������shell������ʱ���������ݱ�����ѭc�����﷨��ʽ
//���Ժ����ĸ�������������c���Ի����������ͣ���֧��c++�������ò���

static void test2(const char* input)
{
    printf("you input string = %s\n",input);
}
DEFINE_DEBUG_FUNC(test2);

static void test3()
{
    printf("hello, world!\n");
}
DEFINE_DEBUG_CMD(hello,test3);
//DEFINE_DEBUG_CMD������һ�����������shell������ʱ��ֻ�������������Ƽ��ɣ���֧�ֲ�����ʹ������������
//�ڱ����У�ֻҪ��shell������hello������Ҫ���ţ�����ִ�к���test3
//��������ͨ������һЩ���ⳡ�ϣ�����ÿ�����̶�֧��exit���ֱ����shell������exit�س������˳�

static int var1;
static char var2;
DEFINE_DEBUG_VAR(var1);
DEFINE_DEBUG_VAR(var2);
//DEFINE_DEBUG_VAR������һ�����Ա�����������shell�д�ӡ�͸�ֵ(�������ݱ�����ѭc�����﷨��ʽ)
//�ñ���ֻ����c���Ի����������ͣ���֧�ֽ�c++�������Ϊ���Ա���

DEFINE_DEBUG_FUNC(Telnetd);
int main()
{
    //Telnetd(true,11000);  //������̲���Ҫ�ں�̨���У���������telnet����
    Shell("test_shell-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}
    
