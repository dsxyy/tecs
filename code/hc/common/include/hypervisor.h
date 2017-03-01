/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：hypervisor.h
* 文件标识：见配置管理计划书
* 内容摘要：跟虚拟化相关，hc各个模块公用的函数接口
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年8月10日
*
* 修改记录1：
*     修改日期：2012/8/10
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#ifndef HH_HYPERVISOR_H
#define HH_HYPERVISOR_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{
inline bool IsXenKernel()
{
    if (0 != access("/proc/xen", F_OK))
    {
        return false; //非xen内核
    }
    
    return true;
}

inline bool IsKvmKernel()
{
    if (0 != access("/dev/kvm", F_OK))
    {
        return false;
    }
    
    return true;
}
} 
#endif 


