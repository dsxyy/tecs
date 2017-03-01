
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
        /*����˿ڲ����ڣ���Ҫ���ovs�е������˿�*/
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

    /*���uplink�ڵ��ӽӿڵ�mtu��admin status��Ҫ��uplink��һ��*/
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

            /*�ҳ����и�uplink�µ��ӽӿ�*/
            vector<string> up_subs;            
            if (SUCCESS == VNetGetAllUplinksSubnets(*itup, up_subs))
            {
                vector<string>::iterator itsub = up_subs.begin();
                for ( ; itsub != up_subs.end(); ++itsub)
                {
                    /*ȡ���ӽӿڵ�mtu����uplink�Ƚϣ������һ�����������ӽӿڵ�mtuΪuplink��һ��*/
                    unsigned int SubMtu;
                    string port_name = *itsub;
                    if (VNET_PLUGIN_SUCCESS == VNetGetMtu(port_name.c_str(), SubMtu))
                    {
                        if (UplinkMtu != SubMtu)
                        {
                            VNetSetMtu(*itsub, UplinkMtu);
                        }          
                    }

                    /*ȡ���ӽӿڵ�admin status����uplink�Ƚϣ������һ�����������ӽӿڵ�admin statusΪuplink��һ��*/
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
    // ���㶨λ
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



