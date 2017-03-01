#include "ssh_session.h"

int cbc_start(int cipher, const BYTE *IV, const BYTE *key,
              int keylen, int num_rounds, symmetric_CBC *cbc)
{
    int x, err;
	
    /* setup cipher */
	
    if ((err = cipher_descriptor[cipher]->setup(key, keylen, num_rounds, &cbc->key)) != SSH_OK)
    {
        return err;
    }
	
    /* copy IV */
    cbc->blocklen = cipher_descriptor[cipher]->block_length;
    cbc->cipher   = cipher;
    for (x = 0; x < cbc->blocklen; x++)
    {
        cbc->IV[x] = IV[x];
    }
    return SSH_OK;
}

int cbc_encrypt(const BYTE *pt, BYTE *ct, symmetric_CBC *cbc)
{
    int x;
    BYTE tmp[MAXBLOCKSIZE];
	
    /* is blocklen valid? */
    if (cbc->blocklen < 0 || cbc->blocklen > (int)sizeof(cbc->IV))
    {
        return -1;
    }
	
    /* xor IV against plaintext */
    for (x = 0; x < cbc->blocklen; x++)
    {
        tmp[x] = pt[x] ^ cbc->IV[x];
    }
	
    /* encrypt */
    cipher_descriptor[cbc->cipher]->ecb_encrypt(tmp, ct, &cbc->key);
	
    /* store IV [ciphertext] for a future block */
    for (x = 0; x < cbc->blocklen; x++)
    {
        cbc->IV[x] = ct[x];
    }
    return SSH_OK;
}

int cbc_decrypt(const BYTE *ct, BYTE *pt, symmetric_CBC *cbc)
{
    int x;
    BYTE tmp[MAXBLOCKSIZE];
    BYTE tmp2[MAXBLOCKSIZE]={0};
	
    cipher_descriptor[cbc->cipher]->ecb_decrypt(ct, tmp, &cbc->key);
	
    /* is blocklen valid? */
    if (cbc->blocklen < 0 || cbc->blocklen > (int)sizeof(cbc->IV))
    {
        return -1;
    }
	
    /* xor IV against the plaintext of the previous step */
    for (x = 0; x < cbc->blocklen; x++)
    {
        /* copy CT in case ct == pt */
        tmp2[x] = ct[x];
		
        /* actually decrypt the byte */
        pt[x] = tmp[x] ^ cbc->IV[x];
    }
	
    /* replace IV with this current ciphertext */
    for (x = 0; x < cbc->blocklen; x++)
    {
        cbc->IV[x] = tmp2[x];
    }
    return SSH_OK;
}
