/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�clirecvreg.h
* �ļ���ʶ��
* ����ժҪ��OAM���ͽ���
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.12
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __CLI_RECVREG_H__
#define __CLI_RECVREG_H__

extern void  OamCliAgingShm(T_OAM_INTERPRET_VAR *ptVar);
extern void RecvAgtReg(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void RecvAgtRegSpecial(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void RecvAgtUnreg(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void RecvAgtRegEx(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void Oam_CfgRecvRegA2M(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void RecvRegExA2M(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void Oam_CfgScanRegTable(void);
extern BOOLEAN OamRegCntOfShm(WORD32 *pdwRegedCnt);

#endif /* __CLI_RECVREG_H__ */
