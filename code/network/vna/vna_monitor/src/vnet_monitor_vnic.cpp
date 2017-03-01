
#include "vnet_monitor.h"
#include "vnet_function.h"

extern int g_monitor_times;


namespace zte_tecs
{

extern int g_dbgVNetMonFlag;

int g_dbgNicHandle = true;
void t_nic_handle(int i)
{
    g_dbgNicHandle = i;
}
DEFINE_DEBUG_FUNC(t_nic_handle);

#define MAX_MONITOR_TIMES     15

// bond monitor object 
CVNetMonitorNic::CVNetMonitorNic()
{
}

CVNetMonitorNic::~CVNetMonitorNic()
{
}

int32 CVNetMonitorNic::Init()
{
    return 0;
}

void CVNetMonitorNic::OvsNicHandle()
{   
    vector<string> nics;
    if (SUCCESS == VNetGetAllVnics(nics))
    {
    vector<string>::iterator it = nics.begin();
    for ( ; it != nics.end(); ++it)
    {
        /*如果端口不存在，需要清除ovs中的垃圾端口*/
        if (!VNetIfconfigIsExist(*it))
        {
            map<string, int32>::iterator pos = m_mapGabageOvsIf.find(*it);
            if (m_mapGabageOvsIf.end() == pos)
            {
                m_mapGabageOvsIf.insert(pair<string, int32> (*it, 1));
                continue;
            }

            if (pos->second < MAX_MONITOR_TIMES)
            {
                pos->second ++;                
            }
            else
            {
                VNetDelVnicOfOVS(*it);
                m_mapGabageOvsIf.erase(pos);
            }            
        }
        else
        {
            map<string, int32>::iterator pos = m_mapGabageOvsIf.find(*it);
            if (m_mapGabageOvsIf.end() != pos)
            {
                m_mapGabageOvsIf.erase(pos);
            }            
        }
    }
}

    /*监控uplink口的子接口的mtu和admin status需要和uplink口一致*/
    vector<string> uplinks;
    if (SUCCESS == VNetGetAllUplinks(uplinks))
    {
        vector<string>::iterator itup = uplinks.begin();
        for ( ; itup != uplinks.end(); ++itup)
        {
            unsigned int UplinkMtu;
            string port_name = *itup;
            if (VNET_PLUGIN_SUCCESS != VNetGetMtu(port_name.c_str(), UplinkMtu))
            {
                continue;
            }

            int UplinkPortStatus;
            if (VNET_PLUGIN_SUCCESS!=VNetGetNicAdminStatus(*itup, UplinkPortStatus))
            {
                continue;
            } 

            /*找出所有该uplink下的子接口*/
            vector<string> up_subs;            
            if (SUCCESS == VNetGetAllUplinksSubnets(*itup, up_subs))
            {
                vector<string>::iterator itsub = up_subs.begin();
                for ( ; itsub != up_subs.end(); ++itsub)
                {
                    /*取出子接口的mtu，与uplink比较，如果不一样，则设置子接口的mtu为uplink的一样*/
                    unsigned int SubMtu;
                    string port_name = *itsub;
                    if (VNET_PLUGIN_SUCCESS == VNetGetMtu(port_name.c_str(), SubMtu))
                    {
                        if (UplinkMtu != SubMtu)
                        {
                            VNetSetMtu(*itsub, UplinkMtu);
                        }          
                    }

                    /*取出子接口的admin status，与uplink比较，如果不一样，则设置子接口的admin status为uplink的一样*/
                    int SubPortStatus;
                    if (VNET_PLUGIN_SUCCESS==VNetGetNicAdminStatus(*itsub, SubPortStatus))
                    {
                        if (UplinkPortStatus != SubPortStatus)
                        {
                            VNetSetNicAdminStatus(*itsub, UplinkPortStatus);                        
                        }                         
                    }                    
                }                
            }
        }        
    }    
}


/*********************************************************

*********************************************************/
void CVNetMonitorNic::MonitorHandle()
{
    // 方便定位
    if( false == g_dbgNicHandle )
    {
        return;
    }
    
    DbgPrint("MonitorHandle~~~");
    
    OvsNicHandle();
}


void CVNetMonitorNic::Print()
{
}

STATUS CVNetMonitorNic::DbgPrint( const char* pcFmt,...)
{   
    if( false == g_dbgVNetMonFlag)
    {
        return 0;
    }
    
    va_list     vList;
    char tmp[256]={0};

    va_start(vList, pcFmt);    
    vsnprintf(tmp, sizeof(tmp), pcFmt, vList);
    cout << tmp << endl;
    va_end(vList);
    return 0;
}

}



