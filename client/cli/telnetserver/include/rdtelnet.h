#ifndef _RDTELNET_H
#define _RDTELNET_H

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

#include "pub_tmp.h"
#include "basesock.h"

#define INSTALL_MP_TELNET      1 /*??标识是否发到其他程序处理，感觉是多余的*/
#define OAM_STY_SCROLL_LINE    1 /*??标识是否支持在同一行滚动显示数据，感觉是多余的*/

/*显示用的一些宏*/
#define OAM_STY_LINE_MAX            80        /*-满行显示的最多字符数-*/
#define OAM_STY_OUTPUT_LINE_MAX     200
#define OAM_STY_HIDE_LEN            2         /*-命令行编辑提示字符数,隐藏时候显示多少个.-*/
#define OAM_STY_SEPARATE            3         /*-命令行编辑隔离字符数-*/
#define OAM_STY_JUMP_LEN            16        /*-命令行编辑滚动字符数，超过行宽时候隐藏字符数-*/
#define OAM_STY_PAGE_LINE           25        /*-一页最多显示的行数-*/

#define MAX_STY_NUMBER            7           /*目前支持1个串口3个TELNET连接、3个SSH连接*/
#define MAX_HISTROY_NUMBER        10
#define OAM_STY_PROMPT_LEN        60

/*rpc 的last_pkt_flag 用，看上去象是标识telnet发送的是否是最后一包*/
#define MSG_MORE_PKT            0
#define MSG_LAST_PKT            1

#define IPV4_ADDR_LEN            4
#define IPV6_ADDR_LEN            16

/* 终端显示用的控制字符*/
#define MARK_END                '\000'
#define MARK_LEFT               '\010'
#define CUT_CHAR                "\010 \010"

/* 字符常量*/
#define MARK_DOT                '.'
#define MARK_PSWD               '*'
#define MARK_BLANK              ' '

typedef struct tagRemote_parse_cookie
{
    BYTE    msg_id;
    BYTE    vty_num;
    BYTE    last_pkt_flag;
    BYTE    determiner;        
    WORD16    seq_no;         /* interactive use from 1 ~ 65535, 0 reserved */
    WORD16    data_len;    
    XOS_TIMER_ID dwTimerNo;
}remote_parse_cookie;

typedef struct tagDISPLAY_NODE
{
    CHAR   *disp_info;    /* 显示信息 */
    struct tagDISPLAY_NODE     *p_next;
}_PACKED_1_ DISPLAY_NODE; /* 分页显示信息列表 */

/*611001013168 【ETCA】DDM查询RPU上路由信息时，有时会出现回显信息不全*/
typedef struct tagT_StySendBuf{
    BYTE *buf;
    WORD16 len;
    struct tagT_StySendBuf  *pNext;
}_PACKED_1_ T_StySendBuf;

#define OAM_STY_HISTORY_SIZE       50
#define OAM_STY_RING_SIZE          512
#define OAM_STY_LINE_SIZE          512
/*#define MAX_STR_TO_SHOW            120 处理半行用*/
#define MAX_SEND_STRING_LEN        OAM_STY_LINE_SIZE /*向解释发送字符串的最大长度*/
#define MSG_HDR_SIZE_OUT           sizeof(TYPE_TELNET_MSGHEAD)

/*sty 的flags 用到的各种状态*/
#define OAM_STY_FLAG_STOPINPUT       0x0001
#define OAM_STY_FLAG_EXECMODE        0x0002
#define OAM_STY_FLAG_ECHOING         0x0004
#define OAM_STY_FLAG_PASSWORD        0x0008
#define OAM_STY_FLAG_USENAME         0x0010
#define OAM_STY_FLAG_RAWMODE         0x10 /*?? equal OAM_STY_FLAG_USENAME ?*/
#define OAM_STY_FLAG_REMOTE_PARSE    0x0020
#define OAM_STY_FLAG_NODISPLAY       0x0080
#define OAM_STY_FLAG_EXEINPUT        0x0040

struct sshd_options {
	const CHAR *hostkey;	/* Filename of host key. */
	const CHAR *address;	/* Address to listen on. */
	const CHAR *port;	/* Port to listen on. */
	const CHAR *jidfile;
	DWORD   v1_compat_mode;
	DWORD keepalive;
};

struct channel_info {
	int used;
	int channel_id;
	int remote_id;

	int fd_normal;
	int fd_ext_stderr;

	int is_closing;
	int window_size;

	int max_packet_size;
};


#define MAX_CHANNEL    4

