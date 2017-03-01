/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：md5.cpp
* 文件标识：见配置管理计划书
* 内容摘要：计算md5
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年5月9日
*
* 修改记录1：
*     修改日期：2012/05/09
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include <sys/mman.h>
#include <openssl/md5.h>
#include "misc.h"

string GetStrMd5sum(const string& str)
{
#if 0
    //计算md5，使用openssl第三方库中的MD5函数
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)str.c_str(), str.size(), result);
    char md5[MD5_DIGEST_LENGTH * 2 + 1] = "";
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
       char temp[3];
       sprintf(temp, "%02x", result[i]);
       strcat(md5, temp);
    }
#else
    static const char hexDigits[17] = "0123456789abcdef";
    unsigned char digest[MD5_DIGEST_LENGTH];
    char md5[2*MD5_DIGEST_LENGTH+1];

    // Count digest
    MD5((const unsigned char*)str.c_str(), str.length(), digest);

    // Convert the hash into a hex string form
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++ )
    {
      md5[i*2]   = hexDigits[(digest[i] >> 4) & 0xF];
      md5[i*2+1] = hexDigits[digest[i] & 0xF];
    }
    md5[MD5_DIGEST_LENGTH*2] = '\0';
#endif
    return string(md5);
}

STATUS GetFileMd5sum(const string& file, string& md5sum)
{
    struct stat64 s;
    if (0 != stat64(file.c_str(),&s))
    {
        return ERROR;
    }
    
    //计算文件md5，使用openssl第三方库中的MD5函数
    unsigned char result[MD5_DIGEST_LENGTH];
    int fd = open(file.c_str(), O_RDONLY);
    if(fd < 0) 
    {
        return ERROR;
    }

    char *buffer = (char *)mmap64(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
    MD5((unsigned char*)buffer, s.st_size, result);
    munmap(buffer,s.st_size);
    close(fd);

    char md5[MD5_DIGEST_LENGTH * 2 + 1] = "";
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
       char temp[3];
       sprintf(temp, "%02x", result[i]);
       strcat(md5, temp);
    }
    
    md5sum = md5;
    return SUCCESS;
}

