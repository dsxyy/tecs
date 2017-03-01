/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：storage_cli.h
* 文件标识：见配置管理计划书
* 内容摘要：
* 当前版本：1.0
* 作    者：冯骏
* 完成日期：2012年9月19日
*
* 修改记录1：
*     修改日期：2012年9月19日
*     版 本 号：V1.0
*     修 改 人：冯骏
*     修改内容：创建
*******************************************************************************/
#ifndef STORAGE_ADAPTOR_INITIATOR_H
#define STORAGE_ADAPTOR_INITIATOR_H
#include "sky.h"
#include "mid.h"
#include "log_agent.h"
#include "storage_adaptor.h"

namespace zte_tecs 
{

class StorageAdaptorInitiator 
{
public:
    static StorageAdaptorInitiator *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new StorageAdaptorInitiator();
        }
        return _instance;
    }

    STATUS Init();       
    ~StorageAdaptorInitiator() 
    { 
        _instance = NULL; 
    }
    StorageAdaptorInitiator(){};  
    bool  CheckCfg(sa_cfg_array &cfg_array);
    
private:   
    bool            _have_startup;  // 是否已启动的标志，避免重新初始化
    string          _sc_name;       // 本SA归属的tecs系统名称    
    vector<sa_cfg>  _sa_cfg;
    TIMER_ID        _timer;    
    vector<Storage_Adaptor *>       _adaptor_array;

    static StorageAdaptorInitiator  *_instance; 
 
    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(StorageAdaptorInitiator);   
};

STATUS EnableSaStoreOptions();
}

#endif // #ifndef HM_HOST_MANAGER_H

