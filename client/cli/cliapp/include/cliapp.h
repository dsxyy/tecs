/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：CliApp.h
* 文件标识：
* 内容摘要：处理内部消息
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __CLIAPP_H__
#define __CLIAPP_H__

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
/*CMDID定义*/
/*宏定义，主要为错误信息出现时提供的宏值*/

#define     SlaveSaveOk      228  
#define     SlaveSaveFai     229  
#define     SlaveAckTimeout     231  
#define     SaveZDBOK     232
#define     SaveZDBFail      233  
#define     SaveZDBErrSaveBothPath     234  
#define     SaveZDBErrMainPath     235
#define     SaveZDBErrBackup       236
#define     SaveZDBErrLocalSaving     237
#define     SaveZDBErrMSSaving     238 
#define     SaveZDBErrOther      239

#define     NotAdmin     240  
#define     CannotDelAdmin     241  
#define     UserNotExists     242  
#define     UserBeenDeleted     243  
#define     CmdExecSucc     244  
#define     CmdExecFailed     245  
#define     CannotCreateAdmin    246
#define     UserDoesExists     247
#define     PasswordRule     248
#define     PwdNotMeetRule     249  
#define     TwoInputPwdNotSame     250 
#define     ConfUserFull     251
#define     ViewUserFull     252


#define     UserLocked     253  
#define     CannotSetAdminRole     254  
#define     UserOnline     255
#define     NeedNotSetRole     256  
#define     CannotSetOtherUserPwd     257  
#define     UserNotLocked     258
#define     NoPermission    259
#define     SaveZDBErrNoTblSave     260
#define     SaveZDBErrReadFileErr     261
#define     SaveZDBErrTblNotNeedSave     262  
#define     SaveZDBErrPowerOnLoading     263 
#define     SaveZDBErrFtpLoading     264
#define     SaveZDBErrAppFtpLoading     265
#define     SaveZDBErrIOBusy     266
#define     SaveZDBErrInvalidHandle     267
#define     OpenFileFail     268
#define     ReadFileFail     269
#define     GetFileLengthFail     270
#define     FileLengthNotRigth     271
#define     StrongPasslen     272
#define     SimplePasslen     273


/*cli语言类型表名
#define OAM_CLI_TABLENAME_R_CLILANGUAGETYPE  "R_CLILANGUAGETYPE"*/
#define MAPTYPE_CLI_LANGTYPE      8/*cli语言类型map类型值*/

extern void Oam_InnerApp(LPVOID ptOamMsg,WORD32 dwMsgId, MSG_COMM_OAM *ptRtnMsg);
#ifdef WIN32
    #pragma pack()
#endif

#endif /* __CLIAPP_H__ */
