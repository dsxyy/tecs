/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ImageManager��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#ifndef FILE_MANGER_FILE_MANAGER_H        
#define FILE_MANGER_FILE_MANAGER_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "vm_messages.h"
#include "image_url.h"

 #define EV_FILE_SCAN_TIMER EV_TIMER_1   
    
namespace zte_tecs
{
STATUS EnableCoredumpFileOptions();

/**
@brief ��������: ӳ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
*/
class FileManager: public MessageHandler
{
public:
    static FileManager* GetInstance()
        {
            if(NULL == instance)
            {
                instance = new FileManager(); 
            }
        
        return instance;
    };

    ~FileManager( )
    {
        if (NULL != _mu)
        {
            delete _mu;
        } 
    }

    STATUS Init()
    {
        //������Ϣ��Ԫ�����߳�
        return StartMu(MU_FILE_MANAGER);
    }

private:
    static FileManager *instance;
    FileManager();
    STATUS StartMu(const string& name);
    virtual void MessageEntry(const MessageFrame &message);
    bool  Mkdir(const string &path);
    bool  ExportDirToNfs(const string &path);
    void  RestartNfs();
    void  SendGetUrlAck(const MID &receiver, const string &action_id);

    MessageUnit *_mu;
    TIMER_ID    _timer_id;
    DISALLOW_COPY_AND_ASSIGN(FileManager); 
};

}
#endif /* FILE_MANGER_FILE_MANAGER_H */

