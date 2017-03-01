/****************************************************************************/
/*    Tiny Encrypt Algorithm (TEA)                                          */
/****************************************************************************/

#include "crypt.h"
#include "user_pool.h"
#include "sessionmanager.h"

unsigned int k[] = {0x33421001, 0x55643711, 0x18765324, 0x88764532};

void encrypt(unsigned int *v,unsigned int *k){
    unsigned int y=v[0], z=v[1], sum=0, i;        /* setup */
    unsigned int delta=0x9e3779b9;                /* a key schedule constant */
    unsigned int a=k[0], b=k[1], c=k[2], d=k[3];  /* cache key */
    
    for(i=0; i<32; i++){                           /* basic cycle start */
        sum += delta;
        y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);        /* end cycle */
    }
    
    v[0] = y;
    v[1] = z;
}

void decrypt(unsigned int *v,unsigned int *k){
    unsigned int y=v[0], z=v[1], sum=0xC6EF3720, i; /* setup */
    unsigned int delta=0x9e3779b9;                  /* a key schedule constant */
    unsigned int a=k[0],b=k[1],c=k[2],d=k[3];       /* cache key */
    
    for(i=0; i<32; i++){                             /* basic cycle start */
        z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
        y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        sum -= delta;                                /* end cycle */
    }
    
    v[0] = y;
    v[1] = z;
}

char intToHexChar(int x) {
    static const char HEX[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'A', 'B',
        'C', 'D', 'E', 'F'
    };
    return HEX[x];
}

int hexCharToInt(char hex) {
    hex = toupper(hex);
    if (isdigit(hex))
        return (hex - '0');
    if (isalpha(hex))
        return (hex - 'A' + 10);
    return 0;
}

string bytesToHexString(unsigned char *in, int size){
    string str;
    for (int i = 0; i < size; i++) {
        int t = in[i];
        int a = t / 16;
        int b = t % 16;
        str.append(1, intToHexChar(a));
        str.append(1, intToHexChar(b));
    }
    return str;
}

int hexStringToBytes(const string str, unsigned char *out){
    int len = str.length()/2;

    if(len != 80) return 0;

    for (int i=0; i<len; i++) {
        int a = hexCharToInt(str[2*i]);
        int b = hexCharToInt(str[2*i+1]);
        out[i] = a * 16 + b;
    }
    
    return len;
}

string tea_encrypt(const string &plain)
{
    char buf[81]={0};

    strncpy(buf, plain.c_str(), 80);

    encrypt((unsigned int*)buf, k);
    encrypt((unsigned int*)(buf+8), k);
    encrypt((unsigned int*)(buf+16), k);
    encrypt((unsigned int*)(buf+24), k);
    encrypt((unsigned int*)(buf+32), k);
    encrypt((unsigned int*)(buf+40), k);
    encrypt((unsigned int*)(buf+48), k); 
    encrypt((unsigned int*)(buf+56), k);
    encrypt((unsigned int*)(buf+64), k);
    encrypt((unsigned int*)(buf+72), k); 
    
    return bytesToHexString((unsigned char *)buf, 80);
}

string tea_decrypt(const string &crypt)
{
    char buf[81]={0};

    int ret = hexStringToBytes(crypt, (unsigned char *)buf);

    if(ret == 0) return "";

    decrypt((unsigned int*)buf, k);
    decrypt((unsigned int*)(buf+8), k);
    decrypt((unsigned int*)(buf+16), k);
    decrypt((unsigned int*)(buf+24), k);
    decrypt((unsigned int*)(buf+32), k);
    decrypt((unsigned int*)(buf+40), k);
    decrypt((unsigned int*)(buf+48), k); 
    decrypt((unsigned int*)(buf+56), k);
    decrypt((unsigned int*)(buf+64), k);
    decrypt((unsigned int*)(buf+72), k);    
    
    return buf;
}


/* ************************************************************************** */
/* FIPS pub 180-1: Secure Hash Algorithm (SHA-1)                              */   
/* based on: http://csrc.nist.gov/fips/fip180-1.txt                           */ 
/* implemented by Jun-ichiro itojun Itoh <itojun@itojun.org>                  */
/* ************************************************************************** */ 

/* constant table */
static uint32 _K[] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};

#define K(t)    _K[(t) / 20]

#define F0(b, c, d) (((b) & (c)) | ((~(b)) & (d)))
#define F1(b, c, d) (((b) ^ (c)) ^ (d))
#define F2(b, c, d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))
#define F3(b, c, d) (((b) ^ (c)) ^ (d))

