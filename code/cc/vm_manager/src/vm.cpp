//#include "vm_pub.h"
#include "vm.h"
#include "db_config.h"
#include "lifecycle_manager.h"
#include "vm_manager.h"
#include "vstorage_manager.h"
#include "host_pool.h"

namespace zte_tecs
{
//表名
const char* VirtualMachine::table = "vm_pool";

/*
注意:虚拟机表中增加新字段，需要修改以下位置:
1.VirtualMachine类中增加新字段成员
2.在枚举中增加新字段名称:enum ColNames
3.在col_names中增加新字段名称
4.在db_bootstrap中增加新字段名称
5.VirtualMachine::Insert中增加新字段
6.VirtualMachine::Update中增加新字段
7.VirtualMachine::SelectCallback中增加新字段
8.VirtualMachine类的默认构造函数
9.VirtualMachine类的序列化操作接口
*/

//查询的列
const char* VirtualMachine::col_names = "oid,vid,hid,hid_next,project_id,config,"
        "vcpu,tcu,cpu_percent,memory,"
        "cpu_rate,memory_rate,net_tx,net_rx,"
        "state,lcm_state,last_moni_at,"
        "last_op,last_op_at,last_op_result,last_op_timeout,last_op_stamp,dnat_out_ip,dnat_out_port,vnc_port,last_op_progress,last_op_detail,"
        "config_version,run_version,serial_port,enable_serial,expected_state,requirement,appoint_host,region_id,log_state";


void VirtualMachine::Init()
{
    //_uid = INVALID_OID;
    _hid = INVALID_OID;
    _hid_next = INVALID_OID;
    _project_id = INVALID_OID;
    _vcpu = 0;
    _tcu = 0;
    _memory = 0;
    //_localdisk = 0;
    _cpu_rate = 0;
    _memory_rate = 0;
    _net_tx = 0;
    _net_rx = 0;
    _state = VM_INIT;
    _expected_state = VM_RUNNING;
    _lcm_state = LCM_NO_STATE;
    _log_state = VM_INIT;
    _lastop = UNKNOWN_OP;
    _lastopresult = ERROR;
    _lastopstamp = "stamp";
    _lastoptimeout = 0;
    _dnat_out_ip = "0.0.0.0";
    _dnat_out_port = 0;
    _vnc_port = 0;
    _serial_port = 0;
    _enable_serial = false;   
    _last_op_progress = 0;
    _run_version = -1;

    _ignore_moni_times = 0;
    _is_image_prepared = false;
    _is_disk_prepared = false;
    
}

int VirtualMachine::Select(SqlDB *db)
{
    ostringstream oss;
    int rc;
    int64 boid;

    //设置select操作使用的callback函数
    SetCallback(static_cast<Callbackable::Callback>(&VirtualMachine::SelectCallback));
    //根据预先已经设置好的_oid字段进行select
    oss << "SELECT " << col_names << " FROM " << table << " WHERE oid = " << _oid;
    //开始select之前需要备份好oid
    boid = _oid;
    //清空oid
    _oid  = INVALID_OID;
    //执行查询
    rc = db->Execute(oss, this);
    UnsetCallback();
    //如果查询成功，即数据库中确定存在该oid的记录，此时_oid应该等于刚才备份的值
    if ( ( 0 != rc ) || (_oid != boid))
    {
        return -1;
    }
    return 0;
}

int VirtualMachine::Insert(SqlDB *db, string& error_str)
{

  int rc;

    rc = InsertReplace(db, false);

    if ( rc != 0 )
    {
        error_str = "Error inserting vm in DB.";
        return rc;
    }

    return 0;

}

int VirtualMachine::Update(SqlDB *db)
{

    int rc;

    rc = InsertReplace(db, true);

    if ( rc != 0 )
    {
        return rc;
    }

    return 0;

}

int VirtualMachine::InsertReplace(SqlDB *db, bool replace)
{
    ostringstream   oss;

    SqlCommand sql(db, table);
    sql.Add("oid", _oid);
    sql.Add("vid", _vid);
//    sql.Add("name", _name);

    if(_hid != INVALID_HID)
        sql.Add("hid", _hid);
    else
        sql.Add("hid");

    if(_hid_next != INVALID_HID)
        sql.Add("hid_next", _hid_next);
    else
        sql.Add("hid_next");

    //sql.Add("uid",  _uid);
    sql.Add("project_id", _project_id);
    sql.Add("config", _config);
    sql.Add("vcpu", (int)_vcpu);
    sql.Add("tcu",  (int)_tcu);
    sql.Add("cpu_percent",(int) _cpu_percent);
    sql.Add("memory",  _memory);
  //  sql.Add("localdisk",  _localdisk);
  //  sql.Add("requirement", _requirement);
    sql.Add("cpu_rate",  (int)_cpu_rate);
    sql.Add("memory_rate",  (int)_memory_rate);
    sql.Add("net_tx", (int)_net_tx);
    sql.Add("net_rx", (int)_net_rx);
    sql.Add("state",  _state);
    sql.Add("lcm_state", _lcm_state);
    sql.Add("log_state", _log_state);
    sql.AddSqlFunc("last_moni_at","now()");
    sql.Add("last_op",  _lastop);
    sql.Add("last_op_at", _lastopat);
    sql.Add("last_op_result", _lastopresult);
    sql.Add("last_op_timeout", (int)_lastoptimeout);
    sql.Add("last_op_stamp", _lastopstamp);
    sql.Add("dnat_out_ip", _dnat_out_ip);
    sql.Add("dnat_out_port", _dnat_out_port);
    sql.Add("vnc_port", _vnc_port);
    sql.Add("last_op_progress", _last_op_progress);
    sql.Add("last_op_detail", _last_op_detail);
    sql.Add("config_version", _config_version);
    sql.Add("serial_port", _serial_port);
    sql.Add("enable_serial", _enable_serial);
    sql.Add("expected_state", _expected_state);
   
    if(_run_version != -1)
        sql.Add("run_version", _run_version);

    sql.Add("requirement", _requirement);
    sql.Add("appoint_host", _appoint_host);

    sql.Add("region_id", _region_id);

    if (replace)
    {
        oss << sql.UpdateSql()<<" WHERE oid = " << _oid;;
    }
    else
    {
        oss << sql.InsertSql();
    }

    if(Transaction::On())
    {
        return Transaction::Append(oss.str());
    }
    else
    {
        return db->Execute(oss);
    }

    return 0;
}

int VirtualMachine::Drop(SqlDB *db)
{
    int rc = -1;
    ostringstream   oss;
    oss << "DELETE FROM " << table << " WHERE vid = " << _vid;

    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }

