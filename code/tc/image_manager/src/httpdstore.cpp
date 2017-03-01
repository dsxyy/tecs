#include "tecs_config.h"
#include "log_agent.h"
#include "image_store.h"

static int httpdstore_print_on = 0;
DEFINE_DEBUG_VAR(httpdstore_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(httpdstore_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{
HttpdStore::HttpdStore()
{

}

HttpdStore::~HttpdStore()
{

}

STATUS HttpdStore::Init()
{
    return SUCCESS;
}

STATUS HttpdStore::GetStorageDir(string& dir)
{
    return SUCCESS;
}

STATUS HttpdStore::GetUrlByPath(const string& path,FileUrl& url)
{
    return SUCCESS;
}
}

