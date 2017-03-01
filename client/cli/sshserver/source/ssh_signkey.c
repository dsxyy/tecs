#include "ssh_signkey.h"
static void buf_put_dss_pub_key(buffer* buf1, dss_key *key);
static void buf_put_dss_sign(buffer* buf1, dss_key *key, const BYTE* data,	unsigned int len);

void buf_put_pub_key(buffer* buf1, sign_key *key)
{
	int curpos = 0;
	WORD32 length = 0;
	/*
    pubkeys = buf_new(1000);
    buf_put_dss_pub_key(pubkeys, key->dsskey);
	*/
	curpos = buf1->pos;
	/*先预留前4个字节作为长度*/
	buf1->pos += 4;
	buf1->len = buf1->pos;
	buf_put_dss_pub_key(buf1, key->dsskey);
	/*
    pubkeys->pos = 0;
    buf_putstring(buf, &pubkeys->data[pubkeys->pos],pubkeys->len);
	*/
	length = buf1->len - curpos -4;
	length = htonl(length);
	memcpy(&buf1->data[curpos],&length,4);
}
void buf_put_sign(buffer* buf1, sign_key *key, const BYTE *data, unsigned int len)
{
	int curpos = 0;
	WORD32 length = 0;
	curpos = buf1->pos;
	/*先预留前4个字节作为长度*/
	buf1->pos += 4;
	buf1->len = buf1->pos;
	/*
    buffer *sigblob;
    sigblob = buf_new(1000);
    buf_put_dss_sign(sigblob, key->dsskey, data, len);
    if (sigblob->len == 0)
    {
        TRACE(("non-matching signing type"));
    }
	*/
	buf_put_dss_sign(buf1, key->dsskey, data, len);
    /*
    buf_putstring(buf, &sigblob->data[sigblob->pos],sigblob->len);
    buf_free(sigblob);
	*/
    length = buf1->len - curpos -4;
	length = htonl(length);
	memcpy(&buf1->data[curpos],&length,4);
}
/* put the dss key into the buffer in the required format:
*
* string	"ssh-dss"
* mpint	p
* mpint	q
* mpint	g
* mpint	y
*/
static void buf_put_dss_pub_key(buffer* buf1, dss_key *key)
{
    buf_putstring(buf1, (BYTE*)"ssh-dss", 7);
    buf_putmpint(buf1, key->p);
    buf_putmpint(buf1, key->q);
    buf_putmpint(buf1, key->g);
    buf_putmpint(buf1, key->y);
}
extern void genrandom(BYTE* buf1, unsigned int len);
/* sign the data presented in len with key, writing the signature contents
 * to the buffer
 *
 * when DSS_PROTOK is #defined:
 * The alternate k generation method is based on the method used in PuTTY. 
 * In particular to avoid being vulnerable to attacks using flaws in random
 * generation of k, we use the following:
 *
 * proto_k = SHA512 ( SHA512(x) || SHA160(message) )
 * k = proto_k mod q
 *
 * Now we aren't relying on the random number generation to protect the private
 * key x, which is a long term secret */
static void buf_put_dss_sign(buffer* buf1, dss_key *key, const BYTE* data,	unsigned int len)
{
    BYTE msghash[SHA1_HASH_SIZE];
    unsigned int writelen;
    unsigned int i;
    BYTE kbuf[SHA1_HASH_SIZE];
    mp_int dss_k, dss_m;
    mp_int dss_temp1, dss_temp2;
    mp_int dss_r, dss_s;
    hash_state hs;

    /* hash the data */
    sha1_init(&hs);
    sha1_process(&hs, data, len);
    sha1_done(&hs, msghash);

    mp_init(&dss_k);

    do
    {
        genrandom(kbuf, SHA1_HASH_SIZE);
        if (mp_read_unsigned_bin(&dss_k, kbuf, SHA1_HASH_SIZE) != MP_OKAY)
        {
            printf("dss error");
        }
    }
    while (mp_cmp(&dss_k, key->q) == MP_GT || mp_cmp_d(&dss_k, 0) != MP_GT);
    m_burn(kbuf, SHA1_HASH_SIZE);

    /* now generate the actual signature */
    mp_init(&dss_temp1);
    mp_init(&dss_temp2);
    mp_init(&dss_r);
    mp_init(&dss_s);
    mp_init(&dss_m);

    if (mp_read_unsigned_bin(&dss_m, msghash, SHA1_HASH_SIZE) != MP_OKAY)
    {
        printf("mem alloc error");
    }

    /* g^k mod p */
    if (mp_exptmod(key->g, &dss_k, key->p, &dss_temp1) !=  MP_OKAY)
    {
        printf("dss error");
    }

    /* r = (g^k mod p) mod q */
    if (mp_mod(&dss_temp1, key->q, &dss_r) != MP_OKAY)
    {
        printf("dss error");
    }

    /* x*r mod q */
    if (mp_mulmod(&dss_r, key->x, key->q, &dss_temp1) != MP_OKAY)
    {
        printf("dss error");
    }
    /* (SHA1(M) + xr) mod q) */
    if (mp_addmod(&dss_m, &dss_temp1, key->q, &dss_temp2) != MP_OKAY)
    {
        printf("dss error");
    }
    mp_clear(&dss_m);

    /* (k^-1) mod q */
    if (mp_invmod(&dss_k, key->q, &dss_temp1) != MP_OKAY)
    {
        printf("dss error");
    }
    mp_clear(&dss_k);

    /* s = (k^-1(SHA1(M) + xr)) mod q */
    if (mp_mulmod(&dss_temp1, &dss_temp2, key->q, &dss_s) != MP_OKAY)
    {
        printf("dss error");
    }

    mp_clear(&dss_temp1);
    mp_clear(&dss_temp2);

    /* create the signature to return */
    buf_putstring(buf1, (BYTE*)"ssh-dss", 7);
    buf_putint(buf1, 2*SHA1_HASH_SIZE);

    writelen = mp_unsigned_bin_size(&dss_r);
    /* need to pad to 160 bits with leading zeros */
    for (i = 0; i < SHA1_HASH_SIZE - writelen; i++)
    {
        buf_putbyte(buf1, 0);
    }
    if (mp_to_unsigned_bin(&dss_r, &buf1->data[buf1->pos])!= MP_OKAY)
    {
        printf("dss error");
    }
    mp_clear(&dss_r);
    buf_incrwritepos(buf1, writelen);

    writelen = mp_unsigned_bin_size(&dss_s);

    /* need to pad to 160 bits with leading zeros */
    for (i = 0; i < SHA1_HASH_SIZE - writelen; i++)
    {
        buf_putbyte(buf1, 0);
    }
    if (mp_to_unsigned_bin(&dss_s, &buf1->data[buf1->pos])
            != MP_OKAY)
    {
        printf("dss error");
    }
    mp_clear(&dss_s);
    buf_incrwritepos(buf1, writelen);
}
