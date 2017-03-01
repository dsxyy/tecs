
#include "ssh_session.h"
/* ----------------------------------------------------------------------
 * Core SHA algorithm: processes 16-word blocks into a message digest.
 */

#define rol(x,y) ( ((x) << (y)) | (((WORD32)x) >> (32-y)) )

void SHA_Core_Init(WORD32 h[5])
{
    h[0] = 0x67452301;
    h[1] = 0xefcdab89;
    h[2] = 0x98badcfe;
    h[3] = 0x10325476;
    h[4] = 0xc3d2e1f0;
}

void SHATransform(WORD32 * digest, WORD32 * block)
{
    WORD32 w[80];
    WORD32 a, b, c, d, e;
    int t;
    for (t = 0; t < 16; t++)
	w[t] = block[t];
    for (t = 16; t < 80; t++)
	{
		WORD32 tmp = w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16];
		w[t] = rol(tmp, 1);
    }
    a = digest[0];
    b = digest[1];
    c = digest[2];
    d = digest[3];
    e = digest[4];

    for (t = 0; t < 20; t++) 
	{
		WORD32 tmp = rol(a, 5) + ((b & c) | (d & ~b)) + e + w[t] + 0x5a827999;
		e = d;
		d = c;
		c = rol(b, 30);
		b = a;
		a = tmp;
    }
    for (t = 20; t < 40; t++) 
	{
		WORD32 tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0x6ed9eba1;
		e = d;
		d = c;
		c = rol(b, 30);
		b = a;
		a = tmp;
    }
    for (t = 40; t < 60; t++) 
	{
		WORD32 tmp = rol(a,5) + ((b & c) | (b & d) | (c & d)) + e + w[t] +0x8f1bbcdc;
		e = d;
		d = c;
		c = rol(b, 30);
		b = a;
		a = tmp;
    }
    for (t = 60; t < 80; t++) 
	{
		WORD32 tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0xca62c1d6;
		e = d;
		d = c;
		c = rol(b, 30);
		b = a;
		a = tmp;
    }

    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;
}

/* ----------------------------------------------------------------------
 * Outer SHA algorithm: take an arbitrary length byte string,
 * convert it into 16-word blocks with the prescribed padding at
 * the end, and pass those blocks to the core SHA algorithm.
 */

void SHA_Init(SHA_State * s)
{
    SHA_Core_Init(s->h);
    s->blkused = 0;
    s->lenhi = s->lenlo = 0;
}

void SHA_Bytes(SHA_State * s, void *p, int len)
{
    BYTE *q = (BYTE *) p;
    WORD32 wordblock[16];
    WORD32 lenw = len;
    int i;

    /* Update the length field.  */
    s->lenlo += lenw;
    s->lenhi += (WORD32)(s->lenlo < lenw);

    if (s->blkused && s->blkused + len < 64) 
	{
	/*
	 * Trivial case: just add to the block.
	 */
	memcpy(s->block + s->blkused, q, len);
	s->blkused += len;
    } else {
	/*
	 * We must complete and process at least one block.
	 */
	while (s->blkused + len >= 64) {
	    memcpy(s->block + s->blkused, q, 64 - s->blkused);
	    q += 64 - s->blkused;
	    len -= 64 - s->blkused;
	    /* Now process the block. Gather bytes big-endian into words */
	    for (i = 0; i < 16; i++) {
		wordblock[i] =
		    (((WORD32) s->block[i * 4 + 0]) << 24) |
		    (((WORD32) s->block[i * 4 + 1]) << 16) |
		    (((WORD32) s->block[i * 4 + 2]) << 8) |
		    (((WORD32) s->block[i * 4 + 3]) << 0);
	    }
	    SHATransform(s->h, wordblock);
	    s->blkused = 0;
	}
	memcpy(s->block, q, len);
	s->blkused = len;
    }
}

