/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：servers_building.cpp
* 文件标识：见配置管理计划书
* 内容摘要：TECS内DNS服务器架设模块
* 当前版本：1.0
* 作    者：谢涛涛
* 完成日期：2014年01月02日
*
* 修改记录1：
*     修改日期：2014/01/02
*     版 本 号：V1.0
*     修 改 人：谢涛涛
*     修改内容：创建
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

