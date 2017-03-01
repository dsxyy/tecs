/*******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：exitpoint.cpp
* 文件标识：见配置管理计划书
* 内容摘要：退出点
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2013年5月31日
*
* 修改记录1：
*     修改日期：2013/05/31
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "common.h"
#include "misc.h"

static map<string,bool> exit_points;
static Mutex points_mutex;

STATUS ExitPointInit()
{
    return points_mutex.Init();
}

void CheckExitPoint(const string& file, const string& function,int line)
{
    //由于性能问题，本功能仅在debug模式下才有效
    if(false == IsDebug())
    {
        return;
    }
    
    ostringstream name;
    name << file << ":" << function << ":" << line;
    MutexLock lock (points_mutex);
    map<string,bool>::iterator it = exit_points.find(name.str());
    if(it != exit_points.end()) 
    {
        if(true == it->second)
        {
            string lastwords = "process will exit at exit point " + it->first + "!";
            SkyExit(-1,lastwords);
        }
    }
    else
    {
        exit_points.insert(make_pair(name.str(),false));
    }
}

static STATUS ModifyExitPoint(const string& name,bool state)
{
    MutexLock lock (points_mutex);
    map<string,bool>::iterator it = exit_points.find(name);
    if(it == exit_points.end()) 
    {
        printf("no exit point: %s!\n",name.c_str());
        return ERROR_OBJECT_NOT_EXIST;
    }
    it->second = state;
    return SUCCESS;
}

static string InputExitPoint()
{
    map<int,string> tmp;
    int i = 1;
    map<string,bool>::iterator it;
    points_mutex.Lock();
    for(it = exit_points.begin(); it != exit_points.end(); ++it) 
    {
        if(true == it->second)
        {
            cout << i << ". " << it->first << " --- on" << endl;
        }
        else
        {
            cout << i << ". " << it->first << " --- off" << endl;
        }
        
        tmp.insert(make_pair(i,it->first));
        i++;
    }
    points_mutex.Unlock();

    int count = tmp.size();
    if(count == 0)
    {
        printf("No exit points.\n");
        return "";
    }

    string input;
    int index = 0;
    while(1)
    {
        cout << "Please input a correct index(" << 1 << "~" << count <<", 0 to exit): ";
        getline(cin,input);
        if(input.empty())
        {
            continue;
        }

        index = atoi(input.c_str());
        if(0 == index)
        {
            return "";
        }
        
        if(index < 1 || index > count)
        {
            printf("illegal index: %d\n",index);
            continue;
        }

        break;
    }

    return tmp[index];
}

void DbgEnableExitPoint()
{
    if(false == IsDebug())
    {
        printf("exit points will only work in debug mode!!!\n");
        return;
    }

    string name = InputExitPoint();
    if(name.empty())
    {
        return;
    }

    printf("enable exit point at %s...\n",name.c_str());
    STATUS ret = ModifyExitPoint(name,true);
    if(SUCCESS != ret)
    {
        printf("failed to enable exit point: %s!\n",name.c_str());
    }

    printf("ok!\n");
    return;
}
DEFINE_DEBUG_FUNC(DbgEnableExitPoint);
DEFINE_DEBUG_CMD(exitpoint,DbgEnableExitPoint);

void DbgDisableExitPoint()
{
    if(false == IsDebug())
    {
        printf("exit points will only work in debug mode!!!\n");
        return;
    }

    string name = InputExitPoint();
    if(name.empty())
    {
        return;
    }

    printf("disable exit point at %s...\n",name.c_str());
    STATUS ret = ModifyExitPoint(name,false);
    if(SUCCESS != ret)
    {
        printf("failed to disable exit point: %s!\n",name.c_str());
    }
    printf("ok!\n");
    return;
}
DEFINE_DEBUG_FUNC(DbgDisableExitPoint);



