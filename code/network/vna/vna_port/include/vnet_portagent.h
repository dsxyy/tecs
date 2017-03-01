/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_portagent.h
* 文件标识：
* 内容摘要：CPortAgent类的定义文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年2月25日
******************************************************************************/

#if !defined(VNET_PORTAGENT_INCLUDE_H__)
#define VNET_PORTAGENT_INCLUDE_H__

#include "vm_messages.h"
#include "vnet_msg.h"
#include "vnet_portinfo.h"
#include "vnet_mid.h"

namespace zte_tecs
{

class CPortAgent : public MessageHandler
{
public:
    virtual ~CPortAgent();

public:

    int32  GetPortReportInfo(CMessageVNAPortInfoReport &PortInfo);

    int32  GetEvbPortInfo(vector<CEvbPortInfo> &vPortInfo);

    STATUS ProcKernelPortCfg(const MessageFrame& message);
    STATUS ProcLoopBackPortCfg(const MessageFrame& message);	

    STATUS ProcPortCfg(const MessageFrame& message);	

    static CPortAgent *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CPortAgent();
        }

        return s_pInstance;
    };

    STATUS Init(MessageUnit *mu);

    void MessageEntry(const MessageFrame& message);

    void SetDbgPrint(int32 nFlag);	

public:

protected:
    //void MessageEntry(const MessageFrame& message);

private:
    CPortAgent();

private:
    static CPortAgent *s_pInstance;

private:
    MessageUnit *m_pMU;
    int32 m_bOpenDbgInf;

private:

};
}// namespace zte_tecs

#endif // VNET_PORTAGENT_INCLUDE_H__

