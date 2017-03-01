/******************************************************************************************
* 版权所有 (C)2011, 中兴通讯股份有限公司。
*
*   All rights reserved
*
* 文件名称：smart_api.cpp
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
* 修 改 人：xiett
* 修改内容：改为使用smartctl命令脚本的方式
* 修改记录2：…
* 修 改 人：
* 修改日期：
* 修改内容：
****************************************************************************************/
#include    <fcntl.h>
#include    <stdio.h>
#include    <errno.h>
#include    <time.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stdlib.h>
#include    <dirent.h>
#include    <getopt.h>
#include    <dirent.h>
#include    <sys/stat.h>
#include    <ctype.h>
#include    <unistd.h>
#include    <time.h>
#include    "smart_dbg.h"
#include    "smart_api.h"
#include    "sky.h"

/******************************************************************************/
/* brief: 使用如下命令获取磁盘路径
 *        ls -l /sys/dev/block | grep pci | grep -E '[^0-9]$ | grep -v usb'
 * in   :
 * out  : disk_path - 记录磁盘路径如/dev/sda
 * return: SUCCESS/ERROR
 * note : 
 *   1) disk_path 和 disk_name 的下标是一一对应的
 *   2) 获取到是本地真实的物理磁盘, 过滤掉逻辑盘, 磁阵盘, USB盘(待测)
 */
int GetDiskPath(vector<string>& vec_disk)
{
    string cmd = "ls -l /sys/dev/block | grep pci | grep -E '[^0-9]$' | grep -v usb";
    string value;
    int ret = RunCmd(cmd, value);
    if (value.empty() || (SUCCESS != ret))
    {
        dm_debug("RunCmd failed or get no value! ret = %d\n", ret);
        return ERROR;
    }
    string::size_type pos = 0;
    string line, disk;
    stringstream iss(value);
    while (getline(iss, line))
    {
        pos = 0;
        disk.clear();
        pos = line.find_last_of('/', line.size());
        if (line.npos != pos)
        {// 查找到设备名, 其对应/dev/name
            disk = "/dev/" + line.substr(pos+1, line.size());
            vec_disk.push_back(disk);
        }
    }
    return SUCCESS;
}

/******************************************************************************/
/* brief: 使用smartctl -i区别ATA disk和SCSI disk
 *     ATA: grep '=== START OF INFORMATION SECTION ==='
 *    SCSI: grep 'Transport protocol:'
 *       ps: 依据为smartctl源码
 * in  : disk - disk, 如/dev/sda
 * out : smart_info中ucIsSupportSmart, ucIsSmartEnable, ucDiskType
 * return: SUCCESS/ERROR
 * note:
 */
