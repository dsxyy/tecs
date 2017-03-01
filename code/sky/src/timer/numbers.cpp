#include "common.h"
#include "datetime.h"

Number::Number(char *buf, unsigned width)
{
    if(width > 10)
        width = 10;
    if(width < 1)
        width = 1;
    size = width;

    buffer = buf;
}

long Number::getValue(void) const
{
    int count = size;
    bool sign = false;
    long ret = 0;
    char *bp = buffer;

    if(*bp == '-') {
        --count;
        ++bp;
        sign = true;
    }
    else if(*bp == '+') {
        --count;
        ++bp;
    }
    while(count && *bp >='0' && *bp <='9') {
        ret = ret * 10l + (*bp - '0');
        --count;
        ++bp;
    }

    if(sign)
        ret = -ret;
    return ret;
}

void Number::setValue(long value)
{
    int count = size;
    char *bp = buffer;
    long max = 1;
    int exp;
    bool z = false;

    if(value < 0) {
        value = -value;
        --count;
        *(bp++) = '-';
    }

    exp = count;
    while(--exp)
        max *= 10;

    while(max) {
        if(value >= max || z) {
            --count;
            *(bp++) = '0' + ((char)(value / max));
        }
        if(value >= max) {
            z = true;
            value -= (value / max) * max;
        }
        max /= 10;
    }
    while(count-- && *bp >= '0' && *bp <='9')
        *(bp++) = ' ';
}

long Number::operator=(long value)
{
    setValue(value);
    return value;
}

long Number::operator+=(const long value)
{
    long value1 = getValue() + value;
    setValue(value1);
    return value1;
}

long Number::operator-=(const long value)
{
    long value1 = getValue() - value;
    setValue(value1);
    return value1;
}

long Number::operator--()
{
    long val = getValue();
    setValue(--val);
    return val;
}

long Number::operator++()
{
    long val = getValue();
    setValue(++val);
    return val;
}

int Number::operator==(const Number &num)
{
    return getValue() == num.getValue();
}

int Number::operator!=(const Number &num)
{
    return getValue() != num.getValue();
}

int Number::operator<(const Number &num)
{
    return getValue() < num.getValue();
}

int Number::operator<=(const Number &num)
{
    return getValue() <= num.getValue();
}

int Number::operator>(const Number &num)
{
    return getValue() > num.getValue();
}

int Number::operator>=(const Number &num)
{
    return getValue() >= num.getValue();
}

ZNumber::ZNumber(char *buf, unsigned chars) :
Number(buf, chars)
{}

void ZNumber::setValue(long value)
{
    int count = size;
    char *bp = buffer;
    long max = 1;
    int exp;

    if(value < 0) {
        value = -value;
        --count;
        *(bp++) = '-';
    }

    exp = count;
    while(--exp)
        max *= 10;

    while(max) {
        --count;
        *(bp++) = '0' + (char)(value / max);
        value -= (value / max) * max;
        max /= 10;
    }
}

long ZNumber::operator=(long value)
{
    setValue(value);
    return value;
}

long operator+(const Number &num, long val)
{
    return num.getValue() + val;
}

long operator+(long val, const Number &num)
{
    return num.getValue() + val;
}

long operator-(const Number &num, long val)
{
    return num.getValue() - val;
}

long operator-(long val, const Number &num)
{
    return num.getValue() - val;
}


