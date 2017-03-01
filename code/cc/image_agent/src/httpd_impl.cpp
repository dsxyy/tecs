/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�httpd_impl.cpp
* �ļ���ʶ��
* ����ժҪ������httpd��ӳ�����ģ��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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



