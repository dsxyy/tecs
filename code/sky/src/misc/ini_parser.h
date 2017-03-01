#include <string>

using namespace std;
class IniParser
{
public:
    class Event
    { 
    public:
        // events return true, if parsing should be stopped
        virtual bool OnSection(const string& section) { return false; }
        virtual bool OnKey(const string& key) { return false; }
        virtual bool OnValue(const string& key) { return false; }
        virtual bool OnComment(const string& comment) { return false; }
        virtual bool OnError(const string& error_data="") { return true; }
        virtual ~Event() {};
    };

    IniParser(Event &event): _event(event), _state(STATE_0) {}
    bool Parse(char ch);
    void End();
    void Parse(istream& in);    

private:
    Event& _event;
    string _data;
    enum
    {
        STATE_0,
        STATE_SECTION,
        STATE_KEY,
        STATE_KEY_SP,
        STATE_VALUE0,
        STATE_VALUE,
        STATE_VALUE_SP,
        STATE_COMMENT
    } _state;
};

