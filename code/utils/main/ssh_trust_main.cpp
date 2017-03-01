/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ssh_trust_main.cpp
* 文件标识：
* 内容摘要：ssh 认证进程的main函数
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年7月26日
*
* 修改记录1：
*     修改日期：2012年7月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#include "tecs_config.h"
#include "log_agent.h"
#include "ssh_trust.h"

using namespace zte_tecs;
//namespace zte_tecs
//{
#define VALID_CHAR(c)   \
    ((((c) >= '0') && ((c) <= '9')) || (((c) >= 'a') && ((c) <= 'z')) || \
     (((c) >= 'A') && ((c) <= 'Z')) || ((c) == '_') || ((c) == '-'))

static const char *filename_without_path(const char *path)
{
    if (path == NULL)
    {
        return NULL;
    }
    
    /* 找到斜杠出现的最后位置，如果找不到则返回NULL */
    const char *slash = strrchr(path, '/');

    if (!slash || (slash == path && !slash[1]))
    {
        return (char*)path;
    }
    return slash + 1;
}

static string get_proc_name ()
{
    TecsConfig *config = TecsConfig::Get();

    stringstream nstream;
    int instance = config->GetBackupInst();
    //实例号，在不配置主备时为负数，以0替代
    if (DEFAULT_MS_INST_NOT_CFG == config->GetBackupInst())
    {
        nstream << filename_without_path(config->get_exe_name().c_str());
    }
    else
    {
        nstream << filename_without_path(config->get_exe_name().c_str())<< "_" << instance;
    }    

    string name;
    name = nstream.str();
    int   size   =   name.size();
    //去除"."符号，，以"_"替代，否则进程名校验合法性失败    
    for(int pos=0; pos < size; ++pos) 
    {
        if (VALID_CHAR(name.at(pos)))
        {
            continue;
        }
        name.replace(pos,1,"_");
    }
    
    return name;
}

int main(int argc, char** argv)
{    
    STATUS ret;

    TecsConfig *config = TecsConfig::Get();
    config->EnableSky();
    // 使能日志打印级别配置参数
    EnableLogOptions();

    ret = config->Parse(argc, argv);
    //解析命令行参数以及配置文件
    if(SUCCESS == ret)
    {
        if (config->IsHelp())
        {
            return 0;
        }        
    }	
	
    //初始化配置
    if(ERROR == ret)
    {
        if (!config->IsHelp())
        {
            SkyAssert(0);		  		
            exit(-1);
        }
    }
    else if(ERROR_OPTION_NOT_EXIST == ret)
    {		
        exit(-1);
    }

    //设置进程名称，根据文件名与实例号
    config->SetProcess(get_proc_name().c_str());
    ret = tecs_init();
    if(ret != SUCCESS)
    {
        printf("tecs init failed! ret = %d\n",ret);
        SkyExit(-1,"main(SShTrust): call tecs_init failed.");
    }

    ret = LogInit();
    if(ret != SUCCESS)
    {
        SkyAssert(0);
        SkyExit(-1, "SShTrust call LogInit failed.");
    }
    AddKeyMu(MU_SSH_TRUST);

    // 启动调试shell
    Shell("tecs_sshTrust-> ");

    SShTrust *pst = SShTrust::GetInstance();
    if(!pst)
    {
        SkyAssert(0);
        SkyExit(-1,"SShTrust GetInstance failed!\n");
    }
    ret = pst->Init();
    if(SUCCESS != ret)
    {
        SkyAssert(0);
        SkyExit(-1,"SShTrust Init failed!\n");
    }
    //等待SIGINT、SIGTERM信号到来后退出
    wait_signal_exit(); 
    return 0;
}
//}
