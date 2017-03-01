/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：exc_log_agent.cpp
* 文件标识：
* 内容摘要：CC上的异常日志代理模块实现文件
* 当前版本：1.0
* 作    者：邓红波
* 完成日期：2011年9月16日
*
* 修改记录1：
*     修改日期：2011/09/16
*     版 本 号：V1.0
*     修 改 人：邓红波
*     修改内容：创建
*******************************************************************************/
#include "exc_log_agent.h"
#include "log_agent.h"
using namespace zte_tecs;
namespace zte_tecs
{

ExcLogAgent *ExcLogAgent::_instance = NULL;

ExcLogAgent::ExcLogAgent()
{
    _mu = NULL;
};

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS ExcLogAgent::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

ExcLogAgent::~ExcLogAgent()
{
    delete _mu;
};

STATUS ExcLogAgent::Receive(const MessageFrame& message)
{
    return _mu->Receive(message);
};

void ExcLogAgent::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            break;
        }
        break;
    }

    case S_Work:
    {
        //根据收到的消息id进行处理
        switch (message.option.id())
        {
        case EV_EXC_LOG:
        {
            SysLogMessage log_msg;
            struct stat file_state;
            int ret;
            log_msg.deserialize(message.data);
            //判断异常日志目录是否存在，如果不存在则创建
            string file;
            file = _exc_log_path+"/"+log_msg._host_name+".txt";
            ret  = stat(file.c_str(),&file_state);
            if (ret == 0)
            {
                if (file_state.st_size >=  EXC_LOG_FILE_SIZE)
                {
                    string file_bak;
                    file_bak = file+".bak";
                    unlink(file.c_str());
                    rename(file.c_str(),file.c_str());
                }
            }

            //写入文件
            ofstream ofile;
            ofile.open(file.c_str(),ios::app|ios::out); 
            if (!ofile.is_open())
            {
                //失败则退出
                cerr << "Error opening out file!errno =" << errno << endl;
                return ; 
            }
            ofile << "<" << log_msg._log_level_count << ">" \
                  << log_msg._log_time << " " \
                  << log_msg._pro_name << ": " \
                  << log_msg._log_content<< endl;
            ofile.close();
            break;
        }
        default:
            return;
        }
        break;
    }

    default:
        break;
    }

    return;
}

}


