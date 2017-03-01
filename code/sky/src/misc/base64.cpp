/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�base64.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��base64������ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "misc.h"

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
string base64_encode(const unsigned char *buffer, int size)
{
    vector<unsigned char> base64string;
    unsigned char in[3], out[4];
    int i, len;
    int total = 0;
    while (1)
    {
        len = 0;
        for (i = 0; i < 3; i++)
        {
            if (total < size)
            {
                in[i] = *(buffer + total);
                len++;
                total++;
            }
            else
            {
                in[i] = 0;
            }
        }

        if (len)
        {
            encodeblock(in, out, len);

            for (i = 0; i < 4; i++)
            {
                base64string.push_back(out[i]);
            }
        }
        else
        {
            break;
        }
    }

    string s(base64string.begin(), base64string.end());
    return s;
}


/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
void decodeblock( unsigned char in[4], unsigned char out[3] )
{
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
void base64_decode(const string& base64string, unsigned char *buffer)
{
    unsigned char in[4], out[3], v;
    int i, len;

    vector<unsigned char> input(base64string.begin(), base64string.end());
    vector<unsigned char>::iterator it = input.begin();
    
    while (it < input.end())
    {
        for ( len = 0, i = 0; i < 4 && it < input.end(); i++ )
        {
            v = 0;
            while ( it < input.end() && v == 0 )
            {
                v = *it;
                it ++;
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if ( v )
                {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
            if ( it < input.end() )
            {
                len++;
                if ( v )
                {
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else
            {
                in[i] = 0;
            }
        }
        
        if ( len )
        {
            decodeblock( in, out );
            for ( i = 0; i < len - 1; i++ )
            {
                *(buffer++) = out[i];
            }
        }
    }
}


