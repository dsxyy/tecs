
#include "ssh_string.h"
#include <string.h>
#include <arpa/inet.h>
/* 字符串长度 */
DWORD ssh_string_length(Tssh_string * ssh_string)
{
    return ntohl(ssh_string->length);
}

/* 字符串数量 */
int ssh_namelist_count(Tssh_namelist * ssh_string)
{
    int count = 0;
    WORD32 i = 0;

    DWORD length = ssh_string_length(ssh_string);
    for (i=0; i<length; i++)
    {
        if (ssh_string->data[i] == ',')
        {
            count++;
        }
    }
    /* 如果最后一个字符是','，count需要加1 */
    if ((length != 0) && (ssh_string->data[length-1] == ','))
    {
        count++;
    }

    return count;
}

static DWORD ssh_namelist_substring_length(Tssh_string * ssh_string, DWORD current_position)
{
    DWORD length = ssh_string_length(ssh_string);

    DWORD position = current_position;
    while ((position < length) && (ssh_string->data[position] != ','))
    {
        position++;
    }

    return position-current_position;
}
/* 是否包含某个字符串 */
BOOL ssh_namelist_contained(Tssh_string * ssh_string, char * str)
{
    DWORD length = ssh_string_length(ssh_string);
    size_t str_length = strlen(str);
    DWORD current_position = 0;

    /* 遍历每个substring */
    for (; current_position < length; )
    {
        DWORD substring_length = ssh_namelist_substring_length(ssh_string, current_position);
        if ((substring_length == str_length) &&
            (strncmp(str, ssh_string->data+current_position, substring_length) == 0))
        {
            return TRUE;
        }

        /* 下一个substring */
        current_position += substring_length + 1;
    }

    return FALSE;
}

/* 与buffer的交互 */
char * ssh_string_to_buffer(char * str, char * buffer)
{
    DWORD str_length = (DWORD)strlen(str);
    DWORD ssh_string_length1 = ntohl(str_length);

    memmove(buffer, &ssh_string_length1, sizeof(DWORD));
    if (ssh_string_length1 != 0)
    {
        memmove(buffer+sizeof(DWORD), str, str_length);
    }

    return buffer+sizeof(DWORD)+str_length;
}

BYTE *ssh_byte_to_buffer(BYTE ucTemp, BYTE * buffer)
{
	memmove(buffer,&ucTemp,sizeof(BYTE));
    return buffer+sizeof(BYTE);
}

BYTE *ssh_word32_to_buffer(WORD32 dwTemp, BYTE * buffer)
{
	dwTemp = htonl(dwTemp);
	memmove(buffer,&dwTemp,sizeof(WORD32));
    return buffer+sizeof(WORD32);
}

char * ssh_string_mem_to_buffer(void * mem, DWORD mem_size, char * buffer)
{
    DWORD ssh_string_length1 = ntohl(mem_size);

    memmove(buffer, &ssh_string_length1, sizeof(DWORD));
    if (ssh_string_length1 != 0)
    {
        memmove(buffer+sizeof(DWORD), mem, mem_size);
    }

    return buffer+sizeof(DWORD)+mem_size;
}
char * ssh_string_from_buffer(Tssh_string ** pssh_string, char * buffer)
{
    *pssh_string = (Tssh_string*)buffer;

    DWORD ssh_string_length1 = ntohl((*pssh_string)->length);

    return buffer+sizeof(DWORD)+ssh_string_length1;
}

/* C字符串转为ssh_string */
BOOL ssh_string_from_chars(Tssh_string * ssh_string, DWORD max_string_length, char * str)
{
    DWORD str_length = (DWORD)strlen(str);

    if (str_length <= max_string_length)
    {
        ssh_string->length = htonl(str_length);
        memmove(ssh_string->data, str, str_length);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
