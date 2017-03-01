/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：log_agent.h
* 文件标识：见配置管理计划书
* 内容摘要：日志文件的函数接口定义
* 当前版本：1.0
* 作    者：张业兵
* 完成日期：2012年8月23日
*
* 修改记录1：
*     修改日期：2012年8月23日
*     版 本 号：V1.0
*     修 改 人：张业兵
*     修改内容：创建
******************************************************************************/

#ifndef LOG_AGENT_H
#define LOG_AGENT_H
namespace zte_tecs
{

//输出方向
#define DIR_CONSOLE 1  //控制台方向
#define DIR_EXC_LOG 2  //异常日志方向
#define DIR_SYS_LOG 3  //系统日志方向

//输出信息的重要级别
#define SYS_EMERGENCY           0   //Emergency: system is unusable
#define SYS_ALERT               1   //Alert: action must be taken immediately
#define SYS_CRITICAL            2   //Critical: critical conditions
#define SYS_ERROR               3   //Error: error conditions
#define SYS_WARNING             4   //Warning: warning conditions
#define SYS_NOTICE              5   //Notice: normal but significant condition
#define SYS_INFORMATIONAL       6   //Informational: informational messages
#define SYS_DEBUG               7   //Debug: debug-level messages

#define SYS_OUTPUT_CLOSE        -1  //close output

STATUS LogInit(int iHc = 0);
STATUS OutPut(int iLevel, const char* pcFmt,...);
STATUS OutPut(int level, const string& log);
STATUS OutPut(int level, ostringstream& log);
STATUS SetLevel(int iOutPutDir,int iLevel);
STATUS GetLevel(int iOutPutDir);
STATUS EnableLogOptions();
STATUS LogLastwords();
}
#endif

