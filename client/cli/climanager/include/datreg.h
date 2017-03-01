/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�datreg.h
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
#ifndef __DATREG_H__
#define __DATREG_H__

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

#define OAM_DAT_NEW 0
#define OAM_DAT_OLD 1

#define OAM_CFG_DAT_FUNCTYPE_OAM  FUNC_SCRIPT_CLIS_OAM  /*oam���õ�dat��������*/
#define FUNC_INVALID_DAT 0

extern BYTE OAM_CFGDatRegister(const JID tJid, const WORD32 dwDatFuncType);
extern BYTE OAM_CFGDatRegisterForRestore(const JID tJid, const WORD32 dwDatFuncType);
extern BYTE OAM_CFGDatUnregister(const JID tJid);

extern void OAM_CFGRegisterDat(T_OAM_Cfg_Dat *ptCfgDat, const JID tJid);
extern void OAM_CFGUnRegisterDat(const JID);

extern void OAM_CliReRegisterAllRegistedJobs(void);
extern T_DAT_LINK * Oam_CliSearchDat(const JID);
#ifdef WIN32
    #pragma pack()
#endif

#endif /* __DATREG_H__ */
