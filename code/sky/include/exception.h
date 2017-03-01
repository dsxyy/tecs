#ifndef SKY_EXCEPTION_H
#define SKY_EXCEPTION_H
#include "pub.h"
#define SKY_STRINGIFY(x) #x
#define SKY_TOSTRING(x) SKY_STRINGIFY(x)
#define SKY_ERROR_MSG(msg) __FILE__ ":" SKY_TOSTRING(__LINE__) ": " #msg
#define SKY_SOURCEINFO SourceInfo(__FILE__, SKY_TOSTRING(__LINE__), __PRETTY_FUNCTION__)

string getErrnoString(int err, const char* fn);
inline string getErrnoString(const char* fn)
{
    return getErrnoString(errno, fn);
}

class SourceInfo 
{
public:
    inline SourceInfo(const char* file, const char* line, const char* func)
    : _file(file), _line(line), _func(func)
    { }

    inline const char* file() const
    { return _file; }

    inline const char* line() const
    { return _line; }

    inline const char* func() const
    { return _func; }
private:
    const char* _file;
    const char* _line;
    const char* _func;
};

inline string operator+(const string& what, const SourceInfo& info)
{
    return string( info.file() ) + ':' + info.line() + ": " += what;
}

inline string operator+(const char* what, const SourceInfo& info)
{
    return string( info.file() ) + ':' + info.line() + ": " += what;
}

inline string operator+( const SourceInfo& info, const string& what)
{
    return string( info.file() ) + ':' + info.line() + ": " += what;
}

inline string operator+(const SourceInfo& info, const char* what)
{
    return string( info.file() ) + ':' + info.line() + ": " += what;
}

class SystemError: public runtime_error
{
public:
    SystemError(int err, const char* fn)
    :runtime_error(getErrnoString(err, fn)), m_errno(err)
    {
  
    };

    explicit SystemError(const char* fn)
    :runtime_error(getErrnoString(fn)), m_errno(errno)
    {

    };

    SystemError(const std::string& what, const SourceInfo& si)
    :std::runtime_error(what + si), m_errno(0)
    {

    };

    ~SystemError() throw()
    {
    };
private:
    int m_errno;
};

struct ExceptionConfig
{
    ExceptionConfig()
    {
        debug = false;
    };
    
    bool debug;
    string lastwords_file;
};

STATUS ExcInit(const ExceptionConfig& config);
bool IsDebug();
void assertion_failed(const string& exp, const char *const file,int const line);
#define SkyAssert(cond) \
    do \
        if (!(cond)) \
            assertion_failed(#cond,__FILE__, __LINE__); \
    while (0)

void SkyExit(int code,const string& lastwords = "null");
STATUS GetLastwords(string &lastwords, uint32 maxlen);
STATUS ClearLastwords();
STATUS GetAsserts(vector<string>& asserts);
int GetExceptionCount();
#endif

