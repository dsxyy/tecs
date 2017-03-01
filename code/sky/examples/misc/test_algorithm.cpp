#include "sky.h"

void test1()
{
    vector<string> strings;
    strings.push_back("hello");
    strings.push_back("world");
    strings.push_back("tecs");
    cout << JoinStrings(strings,",") << endl;
}
DEFINE_DEBUG_CMD(test1,test1);

void test2()
{
    vector<string> strings;
    strings.push_back("hello");
    cout << JoinStrings(strings,",") << endl;
}
DEFINE_DEBUG_CMD(test2,test2);

int main()
{
    Shell("test_algorithm-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}


