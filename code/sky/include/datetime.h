#ifndef SKY_DATETIME_H
#define SKY_DATETIME_H

/**
 * A number manipulation class.  This is used to extract, convert,
 * and manage simple numbers that are represented in C ascii strings
 * in a very quick and optimal way.
 */
class Number
{
protected:
    char *buffer;
    unsigned size;

public:
    /**
     * Create an instance of a number.
     * @param buffer or NULL if created internally.
     * @param size use - values for zero filled.
     */
    Number(char *buffer, unsigned size);

    void setValue(long value);
    const char *getBuffer() const
        {return buffer;};

    long getValue() const;

    long operator()()
        {return getValue();};

    operator long()
        {return getValue();};

    operator char*()
        {return buffer;};

    long operator=(const long value);
    long operator+=(const long value);
    long operator-=(const long value);
    long operator--();
    long operator++();
    int operator==(const Number &num);
    int operator!=(const Number &num);
    int operator<(const Number &num);
    int operator<=(const Number &num);
    int operator>(const Number &num);
    int operator>=(const Number &num);

    friend long operator+(const Number &num, const long val);
    friend long operator+(const long val, const Number &num);
    friend long operator-(const Number &num, long val);
    friend long operator-(const long val, const Number &num);
};

class ZNumber : public Number
{
public:
    ZNumber(char *buf, unsigned size);
    void setValue(long value);
    long operator=(long value);
};

/**
 * The Date class uses a julian date representation of the current
 * year, month, and day.  This is then manipulated in several forms
 * and may be exported as needed.
 * @short julian number based date class.
 */
class Date
{
protected:
    long julian;

protected:
    void toJulian(long year, long month, long day);
    void fromJulian(char *buf) const;

    /**
     * A method to use to "post" any changed values when shadowing
     * a mixed object class.  This is used by DateNumber.
     */
    virtual void update(void);

public:
    void refresh();
    Date(time_t tm);
    Date(tm *dt);
    Date(char *str, size_t size = 0);
    Date(int year, unsigned month, unsigned day);
    Date();
    virtual ~Date();

    int getYear(void) const;
    unsigned getMonth(void) const;
    unsigned getDay(void) const;
    unsigned getDayOfWeek(void) const;
    char *getDate(char *buffer) const;
    time_t getDate(void) const;
    time_t getDate(tm *buf) const;
    long getValue(void) const;
    void setDate(const char *str, size_t size = 0);
    bool isValid(void) const;

    friend Date operator+(const Date &date, const long val);
    friend Date operator-(const Date &date, const long val);
    friend Date operator+(const long val, const Date &date);
    friend Date operator-(const long val, const Date &date);

    operator long() const
        {return getValue();};

    string operator()() const;
    Date& operator++();
    Date& operator--();
    Date& operator+=(const long val);
    Date& operator-=(const long val);
    int operator==(const Date &date);
    int operator!=(const Date &date);
    int operator<(const Date &date);
    int operator<=(const Date &date);
    int operator>(const Date &date);
    int operator>=(const Date &date);
    bool operator!() const
        {return !isValid();};
};

/**
 * The Time class uses a integer representation of the current
 * time.  This is then manipulated in several forms
 * and may be exported as needed.
 * @short Integer based time class.
 */

class Time
{
protected:
    long seconds;

protected:
    void toSeconds(int hour, int minute, int second);
    void fromSeconds(char *buf) const;
    virtual void update(void);

public:
    void refresh();
    Time(time_t tm);
    Time(tm *dt);
    Time(char *str, size_t size = 0);
    Time(int hour, int minute, int second);
    Time();
    virtual ~Time();

    long getValue(void) const;
    int getHour(void) const;
    int getMinute(void) const;
    int getSecond(void) const;
    char *getTime(char *buffer) const;
    time_t getTime(void) const;
    tm *getTime(tm *buf) const;
    void setTime(char *str, size_t size = 0);
    bool isValid(void) const;

    friend Time operator+(const Time &time1, const Time &time2);
    friend Time operator-(const Time &time1, const Time &time2);
    friend Time operator+(const Time &time, const int val);
    friend Time operator-(const Time &time, const int val);
    friend Time operator+(const int val, const Time &time);
    friend Time operator-(const int val, const Time &time);

    operator long()
        {return getValue();};

    string operator()() const;
    Time& operator++();
    Time& operator--();
    Time& operator+=(const int val);
    Time& operator-=(const int val);
    int operator==(const Time &time);
    int operator!=(const Time &time);
    int operator<(const Time &time);
    int operator<=(const Time &time);
    int operator>(const Time &time);
    int operator>=(const Time &time);
    bool operator!() const
        {return !isValid();};
};

/**
 * The Datetime class uses a julian date representation of the current
 * year, month, and day and a integer representation of the current
 * time.  This is then manipulated in several forms
 * and may be exported as needed.
 * @short Integer based time class.
 */
class Serializable;
class Datetime : public Date, public Time, public Serializable
{
  public:
    void refresh();
    Datetime(time_t tm);
    Datetime(tm *dt);
    Datetime(const char *str, size_t size = 0);
    Datetime(int year, unsigned month, unsigned day, int hour, int minute, int second);
    Datetime();
    virtual ~Datetime();
    
    bool setDatetime(const char *a_str, size_t size = 0);
    void getDatetime(struct tm *pdt) const;
    bool isValid(void) const;

    Datetime& operator=(const Datetime datetime);
    Datetime& operator+=(const Datetime &datetime);
    Datetime& operator-=(const Datetime &datetime);
    Datetime& operator+=(const Time &time);
    Datetime& operator-=(const Time &time);

    int operator==(const Datetime&);
    int operator!=(const Datetime&);
    int operator<(const Datetime&);
    int operator<=(const Datetime&);
    int operator>(const Datetime&);
    int operator>=(const Datetime&);
    bool operator!() const;
    time_t tointeger() const;
    string tostr(bool utc = true);
    bool fromstr(const char *a_str, size_t size);
    string strftime(const char *format,bool utc = true) const;
    string serialize() const;
    bool deserialize(const string& input);
private:
};

/**
 * A number class that manipulates a string buffer that is also a date.
 * @short a number that is also a date string.
 */
class DateNumber : public Number, public Date
{
protected:
    void update(void)
        {fromJulian(buffer);};

public:
    DateNumber(char *buffer);
    virtual ~DateNumber();
};
#endif

