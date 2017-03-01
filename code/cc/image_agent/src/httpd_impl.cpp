/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：httpd_impl.cpp
* 文件标识：
* 内容摘要：基于httpd的映像代理模块实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年3月12日
*
* 修改记录1：
*     修改日期：2012/3/12
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
//#include "tecs_config.h"
//#include "vm_messages.h"
//#include "nfs.h"
#include "image_cache.h"
#include "image_agent.h"
//#include "log_agent.h"
//#include "mid.h"
//#include "event.h"
#include <boost/algorithm/string.hpp>
using namespace boost;

static int httpd_ia_impl_print_on = 0;  
DEFINE_DEBUG_VAR(httpd_ia_impl_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(httpd_ia_impl_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)
        
namespace zte_tecs
{
HttpdIAImpl::HttpdIAImpl(const HttpdIAConfig& config):_config(config)
{

}

STATUS HttpdIAImpl::Init()
{
     return SUCCESS;
}

STATUS HttpdIAImpl::GetMappedUrl(ImageCache& cache,string& error)
{
     return SUCCESS;
}

STATUS HttpdIAImpl::CachePrepare(ImageCache& cache)
{
    return SUCCESS;
}
STATUS HttpdIAImpl::CacheRelease(ImageCache& cache)
{
    return SUCCESS;
}
}



