/******************************************************************************************
* ��Ȩ���� (C)2011, ����ͨѶ�ɷ����޹�˾��
*
*   All rights reserved
*
* �ļ����ƣ�smart.cpp
* �ļ���ʶ��
* ����ժҪ��S.M.A.R.T����ӿ�ʵ���ļ�
* ����˵����
* ��ǰ�汾�� 1.0
* ��    �ߣ� gjsh
* ������ڣ� 2011-08-20
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
* �޸����ڣ�2013-07-25
* �� �� �ţ�2.0
* �� �� �ˣ�л����
* �޸����ݣ���Ϊʹ��smartctl����ű��ķ�ʽ
* �޸ļ�¼2����
* �� �� �ˣ�
* �޸����ڣ�
* �޸����ݣ�
****************************************************************************************/

#include <iostream>
#include <vector>
#include <list>
#include "lvm.h"
#include "smart.h"
#include "log_agent.h"

using namespace std;

#if 0 //����smartctl�������Щ�����ݲ�����
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
        //��¼��������������
        
    }
#endif
    return _SMART_SUCCESS;
}

/******************************************************************************/
/* ��ȡ����SMART��Ϣ */
int DiskSmart::getSmartInfo(vector<T_DmPdSmartInfo> &diskSmartInfo)
{
    T_DmPdSmartInfo         smart_info;
    vector<string> vec_disk;
    int    status = -1;
    int    ret = -1;
    /* ʹ��·����/dev/sda��ʶһ��disk
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
        // ���disk�Ľӿ�����, ��ͬ�ӿ�(��: ATA/SCSI)��disk�߲�ͬ��·��
        ret = GetDiskInterface(*it, smart_info);
        if (SUCCESS != ret)
        {
            continue;
        }
        if (DISK_TYPE_ATA == smart_info.ucDiskType)
        {
            // ATA disk���
            ret = SenseATADisk(*it, status);
            if (ERROR == ret)
                continue;
        }
        else if (DISK_TYPE_SCSI == smart_info.ucDiskType)
        {
            /* SCSI diskʹ��smartctl -Hֱ�ӽ��ô������
             * ������⹦���ж��Ƿ��ϱ��澯
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

