#ifndef _SSH_STRING
#define _SSH_STRING

#include "pub_tmp.h"

typedef struct tag_ssh_string
{
    WORD32 length;
    char data[1];
}Tssh_string, Tssh_namelist;

/* �ַ������� */
extern DWORD ssh_string_length(Tssh_string * ssh_string);

/* �ַ������� */
extern int ssh_namelist_count(Tssh_namelist * ssh_string);
/* �Ƿ����ĳ���ַ��� */
extern BOOL ssh_namelist_contained(Tssh_string * ssh_string, char * str);

/* ��buffer�Ľ��� */
extern char * ssh_string_to_buffer(char * str, char * buffer);
extern BYTE *ssh_byte_to_buffer(BYTE ucTemp, BYTE * buffer);
extern BYTE *ssh_word32_to_buffer(WORD32 dwTemp, BYTE * buffer);

extern char * ssh_string_mem_to_buffer(void * mem, DWORD mem_size, char * buffer);

extern char * ssh_string_from_buffer(Tssh_string ** pssh_string, char * buffer);

/* C�ַ���תΪssh_string */
/*bool ssh_string_from_chars(Tssh_string * ssh_string, DWORD max_string_length, char * str);*/

#endif
