/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_metadata.h
* 文件标识： 
* 内容摘要：虚拟机元数据服务实现头文件
* 当前版本：1.0
* 作    者：钟春山 
* 完成日期： 
*******************************************************************************/
#ifndef VM_METADATA_H
#define VM_METADATA_H

#include "vm_messages.h"
#include "vm_instance.h"
#include "log_agent.h"

namespace zte_tecs
{
    /**
    @brief 功能描述:收集虚拟机元数据信息，按xml格式组织。

    @li @b 入参列表：
    @verbatim
    VmInstance *pinstance   指向当前实例的指针
    @endverbatim

    @li @b 出参列表：
    @verbatim
    无
    @endverbatim

    @li @b 返回值列表：
    @verbatim
    无
    @endverbatim

    @li @b 接口限制：该函数收集的元数据是通过iso传递给虚拟机使用，需要传递的信息可以在此函数中添加。
    @li @b 其它说明：无
    */
    string CreateVmMetadata(VmInstance *pinstance);

}
#endif

