#include "vm_pool.h"
#include "host_pool.h"

namespace zte_tecs
{
VirtualMachinePool *VirtualMachinePool::instance = NULL;

VirtualMachinePool::VirtualMachinePool(SqlDB *db):PoolSQL(db,VirtualMachine::table)
{

}

VirtualMachinePool::~VirtualMachinePool()
{
}

STATUS VirtualMachinePool::Init()
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::InitCallback));
    sql << "SELECT oid,vid FROM " << VirtualMachine::table;
    _db->Execute(sql, this);
    UnsetCallback();
    return SUCCESS;
}

int VirtualMachinePool::InitCallback(void *nil, SqlColumn *columns)
{
    //将数据库中所有的虚拟机oid->vid读取出来
    if ( NULL == columns || 2 != columns->get_column_num())
    {
        return -1;
    }
    int64 oid = columns->GetValueInt64(0); 
    int64 vid = columns->GetValueInt64(1); 
    _known_vms.insert(make_pair(vid,oid));
    return 0;
}

int VirtualMachinePool::SearchVmsCallback(void *nil, SqlColumn *columns)
{
    vector<int64>* vids;
    if(!nil)
    {
        return 0;
    }
    
    vids = static_cast<vector<int64> *>(nil);
    if ( NULL == columns || 1 != columns->get_column_num())
    {
        return -1;
    }
    int64 vid = columns->GetValueInt64(0);
    vids->push_back(vid);
    return 0;
}

int VirtualMachinePool::GetAllVms(vector<int64>& vids,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),(void *)&vids);
    sql << "SELECT vid FROM " << VirtualMachine::table 
        << " ORDER BY oid ASC LIMIT " << limit;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int VirtualMachinePool::GetVmsNotOnHost(vector<int64>& vids,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),(void *)&vids);
    sql << "SELECT vid FROM " << VirtualMachine::table 
        << " WHERE hid = " << INVALID_HID
        << " ORDER BY oid ASC LIMIT " << limit;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int VirtualMachinePool::GetVmsByHost(vector<int64>& vids,int64 hid,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),(void *)&vids);
    sql << "SELECT vid FROM " << VirtualMachine::table 
        << " WHERE hid = " << hid 
        << " ORDER BY oid ASC LIMIT " << limit;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int VirtualMachinePool::GetVmsByHostAndNext(vector<int64>& vids,int64 hid,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),(void *)&vids);
    sql << "SELECT vid FROM " << VirtualMachine::table 
        << " WHERE hid = " << hid 
        << " or hid_next = " << hid 
        << " ORDER BY oid ASC LIMIT " << limit;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int VirtualMachinePool::GetVmsByLcmState(vector<int64>& vids,LcmState state,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),(void *)&vids);
    sql << "SELECT vid FROM " << VirtualMachine::table 
        << " WHERE lcm_state = " << state 
        << " ORDER BY oid ASC LIMIT " << limit;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int VirtualMachinePool::GetVmsByLastOpResult(vector<int64>& vids,STATUS lastopresult,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),(void *)&vids);
    sql << "SELECT vid FROM " << VirtualMachine::table 
        << " WHERE last_op_result = " << lastopresult 
        << " ORDER BY oid ASC LIMIT " << limit;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int64 VirtualMachinePool::GetHid(int64 vid)
{
    int64 hid = INVALID_HID;
    VirtualMachine *pvm = Get(vid,true);
    if(pvm)
    {
        hid = pvm->_hid;
        Put(pvm,true);
    }
    return hid;
}

int VirtualMachinePool::GetLcmState(int64 vid,LcmState& state)
{
    VirtualMachine *pvm = Get(vid,false);
    if(!pvm)
    {
        state = LCM_NO_STATE;
        return -1;
    }
    
    state = (LcmState)pvm->_lcm_state;
    //Put(pvm,true);
    return 0;
}

int VirtualMachinePool::GetHAPolicy(int64 vid,VmHAPolicy& policy)
{
#if 0
    VirtualMachine *pvm = Get(vid,true);
    if(!pvm)
    {
        return -1;
    }
    
    policy = pvm->_ha_policy;
    Put(pvm,true);
#endif
    return 0;
}

