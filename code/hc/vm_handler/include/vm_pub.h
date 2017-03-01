/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_pub.h
* 文件标识：
* 内容摘要：虚拟机相关公用头文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年8月11日
*
* 修改记录1：
*     修改日期：2011/8/11
*     版 本 号：V1.0
*     修 改 人：mhb
*     修改内容：创建
*******************************************************************************/

#ifndef HC_VMPUB_H
#define HC_VMPUB_H



#include "sky.h"
#include "mid.h"
#include "event.h"

#include "log_agent.h"
#include "exc_log_agent.h"

//using namespace std;
using namespace zte_tecs;




#define MAX_DOM_NUM             64                     /* HC最多运行64个domain */


/* 看门狗消息类型 */
#define    ENABLE_WATCHDOG    0
#define    DISABLE_WATCHDOG   1
#define    FEED_WATCHDOG      2



/* 调试打印开关 */
extern bool vmx_print_flag;

#define Debug(fmt,arg...) \
        do \
        { \
            if(vmx_print_flag) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

/* 是否运行在64位系统上 */
extern bool Is64BitSystem();

/* 执行系统命令 */
extern int ExecCmd(const char* cmd);

#endif


