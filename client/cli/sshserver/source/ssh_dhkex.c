#if 1
#include "ssh_session.h"
#include "ssh_string.h"
#include "ssh_pub.h"
#include "ssh_signkey.h"
#include "ssh_crypt.h"
static BOOL KexInit(TSSHSession *ptSess, CHAR * message_body, const WORD32 message_length);
static CHAR *SetKexInit(CHAR * buffer);

/* diffie-hellman-group1-sha1 values for g and p */
static const BYTE dh_p_val[] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
        0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
        0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
        0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
        0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
        0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
        0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
        0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
        0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
        0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE6, 0x53, 0x81,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
static const WORD dh_g_val = 2;

/* Create the packet mac, and append H(seqno|clearbuf) to the output */
static void writemac(TSSHSession *ptSess)
{
    WORD16 wMacSize;
    BYTE seqbuf[4];
    WORD32 dwHashSize;
    hmac_state hmac;
    WORD32 dwPackSize;
    TSSHPack *ptSshPack = (TSSHPack *)ptSess->aucSndBuf;

    wMacSize = ptSess->tNewKey.trans_algo_mac->hashsize;

    dwPackSize = ntohl(ptSshPack->dwPackLen)+sizeof(WORD32);
    if (wMacSize > 0)
    {
        /* calculate the mac */
        if (t_hmac_init(&hmac, 0, ptSess->tNewKey.transmackey, 20) != SSH_OK)
        {
            printf("HMAC error");
        }

        /* sequence number */
        STORE32H(ptSess->dwSndSeq, seqbuf);
        if (hmac_process(&hmac, seqbuf, 4) != SSH_OK)
        {
            printf("HMAC error");
        }

        /* the actual contents */
        if (hmac_process(&hmac, (BYTE*)ptSess->aucSndBuf, dwPackSize) != SSH_OK)
        {
            printf("HMAC error");
        }

        dwHashSize = wMacSize;
        if (hmac_done(&hmac, (BYTE*)ptSess->aucSndEncryptBuf+dwPackSize, &dwHashSize) != SSH_OK)
        {
            printf("HMAC error");
        }
    }
}

void EncryptPacket(TSSHSession *ptSess)
{
    WORD32 dwPackSize = *(WORD32 *)(ptSess->aucSndBuf);
    dwPackSize = ntohl(dwPackSize)+sizeof(WORD32);
    BYTE *in = (BYTE*)ptSess->aucSndBuf;
    BYTE *out = (BYTE*)ptSess->aucSndEncryptBuf;
    while (dwPackSize>0)
    {
        if (cbc_encrypt(in,out,&ptSess->tNewKey.trans_symmetric_struct)!= SSH_OK)
        {
            printf("error encrypting");
        }
        in  += ptSess->tNewKey.trans_algo_crypt->blocksize;
        out += ptSess->tNewKey.trans_algo_crypt->blocksize;
        dwPackSize -= ptSess->tNewKey.trans_algo_crypt->blocksize;
    }
    writemac(ptSess);
}


/* 收到 SSH_MSG_KEXINIT的处理函数 */
void HandleMsgKexInit(TSSHSession *ptSess,TSSHPack *ptPack)
{
    WORD32 dwPayloadLen = ptPack->dwPackLen-ptPack->ucPadLen-1;

    if (KexInit(ptSess, (CHAR *)ptPack->pucPayload+sizeof(BYTE), dwPayloadLen))
    {
        /* 发送服务器的KEX_INIT */
        CHAR *payload = (CHAR *)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
        WORD32 dwSndPayloadLen = SetKexInit(payload) - payload;

        FillPkgHeadLen(dwSndPayloadLen, ptSess->aucSndBuf);
        
        /* V_C, the client's version string (CR and NL excluded) */
        buf_putstring(ptSess->pKexBuf,(BYTE*)ptSess->clientVersion,strlen(ptSess->clientVersion));
        /* V_S, the server's version string (CR and NL excluded) */
        buf_putstring(ptSess->pKexBuf,(BYTE*)LOCAL_IDENT, strlen(LOCAL_IDENT));
        /* I_C, the payload of the client's SSH_MSG_KEXINIT */
        buf_putstring(ptSess->pKexBuf,ptSess->aucRcvDecryptBuf+sizeof(WORD32)+sizeof(BYTE),
                      ptPack->dwPackLen - ptPack->ucPadLen -sizeof(BYTE));
        /* I_S, the payload of the server's SSH_MSG_KEXINIT */
        buf_putstring(ptSess->pKexBuf,ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE),dwSndPayloadLen);

		SSHSendPkg(ptSess);

    }
    else
    {
        printf("error occur in Recv_Msg_KexInit!");
    }
}