int VirtualMachinePool::GetRunningInfo(int64 vid,VmRunningInfo& info)
{
    VirtualMachine *pvm = Get(vid,false);
    if(!pvm)
    {
        return ERR_OBJECT_GET_FAILED;
    }
    
    if(pvm->_hid == INVALID_HID)
    {
        //info._state = VM_UNKNOWN;
        info._hc_name = "";
        info._vnc_out_ip = "";
    }
    else
    {    
        HostPool *host_pool = HostPool::GetInstance();
        info._hc_name = host_pool->GetHostNameById(pvm->_hid);
        info._vnc_out_ip = host_pool->GetHostIpById(pvm->_hid);
    }
    
    info._state = VM_UNKNOWN;
    ostringstream where;
    where << "vid = " << vid;
    SelectColumn("web_view_vmstate", "state", where.str(), info._state); //状态字段要保持一致，所以，统一从view取状态

    info._vnc_port = pvm->_vnc_port;
    info._serial_port = pvm->_serial_port;

    //info._vnc_out_ip = pvm->_dnat_out_ip;
    //info._vnc_port = pvm->_dnat_out_port;
    info._cpu_rate = pvm->_cpu_rate;
    info._memory_rate = pvm->_memory_rate;
    info._net_tx = pvm->_net_tx;
    info._net_rx = pvm->_net_rx;

    return 0;
}

int VirtualMachinePool::GetNicConfig(int64 vid,vector<VmNicConfig>& nics)
{
    VirtualMachine *pvm = Get(vid,true);
    if(!pvm)
    {
        return -1;
    }
    
    nics = pvm->_config._nics;
    Put(pvm,true);
    return 0;
}

int VirtualMachinePool::IsVmExist(int64 vid)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::SearchVmsCallback),NULL);
    sql << "SELECT vid FROM " << VirtualMachine::table << " WHERE vid = " << vid;
    int ret = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int64 VirtualMachinePool::Allocate(int64 *oid,int64 vid)
{
    string error_str;
    VirtualMachine *pvm = new VirtualMachine(vid);
    // Insert the Object in the pool
    *oid = PoolSQL::Allocate(pvm, error_str);
    if (*oid != INVALID_OID)
    {
        // Add the user to the map of known_users
        _known_vms.insert(make_pair(vid,*oid));
    }
    return *oid;
}

int64 VirtualMachinePool::Allocate(int64 *oid,VirtualMachine& vm)
{
    string error_str;
    // Insert the Object in the pool
    *oid = PoolSQL::Allocate(vm, error_str);
    if (*oid != INVALID_OID)
    {
        // Add the user to the map of known_users
        _known_vms.insert(make_pair(vm._vid,*oid));
    }
    return *oid;
}

int VirtualMachinePool::DumpCallback(void *oss, SqlColumn *columns)
{
    ostringstream *temp_oss;
    temp_oss = static_cast<ostringstream *>(oss);
    return VirtualMachine::Dump(*temp_oss, columns);
}

int VirtualMachinePool::Dump(ostringstream& oss, const string& where)
{
    int rc;
    ostringstream cmd;

    oss << "<VM_POOL>";
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachinePool::DumpCallback),static_cast<void *>(&oss));

    cmd << "SELECT " << VirtualMachine::col_names << " FROM " << VirtualMachine::table;
    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);
    UnsetCallback();
    oss << "</VM_POOL>";
    return rc;
}

int32 VirtualMachinePool::GetVmsByCond(vector<int64> &vids, const string &cond)
{
    return SelectColumn("vm_pool","vid", cond,vids);    
}

int VirtualMachinePool::GetVmWorkFlowId(int64 vid, string &wf_id)
{
    ostringstream where;
    where << "vid = " << vid;

    return SelectColumn(VM_WF_VIEW, "workflow_id", where.str(), wf_id);
}

int VirtualMachinePool::GetVmWorkFlowId(int64 vid, const string &op, string &wf_id)
{
    ostringstream where;
    where << " name = '" << op << "' and vid = " << vid;

    return SelectColumn(VM_WF_VIEW, "workflow_id", where.str(), wf_id);
}

/******************************************************************************/
const char * VmDevicePool::_table_name = 
        "vm_device"; 
const char * VmDevicePool::_col_names = 
        "vid,"
        "type,"
        "count";

const char * VmDevicePool::_cancel_table_name = 
        "vm_device_cancel"; 
const char * VmDevicePool::_cancel_col_names = 
        "vid,"
        "hid,"
        "type,"
        "count";

VmDevicePool *VmDevicePool::_instance = NULL;




