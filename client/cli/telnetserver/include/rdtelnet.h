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

#define INSTALL_MP_TELNET      1 /*??��ʶ�Ƿ񷢵������������о��Ƕ����*/
#define OAM_STY_SCROLL_LINE    1 /*??��ʶ�Ƿ�֧����ͬһ�й�����ʾ���ݣ��о��Ƕ����*/

/*��ʾ�õ�һЩ��*/
#define OAM_STY_LINE_MAX            80        /*-������ʾ������ַ���-*/
#define OAM_STY_OUTPUT_LINE_MAX     200
#define OAM_STY_HIDE_LEN            2         /*-�����б༭��ʾ�ַ���,����ʱ����ʾ���ٸ�.-*/
#define OAM_STY_SEPARATE            3         /*-�����б༭�����ַ���-*/
#define OAM_STY_JUMP_LEN            16        /*-�����б༭�����ַ����������п�ʱ�������ַ���-*/
#define OAM_STY_PAGE_LINE           25        /*-һҳ�����ʾ������-*/

#define MAX_STY_NUMBER            7           /*Ŀǰ֧��1������3��TELNET���ӡ�3��SSH����*/
#define MAX_HISTROY_NUMBER        10
#define OAM_STY_PROMPT_LEN        60

/*rpc ��last_pkt_flag �ã�����ȥ���Ǳ�ʶtelnet���͵��Ƿ������һ��*/
#define MSG_MORE_PKT            0
#define MSG_LAST_PKT            1

#define IPV4_ADDR_LEN            4
#define IPV6_ADDR_LEN            16

/* �ն���ʾ�õĿ����ַ�*/
#define MARK_END                '\000'
#define MARK_LEFT               '\010'
#define CUT_CHAR                "\010 \010"

/* �ַ�����*/
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
    CHAR   *disp_info;    /* ��ʾ��Ϣ */
    struct tagDISPLAY_NODE     *p_next;
}_PACKED_1_ DISPLAY_NODE; /* ��ҳ��ʾ��Ϣ�б� */

/*611001013168 ��ETCA��DDM��ѯRPU��·����Ϣʱ����ʱ����ֻ�����Ϣ��ȫ*/
typedef struct tagT_StySendBuf{
    BYTE *buf;
    WORD16 len;
    struct tagT_StySendBuf  *pNext;
}_PACKED_1_ T_StySendBuf;

#define OAM_STY_HISTORY_SIZE       50
#define OAM_STY_RING_SIZE          512
#define OAM_STY_LINE_SIZE          512
/*#define MAX_STR_TO_SHOW            120 ���������*/
#define MAX_SEND_STRING_LEN        OAM_STY_LINE_SIZE /*����ͷ����ַ�������󳤶�*/
#define MSG_HDR_SIZE_OUT           sizeof(TYPE_TELNET_MSGHEAD)

/*sty ��flags �õ��ĸ���״̬*/
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
    JID      interpJid;                /* ����ÿͻ��˵Ľ���ʵ��JID*/
    PID      sshsvrJid;
    BYTE    flags;                    /*���������ն��������,�������OAM_STY_FLAG_��*/
    BYTE    bCeaseAttr;                /*��ǰ�������ֹ����*/
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
    BOOL   sshconnmsg;   /* �Ƿ�ΪSSH������Ϣ */
    BOOL   sshauthmsg;  /* �Ƿ�SSH��¼��֤��Ϣ */
    BOOL   sshauthrslt;  /* SSH�û���¼��� */

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
    BYTE    sty_id;                        /*�ն˱��,ȫ�������±��1*/
    WORD16    edit_pos;                    /*-�����б༭��λ��-*/
    WORD16    line_max;                    /*-������ʾ����λ��-*/
    WORD16    disp_line;                    /*-�ն���ʾ������-*/
    WORD16    disp_pos;                    /*-��ʾ����ʼλ��-*/
    WORD16    fill_len;                        /*-һ�����ѵݽ���ʾ���ַ���-*/
    remote_parse_cookie    *link_cookie;                /*����Ự��Ϣ,����ִ�������*/
    CHAR    prompt_line[OAM_STY_PROMPT_LEN];
    BYTE    ucToShowLabel;                /*�Ƿ�����Ϣ���ٴ�Ҫ����ʱ��ʾ*/
    CHAR    szStrToShow[OAM_STY_LINE_MAX * 2];            /*��ʾ������*/
    DISPLAY_NODE    *p_disp_head;/*-��ҳ��ʾ���б�ͷ-*/
    T_StySendBuf *ptSendBufHead;
    T_StySendBuf *ptSendBufTail; 
}_PACKED_1_ sty;

/*����һ���ն��õ��ĺ����ͷ���ֵ*/
#define ADD_STY_SUCCESS               0    /*AddSty�ɹ�*/
#define ADD_STY_CONNECTIONFULL        1    /*����������*/
#define ADD_STY_INTERPRETSTARTFAIL    2    /*����ʵ������ʧ��*/
#define ADD_STY_SLAVEBOARD            3     /*��ǰ�����Ǳ���*/
#define ADD_STY_OTHERERROR            4     /*��������*/
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
/*����������صĺ���*/
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

