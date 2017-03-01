#ifndef SKY_UDS_MESSENGER_H
#define SKY_UDS_MESSENGER_H
#include "pub.h"
#include "timer.h"

enum UdsMessageType
{
    UDS_COMMON,
    UDS_SYSTEM,
    UDS_TIMER,
    UDS_CALL,
};

typedef struct tagUdsMsgHeader
{
    uint32 headver;
    uint32 headlen;
    uint32 bodyver;
    uint32 bodylen;
    uint32 msgid;
    UdsMessageType msgtype;
    char  sender[128];
}UdsHeader;

class UdsParam
{
public:
    UdsParam()
    {
        msgid = 0;
        msgdata = NULL;
        msgsize = 0;
        msgver = 0;
        timeout = WAIT_FOREVER;
    };

    UdsParam(uint32 id,void *data,uint32 len,uint32 version)
    {
        msgid = id;
        msgdata = data;
        msgsize = len;
        msgver = version;
        timeout = WAIT_FOREVER;
    };

    uint32 msgid;
    uint32 msgver;
    uint32 msgsize;
    const void *msgdata;
    uint32 timeout;
};

class UdsMessageOption
{
public:
    UdsMessageOption()
    {
        msgid = 0;
        msgver = 0;
    };

    UdsMessageOption(const string& _receiver,uint32 _msgid):receiver(_receiver)
    {
        msgid = _msgid;
        msgver = 0;
    };

    UdsMessageOption(const string& _receiver,uint32 _msgid,uint32 _msgver):receiver(_receiver)
    {
        msgid = _msgid;
        msgver = _msgver;
    };

    string sender;
    string receiver;
    uint32 msgid;
    uint32 msgver;
};

typedef struct tagUdsResult
{
    uint32 msgid;
    uint32 msgver;
    uint32 msgsize;
    uint32 msgtype;
    void *buf;
    uint32 bufsize;
}UdsResult;

class UdsMessenger:public TimerClient
{
public:
    enum {
        MESSAGE_STRING_MODE = 1,    //��Ϣ����Ϊ�ַ���
        MESSAGE_BINARY_MODE = 2,   //��Ϣ����Ϊ���������ݿ�
    };

    UdsMessenger(const string& name):TimerClient(name)
    {
        _sockfd = -1;
        _listener = INVALID_THREAD_ID;
        _current = NULL;
        _message_mode = MESSAGE_BINARY_MODE;
    };

    ~UdsMessenger()
    {
        if(_listener != INVALID_THREAD_ID)
        {
            KillThread(_listener);
        }
        
        if(_sockfd > 0)
        {
            close(_sockfd);
        }
    };
	
    void MakeFullPath(const char *strParent,const char *strSub,char *strOut,int iOutSize);
    //��ʼ��һ��UdsMessenger������ļ������ڻ��Զ�����������Ŀ¼Ҫ�û��Լ�ȥ��
    STATUS Init(const char *sockfile);
    //���send������������ʵ����һ���ģ�������ʵ��Ҫȡһ�����õİ�
    STATUS Send(const char *dest,const UdsParam& param);//���Ͷ��������ݿ�
    STATUS Send(const Serializable& data,UdsMessageOption& option);//����c++�����л��ṹ��
    STATUS Send(const string& data,UdsMessageOption& option);//����c++�ַ���
    STATUS Send(const string& data,uint32 message_id,const string& receiver);
    STATUS Send(const Serializable& data,uint32 message_id,const string& receiver);
    //���ն��������ݿ�ģʽ������Ϣ
    STATUS Wait(char *rx_buf,int rx_size,uint32 timeout);
    //����c++�ַ�����ʽ������Ϣ
    STATUS Wait(string& data,UdsMessageOption& option,uint32 timeout);
    STATUS Run(int priority = 1);
    const char *GetPeerPath();
    const char *GetSelfPath();
    int GetMessageMode();
    void SetMessageMode(int mode);
    //���ϣ��ʹ�ñ�������Ϊ��Ϣ���ջص���ڣ��������SetMessageMode(MESSAGE_BINARY_MODE)����ΪĬ���Ѿ���MESSAGE_BINARY_MODE
    virtual void MessageEntry(uint32 id,void *data,uint32 length,uint32 version) {};
    //���ϣ��ʹ�ñ�������Ϊ��Ϣ���ջص���ڣ�����Init֮�����SetMessageMode(MESSAGE_STRING_MODE)
    virtual void MessageEntry(const string& data,const UdsMessageOption& option) {};
    void DoTimer(const TimerMessage&);
    TIMER_ID GetTimerId();
    uintptr GetTimerArg();
    STATUS Call(const char *destfile,const UdsParam& param,UdsResult& result);
	
private:    
    DISALLOW_COPY_AND_ASSIGN(UdsMessenger);
    STATUS _SendToFile(int fd,
                       const char *destfile,
                       const UdsParam& param,
                       UdsMessageType type);
    
    STATUS _SendToDir(int fd,
                      const char *destdir,
                      const UdsParam& param,
                      UdsMessageType type);
    string    _sockfile;
    int       _sockfd;
    THREAD_ID _listener;
    UdsHeader *_current;
    int _message_mode;
};
#endif


