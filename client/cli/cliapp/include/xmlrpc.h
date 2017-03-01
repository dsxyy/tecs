/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�SavePrtclCfg.h
* �ļ���ʶ��
* ����ժҪ���������ô���
* ����˵����
            
* ��ǰ�汾��
* ��    ��       �����
* ������ڣ�20080307
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __CLI_XMLRPC_H__
#define __CLI_XMLRPC_H__

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

extern WORD32 tCliMsgBufMutexSem;

#define MAX_METHOD_NUM                      1000
#define MAX_METHOD_NAME_LEN            50
#define MAX_METHOD_PARAM_NUM         255
typedef struct tagMethodConfig
{
    DWORD dwCmdId;
    BOOLEAN bIsNo;
    CHAR    aucMethodName[MAX_METHOD_NAME_LEN];
}T_MethodConfig;

extern void DisposeXmlRpcCmd(TYPE_LINK_STATE *pLinkState);
extern BOOLEAN LoadClientConfigInfo();
extern bool CliLoginAuth(TYPE_LINK_STATE *pLinkState);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __SAVE_PROTOCOL_CONFIG_H__*/



