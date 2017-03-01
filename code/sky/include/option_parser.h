#ifndef SKY_OPTIONPARSER_H
#define SKY_OPTIONPARSER_H
#include <map>
#include <string>
#include <vector>

#define  LENGTH_WITHOUT_DESCRIPTION                30
#define  LENGTH_WITH_DESCRIPTION                   50
#define  LENGTH_OF_PRINTLINE                       (LENGTH_WITHOUT_DESCRIPTION + LENGTH_WITH_DESCRIPTION)

class Option;
class OptionParser
{
public:
    OptionParser(const string& usage, const string& description);
    ~OptionParser();
    void Add(const string& name, string& value, const string& description = string());
    void Add(const string& name, int& value, const string& description = string());
    void Add(const string& name, bool& value, const string& description = string());
    void Add(const string& name, vector<string>& value, const string& description = string());
    void PrintHelp();
    void PrintAll();
    STATUS ParseCmdArgs(int argc, char** argv);
    bool ParseIniFile(const char* filename,const char* section);
    void error(const string& message);
    vector<string>& getArguments();
protected:
    typedef vector<Option*> Options;
    Options options;
private:
    const string summary;
    const string description;
    bool help;
    vector<string> arguments;
    void Add(Option*);
    Option* getOption(const string& argument);
};

#endif  /*!OPTIONPARSER_H*/

