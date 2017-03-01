/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�CliApp.h
* �ļ���ʶ��
* ����ժҪ�������ڲ���Ϣ
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
/*CMDID����*/
/*�궨�壬��ҪΪ������Ϣ����ʱ�ṩ�ĺ�ֵ*/

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


/*cli�������ͱ���
#define OAM_CLI_TABLENAME_R_CLILANGUAGETYPE  "R_CLILANGUAGETYPE"*/
#define MAPTYPE_CLI_LANGTYPE      8/*cli��������map����ֵ*/

extern void Oam_InnerApp(LPVOID ptOamMsg,WORD32 dwMsgId, MSG_COMM_OAM *ptRtnMsg);
#ifdef WIN32
    #pragma pack()
#endif

#endif /* __CLIAPP_H__ */
