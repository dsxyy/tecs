/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_common.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��VNA�������ܺ���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2012��12��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
*******************************************************************************/
#include "vnet_comm.h"
#include "vna_common.h"
#include <algorithm>

/************************************************************
* �������ƣ� GetVNAUUIDByVNA
* ���������� ��ȡVNA��application����VNA���̵��ã����û���򴴽�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
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

        //hostname���ȡ20���ֽڣ�app�ĳ�����64������
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
* �������ƣ� GetVNAUUIDByLib
* ���������� ��ȡVNA��application����ҵ����̵��ã����û���򷵻ؿ�
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
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
* �������ƣ� GetHostBoisUUID
* ���������� ��ȡ����BOIS��UUID����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
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
* �������ƣ� GetHypeVisorType
* ���������� ��ȡ��ǰ���⻯��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
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


