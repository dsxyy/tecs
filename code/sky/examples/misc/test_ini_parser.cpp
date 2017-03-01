#include "sky.h"

void TestIniFile(const char *filename)
{
    ifstream in(filename);
    if (!in)
    {
        cout << "could not open file \"" << filename << '"' << endl;
    }

    IniFile ini(in);

    cout << "whoami-application = " << ini.GetValue("", "application") << endl;
    cout << "not-exist = " << ini.GetValue("", "unknown") << endl;
    cout << ini;    
}
DEFINE_DEBUG_FUNC(TestIniFile);

int main()
{
    Shell("test_ini_file-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}


