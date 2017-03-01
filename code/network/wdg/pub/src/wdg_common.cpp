/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�wdg_common.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��WDG�������ܺ���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2012��12��15��
*******************************************************************************/
#include "vnet_comm.h"
#include "wdg_common.h"
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
        
        //hostname���ȡ20���ֽڣ�app�ĳ�����64������
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


