/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：identity.cpp
* 文件标识：
* 内容摘要：GetRegisteredSystem接口的实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年9月22日
*
* 修改记录1：
*     修改日期：2011/9/22
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "registered_system.h"

static string registered_system_name;

/******************************************************************************/
STATUS GetRegisteredSystem(string &reg_system)
{
    reg_system = registered_system_name;

    if (reg_system.empty())
    {
        return ERROR;
    }
    return SUCCESS;
}

/******************************************************************************/
void SetRegisteredSystem(const string &reg_system)
{
    // 无改变
    if (registered_system_name == reg_system)
    {
        return;
    }

    // 从无到有
    if (registered_system_name.empty())
    {
        registered_system_name = reg_system;
        return;
    }

    // 从有到无
    if (reg_system.empty())
    {
        exit(-1);
        return;
    }

    // 从有到有，异常
    SkyAssert(0);
    exit(-1);
    return;
}

/******************************************************************************/
void WaitRegisteredSystem(void)
{
    cerr << "Waite register into reg_system......";
    string reg_system;
    for(;;)
    {
        if (SUCCESS == GetRegisteredSystem(reg_system))
        {
            break;
        }
        else
        {
            sleep(1);
            cerr << ".";
        }
    }
    cerr << endl << "Have registered into reg_system: " 
         << reg_system << endl;
}

void DbgShowRegisteredSystem()
{
    cout << "registered system = " << registered_system_name << endl;
}
DEFINE_DEBUG_CMD(regto,DbgShowRegisteredSystem);


