#ifndef _SSH_SESSION_H
#define _SSH_SESSION_H

#include "pub_tmp.h"             
#include "pub_addition.h"

#include <arpa/inet.h>

#include "oam_fd.h"
#include "oam_cfg.h"
#include "oam_cfg_common.h"
#define  MAXBLOCKSIZE 128

#include "ssh_pub.h"
#include "ssh_dss.h"
#include "ssh_sha1.h"
#include "ssh_hmac.h"
#include "ssh_buffer.h"
#include "ssh_des.h"
#include "ssh_channel.h"

#define S_Init                    (WORD)0
#define S_Work                    (WORD)1

#define MAX_SSH_USER  3  /*最大客户端数*/

#define SSH_BANNER               "      *************************************************************\r\n             Welcome to CLI config Shell of ZTE Corporation\r\n      *************************************************************\r\n"


#define  SSH_MAX_PACKAGE_LENGTH  4096  /* 35000 */
#define  SSH_MAX_PAYLOAD_LENGTH  3000  /* 32768 */
#define  SSH_MAX_PADDING_LENGTH  255
#define  SSH_MAX_MAC_LENGTH      20

#define NTOHL(a)   a = ntohl(a)
#define HTONL(a)   a = htonl(a)

#define GETDWORD(ptrData) (*(WORD32*)(ptrData))
#define SETDWORD(ptrData,data) (*(WORD32*)(ptrData)=data)
#define GETWORD(ptrData) (*(WORD16*)(ptrData))
#define SETWORD(ptrData,data) (*(WORD16*)(ptrData)=data)

#ifndef BIG_ENDINE
#define DEFDWORD(char1,char2,char3,char4) (char1+(char2<<8)+(char3<<16)+(char4<<24))
#define DEFWORD(char1,char2) (char1+(char2<<8))
#else
#define DEFDWORD(char1,char2,char3,char4) (char4+(char3<<8)+(char2<<16)+(char1<<24))
#define DEFWORD(char1,char2) (char2+(char1<<8))
#endif

#define SSH_VERSION_END DEFWORD('\x0D','\x0A')

#define  MAX_VERSION_LENGTH 255 /* 版本字符串最大长度 */

#define SSH_OK 0
#define SSH_ERR -1

#define TRACE(X) dropbear_trace X;

/* 获取telnet连接应答定时器 */
#define TIMER_SSH_GET_CONNACK          (TIMER_NO_1)
#define EV_TIMER_GET_CONNACK           (EV_TIMER_1)
#define DURATION_GET_CONNACK        (SWORD32)(5*1000)

/* SSH空闲超时定时器 */
#define TIMER_SSH_IDLE_CONNACK          (TIMER_NO_2)
#define EV_TIMER_IDLE_CONNACK           (EV_TIMER_2)
#define DURATION_IDLE_CONNACK        (SWORD32)(120*60*1000)


/* SSH package结构 */
typedef struct tagSSHPackage
{
    WORD32   dwPackLen;       
    BYTE     ucPadLen;         
    BYTE	 *pucPayload;            
	BYTE	 *pucPadding;
	BYTE	 *pucMAC;     
}TSSHPack;

struct des3_key 
{
    WORD32 ek[3][32], dk[3][32];
};

typedef union Symmetric_key 
{
	struct des3_key des3;
} symmetric_key;

/* cipher descriptor table, last entry has "name == NULL" to mark the end of table */
struct _cipher_descriptor 
{
   char *name;
   unsigned char ID;
   int  min_key_length, max_key_length, block_length, default_rounds;
   int  (*setup)(const unsigned char *key, int keylength, int num_rounds, symmetric_key *skey);
   void (*ecb_encrypt)(const unsigned char *pt, unsigned char *ct, symmetric_key *key);
   void (*ecb_decrypt)(const unsigned char *ct, unsigned char *pt, symmetric_key *key);
   int  (*keysize)(int *desired_keysize);
};


struct _hash_descriptor
{
    char *name;
    unsigned char ID;
    unsigned long hashsize;       /* digest output size in bytes  */
    unsigned long blocksize;      /* the block size the hash uses */
    void (*init)(hash_state *);
    void (*process)(hash_state *, const unsigned char *, unsigned long);
    void (*done)(hash_state *, unsigned char *);
    int  (*test)(void);
};

extern struct _hash_descriptor* hash_descriptor[];

struct dropbear_cipher
{
	const struct _cipher_descriptor *cipherdesc;
	unsigned long keysize;
	unsigned char blocksize;
};

struct dropbear_hash
{
	const struct _hash_descriptor *hashdesc;
	unsigned long keysize;
	unsigned char hashsize;
};

/* A block cipher CBC structure */
typedef struct Symmetric_CBC
{
	int                 cipher, blocklen;
	unsigned char       IV[MAXBLOCKSIZE];
	symmetric_key       key;
} symmetric_CBC;


