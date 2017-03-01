/******************************************************************************************
* ��Ȩ���� (C)2011, ����ͨѶ�ɷ����޹�˾��
*
*   All rights reserved
*
* �ļ����ƣ�smart_api.cpp
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
* �� �� �ˣ�xiett
* �޸����ݣ���Ϊʹ��smartctl����ű��ķ�ʽ
* �޸ļ�¼2����
* �� �� �ˣ�
* �޸����ڣ�
* �޸����ݣ�
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
/* brief: ʹ�����������ȡ����·��
 *        ls -l /sys/dev/block | grep pci | grep -E '[^0-9]$ | grep -v usb'
 * in   :
 * out  : disk_path - ��¼����·����/dev/sda
 * return: SUCCESS/ERROR
 * note : 
 *   1) disk_path �� disk_name ���±���һһ��Ӧ��
 *   2) ��ȡ���Ǳ�����ʵ���������, ���˵��߼���, ������, USB��(����)
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
        {// ���ҵ��豸��, ���Ӧ/dev/name
            disk = "/dev/" + line.substr(pos+1, line.size());
            vec_disk.push_back(disk);
        }
    }
    return SUCCESS;
}

/******************************************************************************/
/* brief: ʹ��smartctl -i����ATA disk��SCSI disk
 *     ATA: grep '=== START OF INFORMATION SECTION ==='
 *    SCSI: grep 'Transport protocol:'
 *       ps: ����ΪsmartctlԴ��
 * in  : disk - disk, ��/dev/sda
 * out : smart_info��ucIsSupportSmart, ucIsSmartEnable, ucDiskType
 * return: SUCCESS/ERROR
 * note:
 */
