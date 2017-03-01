#ifndef _SSH_CHANNEL_H_
#define _SSH_CHANNEL_H_
/* channel->type values */
#define CHANNEL_ID_NONE 0
#define CHANNEL_ID_SESSION 1
#define CHANNEL_ID_X11 2
#define CHANNEL_ID_AGENT 3

#define RECV_MAXWINDOW 24576 /*  tweak */
#define RECV_MAXPACKET 32768 /* 1400tweak */
#define RECV_MINWINDOW 10000 /* when we get below this, we send a windowadjust*/

typedef struct ChanSess 
{
	unsigned char * cmd; /* command to exec */
	/* pty details */
	char term[64];
	unsigned int termw, termh, termc, termr; /* width, height, col, rows */
    BYTE   writebuf[1024];
}TsshChanSess;
typedef struct Channel 
{
	WORD32 index; /* the local channel index */
	WORD32 remotechan;
	WORD32 recvdonelen;
	BYTE   type; /* CHANNEL_ID_SESSION, CHANNEL_ID_X11 etc */
	WORD32 recvwindow, transwindow;
	WORD32 recvmaxpacket, transmaxpacket;
	TsshChanSess typedata; 
}TsshChannel;


#endif