unsigned int counter = 0;
BYTE hashpool[SHA1_HASH_SIZE];
#define MAX_COUNTER 1000000/* the max value for the counter, so it won't loop 
*/
/* return len bytes of pseudo-random data */
void genrandom(BYTE* buf1, unsigned int len)
{

    hash_state hs;
    BYTE hash[SHA1_HASH_SIZE];
    unsigned int copylen;

    while (len > 0)
    {
        sha1_init(&hs);
        sha1_process(&hs, (BYTE*)hashpool, sizeof(hashpool));
        sha1_process(&hs, (BYTE*)&counter, sizeof(counter));
        sha1_done(&hs, hash);

        counter++;
        if (counter > MAX_COUNTER)
        {
            printf("error:counter > MAX_COUNTER");
            /*seedrandom();*/
        }

        copylen = MIN(len, SHA1_HASH_SIZE);
        memcpy(buf1, hash, copylen);
        len -= copylen;
        buf1 += copylen;
    }

    m_burn(hash, sizeof(hash));

}
/* Clear the data, based on the method in David Wheeler's
* "Secure Programming for Linux and Unix HOWTO" */
void m_burn(void *data, unsigned int len)
{
    volatile CHAR *p = (CHAR*)data;

    if (data == NULL)
        return;
    while (len--)
    {
        *p++ = 0x66;
    }
}