    if ( 0 == rc )
    {
        set_valid(false);
    }
    return rc;
}

int VirtualMachine::SelectCallback(void *nil, SqlColumn *columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
       int             tmp_int = 0;

    columns->GetValue(OID , _oid);
    columns->GetValue(VID , _vid);
    columns->GetValue(HID , _hid);
    columns->GetValue(HID_NEXT , _hid_next);
    //columns->GetValue(UID , _uid);
    columns->GetValue(PROJECT_ID , _project_id);
    //columns->GetValue(NAME , _name);

    columns->GetValue(CONFIG , _config);
    columns->GetValue(VCPU , tmp_int);
    _vcpu = (unsigned int)tmp_int;

    columns->GetValue(TCU , tmp_int);
    _tcu= (unsigned int)tmp_int;

    columns->GetValue(CPU_PERCENT , tmp_int);
    _cpu_percent= (unsigned int)tmp_int;

    columns->GetValue(MEMORY , _memory);
    //columns->GetValue(LOCALDISK , _localdisk);
    //columns->GetValue(REQUIREMENT , _requirement);

    columns->GetValue(CPU_RATE , tmp_int);
    _cpu_rate = (unsigned int)tmp_int;

    columns->GetValue(MEMORY_RATE , tmp_int);
    _memory_rate= (unsigned int)tmp_int;

    columns->GetValue(NET_TX , tmp_int);
    _net_tx= (unsigned int)tmp_int;

    columns->GetValue(NET_RX , tmp_int);
    _net_rx= (unsigned int)tmp_int;

    columns->GetValue(STATE , _state);
    columns->GetValue(EXPECTED_STATE, _expected_state);
    columns->GetValue(LCM_STATE , _lcm_state);
    columns->GetValue(LOG_STATE , _log_state);
    columns->GetValue(LAST_MONI_AT , _lastmoniat);

    columns->GetValue(LAST_OP , tmp_int);
    _lastop = (VmOperation)(tmp_int);

    columns->GetValue(LAST_OP_AT , _lastopat);
    columns->GetValue(LAST_OP_PROCESS , _last_op_progress);
    columns->GetValue(LAST_OP_DETAIL, _last_op_detail);

    columns->GetValue(LAST_OP_RESULT , _lastopresult);
    columns->GetValue(LAST_OP_TIMEOUT , tmp_int);
    _lastoptimeout= (unsigned int)tmp_int;

    columns->GetValue(LAST_OP_STAMP , _lastopstamp);
    columns->GetValue(DNAT_OUT_IP , _dnat_out_ip);
    columns->GetValue(DNAT_OUT_PORT , tmp_int);
    _dnat_out_port= (unsigned int)tmp_int;

    columns->GetValue(VNC_PORT , tmp_int);
    _vnc_port = (unsigned int)tmp_int;

    columns->GetValue(SERIAL_PORT , tmp_int);
    _serial_port = (unsigned int)tmp_int;

    columns->GetValue(ENABLE_SERIAL , tmp_int);
   _enable_serial = (unsigned int)tmp_int;
   
    columns->GetValue(RUN_VERSION , _run_version);

    columns->GetValue(REQUIREMENT , _requirement);
    columns->GetValue(APPOINT_HOST , _appoint_host);

    columns->GetValue(REGION_LEVEL , _region_id);

    return 0;
}

int VirtualMachine::Dump(ostringstream& oss, SqlColumn *columns)
{
    //根据需要组织查询结果
    //资源池dump回调函数中调用
    return 0;
}

/*****************************************************************************/
/************************以下函数操作跟数据库无关*****************************/
/*****************************************************************************/

int VirtualMachine::SetValue(const string &key, int64 value)
{
    if ( key =="hid" )
        _hid = value;
    else if ( key =="hid_next" )
        _hid_next = value;
    else if ( key =="last_op" )
        _lastop = value;
    else if ( key =="state" )
        _state = value;
    else if ( key =="expected_state" )
        _expected_state = value;
    else if ( key =="log_state" )
        _log_state = value;
    else if ( key =="vcpu" )
        _vcpu = value;
    else if ( key =="tcu" )
        _tcu = value;
    else if ( key =="memory" )
        _memory = value;
    else
        SkyAssert(false);

    return 0;
}

}