int GetDiskInterface(const string& disk, T_DmPdSmartInfo& smart_info)
{
    // ������
    if (disk.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
    // ����ű�smartctl -i disk
    // ����Ƿ�֧��SMART
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
    // ���SMART�Ƿ�ʹ��
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
    // �ȼ���Ƿ��� ATA Ӳ��
    cmd = "smartctl -i " + disk + "|grep -E '=== START OF INFORMATION SECTION ==='";
    string str_ata;
    ret = RunCmd(cmd, str_ata);
    if (!str_ata.empty())
    {
        smart_info.ucDiskType = DISK_TYPE_ATA;
    }
    else
    {
        // �ټ���Ƿ��� SCSI Ӳ��
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
/* brief: ATA disk �ؼ������ж�׼��
 * in   : value_cmd - ��ȡ��¼��VALUEֵ
 *        thresh_cmd - ��ȡ��¼��THRESHֵ
 *        type_cmd - ��ȡ��¼��TYPEֵ
 * out  : status - ״̬
 * return: SUCCESS/ERROR
 * note : �ű����� grep �� awk ��ժ��VALUE/THRESH/TYPE
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
    status = PD_STATUS_NORMAL; // ��ʼֵΪnormal
    // ��ȡvalueֵ, 0-255
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

    // ��ȡthresholdֵ, 0-255
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
        // ��ȡtype, ��Ӧһ��bit, 1: pre-fail, 0: Old_age
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
/* brief: ʹ��smartctl -A��ȡATAӲ�̲���
 * in   : disk - disk, ��/dev/sda
 * out  : status - Ӳ��״̬,
 *                   0   init value
 *                   1   DISK_STATUS_NORMAL
 *                   2   DISK_STATUS_WARN
 *                   3   DISK_STATUS_FAULT
 * return: SUCCESS/ERROR
 * note : ��ʹ��һ����������, �粻��ʱ, �ɿ���ʹ������
 *      5 Reallocated_Sector_Ct
 */
int SenseATADisk(const string& disk, int& status)
{
    if (disk.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
/* smartctl -A ��ӡ��¼��ʽ����:
ID# ATTRIBUTE_NAME          FLAG     VALUE WORST THRESH TYPE      UPDATED  WHEN_FAILED RAW_VALUE
  5 Reallocated_Sector_Ct   0x0033   200   200   140    Pre-fail  Always       -       0
199 UDMA_CRC_Error_Count    0x0032   200   200   000    Old_age   Always       -       0
 */
    status = 0; // 0Ϊ��ʼֵ
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
/* brief: ʹ��smartctl -H��ȡSCSI���̵Ľ���״̬
 * in   : disk - disk, ��/dev/sda
 * out  : status - Ӳ��״̬, (0)init, (1)ok, (2)fatal
 *        �ж�׼��:
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
        /* ժ��asc/ascq
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
        {// 5dΪ����״̬, ����ϸ���ɼ����鿴ascq, �����������ٿ���
            status = PD_STATUS_FAULT;
        }
        else if (!asc.compare("b"))
        {// b Ϊ����״̬, ����ϸ���ɼ����鿴ascq, �����������ٿ���
            status = PD_STATUS_WARN;
        }
        else
        {// �˲����ܻᷢ��, ����smartctl�����±仯
            status = PD_STATUS_NORMAL; // ��Ϊ����
        }
    }
    return SUCCESS;
}

#if 0
/* ��ӡ���� */
extern int smartctl_main(int argc, const char **argv, T_DmPdSmartInfo *SmartValue);

/*****************************************************************************
* �������ƣ� dmGetHddSmart
* ���������� ��ȡ�������̵�SMART��Ϣ
* ��������� pPdSmartInfo
* ��������� pPdSmartInfo
* �� �� ֵ�� 0-�ɹ� ����-ʧ��
* ����˵���� �����ʽΪ�� smartctl -H /dev/sda
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/07/01         V1.0        ����       ������
* 2011/08/10         V1.1        guojsh       �޸�(tecs)
*****************************************************************************/
int dmGetHddSmart(T_DmPdSmartInfo *pPdSmartInfo)
{  
    int           sdwRet;
    unsigned int  dwCmdParamCnt  = 2;  /* ���ݸ�smartctl�Ĳ������� */
    const char         *pcCmd[5] = {"smartctl", "-H", NULL, NULL, NULL}; /* ���ݸ�smartctl������ */
	
    if(NULL == pPdSmartInfo || NULL == pPdSmartInfo->aucPdName)
    {
        dm_debug("get %s pPdSmartInfo or pPdSmartInfo->aucPdName is NULL!\n");
        return _SMART_ERROR;
    }
	
    pcCmd[dwCmdParamCnt] = pPdSmartInfo->aucPdName;
    dwCmdParamCnt++;
	
    /* ��ȡ����SMART��Ϣ */    
    sdwRet = smartctl_main(dwCmdParamCnt, pcCmd, pPdSmartInfo);
    if (sdwRet)
    {
        dm_debug("get %s smart value fail(ret=0x%x)\n", \
			pPdSmartInfo->aucPdName, sdwRet);
        return _SMART_E_GET_STATUS;
    }
    
    dm_debug("DM SMART: support(%d), enable(%d), OK(%d)\n", pPdSmartInfo->ucIsSupportSmart,
		pPdSmartInfo->ucIsSmartEnable,pPdSmartInfo->ucSmartStatus);
	
    /* ������̲�֧��SMART����ô���̽���״̬ΪGOOD */
    if (pPdSmartInfo->ucIsSupportSmart != PD_SUPPORT_SMART)
    {
        pPdSmartInfo->ucSmartStatus = PD_GOOD;
    }
    
    return _SMART_SUCCESS;
} 

/*****************************************************************************
* �������ƣ� dmExecHddSmartSelfTest
* ���������� ִ�е������̵�SMART�Լ�
* ��������� dwSelfTestType
* ��������� pPdSmartInfo
* ��������� pPdSmartInfo
* �� �� ֵ�� 0-�ɹ� ����-ʧ��
* ����˵���� �����ʽΪ�� smartctl -t short /dev/sda
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2011/07/01         V1.0        ����       ������
* 2011/08/10         V1.1        guojsh       �޸�(tecs)
*****************************************************************************/
int dmExecHddSmartSelfTest(unsigned int dwSelfTestType, T_DmPdSmartInfo *pPdSmartInfo)
{
	
    int sdwRet;
    unsigned int dwCmdParamCnt= 2;    /* ���ݸ�smartctl�Ĳ������� */
    char aucSmartTestType[6];
    const char *pcCmd[5]  ={"smartctl", "-t", NULL, NULL, NULL}; /* ���ݸ�smartctl������ */
	
    if(NULL == pPdSmartInfo || NULL == pPdSmartInfo->aucPdName)
    {
        dm_debug("get %s pPdSmartInfo or pPdSmartInfo->aucPdName is NULL!\n");
        return _SMART_ERROR;
    }
	
    /* �жϴ����Լ������ */
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
	
    /* ִ�д���SMART�Լ� */    
    sdwRet = smartctl_main(dwCmdParamCnt, pcCmd, pPdSmartInfo);
    if (sdwRet)
    {
        dm_debug("Exec  smart test fail(ret=0x%x)\n", sdwRet);
        return _SMART_E_SELFTEST;
    } 
    
    return _SMART_SUCCESS;
}
#endif


