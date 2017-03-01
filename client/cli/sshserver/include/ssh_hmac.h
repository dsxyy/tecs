#ifndef _SSH_HMAC
#define _SSH_HMAC

typedef struct Hmac_state
{
	hash_state md;
	int hash;
	unsigned long hashsize; /* here for your reference */
	hash_state hashstate;
	unsigned char key[MAXBLOCKSIZE];
} hmac_state;

extern void zeromem(void *dst, size_t len);
extern int hmac_memory(int hash, const unsigned char *key, unsigned long keylen,
					   const unsigned char *data, unsigned long len, 
                unsigned char *dst, WORD32 *dstlen);
extern int t_hmac_init(hmac_state *hmac, int hash, const unsigned char *key, unsigned long keylen);
extern int hmac_process(hmac_state *hmac, const unsigned char *buf, unsigned long len);
extern int hmac_done(hmac_state *hmac, unsigned char *hashOut, WORD32 *outlen);

#endif

