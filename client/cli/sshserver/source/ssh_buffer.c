#include "ssh_session.h"

/* for our purposes we only need positive (or 0) numbers, so will
* fail if we get negative numbers */
void buf_putmpint(buffer* buf1, mp_int * mp)
{
	unsigned int len, pad = 0;
	if (mp->sign == MP_NEG)
	{
		printf("negative bignum");
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
	buf_putint(buf1, len);
	/* store the actual value */
	if (len > 0) 
	{
		if (pad) 
		{
			buf_putbyte(buf1, 0x00);
		}
		if (mp_to_unsigned_bin(mp, &buf1->data[buf1->pos]) != MP_OKAY)
		{
			printf("mpint error");
		}
		buf_incrwritepos(buf1, len-pad);
	}
}

/* Create (malloc) a new buffer of size */
buffer* buf_new(unsigned int size) 
{
	buffer* buf1;
	if (size > BUF_MAX_SIZE) 
	{
		printf("buf->size too big");
	}
	buf1 = (buffer*)XOS_GetUB(sizeof(buffer));
    if (size > 0)
	{
		buf1->data = (unsigned char*)XOS_GetUB(size);
	}
	else
	{
		buf1->data = NULL;
	}
	buf1->size = size;
	buf1->pos = 0;
	buf1->len = 0;
	return buf1;
}

/* free the buffer's data and the buffer itself */
void buf_free(buffer* buf1) 
{
	if(XOS_RetUB(buf1->data))
	{
		buf1->data=NULL;

	}
	if(XOS_RetUB((BYTE*)buf1))
	{
		buf1=NULL;

	}
}

/* put a 32bit uint into the buffer, incr bufferlen & pos if required */
void buf_putint(buffer* buf1, int unsigned val) 
{
	WORD32 nval = htonl(val);
	memcpy(&buf1->data[buf1->pos],&nval,4);
	buf_incrwritepos(buf1, 4);
}

/* put a SSH style string into the buffer, increasing buffer len if required */
void buf_putstring(buffer* buf1, const unsigned char* str, unsigned int len) 
{
	buf_putint(buf1, len);
	buf_putbytes(buf1, str, len);
	
}
/* put the set of len bytes into the buffer, incrementing the pos, increasing
* len if required */
void buf_putbytes(buffer *buf1, const unsigned char *bytes, unsigned int len)
{
	memcpy(&buf1->data[buf1->pos], bytes, len);
	buf_incrwritepos(buf1, len);
}

/* increment the postion by incr, increasing the buffer length if required */
void buf_incrwritepos(buffer* buf1, unsigned int incr) 
{
	if (incr > BUF_MAX_INCR || buf1->pos + incr > buf1->size) 
	{
		printf("bad buf_incrwritepos");
	}
	buf1->pos += incr;
	if (buf1->pos > buf1->len) 
	{
		buf1->len = buf1->pos;
	}
}
/* put a byte, incrementing the length if required */
void buf_putbyte(buffer* buf1, unsigned char val) 
{
	buf1->data[buf1->pos] = val;
	buf_incrwritepos(buf1, 1);
}


