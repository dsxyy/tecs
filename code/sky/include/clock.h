#ifndef SKY_CLOCK_H
#define SKY_CLOCK_H

typedef struct tagClock
{
    int year;      
    int month;     
    int day;       
    int week;      
    int hour;      
    int minute;    
    int second;   
}T_Clock;

class Timespan
{
public:
    Timespan():_name("anonymous")    
    {
        _span = 0;
    };
    
    Timespan(const string& name):_name(name)    
    {
        _span = 0;
    };

    void Begin()
    {
        _span = 0;
        clock_gettime(CLOCK_MONOTONIC,&_begin);
    };

    void End()
    {
        clock_gettime(CLOCK_MONOTONIC,&_end);
        uint64 begin_second = _begin.tv_sec;
        uint64 begin_usec = _begin.tv_nsec / 1000;
        uint64 end_second = _end.tv_sec;
        uint64 end_usec = _end.tv_nsec / 1000;
        _span = (end_second - begin_second) * 1000 * 1000 + (end_usec - begin_usec);
    };

    uint64 GetSpan()
    {
        return _span;
    };
    
    void ShowSpan()
    {
        printf("clock %s span = %llu microseconds\n",_name.c_str(),_span);
    };
    
private:
    uint64 _span;
    string _name;
    struct timespec _begin;
    struct timespec _end;
};


void Delay(uint32 miliseconds);
STATUS GetLocalClock(T_Clock *pclock);
STATUS GetStdClock(T_Clock *pclock);
string ClockToString(const T_Clock& clock);

#endif

