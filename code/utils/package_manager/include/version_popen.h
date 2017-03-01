/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_popen.h
* 文件标识：
* 内容摘要：版本管理popen相关的实现头文件
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年9月26日
*
* 修改记录1：
*     修改日期：2012年9月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/

#ifndef TC_VERSION_POPEN_H
#define TC_VERSION_POPEN_H
#include "tecs_errcode.h"
#include "tecs_config.h"
#include "log_agent.h"

namespace zte_tecs
{
class VersionPopen
{
public:
    VersionPopen()
    {
        _data_pos=0;
        _retcode=ERROR_NOT_READY;
        _is_cancel=false;
        _pid = 0;
        _time_out = 30*60;//半消息超时自动退出
    };
    VersionPopen(int timeout)
    {
        _data_pos=0;
        _retcode=ERROR_NOT_READY;
        _is_cancel=false;
        _pid = 0;
        _time_out = timeout;//半消息超时自动退出
    };
    ~VersionPopen(){};
    void SetCancel()
    {
        _is_cancel=true;
    };
    void SetTimeOut(int timeout)
    {
        _time_out=timeout;
    };
    virtual STATUS ProcessData()
    { 
        OutPut(SYS_DEBUG,"popen need't process data\n");
        return SUCCESS;
    }
    bool GetSubStr(const string &name,const string &s_str,const string &e_str,string & value);
    bool GetSubStr(const string &name,const string &s_str,const string &e_str,int & value);
    STATUS ExecutePopen(const string &cmd);
    STATUS  _retcode;
    string::size_type   _data_pos;
    string  _detail; 
private:
    
    bool IsCancel()
    {
        return (_is_cancel == true);
    }
    bool IsTimeOut()
    {
        if (_time_out<=0)
        {
            OutPut(SYS_EMERGENCY,"popen %u pid time out\n",_pid);
            return true;
        }
        _time_out--;
        
        return false;
    }
    STATUS KillPopen()
    {
        int ret;
        ret = kill(_pid,9);
        /*stringstream s;
        s << " kill -9 " << _pid;
        ret = RunCmd(s.str());*/
        if (0 != ret)
        {
            OutPut(SYS_EMERGENCY,"popen kill %u pid fail\n",_pid);
        }
        return ret;
    }
    bool    _is_cancel;
    pid_t   _pid;
    int     _time_out;
    DISALLOW_COPY_AND_ASSIGN(VersionPopen);
};
} // namespace zte_tecs
#endif //end TC_VERSION_POPEN_H

