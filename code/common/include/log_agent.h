/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�log_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ����־�ļ��ĺ����ӿڶ���
* ��ǰ�汾��1.0
* ��    �ߣ���ҵ��
* ������ڣ�2012��8��23��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��8��23��
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ҵ��
*     �޸����ݣ�����
******************************************************************************/

#ifndef LOG_AGENT_H
#define LOG_AGENT_H
namespace zte_tecs
{

//�������
#define DIR_CONSOLE 1  //����̨����
#define DIR_EXC_LOG 2  //�쳣��־����
#define DIR_SYS_LOG 3  //ϵͳ��־����

//�����Ϣ����Ҫ����
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

