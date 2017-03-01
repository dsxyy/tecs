/*************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司
* All rights reserved.
* 
* 文件名称：smart_api.h
* 文件标识：见配置管理计划书
* 内容摘要：磁盘检测对外头文件
* 当前版本：1.0
* 作    者：guojsh
* 完成日期：2011年8月20日
*
* 修改记录1：
*  修改日期：
*  版 本 号：
*  修 改 人：
*  修改内容：
*************************************************************************/
#ifndef _SMART_H
#define _SMART_H

#include <vector>
#include "smart_api.h"


// 禁止使用拷贝构造函数和 operator= 赋值操作的宏
// 应该类的 private： 中使用
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

namespace zte_tecs {
    
    
/**
@brief 功能描述: 磁盘SMART检测
@li @b 其它说明：无
    */
    class DiskSmart
    {
        
    public:
        DiskSmart();
        
        ~DiskSmart();
        
        /**
        @brief 功能描述:磁盘自检
        
        @li @b 入参列表：
        @verbatim
        testType        自检类型
        @endverbatim
          
        @li @b 出参列表：
        @verbatim
        无
        @endverbatim
            
        @li @b 返回值列表：
        @verbatim
        _SMART_SUCCESS           成功
        _SMART_ERROR             失败
        @endverbatim
              
        @li @b 接口限制：无
        @li @b 其它说明：无
        */
        static int diskSmartTest(unsigned int testType);
        
        /**
        @brief 功能描述:获取磁盘SMART状态
        
        @li @b 入参列表：
        @verbatim
        无
        @endverbatim
          
        @li @b 出参列表：
        @verbatim
        diskSmartInfo        磁盘SMART信息
        @endverbatim
    
        @li @b 返回值列表：
        @verbatim
        _SMART_SUCCESS           成功
        _SMART_ERROR             失败
        @endverbatim
              
        @li @b 接口限制：无
        @li @b 其它说明：无
        */
        static int getSmartInfo(std::vector<T_DmPdSmartInfo> &diskSmartInfo);
        
    private:
        DISALLOW_COPY_AND_ASSIGN(DiskSmart);
    };
}//namespace zte_tecs

#endif /*_SMART_H*/


