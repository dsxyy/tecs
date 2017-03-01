/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_pub.h
* �ļ���ʶ��
* ����ժҪ���������ع���ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��8��11��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/11
*     �� �� �ţ�V1.0
*     �� �� �ˣ�mhb
*     �޸����ݣ�����
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




#define MAX_DOM_NUM             64                     /* HC�������64��domain */


/* ���Ź���Ϣ���� */
#define    ENABLE_WATCHDOG    0
#define    DISABLE_WATCHDOG   1
#define    FEED_WATCHDOG      2



/* ���Դ�ӡ���� */
extern bool vmx_print_flag;

#define Debug(fmt,arg...) \
        do \
        { \
            if(vmx_print_flag) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

/* �Ƿ�������64λϵͳ�� */
extern bool Is64BitSystem();

/* ִ��ϵͳ���� */
extern int ExecCmd(const char* cmd);

#endif


