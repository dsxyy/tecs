#ifndef SKY_MISC_H
#define SKY_MISC_H
#include "pub.h"

#define EXIT_POINT() CheckExitPoint(__FILE__,__func__,__LINE__);

//misc目录下集中存放的是各种琐碎繁杂、无处归类的封装接口
string GetStrMd5sum(const string& str);
STATUS GetFileMd5sum(const string& file, string& md5sum);
string JoinStrings(const vector<string>& strings,const string& separator);
STATUS SshGetPubkey(string& pubkey);
STATUS SshTrust(const string &pubkey);
STATUS SshUntrust(const string &pubkey);
STATUS RunCmd(const string& cmd, vector<string>& result);
STATUS RunCmd(const string& cmd, string& result);
STATUS RunCmd(const string& cmd);
FILE *Popen(const char *cmdstring, const char *type,pid_t &pid);
int  Pclose(FILE *fp,const pid_t &pid);
string GenerateUUID(void); /* GenerateUUID -- generate a string UUID */
void CheckExitPoint(const string& file, const string& function,int line);
void Log(int priority,const string& log);
void Log(int priority,const char* format,...);

bool Access(const string& pathname, int mode);
bool IsFileExist(const string& pathname);
STATUS CreateDir(string& dirname, int mode);
DIR* OpenDir(const string& dirname);
typedef struct dirent DIRENT;
//需要检查返回的文件名是否目录，如果是，需要递归
DIRENT* ReadDir(const DIR *dir);
#endif

