/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：clirecvreg.h
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
