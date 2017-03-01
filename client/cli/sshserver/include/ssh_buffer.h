#ifndef _SSH_BUFFER_H_
#define _SSH_BUFFER_H_

#include "ssh_mp.h"

#define BUF_MAX_INCR 1000000000
#define BUF_MAX_SIZE 1000000000
typedef struct buf 
{
	unsigned char * data;
	unsigned int len; /* the used size */
	unsigned int pos;
	unsigned int size; /* the memory size */
}buffer;

void buf_putmpint(buffer* buf, mp_int * mp);
buffer* buf_new(unsigned int size);
void buf_free(buffer* buf) ;
void buf_putint(buffer* buf, int unsigned val);
void buf_putstring(buffer* buf, const unsigned char* str, unsigned int len);
void buf_putbytes(buffer *buf, const unsigned char *bytes, unsigned int len);
void buf_incrwritepos(buffer* buf, unsigned int incr);
void buf_putbyte(buffer* buf, unsigned char val) ;
#endif
