/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_fd.h
* 文件标识：
* 内容摘要：OAM回调函数部分
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
#ifndef __OAM_FD__
#define __OAM_FD__

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

#define OAM_OK          0
#define OAM_ERROR       (-1)

#define MAX_BUF_LEN     512

typedef struct tagOAM_FD_MSG
{
    WORD16    fd;      /*文件描述符*/
    WORD16    flag;    /*连接类型 1-串口 2-TELNET 3-HTTP 4-SSH*/
    WORD16    len;     /*消息长度*/
    BYTE      buf[MAX_BUF_LEN]; /*消息体*/
}_PACKED_1_ OAM_FD_MSG;

typedef struct 
{
    WORD16    fd;      /*文件描述符*/
    WORD16    flag;    /*连接类型 1-串口 2-TELNET 3-HTTP 4-SSH*/
    WORD16    len;     /*消息长度*/
    BYTE      buf[1];  /*消息体*/
}_PACKED_1_ OAM_FD_MSG_EX;

typedef struct tagOAM_SSH_TEL_MSG
{
    WORD16    fd;      /*文件描述符*/
    WORD16    result;  /*成功与否，1-成功,0-失败 */
    WORD16    len;     /*消息长度*/
    BYTE      buf[MAX_BUF_LEN]; /*消息体*/
}_PACKED_1_ OAM_SSH_TEL_MSG;


extern WORD32 OamRegCallback(void);
extern WORD32 OamRegConsoleCallback(void);

#ifdef WIN32
    #pragma pack()
#endif

#endif
