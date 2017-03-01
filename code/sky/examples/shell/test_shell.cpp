#include "sky.h"
static void test1(int a)
{
    printf("you input a number = %d\n",a);
}
DEFINE_DEBUG_FUNC(test1);
//DEFINE_DEBUG_FUNC定义了一个调试函数，在shell中输入时，输入内容必须遵循c语言语法格式
//调试函数的各个参数必须是c语言基本数据类型，不支持c++对象引用参数

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
//DEFINE_DEBUG_CMD定义了一个调试命令，在shell中输入时，只需输入命名名称即可，不支持参数，使用起来简单明了
//在本例中，只要在shell下输入hello，不需要括号，即可执行函数test3
//调试命令通常用于一些特殊场合，例如每个进程都支持exit命令，直接在shell下输入exit回车即可退出

static int var1;
static char var2;
DEFINE_DEBUG_VAR(var1);
DEFINE_DEBUG_VAR(var2);
//DEFINE_DEBUG_VAR定义了一个调试变量，可以在shell中打印和赋值(输入内容必须遵循c语言语法格式)
//该变量只能是c语言基本数据类型，不支持将c++类对象作为调试变量

DEFINE_DEBUG_FUNC(Telnetd);
int main()
{
    //Telnetd(true,11000);  //如果进程不需要在后台运行，则不用运行telnet服务
    Shell("test_shell-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}
    
