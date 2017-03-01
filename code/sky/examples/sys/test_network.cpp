#include "sky.h"

void DbgGetIp(const char* if_name)
{
    string ip_address;
    if(SUCCESS != GetIpAddress(if_name,ip_address))
    {
        return; 
    }
    printf("%s ip = %s\n",if_name,ip_address.c_str());
    uint32 ip;
    if(SUCCESS != GetIpAddress(if_name,ip))
    {
        return; 
    }
    printf("%s ip = 0x%x\n",if_name,ip);
}
DEFINE_DEBUG_FUNC(DbgGetIp);

int main()
{
    Shell("test_network-> ");
    while(1)
    {
        sleep(1);
    }
    return 0;
}



