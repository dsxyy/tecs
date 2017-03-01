/*
 * smart_dbg.cpp
 *
 */
#include "smart_dbg.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "sky.h"

#define SMART_INFO_LEN 128
#define SMART_INFO_NUM 8

/*��־��Ϣ��¼����ÿ����Ϣ���128���ַ�(��'\0'), ����¼8������ռ�ڴ�1k */
static char  sg_dbg_msg[SMART_INFO_NUM][SMART_INFO_LEN];
static unsigned int sg_dbg_log_cnt = 0;

/*��ӡ����*/
bool  g_smart_dbg_prn = false;

/*��־��ӡ
*/
void Smart_Vlog(const char *fmt, va_list list)
{
    unsigned int cnt;
    char tmp[256];
	
    cnt = sg_dbg_log_cnt%SMART_INFO_NUM;
    sg_dbg_log_cnt++;
    vsnprintf(tmp, sizeof(tmp), fmt, list);
    strncpy(sg_dbg_msg[cnt], tmp, SMART_INFO_LEN);
    sg_dbg_msg[cnt][SMART_INFO_LEN - 1]='\0';

    if(g_smart_dbg_prn)
    {
        printf("%s\n", tmp);
    }
    
    return;
}

/*��־��ӡ
*/
void Smart_Log(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    Smart_Vlog(fmt, ap);
    va_end(ap);
    return;
}

/*����Ϣ���(printf)����
*/
void  Smart_DbgEnablePrn()
{
    g_smart_dbg_prn = true;
}
DEFINE_DEBUG_FUNC(Smart_DbgEnablePrn);	

/*�ر���Ϣ���(printf)����
*/
void  Smart_DbgDisablePrn()
{
    g_smart_dbg_prn = false;
}
DEFINE_DEBUG_FUNC(Smart_DbgDisablePrn);	


/*��ʾ��־��Ϣ
*/
void  Smart_showDbgMsg()
{
    int i;
  
    printf("g_smart_dbg_prn = %s\n", g_smart_dbg_prn?"true":"false");
    printf("sg_dbg_log_cnt = %u\n", sg_dbg_log_cnt);
    printf("sg_dbg_log_cnt%%SMART_INFO_NUM = %u\n", sg_dbg_log_cnt%SMART_INFO_NUM);

    for(i = 0; i < SMART_INFO_NUM; i++)
    {
        printf("%s\n",sg_dbg_msg[i]);
    }
}
DEFINE_DEBUG_FUNC(Smart_showDbgMsg);	

