/******************************************************************************************
* 版权所有 (C)2011, 中兴通讯股份有限公司。
*
*   All rights reserved
*
* 文件名称：smart.cpp
* 文件标识：
* 内容摘要：S.M.A.R.T对外接口实现文件
* 其它说明：
* 当前版本： 1.0
* 作    者： gjsh
* 完成日期： 2011-08-20
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
* 修改日期：2013-07-25
* 版 本 号：2.0
* 修 改 人：谢涛涛
* 修改内容：改为使用smartctl命令脚本的方式
* 修改记录2：…
* 修 改 人：
* 修改日期：
* 修改内容：
****************************************************************************************/

#include <iostream>
#include <vector>
#include <list>
#include "lvm.h"
#include "smart.h"
#include "log_agent.h"

using namespace std;

#if 0 //调用smartctl命令后这些函数暂不用了
extern int dmGetHddSmart(T_DmPdSmartInfo *pPdSmartInfo);  
extern int dmExecHddSmartSelfTest(unsigned int dwSelfTestType, T_DmPdSmartInfo *pPdSmartInfo);
#endif
extern int GetDiskPath(vector<string>& vec_disk);
extern int GetDiskInterface(const string& disk, T_DmPdSmartInfo& smart_info);
extern int ATADiskParamSenseRule(const string value_cmd,
                                const string thresh_cmd,
                                const string type_cmd, 
                                int& status);
extern int SenseATADisk(const string& disk, int& status);
extern int SenseSCSIDisk(const string& disk, int& status);

namespace zte_tecs {

/******************************************************************************/
DiskSmart:: DiskSmart()
{
    
}

/******************************************************************************/
DiskSmart:: ~DiskSmart() 
{
    
}

/******************************************************************************/
int DiskSmart:: diskSmartTest(unsigned int testType)
{
#if 0
    T_DmPdSmartInfo         smartInfo; 
    list<string>            diskList;
    int                     ret;
    
    
    if(!getDiskName(diskList))
    {
        OutPut(SYS_ERROR, "LvmDiskManage getDiskName failed");
        return _SMART_ERROR;
    }

    for(list<string>::iterator it = diskList.begin(); it != diskList.end(); it++)
    {
        memset(&smartInfo, 0, sizeof(smartInfo));
        strcpy(smartInfo.aucPdName, (*it).c_str());
        ret = dmExecHddSmartSelfTest(testType, &smartInfo);
        if(_SMART_SUCCESS != ret)
        {
            OutPut(SYS_ERROR, "dmExecHddSmartSelfTest failed, ret: %u, dname: %s", ret, smartInfo.aucPdName);
        }
        //记录检测结果，方便调试
        
    }
#endif
    return _SMART_SUCCESS;
}

/******************************************************************************/
/* 获取磁盘SMART信息 */
int DiskSmart::getSmartInfo(vector<T_DmPdSmartInfo> &diskSmartInfo)
{
    T_DmPdSmartInfo         smart_info;
    vector<string> vec_disk;
    int    status = -1;
    int    ret = -1;
    /* 使用路径如/dev/sda标识一个disk
     */
    ret = GetDiskPath(vec_disk);
    if (SUCCESS != ret)
    {
        return ERROR;
    }
    vector<string>::iterator it;
    for (it  = vec_disk.begin();
         it != vec_disk.end();
         it++)
    {
        strncpy(smart_info.aucPdName, it->c_str(), DM_PD_NAME_LEN);
        // 检测disk的接口类型, 因不同接口(如: ATA/SCSI)的disk走不同的路线
        ret = GetDiskInterface(*it, smart_info);
        if (SUCCESS != ret)
        {
            continue;
        }
        if (DISK_TYPE_ATA == smart_info.ucDiskType)
        {
            // ATA disk检测
            ret = SenseATADisk(*it, status);
            if (ERROR == ret)
                continue;
        }
        else if (DISK_TYPE_SCSI == smart_info.ucDiskType)
        {
            /* SCSI disk使用smartctl -H直接借用此命令的
             * 健康检测功能判断是否上报告警
             */
            ret = SenseSCSIDisk(*it, status);
            if (ERROR == ret)
                continue;
        }
        else
        {
            OutPut(SYS_WARNING, "SMART do not Support or disable! Or Unknown disk interface!");
            continue;
        }
        smart_info.ucSmartStatus = status;
        diskSmartInfo.push_back(smart_info);
    }
    return SUCCESS;
}

/* test code*/
void smart_test(char *devname)
{
#if 0
    T_DmPdSmartInfo tInfo;
    int ret;

    if(NULL == devname)
    {
        printf("The input device name is invalid!\n");
        return;
    }

    memset(&tInfo, 0, sizeof(T_DmPdSmartInfo));
    strcpy(tInfo.aucPdName, devname);
    ret = dmExecHddSmartSelfTest(PD_LONG_SELFTEST, &tInfo);
    printf("ret:%u\n tInfo.aucPdName:%s\n tInfo.ucIsSupportSmart:%u\n tInfo.ucIsSmartEnable:%u\n tInfo.ucSmartStatus:%u\n tInfo.sdwSelfTestTime:%u\n",
        ret, tInfo.aucPdName, tInfo.ucIsSupportSmart, tInfo.ucIsSmartEnable, tInfo.ucSmartStatus, tInfo.sdwSelfTestTime);    


    memset(&tInfo, 0, sizeof(T_DmPdSmartInfo));
    strcpy(tInfo.aucPdName, devname);
    ret = dmGetHddSmart(&tInfo);
    printf("ret:%u\n tInfo.aucPdName:%s\n tInfo.ucIsSupportSmart:%u\n tInfo.ucIsSmartEnable:%u\n tInfo.ucSmartStatus:%u\n tInfo.sdwSelfTestTime:%u\n",
        ret, tInfo.aucPdName, tInfo.ucIsSupportSmart, tInfo.ucIsSmartEnable, tInfo.ucSmartStatus, tInfo.sdwSelfTestTime);    
#endif 
    return;
}

}// namespace zte_tecs

//end of file.

