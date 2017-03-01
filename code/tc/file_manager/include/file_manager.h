/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_manager.h
* 文件标识：见配置管理计划书
* 内容摘要：ImageManager类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
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
@brief 功能描述: 映像资源池的实体类\n
@li @b 其它说明: 无
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
        //启动消息单元工作线程
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