void SHA_Final(SHA_State * s, BYTE *output)
{
    int i;
    int pad;
    BYTE c[64];
    WORD32 lenhi, lenlo;

    if (s->blkused >= 56)
	pad = 56 + 64 - s->blkused;
    else
	pad = 56 - s->blkused;

    lenhi = (s->lenhi << 3) | (s->lenlo >> (32 - 3));
    lenlo = (s->lenlo << 3);

    memset(c, 0, pad);
    c[0] = 0x80;
    SHA_Bytes(s, &c, pad);

    c[0] = (lenhi >> 24) & 0xFF;
    c[1] = (lenhi >> 16) & 0xFF;
    c[2] = (lenhi >> 8) & 0xFF;
    c[3] = (lenhi >> 0) & 0xFF;
    c[4] = (lenlo >> 24) & 0xFF;
    c[5] = (lenlo >> 16) & 0xFF;
    c[6] = (lenlo >> 8) & 0xFF;
    c[7] = (lenlo >> 0) & 0xFF;

    SHA_Bytes(s, &c, 8);

    for (i = 0; i < 5; i++) {
	output[i * 4] = (s->h[i] >> 24) & 0xFF;
	output[i * 4 + 1] = (s->h[i] >> 16) & 0xFF;
	output[i * 4 + 2] = (s->h[i] >> 8) & 0xFF;
	output[i * 4 + 3] = (s->h[i]) & 0xFF;
    }
}

void SHA_Simple(void *p, int len, BYTE *output)
{
    SHA_State s;

    SHA_Init(&s);
    SHA_Bytes(&s, p, len);
    SHA_Final(&s, output);
}

/* ----------------------------------------------------------------------
 * The above is the SHA-1 algorithm itself. Now we implement the
 * HMAC wrapper on it.
 */


static void sha1_key(SHA_State * s1, SHA_State * s2,
		     BYTE *key, int len)
{
    BYTE foo[64];
    int i;

    memset(foo, 0x36, 64);
    for (i = 0; i < len && i < 64; i++)
	foo[i] ^= key[i];
    SHA_Init(s1);
    SHA_Bytes(s1, foo, 64);

    memset(foo, 0x5C, 64);
    for (i = 0; i < len && i < 64; i++)
	foo[i] ^= key[i];
    SHA_Init(s2);
    SHA_Bytes(s2, foo, 64);

    memset(foo, 0, 64);		       /* burn the evidence */
}

void hmac_sha1_simple(void *key, int keylen, void *data, int datalen,
		      BYTE *output) {
    SHA_State s1, s2;
    BYTE intermediate[20];
	
    sha1_key(&s1, &s2, (BYTE*)key, keylen);
    SHA_Bytes(&s1, data, datalen);
    SHA_Final(&s1, intermediate);

    SHA_Bytes(&s2, intermediate, 20);
    SHA_Final(&s2, output);
}

void sha1_init(hash_state * md)
{
    SHA_Init(&(md->sha1));
}
void sha1_process(hash_state * md, const BYTE *buf1, unsigned long len)
{
    SHA_Bytes(&(md->sha1), (void*)buf1, len);
}
void sha1_done(hash_state * md, BYTE *hash)
{
    SHA_Final(&(md->sha1), hash);
}
#if 0
/* hash the ssh representation of the mp_int mp */
void sha1_process_mp(hash_state *hs, mp_int *mp) 
{
	
	int i;
	buffer * buf;
	
	buf = buf_new(512 + 20); /* max buffer is a 4096 bit key, plus header + some leeway*/
	buf_putmpint(buf, mp);
	i = buf->pos;
	buf->pos = 0;
	sha1_process(hs, &buf->data[buf->pos], i);
	buf_free(buf);
}
#endif

int bytes_putmpint(BYTE* buf1, mp_int * mp)
{
	unsigned int len, pad = 0;
	int pos = 0;
	if (mp->sign == MP_NEG)
	{
		return 0;
	}
	/* zero check */
	if (mp->used == 1 && mp->dp[0] == 0)
	{
		len = 0;
	}
	else
	{
	    /* SSH spec requires padding for mpints with the MSB set, this code implements it */
		len = mp_count_bits(mp);
		/* if the top bit of MSB is set, we need to pad */
		pad = (len%8 == 0) ? 1 : 0;
		len = len / 8 + 1; /* don't worry about rounding, we need it for
		padding anyway when len%8 == 0 */
	}
	/* store the length */
	{
		WORD32 nval = htonl(len);
		memcpy(buf1,&nval,4);
		pos +=4;
	}
	/* store the actual value */
	if (len > 0) 
	{
		if (pad) 
		{
			buf1[pos] = 0x00;
			pos += 1;
		}
		if (mp_to_unsigned_bin(mp, &buf1[pos]) != MP_OKAY)
		{
			/* printf("mpint error"); */
			return 0;
		}
		pos += len-pad;
	}
	return pos;
}
#if 1
/* hash the ssh representation of the mp_int mp */
void sha1_process_mp(hash_state *hs, mp_int *mp) 
{
	
	int i;
	BYTE buf1[532]={0};
	
	i = bytes_putmpint(buf1,mp);
	sha1_process(hs, buf1, i);
}
#endif
