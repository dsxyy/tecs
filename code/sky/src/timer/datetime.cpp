#include "common.h"
#include "datetime.h"

void Date::refresh()
{
    time_t now = time(NULL);
    struct tm dt;
    gmtime_r(&now, &dt);
    
    //tm_year是1900年以来的年数
    //tm_mon从一月算起，范围从0-11
    //tm_mday是目前月份的日数，范围01-31
    //将日期转换为儒略日保存
    toJulian(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
}

Date::Date()
{
    refresh();
}

Date::Date(struct tm *dt)
{
    if(!dt)
        julian = 0;
    else
    toJulian(dt->tm_year + 1900, dt->tm_mon + 1, dt->tm_mday);
}

Date::Date(time_t tm)
{
    struct tm dt;
    gmtime_r(&tm, &dt);
    toJulian(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
}

Date::Date(char *str, size_t size)
{
    setDate(str, size);
}

Date::Date(int year, unsigned month, unsigned day)
{
    toJulian(year, month, day);
}

Date::~Date()
{}

void Date::setDate(const char *str, size_t size)
{
    time_t now = time(NULL);
    struct tm dt;
    gmtime_r(&now, &dt);
    int year = 0;
    const char *mstr = str;
    const char *dstr = str;

    if (!size)
        size = strlen(str);
//0000
    if (size == 4)
    {
#ifdef DEBUG
        cout << "Date::SetDate(): 0000" << endl;
#endif
        year = dt.tm_year + 1900;
        mstr = str;
        dstr = str + 2;
    }
//00/00
    else if (size == 5)
    {
#ifdef DEBUG
        cout << "Date::SetDate(): 00/00" << endl;
#endif
        year = dt.tm_year + 1900;
        mstr = str;
        dstr = str + 3;
    }
//000000
    else if (size == 6)
    {
#ifdef DEBUG
        cout << "Date::SetDate(): 000000" << endl;
#endif
        ZNumber nyear((char*)str, 2);
        year = ((dt.tm_year + 1900) / 100) * 100 + nyear();
        mstr = str + 2;
        dstr = str + 4;
    }
//00000000
    else if (size == 8 && str[2] >= '0' && str[2] <= '9' && str[5] >= '0' && str[5] <= '9')
    {
#ifdef DEBUG
        cout << "Date::SetDate(): 00000000" << endl;
#endif
        ZNumber nyear((char*)str, 4);
        year = nyear();
        mstr = str + 4;
        dstr = str + 6;
    }
//00/00/00
    else if (size == 8)
    {
#ifdef DEBUG
        cout << "Date::SetDate(): 00/00/00" << endl;
#endif
        ZNumber nyear((char*)str, 2);
        year = ((dt.tm_year + 1900) / 100) * 100 + nyear();
        mstr = str + 3;
        dstr = str + 6;
    }

//0000/00/00
    else if (size == 10)
    {
#ifdef DEBUG
        cout << "Date::SetDate(): 0000-00-00" << endl;
#endif
        ZNumber nyear((char*)str, 4);
        year = nyear();
        mstr = str + 5;
        dstr = str + 8;
    }
    else
    {
        julian = 0x7fffffffl;
        return;
    }

    ZNumber nmonth((char*)mstr, 2);
    ZNumber nday((char*)dstr, 2);
    toJulian(year, nmonth(), nday());
}

void Date::update(void)
{}

bool Date::isValid(void) const
{
    if (julian == 0x7fffffffl)
        return false;
    return true;
}

char *Date::getDate(char *buf) const
{
    fromJulian(buf);
    return buf;
}

time_t Date::getDate(tm* dt) const
{
    char buf[11];
    memset(dt, 0, sizeof(tm));
    fromJulian(buf);
    Number nyear(buf, 4);
    Number nmonth(buf + 5, 2);
    Number nday(buf + 8, 2);

    dt->tm_year = nyear() - 1900;
    dt->tm_mon = nmonth() - 1;
    dt->tm_mday = nday();

    return timegm(dt); // to fill in day of week etc.
}

time_t Date::getDate(void) const
{
    struct tm dt;
    return getDate(&dt);
}

int Date::getYear(void) const
{
    char buf[11];
    fromJulian(buf);
    Number num(buf, 4);
    return num();
}

unsigned Date::getMonth(void) const
{
    char buf[11];
    fromJulian(buf);
    Number num(buf + 5, 2);
    return num();
}

unsigned Date::getDay(void) const
{
    char buf[11];
    fromJulian(buf);
    Number num(buf + 8, 2);
    return num();
}

unsigned Date::getDayOfWeek(void) const
{
    return (unsigned)((julian + 1l) % 7l);
}

long Date::getValue(void) const
{
    char buf[11];
    fromJulian(buf);
    return atol(buf) * 10000 + atol(buf + 5) * 100 + atol(buf + 8);
}

string Date::operator()() const
{
    char buf[11];

    fromJulian(buf);
    string date(buf);

    return date;
}

Date& Date::operator++()
{
    ++julian;
    update();
    return *this;
}

Date& Date::operator--()
{
    --julian;
    update();
    return *this;
}

Date& Date::operator+=(const long val)
{
    julian += val;
    update();
    return *this;
}

Date& Date::operator-=(long val)
{
    julian -= val;
    update();
    return *this;
}

int Date::operator==(const Date &d)
{
    return julian == d.julian;
}

int Date::operator!=(const Date &d)
{
    return julian != d.julian;
}

int Date::operator<(const Date &d)
{
    return julian < d.julian;
}

int Date::operator<=(const Date &d)
{
    return julian <= d.julian;
}

int Date::operator>(const Date &d)
{
    return julian > d.julian;
}

int Date::operator>=(const Date &d)
{
    return julian >= d.julian;
}

void Date::toJulian(long year, long month, long day)
{
    //公历转换为儒略日
    julian = 0x7fffffffl;

    if (month < 1 || month > 12 || day < 1 || day > 31 || year == 0)
    {
        throw runtime_error("Illegal date!");
        return;
    }

    if (year < 0)
        year--;

    julian = day - 32075l +
             1461l * (year + 4800l + ( month - 14l) / 12l) / 4l +
             367l * (month - 2l - (month - 14l) / 12l * 12l) / 12l -
             3l * ((year + 4900l + (month - 14l) / 12l) / 100l) / 4l;
}

void Date::fromJulian(char *buffer) const
{
// The following conversion algorithm is due to
// Henry F. Fliegel and Thomas C. Van Flandern:

    ZNumber nyear(buffer, 4);
    buffer[4] = '-';
    ZNumber nmonth(buffer + 5, 2);
    buffer[7] = '-';
    ZNumber nday(buffer + 8, 2);

    double i, j, k, l, n;

    l = julian + 68569.0;
    n = int( 4 * l / 146097.0);
    l = l - int( (146097.0 * n + 3)/ 4 );
    i = int( 4000.0 * (l+1)/1461001.0);
    l = l - int(1461.0*i/4.0) + 31.0;
    j = int( 80 * l/2447.0);
    k = l - int( 2447.0 * j / 80.0);
    l = int(j/11);
    j = j+2-12*l;
    i = 100*(n - 49) + i + l;
    nyear = int(i);
    nmonth = int(j);
    nday = int(k);

    buffer[10] = '\0';
}

Date operator+(const Date &date, const long val)
{
    Date d = date;
    d.julian += val;
    d.update();
    return d;
}

Date operator+(const long val, const Date &date)
{
    Date d = date;
    d.julian += val;
    d.update();
    return d;
}

Date operator-(const Date &date, const long val)
{
    Date d = date;
    d.julian -= val;
    d.update();
    return d;
}

Date operator-(const long val, const Date &date)
{
    Date d = date;
    d.julian -= val;
    d.update();
    return d;
}

void Time::refresh()
{
    time_t now = time(NULL);
    struct tm dt;
    gmtime_r(&now, &dt);

    toSeconds(dt.tm_hour, dt.tm_min, dt.tm_sec);
}

Time::Time()
{
    refresh();
}

Time::Time(struct tm *dt)
{
    if(!dt)
        seconds = 0;
    else
    toSeconds(dt->tm_hour, dt->tm_min, dt->tm_sec);
}

Time::Time(time_t tm)
{
    struct tm dt;
    gmtime_r(&tm, &dt);
    toSeconds(dt.tm_hour, dt.tm_min, dt.tm_sec);
}

Time::Time(char *str, size_t size)
{
    setTime(str, size);
}

Time::Time(int hour, int minute, int second)
{
    toSeconds(hour, minute, second);
}

Time::~Time()
{}

bool Time::isValid(void) const
{
    if (seconds == -1)
        return false;
    return true;
}

char *Time::getTime(char *buf) const
{
    fromSeconds(buf);
    return buf;
}

time_t Time::getTime(void) const
{
    return static_cast<time_t>(seconds);
}

int Time::getHour(void) const
{
    char buf[7];
    fromSeconds(buf);
    ZNumber num(buf, 2);
    return num();
}

int Time::getMinute(void) const
{
    char buf[7];
    fromSeconds(buf);
    ZNumber num(buf + 2, 2);
    return num();
}

int Time::getSecond(void) const
{
    char buf[7];
    fromSeconds(buf);
    ZNumber num(buf + 4, 2);
    return num();
}

void Time::update(void)
{}

void Time::setTime(char *str, size_t size)
{
    int sec = 00;

    if (!size)
        size = strlen(str);

//00:00
    if (size == 5)
    {
        sec = 00;
    }
//00:00:00
    else if (size == 8)
    {
        ZNumber nsecond(str + 6, 2);
        sec = nsecond();
    }
    else
    {
        return;
    }

    ZNumber nhour(str, 2);
    ZNumber nminute(str+3, 2);
    toSeconds(nhour(), nminute(), sec);
}

string Time::operator()() const
{
    char buf[7];

    fromSeconds(buf);
    string strTime(buf);

    return strTime;
}

long Time::getValue(void) const
{
    return seconds;
}

Time& Time::operator++()
{
    ++seconds;
    update();
    return *this;
}

Time& Time::operator--()
{
    --seconds;
    update();
    return *this;
}

Time& Time::operator+=(const int val)
{
    seconds += val;
    update();
    return *this;
}

Time& Time::operator-=(const int val)
{
    seconds -= val;
    update();
    return *this;
}

int Time::operator==(const Time &t)
{
    return seconds == t.seconds;
}

int Time::operator!=(const Time &t)
{
    return seconds != t.seconds;
}

int Time::operator<(const Time &t)
{
    return seconds < t.seconds;
}

int Time::operator<=(const Time &t)
{
    return seconds <= t.seconds;
}

int Time::operator>(const Time &t)
{
    return seconds > t.seconds;
}

int Time::operator>=(const Time &t)
{
    return seconds >= t.seconds;
}

void Time::toSeconds(int hour, int minute, int second)
{
    seconds = -1;

    if (hour > 23 ||minute > 59 ||second > 59)
    {
        return;
    }

    seconds = 3600 * hour + 60 * minute + second;
}

void Time::fromSeconds(char *buffer) const
{
    ZNumber hour(buffer, 2);
    ZNumber minute(buffer + 2, 2);
    ZNumber second(buffer + 4, 2);

    hour = seconds / 3600;
    minute = (seconds - (3600 * hour())) / 60;
    second = seconds - (3600 * hour()) - (60 * minute());
    buffer[6] = '\0';
}

Time operator+(const Time &time1, const Time &time2)
{
    Time t;
    t.seconds = time1.seconds + time2.seconds;
    t.update();
    return t;
}

Time operator-(const Time &time1, const Time &time2)
{
    Time t;
    t.seconds = time1.seconds - time2.seconds;
    t.update();
    return t;
}

Time operator+(const Time &time, const int val)
{
    Time t = time;
    t.seconds += val;
    t.update();
    return t;
}

Time operator+(const int val, const Time &time)
{
    Time t = time;
    t.seconds += val;
    t.update();
    return t;
}

Time operator-(const Time &time, const int val)
{
    Time t = time;
    t.seconds -= val;
    t.update();
    return t;
}

Time operator-(const int val, const Time &time)
{
    Time t = time;
    t.seconds -= val;
    t.update();
    return t;
}

Datetime::Datetime(time_t tm)
{
    struct tm dt;
    gmtime_r(&tm, &dt);
    toJulian(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
    toSeconds(dt.tm_hour, dt.tm_min, dt.tm_sec);
}

Datetime::Datetime(tm *dt) :
        Date(dt), Time(dt)
{}

Datetime::Datetime(const char *a_str, size_t size)
{
    fromstr(a_str, size);
}

Datetime::Datetime(int year, unsigned month, unsigned day,
                   int hour, int minute, int second) :
        Date(year, month, day), Time(hour, minute, second)
{}

Datetime::Datetime() : Date((struct tm*)0), Time((struct tm*)0)
{
    time_t now = time(NULL);
    struct tm dt;
    gmtime_r(&now, &dt);
    toSeconds(dt.tm_hour, dt.tm_min, dt.tm_sec);
    toJulian(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
}

Datetime::~Datetime()
{}

void Datetime::refresh()
{
    time_t now = time(NULL);
    struct tm dt;
    gmtime_r(&now, &dt);
    toSeconds(dt.tm_hour, dt.tm_min, dt.tm_sec);
    toJulian(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
}

bool Datetime::isValid(void) const
{
    return Date::isValid() && Time::isValid();
}

bool Datetime::setDatetime(const char *a_str, size_t size)
{
    return fromstr(a_str, size);
}

void Datetime::getDatetime(struct tm *pdt) const
{
    char buf[11];
    memset(pdt, 0, sizeof(*pdt));

    fromJulian(buf);
    ZNumber nyear(buf, 4);
    ZNumber nmonth(buf + 5, 2);
    ZNumber nday(buf + 8, 2);
    pdt->tm_year = nyear() - 1900;
    pdt->tm_mon = nmonth() - 1;
    pdt->tm_mday = nday();

    fromSeconds(buf);
    ZNumber nhour(buf, 2);
    ZNumber nminute(buf + 2, 2);
    ZNumber nsecond(buf + 4, 2);
    pdt->tm_hour = nhour();
    pdt->tm_min = nminute();
    pdt->tm_sec = nsecond();
    pdt->tm_isdst = -1;
}

Datetime& Datetime::operator=(const Datetime datetime)
{
    julian = datetime.julian;
    seconds = datetime.seconds;
    return *this;
}

Datetime& Datetime::operator+=(const Datetime &datetime)
{
    seconds += datetime.seconds;
    julian += datetime.julian;
    Date::update();
    Time::update();
    return *this;
}

Datetime& Datetime::operator-=(const Datetime &datetime)
{
    seconds -= datetime.seconds;
    julian -= datetime.julian;
    Date::update();
    Time::update();
    return *this;
}

Datetime& Datetime::operator+=(const Time &time)
{
    seconds += time.getValue();
    Date::update();
    Time::update();
    return *this;
}

Datetime& Datetime::operator-=(const Time &time)
{
    seconds -= time.getValue();
    Date::update();
    Time::update();
    return *this;
}

int Datetime::operator==(const Datetime &d)
{
    return (julian == d.julian) && (seconds == d.seconds);
}

int Datetime::operator!=(const Datetime &d)
{
    return (julian != d.julian) || (seconds != d.seconds);
}

int Datetime::operator<(const Datetime &d)
{
    if (julian != d.julian)
    {
        return (julian < d.julian);
    }
    else
    {
        return (seconds < d.seconds);
    }
}

int Datetime::operator<=(const Datetime &d)
{
    if (julian != d.julian)
    {
        return (julian < d.julian);
    }
    else
    {
        return (seconds <= d.seconds);
    }
}

int Datetime::operator>(const Datetime &d)
{
    if (julian != d.julian)
    {
        return (julian > d.julian);
    }
    else
    {
        return (seconds > d.seconds);
    }
}

int Datetime::operator>=(const Datetime &d)
{
    if (julian != d.julian)
    {
        return (julian > d.julian);
    }
    else
    {
        return (seconds >= d.seconds);
    }
}

bool Datetime::operator!() const
{
    return !(Date::isValid() && Time::isValid());
}

string Datetime::strftime(const char *format,bool utc) const
{
    char buffer[64];
    size_t last;
    struct tm tbp;
    string retval;
    getDatetime(&tbp);

    if(!utc)
    {
        time_t t = timegm(&tbp);
        localtime_r(&t, &tbp);
    }

    last = ::strftime(buffer, 64, format, &tbp);
    buffer[last] = '\0';
    retval = buffer;
    return retval;
}

time_t Datetime::tointeger() const
{
    struct tm tbp;
    getDatetime(&tbp);
    return timegm(&tbp);
}

string Datetime::tostr(bool utc)
{
    return strftime("%Y-%m-%d %X",utc);
}

bool Datetime::fromstr(const char *a_str, size_t size)
{
    char *timestr;

    if (!size)
        size = strlen(a_str);

    char *str = new char[size+1];
    strncpy(str, a_str, size);
    str[size]=0;

// 00/00 00:00
    if (size ==  11)
    {
        timestr = str + 6;
        setDate(str, 5);
        setTime(timestr, 5);
    }
// 00/00/00 00:00
    else if (size == 14)
    {
        timestr = str + 9;
        setDate(str, 8);
        setTime(timestr,5);
    }
// 00/00/00 00:00:00
    else if (size == 17)
    {
        timestr = str + 9;
        setDate(str, 8);
        setTime(timestr,8);
    }
// 0000/00/00 00:00:00
    else if (size == 19)
    {
        timestr = str + 11;
        setDate(str, 10);
        setTime(timestr,8);
    }
    delete []str;
    return true;
}

string Datetime::serialize() const
{
    return strftime("%Y-%m-%d %X",true);
}

bool Datetime::deserialize(const string& input)
{
    return fromstr(input.c_str(),input.length());
}
    
DateNumber::DateNumber(char *str) :
        Number(str, 10), Date(str, 10)
{}

DateNumber::~DateNumber()
{}