/* 收到 SSH_MSG_KEXDH_INIT的处理函数 */
int HandleMsgDHKexInit(TSSHSession *ptSess,TSSHPack *ptPack)
{
    buffer *writepayload;
    mp_int e;

    sign_key *key;
    WORD32 dwPayloadLen = ptPack->dwPackLen -ptPack->ucPadLen-sizeof(BYTE);
    WORD32 dwBufLen = dwPayloadLen -1 ; /* 去掉message type */
    WORD32 dweLen;
    BYTE hash[SHA1_HASH_SIZE]; /* the hash*/
    BYTE *e_buf = ptPack->pucPayload+sizeof(BYTE);
    mp_init(&e);
    mp_init(&ptSess->dh_K);
    mp_zero(&e);
    mp_zero(&ptSess->dh_K);
    /* get 'e' from buf */
    memcpy(&dweLen,e_buf,sizeof(WORD32));
    e_buf+=sizeof(WORD32);
    dweLen = ntohl(dweLen);
    dwBufLen -= sizeof(WORD32);
    mp_read_unsigned_bin(&e,(BYTE*)e_buf,dwBufLen);

    {
        mp_int dh_p, dh_q, dh_g, dh_y, dh_f;
        BYTE randbuf[DH_P_LEN];
        int dh_q_len;
        hash_state hs;
        mp_init(&dh_g);
        mp_init(&dh_p);
        mp_init(&dh_q);
        mp_init(&dh_y);
        mp_init(&dh_f);

        /* read the prime and generator*/
        if (mp_read_unsigned_bin(&dh_p, (BYTE*)dh_p_val, DH_P_LEN)!= MP_OKAY)
        {
            return MP_ERROR;
        }

        if (mp_set_int(&dh_g, dh_g_val) != MP_OKAY)
        {
            return MP_ERROR;
        }

        /* calculate q = (p-1)/2 */
        if (mp_sub_d(&dh_p, 1, &dh_y) != MP_OKAY)
        {
            /*dh_y is just a temp var here*/
            return MP_ERROR;
        }

        if (mp_div_2(&dh_y, &dh_q) != MP_OKAY)
        {
            return MP_ERROR;
        }

        dh_q_len = mp_unsigned_bin_size(&dh_q);
        /* calculate our random value dh_y */
        do
        {
            /* assert((unsigned int)dh_q_len <= sizeof(randbuf)); */
            genrandom(randbuf, dh_q_len);
            if (mp_read_unsigned_bin(&dh_y, randbuf, dh_q_len) != MP_OKAY)
            {
                return MP_ERROR;
            }
        }
        while ((mp_cmp(&dh_y, &dh_q) == MP_GT) || (mp_cmp_d(&dh_y, 0) != MP_GT));

        /* f = g^y mod p */
        if (mp_exptmod(&dh_g, &dh_y, &dh_p, &dh_f) != MP_OKAY)
        {
            return MP_ERROR;
        }

        mp_clear(&dh_g);
        /* K = e^y mod p */
        mp_init(&ptSess->dh_K);
        if (mp_exptmod(&e, &dh_y, &dh_p, &ptSess->dh_K) != MP_OKAY)
        {
            return MP_ERROR;
        }

        /* clear no longer needed vars */
        mp_clear(&dh_y);
        mp_clear(&dh_p);
        mp_clear(&dh_q);
        /* Create the remainder of the hash buffer, to generate the exchange hash */
        /* K_S, the host key */
        key = (sign_key*)XOS_GetUB(sizeof(sign_key));
        key->dsskey = gen_dss_priv_key(); /* 128 bytes = 1024 bit */
        buf_put_pub_key(ptSess->pKexBuf, key);
        /* e, exchange value sent by the client */
        buf_putmpint(ptSess->pKexBuf, &e);
        /* f, exchange value sent by the server */
        buf_putmpint(ptSess->pKexBuf, &dh_f);
        /* K, the shared secret */
        buf_putmpint(ptSess->pKexBuf, &ptSess->dh_K);
        /* calculate the hash H to sign */
        sha1_init(&hs);
        ptSess->pKexBuf->pos = 0;
        sha1_process(&hs, &ptSess->pKexBuf->data[ptSess->pKexBuf->pos],ptSess->pKexBuf->len);
        sha1_done(&hs, hash);
        /* copy session id */
        if (ptSess->aucSessionId[0]==0)
        {
            memcpy(ptSess->aucSessionId,hash,SHA1_HASH_SIZE);
        }
        memcpy(ptSess->aucHash,hash,SHA1_HASH_SIZE);

        ptSess->pKexBuf->len = ptSess->pKexBuf->pos = 0;
        /* first time around, we set the session_id to H */
        //if (ses[sshsty->sty_id].session_id == NULL)
        //{
        /* create the session_id, this never needs freeing */
        //    ses[sshsty->sty_id].session_id = (BYTE*)malloc(SHA1_HASH_SIZE);
        //    memcpy(ses[sshsty->sty_id].session_id, ses[sshsty->sty_id].hash, SHA1_HASH_SIZE);
        //}
        /* we can start creating the kexdh_reply packet */
        {
            writepayload = ptSess->pKexBuf;
            buf_putbyte(writepayload, SSH_MSG_KEXDH_REPLY);
            /* put K_S */
            buf_put_pub_key(writepayload, key);
            /* put f */
            buf_putmpint(writepayload, &dh_f);
            mp_clear(&dh_f);
            /* calc the signature H */
            buf_put_sign(writepayload, key, hash, SHA1_HASH_SIZE);
            /*消除PCLINT告警，将
            XOS_RetUB(key);
		修改为:
	      if(XOS_RetUB(key))
            {
            	key =NULL;
            }

		*/
            if(XOS_RetUB((BYTE *)key))
            {
            	key =NULL;
            }

            WORD32 dwSndPayloadLen = writepayload->len;
            FillPkgHeadLen(dwSndPayloadLen, ptSess->aucSndBuf);

            memcpy(ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE),&writepayload->data[0],writepayload->len);

            SSHSendPkg(ptSess);

            /* send SSH_MSG_NEWKEYS */
            BYTE *pStr;
			WORD32 dwPayloadLen1 = 1;
            pStr = ssh_byte_to_buffer(SSH_MSG_NEWKEYS,ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
			FillPkgHeadLen(dwPayloadLen1,ptSess->aucSndBuf);
            SSHSendPkg(ptSess);
        }
        ptSess->pKexBuf->len = ptSess->pKexBuf->pos = 0;
        return SSH_OK;
    }
}
static void hashkeys(TSSHSession *ptSess,BYTE *out, int outlen, const hash_state * hs, const BYTE X)
{

    hash_state hs2;
    BYTE k2[SHA1_HASH_SIZE]; /* used to extending */

    memcpy(&hs2, hs, sizeof(hash_state));
    sha1_process(&hs2, &X, 1);
    sha1_process(&hs2, ptSess->aucSessionId, SHA1_HASH_SIZE);
    sha1_done(&hs2, out);
    if (SHA1_HASH_SIZE < outlen)
    {
        /* need to extend */
        memcpy(&hs2, hs, sizeof(hash_state));
        sha1_process(&hs2, out, SHA1_HASH_SIZE);
        sha1_done(&hs2, k2);
        memcpy(&out[SHA1_HASH_SIZE], k2, outlen - SHA1_HASH_SIZE);
    }
}
/* 收到 SSH_MSG_NEWKEY的处理函数 */
void HandleMsgNewKeys(TSSHSession *ptSess,TSSHPack *ptPack)
{
    BYTE IV[20];
    BYTE key[24];
    hash_state hs;
    unsigned int keysize;

    /* the dh_K and hash are the start of all hashes, we make use of that */
    sha1_init(&hs);
    sha1_process_mp(&hs, &ptSess->dh_K);
    mp_clear(&ptSess->dh_K);
    sha1_process(&hs, ptSess->aucHash, SHA1_HASH_SIZE);
    //m_burn(ssh_context->hash, SHA1_HASH_SIZE);

    ptSess->tNewKey.recv_algo_crypt = &dropbear_3des;
    ptSess->tNewKey.trans_algo_crypt = &dropbear_3des;
    ptSess->tNewKey.recv_algo_mac = &dropbear_sha1;
    ptSess->tNewKey.trans_algo_mac = &dropbear_sha1;

    /* client->server IV */
    hashkeys(ptSess,IV, SHA1_HASH_SIZE, &hs, 'A');

    /* client->server encryption key */
    keysize = dropbear_3des.keysize;
    hashkeys(ptSess,key, keysize, &hs, 'C');
    if (cbc_start(
                0,
                IV, key, keysize, 0,
                &ptSess->tNewKey.recv_symmetric_struct) != SSH_OK)
    {
        printf("crypto error");
    }

    /* server->client IV */
    hashkeys(ptSess,IV, SHA1_HASH_SIZE, &hs, 'B');

    /* server->client encryption key */
    keysize = dropbear_3des.keysize;
    hashkeys(ptSess,key, keysize, &hs, 'D');
    if (cbc_start(
                0,
                IV, key, keysize, 0,
                &ptSess->tNewKey.trans_symmetric_struct) != SSH_OK)
    {
        printf("crypto error");
    }
    /* MAC key client->server */
    keysize = dropbear_sha1.keysize;
    hashkeys(ptSess,ptSess->tNewKey.recvmackey, keysize, &hs, 'E');

    /* MAC key server->client */
    keysize = dropbear_sha1.keysize;
    hashkeys(ptSess,ptSess->tNewKey.transmackey, keysize, &hs, 'F');

    /* Switch over to the new keys */

    /* 清空发送接受缓存 */
    memset(ptSess->aucRcvBuf,0,SSH_MAX_PACKAGE_LENGTH);
    ptSess->pRcvData = ptSess->aucRcvBuf;
    memset(ptSess->aucRcvDecryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
    ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
    memset(ptSess->aucSndBuf,0,SSH_MAX_PACKAGE_LENGTH);
    ptSess->pSndData = ptSess->aucSndBuf;
    memset(ptSess->aucSndEncryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
    ptSess->pSndEncryptData = ptSess->aucSndEncryptBuf;
}



static BOOL KexInit(TSSHSession* ptSess, CHAR * message_body, const DWORD message_length)
{
    Tssh_namelist * pssh_string;
    /* byte        random[16];                              */
    /* name_list   kex_algorithms;                          */
    /* name_list   server_host_key_algorithms;              */
    /* name_list   encryption_algorithms_client_to_server;  */
    /* name_list   encryption_algorithms_server_to_client;  */
    /* name_list   mac_algorithms_client_to_server;         */
    /* name_list   mac_algorithms_server_to_client;         */
    /* name_list   compression_algorithms_client_to_server; */
    /* name_list   compression_algorithms_server_to_client; */
    /* name_list   languages_client_to_server;              */
    /* name_list   languages_server_to_client;              */
    /* bool        first_kex_package_follows;               */
    /* uint32      0                                        */
    do
    {
        /* byte        random[16];                              */
        message_body += 16;
        /* name_list   kex_algorithms;                          */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "diffie-hellman-group1-sha1"))
        {
            break;
        }
        /* name_list   server_host_key_algorithms;              */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "ssh-dss"))
        {
            break;
        }
        /* name_list   encryption_algorithms_client_to_server;  */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "3des-cbc"))
        {
            break;
        }
        /* name_list   encryption_algorithms_server_to_client;  */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "3des-cbc"))
        {
            break;
        }
        /* name_list   mac_algorithms_client_to_server;         */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "hmac-sha1"))
        {
            break;
        }
        /* name_list   mac_algorithms_server_to_client;         */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "hmac-sha1"))
        {
            break;
        }
        /* name_list   compression_algorithms_client_to_server; */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "none"))
        {
            break;
        }
        /* name_list   compression_algorithms_server_to_client; */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        if (!ssh_namelist_contained(pssh_string, "none"))
        {
            break;
        }
        /* name_list   languages_client_to_server;              */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        /* name_list   languages_server_to_client;              */
        message_body = ssh_string_from_buffer(&pssh_string, message_body);
        /* bool        first_kex_package_follows;               */
        /* uint32      0                                        */
        return TRUE;
    }while(FALSE);
    return FALSE;
}
static CHAR * SetKexInit(CHAR * buffer1)
{
    int i = 0;
    int j = 0;
    *buffer1 = SSH_MSG_KEXINIT;
    buffer1++;

    for (;i<16;i++)
    {
        buffer1[i] = 'A'+i;
    }
    buffer1 += 16;

    buffer1 = ssh_string_to_buffer("diffie-hellman-group1-sha1", buffer1);
    buffer1 = ssh_string_to_buffer("ssh-dss", buffer1);
    buffer1 = ssh_string_to_buffer("3des-cbc", buffer1);
    buffer1 = ssh_string_to_buffer("3des-cbc", buffer1);
    buffer1 = ssh_string_to_buffer("hmac-sha1", buffer1);
    buffer1 = ssh_string_to_buffer("hmac-sha1", buffer1);
    buffer1 = ssh_string_to_buffer("none", buffer1);
    buffer1 = ssh_string_to_buffer("none", buffer1);
    buffer1 = ssh_string_to_buffer("", buffer1);
    buffer1 = ssh_string_to_buffer("", buffer1);

    for (;j<5;j++)
    {
        buffer1[j] = 0;
    }
    buffer1 += 5;

    return buffer1;
}

#endif