#define S(n, x)     (((x) << (n)) | ((x) >> (32 - (n))))

#define H(n)    (ctxt->h.b32[(n)])
#define COUNT   (ctxt->count)
#define BCOUNT  (ctxt->c.b64[0] / 8)
#define W(n)    (ctxt->m.b32[(n)])

static void sha1_step(struct sha1_ctxt *);

#define PUTBYTE(x) \
do { \
    ctxt->m.b8[(COUNT % 64)] = (x);     \
    COUNT++;                \
    COUNT %= 64;                \
    ctxt->c.b64[0] += 8;            \
    if (COUNT % 64 == 0)            \
        sha1_step(ctxt);        \
} while (0)

#define PUTPAD(x) \
do { \
    ctxt->m.b8[(COUNT % 64)] = (x);     \
    COUNT++;                \
    COUNT %= 64;                \
    if (COUNT % 64 == 0)            \
        sha1_step(ctxt);        \
} while (0)

static void
sha1_step(struct sha1_ctxt * ctxt)
{
    uint32      a,
                b,
                c,
                d,
                e;
    size_t      t,
                s;
    uint32      tmp;

#ifndef WORDS_BIGENDIAN
    struct sha1_ctxt tctxt;

    memmove(&tctxt.m.b8[0], &ctxt->m.b8[0], 64);
    ctxt->m.b8[0] = tctxt.m.b8[3];
    ctxt->m.b8[1] = tctxt.m.b8[2];
    ctxt->m.b8[2] = tctxt.m.b8[1];
    ctxt->m.b8[3] = tctxt.m.b8[0];
    ctxt->m.b8[4] = tctxt.m.b8[7];
    ctxt->m.b8[5] = tctxt.m.b8[6];
    ctxt->m.b8[6] = tctxt.m.b8[5];
    ctxt->m.b8[7] = tctxt.m.b8[4];
    ctxt->m.b8[8] = tctxt.m.b8[11];
    ctxt->m.b8[9] = tctxt.m.b8[10];
    ctxt->m.b8[10] = tctxt.m.b8[9];
    ctxt->m.b8[11] = tctxt.m.b8[8];
    ctxt->m.b8[12] = tctxt.m.b8[15];
    ctxt->m.b8[13] = tctxt.m.b8[14];
    ctxt->m.b8[14] = tctxt.m.b8[13];
    ctxt->m.b8[15] = tctxt.m.b8[12];
    ctxt->m.b8[16] = tctxt.m.b8[19];
    ctxt->m.b8[17] = tctxt.m.b8[18];
    ctxt->m.b8[18] = tctxt.m.b8[17];
    ctxt->m.b8[19] = tctxt.m.b8[16];
    ctxt->m.b8[20] = tctxt.m.b8[23];
    ctxt->m.b8[21] = tctxt.m.b8[22];
    ctxt->m.b8[22] = tctxt.m.b8[21];
    ctxt->m.b8[23] = tctxt.m.b8[20];
    ctxt->m.b8[24] = tctxt.m.b8[27];
    ctxt->m.b8[25] = tctxt.m.b8[26];
    ctxt->m.b8[26] = tctxt.m.b8[25];
    ctxt->m.b8[27] = tctxt.m.b8[24];
    ctxt->m.b8[28] = tctxt.m.b8[31];
    ctxt->m.b8[29] = tctxt.m.b8[30];
    ctxt->m.b8[30] = tctxt.m.b8[29];
    ctxt->m.b8[31] = tctxt.m.b8[28];
    ctxt->m.b8[32] = tctxt.m.b8[35];
    ctxt->m.b8[33] = tctxt.m.b8[34];
    ctxt->m.b8[34] = tctxt.m.b8[33];
    ctxt->m.b8[35] = tctxt.m.b8[32];
    ctxt->m.b8[36] = tctxt.m.b8[39];
    ctxt->m.b8[37] = tctxt.m.b8[38];
    ctxt->m.b8[38] = tctxt.m.b8[37];
    ctxt->m.b8[39] = tctxt.m.b8[36];
    ctxt->m.b8[40] = tctxt.m.b8[43];
    ctxt->m.b8[41] = tctxt.m.b8[42];
    ctxt->m.b8[42] = tctxt.m.b8[41];
    ctxt->m.b8[43] = tctxt.m.b8[40];
    ctxt->m.b8[44] = tctxt.m.b8[47];
    ctxt->m.b8[45] = tctxt.m.b8[46];
    ctxt->m.b8[46] = tctxt.m.b8[45];
    ctxt->m.b8[47] = tctxt.m.b8[44];
    ctxt->m.b8[48] = tctxt.m.b8[51];
    ctxt->m.b8[49] = tctxt.m.b8[50];
    ctxt->m.b8[50] = tctxt.m.b8[49];
    ctxt->m.b8[51] = tctxt.m.b8[48];
    ctxt->m.b8[52] = tctxt.m.b8[55];
    ctxt->m.b8[53] = tctxt.m.b8[54];
    ctxt->m.b8[54] = tctxt.m.b8[53];
    ctxt->m.b8[55] = tctxt.m.b8[52];
    ctxt->m.b8[56] = tctxt.m.b8[59];
    ctxt->m.b8[57] = tctxt.m.b8[58];
    ctxt->m.b8[58] = tctxt.m.b8[57];
    ctxt->m.b8[59] = tctxt.m.b8[56];
    ctxt->m.b8[60] = tctxt.m.b8[63];
    ctxt->m.b8[61] = tctxt.m.b8[62];
    ctxt->m.b8[62] = tctxt.m.b8[61];
    ctxt->m.b8[63] = tctxt.m.b8[60];
#endif

    a = H(0);
    b = H(1);
    c = H(2);
    d = H(3);
    e = H(4);

    for (t = 0; t < 20; t++)
    {
        s = t & 0x0f;
        if (t >= 16)
            W(s) = S(1, W((s + 13) & 0x0f) ^ W((s + 8) & 0x0f) ^ W((s + 2) & 0x0f) ^ W(s));
        tmp = S(5, a) + F0(b, c, d) + e + W(s) + K(t);
        e = d;
        d = c;
        c = S(30, b);
        b = a;
        a = tmp;
    }
    for (t = 20; t < 40; t++)
    {
        s = t & 0x0f;
        W(s) = S(1, W((s + 13) & 0x0f) ^ W((s + 8) & 0x0f) ^ W((s + 2) & 0x0f) ^ W(s));
        tmp = S(5, a) + F1(b, c, d) + e + W(s) + K(t);
        e = d;
        d = c;
        c = S(30, b);
        b = a;
        a = tmp;
    }
    for (t = 40; t < 60; t++)
    {
        s = t & 0x0f;
        W(s) = S(1, W((s + 13) & 0x0f) ^ W((s + 8) & 0x0f) ^ W((s + 2) & 0x0f) ^ W(s));
        tmp = S(5, a) + F2(b, c, d) + e + W(s) + K(t);
        e = d;
        d = c;
        c = S(30, b);
        b = a;
        a = tmp;
    }
    for (t = 60; t < 80; t++)
    {
        s = t & 0x0f;
        W(s) = S(1, W((s + 13) & 0x0f) ^ W((s + 8) & 0x0f) ^ W((s + 2) & 0x0f) ^ W(s));
        tmp = S(5, a) + F3(b, c, d) + e + W(s) + K(t);
        e = d;
        d = c;
        c = S(30, b);
        b = a;
        a = tmp;
    }

    H(0) = H(0) + a;
    H(1) = H(1) + b;
    H(2) = H(2) + c;
    H(3) = H(3) + d;
    H(4) = H(4) + e;

    memset(&ctxt->m.b8[0], 0, 64);
}