typedef struct tagSty
{
    TYPE_FD   *tSocket;
    JID      interpJid;                /* 处理该客户端的解释实例JID*/
    PID      sshsvrJid;
    BYTE    flags;                    /*用来控制终端输入输出,见上面的OAM_STY_FLAG_宏*/
    BYTE    bCeaseAttr;                /*当前命令的中止属性*/
    BYTE    firstpacket;              /* add by wushg */
    BYTE     session_identification_done;
    BYTE     protocol_version;
    BYTE     beused;
    DWORD    inputdataleftlen;
    DWORD    usernamelen;
    DWORD    running_as_server;
    DWORD    passwordlen;
    DWORD    firstpkt_decrypt;
    DWORD    sessinitdone;
    DWORD    supported_ciphers;
    DWORD    supported_auths;

    BYTE    *username;
    BYTE   *password;
    BYTE   *inputdataleft;
    struct sshd_options opts;
    BOOL   sshconnmsg;   /* 是否为SSH连接消息 */
    BOOL   sshauthmsg;  /* 是否SSH登录验证消息 */
    BOOL   sshauthrslt;  /* SSH用户登录结果 */

    struct ssh_cipher *cipher;	/* type, enc/dec functions, key data */
	
    BYTE    ring_buf[OAM_STY_RING_SIZE];    /* input character ring buffer */
    WORD16 ring_start;
    WORD16 ring_end;        /* portion of ring buffer in use */
    CHAR    line_buf[OAM_STY_LINE_SIZE];    /* input line buffer */
    WORD16    line_len;                /* length of current line */
    CHAR    *history[OAM_STY_HISTORY_SIZE];    /* command line history */
    /*struct ssh_v1_context v1_ctx;
    struct ssh_v2_context v2_ctx;*/
    struct channel_info channels[MAX_CHANNEL];
    WORD16    history_ptr;                    /* current command history line */
    BYTE    sty_id;                        /*终端编号,全局数组下标加1*/
    WORD16    edit_pos;                    /*-命令行编辑的位置-*/
    WORD16    line_max;                    /*-允许显示的列位数-*/
    WORD16    disp_line;                    /*-终端显示的行数-*/
    WORD16    disp_pos;                    /*-显示的起始位置-*/
    WORD16    fill_len;                        /*-一行中已递交显示的字符数-*/
    remote_parse_cookie    *link_cookie;                /*保存会话信息,命令执行情况等*/
    CHAR    prompt_line[OAM_STY_PROMPT_LEN];
    BYTE    ucToShowLabel;                /*是否还有信息在再次要数据时显示*/
    CHAR    szStrToShow[OAM_STY_LINE_MAX * 2];            /*显示的内容*/
    DISPLAY_NODE    *p_disp_head;/*-分页显示的列表头-*/
    T_StySendBuf *ptSendBufHead;
    T_StySendBuf *ptSendBufTail; 
}_PACKED_1_ sty;

/*新增一个终端用到的函数和返回值*/
#define ADD_STY_SUCCESS               0    /*AddSty成功*/
#define ADD_STY_CONNECTIONFULL        1    /*连接数已满*/
#define ADD_STY_INTERPRETSTARTFAIL    2    /*解释实例启动失败*/
#define ADD_STY_SLAVEBOARD            3     /*当前单板是备板*/
#define ADD_STY_OTHERERROR            4     /*其他错误*/
void ReadTelnet(TYPE_FD *tSocket, CHAR *buf, WORD16 iLen);
void terminate_conn_to_remote_parse(sty *vty);
/*extern void rcv_msg_from_remote_parse(LPVOID pMsgPara);*/
void oam_sty_close(sty *vty);
void make_conn_to_remote_parse(sty *vty);

void ProcessSSHAuthEvent(TYPE_FD *tSocket, CHAR *buf, int iLen);
void send_close_ssh(sty *vty);
void oam_fd_send_for_ssh(WORD16 fd, CHAR *text);
void oam_fd_quit_for_ssh(WORD16 fd);
void oam_ssh_conn_ack(sty *cty, BYTE* pMsgPara);
void oam_sty_put_prompt_cmdline(sty *cty);
WORD16 sdp_puts(sty *cty, CHAR *text);
void oam_sty_roll_progress(sty *cty, CHAR *text);
void oam_sty_put_prompt_line(sty *cty, CHAR *p, int nlen);
void oam_sty_put_command_line(sty *cty, CHAR *p, WORD16 nlen);
void oam_sty_input_password(sty *cty, int flag);
WORD16 oam_sty_puts_with_line_process(sty *cty, CHAR *text);
void oam_sty_enable_input(sty *sty, BOOL bFlag);
/*具体命令相关的函数*/
void send_data_to_remote_parse(sty *cty, CHAR *data, BYTE datalen);
 int req_more_data_to_remote_parse(sty *vty);


void Oam_CfgClearSendBuf(sty *ptSty);
/*
BYTE  OAMinputdata[65532]= {0};
*/

#define MSG_NATPT_BASE               	(1)
#define MSG_SSH_DHKEX_SETUP           	(MSG_NATPT_BASE + 1)
#define MSG_SSH_DHKEX_FINISH		    (MSG_NATPT_BASE + 2)
#define MSG_SSH_RSAKEX_SETUP            (MSG_NATPT_BASE + 3)
#define MSG_SSH_RSAKEX_FINISH		    (MSG_NATPT_BASE + 4)


#ifdef WIN32
    #pragma pack()
#endif

#endif

