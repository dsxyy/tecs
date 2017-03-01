/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：algorithm.cpp
* 文件标识：见配置管理计划书
* 内容摘要：主要是移植和重新实现了boost::algorithm中的一些算法
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年5月14日
*
* 修改记录1：
*     修改日期：2012/05/14
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
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

