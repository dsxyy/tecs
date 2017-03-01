/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：datreg.h
* 文件标识：
* 内容摘要：OAM解释进程
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.12
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
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

#define OAM_CFG_DAT_FUNCTYPE_OAM  FUNC_SCRIPT_CLIS_OAM  /*oam自用的dat功能类型*/
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
