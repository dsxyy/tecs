/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：servers_building.cpp
* 文件标识：见配置管理计划书
* 内容摘要：TECS内NTP的C/S模型架设模块
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

