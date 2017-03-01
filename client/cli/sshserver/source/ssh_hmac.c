#include "ssh_session.h"

#define HMAC_BLOCKSIZE hash_descriptor[hash]->blocksize

int hash_memory(int hash, const BYTE *data, unsigned long len, BYTE *dst, unsigned long *outlen)
{
    hash_state md;
	
    if (*outlen < hash_descriptor[hash]->hashsize) {
		return -1;
    }
    *outlen = hash_descriptor[hash]->hashsize;
	
    hash_descriptor[hash]->init(&md);
    hash_descriptor[hash]->process(&md, data, len);
    hash_descriptor[hash]->done(&md, dst);
    return SSH_OK;
}


int t_hmac_init(hmac_state *hmac, int hash, const BYTE *key, unsigned long keylen)
{
    BYTE buf1[MAXBLOCKSIZE];
    unsigned long hashsize;
    unsigned long i, z;
    int err;
	
    /* valid key length? */
    if (keylen == 0) {
        return -1;
    }
	
    hmac->hash = hash;
	
    /* (1) make sure we have a large enough key*/
    hmac->hashsize = hashsize = hash_descriptor[hash]->hashsize;
    if(keylen > HMAC_BLOCKSIZE) {
        z = (unsigned long)sizeof(hmac->key);
        if ((err = hash_memory(hash, key, keylen, hmac->key, &z)) != SSH_OK) {
			return err;
        }
        if(hashsize < HMAC_BLOCKSIZE) {
            zeromem((hmac->key) + hashsize, (size_t)(HMAC_BLOCKSIZE - hashsize));
        }
        keylen = hashsize;
    } else {
        memcpy(hmac->key, key, (size_t)keylen);
        if(keylen < HMAC_BLOCKSIZE) {
            zeromem((hmac->key) + keylen, (size_t)(HMAC_BLOCKSIZE - keylen));
        }
    }
	
    /* Create the initial vector for step (3)*/
    for(i=0; i < HMAC_BLOCKSIZE;   i++) {
		buf1[i] = hmac->key[i] ^ 0x36;
    }
	
    /* Pre-pend that to the hash data*/
    hash_descriptor[hash]->init(&hmac->md);
    hash_descriptor[hash]->process(&hmac->md, buf1, HMAC_BLOCKSIZE);
	
    return SSH_OK;
}
int hmac_process(hmac_state *hmac, const BYTE *buf1, unsigned long len)
{
    hash_descriptor[hmac->hash]->process(&hmac->md, buf1, len);
    return SSH_OK;
}

int hmac_done(hmac_state *hmac, BYTE *hashOut, WORD32 *outlen)
{
    BYTE buf1[MAXBLOCKSIZE];
    BYTE isha[MAXBLOCKSIZE];
    WORD32 hashsize, i;
    int hash;
	
    hash = hmac->hash;
	
    /* ensure the output size is valid */
    hashsize = hash_descriptor[hmac->hash]->hashsize;
    if (*outlen < hashsize) {
		return -1;
    }
    *outlen = hashsize;
	
    /* Get the hash of the first HMAC vector plus the data*/
    hash_descriptor[hmac->hash]->done(&hmac->md, isha);
	
    /* Create the second HMAC vector vector for step (3)*/
    for(i=0; i < HMAC_BLOCKSIZE; i++) {
        buf1[i] = hmac->key[i] ^ 0x5C;
    }
	
    /* Now calculate the "outer" hash for step (5), (6), and (7)*/
    hash_descriptor[hmac->hash]->init(&hmac->md);
    hash_descriptor[hmac->hash]->process(&hmac->md, buf1, HMAC_BLOCKSIZE);
    hash_descriptor[hmac->hash]->process(&hmac->md, isha, hashsize);
    hash_descriptor[hmac->hash]->done(&hmac->md, hashOut);
	
	
    zeromem(isha, sizeof(buf1));
    zeromem(buf1,  sizeof(isha));
    zeromem(hmac->key, sizeof(hmac->key));
	
    return SSH_OK;
}

int hmac_memory(int hash, const BYTE *key, unsigned long keylen,
                const BYTE *data, unsigned long len, 
                BYTE *dst, WORD32 *dstlen)
{
    hmac_state hmac;
    int err;
    
    if ((err = t_hmac_init(&hmac, hash, key, keylen)) != SSH_OK) {
        return err;
    }
	
    if ((err = hmac_process(&hmac, data, len)) != SSH_OK) {
		return err;
    }
	
    if ((err = hmac_done(&hmac, dst, dstlen)) != SSH_OK) {
		return err;
    }
    return SSH_OK;
}

