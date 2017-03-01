/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_handler.h
* 文件标识：见配置管理计划书
* 内容摘要：HostHandler类定义文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月9日
*
* 修改记录1：
*     修改日期：2011/8/9
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#ifndef HH_HOST_HANDLER_H
#define HH_HOST_HANDLER_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{
/**
@brief 功能描述: 物理机操作的实体类\n
@li @b 其它说明: 无
*/
class HostHandler : public MessageHandler
{
public:
    static HostHandler* GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new HostHandler(); 
        }
        return _instance;
    };

    STATUS Init()
    {
        return StartMu(MU_HOST_HANDLER);
    }
    
    virtual ~HostHandler() {}   // 默认析构函数
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    HostHandler();              // 构造函数
    STATUS StartMu(const string& name);

    static HostHandler* _instance;
    string      _cluster_name;  // 本主机归属的集群名称
    MessageUnit *_mu;           // 消息单元

    void DoStartUp();

#if 0
    /**************************************************************************/
    /**
    @brief 功能描述: 处理来自集群的原始操作命令

    @li @b 入参列表：
    @verbatim
        req     
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    void DoRawHandle(const MessageFrame &message);
#endif

    /**************************************************************************/
    /**
    @brief 功能描述: 处理来自集群的预定义操作命令

    @li @b 入参列表：
    @verbatim
        req     
    @endverbatim

    @li @b 出参列表：
    @verbatim
        无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
        无
    @endverbatim

    @li @b 接口限制：无
    @li @b 其它说明：无
    */
    /**************************************************************************/
    void DoDefHandle(const MessageFrame &message);

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(HostHandler);   
};

} // namespace zte_tecs
#endif // #ifndef HH_HOST_HANDLER_H


