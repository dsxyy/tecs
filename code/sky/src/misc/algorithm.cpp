/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�algorithm.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ����Ҫ����ֲ������ʵ����boost::algorithm�е�һЩ�㷨
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��5��14��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/05/14
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "misc.h"

string JoinStrings(const vector<string>& strings,const string& separator)
{
    string result;
    vector<string>::const_iterator it;
    for(it = strings.begin(); it != strings.end(); it++)
    {
        result.append(*it);
        if(it != strings.end() - 1)
        {
            result.append(separator);
        }
    }
    return result;
}

#if 0
string JoinStrings(const list<string>& strings,const string& separator)
{
    string result;
    list<string>::const_iterator it;
    for(it = strings.begin(); it != strings.end(); it++)
    {
        result.append(*it);
        if(it != strings.end() - 1)
        {
            result.append(separator);
        }
    }
    return result;
}
#endif

