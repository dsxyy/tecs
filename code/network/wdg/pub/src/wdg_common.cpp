/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：wdg_common.cpp
* 文件标识：见配置管理计划书
* 内容摘要：WDG公共功能函数实现文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2012年12月15日
*******************************************************************************/
#include "vnet_comm.h"
#include "wdg_common.h"
#include <algorithm>

/************************************************************
* 函数名称： GetVNAUUIDByVNA
* 功能描述： 获取VNA的application，供VNA进程调用，如果没有则创建
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int GetWDGUUID(string &strWDGUUID)
{
    strWDGUUID.clear();
    string strWdgApp;
    
    ostringstream file;
    file << WDG_UUID_FILE ;
    ifstream fin(file.str().c_str());
    fin >> strWdgApp;

    if ("" == strWdgApp)
    {
        if (0 != access(WDG_UUID_FILE, 0))
        {
            ostringstream oss;
            oss<< "mkdir -p " << WDG_UUID_DIR;
            if (0 != RunCmd(oss.str()))
            {
                VNET_LOG(VNET_LOG_ERROR, "GetWDGUUID: call system(%s) failed\n",oss.str().c_str());
                return -1;
            }
        }

        string tmpWDGUUID;
        if (GetUUID(tmpWDGUUID))
        {
            return -1;
        }

        char hostname[256] = "";

        gethostname(hostname, sizeof(hostname));
        
        //hostname最多取20个字节，app的长度有64的限制
        if (sizeof(hostname) > 20)
        {
            hostname[20]=0;
        }
        
        strWdgApp = hostname;
        strWdgApp += "_";
        strWdgApp += tmpWDGUUID;

        ostringstream file;
        file << WDG_UUID_FILE;
        ofstream fout(file.str().c_str(), ios::out);
        fout << strWdgApp;
        
        strWDGUUID = strWdgApp;
    }
    else
    {
        strWDGUUID = strWdgApp;
    }
    
    return 0;
}

/************************************************************
* 函数名称： GetVNAUUIDByLib
* 功能描述： 获取VNA的application，供业务进程调用，如果没有则返回空
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int GetWDGUUIDByLib(string &strWDGUUID)
{
    strWDGUUID.clear();
    string tmpWDGUUID;

    if (0 != access(WDG_UUID_FILE, 0))
    {
        return -1;
    }
            
    ostringstream file;
    file << WDG_UUID_FILE ;
    ifstream fin(file.str().c_str());
    fin >> tmpWDGUUID;

    strWDGUUID = tmpWDGUUID;
    
    return 0;
}

/************************************************************
* 函数名称： GetHostBoisUUID
* 功能描述： 获取主机BOIS的UUID函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
int GetUUID(string & strUUID)
{
    ostringstream oss;

    oss.str("");
    //oss<< "dmidecode | grep 'UUID'|awk '{print $2}' ";
    oss<< "cat /proc/sys/kernel/random/uuid";

    vector<string> vUUID;
    if (0 != RunCmd(oss.str(), vUUID))
    {
        VNET_LOG(VNET_LOG_ERROR, "GetUUID: call system(%s) failed\n",oss.str().c_str());
        return -1;
    }
    
    vector<string>::iterator itUUID;
    if (vUUID.begin() != vUUID.end())
    {
        itUUID = vUUID.begin();
        strUUID = (*itUUID);
    }
    else
    {
        VNET_LOG(VNET_LOG_ERROR, "GetUUID: call system(%s) failed\n",oss.str().c_str());
        return -1;
    }
    
    remove( strUUID.begin(), strUUID.end(), '-');

    return 0;
}