/*------------------------------------------------------------*/

void
sha1_init(struct sha1_ctxt * ctxt)
{
    memset(ctxt, 0, sizeof(struct sha1_ctxt));
    H(0) = 0x67452301;
    H(1) = 0xefcdab89;
    H(2) = 0x98badcfe;
    H(3) = 0x10325476;
    H(4) = 0xc3d2e1f0;
}

void
sha1_pad(struct sha1_ctxt * ctxt)
{
    size_t      padlen;         /* pad length in bytes */
    size_t      padstart;

    PUTPAD(0x80);

    padstart = COUNT % 64;
    padlen = 64 - padstart;
    if (padlen < 8)
    {
        memset(&ctxt->m.b8[padstart], 0, padlen);
        COUNT += padlen;
        COUNT %= 64;
        sha1_step(ctxt);
        padstart = COUNT % 64;  /* should be 0 */
        padlen = 64 - padstart; /* should be 64 */
    }
    memset(&ctxt->m.b8[padstart], 0, padlen - 8);
    COUNT += (padlen - 8);
    COUNT %= 64;
#ifdef WORDS_BIGENDIAN
    PUTPAD(ctxt->c.b8[0]);
    PUTPAD(ctxt->c.b8[1]);
    PUTPAD(ctxt->c.b8[2]);
    PUTPAD(ctxt->c.b8[3]);
    PUTPAD(ctxt->c.b8[4]);
    PUTPAD(ctxt->c.b8[5]);
    PUTPAD(ctxt->c.b8[6]);
    PUTPAD(ctxt->c.b8[7]);
#else
    PUTPAD(ctxt->c.b8[7]);
    PUTPAD(ctxt->c.b8[6]);
    PUTPAD(ctxt->c.b8[5]);
    PUTPAD(ctxt->c.b8[4]);
    PUTPAD(ctxt->c.b8[3]);
    PUTPAD(ctxt->c.b8[2]);
    PUTPAD(ctxt->c.b8[1]);
    PUTPAD(ctxt->c.b8[0]);
#endif
}

