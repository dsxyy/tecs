#ifndef SKY_LEX_TOKEN_H
#define SKY_LEX_TOKEN_H

//0~256已经被ASCII字符占用
#define IDENTIFIER          300
#define INTEGER_DEC         301
#define INTEGER_HEX         302
#define STRING              303
#define EMPTY               0

inline bool IsConstant(int flag) 
{
    return (flag == INTEGER_HEX || flag == INTEGER_DEC || flag == STRING);
}
#endif