int GetDiskInterface(const string& disk, T_DmPdSmartInfo& smart_info)
{
    // 检查入参
    if (disk.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
    // 构造脚本smartctl -i disk
    // 检测是否支持SMART
    string cmd = "smartctl -i " + disk + "|grep -E '(SMART support is: Available)|(Device supports SMART)'";
    string str_isSupport;
    int ret = RunCmd(cmd, str_isSupport);
    if ((SUCCESS != ret) || str_isSupport.empty())
    {
        dm_debug("RunCmd failed or disk do not support SMART! ret = %d\n", ret);
        smart_info.ucIsSupportSmart = PD_NOT_SUPPORT_SMART;
        return ERROR;
    }
    smart_info.ucIsSupportSmart = PD_SUPPORT_SMART;
    // 检测SMART是否使能
    cmd = "smartctl -i " + disk + "|grep -E '(SMART support is: Enabled)|(Device supports SMART and is Enabled)'";
    string str_isEnable;
    ret = RunCmd(cmd, str_isEnable);
    if ((SUCCESS != ret) || str_isEnable.empty())
    {
        dm_debug("RunCmd failed or disk SMART switch is disable! ret = %d\n", ret);
        smart_info.ucIsSmartEnable = PD_SMART_OFF;
        return ERROR;
    }
    smart_info.ucIsSmartEnable = PD_SMART_ON;
    // 先检测是否是 ATA 硬盘
    cmd = "smartctl -i " + disk + "|grep -E '=== START OF INFORMATION SECTION ==='";
    string str_ata;
    ret = RunCmd(cmd, str_ata);
    if (!str_ata.empty())
    {
        smart_info.ucDiskType = DISK_TYPE_ATA;
    }
    else
    {
        // 再检测是否是 SCSI 硬盘
        cmd = "smartctl -i " + disk + "|grep -E 'Transport protocol:'";
        string str_scsi;
        ret = RunCmd(cmd, str_scsi);
        if (!str_scsi.empty())
        {
            smart_info.ucDiskType = DISK_TYPE_SCSI;
        }
    }
    return SUCCESS;
}

/******************************************************************************/
/* brief: ATA disk 关键参数判断准则
 * in   : value_cmd - 获取记录中VALUE值
 *        thresh_cmd - 获取记录中THRESH值
 *        type_cmd - 获取记录中TYPE值
 * out  : status - 状态
 * return: SUCCESS/ERROR
 * note : 脚本借用 grep 和 awk 来摘出VALUE/THRESH/TYPE
 * From SFF 8035i Revision 2 page 19: Bit 0 (pre-failure/advisory bit)
 * - If the value of this bit equals zero, an attribute value less
 * than or equal to its corresponding attribute threshold indicates an
 * advisory condition where the usage or age of the device has
 * exceeded its intended design life period. If the value of this bit
 * equals one, an attribute value less than or equal to its
 * corresponding attribute threshold indicates a prefailure condition
 * where imminent loss of data is being predicted.
 */
int ATADiskParamSenseRule(const string value_cmd,
                                const string thresh_cmd,
                                const string type_cmd, 
                                int& status)
{
    if (value_cmd.empty() || thresh_cmd.empty() || type_cmd.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
    status = PD_STATUS_NORMAL; // 初始值为normal
    // 获取value值, 0-255
    string value;
    int ret = RunCmd(value_cmd, value);
    if ((SUCCESS != ret) || value.empty())
    {
        dm_debug("RunCmd failed or value is empty! ret = %d\n", ret);
        return ERROR;
    }
    int pos = value.find('\n', 0);
    value = value.erase(pos, value.size());
    int ivalue = atoi(value.c_str());

    // 获取threshold值, 0-255
    string thresh;
    ret = RunCmd(thresh_cmd, thresh);
    if ((SUCCESS != ret) || thresh.empty())
    {
        dm_debug("RunCmd failed or threshold is empty! ret = %d\n", ret);
        return ERROR;
    }
    pos = thresh.find('\n', 0);
    thresh = thresh.erase(pos, thresh.size());
    int ithresh = atoi(thresh.c_str());

    /* compare value and threshold
     * if value <= threshold, check type if 'Pre-fail'
     */
    if (ivalue <= ithresh)
    {
        // 获取type, 对应一个bit, 1: pre-fail, 0: Old_age
        string type;
        ret = RunCmd(type_cmd, type);
        if (value.empty())
        {
            dm_debug("RunCmd failed or type is empty! ret = %d\n", ret);
            return ERROR;
        }
        pos = type.find('\n', 0);
        type = type.erase(pos, type.size());
        if (!type.compare("Pre-fail"))
        {
            status = PD_STATUS_FAULT;
        }
        else
        {
            status = PD_STATUS_WARN;
        }
    }

    return SUCCESS;
}

/******************************************************************************/
/* brief: 使用smartctl -A获取ATA硬盘参数
 * in   : disk - disk, 如/dev/sda
 * out  : status - 硬盘状态,
 *                   0   init value
 *                   1   DISK_STATUS_NORMAL
 *                   2   DISK_STATUS_WARN
 *                   3   DISK_STATUS_FAULT
 * return: SUCCESS/ERROR
 * note : 暂使用一个参数如下, 如不够时, 可考虑使用数组
 *      5 Reallocated_Sector_Ct
 */
int SenseATADisk(const string& disk, int& status)
{
    if (disk.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
/* smartctl -A 打印记录格式如下:
ID# ATTRIBUTE_NAME          FLAG     VALUE WORST THRESH TYPE      UPDATED  WHEN_FAILED RAW_VALUE
  5 Reallocated_Sector_Ct   0x0033   200   200   140    Pre-fail  Always       -       0
199 UDMA_CRC_Error_Count    0x0032   200   200   000    Old_age   Always       -       0
 */
    status = 0; // 0为初始值
    /* process key param - 5 Reallocated_Sector_Ct
     */
    string value_cmd = "smartctl -A " + disk +
                       "| grep -E '  5 Reallocated_Sector_Ct'"
                       "| awk -F ' ' '{print $4}'";
    string thresh_cmd = "smartctl -A " + disk +
                        "| grep -E '  5 Reallocated_Sector_Ct'"
                        "| awk -F ' ' '{print $6}'";
    string type_cmd = "smartctl -A " + disk +
                      "| grep -E '  5 Reallocated_Sector_Ct'"
                      "| awk -F ' ' '{print $7}'";
    
    return ATADiskParamSenseRule(value_cmd, thresh_cmd, type_cmd, status);
}

/******************************************************************************/
/* brief: 使用smartctl -H获取SCSI磁盘的健康状态
 * in   : disk - disk, 如/dev/sda
 * out  : status - 硬盘状态, (0)init, (1)ok, (2)fatal
 *        判断准则:
 *        ok: get outprint "SMART Health Status: OK"
 *     error: except ok
SMART Health Status: HARDWARE IMPENDING FAILURE DATA ERROR RATE TOO HIGH [asc=5d, ascq=12]
 * return: SUCCESS/ERROR
 * note :
 */
int SenseSCSIDisk(const string& disk, int& status)
{
    if (disk.empty())
    {
        SkyAssert(false);
        return ERROR;
    }

    status = 0;

    string value;
    string cmd = "smartctl -H " + disk + "|grep -E 'SMART Health Status:'";
    int ret = RunCmd(cmd, value);
    if ((SUCCESS != ret) || value.empty())
    {
        dm_debug("RunCmd failed or not SCSI disk! ret = %d\n", ret);
        return ERROR;
    }

    string::size_type pos = value.find("OK", 0);
    if (pos != value.npos)
    {// Disk is OK
        status = PD_STATUS_NORMAL;
    }
    else
    {
        /* 摘出asc/ascq
         */
        string asc;
        string cmd = "smartctl -H " + disk +
              "| grep -E 'SMART Health Status:'"
              "| awk -F '=' '{print $2}'"
              "| awk -F ',' '{print $1}'";
        ret = RunCmd(cmd, asc);
        if ((SUCCESS != ret) || asc.empty())
        {
            dm_debug("RunCmd failed or Get asc failed! ret = %d\n", ret);
            return ERROR;
        }
        int pos = asc.find('\n', 0);
        asc = asc.erase(pos, asc.size());
        if (!asc.compare("5d"))
        {// 5d为故障状态, 如需细化可继续查看ascq, 待后续需求再考虑
            status = PD_STATUS_FAULT;
        }
        else if (!asc.compare("b"))
        {// b 为警告状态, 如需细化可继续查看ascq, 待后续需求再考虑
            status = PD_STATUS_WARN;
        }
        else
        {// 此不可能会发生, 除非smartctl有了新变化
            status = PD_STATUS_NORMAL; // 暂为正常
        }
    }
    return SUCCESS;
}

#if 0
/* 打印控制 */
extern int smartctl_main(int argc, const char **argv, T_DmPdSmartInfo *SmartValue);

/*****************************************************************************
* 函数名称： dmGetHddSmart
* 功能描述： 获取单个磁盘的SMART信息
* 输入参数： pPdSmartInfo
* 输出参数： pPdSmartInfo
* 返 回 值： 0-成功 其他-失败
* 其它说明： 命令格式为： smartctl -H /dev/sda
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/07/01         V1.0        亢振华       错误码
* 2011/08/10         V1.1        guojsh       修改(tecs)
*****************************************************************************/
int dmGetHddSmart(T_DmPdSmartInfo *pPdSmartInfo)
{  
    int           sdwRet;
    unsigned int  dwCmdParamCnt  = 2;  /* 传递给smartctl的参数个数 */
    const char         *pcCmd[5] = {"smartctl", "-H", NULL, NULL, NULL}; /* 传递给smartctl的命令 */
	
    if(NULL == pPdSmartInfo || NULL == pPdSmartInfo->aucPdName)
    {
        dm_debug("get %s pPdSmartInfo or pPdSmartInfo->aucPdName is NULL!\n");
        return _SMART_ERROR;
    }
	
    pcCmd[dwCmdParamCnt] = pPdSmartInfo->aucPdName;
    dwCmdParamCnt++;
	
    /* 读取磁盘SMART信息 */    
    sdwRet = smartctl_main(dwCmdParamCnt, pcCmd, pPdSmartInfo);
    if (sdwRet)
    {
        dm_debug("get %s smart value fail(ret=0x%x)\n", \
			pPdSmartInfo->aucPdName, sdwRet);
        return _SMART_E_GET_STATUS;
    }
    
    dm_debug("DM SMART: support(%d), enable(%d), OK(%d)\n", pPdSmartInfo->ucIsSupportSmart,
		pPdSmartInfo->ucIsSmartEnable,pPdSmartInfo->ucSmartStatus);
	
    /* 如果磁盘不支持SMART，那么磁盘健康状态为GOOD */
    if (pPdSmartInfo->ucIsSupportSmart != PD_SUPPORT_SMART)
    {
        pPdSmartInfo->ucSmartStatus = PD_GOOD;
    }
    
    return _SMART_SUCCESS;
} 

/*****************************************************************************
* 函数名称： dmExecHddSmartSelfTest
* 功能描述： 执行单个磁盘的SMART自检
* 输入参数： dwSelfTestType
* 输入参数： pPdSmartInfo
* 输出参数： pPdSmartInfo
* 返 回 值： 0-成功 其他-失败
* 其它说明： 命令格式为： smartctl -t short /dev/sda
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/07/01         V1.0        亢振华       错误码
* 2011/08/10         V1.1        guojsh       修改(tecs)
*****************************************************************************/
int dmExecHddSmartSelfTest(unsigned int dwSelfTestType, T_DmPdSmartInfo *pPdSmartInfo)
{
	
    int sdwRet;
    unsigned int dwCmdParamCnt= 2;    /* 传递给smartctl的参数个数 */
    char aucSmartTestType[6];
    const char *pcCmd[5]  ={"smartctl", "-t", NULL, NULL, NULL}; /* 传递给smartctl的命令 */
	
    if(NULL == pPdSmartInfo || NULL == pPdSmartInfo->aucPdName)
    {
        dm_debug("get %s pPdSmartInfo or pPdSmartInfo->aucPdName is NULL!\n");
        return _SMART_ERROR;
    }
	
    /* 判断磁盘自检的类型 */
    memset(aucSmartTestType, 0, sizeof(aucSmartTestType));
    
    if (dwSelfTestType == PD_SHORT_SELFTEST)
    {
        sprintf(aucSmartTestType, "short");
    }
    else if (dwSelfTestType == PD_LONG_SELFTEST)
    {
        sprintf(aucSmartTestType, "long");
    }
    else
    {
        dm_debug("input wrong selftest cmd for pd(%d)\n", dwSelfTestType);
        return _SMART_ERROR;
    }
    
    pcCmd[dwCmdParamCnt] = aucSmartTestType;
    dwCmdParamCnt++;
    
    pcCmd[dwCmdParamCnt] = pPdSmartInfo->aucPdName;
    dwCmdParamCnt++;
	
    /* 执行磁盘SMART自检 */    
    sdwRet = smartctl_main(dwCmdParamCnt, pcCmd, pPdSmartInfo);
    if (sdwRet)
    {
        dm_debug("Exec  smart test fail(ret=0x%x)\n", sdwRet);
        return _SMART_E_SELFTEST;
    } 
    
    return _SMART_SUCCESS;
}
#endif


