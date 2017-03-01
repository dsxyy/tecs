/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_common.cpp
* 文件标识：见配置管理计划书
* 内容摘要：VNA公共功能函数实现文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2012年12月15日
*
* 修改记录1：
*     修改日期：2012/12/15
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#include "vnet_comm.h"
#include "vna_common.h"
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
int GetVNAUUIDByVNA(string &strVNAUUID)
{
    strVNAUUID.clear();
    string strVnaApp;
    
    ostringstream file;
    file << VNA_UUID_FILE ;
    ifstream fin(file.str().c_str());
    fin >> strVnaApp; 

    if ("" == strVnaApp)
    {
        if (0 != access(VNA_UUID_FILE, 0))
        {
            ostringstream oss;
            oss<< "mkdir -p " << VNA_UUID_DIR;
            if (0 != RunCmd(oss.str()))
            {
                VNET_LOG(VNET_LOG_ERROR, "GetVNAUUIDByVNA: call system(%s) failed\n",oss.str().c_str());
                return -1;
            }
        }

        string tmpVNAUUID;
        if (GetUUID(tmpVNAUUID))
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
        
        strVnaApp = hostname;
        strVnaApp += "_";
        strVnaApp += tmpVNAUUID;

        ostringstream file;
        file << VNA_UUID_FILE;
        ofstream fout(file.str().c_str(), ios::out);
        fout << strVnaApp;
        
        strVNAUUID = strVnaApp;
    }
    else
    {
        strVNAUUID = strVnaApp;
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
int GetVNAUUIDByLib(string &strVNAUUID)
{
    strVNAUUID.clear();
    string tmpVNAUUID;

    if (0 != access(VNA_UUID_FILE, 0))
    {
        return -1;
    }
            
    ostringstream file;
    file << VNA_UUID_FILE ;
    ifstream fin(file.str().c_str());
    fin >> tmpVNAUUID; 

    strVNAUUID = tmpVNAUUID;
    
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

/************************************************************
* 函数名称： GetHypeVisorType
* 功能描述： 获取当前虚拟化机制类型
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
int GetHypeVisorType()
{
    string        sTypePath  =  "/sys/hypervisor/type";
    string        sHyType;
    const int     iTypeLen = 12;
    char          cType[iTypeLen];

    ifstream fin(sTypePath.c_str()); 
    if (!fin.is_open())    /* in kvm or other vmm, the file is not exist */
    {
        /* is kvm ? */
        string  scdev_kvm = "/dev/kvm";
        ifstream fin_kvm(scdev_kvm.c_str()); 
        if (fin_kvm.is_open())
        {
            return HYPE_VISOR_TYPE_KVM;
        }
        else
        {
            return MAX_DRIVER_TYPE;
        }
    } 

    memset(cType, 0, sizeof(cType));
    fin.getline(cType,iTypeLen);

    sHyType = cType;
    if (string::npos != sHyType.find("xen", 0))
    {
        return HYPE_VISOR_TYPE_XEN;
    }

    if (string::npos != sHyType.find("kvm", 0))
    {
        return HYPE_VISOR_TYPE_KVM;
    }

    if (string::npos != sHyType.find("vmvare", 0))
    {
        return HYPE_VISOR_TYPE_VMWARE;
    }
    
    return MAX_DRIVER_TYPE;
}


