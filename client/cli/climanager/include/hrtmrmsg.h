/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrTmrMsg.h
* �ļ���ʶ��
* ����ժҪ���������Զ�ʱ���ĳ�ʱ��Ϣ
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�������
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __HR_TMR_MSG_H__
#define __HR_TMR_MSG_H__

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
extern void RecvFromTimer(TYPE_LINK_STATE *pLinkState,WORD16 wMsgId);
extern void RecvFromXmlAdapterTimer(TYPE_XMLLINKSTATE *pLinkState,WORD16 wMsgId);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __HR_TMR_MSG_H__ */