void
sha1_loop(struct sha1_ctxt * ctxt, const uint8 *input0, size_t len)
{
    const uint8 *input;
    size_t      gaplen;
    size_t      gapstart;
    size_t      off;
    size_t      copysiz;

    input = (const uint8 *) input0;
    off = 0;

    while (off < len)
    {
        gapstart = COUNT % 64;
        gaplen = 64 - gapstart;

        copysiz = (gaplen < len - off) ? gaplen : len - off;
        memmove(&ctxt->m.b8[gapstart], &input[off], copysiz);
        COUNT += copysiz;
        COUNT %= 64;
        ctxt->c.b64[0] += copysiz * 8;
        if (COUNT % 64 == 0)
            sha1_step(ctxt);
        off += copysiz;
    }
}

void
sha1_result(struct sha1_ctxt * ctxt, uint8 *digest0)
{
    uint8      *digest;

    digest = (uint8 *) digest0;
    sha1_pad(ctxt);
#ifdef WORDS_BIGENDIAN
    memmove(digest, &ctxt->h.b8[0], 20);
#else
    digest[0] = ctxt->h.b8[3];
    digest[1] = ctxt->h.b8[2];
    digest[2] = ctxt->h.b8[1];
    digest[3] = ctxt->h.b8[0];
    digest[4] = ctxt->h.b8[7];
    digest[5] = ctxt->h.b8[6];
    digest[6] = ctxt->h.b8[5];
    digest[7] = ctxt->h.b8[4];
    digest[8] = ctxt->h.b8[11];
    digest[9] = ctxt->h.b8[10];
    digest[10] = ctxt->h.b8[9];
    digest[11] = ctxt->h.b8[8];
    digest[12] = ctxt->h.b8[15];
    digest[13] = ctxt->h.b8[14];
    digest[14] = ctxt->h.b8[13];
    digest[15] = ctxt->h.b8[12];
    digest[16] = ctxt->h.b8[19];
    digest[17] = ctxt->h.b8[18];
    digest[18] = ctxt->h.b8[17];
    digest[19] = ctxt->h.b8[16];
#endif
}


string Sha1Digest(const string& name, const string& pass)
{
    ostringstream oss;
    SHA1_CTX      ctx;
    uint8         md_value[SHA1_RESULTLEN];
    string        rname = name, rpass = pass;
    string        sha1_raw;

    // reverse user and pass    
    reverse(rname.begin(), rname.end());
    reverse(rpass.begin(), rpass.end());

    sha1_raw.append(name);
    sha1_raw.append(rpass);
    sha1_raw.append(rname);
    sha1_raw.append(pass);

    SHA1Init(&ctx);
    SHA1Update(&ctx, (const uint8 *)sha1_raw.c_str(), sha1_raw.length());
    SHA1Final(md_value, &ctx);
    
    for(unsigned int i = 0; i<SHA1_RESULTLEN; i++)
    {
        oss << setfill('0') << setw(2) << hex << nouppercase
            << (unsigned short) md_value[i];
    }

    return oss.str();
}

string GetSessionByName(const string& name)
{
    string session = "";

    zte_tecs::User* u = zte_tecs::UserPool::GetInstance()->Get(name, false);

    if(u != NULL)
    {
       session = zte_tecs::SessionManager::GetInstance()->NewSession("", name + ":" + u->get_password());
    }

    return session;
}

/*************************************************************************** */
