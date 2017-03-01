#ifndef _SSH_SHA1
#define _SSH_SHA1

#define SHA1_HASH_SIZE 20

typedef struct
{
    unsigned int h[5];
    unsigned char block[64];
    int blkused;
    unsigned int lenhi, lenlo;
}SHA_State ;

typedef union Hash_state
{
    SHA_State sha1;
} hash_state;

void SHA_Init(SHA_State * s);
void SHA_Bytes(SHA_State * s, void *p, int len);
void SHA_Final(SHA_State * s, unsigned char *output);
void SHA_Simple(void *p, int len, unsigned char *output);
void sha1_init(hash_state * md);
void sha1_process(hash_state * md, const unsigned char *buf, unsigned long len);
void sha1_done(hash_state * md, unsigned char *hash);
void sha1_process_mp(hash_state *hs, mp_int *mp);
#endif
