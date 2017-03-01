#ifndef SKY_PUB_H
#define SKY_PUB_H
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <queue>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <assert.h>       
#include <errno.h>       
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/syscall.h> 
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <termios.h>
#include <pty.h>
#include <syslog.h>
#include "errcode.h"
#include <sys/wait.h>
#include <tr1/functional>


typedef unsigned char       byte;
typedef char                int8;
typedef short               int16;
typedef signed int          int32;
typedef long long           int64;
typedef long int            intptr;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
typedef unsigned long int   uintptr;

typedef byte BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned int WORD32;
typedef unsigned short WORD16;
typedef signed short SWORD16;
typedef unsigned long int WORDPTR;
typedef signed long int SWORDPTR;
typedef void VOID;
typedef unsigned char BOOLEAN;
typedef unsigned long long  WORD64;
typedef long long SWORD64;

#ifndef NULL
    #define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef OK
#define OK 0
#endif

#define WAIT_FOREVER ((uint32)0xFFFFFFFF)

/* 支持浮点打印所加 */
typedef double      DOUBLE;
typedef float       FLOAT; 
typedef long double LDOUBLE;
typedef	int		    BOOL;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)

using namespace std;

template <class T>
class tpl_from_string
{
public:
    static bool from(T& t,const string& s,ios_base& (*f)(std::ios_base&))
    {
        stringstream ss;
        ss << f << s;
        return !(ss >> t).fail();
    }
};

template <>
class tpl_from_string<string>
{
public:
    static bool from(string& t,const string& s,ios_base& (*f)(std::ios_base&))
    {
        t = s;
        return true;
    }
};

template <class T>
bool from_string(T& t,const string& s,ios_base& (*f)(std::ios_base&))
{
    return tpl_from_string<T>::from(t,s,f);
}

template <class T>
class tpl_to_string
{
public:
    static string to(const T& t,ios_base& (*f)(std::ios_base&))
    {
        stringstream ss;
        ss << f << t;
        return ss.str();
    }
};

template <>
class tpl_to_string<string>
{
public:
    static string to(const string& t,ios_base& (*f)(std::ios_base&))
    {
        return t;
    }
};

template <class T>
string to_string (const T& t,ios_base& (*f)(std::ios_base&))
{
    return tpl_to_string<T>::to(t,f);
}
#endif


