/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�CliFunc.h
* �ļ���ʶ��
* ����ժҪ��������ƥ����غ���
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����� ��ֲ����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2��
**********************************************************************/
#ifndef __CLI_FUNC_H__
#define __CLI_FUNC_H__

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

/************************* the states of parsing the command line *************************/
/* begin state */
#define PARSESTATE_BEGINPARSE       0   /* begin parsing the command line */
/* parse state */
#define PARSESTATE_WORD_PARSING     10  /* parsing the words */
#define PARSESTATE_WORD_PARSED      20  /* finish parsing the words */
#define PARSESTATE_PARA_PARSING     30  /* parsing the parameters */
#define PARSESTATE_PARA_PARSED      40  /* finish parsing the parameters*/
/* finish parsing */
#define PARSESTATE_FINISH_PARSE     100 /* finish parsing the command line */
/* deal states */
#define PARSESTATE_WORD_DETERMINER  110 /* deal the words determiner */
#define PARSESTATE_WORD_EXCEPTION   120 /* deal the words exception */
#define PARSESTATE_PARA_DETERMINER  130 /* deal the para determiner */
#define PARSESTATE_PARA_EXCEPTION   140 /* deal the para exception */
#define PARSESTATE_ASK_TELNET       150 /* need to ask telnet*/

/* finish deal */
#define PARSESTATE_FINISH_DEAL      200 /* finish dealing the determiner or the exceptions */

/* ִ�г�����CPU�ϵ�λ�� */
#define EXEPOS_MP                   1
#define EXEPOS_XMLRPC                        250
#define EXEPOS_INTERPRETATION       251

#define MAX_LINEWIDTH               80

#define MORE_FLAG_ONE(a) ((((a) & 0x01) > 0) ? TRUE : FALSE)
#define MORE_FLAG_SEQ(a) ((((a) & 0x02) > 0) ? TRUE : FALSE)

/* ������Ϊ������־�ļ�������ĺ���*/
#ifdef WIN32
#define OAM_CFG_PRTCL_CFG_LOG_DIR    "U:/OAM"
#else
#define OAM_CFG_PRTCL_CFG_LOG_DIR    "/DOC0/OAM"
#endif

#ifdef WIN32
    #pragma pack()
#endif

/* ���Ⱪ¶�ӿ� */
int CalculateMatchPosition(TYPE_LINK_STATE *pLinkState);
void ParseCmdLine(TYPE_LINK_STATE   * pLinkState, CHAR *pInString, BYTE bDeterminer, BOOL bIsRestoreProcess);

#endif  /* __CLI_FUNC_H__ */
