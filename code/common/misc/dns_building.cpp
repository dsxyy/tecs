/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�servers_building.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��TECS��DNS����������ģ��
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
#include "dns_building.h"

namespace zte_tecs
{

const string DNS_SHELL_PATH = "/usr/local/bin/";

STATUS SetDnsServerConfig(const string& reg,
                          const string& ip,
                          const string& domain_name)
{
    if (reg.empty() || ip.empty() || domain_name.empty())
        return ERROR;

    ostringstream cmd;
    cmd << DNS_SHELL_PATH << "config_dns_server.sh " << reg << " "
        << ip << " " << domain_name << " 2>/dev/null";
    string result;
    if (SUCCESS != RunCmd(cmd.str(), result))
        return ERROR;
    else
        return SUCCESS;
}

STATUS SetDnsClientConfig(const string& reg,
                          string& ip,
                          const string& character)
{
    if (reg.empty() || ip.empty() || character.empty())
        return ERROR;

    if (true == IsLocalIp(ip))
        ip = "127.0.0.1";

    ostringstream cmd;
    cmd << DNS_SHELL_PATH << "config_dns_client.sh " << reg << " "
        << ip << " " << character << " 2>/dev/null";
    string result;
    if (SUCCESS != RunCmd(cmd.str(), result))
        return ERROR;
    else
        return SUCCESS;    
}

STATUS DnsServiceRestart()
{
    string cmd = "service dnsmasq restart 2>/dev/null";
    string result;
    if (SUCCESS != RunCmd(cmd, result))
        return ERROR;
    else
        return SUCCESS;
}

}

