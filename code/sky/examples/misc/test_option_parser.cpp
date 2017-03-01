#include "sky.h"
#include "option_parser.h"

struct CommonOptions : OptionParser
{
    string url;
    string address;
    string connectionOptions;
    bool forever;
    int count;

    CommonOptions(): OptionParser("Usage: test_option_parser [OPTIONS] ADDRESS", "just a test"),
          url("127.0.0.1"),
          forever(false),
          count(1)
    {
        //OptionParser::handle_unrecognised(OptionParser::IGNORE_UNRECOGNISED);
        Add("broker,b", url, "Url of broker to connect to.");
        Add("forever,f", forever, "Ignore timeout and wait forever.");
        Add("connection-options", connectionOptions, "Connection options string in the form {name1:value1, name2:value2}.");
        Add("count,c", count, "Number of messages to read before exiting.");
    }

    int getCount()
    {
        return count;
    }
    
    bool checkAddress()
    {
        if (getArguments().empty()) 
        {
            error("Address is required");
            return false;
        } 
        else 
        {
            address = getArguments()[0];
            return true;
        }
    }
};

struct SpecialOptions: public CommonOptions
{
    int port;
    SpecialOptions():CommonOptions(),port(1234)
    {
        Add("port,p", port, "Message broker port.");
    }
};

void test()
{
    SpecialOptions special_options;
    special_options.ParseIniFile("test1.ini","");
    cout << "special_options.url = " << special_options.url << endl;
    cout << "special_options.address = " << special_options.address << endl;
    cout << "special_options.connectionOptions = " << special_options.connectionOptions << endl;
    cout << "special_options.forever = " << special_options.forever << endl;
    cout << "special_options.count = " << special_options.count<< endl;
    cout << "special_options.port = " << special_options.port << endl;
}
DEFINE_DEBUG_FUNC(test);

int main(int argc, char** argv)
{
    SpecialOptions special_options;
    string username;
    special_options.Add("user,u", username, "Username to connect.");
    if (special_options.ParseCmdArgs(argc, argv) && special_options.checkAddress()) 
    {
        cout << "special_options.url = " << special_options.url << endl;
        cout << "special_options.address = " << special_options.address << endl;
        cout << "special_options.connectionOptions = " << special_options.connectionOptions << endl;
        cout << "special_options.forever = " << special_options.forever << endl;
        cout << "special_options.count = " << special_options.count<< endl;
        cout << "special_options.port = " << special_options.port << endl;
        cout << "username = " << username << endl;
    }

    Shell("test_option_parser-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;   
}