typedef struct tagKeyContext
{
	const struct dropbear_cipher *recv_algo_crypt;  /* NULL for none */
	const struct dropbear_cipher *trans_algo_crypt; /* NULL for none */
	const struct dropbear_hash *recv_algo_mac;      /* NULL for none */
	const struct dropbear_hash *trans_algo_mac;     /* NULL for none */

    char algo_kex;
	char algo_hostkey;
	char recv_algo_comp; /* compression */
	char trans_algo_comp;

	/* actual keys */
	symmetric_CBC recv_symmetric_struct;
	symmetric_CBC trans_symmetric_struct;
	BYTE recvmackey[SSH_MAX_MAC_LENGTH];
	BYTE transmackey[SSH_MAX_MAC_LENGTH];
}TKeyContext;
#if 0
struct channel 
{
	WORD32 index; /* the local channel index */
	WORD32 remotechan;
	unsigned char type; /* CHANNEL_ID_SESSION, CHANNEL_ID_X11 etc */
	unsigned int recvwindow, transwindow;
	unsigned int recvmaxpacket, transmaxpacket;
	void* typedata; /* a pointer to type specific data */
	int infd; /* stdin for the program, we write to this */
	int outfd; /* stdout for the program, we read from this */
	int errfd; /* stdout for a program. This doesn't really fit here,
				but makes the code a lot tidyer without being too bad. This
				is -1 for channels which don't requre it. Currently only
			   a 'session' without a pty will use it */
	void* writebuf; /* data for the program */
    int sentclosed; /* whether we've reached the end of reading/writing to/from/err for a pipe
			   * or program */
	int transeof, recveof, erreof; 
};
#endif
typedef struct SIGN_key
{
	dss_key * dsskey;
}sign_key;

struct sshsession 
{
	SOCKET sockfd;
    BYTE   verHasExchange;                      /* 是否进行过版本交互 */
	char   clientVersion[MAX_VERSION_LENGTH];	/*客户端版本字符串*/
	SWORD32 dwRcvSeq;							/*接收的消息序号*/
	SWORD32 dwSndSeq;							/*发送的消息序号*/
	BYTE   aucRcvBuf[SSH_MAX_PACKAGE_LENGTH];	/*接收消息缓存--收到的消息原版*/
    BYTE   aucRcvDecryptBuf[SSH_MAX_PACKAGE_LENGTH];	/*解密后接收消息缓存*/
	BYTE   aucSndBuf[SSH_MAX_PACKAGE_LENGTH];	/*发送消息缓存*/
    BYTE   aucSndEncryptBuf[SSH_MAX_PACKAGE_LENGTH]; /*加密后发送消息缓存--发送的消息原版*/
    BYTE   *pRcvData;                           /*指向接收缓存当前位置*/
    BYTE   *pRcvDecryptData;
	BYTE   *pSndData;
    BYTE   *pSndEncryptData;

    buffer *pKexBuf;
    
	BYTE   aucSessionId[SHA1_HASH_SIZE];		/*the hash from the first kex*/
    BYTE   aucHash[SHA1_HASH_SIZE];
	struct tagKeyContext tNewKey;					/*会话密钥*/
	sign_key tHostKey;					/*主机密钥*/	
	/* these are used temorarily during kex, are freed after use */
	mp_int  dh_K; /* SSH_MSG_KEXDH_REPLY and sending SSH_MSH_NEWKEYS */
    #if 0
	BYTE    *kexHashBuf; /* session hash buffer 即 H */
	BYTE    *transKexInit; /* server发送的kexinit payload */
    #endif
	/* channels */
	TsshChannel *ptChannel; /* these pointers may be null */
    #if 0
	/* 标识 */
	BYTE   exchangeVersion; /*是否已经进行过版本交换*/
	BYTE   sendNewKeys;     /*是否发送过SSH_MSG_NEWKEYS消息*/
    #endif
    BYTE aucPromptBuf[255];
    XOS_TIMER_ID wConnTimer;  /* 连接超时定时器 */
    XOS_TIMER_ID wIdleTimer;  /* 空闲超时定时器 */
};
typedef struct sshsession TSSHSession;

extern TSSHSession* g_ptSSHSession;
extern struct _cipher_descriptor des3_desc;
extern struct _hash_descriptor sha1_desc;

extern struct _cipher_descriptor* cipher_descriptor[];

extern struct _hash_descriptor* hash_descriptor[];

extern const struct dropbear_cipher dropbear_3des;

extern const struct dropbear_hash dropbear_sha1;

extern void InitialSess(TSSHSession *ptsess);
extern TSSHSession * GetSessBySockFd(SOCKET fd);

extern JID jidTelnet;
extern void FillPkgHeadLen(WORD32 dwPayloadLen,BYTE *pSndBuf);
extern void SSHSendPkg(TSSHSession *ptSess);
extern void SSHSendDisconnect(WORD32 dwReasonCode,SOCKET sockfd,char *pstrDesc);
extern void SSHSendDisconnectEncrypt(WORD32 dwReasonCode,TSSHSession *ptSess,char *pstrDesc);

extern void SSHSendDebugMsg(char *msg,TSSHSession *ptSess);
extern void SSHSendBannerMsg(char *msg,TSSHSession *ptSess);

extern void dropbear_trace(const char* format, ...);

#endif