/******************************************************************************/
int VmDevicePool::InsertVmDevices(const vector<VmDeviceConfig> &devices, int64 id)
{
    int rc = 0;

    SqlCommand    sql(db, SqlCommand::WITH_TRANSACTION);

    sql.SetTable(VmDevicePool::_table_name);

    vector<VmDeviceConfig>::const_iterator  it;
    for (it  = devices.begin(); 
         it != devices.end();
         it++ )
    {
        sql.Clear();

        sql.Add("vid",      id);
        sql.Add("type",     it->_type);
        sql.Add("count",    it->_count);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    
    return rc;

}


/******************************************************************************/
int VmDevicePool::InsertVmDevicesCancel(const vector<VmDeviceConfig> &devices, int64 vid, int64 hid)
{
    int rc = 0;

    SqlCommand    sql(db, SqlCommand::WITH_TRANSACTION);

    sql.SetTable(VmDevicePool::_cancel_table_name);

    vector<VmDeviceConfig>::const_iterator  it;
    for (it  = devices.begin(); 
         it != devices.end();
         it++ )
    {
        sql.Clear();

        sql.Add("vid",      vid);
        sql.Add("hid",      hid);
        sql.Add("type",     it->_type);
        sql.Add("count",    it->_count);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    
    return rc;
}

/******************************************************************************/
const char * VmUsbPool::_table_name = 
        "vm_usb"; 
const char * VmUsbPool::_col_names = 
        "vid,"
        "vendor_id,"
        "product_id,"
        "name";

VmUsbPool *VmUsbPool::_instance = NULL;

/* vm_usb_pool 根据hid查询该hid下面的usb info */ 
int VmUsbPool::GetUsbInfoByVid(vector<int64> &vids, vector<VmQueryUsbInfo> &usb_list)
{
    ostringstream oss_sql;
    SetCallback(static_cast<Callbackable::Callback>(&VmUsbPool::SearchUsbInfoCallback),(void *)&usb_list);

    SqlCommand sql(db, _table_name);
    sql.Add("vid");
    sql.Add("vendor_id");
    sql.Add("product_id");
    sql.Add("name");

    oss_sql.str("");
    if (vids.size() != 0)
    {
        oss_sql << sql.SelectSql() << " WHERE vid = " << vids.at(0);
    }
    else
    {
        UnsetCallback();
        return -1;
    }
    
    uint32 i;
    for (i = 1; i < vids.size(); i++)
    {
        oss_sql << " or vid = " << vids.at(i);
    }
    int ret = db->Execute(oss_sql, this);
    UnsetCallback();
    return ret;
}


/******************************************************************************/
bool VmUsbPool::IsUsbExist(int64 vid, int vendor_id, int product_id)
{
    ostringstream where;
    
    where << "vid = " << vid << " and vendor_id = "\
              << vendor_id << " and product_id = " << product_id;

    int ret = SelectColumn(_table_name,"vid",where.str(),vid);
    // 返回NULL也表示没找到
    if(SQLDB_OK != ret)
    {
        return false;
    }
    return true;
}

int VmUsbPool::GetVidbyUsb(int64 &vid, int vendor_id, int product_id)
{
    ostringstream where;
    
    where << "vendor_id = " << vendor_id << " and product_id = " << product_id;

    return SelectColumn(_table_name,"vid",where.str(), vid);
}

/* 向vm_usb_pool写入USB信息 */ 
int VmUsbPool::InsertVmUsbInfo(int64 vid, const VmUsbInfo &info)
{
    SqlCommand sql(db, _table_name);
    sql.Add("vid", vid);
    sql.Add("vendor_id", info._vendor_id);
    sql.Add("product_id", info._product_id);
    sql.Add("name", info._name);

    return sql.Insert();
}

/* 向vm_usb_pool 删除USB信息 */ 
int VmUsbPool::DeleteVmUsbInfo(int64 vid, const VmUsbInfo &info)
{
    SqlCommand sql(db, _table_name);
    sql.Add("vid", vid);
    sql.Add("vendor_id", info._vendor_id);
    sql.Add("product_id", info._product_id);
    sql.Add("name", info._name);

    ostringstream oss_where;
    oss_where << "where vid = " << vid << " and vendor_id = " << info._vendor_id   \
                    << " and product_id = " << info._product_id << " and name = '" << info._name <<"'";
    return sql.Delete(oss_where.str());
}


int VmUsbPool::SearchUsbInfoCallback(void *nil, SqlColumn *columns)
{
    vector<VmQueryUsbInfo>* usb_list_p;
    VmQueryUsbInfo                usb_info;
    if(!nil)
    {
        return 0;
    }
    
    usb_list_p = static_cast<vector<VmQueryUsbInfo> *>(nil);
    if ( NULL == columns)
    {
        return -1;
    }
    usb_info.use_vid = columns->GetValueInt64(VmUsbPool::VID);
    usb_info.vendor_id = columns->GetValueInt(VmUsbPool::VENDOR_ID);
    usb_info.product_id  = columns->GetValueInt(VmUsbPool::PRODUCT_ID);
    usb_info.name = columns->GetValueText(VmUsbPool::NAME);
    usb_list_p->push_back(usb_info);
    return 0;
}


}

