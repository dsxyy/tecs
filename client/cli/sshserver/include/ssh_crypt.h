#ifndef _SSH_CRYPT_H_
#define _SSH_CRYPT_H_

int cbc_start(int cipher, const unsigned char *IV, const unsigned char *key,
              int keylen, int num_rounds, symmetric_CBC *cbc);
int cbc_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_CBC *cbc);
int cbc_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_CBC *cbc);

#endif
