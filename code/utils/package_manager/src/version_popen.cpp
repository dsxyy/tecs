/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_popen.cpp
* 文件标识：
* 内容摘要：版本管理popen相关的实现
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

#include "version_popen.h"

namespace zte_tecs
{
static int version_popen_print_on = 0;
DEFINE_DEBUG_VAR(version_popen_print_on);
#define Debug(fmt,arg...) \
                    do \
                    { \
                        if(version_popen_print_on) \
                        { \
                                OutPut(SYS_DEBUG,fmt,##arg); \
                        } \
                    }while(0)
bool VersionPopen::GetSubStr(const string &name,const string &s_str,const string &e_str,string & value)
{
    //通过一些标志位获取从脚本返回的一些数据信息
    string::size_type pos;
    pos = _detail.find(name,_data_pos);
    if( pos == string::npos )
    {
        return false;
    }
    if( pos == string::npos )
    {
        return false;
    }

    pos = _detail.find( s_str,pos);
    if( pos == string::npos )
    {
        return false;
    }
    string::size_type shift_pos;
    shift_pos = _detail.find( e_str,pos);
    if( shift_pos == string::npos )
    {
        return false;
    }
    if (shift_pos < (pos+2))
    {
        return false;
    }

    value = _detail.substr(pos+1,shift_pos-pos-1);
    //OutPut(SYS_DEBUG,"%s pos %d %d\n",value.c_str(),pos,shift_pos);
    _data_pos = shift_pos;

    return true;
}
bool VersionPopen::GetSubStr(const string &name,const string &s_str,const string &e_str,int & value)
{
    string vstr;
    if (GetSubStr(name,s_str,e_str,vstr))
    {
        from_string<int>(value,vstr,dec);
        return true;
    }
    return false;
}

STATUS VersionPopen:: ExecutePopen(const string &cmd)
{
    FILE* fd;
    fd = Popen(cmd.c_str(), "r",_pid);
    if (!fd)
    {
        OutPut(SYS_EMERGENCY,"Version popen fail!\n");
        _retcode=ERROR_POPEN_FAIL;
        return _retcode;
    }
    char            r[1024];
    int             result=0;
    int             cout=0;
    fd_set          rfs;
    struct timeval  timeout;
    while (1)
    {
        if (IsCancel())
        {
            _retcode=ERROR_PROCESS_WILL_EXIT;
            KillPopen();
            break;
        }
        if (IsTimeOut())
        {
            _retcode=ERROR_TIME_OUT;
            KillPopen();
            break;
        }        
        FD_ZERO(&rfs);
        FD_SET(fileno (fd),&rfs);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        result = select(fileno (fd) + 1, &rfs, NULL, NULL , &timeout); 
        if (-1 == result) 
        {
            if (EAGAIN == errno)
            {
                continue;
            }
            else 
            {
                OutPut(SYS_EMERGENCY,"Version popen select error %d\n",errno);
                _retcode=ERROR_POPEN_SELECT_FAIL;
                KillPopen();
                break;
            }
        } 
        else if (0 == result) 
        {
            Debug("%s:timeout\n",cmd.c_str());
            continue;
        }
        
        if (!FD_ISSET(fileno(fd),&rfs)) 
        {
            //OutPut(SYS_DEBUG,"Version popen select others!\n");
            continue;
        }
        memset(r,0,sizeof(r));
        cout=read(fileno(fd),r,sizeof(r));
        if (cout<=0)
        {
            _retcode = SUCCESS;
            break;
        }            
        Debug("%s\n",r);
        _detail += r;
        _retcode = ProcessData();
        if (SUCCESS != _retcode &&
            ERROR_OP_RUNNING != _retcode)
        {
            Debug("popen process error %u\n",_retcode);
            KillPopen();
            break;
        }
    }
    Pclose(fd,_pid);
    return _retcode;
}
} // namespace zte_tecs

