/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrTelMsg.h
* �ļ���ʶ��
* ����ժҪ����������Telnet����Ϣ
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __HR_TEL_MSG_H__
#define __HR_TEL_MSG_H__

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

/* ���Ⱪ¶�ӿ� */
extern void RecvFromTelnet(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara);
extern BOOLEAN Oam_CfgGlobalPointersIsOk(void);
extern void Oam_CfgStopExcutingCmd(TYPE_LINK_STATE *pLinkState, CHAR *pstrReason);
extern void Omm_CfgStopExcutingCmd(TYPE_XMLLINKSTATE *pLinkState);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __HR_TEL_MSG_H__ */
