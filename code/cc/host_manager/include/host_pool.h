/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�host_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��Host����������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HM_HOST_POOL_H
#define HM_HOST_POOL_H

#include "pool_sql.h"
#include "host.h"
#include "vm_messages.h"

using namespace std;
namespace zte_tecs {

class HostPool : public PoolSQL 
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static HostPool *GetInstance()
    {
        return _instance;
    }
    
    static HostPool *CreateInstance(SqlDB *db)
    {
        if (NULL == _instance)
        {
            _instance = new HostPool(db); 
        }
        return _instance;
    }
    
    virtual ~HostPool() { _instance = NULL; }

    int64 Allocate(int64 *oid, const string &host_name, int64 cpu_info_id,string &error_str);
    Host *Get(int64 oid, bool lock);
    Host *Get(const string &host_name, bool lock);
    int32 Update(Host *host);
    int32 Drop(Host *host);
    int32 Count();
    int32 Dump(ostringstream& oss, const string& where) {return -1;}

    int32 Search(vector<int64> &oids, const string &where)
    {
        return PoolSQL::Search(oids, _table_host_in_use, where);
    }

    int32 Match(vector<int64> &oids, const string &where);
    int32 MatchSDevice(vector<int64> &oids, const vector<VmDeviceConfig> &devices);
    int32 SortHost(vector<int64> &oids, const string &orderby);    
    int32 List(vector<int64> &oids,
               int32 duration);

    int32 GetHostFree(int64 oid, HostFree &host_free);
    int32 GetHostRunningVms(Host *host);

    const string GetHostNameById(int64 oid);
    const string GetIscsinameById(int64 oid);
    int64 GetHostIdByName(const string &host_name);
    const string GetHostIpById(int64 oid);
    const string GetMediaAddressById(int64 oid);
    int32 GetClusterNetPlane(vector<string> &net_plane);
    int32 CheckNetPlaneExt(const string &strNetPlane);
    void CountSriovFromVmNicConfig(const vector<VmNicConfig>& _nics, vector<NicNetplaneInfo> &nic_netplane);
    bool IsNetplaneMapWithLoopbackPort(const string &netplane);
    bool IsNetplaneMapWithAllPort(const string &netplane);

    void  Print();
    void  PrintPort();
    void  PrintTrunk();
    void  PrintAlarm();
    void  PrintNetplaneMap();
    string GetHostSshkeyById(int64 oid);    
    void GetAllHostId(vector<int64> &oids);
    bool IsNetplaneUsedByVm(const int64 &hid,const string &nic_name);    
    static void BootStrap(SqlDB *db);
    
    int32 GetReportClusterInfo(int64 &value,const string &columnname,const string &tablename);
    int32 GetReportClusterInfo(int32 &value,const string &columnname,const string &tablename);
    bool CheckPositionIsRepeat(BoardPosition board_position);
    BoardPosition GetHostPosition(const int64 &hid);
    bool IsHostOffline(int64 oid);
    const string GetHostLocationById(int64 oid);

/*******************************************************************************
* 2. protected section
*******************************************************************************/
//protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    map<string, int64> _host_names;     /* �����������ݿ�oid�Ķ�Ӧ��ϵ */

    //����map:_host_names���ź���
    Mutex _mutex;

    int32 InitCallback(void *nil, SqlColumn * columns);
    int32 CountCallback(void *nil, SqlColumn * columns);    
    int32 SelectFreeCallback(void *nil, SqlColumn *columns);
    int32 FreeSriovCallback(void *nil, SqlColumn * columns);    
    int32 FreePortCallback(void *nil, SqlColumn *columns);
    int32 ClusterNetPlaneCallback(void *nil, SqlColumn *columns);
    int32 GetAllPowerOffHostCallback(void *nil, SqlColumn *columns);

    PoolObjectSQL * Create()
    {
        return new Host();
    }
    
    HostPool(SqlDB * db);
    static HostPool *_instance;
    static const char *_table_host_in_use;
    static const char *_table_host_in_use_with_cpuinfo;
    static const char *_host_in_use_stat;
    
    DISALLOW_COPY_AND_ASSIGN(HostPool);
};

} /* end namespace zte_tecs */

#endif // HM_HOST_POOL_H

