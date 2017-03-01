#ifndef CRYPT_H
#define CRYPT_H

/* º”√‹À„∑® */
#include <iostream>
#include <string>

#include <openssl/evp.h>
#include <iomanip>

#include "pub.h"

using namespace std;

/* TEA  */
string tea_encrypt(const string &plain);
string tea_decrypt(const string &crypt);


/* SHA-1 */
void sha1_init(struct sha1_ctxt * ctxt);
void sha1_loop(struct sha1_ctxt * ctxt, const uint8 *input0, size_t len);
void sha1_result(struct sha1_ctxt * ctxt, uint8 *digest0);

/* compatibilty with other SHA1 source codes */
struct sha1_ctxt
{
    union
    {
        uint8       b8[20];
        uint32      b32[5];
    }           h;
    union
    {
        uint8       b8[8];
        uint64      b64[1];
    }           c;
    union
    {
        uint8       b8[64];
        uint32      b32[16];
    }           m;
    uint8       count;
};

typedef struct sha1_ctxt SHA1_CTX;

#define SHA1Init(x)         sha1_init((x))
#define SHA1Update(x, y, z) sha1_loop((x), (y), (z))
#define SHA1Final(x, y)     sha1_result((y), (x))

#define SHA1_RESULTLEN  (160/8)

string Sha1Digest(const string& name, const string& pass);
string GetSessionByName(const string& name);

#endif