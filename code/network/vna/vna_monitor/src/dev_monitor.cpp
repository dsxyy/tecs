/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_monitor.cpp
* 文件标识：
* 内容摘要：监控相关类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月16日
*
* 修改记录1：
*     修改日期：2013/1/16
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#include <typeinfo>
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "dev_mon_reg_func.h"
#include "vna_switch_module.h"
#include "vnet_vnic.h"
#include "vna_vnic_pool.h"


namespace zte_tecs
{

/************************************************************
* 函数名称： VisitDev
* 功能描述： 监控网络设备MTU的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CDevMonMTU::VisitDev(CNetDev *dev_entity)
{ 
    int32 nSize = 0;

    TDevMonFuncReg *pMonFuncReg = GetMonMTUFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};

/************************************************************
* 函数名称： VisitDev
* 功能描述： 监控网络设备环回属性函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CPortMonBridge::VisitPort(CPortNetDev *dev_entity)
{ 
    int32 nSize = 0;

    TPortMonFuncReg *pMonFuncReg = GetMonBridgeFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};  

/************************************************************
* 函数名称： VisitBridge
* 功能描述： 监控网桥的上行端口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CPortMonLoop::VisitPort(CPortNetDev *dev_entity)
{ 
    int32 nSize = 0;

    TPortMonFuncReg *pMonFuncReg = GetMonLoopFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};

/************************************************************
* 函数名称： VisitBridge
* 功能描述： 监控网桥的上行端口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
void CBridgeMonUplink::VisitBridge(CSwitchDev *dev_entity)
{ 
    int32 nSize = 0;

    TDVSMonFuncReg *pMonFuncReg = GetMonUpLinkFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};  

void CVnicVisitor::VisitPort(CVnicPortDev *dev_entity)
{
    //cout << "enter CVnicVisitor::VisitPort" <<endl;

    int32 nSize = 0;

    TVnicMonFuncReg *pMonFuncReg = GetMonVnicFuncReg(nSize);

    for (int32 i = 0; i < nSize; ++i)
    {
        if (*(pMonFuncReg[i].m_TypeInfo) == typeid(*dev_entity))
        {
            pMonFuncReg[i].m_FuncPort(dev_entity);
        }
    }
};


}


