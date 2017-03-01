/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：proc_admin.h
* 文件标识：见配置管理计划书
* 内容摘要：ProcAdmin类定义文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年11月12日
*
* 修改记录1：
*     修改日期：2011/11/12
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HA_PROC_ADMIN_H
#define HA_PROC_ADMIN_H
#include "sky.h"

namespace zte_tecs 
{

class ProcAdmin : public MessageHandlerTpl<MessageUnit>
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ProcAdmin *GetInstance();
    STATUS Init();
    
    virtual ~ProcAdmin() { _instance = NULL; }
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    STATUS DoStartUp();
    void DoRegisterSystem(const MessageFrame &message);

    static ProcAdmin *_instance;  
    ProcAdmin() {}

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(ProcAdmin);   
};

} // namespace zte_tecs
#endif // #ifndef HA_PROC_ADMIN_H

