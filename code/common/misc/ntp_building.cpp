/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�servers_building.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��TECS��NTP��C/Sģ�ͼ���ģ��
* ��ǰ�汾��1.0
* ��    �ߣ�л����
* ������ڣ�2014��01��02��
*
* �޸ļ�¼1��
*     �޸����ڣ�2014/01/02
*     �� �� �ţ�V1.0
*     �� �� �ˣ�л����
*     �޸����ݣ�����
*******************************************************************************/
#include "sky.h"
#include "ntp_building.h"

namespace zte_tecs
{

STATUS SetNtpConfig(string ntp_config_shell,
                    string reg,
                    string ip,
                    bool check_local_ip)
{
    if (ntp_config_shell.empty()
        || reg.empty()
        || ip.empty())
        return ERROR;

    if ((true == check_local_ip)
        && (true == IsLocalIp(ip)))
        return SUCCESS;

    ostringstream cmd;
    cmd << "/usr/local/bin/" << ntp_config_shell << " "
        << reg << " " << ip << " 2>/dev/null";
    string result;
    if (SUCCESS != RunCmd(cmd.str(), result))
        return ERROR;
    else
        return SUCCESS;
}

STATUS SetNtpServerConfig(const string& reg,
                          const string& ip,
                          bool check_local_ip)
{
    return SetNtpConfig("config_ntp_server.sh", reg, ip, check_local_ip);
}

STATUS SetNtpClientConfig(const string& reg,
                          const string& ip,
                          bool check_local_ip)
{
    return SetNtpConfig("config_ntp_client.sh", reg, ip, check_local_ip);
}

STATUS NtpServiceRestart()
{
    string cmd = "service ntpd restart 2>/dev/null";
    string result;
    if (SUCCESS != RunCmd(cmd, result))
        return ERROR;
    else
        return SUCCESS;
}

}

