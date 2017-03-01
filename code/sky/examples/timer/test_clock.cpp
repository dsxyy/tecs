#include "sky.h"
int main()
{
    T_Clock clock;
    GetLocalClock(&clock);
    cout << ClockToString(clock) << endl;
}

